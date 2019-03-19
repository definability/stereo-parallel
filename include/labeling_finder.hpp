/*
 * MIT License
 *
 * Copyright (c) 2018-2019 char-lie
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
/**
 * @file
 */
#ifndef LABELING_FINDER_HPP
#define LABELING_FINDER_HPP

#include <lowest_penalties.hpp>

/**
 * \brief Construct an array of available differences
 * between penalties of nodes of the pixel
 * with the lowest penalty in the pixel.
 * The output is sorted in ascending order.
 */
FLOAT_ARRAY fetch_pixel_available_penalties(
    const DisparityGraph& graph,
    struct Pixel pixel,
    FLOAT minimal_penalty
);
/**
 * \brief Construct an array of available differences
 * between penalties of edges of the neighborhood
 * with the lowest penalty in the neighborhood.
 * The output is sorted in ascending order.
 */
FLOAT_ARRAY fetch_edge_available_penalties(
    const struct DisparityGraph& graph,
    struct Edge edge,
    FLOAT minimal_penalty
);
/**
 * \brief Construct an array of available differences
 * gathered from all pixels via ::fetch_pixel_available_penalties
 * and from all neighbors via ::fetch_edge_available_penalties.
 * The output is sorted in ascending order.
 */
FLOAT_ARRAY fetch_available_penalties(
    const struct LowestPenalties& lowest_penalties
);
/**
 * \brief Calculate the minimal threshold
 * for ConstraintGraph to have a solution.
 *
 * Nodes and edges with low penalty
 * may be consistent with graphs,
 * containing nodes and edges with high penalty.
 *
 * If we want to have a guarantee
 * that penalties of nodes and edges of the solution
 * differ from the lowest penalties
 * not more than by specified threshold,
 * we need to solve CSP,
 * removing all elements,
 * that have a big deviation from the corresponding minimal values.
 * The ::solve_csp does this.
 *
 * If we want to find the minimal available threshold,
 * which still allows the problem to be solvable,
 * we can find it using the following conclusions:
 *
 *   - Set of all available thresholds contains only differences
 *   between penalties of nodes and corresponding best nodes,
 *   and differences between penalties of edges
 *   and corresponding best edges.
 *   - We can use a binary search to find the best threshold,
 *   using ::solve_csp as the \f$ \ge \f$ comparison
 *   between the following solution and the best one.
 *
 * The algorithm:
 *
 *   -# Initialize indices of the first \f$ f \f$ and the last \f$ \ell \f$
 *   indices of the array.
 *   -# Set the current index \f$ i \gets \frac{f + \ell}{2} \f$.
 *   -# If \f$ f = \ell \f$,
 *   the solution is the \f$ i \f$-th threshold from the list.
 *   Finish the algorithm.
 *   -# If the problem is solvable,
 *   move the end to the center \f$ \ell \gets i \f$.
 *   -# Otherwise, move the start just after the center \f$ f \gets i + 1 \f$.
 *   -# Go to step `2`.
 *
 * This allows us to execute ::solve_csp
 * not more than \f$ \left[ \log_2 \ell + 1 \right] \f$ times.
 */
FLOAT calculate_minimal_consistent_threshold(
    const struct LowestPenalties& lowest_penalties,
    const struct DisparityGraph& disparity_graph,
    FLOAT_ARRAY available_penalties
);
/**
 * \brief Leave only the best available node in the pixel.
 * Remove all other nodes
 * (mark them as unavailable with ::make_node_unavailable).
 */
struct ConstraintGraph* choose_best_node(
    struct ConstraintGraph* graph,
    struct Pixel pixel
);
/**
 * \brief Find a labeling, consistent with the minimal available threshold.
 *
 * Use ::calculate_minimal_consistent_threshold
 * to find the threshold.
 * For each pixel
 *
 *   - execute ::choose_best_node to use the best node;
 *   - run ::solve_csp for the graph
 *   to remove inconsistent nodes and edges.
 *
 * As the result,
 * we will have a graph,
 * where the maximal deviation
 * from the best penalty in each node and edge
 * will be minimal.
 */
struct ConstraintGraph* find_labeling(struct ConstraintGraph* graph);
/**
 * \brief Build a disparity map by the constraint graph.
 *
 * For each pixel of the graph
 * find the available disparity
 * and add it as the pixel's intensity
 * to the output image.
 */
struct Image build_disparity_map(
    const struct ConstraintGraph& constraint_graph
);

#endif
