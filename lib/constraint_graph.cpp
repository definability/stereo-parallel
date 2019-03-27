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
#include <indexing.hpp>
#include <indexing_checks.hpp>
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

void make_all_nodes_unavailable(struct ConstraintGraph* graph)
{
    for (
        ULONG index = 0;
        index <
            graph->disparity_graph.right.width
            * graph->disparity_graph.right.height
            * graph->disparity_graph.disparity_levels;
        ++index
    )
    {
        graph->nodes_availability[index] = false;
    }
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

BOOL is_edge_available(
    const struct ConstraintGraph& graph,
    struct Edge edge
)
{
    return
        (edge_penalty(graph.disparity_graph, edge)
         - lowest_neighborhood_penalty(graph.lowest_penalties, edge)
         <= graph.threshold)
        && is_node_available(graph, edge.node)
        && is_node_available(graph, edge.neighbor)
        && edge_exists(graph.disparity_graph, edge);
}

BOOL should_remove_node(
    const struct ConstraintGraph& graph,
    struct Node node
)
{
    if (!is_node_available(graph, node))
    {
        return false;
    }

    Edge edge{node, node};
    ULONG initial_disparity = 0;
    BOOL edge_found = false;

    for (
        ULONG neighbor_index = 0;
        neighbor_index < NEIGHBORS_COUNT;
        ++neighbor_index
    )
    {
        if (
            !neighborhood_exists_fast(
                graph.disparity_graph,
                node.pixel,
                neighbor_index
            )
        )
        {
            continue;
        }

        edge.neighbor.pixel = neighbor_by_index(node.pixel, neighbor_index);
        if (
            edge.node.pixel.x + 1 == edge.neighbor.pixel.x
            && edge.node.disparity > 1
        )
        {
            initial_disparity = edge.node.disparity - 1;
        }
        else
        {
            initial_disparity = 0;
        }

        edge_found = false;
        for (
            edge.neighbor.disparity = initial_disparity;
            edge.neighbor.pixel.x + edge.neighbor.disparity
                < graph.disparity_graph.left.width
            && edge.neighbor.disparity
                < graph.disparity_graph.disparity_levels;
            ++edge.neighbor.disparity
        )
        {
            if (is_edge_available(graph, edge))
            {
                edge_found = true;
                break;
            }
        }
        if (!edge_found)
        {
            return true;
        }
    }
    return false;
}

BOOL check_nodes_left(const struct ConstraintGraph& graph)
{
    for (
        ULONG index = 0;
        index <
            graph.disparity_graph.right.width
            * graph.disparity_graph.right.height
            * graph.disparity_graph.disparity_levels;
        ++index
    )
    {
        if (graph.nodes_availability[index])
        {
            return true;
        }
    }
    return false;
}

BOOL csp_solution_iteration(struct ConstraintGraph* graph)
{
    Node node{{0, 0}, 0};
    BOOL pixel_available = false;
    BOOL changed = false;

    for (
        node.pixel.x = 0;
        node.pixel.x < graph->disparity_graph.right.width;
        ++node.pixel.x
    )
    {
        for (
            node.pixel.y = 0;
            node.pixel.y < graph->disparity_graph.right.height;
            ++node.pixel.y
        )
        {
            pixel_available = false;
            for (
                node.disparity = 0;
                node.pixel.x + node.disparity
                    < graph->disparity_graph.right.width
                && node.disparity < graph->disparity_graph.disparity_levels;
                ++node.disparity
            )
            {
                if (should_remove_node(*graph, node))
                {
                    make_node_unavailable(graph, node);
                    changed = true;
                }
                else if (is_node_available(*graph, node))
                {
                    pixel_available = true;
                }
            }
            if (!pixel_available)
            {
                break;
            }
        }
        if (!pixel_available)
        {
            break;
        }
    }
    if (!pixel_available)
    {
        if (!check_nodes_left(*graph))
        {
            return false;
        }
        make_all_nodes_unavailable(graph);
        return true;
    }
    return changed;
}

BOOL solve_csp(struct ConstraintGraph* graph)
{
    while (csp_solution_iteration(graph))
    {
    }
    return check_nodes_left(*graph);
}
