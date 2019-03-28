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
#ifndef INDEXING_CHECKS_HPP
#define INDEXING_CHECKS_HPP

#include <disparity_graph.hpp>

/**
 * \brief Checks for availability of indices.
 */
namespace sp::indexing::checks
{

using sp::graph::disparity::DisparityGraph;
using sp::types::Edge;
using sp::types::FLOAT;
using sp::types::Node;
using sp::types::Pixel;
using sp::types::ULONG;

/**
 * \brief Check wheter provided Pixel instances
 * in given DisparityGraph are neighbors
 * and exist at all.
 *
 * As it's explained in sp::graph::disparity::NEIGHBORS_COUNT,
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
 * See sp::indexing::checks::neighborhood_exists for more information.
 *
 * This function differs from sp::indexing::checks::neighborhood_exists
 * just by the second parameter:
 * it takes a neighbor index
 * calculated by sp::indexing::neighbor_index function.
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
 * Similar to sp::indexing::checks::neighborhood_exists,
 * but also checks constraints imposed on disparities
 * of the neighboring pixels.
 */
bool edge_exists(
    const struct DisparityGraph& graph,
    struct Edge edge
);

}

#endif
