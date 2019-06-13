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
#ifndef INDEXING_HPP
#define INDEXING_HPP

#include <disparity_graph.hpp>
#include <image.hpp>
#include <types.hpp>

/**
 * \brief Functions for getting access to arrays
 * by abstract indices like sp::types::Pixels, sp::types::Nodes, etc.
 */
namespace sp::indexing
{

using sp::graph::disparity::DisparityGraph;
using sp::image::Image;
using sp::types::Edge;
using sp::types::FLOAT;
using sp::types::Node;
using sp::types::Pixel;
using sp::types::ULONG;

/**
 * \brief Get position of the pixel in data array of the image.
 *
 * Despite instensities of pixels are stored in 1D array Image::data,
 * it's convenient to access them using their coordinates.
 * That's why the function for coordinates' conversion is needed.
 *
 * Result of accessing non-existent value
 * depends on sp::types::ULONG_ARRAY.
 */
ULONG pixel_index(const struct Image* image, struct Pixel pixel);

/**
 * \brief Get intensity of the pixel in the image.
 *
 * Simply call sp::indexing::pixel_index and take a value with returned index
 * from Image::data.
 */
ULONG pixel_value(const struct Image* image, struct Pixel pixel);

/**
 * \brief Get an index of sp::graph::disparity::DisparityGraph::reparametrization element
 * using a Node and an index of its neighbor.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
ULONG reparametrization_index_fast(
    const struct DisparityGraph* graph,
    struct Node node,
    ULONG neighbor_index
);

/**
 * \brief Get an index of sp::graph::disparity::DisparityGraph::reparametrization element
 * using a Node and a Pixel.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
ULONG reparametrization_index(
    const struct DisparityGraph* graph,
    struct Node node,
    struct Pixel neighbor
);

/**
 * \brief Get an index of sp::graph::disparity::DisparityGraph::reparametrization element
 * using an Edge.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
ULONG reparametrization_index_slow(
    const struct DisparityGraph* graph,
    struct Edge edge
);

/**
 * \brief Get a value of sp::graph::disparity::DisparityGraph::reparametrization element
 * using a Node and an index of its neighbor.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
FLOAT reparametrization_value_fast(
    const struct DisparityGraph* graph,
    struct Node node,
    ULONG neighbor_index
);

/**
 * \brief Get a value of sp::graph::disparity::DisparityGraph::reparametrization element
 * using a Node and a Pixel.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
FLOAT reparametrization_value(
    const struct DisparityGraph* graph,
    struct Node node,
    struct Pixel neighbor
);

/**
 * \brief Get a value of sp::graph::disparity::DisparityGraph::reparametrization element
 * using an Edge.
 *
 * The function doesn't check existence of neighbor.
 * You should perform it by yourself
 * using sp::indexing::checks::neighborhood_exists.
 */
FLOAT reparametrization_value_slow(
    const struct DisparityGraph* graph,
    struct Edge edge
);

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
 *  Integer from `[0; sp::graph::disparity::NEIGHBORS_COUNT - 1]` range
 *  if there should be at least one Edge between two provided pixels.
 *  sp::graph::disparity::NEIGHBORS_COUNT if the `neighbor` is not a neighbor of the pixel.
 */
ULONG neighbor_index(
    struct Pixel pixel,
    struct Pixel neighbor
);

/**
 * \brief Get an index of sp::graph::constraint::ConstraintGraph::nodes_availability element
 * using a Node.
 *
 * The function doesn't check existence of the Node.
 * You should perform it by yourself
 * using sp::indexing::checks::node_exists.
 */
ULONG node_index(const struct DisparityGraph* graph, struct Node node);

/**
 * \brief Get index of a neighborhood in
 * sp::graph::lowest_penalties::LowestPenalties::neighborhoods.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists_fast to make sure that you use it right.
 */
ULONG neighborhood_index_fast(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    ULONG neighbor_index
);

/**
 * \brief Get index of a neighborhood given two neighboring pixels.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists to make sure that you use it right.
 */
ULONG neighborhood_index(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    struct Pixel neighbor
);

/**
 * \brief Get index of a neighborhood by arbitrary edge from the neighborhood.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::edge_exists to make sure that you use it right.
 */
ULONG neighborhood_index_slow(
    const struct DisparityGraph* graph,
    struct Edge edge
);

/**
 * \brief Get neighbor pixel to current one using neighbor index.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists to make sure that you use it right.
 */
struct Pixel neighbor_by_index(
    struct Pixel pixel,
    ULONG neighbor_index
);

}

#endif
