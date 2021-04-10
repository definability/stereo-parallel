/*
 * MIT License
 *
 * Copyright (c) 2018-2021 char-lie
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
#ifndef CONSTRAINT_GRAPH_HPP
#define CONSTRAINT_GRAPH_HPP

#include <disparity_graph.hpp>
#include <image.hpp>
#include <lowest_penalties.hpp>
#include <types.hpp>

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
/**
 * \brief Utilities to solve CSP.
 */
namespace sp
{
namespace graph
{
namespace constraint
{

using sp::graph::disparity::DisparityGraph;
using sp::graph::lowest_penalties::LowestPenalties;
using sp::types::BOOL;
using sp::types::BOOL_ARRAY;
using sp::types::Edge;
using sp::types::FLOAT;
using sp::types::Node;
using sp::types::Pixel;
using sp::types::ULONG;
#endif

/**
 * \brief Structure to represent a graph with constraints
 * on choice of disparities for pixels (constraint satisfaction problem, CSP).
 *
 * \section csp-problem-statement Statement of the problem
 *
 * After optimization performed on sp::graph::disparity::DisparityGraph,
 * it's needed to choose one labeling of many others
 * that satisfy constraints of the problem.
 *
 * Assuming the \f$\mathbb{I}\f$ to be an indicator function,
 * we have the following formulas to set initial constraints for nodes
 *
 * \f[
 *  b_i\left( d \right)
 *  = \mathbb{I}\left(
 *      q_i\left( d; \varphi \right)
 *      - \min\limits_{\delta \in D}{q_i\left( \delta; \varphi \right)}
 *      \le \varepsilon
 *  \right),
 *  \quad i \in I,
 *  \quad d \in D
 * \f]
 *
 * and edges
 *
 * \f[
 *  a_{ij}\left( d, d' \right)
 *  = \mathbb{I}\left(
 *      g_{ij}\left( d, d'; \varphi \right)
 *      - \min\limits_{\left\langle \delta, \delta' \right\rangle \in D^2}{
 *          g_{ij}\left( \delta, \delta'; \varphi \right)}
 *      \le \varepsilon
 *  \right),
 *  \quad i \in I,
 *  \quad j \in N_i,
 *  \quad \left\langle d, d' \right\rangle \in D^2.
 * \f]
 *
 * The problem is to find a disparity map,
 * for which all constraints are satisfied,
 * i. e., all nodes contained in the disparity map exist
 * and neighbors are connected by existent edges.
 *
 * There are cases when the constraint satisfaction problem.
 * One of these cases is ours.
 *
 * \section csp-solution Solution
 *
 * It's easy to solve the problem in our case
 * in iterative maneer.
 * Let's note initial availability of nodes \f$b^0\f$
 * and initial availability of edges \f$a^0\f$.
 * They're already defined.
 *
 * Iteration of the algorithm is following
 *
 * - Take each node.
 *  If the node is unavailable, it will never become available.
 *  Otherwise,
 *  check whether in each neighbor of pixel of the node
 *  there exists an available edge between at least one node of the pixel
 *  and current node.
 *  If there is no connection with at least one neighboring pixel,
 *  the node becomes unavailable.
 * - Take each edge.
 *  If the edge is unavailable, it will never become available.
 *  Otherwise,
 *  check whether both nodes of the edge are available.
 *  If at least one node is unavailable,
 *  the edge becomes unavailable.
 *
 * \f[
 *  \begin{cases}
 *      b^{k + 1}_i\left( d \right)
 *      = b^k_i\left( d \right)
 *          \wedge
 *              \bigwedge\limits_{j \in \mathcal{N}_i}
 *              \bigvee\limits_{d' \in D} a^k_{ij}\left( d, d' \right),
 *      \quad \forall i \in I,
 *      \quad \forall d \in D,
 *      \\
 *      a^{k + 1}_{ij}\left( d, d' \right)
 *      = a^k_{ij}\left( d, d' \right)
 *          \wedge b_i\left( d \right)
 *          \wedge b_j\left( d' \right),
 *      \quad \forall i \in I,
 *      \quad \forall j \in N_i,
 *      \quad \forall \left\langle d, d' \right\rangle \in D^2.
 *  \end{cases}
 * \f]
 *
 * Iteration is an execution of the step for all nodes and edges.
 *
 * If available nodes stay available
 * and available edges stay available after the \f$k\f$th iteration,
 * the algorighm is finished.
 * Otherwise, go to \f$k + 1\f$st iteration.
 *
 * \section csp-memory Memory usage
 *
 * \subsection memory-issue Issue
 *
 * Number of edges is approximately
 *
 * \f[
 *  \left| a \right|
 *  \approx \left| I \right|
 *  \cdot \max\limits_{i \in I}\left| \mathcal{N}_i \right|
 *  \cdot \left| D \right|^2
 * \f]
 *
 * This means, that for 1Mpx image \f$2^{10} \times 2^{10}\f$
 * with maximal disparity equal to \f$128 = 2^7\f$
 * and four neighbors (sp::graph::disparity::NEIGHBORS_COUNT)
 * number of edges is
 *
 * \f[
 *  \left| a \right|
 *  \approx \left( 2^{10} \right)^2 \cdot 4 \cdot \left( 2^7 \right)^2
 *  = 2^{20} \cdot 2^2 \cdot 2^{14}
 *  = 2^{36}
 *  \approx 64 \cdot 10^9
 * \f]
 *
 * Number of reparametrizetion elements under the same conditions is
 *
 * \f[
 *  \left| \varphi \right|
 *  \approx \left| I \right|
 *  \cdot \max\limits_{i \in I}\left| \mathcal{N}_i \right|
 *  \cdot \left| D \right|
 *  = \left( 2^{10} \right)^2 \cdot 4 \cdot 2^7
 *  = 2^{20} \cdot 2^2 \cdot 2^7
 *  = 2^{29}
 *  \approx 0.5 \cdot 10^9
 * \f]
 *
 * If we use 4-byte floating point numbers
 * for sp::graph::disparity::DisparityGraph::reparametrization,
 * the sp::graph::disparity::DisparityGraph will cost \f$\approx\f$ 2GB.
 * If we store information about availability of each edge in a single bit,
 * memory consumption of the sp::graph::constraint::ConstraintGraph
 * will be \f$\approx\f$ 8GB.
 *
 * It's problematic to store the last one,
 * not speaking about parallel solution of different CSPs,
 * because each instance should have its own copy of
 * sp::graph::constraint::ConstraintGraph.
 *
 *
 * \subsection memory-issue-solution Solution
 *
 * We can avoid usage of edge availability markers.
 * In the problem above, usage of node availability is
 *
 * \f[
 *  \left| b \right|
 *  \approx \left| I \right| \cdot \left| D \right|
 *  = 2^{10} \cdot 2^7
 *  = 2^{17}
 *  \approx 128 \cdot 10^3
 * \f]
 *
 * Even if we will use 8 bytes per value,
 * nodes' availability array will consume less than a megabyte
 * of memory.
 *
 * We know, that unavailable edge will never become available.
 * It may be unavailable for two reasons:
 *
 * -# It has failed comparison with \f$\varepsilon\f$;
 * -# One of its node is/became unavailable.
 *
 * Thus, we can use the formula
 *
 * \f[
 *  a^{k + 1}_{ij}\left( d, d' \right)
 *  = \mathbb{I}\left(
 *    g_{ij}\left( d, d'; \varphi \right)
 *    - \min\limits_{\left\langle \delta, \delta' \right\rangle \in D^2}{
 *        g_{ij}\left( \delta, \delta'; \varphi \right)}
 *    \le \varepsilon
 *  \right)
 *  \wedge b^k_i\left( d \right)
 *  \wedge b^k_j\left( d' \right).
 * \f]
 *
 * Now, the formula for computation of
 * \f$a^{k + 1}_{ij}\left( d, d' \right)\f$
 * doesn't depend directly on \f$a^k\f$
 *
 * \f[
 *  b^{k + 1}_i\left( d \right)
 *  = b^k_i\left( d \right)
 *      \wedge \bigwedge\limits_{j \in \mathcal{N}_i} \bigvee\limits_{d' \in D}
 *          b^k_j\left( d' \right)
 *          \wedge \mathbb{I}\left(
 *            g_{ij}\left( d, d'; \varphi \right)
 *            - \min\limits_{
 *              \left\langle \delta, \delta' \right\rangle \in D^2}{
 *              g_{ij}\left( \delta, \delta'; \varphi \right)}
 *            \le \varepsilon
 *          \right).
 * \f]
 *
 * We can easily precompute each
 * \f$\min\limits_{\left\langle \delta, \delta' \right\rangle \in D^2}
 * {g_{ij}\left( \delta, \delta'; \varphi \right)}\f$:
 * there are \f$4 \cdot \left| I \right|\f$ of them
 * (one for each pair of neighboring pixels).
 * In this case,
 * computation of initial availability for an edge
 * will cost almost only computation of its penalty.
 *
 * It's needed to store a pointer to a sp::graph::disparity::DisparityGraph
 * in sp::graph::constraint::ConstraintGraph
 * to use sp::graph::disparity::edge_penalty.
 */
struct ConstraintGraph
{
    /**
     * \brief sp::graph::disparity::DisparityGraph instance
     * for which the sp::graph::constraint::ConstraintGraph instance
     * was created.
     */
    const struct DisparityGraph* disparity_graph;
    /**
     * \brief sp::graph::lowest_penalties::LowestPenalties instance
     * to check availability of nodes and edges faster.
     */
    const struct LowestPenalties* lowest_penalties;
    /**
     * \brief Array that contains markers for availability of nodes.
     *
     * `false` means that the node cannot be chosen.
     * `true` means that the node can be chosen under applied constraints.
     *
     * Index of availability
     * of specific Node from
     * sp::graph::constraint::ConstraintGraph::nodes_availability
     * can be calculated by formula
     *
     * \f[
     *  k\left( \left\langle x, y \right\rangle, d \right) =
     *  d + \max{D} \cdot \left( y + h \cdot x \right),
     * \f]
     *
     * where \f$\left\langle x, y \right\rangle\f$
     * are coordinates of Node::pixel,
     * \f$i\f$ is an index of used neighbor
     * and \f$d\f$ is a Node::disparity.
     */
    __global BOOL_ARRAY nodes_availability;
    /**
     * \brief Threshold to compare penalty of an edge with the minimal one
     * against.
     *
     * \f$\varepsilon\f$ in formulas of the class description.
     */
    FLOAT threshold;
    /**
     * \brief Build a CSP problem for given sp::graph::disparity::DisparityGraph.
     *
     * Corresponding sp::graph::constraint::ConstraintGraph should have
     * the same amount of nodes as corresponding sp::graph::disparity::DisparityGraph.
     *
     * First, all nodes and edges assumed to be unavailable.
     * Then, each Node that has a penalty that differs from the minimal
     * less than by `threshold`, is marked as available one.
     *
     * To not recalculate lowest penalties,
     * it's good to have precalculated
     * sp::graph::lowest_penalties::LowestPenalties instance.
     */
    #if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
    ConstraintGraph() = default;
    ConstraintGraph(
        const struct DisparityGraph* disparity_graph,
        const struct LowestPenalties* lowest_penalties,
        FLOAT threshold
    );
    #endif
};
/**
 * \brief Mark specific Node as available (`true`).
 *
 * The function doesn't check existence of the Node.
 * You should perform it by yourself
 * using sp::indexing::checks::node_exists.
 */
__device__ void make_node_available(
    struct ConstraintGraph* graph,
    struct Node node
);
/**
 * \brief Mark specific Node as unavailable (`false`).
 *
 * The function doesn't check existence of the Node.
 * You should perform it by yourself
 * using sp::indexing::checks::node_exists.
 */
__device__ void make_node_unavailable(
    struct ConstraintGraph* graph,
    struct Node node
);
/**
 * \brief Mark all nodes as unavailable.
 */
__device__ void make_all_nodes_unavailable(struct ConstraintGraph* graph);
/**
 * \brief Check whether the Node is still available.
 *
 * Takes value from sp::graph::constraint::ConstraintGraph::nodes_availability array.
 *
 * The function doesn't check existence of the Node.
 * You should perform it by yourself
 * using sp::indexing::checks::node_exists.
 */
__device__ BOOL is_node_available(
    const struct ConstraintGraph* graph,
    struct Node node
);
/**
 * \brief Check whether the Edge is still available.
 *
 * Compares penalty of the Edge with given
 * sp::graph::constraint::ConstraintGraph::threshold
 * and checks whether two nodes of the Edge are available
 * using sp::graph::constraint::ConstraintGraph::is_node_available.
 *
 * The function checks existence of the Edge
 * using sp::indexing::checks::edge_exists.
 */
__device__ BOOL is_edge_available(
    const struct ConstraintGraph* graph,
    struct Edge edge
);
/**
 * \brief Check whether the Node should be removed.
 *
 * If for each neighbor of the Node
 * there exists at least one Edge that was not removed,
 * the Node is still available.
 * Otherwise, it should be marked as removed.
 *
 * The function doesn't check existence of the Node.
 * You should perform it by yourself
 * using sp::indexing::checks::node_exists.
 */
__device__ BOOL should_remove_node(
    const struct ConstraintGraph* graph,
    struct Node node
);
/**
 * \brief Check all nodes of the Pixel and remove the redundant ones.
 *
 * @return
 *  Boolean flag.
 *  `true` if availability of at least one node was changed.
 *  `false` if a solution was found (at least an empty one)
 *  and nothing was changed during iteration.
 */
__device__ BOOL csp_process_pixel(
    struct ConstraintGraph* graph,
    struct Pixel pixel
);
/**
 * \brief Perform one iteration of sp::graph::constraint::solve_csp.
 *
 * Can be used for the parallel processing
 * of the sp::graph::constraint::ConstraintGraph.
 * Each node can be processed independently:
 * if different threads decided
 * that an existent node needs to be removed,
 * and will remove it simultaneously,
 * there's almost nothing bad in this case.
 * The only bad thing is
 * that these threads will do the same work,
 * and this will eat processor time to no purpose.
 *
 * @return
 *  Boolean flag.
 *  `true` if availability of one node was changed.
 *  `false` if a solution was found (at least, an empty one)
 *  and nothing was changed during iteration.
 */
__device__ BOOL csp_solution_iteration(
    struct ConstraintGraph* graph,
    ULONG jobs,
    ULONG job_number
);
/**
 * \brief Remove all nodes that don't belong to any soluton.
 *
 * @return
 *  Boolean flag.
 *  `true` if nonempty solution was found.
 *  `false` if all nodes were removed --- the problem is unsolvable.
 */
__device__ BOOL solve_csp(struct ConstraintGraph* graph);
/**
 * \brief Check whether at least one node is available.
 */
__device__ BOOL check_nodes_left(const struct ConstraintGraph* graph);

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
}
}
}
#endif

#endif
