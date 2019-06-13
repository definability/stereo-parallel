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
#include <indexing_checks.hpp>
#include <lowest_penalties.hpp>

namespace sp::graph::lowest_penalties
{

using sp::graph::disparity::NEIGHBORS_COUNT;
using sp::indexing::checks::neighborhood_exists_fast;
using sp::indexing::neighbor_by_index;
using sp::indexing::neighborhood_index;
using sp::indexing::neighborhood_index_fast;
using sp::indexing::neighborhood_index_slow;
using sp::types::Node;

LowestPenalties::LowestPenalties(const struct DisparityGraph* graph)
    : graph{graph}
    , pixels(graph->right.height * graph->right.width)
    , neighborhoods(
        NEIGHBORS_COUNT
        * graph->right.height
        * graph->right.width
    )
{
    fill(this->pixels.begin(), this->pixels.end(), 0);
    fill(this->neighborhoods.begin(), this->neighborhoods.end(), 0);
    struct Pixel pixel{0, 0};
    for (pixel.x = 0; pixel.x < this->graph->right.width; ++pixel.x)
    {
        for (
            pixel.y = 0;
            pixel.y < this->graph->right.height;
            ++pixel.y)
        {
            this->pixels[
                pixel_index(&(this->graph->right), pixel)
            ] = calculate_lowest_pixel_penalty(
                this->graph,
                pixel
            );
            for (
                ULONG neighbor_index = 0;
                neighbor_index < NEIGHBORS_COUNT;
                ++neighbor_index
            )
            {
                if (
                    !neighborhood_exists_fast(
                        this->graph, pixel, neighbor_index
                    )
                )
                {
                    continue;
                }
                this->neighborhoods[
                    neighborhood_index_fast(
                        this->graph, pixel, neighbor_index
                    )
                ] = calculate_lowest_neighborhood_penalty_slow(
                    this->graph,
                    pixel,
                    neighbor_index
                );
            }
        }
    }
}

FLOAT calculate_lowest_pixel_penalty(
    const struct DisparityGraph* graph,
    struct Pixel pixel
)
{
    struct Node node;
    node.pixel = pixel;
    node.disparity = 0;
    FLOAT minimal_penalty = node_penalty(graph, node);
    for (
        node.disparity = 0;
        node.pixel.x + node.disparity < graph->left.width
            && node.disparity < graph->disparity_levels;
        ++node.disparity
    )
    {
        minimal_penalty = MIN(
            node_penalty(graph, node),
            minimal_penalty
        );
    }
    return minimal_penalty;
}

FLOAT calculate_lowest_neighborhood_penalty(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    struct Edge edge;

    edge.node.pixel = pixel;
    edge.node.disparity = 0;

    edge.neighbor.pixel = neighbor;
    edge.neighbor.disparity = 0;

    return calculate_lowest_neighborhood_penalty_fast(
        graph,
        edge
    );
}

FLOAT calculate_lowest_neighborhood_penalty_fast(
    const struct DisparityGraph* graph,
    struct Edge edge
)
{
    FLOAT minimal_penalty = edge_penalty(graph, edge);
    ULONG initial_disparity = 0;
    for (
        edge.node.disparity = 0;
        edge.node.pixel.x + edge.node.disparity < graph->left.width
            && edge.node.disparity < graph->disparity_levels;
        ++edge.node.disparity
    )
    {
        if (
            edge.node.disparity <= 1
            || edge.neighbor.pixel.x == edge.node.pixel.x
        )
        {
            initial_disparity = 0;
        }
        else
        {
            initial_disparity = edge.node.disparity - 1;
        }
        for (
            edge.neighbor.disparity = initial_disparity;
            edge.neighbor.pixel.x + edge.neighbor.disparity < graph->left.width
                && edge.neighbor.disparity < graph->disparity_levels;
            ++edge.neighbor.disparity
        )
        {
            minimal_penalty = MIN(
                edge_penalty(graph, edge),
                minimal_penalty
            );
        }
    }
    return minimal_penalty;
}

FLOAT calculate_lowest_neighborhood_penalty_slow(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    struct Edge edge;

    edge.node.pixel = pixel;
    edge.node.disparity = 0;

    edge.neighbor.pixel = neighbor_by_index(pixel, neighbor_index);
    edge.neighbor.disparity = 0;

    return calculate_lowest_neighborhood_penalty_fast(
        graph,
        edge
    );
}

FLOAT lowest_pixel_penalty(
    const struct LowestPenalties* penalties,
    struct Pixel pixel
)
{
    return penalties->pixels[pixel_index(&(penalties->graph->right), pixel)];
}

FLOAT lowest_neighborhood_penalty_fast(
    const struct LowestPenalties* penalties,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    return penalties->neighborhoods[
        neighborhood_index(penalties->graph, pixel, neighbor)
    ];
}

FLOAT lowest_neighborhood_penalty(
    const struct LowestPenalties* penalties,
    struct Edge edge
)
{
    return penalties->neighborhoods[
        neighborhood_index_slow(penalties->graph, edge)
    ];
}

}
