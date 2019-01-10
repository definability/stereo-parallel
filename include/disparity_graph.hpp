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
 * \section problem-statement Statement of the problem
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
 *  I \ni i = \left\langle i^x, i^y \right\rangle.
 * \f]
 *
 * We can represent images as mappings
 *
 * \f[
 *  L: I \rightarrow C, \\
 *  R: I \rightarrow C. \\
 * \f]
 *
 * Disparity is a difference
 * between number of a column of a pixel of the left image
 * and number of a column of corresponding pixel of the right image.
 * Given number of disparities (DisparityGraph::disparity_levels)
 * \f$\left| D \right|\f$ and denoting
 * \f$D = \left\{ 0, \dots, \max{D} \right\}\f$,
 * correspondence function is called labeling and its signature is
 *
 * \f[
 *  k: I \rightarrow D.
 * \f]
 *
 * Thus, given an index \f$i_R\f$ of a pixel on the right image
 * and specific \f$k\f$ mapping,
 * we can find an index of corresponding pixel of the left image
 * by the formula
 *
 * \f[
 *  i_L = \left\langle i_R^x + k_{i_R}, i_R^y \right\rangle.
 * \f]
 *
 * Color scales may be different,
 * as well as noise level of images.
 * Weight \f$\alpha\f$ (DisparityGraph::cleanness)
 * allows to control these factors.
 *
 * Observed scene may be smooth or sharp.
 * Also, it may be inconvenient to use \f$\alpha\f$ parameter
 * because it needs to be too small or too high.
 * \f$\beta\f$ weight (DisparityGraph::smoothness) serves in this case.
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
 * and positive \f$p \in \mathbb{R}\f$
 *
 * \f[
 *  E\left( k \right) =
 *  \sum\limits_{i \in I} \alpha \cdot \left\|
 *      R\left( i^x, i^y \right)
 *      - L\left( i^x + k_i, i^y \right)
 *  \right\|^p
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \beta \cdot \left\| k_i - k_j \right\|^p.
 * \f]
 *
 * Denoting vertex penalty
 *
 * \f[
 *  q_i\left( d \right)
 *  = \alpha \cdot \left\|
 *        R\left( i^x, i^y \right) - L\left( i^x + d, i^y \right)
 *    \right\|^p
 * \f]
 *
 * and edge penalty
 *
 * \f[
 *  g_{ij}\left( d, d' \right)
 *  = \beta \cdot \left\| d - d' \right\|^p,
 * \f]
 *
 * it's needed to solve
 *
 * \f[
 *  \sum\limits_{i \in I} q_i\left( k_i \right)
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      g_{ij}\left( k_i, k_j \right)
 *  \to \min\limits_{k: I \rightarrow D}.
 * \f]
 *
 * \section dual-problem Dual problem
 *
 * It's easy to check that the following inequality holds
 * for any norm and any labeling
 *
 * \f[
 *  \min\limits_{k: I \rightarrow D}{E\left( k \right)}
 *  = \min\limits_{k: I \rightarrow D}{\left\{
 *      \sum\limits_{i \in I} q_i\left( k_i \right)
 *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *          g_{ij}\left( k_i, k_j \right)
 *  \right\}}
 *  \ge \sum\limits_{i \in I} \min\limits_{d \in D}{
 *      q_i\left( d \right)
 *  }
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \min\limits_{d, d' \in D}{g_{ij}\left( d, d' \right)}
 *  = \widetilde{E}.
 * \f]
 *
 * Thus, \f$\widetilde{E}\f$ is a lower bound for the \f$E\f$.
 * It appears that there are many other possible penalties,
 * which lead to the same \f$E\left( k \right)\f$
 * for specific labeling \f$k\f$.
 * The following function we call a reparametrization
 * (DisparityGraph::reparametrization)
 *
 * \f[
 *  \varphi: I^2 \times K \rightarrow \mathbb{R}.
 * \f]
 *
 * Let's introduce reparametrized energy function
 *
 * \f[
 *  E\left( k; \varphi \right)
 *      = \sum\limits_{i \in I} \left[
 *          q_i\left( k_i \right)
 *          + \sum\limits_{j \in \mathcal{N}_i}
 *              \varphi_{i j}\left( k_i \right)
 *      \right]
 *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i} \left[
 *          g_{ij}\left( k_i, k_j \right)
 *          - \varphi_{i j}\left( k_i \right)
 *          - \varphi_{j i}\left( k_j \right)
 *      \right]
 *  = E\left( k \right),
 *  \quad \forall \varphi: I^2 \times K \rightarrow \mathbb{R},
 * \f]
 *
 * On the other hand,
 * the lower boundary of the reparametrized energy may change
 *
 * \f[
 *  \widetilde{E}\left( \varphi \right)
 *      = \sum\limits_{i \in I} \min_{d \in D}{\left[
 *          q_i\left( d \right)
 *          + \sum\limits_{j \in \mathcal{N}_i}
 *              \varphi_{i j}\left( d \right)
 *      \right]}
 *      + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *          \min_{d, d' \in D}{\left[
 *              g_{ij}\left( d, d' \right)
 *              - \varphi_{i j}\left( d \right)
 *              - \varphi_{j i}\left( d' \right)
 *          \right]}
 *  \neq \widetilde{E}.
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
 *      q_i\left( d \right)
 *      + \sum\limits_{j \in \mathcal{N}_i}
 *          \varphi_{i j}\left( d \right)
 *  \right]}
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \min_{d, d' \in D}{\left[
 *          g_{ij}\left( d, d' \right)
 *          - \varphi_{i j}\left( d \right)
 *          - \varphi_{j i}\left( d' \right)
 *      \right]}
 *  \to \max_{\varphi: I^2 \times K \rightarrow \mathbb{R}}.
 * \f]
 *
 * Introducing reparametrized vertex penalty
 *
 * \f[
 *  q_i\left( d; \varphi \right)
 *  = q_i\left( d \right)
 *    + \sum\limits_{j \in \mathcal{N}_i}
 *        \varphi_{i j}\left( d \right)
 * \f]
 *
 * and reparametrized edge penalty
 *
 * \f[
 *  g_{ij}\left( d, d'; \varphi \right)
 *  = g_{ij}\left( d, d' \right)
 *    - \varphi_{i j}\left( d \right)
 *    - \varphi_{j i}\left( d' \right),
 * \f]
 *
 * now we need to find such \f$\varphi\f$ that
 *
 * \f[
 *  \sum\limits_{i \in I} \min\limits_{d \in D}{q_i\left( d; \varphi \right)}
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \min\limits_{\left\langle d, d' \right\rangle \in D^2}
 *          g_{ij}\left( d, d'; \varphi \right)
 *  \to \max\limits_{\varphi: I^2 \times K \rightarrow \mathbb{R}}.
 * \f]
 *
 * You can find more information in the following sources
 * - <a href="
 *    https://hci.iwr.uni-heidelberg.de/vislearn/HTML/people/
 *bogdan/publications/papers/Dense-CombiLP-Haller-AAAI2017.pdf
 *   ">
 *   Exact MAP-inference by Confining Combinatorial Search
 *   with LP Relaxation
 *   </a> (2017) by Stefan Haller, Paul Swoboda and Bogdan Savchynskyy;
 * - <a href="
 *    http://cmp.felk.cvut.cz/~shekhovt/publications/as-phd-thesis-TR.pdf
 *   ">
 *   Exact and Partial Energy Minimization in Computer Vision
 *   </a> (2013) by Alexander Shekhovtsov.
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
     * \brief Number of disparity levels.
     *
     * Number of disparity levels is more generic value
     * than maximal disparity level.
     * This measure may be applied in the case
     * when we use pyramidal disparity map search
     * and thus have initial values for different pixels.
     * In this situation,
     * we can set the disparity map we've got
     * as the middle of disparity scale of a pixel,
     * and find finer disparity map
     * using DisparityGraph::disparity_levels disparity values.
     *
     * For now,
     * we assume the minimal disparity for each pixel to be equal `0`,
     * so the DisparityGraph::disparity_levels is used simply as
     * \f$\max{D} - 1\f$ in set of available disparities
     * \f$D = \left\{ 0, 1, \dots, \max{D} - 1 \right\}\f$.
     */
    ULONG disparity_levels;
    /**
     * \brief Reparametrization is a helpful vector
     * for the optimization problem.
     *
     * It assigns a floating point value
     * to each pair of Node and neighboring Pixel instances
     *
     * \f[
     *  \varphi: I^2 \times K \rightarrow \mathbb{R}
     * \f]
     *
     * It's infeasible to use a tree for such purpose.
     * Also, it may be slow to use an k-D array.
     *
     * The DisparityGraph::reparametrization is a 1D array,
     * with specific indexing rules:
     * it uses a generalization of row/column-major order.
     *
     * Used "dimensions" are following (upper ones are more nested):
     *
     * - Pixel::y of Node::pixel,
     * - Node::disparity,
     * - Index of current neighbor of Node instance,
     * - Pixel::x of Node::pixel
     *
     * Index of an element of the DisparityGraph::reparametrization
     * for specific Node and neighbor index
     * can be calculated by formula
     *
     * \f[
     *  k\left( \left\langle x, y \right\rangle, i, d \right) =
     *      y + h \cdot \left(
     *          d + \left| D \right| \cdot \left(
     *              i + \max_j{\left| \mathcal{N}_j \right|} \cdot x \cdot w
     *              \right)
     *          \right),
     * \f]
     *
     * where \f$\left\langle x, y \right\rangle\f$
     * are coordinates of Node::pixel,
     * \f$i\f$ is an index of used neighbor
     * and \f$d\f$ is a Node::disparity.
     */
    FLOAT_ARRAY reparametrization;
    /**
     * \brief Weight of difference in colors
     * between pixel and its neighbor.
     * Noted ad \f$\alpha\f$ in problem description.
     *
     * Heigher value means that the image is clean
     * and you trust its color information
     * in a sense that one vertex of displayed object
     * has the same color from both images.
     * The weight is opposite to DisparityGraph::smoothness.
     */
    FLOAT cleanness;
    /**
     * \brief Weight of difference between disparities of neighboring nodes.
     * Noted ad \f$\beta\f$ in problem description.
     *
     * Heigher value means that the surface you observe
     * tends to be smooth rather than sharp.
     * The weight is opposite to DisparityGraph::cleanness.
     */
    FLOAT smoothness;
    /**
     * \brief Create DisparityGraph entity
     * and initialize its DisparityGraph::reparametrization.
     */
    DisparityGraph(
        struct Image left,
        struct Image right,
        ULONG disparity_levels,
        FLOAT cleanness,
        FLOAT smoothness
    );
};

/**
 * \brief Node of DisparityGraph is a pair
 * consisting of Pixel and disparity assigned to it.
 */
struct Node
{
    /**
     * \brief Pixel to which the Node instance belongs.
     */
    struct Pixel pixel;
    /**
     * \brief Disparity assigned to the Node::pixel in the Node instance.
     *
     * Disparity specifies the difference between columns
     * of current pixel on the right image
     * and corresponding one on the left image.
     */
    ULONG disparity;
};

/**
 * \brief Edge is an ordered pair of two Node instances.
 */
struct Edge
{
    /**
     * \brief Start Node of the directed Edge.
     */
    struct Node node;
    /**
     * \brief Start Node of the directed Edge.
     */
    struct Node neighbor;
};

/**
 * \brief Get index of a neighbor for fast access in different data arrays.
 *
 * Pixel is a more complex structure than a number.
 * Tree is a slow structure for our purposes.
 *
 * We can store information associated with pixel and its neighbors
 * in an array,
 * but this needs a calculation of corresponding indices for data access.
 * This function provides these indices.
 *
 * @return
 *  Integer from `[0; ::NEIGHBORS_COUNT - 1]` range
 *  if there should be at least one Edge between two provided pixels.
 *  ::NEIGHBORS_COUNT if the `neighbor` is not a neighbor of the pixel.
 */
ULONG neighbor_index(
    struct Pixel pixel,
    struct Pixel neighbor
);

/**
 * \brief Check wheter provided Pixel instances
 * in given DisparityGraph are neighbors
 * and exist at all.
 *
 * As it's explained in ::NEIGHBORS_COUNT,
 * there are four possible neighbors.
 *
 * Logic says that
 * top pixels don't have top neighbors,
 * bottom pixels don't have bottom neighbors,
 * right pixels don't have right neighbors
 * and left pixels don't have left pixels.
 *
 * Also, pixels that are not the nearest straight neighbors,
 * are not a neighbors in the DisparityGraph.
 */
bool neighborhood_exists(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
);

/**
 * \brief Check existence of provided Pixel instances in given DisparityGraph.
 *
 * See ::neighborhood_exists for more information.
 *
 * This function differs from ::neighborhood_exists
 * just by the second parameter:
 * it takes a neighbor index
 * calculated by ::neighbor_index function.
 */
bool neighborhood_exists_fast(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
);

/**
 * \brief Check existence of provided Node.
 *
 * Node exists if position of its pixel doesn't overflow
 * DisparityGraph::right image,
 * corresponding pixel (calculated by the disparity)
 * doesn't overflow the DisparityGraph::left image,
 * and its disparity doesn't overflow
 * the maximal allowed.
 */
bool node_exists(
    const struct DisparityGraph& graph,
    struct Node node
);

/**
 * \brief Check existence of provided Edge in given DisparityGraph.
 *
 * Similar to ::neighborhood_exists,
 * but also checks constraints imposed on disparities
 * of the neighboring pixels.
 */
bool edge_exists(
    const struct DisparityGraph& graph,
    struct Edge edge
);

/**
 * \brief Get an index of DisparityGraph::reparametrization element
 * using a Node and an index of its neighbor.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
ULONG reparametrization_index_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
);

/**
 * \brief Get an index of DisparityGraph::reparametrization element
 * using a Node and a Pixel.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
ULONG reparametrization_index(
    const struct DisparityGraph& graph,
    struct Node node,
    struct Pixel neighbor
);

/**
 * \brief Get an index of DisparityGraph::reparametrization element
 * using an Edge.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
ULONG reparametrization_index_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
);

/**
 * \brief Get a value of DisparityGraph::reparametrization element
 * using a Node and an index of its neighbor.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
FLOAT reparametrization_value_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
);

/**
 * \brief Get a value of DisparityGraph::reparametrization element
 * using a Node and a Pixel.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
FLOAT reparametrization_value(
    const struct DisparityGraph& graph,
    struct Node node,
    struct Pixel neighbor
);

/**
 * \brief Get a value of DisparityGraph::reparametrization element
 * using an Edge.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using ::neighborhood_exists.
 */
FLOAT reparametrization_value_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
);

/**
 * \brief Calculate penalty of Edge without neighborhood check.
 *
 * You should use ::edge_exists function
 * to check that the Edge actually exists.
 * If it doesn't, the penalty is assumed to be \f$\infty\f$
 * (but this function doesn't check existence).
 *
 * Otherwise, the penalty is a norm of a difference
 * between disparities of Node instances that the Edge connects.
 */
FLOAT edge_penalty(const struct DisparityGraph& graph, struct Edge edge);
/**
 * \brief Calculate penalty of Node.
 *
 * You should use ::node_exists function
 * to check that the Node actually exists.
 * If it doesn't, the penalty is assumed to be \f$\infty\f$
 * (but this function doesn't check existence).
 *
 * Otherwise, the penalty is a norm of a difference
 * between disparities of Node instances that the Edge connects.
 */
FLOAT node_penalty(const struct DisparityGraph& graph, struct Node node);

#endif
