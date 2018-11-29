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
#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <image.hpp>

#include <algorithm>
#include <vector>

using FLOAT = double;
using FLOAT_ARRAY = std::vector<FLOAT>;

const ULONG NEIGHBORS_COUNT = 4;

struct DisparityGraph
{
    struct Image left;
    struct Image right;
    ULONG maximal_disparity;
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
