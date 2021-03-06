#ifndef _KERNELS_ALT_H_
#define _KERNELS_ALT_H_

typedef enum
{
    //these must be arranged in alphabetical order by kernel function name!
    CROSS_MUT_VEC_KERNEL_ALT,
    
    F1_KERNEL_ALT,
    F10_KERNEL_ALT,
    F11_KERNEL_ALT,
    F12_KERNEL_ALT,
    F13_KERNEL_ALT,
    F14_KERNEL_ALT,
    F15_KERNEL_ALT,
    F16_KERNEL_ALT,
    F17_KERNEL_ALT,
    F18_KERNEL_ALT,
    F19_KERNEL_ALT,
    F2_KERNEL_ALT,
    F20_KERNEL_ALT,
    F21_KERNEL_ALT,
    F22_KERNEL_ALT,
    F23_KERNEL_ALT,
    F24_KERNEL_ALT,
    F3_KERNEL_ALT,
    F4_KERNEL_ALT,
    F5_KERNEL_ALT,
    F6_KERNEL_ALT,
    F7_KERNEL_ALT,
    F8_KERNEL_ALT,
    F9_KERNEL_ALT,
    
    FIND_BEST_WORST_ALT_KERNEL_ALT,
    FIND_BEST_WORST_ALT2_KERNEL_ALT,
    FIND_BEST_WORST_VEC2_KERNEL_ALT,

    FIND_MIN_VEC_KERNEL_ALT,

    INIT_ROT_MATRIX_VEC_KERNEL_ALT,

    PARTICLE_INIT_VEC_KERNEL_ALT,
    PERMUTE_VEC_KERNEL_ALT,

    SWAP_PARTICLES_VEC_KERNEL_ALT,

    UPDATE_BEST_VALS_VEC_KERNEL_ALT,
    
    UPDATE_POS_VEL_VEC_KERNEL_ALT,

    UPDATE_SAMPLES_VEC_KERNEL_ALT,
    
    NUM_KERNELS_ALT
} kernel_names_alt;

#endif
