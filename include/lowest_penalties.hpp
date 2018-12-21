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
#ifndef LOWEST_PENALTIES_HPP
#define LOWEST_PENALTIES_HPP

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

#include <disparity_graph.hpp>
#include <image.hpp>

struct LowestPenalties
{
    const struct DisparityGraph& graph;
    FLOAT_ARRAY pixels;
    FLOAT_ARRAY neighborhoods;
    explicit LowestPenalties(const struct DisparityGraph& graph);
};
ULONG pixel_index(
    const struct DisparityGraph& graph,
    struct Pixel pixel
);
ULONG neighborhood_index_fast(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
);
ULONG neighborhood_index(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
);
ULONG neighborhood_index_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
);
FLOAT calculate_lowest_pixel_penalty(
    const struct DisparityGraph& graph,
    struct Pixel pixel
);
FLOAT calculate_lowest_neighborhood_penalty(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
);
FLOAT calculate_lowest_neighborhood_penalty_fast(
    const struct DisparityGraph& graph,
    struct Edge edge
);
FLOAT calculate_lowest_neighborhood_penalty_slow(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
);
FLOAT lowest_pixel_penalty(
    const struct LowestPenalties& penalties,
    struct Pixel pixel
);
FLOAT lowest_neighborhood_penalty_fast(
    const struct LowestPenalties& penalties,
    struct Pixel pixel,
    struct Pixel neighbor
);
FLOAT lowest_neighborhood_penalty(
    const struct LowestPenalties& penalties,
    struct Edge edge
);

#endif
