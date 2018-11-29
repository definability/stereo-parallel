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
#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <image.hpp>

#include <algorithm>
#include <vector>

/**
 * \brief Floating point type alias.
 * to use the same name on CPU and GPU.
 */
using FLOAT = double;
/**
 * \brief Floating point array type alias
 * to use the same name on CPU and GPU.
 */
using FLOAT_ARRAY = std::vector<FLOAT>;
/**
 * \brief Maximal number of neighbors of each vertex of disparity graph.
 *
 * Each pixel of an image has four neighbors in current problem:
 * - right
 * - left
 * - bottom
 * - top
 */
const ULONG NEIGHBORS_COUNT = 4;

/**
 * \brief Structure to represent a graph of MRF
 * (Markov random field) for MAP (maximal a posteriory) problem
 * of stereo vision.
 *
 * Let's call width of images \f$w\f$ and height \f$h\f$,
 * introduce sets \f$X = \left\{ 1, \dots, w \right\}\f$
 * and \f$Y = \left\{ 1, \dots, h \right\}\f$,
 * and available grayscale colors
 * \f$C = \left\{ 1, \dots, \max\left( C \right) \right\}\f$.
 * For convenience, we'll define \f$I = X \times Y\f$.
 * One element of the index set \f$I\f$ can be written as
 *
 * \f[
 *  I \ni i = \left\langle i^x, i^y \right\rangle
 * \f]
 *
 * We can represent images as mappings
 *
 * \f[
 *  L: I \rightarrow C \\
 *  R: I \rightarrow C \\
 * \f]
 *
 * Disparity is a difference
 * between number of a column of a pixel of the left image
 * and number of a column of corresponding pixel of the right image.
 * Given maximal disparity \f$d\f$ and denoting
 * \f$D = \left\{ 1, \dots, d \right\}\f$,
 * correspondence function is called labeling and its signature is
 *
 * \f[
 *  k: I \rightarrow D
 * \f]
 *
 * Thus, given an index \f$i_R\f$ of a pixel on the right image
 * and specific \f$k\f$ mapping,
 * we can find an index of corresponding pixel of the left image
 * by the formula
 *
 * \f[
 *  i_L = \left\langle i_R^x + k_{i_R}, i_R^y \right\rangle
 * \f]
 *
 * Set of all neighbors of a pixel will be noted \f$\mathcal{N}_i\f$.
 * Set with right and buttom neighbors
 * will be noted \f$N_i\f$.
 * The last one is empty for the right bottom pixel,
 * contains only one element for pixels
 * located on the right column or on the bottom row.
 *
 * The problem is to find such \f$k\f$
 * that minimizes the following function
 * given a norm \f$\left\| \cdot \right\|\f$
 *
 * \f[
 *  E\left( k \right) =
 *  \sum\limits_{i \in I} \left\|
 *      R\left( i^x, i^y \right)
 *      - L\left( i^x + k_i, i^y \right)
 *  \right\|
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \left\| k_i - k_j \right\|
 * \f]
 */
