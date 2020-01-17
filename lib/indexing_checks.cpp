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
#include <indexing.hpp>
#include <indexing_checks.hpp>

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
namespace sp
{
namespace indexing
{
namespace checks
{

using sp::types::FLOAT;
#endif

__device__ BOOL neighborhood_exists(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    return neighborhood_exists_fast(
        graph,
        pixel,
        neighbor_index(pixel, neighbor)
    );
}

__device__ BOOL neighborhood_exists_fast(
    const struct DisparityGraph* graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    return !(neighbor_index > 3
        || pixel.y >= graph->right.height
        || pixel.x >= graph->right.width
        || (pixel.x + 1 == graph->left.width && neighbor_index == 0)
        || (pixel.x == 0 && neighbor_index == 1)
        || (pixel.y + 1 == graph->left.height && neighbor_index == 2)
        || (pixel.y == 0 && neighbor_index == 3)
    );
}

__device__ BOOL node_exists(
    const struct DisparityGraph* graph,
    struct Node node
)
{
    return !(
        node.disparity >= graph->disparity_levels
        || node.pixel.y >= graph->right.height
        || node.pixel.x >= graph->right.width
        || node.pixel.x + node.disparity >= graph->left.width
    );
}

__device__ BOOL edge_exists(
    const struct DisparityGraph* graph,
    struct Edge edge
)
{
    if (!node_exists(graph, edge.node))
    {
        return false;
    }
    if (!node_exists(graph, edge.neighbor))
    {
        return false;
    }
    if (!neighborhood_exists(graph, edge.node.pixel, edge.neighbor.pixel))
    {
        return false;
    }

    if (edge.node.pixel.y != edge.neighbor.pixel.y)
    {
        return true;
    }

    if (edge.node.pixel.x + 1 == edge.neighbor.pixel.x
        && edge.node.disparity > edge.neighbor.disparity + 1)
    {
        return false;
    }
    if (edge.node.pixel.x == edge.neighbor.pixel.x + 1
        && edge.node.disparity + 1 < edge.neighbor.disparity)
    {
        return false;
    }

    return true;
}

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
}
}
}
#endif
