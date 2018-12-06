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
/**
 * @file
 */
#ifndef CONSTRAINT_GRAPH_HPP
#define CONSTRAINT_GRAPH_HPP

#include <image.hpp>

#include <vector>

/**
 * \brief Boolean type alias.
 * to use the same name on CPU and GPU.
 */
using BOOL = bool;
/**
 * \brief Boolean array type alias
 * to use the same name on CPU and GPU.
 */
using BOOL_ARRAY = std::vector<BOOL>;

/**
 * \brief Structure to represent a graph with constraints
 * on choice of disparities for pixels (CSP problem).
 *
 * After optimization performed on ::DisparityGraph,
 * it's needed to choose one labeling of many others
 * that satisfy constraints of the problem.
 */
struct ConstraintGraph
{
    /**
     * \brief Array that contains markers for availability of nodes.
     *
     * `false` means that the node cannot be chosen.
     * `true` means that the node can be chosen under applied constraints.
     */
    BOOL_ARRAY nodes_availability;
    /**
     * \brief Array that contains markers for availability of edges.
     *
     * `false` means that the edge cannot be chosen.
     * `true` means that the edge can be chosen under applied constraints.
     */
    BOOL_ARRAY edges_availability;
};

#endif
