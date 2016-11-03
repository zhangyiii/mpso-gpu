# mpso-gpu
A parallel, GPU-accelerated hybrid metaheuristic algorithm that combines Multi-Swarm Particle Swarm Optimization with components of a Genetic Algorithm. The purpose of this hybridization was to (attempt to) improve the algorithm's ability to escape from local optima encountered in the solution space. This code was written for my Masters thesis - and as such, documentation and cleanup are still a work in progress :) The code is written in OpenCL and is designed to leverage the zero-copy features of the AMD APU architecture, which had just come out at the time.
