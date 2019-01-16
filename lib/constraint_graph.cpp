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
#include <constraint_graph.hpp>

#include <disparity_graph.hpp>
#include <lowest_penalties.hpp>

ConstraintGraph::ConstraintGraph(
    const struct DisparityGraph& disparity_graph,
    const struct LowestPenalties& lowest_penalties,
    FLOAT threshold
)
    : disparity_graph{disparity_graph}
    , lowest_penalties{lowest_penalties}
    , threshold{threshold}
{
    this->nodes_availability = BOOL_ARRAY(
        disparity_graph.right.width
        * disparity_graph.right.height
        * disparity_graph.disparity_levels
    );
    fill(
        this->nodes_availability.begin(),
        this->nodes_availability.end(),
        false
    );

    Node node{{0, 0}, 0};
    for (
        node.pixel.x = 0;
        node.pixel.x < this->disparity_graph.right.width;
        ++node.pixel.x
    )
    {
        for (
            node.pixel.y = 0;
            node.pixel.y < this->disparity_graph.right.height;
            ++node.pixel.y
        )
        {
            for (
                node.disparity = 0;
                node.pixel.x + node.disparity
                    < this->disparity_graph.left.width
                && node.disparity < this->disparity_graph.disparity_levels;
                ++node.disparity
            )
            {
                if (
                    node_penalty(this->disparity_graph, node)
                    - lowest_pixel_penalty(this->lowest_penalties, node.pixel)
                    <= threshold
                )
                {
                    make_node_available(this, node);
                }
            }
        }
    }
}

ULONG node_index(const struct DisparityGraph& graph, struct Node node)
{
    return node.disparity + graph.disparity_levels
        * (node.pixel.y + graph.right.height * node.pixel.x);
}

void make_node_available(
    struct ConstraintGraph* graph,
    struct Node node
)
{
    graph->nodes_availability[node_index(graph->disparity_graph, node)]
        = true;
}

void make_node_unavailable(
    struct ConstraintGraph* graph,
    struct Node node
)
{
    graph->nodes_availability[node_index(graph->disparity_graph, node)]
        = false;
}

BOOL is_node_available(
    const struct ConstraintGraph& graph,
    struct Node node
)
{
    return graph.nodes_availability[
        node_index(graph.disparity_graph, node)
    ];
}

