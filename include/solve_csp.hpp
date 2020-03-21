#ifndef SOLVE_CSP_HPP
#define SOLVE_CSP_HPP

#include <cuda.h>
#include <cuda_runtime.h>

__global__ void choose_best_node_gpu(
    int* nodes_availability,
    unsigned* left_image,
    unsigned* right_image,
    float* min_penalties_pixels,
    float* min_penalties_edges,
    float* reparametrization,
    unsigned height,
    unsigned width,
    unsigned max_value,
    unsigned disparity_levels,
    float threshold,
    float cleanness,
    float smoothness,
    unsigned pixel_x,
    unsigned pixel_y
);

__global__ void csp_iteration_cuda(
    int* nodes_availability,
    int* changed,
    unsigned* left_image,
    unsigned* right_image,
    float* min_penalties_pixels,
    float* min_penalties_edges,
    float* reparametrization,
    unsigned height,
    unsigned width,
    unsigned max_value,
    unsigned disparity_levels,
    float threshold,
    float cleanness,
    float smoothness
);

#endif
