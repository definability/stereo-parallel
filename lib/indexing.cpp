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
#include <disparity_graph.hpp>
#include <indexing.hpp>

namespace sp::indexing
{

using sp::graph::disparity::NEIGHBORS_COUNT;

ULONG pixel_index(const struct Image* image, struct Pixel pixel)
{
    return image->width * pixel.y + pixel.x;
}

ULONG pixel_value(const struct Image* image, struct Pixel pixel)
{
    return image->data[pixel_index(image, pixel)];
}

ULONG neighbor_index(
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    if (pixel.x != neighbor.x && pixel.y != neighbor.y)
    {
        return NEIGHBORS_COUNT;
    }
    if (pixel.x + 1 == neighbor.x)
    {
        return 0;
    }
    if (pixel.x == neighbor.x + 1)
    {
        return 1;
    }
    if (pixel.y + 1 == neighbor.y)
    {
        return 2;
    }
    if (pixel.y == neighbor.y + 1)
    {
        return 3;
    }
    return NEIGHBORS_COUNT;
}

ULONG reparametrization_index_fast(
    const struct DisparityGraph* graph,
    struct Node node,
    ULONG neighbor_index
)
{
    ULONG index = 0;
    index *= graph->right.width;
    index += node.pixel.x;
    index *= NEIGHBORS_COUNT;
    index += neighbor_index;
    index *= graph->disparity_levels;
    index += node.disparity;
    index *= graph->left.height;
    index += node.pixel.y;

    return index;
}

ULONG reparametrization_index(
    const struct DisparityGraph* graph,
    struct Node node,
    struct Pixel neighbor
)
{
    return reparametrization_index_fast(
        graph,
        node,
        neighbor_index(node.pixel, neighbor)
    );
}

ULONG reparametrization_index_slow(
    const struct DisparityGraph* graph,
    struct Edge edge
)
{
    return reparametrization_index_fast(
        graph,
        edge.node,
        neighbor_index(edge.node.pixel, edge.neighbor.pixel)
    );
}

FLOAT reparametrization_value(
    const struct DisparityGraph* graph,
    struct Node node,
    struct Pixel neighbor
)
{
    return graph->reparametrization[reparametrization_index(graph, node, neighbor)];
}

FLOAT reparametrization_value_slow(
    const struct DisparityGraph* graph,
    struct Edge edge
)
{
    return graph->reparametrization[reparametrization_index_slow(graph, edge)];
}

FLOAT reparametrization_value_fast(
    const struct DisparityGraph* graph,
    struct Node node,
    ULONG neighbor_index
)
{
    return graph->reparametrization[
        reparametrization_index_fast(graph, node, neighbor_index)
    ];
}

ULONG node_index(const struct DisparityGraph* graph, struct Node node)
{
    return node.disparity + graph->disparity_levels
        * (node.pixel.y + graph->right.height * node.pixel.x);
}

ULONG neighborhood_index_fast(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    return neighbor_index
        + NEIGHBORS_COUNT * (pixel.y + graph->right.height * pixel.x);
}

ULONG neighborhood_index(
    const struct DisparityGraph* graph,
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
    const struct DisparityGraph* graph,
    struct Edge edge
)
{
    return neighborhood_index_fast(
        graph,
        edge.node.pixel,
        neighbor_index(edge.node.pixel, edge.neighbor.pixel)
    );
}

struct Pixel neighbor_by_index(
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    if (neighbor_index == 0)
    {
        ++pixel.x;
    }
    else if (neighbor_index == 1)
    {
        --pixel.x;
    }
    else if (neighbor_index == 2)
    {
        ++pixel.y;
    }
    else if (neighbor_index == 3)
    {
        --pixel.y;
    }
    return pixel;
}

}
