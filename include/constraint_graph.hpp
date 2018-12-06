/*
 * MIT License
 *
 * Copyright (c) 2018 char-lie
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
#ifndef CONSTRAINT_GRAPH_HPP
#define CONSTRAINT_GRAPH_HPP

#include <disparity_graph.hpp>
#include <image.hpp>

#include <vector>

/**
 * \brief Boolean type alias.
 * to use the same name on CPU and GPU.
 */
using BOOL = bool;
/**
 * \brief Boolean array type alias
 * to use the same name on CPU and GPU.
 */
using BOOL_ARRAY = std::vector<BOOL>;

/**
 * \brief Structure to represent a graph with constraints
 * on choice of disparities for pixels (CSP problem).
 *
 * After optimization performed on DisparityGraph,
 * it's needed to choose one labeling of many others
 * that satisfy constraints of the problem.
 */
struct ConstraintGraph
{
    /**
     * \brief Array that contains markers for availability of nodes.
     *
     * `false` means that the node cannot be chosen.
     * `true` means that the node can be chosen under applied constraints.
     */
    BOOL_ARRAY nodes_availability;
    /**
     * \brief Array that contains markers for availability of edges.
     *
     * `false` means that the edge cannot be chosen.
     * `true` means that the edge can be chosen under applied constraints.
     */
    BOOL_ARRAY edges_availability;
};

/**
 * \brief Build a CSP problem for given DisparityGraph.
 *
 * Corresponding ConstraintGraph should have
 * the same amount of nodes and edges as corresponding DisparityGraph.
 *
 * First, all nodes and edges assumed to be unavailable.
 * Then, each Node that has a penalty that differs from the minimal
 * less than by `threshold`, is marked as available one.
 * The same procedure is performed for each Edge:
 * if an Edge is worse than the best one not more than for `threshold`,
 * then it's marked as available.
 *
 * Denoting vertex penalty
 *
 * \f[
 *  q_i\left( d; \Phi \right)
 *  = \left\|
 *        R\left( i^x, i^y \right) - L\left( i^x + d, i^y \right)
 *    \right\|^p
 *    + \sum\limits_{j \in \mathcal{N}_i}
 *        \varphi_{i j}\left( d \right)
 * \f]
 *
 * and edge penalty
 *
 * \f[
 *  g_{ij}\left( d, d'; \Phi \right)
 *  = \left\| d - d' \right\|^p
 *    - \varphi_{i j}\left( d \right)
 *    - \varphi_{j i}\left( d' \right),
 * \f]
 *
 * assuming the \f$\mathbb{I}\f$ to be an indicator function,
 * we have the following formulas to set initial constraints for nodes
 *
 * \f[
 *  b_i\left( d \right)
 *  = \mathbb{I}\left(
 *      q_i\left( d; \Phi \right)
 *      - \min\limits_{\delta \in D}{q_i\left( \delta; \Phi \right)}
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
 *      g_{ij}\left( d, d'; \Phi \right)
 *      - \min\limits_{\left\langle \delta, \delta' \right\rangle \in D^2}{
 *          g_{ij}\left( \delta, \delta'; \Phi \right)}
 *      \le \varepsilon
 *  \right),
 *  \quad i \in I,
 *  \quad j \in N_i,
 *  \quad \left\langle d, d' \right\rangle \in D^2.
 * \f]
 */
struct ConstraintGraph disparity2constraint(
    const struct DisparityGraph& disparity_graph,
    FLOAT threshold
);

#endif
