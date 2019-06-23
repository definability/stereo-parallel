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

#ifndef __OPENCL_C_VERSION__
#include <limits>
#include <stdexcept>
#include <string>

#define TO_FLOAT(x) (static_cast<FLOAT>(x))

#endif

#define SQR(x) ((x) * (x))

#ifndef __OPENCL_C_VERSION__
namespace sp::graph::disparity
{

using sp::indexing::pixel_value;
using sp::indexing::reparametrization_value;
using sp::indexing::reparametrization_value_fast;
using sp::indexing::reparametrization_value_slow;
using sp::types::Pixel;
using std::invalid_argument;
using std::move;
using std::numeric_limits;
using std::to_string;

DisparityGraph::DisparityGraph(
    struct Image left,
    struct Image right,
    ULONG disparity_levels,
    FLOAT cleanness,
    FLOAT smoothness
)
    : left{std::move(left)}
    , right{std::move(right)}
    , disparity_levels{disparity_levels}
    , cleanness{cleanness}
    , smoothness{smoothness}
{
    if (!image_valid(&(this->left)))
    {
        throw std::invalid_argument("Left image is invalid.");
    }
    if (!image_valid(&(this->right)))
    {
        throw std::invalid_argument("Right image is invalid.");
    }
    if (this->disparity_levels <= 1)
    {
        throw std::invalid_argument(
            "Number of disparity levels should be greater than one."
        );
    }
    if (this->disparity_levels > this->left.width)
    {
        throw std::invalid_argument(
            "Number of disparity levels "
            "should not be greater than width of the left image. "
            "Width of the left image is "
            + std::to_string(this->left.width)
            + ". Provided number of disparity levels is "
            + std::to_string(this->disparity_levels)
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
    if (this->cleanness < 0)
    {
        throw std::invalid_argument(
            "Cleanness weight should not be negative. "
            "Actual value is "
            + std::to_string(this->cleanness)
            + "."
        );
    }
    if (this->smoothness < 0)
    {
        throw std::invalid_argument(
            "Smoothness weight should not be negative. "
            "Actual value is "
            + std::to_string(this->smoothness)
            + "."
        );
    }
    if (
        this->cleanness < std::numeric_limits<FLOAT>::epsilon()
        && this->smoothness < std::numeric_limits<FLOAT>::epsilon()
    )
    {
        throw std::invalid_argument(
            "Either cleanness or smoothness, or both, "
            "should be creater than zero. "
            "You've provided smoothness "
            + std::to_string(this->smoothness)
            + " and cleanness "
            + std::to_string(this->cleanness)
            + "."
        );
    }

    this->reparametrization.resize(
        this->left.width
        * this->left.height
        * NEIGHBORS_COUNT
        * this->disparity_levels
    );
    fill(
        this->reparametrization.begin(),
        this->reparametrization.end(),
        static_cast<FLOAT>(0)
    );
}
#endif

FLOAT edge_penalty(const struct DisparityGraph* graph, struct Edge edge)
{
    return
        graph->smoothness
        * SQR(TO_FLOAT(edge.node.disparity) - TO_FLOAT(edge.neighbor.disparity))
        - reparametrization_value_slow(graph, edge)
        - reparametrization_value(graph, edge.neighbor, edge.node.pixel);
}

FLOAT node_penalty(const struct DisparityGraph* graph, struct Node node)
{
    struct Pixel left_pixel;
    left_pixel.x = node.pixel.x + node.disparity;
    left_pixel.y = node.pixel.y;
    return
        graph->cleanness
        * SQR(TO_FLOAT(pixel_value(&(graph->right), node.pixel))
            - TO_FLOAT(pixel_value(&(graph->left), left_pixel)))
        + reparametrization_value_fast(graph, node, 0)
        + reparametrization_value_fast(graph, node, 1)
        + reparametrization_value_fast(graph, node, 2)
        + reparametrization_value_fast(graph, node, 3);
}

#ifndef __OPENCL_C_VERSION__
}
#endif
