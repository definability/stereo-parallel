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
#include <constraint_graph.hpp>

#include <disparity_graph.hpp>

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))

ULONG node_index(const struct DisparityGraph& graph, struct Node node)
{
    return node.disparity + graph.maximal_disparity
        * (node.pixel.row + graph.right.height * node.pixel.column);
}

void make_node_available(
    struct ConstraintGraph* graph,
    struct Node node
)
{
    graph->nodes_availability[node_index(*(graph->disparity_graph), node)]
        = true;
}

void make_node_unavailable(
    struct ConstraintGraph* graph,
    struct Node node
)
{
    graph->nodes_availability[node_index(*(graph->disparity_graph), node)]
        = false;
}

BOOL is_node_available(
    const struct ConstraintGraph& graph,
    struct Node node
)
{
    return graph.nodes_availability[
        node_index(*(graph.disparity_graph), node)
    ];
}

struct ConstraintGraph disparity2constraint(
    const struct DisparityGraph& disparity_graph,
    FLOAT threshold
)
{
    struct ConstraintGraph constraint_graph{
        &disparity_graph,
        BOOL_ARRAY(
            disparity_graph.right.width
            * disparity_graph.right.height
            * disparity_graph.maximal_disparity
        ),
        threshold
    };
    fill(
        constraint_graph.nodes_availability.begin(),
        constraint_graph.nodes_availability.end(),
        false
    );

    Node node{{0, 0}, 0};
    FLOAT minimal_penalty = 0;
    for (
        node.pixel.column = 0;
        node.pixel.column < disparity_graph.right.width;
        ++node.pixel.column
    )
    {
        for (
            node.pixel.row = 0;
            node.pixel.row < disparity_graph.right.height;
            ++node.pixel.row
        )
        {
            minimal_penalty = node_penalty(disparity_graph, node);
            for (
                node.disparity = 0;
                node.pixel.column + node.disparity
                    < disparity_graph.left.width;
                ++node.disparity
            )
            {
                minimal_penalty = MIN(
                    node_penalty(disparity_graph, node),
                    minimal_penalty
                );
            }

            for (
                node.disparity = 0;
                node.pixel.column + node.disparity
                    < disparity_graph.left.width;
                ++node.disparity
            )
            {
                if (
                    node_penalty(disparity_graph, node)
                    - minimal_penalty <= threshold
                )
                {
                    make_node_available(&constraint_graph, node);
                }
            }
        }
    }
    return constraint_graph;
}
