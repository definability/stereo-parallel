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
#ifndef LOWEST_PENALTIES_HPP
#define LOWEST_PENALTIES_HPP

/**
 * \brief Choose minimal element between two provided.
 */
#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

#include <disparity_graph.hpp>
#include <indexing.hpp>
#include <types.hpp>

/**
 * \brief Utilities to find locally best nodes and edges.
 */
namespace sp::graph::lowest_penalties
{

using sp::graph::disparity::DisparityGraph;
using sp::indexing::pixel_index;
using sp::types::Edge;
using sp::types::FLOAT;
using sp::types::FLOAT_ARRAY;
using sp::types::Pixel;
using sp::types::ULONG;

/**
 * \brief Graph containing lowest penalties for
 * pixels and neighborhoods.
 *
 * ConstraintGraph uses minimal penalties
 * to check availability of nodes and edges.
 *
 * Corresponding element of LowestPenalties::pixels array
 * contains minimal value of nodes of observed pixel.
 * It can be fetched by ::lowest_pixel_penalty and contains
 * \f$\min\limits_{\delta \in D}{q_i\left( \delta; \varphi \right)}\f$,
 * where \f$i\f$ is an index of the pixel and can be got from ::pixel_index.
 *
 * Corresponding element of LowestPenalties::neighborhoods array
 * contains minimal value of edges of observed neighborhood
 * (pair of neighboring pixels).
 * It can be fetched by ::lowest_neighborhood_penalty or
 * ::lowest_neighborhood_penalty_fast (depending on what you can provide)
 * and contains
 * \f$\min\limits_{\left\langle \delta, \delta' \right\rangle \in D^2}
 *                {g_{ij}\left( \delta, \delta'; \varphi \right)}\f$,
 * where \f$i\f$ is an index of needed pixel and can be got from ::pixel_index,
 * and \f$j \in \mathcal{N}_i\f$ is an index of the neighbor.
 */
struct LowestPenalties
{
    const struct DisparityGraph& graph;
    /**
     * \brief Minimal penalties of nodes per pixels.
     *
     * Column-major order is used, so the index is calculated by the formula
     *
     * \f[
     *  k\left( \left\langle x, y \right\rangle \right) = y + h \cdot x.
     * \f]
     */
    FLOAT_ARRAY pixels;
    /**
     * \brief Minimal penalties of edges per neighborhood.
     *
     * Formula for index calculation is
     *
     * \f[
     *  k\left( \left\langle x, y \right\rangle, i \right)
     *  = i + \max\limits_j{\left| \mathcal{N}_j \right|}
     *      \cdot \left( y + h \cdot x \right),
     * \f]
     *
     * where \f$i\f$ is an index of the neighbor,
     * and \f$\max\limits_j{\left| \mathcal{N}_j \right|}\f$ is ::NEIGHBORS_COUNT.
     */
    FLOAT_ARRAY neighborhoods;
    /**
     * \brief Calculate lowest penalties from DisparityGraph.
     *
     * Uses ::calculate_lowest_pixel_penalty
     * and ::calculate_lowest_neighborhood_penalty_slow
     * to fetch minimal penalties from provided DisparityGraph
     * and save them to LowestPenalties::pixels
     * and LowestPenalties::neighborhoods.
     *
     * Note that if you will change provided DisparityGraph instance
     * after LowestPenalties construction,
     * the LowestPenalties instance will not be changed.
     */
    explicit LowestPenalties(const struct DisparityGraph& graph);
};
/**
 * \brief Calculate minimal penalty among nodes of a pixel.
 */
FLOAT calculate_lowest_pixel_penalty(
    const struct DisparityGraph& graph,
    struct Pixel pixel
);
/**
 * \brief Calculate minimal penalty among edges of a neighborhood.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists to make sure that you use it right.
 */
FLOAT calculate_lowest_neighborhood_penalty(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
);
/**
 * \brief Calculate minimal penalty among edges of a neighborhood.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists_fast to make sure that you use it right.
 */
FLOAT calculate_lowest_neighborhood_penalty_fast(
    const struct DisparityGraph& graph,
    struct Edge edge
);
/**
 * \brief Calculate minimal penalty among edges of a neighborhood.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use ::edge_exists to make sure that you use it right.
 */
FLOAT calculate_lowest_neighborhood_penalty_slow(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
);
/**
 * \brief Get precalculated minimal penalty among edges of a neighborhood
 * from LowestPenalties::pixels.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists to make sure that you use it right.
 */
FLOAT lowest_pixel_penalty(
    const struct LowestPenalties& penalties,
    struct Pixel pixel
);
/**
 * \brief Get precalculated minimal penalty among edges of a neighborhood
 * from LowestPenalties::neighborhoods by neighboring pixel and its neighbor.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use sp::indexing::checks::neighborhood_exists_fast to make sure that you use it right.
 */
FLOAT lowest_neighborhood_penalty_fast(
    const struct LowestPenalties& penalties,
    struct Pixel pixel,
    struct Pixel neighbor
);
/**
 * \brief Get precalculated minimal penalty among edges of a neighborhood
 * from LowestPenalties::neighborhoods by edge of the neighborhood.
 *
 * Note that the function doesn't check existence of provided neighborhood.
 * Use ::edge_exists to make sure that you use it right.
 */
FLOAT lowest_neighborhood_penalty(
    const struct LowestPenalties& penalties,
    struct Edge edge
);

}

#endif
