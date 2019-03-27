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
#include <labeling_finder.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>

FLOAT_ARRAY fetch_pixel_available_penalties(
    const DisparityGraph& graph,
    struct Pixel pixel,
    FLOAT minimal_penalty
)
{
    FLOAT_ARRAY result(
        pixel.x + graph.disparity_levels < graph.left.width + 1
        ? graph.disparity_levels
        : graph.left.width - pixel.x
    );
    Node node{pixel, 0};
    for (
        node.disparity = 0;
        node.disparity < result.size();
        ++node.disparity
    )
    {
        result[node.disparity] = (
            node_penalty(graph, node)
            - minimal_penalty
        );
    }

    std::sort(result.begin(), result.end());
    auto last = std::unique(result.begin(), result.end());
    result.erase(last, result.end());

    return result;
}

FLOAT_ARRAY fetch_edge_available_penalties(
    const struct DisparityGraph& graph,
    struct Edge edge,
    FLOAT minimal_penalty
)
{
    FLOAT_ARRAY result;

    ULONG initial_disparity = 0;
    for (
        edge.node.disparity = 0;
        edge.node.pixel.x + edge.node.disparity < graph.left.width
            && edge.node.disparity < graph.disparity_levels;
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
            edge.neighbor.pixel.x + edge.neighbor.disparity < graph.left.width
                && edge.neighbor.disparity < graph.disparity_levels;
            ++edge.neighbor.disparity
        )
        {
            result.push_back(edge_penalty(graph, edge) - minimal_penalty);
        }
    }

    std::sort(result.begin(), result.end());
    auto last = std::unique(result.begin(), result.end());
    result.erase(last, result.end());

    return result;
}

FLOAT_ARRAY fetch_available_penalties(
    const struct LowestPenalties& lowest_penalties
)
{
    FLOAT_ARRAY result;
    FLOAT_ARRAY result_;
    FLOAT_ARRAY available_penalties;
    struct Pixel pixel{0, 0};
    for (pixel.x = 0; pixel.x < lowest_penalties.graph.right.width; ++pixel.x)
    {
        for (
            pixel.y = 0;
            pixel.y < lowest_penalties.graph.right.height;
            ++pixel.y)
        {
            available_penalties = fetch_pixel_available_penalties(
                lowest_penalties.graph,
                pixel,
                lowest_pixel_penalty(lowest_penalties, pixel)
            );
            std::merge(
                result.begin(),
                result.end(),
                available_penalties.begin(),
                available_penalties.end(),
                std::back_inserter(result_)
            );
            std::swap(result, result_);
            result_.clear();
            available_penalties.clear();

            std::sort(result.begin(), result.end());
            auto last = std::unique(result.begin(), result.end());
            result.erase(last, result.end());

            for (
                ULONG neighbor_index = 0;
                neighbor_index < NEIGHBORS_COUNT;
                ++neighbor_index
            )
            {
                if (
                    !neighborhood_exists_fast(
                        lowest_penalties.graph, pixel, neighbor_index
                    )
                )
                {
                    continue;
                }
                Edge edge{
                    {pixel, 0},
                    {neighbor_by_index(pixel, neighbor_index), 0}
                };
                available_penalties = fetch_edge_available_penalties(
                    lowest_penalties.graph,
                    edge,
                    lowest_neighborhood_penalty(
                        lowest_penalties,
                        edge
                    )
                );
                std::merge(
                    result.begin(),
                    result.end(),
                    available_penalties.begin(),
                    available_penalties.end(),
                    std::back_inserter(result_)
                );
                std::swap(result, result_);
                result_.clear();
                available_penalties.clear();

                std::sort(result.begin(), result.end());
                auto last = std::unique(result.begin(), result.end());
                result.erase(last, result.end());
            }
        }
    }
    return result;
}

