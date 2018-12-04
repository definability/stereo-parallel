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
 * and positive \f$p \in \mathbb{R}\f$
 *
 * \f[
 *  E\left( k \right) =
 *  \sum\limits_{i \in I} \left\|
 *      R\left( i^x, i^y \right)
 *      - L\left( i^x + k_i, i^y \right)
 *  \right\|^p
 *  + \sum\limits_{i \in I} \sum\limits_{j \in N_i}
 *      \left\| k_i - k_j \right\|^p
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
    DisparityGraph(
        struct Image left,
        struct Image right,
        ULONG maximal_disparity
    );
};

/**
 * \brief Node of DisparityGraph is a pair
 * consisting of Pixel and disparity assigned to it.
 */
struct Node
{
    struct Pixel pixel;
    ULONG disparity;
};

/**
 * \brief Edge is a pair of two Node instances.
 */
struct Edge
{
    struct Node node;
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
 * \brief Check existence of provided Pixel instances in given DisparityGraph.
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
 * Node exists if position of its pixel doesn't overflow its image
 * and corresponding pixel (calculated by the disparity)
 * doesn't overflow another image.
 */
bool node_exists(
    ULONG maximal_disparity,
    const struct Image& image,
    const struct Image& another_image,
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
