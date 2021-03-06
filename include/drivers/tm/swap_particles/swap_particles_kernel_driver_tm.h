#ifndef _SWAP_PARTICLES_KERNEL_DRIVER_TM_H_
#define _SWAP_PARTICLES_KERNEL_DRIVER_TM_H_

#include "CL/cl.h"
#include "global_constants.h"
#include "config_tm.h"
#include "buffers_tm.h"
#include "devices.h"
#include "kernels_tm.h"

/**
 Note: Although TM has a separate driver for swap_particles, it still uses the common/ kernel. The separate driver
 is necessary because of the "combined" parameter that is needed for some of the functions below.
 **/
void set_swap_particles_vec_kernel_args_tm(
    config_tm *conf,
    cl_kernel *kernel,
    mpso_bufs_tm *bufs,
    cl_uint swarms_per_group
    );

/* void set_swap_particles_alt_kernel_args_tm( */
/*     config_tm *conf, */
/*     cl_kernel *kernel, */
/*     mpso_bufs_tm *bufs */
/*     ); */

/* void set_swap_particles_unvec_kernel_args_tm( */
/*     config_tm *conf, */
/*     cl_kernel *kernel, */
/*     mpso_bufs_tm *bufs */
/*     ); */

void launch_swap_particles_kernel_tm(
    config_tm *conf,
    cl_kernel *kernel,
    mpso_bufs_tm *bufs,
    cl_uint iter_num,
    device *dev,
    size_t *global_work_size,
    size_t *local_work_size,
    char *kernel_label,
    cl_uint combined
    );

void launch_swap_particles_vec_kernel_tm(
    config_tm *conf,
    cl_kernel *kernels,
    mpso_bufs_tm *bufs,
    cl_uint iter_num,
    device *dev,
    cl_uint combined
    );

/* void launch_swap_particles_alt_kernel_tm( */
/*     config_tm *conf, */
/*     cl_kernel *kernels, */
/*     mpso_bufs_tm *bufs, */
/*     mpso_events_tm *events, */
/*     cl_uint iter_num, */
/*     device *dev, */
/*     cl_uint combined */
/*     ); */

/* void launch_swap_particles_unvec_kernel_tm( */
/*     config_tm *conf, */
/*     cl_kernel *kernels, */
/*     mpso_bufs_tm *bufs, */
/*     mpso_events_tm *events, */
/*     cl_uint iter_num, */
/*     device *dev, */
/*     cl_uint combined */
/*     ); */

#endif
