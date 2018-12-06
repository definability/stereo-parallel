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
#include <disparity_graph.hpp>

#include <stdexcept>
#include <string>

#define SQR(x) ((x) * (x))
#define TO_FLOAT(x) (static_cast<FLOAT>(x))

DisparityGraph::DisparityGraph(
    struct Image left,
    struct Image right,
    ULONG maximal_disparity
)
    : left{std::move(left)}
    , right{std::move(right)}
    , maximal_disparity{maximal_disparity}
{
    if (!image_valid(this->left))
    {
        throw std::invalid_argument("Left image is invalid.");
    }
    if (!image_valid(this->right))
    {
        throw std::invalid_argument("Right image is invalid.");
    }
    if (this->maximal_disparity == 0)
    {
        throw std::invalid_argument(
            "Maximal disparity should be greater than zero."
        );
    }
    if (this->maximal_disparity >= this->left.width)
    {
        throw std::invalid_argument(
            "Maximal disparity should be less than width of the left image. "
            "Width of the left image is "
            + std::to_string(this->left.width)
            + ". Provided maximal disparity is "
            + std::to_string(this->maximal_disparity)
            + "."
        );
    }
    if (this->left.width != this->right.width)
    {
        throw std::invalid_argument(
            "Number of columns of the images should be equal. "
            "Current left and right images have "
            + std::to_string(this->left.width)
            + " and "
            + std::to_string(this->right.width)
            + " columns respectively."
        );
    }
    if (this->left.height != this->right.height)
    {
        throw std::invalid_argument(
            "Number of rows of the images should be equal. "
            "Current left and right images have "
            + std::to_string(this->left.height)
            + " and "
            + std::to_string(this->right.height)
            + " rows respectively."
        );
    }
    if (this->left.max_value != this->right.max_value)
    {
        throw std::invalid_argument(
            "Maximal intensity of the images should be the same. "
            "Maximal intensity of provided left and right images "
            + std::to_string(this->left.max_value)
            + " and "
            + std::to_string(this->right.max_value)
            + "respectively."
        );
    }

    this->reparametrization.resize(
        this->left.width
        * this->left.height
        * NEIGHBORS_COUNT
        * this->maximal_disparity
    );
    fill(
        this->reparametrization.begin(),
        this->reparametrization.end(),
        static_cast<FLOAT>(0)
    );
}

ULONG neighbor_index(
    struct Pixel pixel,
    struct Pixel neighbor
)
{
    if (pixel.column != neighbor.column && pixel.row != neighbor.row)
    {
        return NEIGHBORS_COUNT;
    }
    if (pixel.column + 1 == neighbor.column)
    {
        return 0;
    }
    if (pixel.column == neighbor.column + 1)
    {
        return 1;
    }
    if (pixel.row + 1 == neighbor.row)
    {
        return 2;
    }
    if (pixel.row == neighbor.row + 1)
    {
        return 3;
    }
    return NEIGHBORS_COUNT;
}

bool neighborhood_exists(
    const struct DisparityGraph& graph,
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

bool neighborhood_exists_fast(
    const struct DisparityGraph& graph,
    struct Pixel pixel,
    ULONG neighbor_index
)
{
    return !(neighbor_index > 3
        || pixel.row >= graph.right.height
        || pixel.column >= graph.right.width
        || (pixel.column + 1 == graph.left.width && neighbor_index == 0)
        || (pixel.column == 0 && neighbor_index == 1)
        || (pixel.row + 1 == graph.left.height && neighbor_index == 2)
        || (pixel.row == 0 && neighbor_index == 3)
    );
}

bool node_exists(
    ULONG maximal_disparity,
    const struct Image& image,
    const struct Image& another_image,
    struct Node node
)
{
    return !(
        node.disparity > maximal_disparity
        || node.pixel.row >= image.height
        || node.pixel.column >= image.width
        || node.pixel.column + node.disparity >= another_image.width
    );
}

bool edge_exists(
    const struct DisparityGraph& graph,
    struct Edge edge
)
{
    if (!node_exists(graph.maximal_disparity, graph.left, graph.right, edge.node))
    {
        return false;
    }
    if (!node_exists(graph.maximal_disparity, graph.left, graph.right, edge.neighbor))
    {
        return false;
    }
    if (!neighborhood_exists(graph, edge.node.pixel, edge.neighbor.pixel))
    {
        return false;
    }

    if (edge.node.pixel.row != edge.neighbor.pixel.row)
    {
        return true;
    }

    if (edge.node.pixel.column + 1 == edge.neighbor.pixel.column
        && edge.node.disparity > edge.neighbor.disparity + 1)
    {
        return false;
    }
    if (edge.node.pixel.column == edge.neighbor.pixel.column + 1
        && edge.node.disparity + 1 < edge.neighbor.disparity)
    {
        return false;
    }

    return true;
}

ULONG reparametrization_index_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
)
{
    ULONG index = 0;
    index *= graph.right.width;
    index += node.pixel.column;
    index *= NEIGHBORS_COUNT;
    index += neighbor_index;
    index *= graph.maximal_disparity;
    index += node.disparity;
    index *= graph.left.height;
    index += node.pixel.row;

    return index;
}

ULONG reparametrization_index(
    const struct DisparityGraph& graph,
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
    const struct DisparityGraph& graph,
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
    const struct DisparityGraph& graph,
    struct Node node,
    struct Pixel neighbor
)
{
    return graph.reparametrization[reparametrization_index(graph, node, neighbor)];
}

FLOAT reparametrization_value_slow(
    const struct DisparityGraph& graph,
    struct Edge edge
)
{
    return graph.reparametrization[reparametrization_index_slow(graph, edge)];
}

FLOAT reparametrization_value_fast(
    const struct DisparityGraph& graph,
    struct Node node,
    ULONG neighbor_index
)
{
    return graph.reparametrization[
        reparametrization_index_fast(graph, node, neighbor_index)
    ];
}

FLOAT edge_penalty(const struct DisparityGraph& graph, struct Edge edge)
{
    return
        SQR(TO_FLOAT(edge.node.disparity) - TO_FLOAT(edge.neighbor.disparity))
        + reparametrization_value_slow(graph, edge)
        + reparametrization_value(graph, edge.neighbor, edge.node.pixel);
}

FLOAT node_penalty(const struct DisparityGraph& graph, struct Node node)
{
    Pixel left_pixel = node.pixel;
    left_pixel.column += node.disparity;
    return
        SQR(TO_FLOAT(get_pixel_value(graph.right, node.pixel))
            - TO_FLOAT(get_pixel_value(graph.left, left_pixel)))
        - reparametrization_value_fast(graph, node, 0)
        - reparametrization_value_fast(graph, node, 1)
        - reparametrization_value_fast(graph, node, 2)
        - reparametrization_value_fast(graph, node, 3);
}
