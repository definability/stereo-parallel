#include <constraint_graph.hpp>
#include <disparity_graph.hpp>
#include <image.hpp>
#include <indexing_checks.hpp>
#include <indexing.hpp>
#include <labeling_finder.hpp>
#include <lowest_penalties.hpp>
#include <solve_csp.hpp>
#include <types.hpp>

#include <cuda.h>
#include <cuda_runtime.h>
#include <cooperative_groups.h>

#if !defined(__CUDA_ARCH__)
using sp::types::ULONG;
using sp::types::ULONG_ARRAY;
using sp::types::Pixel;
using sp::types::Edge;
using sp::types::Node;
using sp::image::Image;
using sp::graph::disparity::DisparityGraph;
using sp::graph::constraint::ConstraintGraph;
using sp::graph::lowest_penalties::LowestPenalties;
using sp::labeling::finder::choose_best_node;
#endif

__device__ void populate_structures_gpu(
    struct DisparityGraph* disparity_graph,
    struct LowestPenalties* lowest_penalties,
    struct ConstraintGraph* constraint_graph,
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
    float smoothness
)
{
    #if defined(__CUDA_ARCH__)
    disparity_graph->left.data = left_image;
    disparity_graph->left.height = height;
    disparity_graph->left.max_value = max_value;
    disparity_graph->left.width = width;

    disparity_graph->right.data = right_image;
    disparity_graph->right.height = height;
    disparity_graph->right.max_value = max_value;
    disparity_graph->right.width = width;

    disparity_graph->cleanness = cleanness;
    disparity_graph->disparity_levels = disparity_levels;
    disparity_graph->reparametrization = reparametrization;
    disparity_graph->smoothness = smoothness;

    lowest_penalties->graph = disparity_graph;
    lowest_penalties->pixels = min_penalties_pixels;
    lowest_penalties->neighborhoods = min_penalties_edges;

    constraint_graph->threshold = threshold;
    constraint_graph->nodes_availability = nodes_availability;
    constraint_graph->lowest_penalties = lowest_penalties;
    constraint_graph->disparity_graph = disparity_graph;
    #endif
}

__device__ void csp_solution_iteration_gpu(
    struct ConstraintGraph* constraint_graph,
    struct Pixel pixel,
    int* changed
)
{
    cooperative_groups::grid_group g = cooperative_groups::this_grid();
    do
    {
        *changed = 0;
        g.sync();
        if (csp_process_pixel(constraint_graph, pixel))
        {
            *changed = 1;
        }
        g.sync();
    } while (*changed);
}
