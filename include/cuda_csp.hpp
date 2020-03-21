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
