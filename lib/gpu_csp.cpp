#include <compile_cl.hpp>
#include <gpu_csp.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace gpu
{

namespace compute = boost::compute;

using boost::compute::command_queue;
using boost::compute::kernel;
using boost::compute::system;
using std::cout;
using std::endl;
using std::string;
using std::vector;

void build_csp_program(struct Problem* problem)
{
    problem->queue = system::default_queue();
    problem->program = create_program(
        SOURCE_FILES,
        problem->queue.get_context()
    );
}

void prepare_problem(struct ConstraintGraph* graph, struct Problem* problem)
{
    problem->left_image.clear();
    problem->left_image.reserve(
        graph->disparity_graph->left.data.size(),
        problem->queue
    );
    std::copy(
        graph->disparity_graph->left.data.begin(),
        graph->disparity_graph->left.data.end(),
        std::back_inserter(problem->left_image)
    );
    problem->right_image.clear();
    problem->right_image.reserve(
        graph->disparity_graph->right.data.size(),
        problem->queue
    );
    std::copy(
        graph->disparity_graph->right.data.begin(),
        graph->disparity_graph->right.data.end(),
        std::back_inserter(problem->right_image)
    );
    problem->min_penalties_pixels.clear();
    problem->min_penalties_pixels.reserve(
        graph->lowest_penalties->pixels.size(),
        problem->queue
    );
    std::copy(
        graph->lowest_penalties->pixels.begin(),
        graph->lowest_penalties->pixels.end(),
        std::back_inserter(problem->min_penalties_pixels)
    );
    problem->min_penalties_edges.clear();
    problem->min_penalties_edges.reserve(
        graph->lowest_penalties->neighborhoods.size(),
        problem->queue
    );
    std::copy(
        graph->lowest_penalties->neighborhoods.begin(),
        graph->lowest_penalties->neighborhoods.end(),
        std::back_inserter(problem->min_penalties_edges)
    );
    problem->reparametrization.clear();
    problem->reparametrization.reserve(
        graph->disparity_graph->reparametrization.size(),
        problem->queue
    );
    std::copy(
        graph->disparity_graph->reparametrization.begin(),
        graph->disparity_graph->reparametrization.end(),
        std::back_inserter(problem->reparametrization)
    );
}

BOOL csp_solution_cl(
    struct ConstraintGraph* graph,
    struct Problem* problem
)
{
    command_queue queue = system::default_queue();

    compute::vector<cl_int> nodes_availability;
    nodes_availability.reserve(
        graph->nodes_availability.size(),
        problem->queue
    );
    std::copy(
        graph->nodes_availability.begin(),
        graph->nodes_availability.end(),
        std::back_inserter(nodes_availability)
    );

    compute::vector<cl_int> changed({0, 0}, problem->queue);

    kernel csp_iteration = problem->program.create_kernel("csp_iteration");
    csp_iteration.set_args(
        nodes_availability.get_buffer(),
        changed.get_buffer(),
        problem->left_image.get_buffer(),
        problem->right_image.get_buffer(),
        problem->min_penalties_pixels.get_buffer(),
        problem->min_penalties_edges.get_buffer(),
        problem->reparametrization.get_buffer(),
        static_cast<cl_ulong>(graph->disparity_graph->right.height),
        static_cast<cl_ulong>(graph->disparity_graph->right.width),
        static_cast<cl_ulong>(graph->disparity_graph->right.max_value),
        static_cast<cl_ulong>(graph->disparity_graph->disparity_levels),
        static_cast<cl_float>(graph->threshold),
        static_cast<cl_float>(graph->disparity_graph->cleanness),
        static_cast<cl_float>(graph->disparity_graph->smoothness)
    );

    kernel choose_best_node_gpu = problem->program.create_kernel(
        "choose_best_node_gpu"
    );
    choose_best_node_gpu.set_args(
        nodes_availability.get_buffer(),
        problem->left_image.get_buffer(),
        problem->right_image.get_buffer(),
        problem->min_penalties_pixels.get_buffer(),
        problem->min_penalties_edges.get_buffer(),
        problem->reparametrization.get_buffer(),
        static_cast<cl_ulong>(graph->disparity_graph->right.height),
        static_cast<cl_ulong>(graph->disparity_graph->right.width),
        static_cast<cl_ulong>(graph->disparity_graph->right.max_value),
        static_cast<cl_ulong>(graph->disparity_graph->disparity_levels),
        static_cast<cl_float>(graph->threshold),
        static_cast<cl_float>(graph->disparity_graph->cleanness),
        static_cast<cl_float>(graph->disparity_graph->smoothness),
        static_cast<cl_ulong>(0),
        static_cast<cl_ulong>(0)
    );

    auto x_index = static_cast<cl_ulong>(choose_best_node_gpu.arity() - 2);
    auto y_index = static_cast<cl_ulong>(choose_best_node_gpu.arity() - 1);

    for (ULONG x = 0; x < graph->disparity_graph->right.width; ++x)
    {
        for (ULONG y = 0; y < graph->disparity_graph->right.height; ++y)
        {
            choose_best_node_gpu.set_arg(x_index, x);
            choose_best_node_gpu.set_arg(y_index, y);
            queue.enqueue_task(choose_best_node_gpu);
            queue.finish();

            do
            {
                queue.enqueue_1d_range_kernel(
                    csp_iteration,
                    0,
                    graph->disparity_graph->right.data.size(),
                    0
                );

                queue.finish();
            } while (changed[1] != 0);
        }
    }

    graph->nodes_availability.clear();
    std::copy(
        nodes_availability.begin(),
        nodes_availability.end(),
        std::back_inserter(graph->nodes_availability)
    );
    return changed[1] != 0;
}

}
