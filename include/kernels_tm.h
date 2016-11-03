#ifndef _KERNELS_TM_H_
#define _KERNELS_TM_H_

typedef enum
{
    //these must be arranged in alphabetical order by kernel function name!
    FIND_BEST_WORST_ALT_KERNEL_TM,
    FIND_BEST_WORST_ALT2_KERNEL_TM,
    //FIND_BEST_WORST_UNVEC_KERNEL_TM,
    //FIND_BEST_WORST_UNVEC_ALT_KERNEL_TM,
    FIND_BEST_WORST_VEC2_KERNEL_TM,
    //FIND_BEST_WORST_VEC2_ALT_KERNEL_TM,

    FIND_MIN_VEC_KERNEL_TM,
    
    //PARTICLE_INIT_UNVEC_KERNEL_TM,
    PARTICLE_INIT_VEC_KERNEL_TM,

    //SWAP_PARTICLES_ALT_KERNEL_TM,
    //SWAP_PARTICLES_UNVEC_KERNEL_TM,
    SWAP_PARTICLES_VEC_KERNEL_TM,

    //UPDATE_BEST_VALS_COMBINED_KERNEL_TM,
    //UPDATE_BEST_VALS_UNVEC_KERNEL_TM,
    UPDATE_BEST_VALS_VEC_KERNEL_TM,
    
    //UPDATE_FITNESS_SHARED_UNVEC_KERNEL_TM,
    UPDATE_FITNESS_SHARED_VEC_KERNEL_TM,
    
    //UPDATE_POS_VEL_ALT_KERNEL_TM,
    UPDATE_POS_VEL_HYBRID_VEC_KERNEL_TM,
    //UPDATE_POS_VEL_SHARED_VEC_KERNEL_TM,
    //UPDATE_POS_VEL_UNVEC_KERNEL_TM,
    UPDATE_POS_VEL_VEC_KERNEL_TM,
    
    NUM_KERNELS_TM
} kernel_names_tm;

#endif