struct DisparityGraph
{
    /**
     * \brief Image made by a camera located by the left hand of another one.
     */
    struct Image left;
    /**
     * \brief Image made by a camera located by the right hand of another one.
     */
    struct Image right;
    /**
     * \brief Maximal distance between positions of pixels
     * that see the same 3D point.
     */
    ULONG maximal_disparity;
    /**
     * \brief Reparametrization is a helpful vector
     * for the optimization problem.
     *
     * It's easy to check that the following inequality holds
     * for any norm and any labeling
     *
     * \f[
     *  \min\limits_{k: I \rightarrow D}{E\left( k \right)}
     *  = \min\limits_{k: I \rightarrow D}{\left\{
     *      \sum\limits_{i \in I} \left\|
     *          R\left( i^x, i^y \right)
     *          - L\left( i^x + k_i, i^y \right)
     *      \right\|
     *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
     *          \left\| k_i - k_j \right\|
     *  \right\}}
     *  \ge \sum\limits_{i \in I} \min\limits_{d \in D}{
     *  \left\|
     *      R\left( i^x, i^y \right)
     *      - L\left( i^x + d, i^y \right)
     *  \right\|}
     *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
     *      \min\limits_{d, d' \in D}{\left\| d - d' \right\|}
     * \f]
     *
     * The last sum is zero, so
     *
     * \f[
     *  \min\limits_{k: I \rightarrow D}{E\left( k \right)}
     *  = \min\limits_{k: I \rightarrow D}{\left\{
     *      \sum\limits_{i \in I} \left\|
     *          R\left( i^x, i^y \right)
     *          - L\left( i^x + k_i, i^y \right)
     *      \right\|
     *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
     *          \left\| k_i - k_j \right\|
     *  \right\}}
     *  \ge \sum\limits_{i \in I} \min\limits_{d \in D}{
     *  \left\|
     *      R\left( i^x, i^y \right)
     *      - L\left( i^x + d, i^y \right)
     *  \right\|}
     *  = \widetilde{E}
     * \f]
     *
     * Thus, \f$\widetilde{E}\f$ is a lower bound for the \f$E\f$.
     * It appears that there are many other possible penalties,
     * which lead to the same \f$E\left( k \right)\f$
     * for specific labeling \f$k\f$.
     * The following function we call a parametrization
     *
     * \f[
     *  \varphi: I^2 \times K \rightarrow \mathbb{R}
     * \f]
     *
     * Let's introduce parametrized energy function
     *
     * \f[
     *  E\left( k; \varphi \right)
     *      = \sum\limits_{i \in I} \left[
     *          \left\|
     *              R\left( i^x, i^y \right) - L\left( i^x + k_i, i^y \right)
     *          \right\|
     *          + \sum\limits_{j \in \mathcal{N}_i}
     *              \varphi_{i j}\left( k_i \right)
     *      \right]
     *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i} \left[
     *          \left\| k_i - k_j \right\|
     *          - \varphi_{i j}\left( k_i \right)
     *          - \varphi_{j i}\left( k_j \right)
     *      \right]
     *  = E\left( k \right),
     *  \quad \forall \varphi: I^2 \times K \rightarrow \mathbb{R}
     * \f]
     *
     * Though, the lower boundary of reparametrized energy may change
     *
     * \f[
     *  \widetilde{E}\left( \varphi \right)
     *      = \sum\limits_{i \in I} \min_{d \in D}{\left[
     *          \left\|
     *              R\left( i^x, i^y \right) - L\left( i^x + k_i, i^y \right)
     *          \right\|
     *          + \sum\limits_{j \in \mathcal{N}_i}
     *              \varphi_{i j}\left( k_i \right)
     *      \right]}
     *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
     *          \min_{d, d' \in D}{\left[
     *              \left\| k_i - k_j \right\|
     *              - \varphi_{i j}\left( k_i \right)
     *              - \varphi_{j i}\left( k_j \right)
     *          \right]}
     *  \neq \widetilde{E}
     * \f]
     *
     * If we will increase the lower boundary
     * \f$\widetilde{E}\left( \varphi \right)\f$,
     * it won't overflow the \f$E\left( k; \varphi \right)\f$,
     * but, in some cases, may become as close to it,
     * that the labels chosen in the lower boundary
     * will be a solution to the minimization problem.
     * We only need to find such \f$\varphi\f$
     * that all \f$d\f$ and \f$d'\f$
     * in the \f$\widetilde{E}\left( \varphi \right)\f$
     * will be consistent, e. g., if each pixel
     * will have only one disparity assigned to it.
     *
     * Dual problem to the original one is a maximization
     * of the \f$\widetilde{E}\left( \varphi \right)\f$
     *
     * \f[
     *  \sum\limits_{i \in I} \min_{d \in D}{\left[
     *      \left\|
     *          R\left( i^x, i^y \right) - L\left( i^x + k_i, i^y \right)
     *      \right\|
     *      + \sum\limits_{j \in \mathcal{N}_i}
     *          \varphi_{i j}\left( k_i \right)
     *  \right]}
     *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
     *      \min_{d, d' \in D}{\left[
     *          \left\| k_i - k_j \right\|
     *          - \varphi_{i j}\left( k_i \right)
     *          - \varphi_{j i}\left( k_j \right)
     *      \right]}
     *  \to \max_{\varphi: I^2 \times K \rightarrow \mathbb{R}}
     * \f]
     *
     * You can find more information in the following sources
     * - <a href="
     *    https://hci.iwr.uni-heidelberg.de/vislearn/HTML/people/
     *    bogdan/publications/papers/Dense-CombiLP-Haller-AAAI2017.pdf
     *   ">
     *   Exact MAP-inference by Confining Combinatorial Search
     *   with LP Relaxation
     *   </a> (2017)by Stefan Haller, Paul Swoboda and Bogdan Savchynskyy;
     * - <a href="
     *    http://cmp.felk.cvut.cz/~shekhovt/publications/
     *    as-phd-thesis-TR.pdf
     *   ">
     *   Exact and Partial Energy Minimization in Computer Vision
     *   </a> (2013) by Alexander Shekhovtsov.
     */
    FLOAT_ARRAY reparametrization;
    DisparityGraph(
        struct Image left,
        struct Image right,
        ULONG maximal_disparity
    );
};

struct Node
{
    struct Pixel pixel;
    ULONG disparity;
};

struct Edge
{
    struct Node node;
    struct Node neighbor;
};

ULONG neighbor_index(
    struct Pixel pixel,
    struct Pixel neighbor
);
bool neighborhood_exists(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
);
bool neighborhood_exists_fast(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
);
bool edge_exists(
    const struct DisparityGraph& graph,
    struct Edge edge
);
ULONG potential_index_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
);
ULONG potential_index(
    const struct DisparityGraph& graph,
    struct Node node,
    struct Pixel neighbor
);
ULONG potential_index_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
);
FLOAT potential_value(
    const struct DisparityGraph& graph,
    struct Node node,
    struct Pixel neighbor
);
FLOAT potential_value_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
);
FLOAT potential_value_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
);
FLOAT edge_penalty(const struct DisparityGraph& graph, struct Edge edge);
FLOAT node_penalty(const struct DisparityGraph& graph, struct Node node);

#endif
