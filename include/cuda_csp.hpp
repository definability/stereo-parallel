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
#ifndef __CUDA_ARCH__
#ifndef CUDA_CSP_HPP
#define CUDA_CSP_HPP

#include <constraint_graph.hpp>

namespace gpu
{

using sp::graph::constraint::ConstraintGraph;
using sp::types::BOOL;
using sp::types::FLOAT;
using sp::types::ULONG;

/**
 * \brief Problem representation to be placed into GPU memory.
 * Flattened version of sp::graph::constraint::ConstraintGraph.
 */
struct CUDAProblem
{
    float* min_penalties_edges;
    float* min_penalties_pixels;
    float* reparametrization;
    int* changed;
    int* nodes_availability;
    unsigned* left_image;
    unsigned* right_image;
};

/**
 * Initialize fields of the CUDAProblem
 * with values from the sp::graph::constraint::ConstraintGraph.
 */
void prepare_problem(struct ConstraintGraph* graph, struct CUDAProblem* problem);

/**
 * Free resources held by the CUDAProblem.
 */
void free_problem(struct ConstraintGraph* graph, struct CUDAProblem* problem);

/**
 * \brief Remove all nodes that have no any edges at least to one neighbor
 * until there is nothing to remove
 * (anything is removed or all nodes have neighbors),
 * and save the nodes availability data
 * (sp::graph::constraint::ConstraintGraph::nodes_availability)
 * into the input sp::graph::constraint::ConstraintGraph.
 */
void csp_solution_cuda(
    struct ConstraintGraph* graph,
    struct CUDAProblem* problem
);

}

#endif
#endif
