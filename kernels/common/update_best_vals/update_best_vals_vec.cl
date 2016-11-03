/*
 Global size: s * p
 Local size: p
 Local memory per thread: 1
 Mapping: One thread per particle
*/

__kernel void update_best_vals_vec(
    __global float *fitness, //s * p
    __global float *position, //s * p * d
    __global float *pbest_fitness,   //s * p
    __global float *pbest_position,  //s * p * d
    __global float *sbest_fitness,   //s
    __global float *sbest_position,  //s * d
    __local float *fitness_scratch, //p
    uint num_swarms,
    uint num_sparticles,
    uint num_dims
    )
{
    uint swarms_per_group = get_local_size(0) / num_sparticles;
    uint group_id = get_group_id(0) * swarms_per_group + get_local_id(0) / num_sparticles; //each id corresponds to one swarm (there are num_swarms workgroups in total)
    uint local_id = get_local_id(0) % num_sparticles; //each id corresponds to an offset within a swarm (each workgroup contains num_sparticles threads)

    uint position_offset = group_id * num_sparticles * num_dims + local_id * num_dims;

    float4 update_occurred = (float4) (-1.0f); //< 0 means no update occurred
    float2 partial = (float2) (0.0f);

    //a subset of threads update the pbest fitnesses
    if (local_id < num_sparticles / 4)// && group_id < num_swarms)
    {
        float4 fitness_chunk = vload4(0, fitness + group_id * num_sparticles + local_id * 4);
        float4 pbest_chunk = vload4(0, pbest_fitness + group_id * num_sparticles + local_id * 4);
        int4 cmp = (fitness_chunk < pbest_chunk) || (pbest_chunk == (float4) (FLT_MAX)); //on the first iteration, pbest_chunk will be equal to FLT_MAX. In the event the fitness_chunk also happens to be equal to FLT_MAX (which is possible, though extremely unlikely), we want to perform the update even though fitness_chunk == pbest_chunk.
        float4 new_fitnesses = select(pbest_chunk, fitness_chunk, cmp);
        //printf("%u, %u: %f, %f, %f, %f\n", group_id, local_id, fitness_chunk.x, fitness_chunk.y, fitness_chunk.z, fitness_chunk.w);

        if (any(cmp))
        {
            //store the updated values in pbest_fitnesses
            vstore4( new_fitnesses, 0, pbest_fitness + group_id * num_sparticles + local_id * 4 );
        }
        /* else */
        /* { */
        /*     printf("%u: %u, %u\n", group_id, any(fitness_chunk >= pbest_chunk), any(pbest_chunk != (float4) FLT_MAX)); */
        /* } */

        //update fitness_scratch - store a -1 if no update occurred, and the new fitness value if an update did occur
        update_occurred = select(update_occurred, new_fitnesses, cmp);
        vstore4( update_occurred, 0, fitness_scratch + (group_id % swarms_per_group) * num_sparticles + local_id * 4);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    float per_thread_fitness = fitness_scratch[get_local_id(0)];
    float4 pos_vec;
    uint i;
    if (local_id < num_sparticles * swarms_per_group)// && group_id < num_swarms)
    {
        //all threads update the pbest positions
        if (per_thread_fitness >= 0.0f)
        {
            for (i = 0; i < num_dims / 4; i++)
            {
                pos_vec = vload4(0, position + position_offset + i * 4);
                vstore4(pos_vec, 0, pbest_position + position_offset + i * 4);
            }
        }
    }

    //perform parallel reduction to find swarm best fitness (use float2s so we can support a swarm size of 4 (using float4s means we need at least 8 particles/swarm for the reduction to work)
    float2 left;
    float2 right;
    float2 updated_vec;
    float2 zero = (float2) (0.0f, 0.0f);
    float2 max_vec = (float2) (FLT_MAX, FLT_MAX);
    uint group_swarm_index = group_id % swarms_per_group;
    uint swarm_base = group_swarm_index * num_sparticles;

    for (i = num_sparticles / 4; i > 0; i /= 2) //start at num_sparticles / 4 because we divide by 2, and then each thread handles 2 elements at a time => 2 * 2 = 4 = 2^2
    {
        if (local_id < i)// && group_id < num_swarms)
        {
            left = vload2(0, fitness_scratch + swarm_base + local_id * 2);
            right = vload2(0, fitness_scratch + swarm_base + (local_id + i) * 2);
            left = select(left, max_vec, left < zero);
            right = select(right, max_vec, right < zero);
            updated_vec = fmin(left, right);
            vstore2(
                updated_vec,
                0,
                fitness_scratch + swarm_base + local_id * 2
                );
        }
        barrier(CLK_LOCAL_MEM_FENCE);

        if (!local_id && i > 1 && i % 2)// && group_id < num_swarms)
        {
            right = vload2(0, fitness_scratch + swarm_base + (i - 1) * 2);
            right = select(left, max_vec, left < zero);
            updated_vec = fmin(updated_vec, right);
            vstore2(
                updated_vec,
                0,
                fitness_scratch + swarm_base //+ local_id * 2 //this is redundant, since local_id == 0 here
                );
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    //note: num_sparticles is always divisible by 4, so there is no need to catch extra after the loop

    int update_needed;
    if (!local_id)// && group_id < num_swarms)
    {
        //In the event that no updates were performed, all components of updated_vec will be FLT_MAX - which is guarenteed to be less than or equal to the old sbest fitness (and therefore we don't need to update it)
        update_needed = any(updated_vec != max_vec);
        float final_val = min(updated_vec.x, updated_vec.y);
        
        if ( update_needed && (update_needed = (final_val < sbest_fitness[group_id])) )
        {
            fitness_scratch[swarm_base] = final_val; //min value is now in fitness_scratch[0] for each workgroup
                    
            sbest_fitness[group_id] = final_val;
        }
        //printf("%u: %f\n", group_id, update_needed ? final_val : -1.0f);
        fitness_scratch[swarm_base + 1] = update_needed; //each workgroup's fitness_scratch[1] now contains a boolean indicating whether or not sbest was updated
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //Multiple threads may have fitnesses equal to the minimum fitness. Use a race condition to select one such thread.
    //This thread will write it's corresponding particle's position to sbest_positions.
    update_needed = (int) fitness_scratch[swarm_base + 1];
    if (local_id < num_sparticles * swarms_per_group && update_needed && per_thread_fitness == fitness_scratch[swarm_base])// && group_id < num_swarms)
    {
        fitness_scratch[swarm_base + 2] = (float) local_id; //for each workgroup, fitness_scratch[2] now contains the index of the thread with the updated position
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    //fitness_scratch[2] now holds the index of the particle whose fitness is best
    if (update_needed && local_id < num_dims / 4)// && group_id < num_swarms)
    {
        uint available_threads = min(num_dims / 4, get_local_size(0) / swarms_per_group); //in case num_sparticles < num_dims / 4
        uint final_index = (uint) fitness_scratch[swarm_base + 2];
        for (i = 0; i < (num_dims / 4) / available_threads; i++)
        {
            pos_vec = vload4(0, position + group_id * num_sparticles * num_dims + final_index * num_dims + (local_id + i) * 4);
            vstore4(pos_vec, 0, sbest_position + group_id * num_dims + (local_id + i) * 4);
        }
        
        //clean up any leftover vectors if (available threads) doesn't divide (number of values we need to copy)  evenly
        uint num_leftover = (num_dims / 4) % available_threads;
        if ( local_id < num_leftover )
        {
            pos_vec = vload4(0, position + group_id * num_sparticles * num_dims + final_index * num_dims + ((num_dims / 4 - num_leftover) + local_id) * 4);
            vstore4(pos_vec, 0, sbest_position + group_id * num_dims + ((num_dims / 4 - num_leftover) + local_id) * 4);
        }
    }
}