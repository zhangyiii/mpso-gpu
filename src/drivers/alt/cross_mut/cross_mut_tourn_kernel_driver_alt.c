#include "drivers/alt/cross_mut/cross_mut_tourn_kernel_driver_alt.h"

void set_cross_mut_tourn_kernel_args_alt(
    config_alt *conf,
    cl_kernel *kernel,
    mpso_bufs_alt *bufs,
    cl_uint threads_per_particle,
    cl_uint local_mem_per_group,
    cl_uint iter_index
    )
{
    cl_uint arg_index = 0;
    cl_int error;
    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_mem),
        &(bufs->positions_buf)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_mem),
        &(bufs->fitnesses_buf)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;
    
    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_mem),
        &(bufs->swarm_types_buf)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;
    
    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(conf->num_swarms)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(conf->num_sparticles)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(conf->num_dims)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_float),
        &(conf->cross_ratio)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_float),
        &(conf->mut_prob)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(conf->tourn_size)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(threads_per_particle)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(iter_index)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        sizeof(cl_uint),
        &(conf->seed)
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
    arg_index++;

    error = clSetKernelArg(
        *kernel,
        arg_index,
        local_mem_per_group * sizeof(cl_float),
        NULL
        );
    check_error(error, "Error setting cross_mut kernel arg %d", arg_index);
}

void launch_cross_mut_tourn_kernel_alt(
    config_alt *conf,
    cl_kernel *kernel_buf,
    mpso_bufs_alt *bufs,
    cl_uint iter_index,
    device *dev
    )
{
    static size_t local_work_size;
    static size_t global_work_size;
    //static size_t particles_per_group;
    static size_t threads_per_particle;
    static size_t local_mem_per_particle;

    if (iter_index == 0)
    {
        //figure out launch dimensions
        cl_uint dim_threads = conf->num_dims / 4;
        cl_uint tourn_threads = conf->tourn_size / 4;
        threads_per_particle = dim_threads > tourn_threads ? dim_threads : tourn_threads;

        /* cl_uint workgroup_size = conf->num_swarms * conf->num_sparticles * threads_per_particle; */

        /* if (workgroup_size > dev->max_workgroup_size) */
        /* { */
        /*     workgroup_size = dev->max_workgroup_size; */
        /* } */
        /* local_work_size = workgroup_size - (workgroup_size % threads_per_particle); */
        
        /* particles_per_group = local_work_size / threads_per_particle; */

        /* size_t num_groups = (conf->num_swarms * conf->num_sparticles) / particles_per_group + ((conf->num_swarms * conf->num_sparticles) % particles_per_group ? 1 : 0); */
        /* global_work_size = num_groups * local_work_size; */

        //figure out local memory requirements
        local_mem_per_particle = conf->tourn_size / 2;

        global_work_size = conf->num_swarms * conf->num_sparticles * threads_per_particle;
        local_work_size = calc_local_size(
            global_work_size,
            threads_per_particle,
            local_mem_per_particle * sizeof(cl_float),
            dev
            );
    }
    
    #if LAUNCH_WARNINGS
    char *name =  get_kernel_name(&(kernel_buf[CROSS_MUT_VEC_KERNEL_ALT]));
    printf("Launching %s kernel.\n", name);
    free(name);
    printf("global_work_size: %u\n", global_work_size);
    printf("local_work_size: %u\n", local_work_size);
    printf("threads_per_particle: %u\n", threads_per_particle);
    //printf("particles_per_group: %u\n", particles_per_group);
    #endif

    set_cross_mut_tourn_kernel_args_alt(
        conf,
        &(kernel_buf[CROSS_MUT_VEC_KERNEL_ALT]),
        bufs,
        threads_per_particle,
        local_mem_per_particle * local_work_size,
        iter_index
        );

    cl_int error = clEnqueueNDRangeKernel(
        dev->cmd_q,
        kernel_buf[CROSS_MUT_VEC_KERNEL_ALT],
        1,
        NULL,
        &global_work_size,
        NULL,
        0,
        NULL,
        NULL
        );
    check_error(error, "Error launching cross_mut kernel.");

    #if LAUNCH_WARNINGS
    printf("Done.\n");
    #endif
}
