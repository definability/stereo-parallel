#ifndef GPU_CSP_HPP
#define GPU_CSP_HPP

#include <compile_cl.hpp>
#include <constraint_graph.hpp>

namespace gpu
{

namespace compute = boost::compute;

using boost::compute::command_queue;
using boost::compute::program;
using sp::graph::constraint::ConstraintGraph;
using sp::types::BOOL;
using sp::types::FLOAT;
using sp::types::ULONG;

/**
 * Source files for CSP solution.
 */
const vector<string> SOURCE_FILES = {
    "lib/constraint_graph.cpp",
    "lib/lowest_penalties.cpp",
    "lib/labeling_finder.cpp",
    "lib/disparity_graph.cpp",
    "lib/image.cpp",
    "lib/indexing_checks.cpp",
    "lib/indexing.cpp",
    "lib/solve_csp.cl",
};

/**
 * \brief Problem representation to be placed into GPU memory.
 * Flattened version of sp::graph::constraint::ConstraintGraph.
 */
struct Problem
{
    program program;
    command_queue queue;
    compute::vector<cl_ulong> left_image;
    compute::vector<cl_ulong> changed;
    compute::vector<cl_ulong> right_image;
    compute::vector<cl_float> min_penalties_pixels;
    compute::vector<cl_float> min_penalties_edges;
    compute::vector<cl_float> reparametrization;
};

/**
 * \brief Create command queue and program for solving CSP,
 * and add them to the Problem.
 */
void build_csp_program(struct Problem* problem);
/**
 * Initialize fields of the Problem
 * with values from the sp::graph::constraint::ConstraintGraph.
 */
void prepare_problem(struct ConstraintGraph* graph, struct Problem* problem);

/**
 * \brief Remove all nodes that have no any edges at least to one neighbor
 * until there is nothing to remove
 * (anything is removed or all nodes have neighbors),
 * and save the nodes availability data
 * (sp::graph::constraint::ConstraintGraph::nodes_availability)
 * into the input sp::graph::constraint::ConstraintGraph.
 */
BOOL csp_solution_cl(
    struct ConstraintGraph* graph,
    struct Problem* problem
);

}

#endif