FLOAT calculate_minimal_consistent_threshold(
    const struct LowestPenalties& lowest_penalties,
    const struct DisparityGraph& disparity_graph,
    FLOAT_ARRAY available_penalties
)
{
    ULONG start = 0;
    ULONG end = available_penalties.size() - 1;
    ULONG current_index;
    for (
        current_index = (start + end) / 2;
        start < end;
        current_index = (start + end) / 2
    )
    {
        struct ConstraintGraph constraint_graph{
            disparity_graph,
            lowest_penalties,
            available_penalties[current_index]
        };
        if (solve_csp(&constraint_graph))
        {
            end = current_index;
        }
        else
        {
            start = current_index + 1;
        }
    }
    return available_penalties[current_index];
}

struct ConstraintGraph* choose_best_node(
    struct ConstraintGraph* graph,
    struct Pixel pixel
)
{
    Node node{pixel, 0};
    FLOAT minimal_penalty = -1;
    for (
        node.disparity = 0;
        node.pixel.x + node.disparity < graph->disparity_graph.left.width
            && node.disparity < graph->disparity_graph.disparity_levels;
        ++node.disparity
    )
    {
        if (!is_node_available(*graph, node))
        {
            continue;
        }

        if (minimal_penalty < 0)
        {
            minimal_penalty = node_penalty(graph->disparity_graph, node);
        }
        else {
            minimal_penalty = MIN(
                node_penalty(graph->disparity_graph, node),
                minimal_penalty
            );
        }
    }

    bool node_chosen = false;
    for (
        node.disparity = 0;
        node.pixel.x + node.disparity < graph->disparity_graph.left.width
            && node.disparity < graph->disparity_graph.disparity_levels;
        ++node.disparity
    )
    {
        if (!is_node_available(*graph, node))
        {
            continue;
        }

        if (
            node_chosen
            || node_penalty(graph->disparity_graph, node) > minimal_penalty
        )
        {
            make_node_unavailable(graph, node);
        }
        else
        {
            node_chosen = true;
        }
    }
    return node_chosen? graph : nullptr;
}

struct ConstraintGraph* find_labeling(
    struct ConstraintGraph* graph
)
{
    for (ULONG x = 0; x < graph->disparity_graph.left.width; ++x)
    {
        for (ULONG y = 0; y < graph->disparity_graph.left.height; ++y)
        {
            if (choose_best_node(graph, {x, y}) == nullptr)
            {
                return nullptr;
            }
            if (!solve_csp(graph))
            {
                return nullptr;
            }
        }
    }
    return graph;
}

struct Image build_disparity_map(
    const struct ConstraintGraph& constraint_graph
)
{
    struct Image result{
        constraint_graph.disparity_graph.left.width,
        constraint_graph.disparity_graph.left.height,
        constraint_graph.disparity_graph.disparity_levels,
        ULONG_ARRAY(
            constraint_graph.disparity_graph.left.height
            * constraint_graph.disparity_graph.left.width
        )
    };
    Node node{{0, 0}, 0};
    BOOL found = false;
    for (
        node.pixel.x = 0;
        node.pixel.x < constraint_graph.disparity_graph.left.width;
        ++node.pixel.x)
    {
        for (
            node.pixel.y = 0;
            node.pixel. y < constraint_graph.disparity_graph.left.height;
            ++node.pixel.y)
        {
            for (
                node.disparity = 0;
                node.pixel.x + node.disparity
                    < constraint_graph.disparity_graph.left.width
                && node.disparity
                    < constraint_graph.disparity_graph.disparity_levels;
                ++node.disparity
            )
            {
                if (is_node_available(constraint_graph, node))
                {
                    if (found)
                    {
                        throw std::logic_error(
                            "Two labels found for the pixel <"
                            + std::to_string(node.pixel.x)
                            + ", "
                            + std::to_string(node.pixel.y)
                            + ">."
                        );
                    }
                    result.data[pixel_index(result, node.pixel)]
                        = node.disparity;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                throw std::logic_error(
                    "Cannot find label for the pixel <"
                    + std::to_string(node.pixel.x)
                    + ", "
                    + std::to_string(node.pixel.y)
                    + ">."
                );
            }
            found = false;
        }
    }
    return result;
}
