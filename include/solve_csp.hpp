/*
 * MIT License
 *
 * Copyright (c) 2018-2020 char-lie
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
