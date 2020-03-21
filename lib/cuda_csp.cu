#ifndef __CUDA_ARCH__
#include <cuda_csp.hpp>
#include <solve_csp.hpp>

#include <cassert>
#include <cstdio>
#include <vector>

#define cdpErrchk(ans) { cdpAssert((ans), __FILE__, __LINE__); }
__host__ __device__ void cdpAssert(cudaError_t code, const char *file, int line)
{
   if (code != cudaSuccess)
   {
      printf("GPU kernel assert: %s %s %d\n", cudaGetErrorString(code), file, line);
      assert(0);
   }
}

#define cudaCheckError() {                                          \
 cudaError_t e=cudaGetLastError();                                 \
 if(e!=cudaSuccess) {                                              \
   printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e));           \
   exit(0); \
 }                                                                 \
}

namespace gpu
{

using std::vector;

void prepare_problem(struct ConstraintGraph* graph, struct CUDAProblem* problem)
{
    vector<unsigned> left_image(
        graph->disparity_graph->left.data.begin(),
        graph->disparity_graph->left.data.end()
    );
    vector<float> min_penalties_edges(
        graph->lowest_penalties->neighborhoods.begin(),
        graph->lowest_penalties->neighborhoods.end()
    );
    vector<float> min_penalties_pixels(
        graph->lowest_penalties->pixels.begin(),
        graph->lowest_penalties->pixels.end()
    );
    vector<int> nodes_availability(
        graph->nodes_availability.begin(),
        graph->nodes_availability.end()
    );
    vector<float> reparametrization(
        graph->disparity_graph->reparametrization.begin(),
        graph->disparity_graph->reparametrization.end()
    );
    vector<unsigned> right_image(
        graph->disparity_graph->right.data.begin(),
        graph->disparity_graph->right.data.end()
    );

    cdpErrchk(cudaMalloc(
        (void**)&(problem->changed),
        sizeof(int))
    );
    cdpErrchk(cudaMalloc(
        &(problem->left_image),
        left_image.size() * sizeof(left_image[0]))
    );
    cdpErrchk(cudaMalloc(
        &(problem->min_penalties_edges),
        min_penalties_edges.size() * sizeof(min_penalties_edges[0]))
    );
    cdpErrchk(cudaMalloc(
        &(problem->min_penalties_pixels),
        min_penalties_pixels.size() * sizeof(min_penalties_pixels[0]))
    );
    cdpErrchk(cudaMalloc(
        &(problem->nodes_availability),
        nodes_availability.size() * sizeof(nodes_availability[0]))
    );
    cdpErrchk(cudaMalloc(
        &(problem->reparametrization),
        reparametrization.size() * sizeof(reparametrization[0]))
    );
    cdpErrchk(cudaMalloc(
        &(problem->right_image),
        right_image.size() * sizeof(right_image[0]))
    );

    cdpErrchk(cudaMemcpy(
        problem->changed,
        changed.data(),
        changed.size() * sizeof(int),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->left_image,
        left_image.data(),
        left_image.size() * sizeof(left_image[0]),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->min_penalties_edges,
        min_penalties_edges.data(),
        min_penalties_edges.size() * sizeof(min_penalties_edges[0]),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->min_penalties_pixels,
        min_penalties_pixels.data(),
        min_penalties_pixels.size() * sizeof(min_penalties_pixels[0]),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->nodes_availability,
        nodes_availability.data(),
        nodes_availability.size() * sizeof(nodes_availability[0]),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->reparametrization,
        reparametrization.data(),
        reparametrization.size() * sizeof(reparametrization[0]),
        cudaMemcpyHostToDevice)
    );
    cdpErrchk(cudaMemcpy(
        problem->right_image,
        right_image.data(),
        right_image.size() * sizeof(right_image[0]),
        cudaMemcpyHostToDevice)
    );
}

void free_problem(struct ConstraintGraph* graph, struct CUDAProblem* problem)
{
    vector<int> nodes_availability(graph->nodes_availability.size());
    cdpErrchk(cudaMemcpy(
        nodes_availability.data(),
        problem->nodes_availability,
        nodes_availability.size() * sizeof(nodes_availability[0]),
        cudaMemcpyDeviceToHost)
    );

    graph->nodes_availability.assign(
        nodes_availability.begin(),
        nodes_availability.end()
    );

    cdpErrchk(cudaDeviceSynchronize());

    cdpErrchk(cudaFree(problem->changed));
    cdpErrchk(cudaFree(problem->left_image));
    cdpErrchk(cudaFree(problem->min_penalties_edges));
    cdpErrchk(cudaFree(problem->min_penalties_pixels));
    cdpErrchk(cudaFree(problem->nodes_availability));
    cdpErrchk(cudaFree(problem->reparametrization));
    cdpErrchk(cudaFree(problem->right_image));

    cdpErrchk(cudaDeviceSynchronize());
}

}
#endif
