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
#include <lowest_penalties.hpp>

#include <disparity_graph.hpp>
#include <image.hpp>

ULONG pixel_index(
    const struct DisparityGraph& graph,
    struct Pixel pixel
)
{
    return pixel.y + graph.right.height * pixel.x;
}

ULONG neighborhood_index_fast(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    return neighbor_index
        + NEIGHBORS_COUNT * (pixel.y + graph.right.height * pixel.x);
}

ULONG neighborhood_index(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    return neighborhood_index_fast(
        graph,
        pixel,
        neighbor_index(pixel, neighbor)
    );
}

ULONG neighborhood_index_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
)
{
    return neighborhood_index_fast(
        graph,
        edge.node.pixel,
        neighbor_index(edge.node.pixel, edge.neighbor.pixel)
    );
}

FLOAT calculate_lowest_pixel_penalty(
    const struct DisparityGraph& graph,
    struct Pixel pixel
)
{
    Node node{pixel, 0};
    FLOAT minimal_penalty = node_penalty(graph, node);
    for (
        node.disparity = 0;
        node.pixel.x + node.disparity < graph.left.width
            && node.disparity < graph.disparity_levels;
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
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    return calculate_lowest_neighborhood_penalty_fast(
        graph,
        {{pixel, 0}, {neighbor, 0}}
    );
}

FLOAT calculate_lowest_neighborhood_penalty_fast(
    const struct DisparityGraph& graph,
    struct Edge edge
)
{
    FLOAT minimal_penalty = edge_penalty(graph, edge);
    for (
        edge.node.disparity = 0;
        edge.node.pixel.x + edge.node.disparity < graph.left.width
            && edge.node.disparity < graph.disparity_levels;
        ++edge.node.disparity
    )
    {
        ULONG initial_disparity = edge.node.disparity <= 1
            ? 0
            : edge.node.disparity - 1;
        for (
            edge.neighbor.disparity = initial_disparity;
            edge.neighbor.pixel.x + edge.neighbor.disparity < graph.left.width
                && edge.neighbor.disparity < graph.disparity_levels;
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
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    struct Pixel neighbor{pixel};
    if (neighbor_index == 0)
    {
        ++neighbor.x;
    }
    else if (neighbor_index == 1)
    {
        --neighbor.x;
    }
    else if (neighbor_index == 2)
    {
        ++neighbor.y;
    }
    else if (neighbor_index == 3)
    {
        --neighbor.y;
    }
    return calculate_lowest_neighborhood_penalty_fast(
        graph,
        {{pixel, 0}, {neighbor, 0}}
    );
}

FLOAT lowest_pixel_penalty(
    const struct LowestPenalties& penalties,
    struct Pixel pixel
)
{
    return penalties.pixels[pixel_index(penalties.disparity_graph, pixel)];
}

FLOAT lowest_neighborhood_penalty_fast(
    const struct LowestPenalties& penalties,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    return penalties.neighborhoods[
        neighborhood_index(penalties.disparity_graph, pixel, neighbor)
    ];
}

FLOAT lowest_neighborhood_penalty(
    const struct LowestPenalties& penalties,
    struct Edge edge
)
{
    return penalties.neighborhoods[
        neighborhood_index_slow(penalties.disparity_graph, edge)
    ];
}

struct LowestPenalties disparity2lowest(struct DisparityGraph& graph)
{
    FLOAT_ARRAY pixels(graph.right.height * graph.right.width);
    FLOAT_ARRAY neighborhoods(
        NEIGHBORS_COUNT * graph.right.height * graph.right.width
    );
    fill(pixels.begin(), pixels.end(), 0);
    fill(neighborhoods.begin(), neighborhoods.end(), 0);
    struct Pixel pixel{0, 0};
    for (pixel.x = 0; pixel.x < graph.right.width; ++pixel.x)
    {
        for (pixel.y = 0; pixel.y < graph.right.height; ++pixel.y)
        {
            pixels[pixel_index(graph, pixel)] = calculate_lowest_pixel_penalty(
                graph,
                pixel
            );
            for (
                ULONG neighbor_index = 0;
                neighbor_index < NEIGHBORS_COUNT;
                ++neighbor_index
            )
            {
                if (!neighborhood_exists_fast(graph, pixel, neighbor_index))
                {
                    continue;
                }
                neighborhoods[
                    neighborhood_index_fast(graph, pixel, neighbor_index)
                ] = calculate_lowest_neighborhood_penalty_slow(
                    graph,
                    pixel,
                    neighbor_index
                );
            }
        }
    }
    return {graph, pixels, neighborhoods};
}
