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
#include <lowest_penalties.hpp>

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
            * disparity_graph.disparity_levels
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
        node.pixel.x = 0;
        node.pixel.x < disparity_graph.right.width;
        ++node.pixel.x
    )
    {
        for (
            node.pixel.y = 0;
            node.pixel.y < disparity_graph.right.height;
            ++node.pixel.y
        )
        {
            minimal_penalty = calculate_lowest_pixel_penalty(
                disparity_graph,
                node.pixel
            );
            for (
                node.disparity = 0;
                node.pixel.x + node.disparity
                    < disparity_graph.left.width
                && node.disparity < disparity_graph.disparity_levels;
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
