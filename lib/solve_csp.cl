#include <types.hpp>
#include <constraint_graph.hpp>
#include <lowest_penalties.hpp>
#include <labeling_finder.hpp>
#include <disparity_graph.hpp>
#include <image.hpp>
#include <indexing_checks.hpp>
#include <indexing.hpp>

void populate_structures_gpu(
    struct DisparityGraph* disparity_graph,
    struct LowestPenalties* lowest_penalties,
    struct ConstraintGraph* constraint_graph,
    __global int* nodes_availability,
    __global ulong* left_image,
    __global ulong* right_image,
    __global float* min_penalties_pixels,
    __global float* min_penalties_edges,
    __global float* reparametrization,
    ulong height,
    ulong width,
    ulong max_value,
    ulong disparity_levels,
    float threshold,
    float cleanness,
    float smoothness
)
{
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
}

void csp_solution_iteration_gpu(
    struct ConstraintGraph* constraint_graph,
    struct Pixel pixel,
    __global int* changed_step,
    __global int* changed_any
)
{
    do
    {
        *changed_step = 0;
        barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
        if (csp_process_pixel(constraint_graph, pixel))
        {
            *changed_step = 1;
            *changed_any = 1;
        }
        barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);
    } while (*changed_step);
}

__kernel void csp_iteration(
    __global int* nodes_availability,
    __global int* changed,
    __global ulong* left_image,
    __global ulong* right_image,
    __global float* min_penalties_pixels,
    __global float* min_penalties_edges,
    __global float* reparametrization,
    ulong height,
    ulong width,
    ulong max_value,
    ulong disparity_levels,
    float threshold,
    float cleanness,
    float smoothness
)
{
    ulong thread_id = get_global_id(0);

    struct DisparityGraph disparity_graph;
    struct LowestPenalties lowest_penalties;
    struct ConstraintGraph constraint_graph;

    populate_structures_gpu(
        &disparity_graph,
        &lowest_penalties,
        &constraint_graph,
        nodes_availability,
        left_image,
        right_image,
        min_penalties_pixels,
        min_penalties_edges,
        reparametrization,
        height,
        width,
        max_value,
        disparity_levels,
        threshold,
        cleanness,
        smoothness
    );

    atomic_and(&changed[0], 0);
    atomic_and(&changed[1], 0);

    struct Pixel pixel;
    pixel.x = thread_id % constraint_graph.disparity_graph->right.width;
    pixel.y = thread_id / constraint_graph.disparity_graph->right.width;

    csp_solution_iteration_gpu(
        &constraint_graph,
        pixel,
        &changed[0],
        &changed[1]
    );
}

__kernel void choose_best_node_gpu(
    __global int* nodes_availability,
    __global ulong* left_image,
    __global ulong* right_image,
    __global float* min_penalties_pixels,
    __global float* min_penalties_edges,
    __global float* reparametrization,
    ulong height,
    ulong width,
    ulong max_value,
    ulong disparity_levels,
    float threshold,
    float cleanness,
    float smoothness,
    ulong pixel_x,
    ulong pixel_y
)
{
    struct DisparityGraph disparity_graph;
    struct LowestPenalties lowest_penalties;
    struct ConstraintGraph constraint_graph;

    populate_structures_gpu(
        &disparity_graph,
        &lowest_penalties,
        &constraint_graph,
        nodes_availability,
        left_image,
        right_image,
        min_penalties_pixels,
        min_penalties_edges,
        reparametrization,
        height,
        width,
        max_value,
        disparity_levels,
        threshold,
        cleanness,
        smoothness
    );

    struct Pixel pixel;
    pixel.x = pixel_x;
    pixel.y = pixel_y;
    choose_best_node(&constraint_graph, pixel);
}
