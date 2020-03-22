/*
 * MIT License
 *
 * Copyright (c) 2018-2020 char-lie
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
#ifndef TYPES_HPP
#define TYPES_HPP

#if !defined(__OPENCL_C_VERSION__)
#define __global
#endif

#if !defined(__device__)
#define __device__
#endif

#if !defined(__host__)
#define __host__
#endif

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
#include <vector>
/**
 * \brief Basic types.
 */
namespace sp
{
namespace types
{

/**
 * \brief Unsigned long type alias
 * to use the same name on CPU and GPU.
 */
using ULONG = unsigned long;

/**
 * \brief Unsigned long array type alias
 * to use the same name on CPU and GPU.
 */
using ULONG_ARRAY = std::vector<ULONG>;

/**
 * \brief Floating point type alias.
 * to use the same name on CPU and GPU.
 */
using FLOAT = double;

/**
 * \brief Floating point array type alias
 * to use the same name on CPU and GPU.
 */
using FLOAT_ARRAY = std::vector<FLOAT>;

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

const BOOL TRUE = true;
const BOOL FALSE = false;

#elif defined(__OPENCL_C_VERSION__)

#define ULONG ulong
#define ULONG_ARRAY ULONG*
#define FLOAT float
#define FLOAT_ARRAY FLOAT*
#define BOOL int
#define BOOL_ARRAY BOOL*

#define TRUE 1
#define FALSE 0

#elif defined(__CUDA_ARCH__)

using ULONG = unsigned;
using ULONG_ARRAY = ULONG*;
using FLOAT = float;
using FLOAT_ARRAY = FLOAT*;
using BOOL = int;
using BOOL_ARRAY = BOOL*;

const BOOL TRUE = 1;
const BOOL FALSE = 0;

#endif

/**
 * Structure that contains position of a pixel.
 */
struct Pixel
{
    /**
     * \brief Column (horizontal offset) of the pixel.
     */
    ULONG x;
    /**
     * \brief Row (vertical offset) of the pixel.
     */
    ULONG y;
};

/**
 * \brief Node of sp::graph::disparity::DisparityGraph is a pair
 * consisting of Pixel and disparity assigned to it.
 */
struct Node
{
    /**
     * \brief Pixel to which the Node instance belongs.
     */
    struct Pixel pixel;
    /**
     * \brief Disparity assigned to the Node::pixel in the Node instance.
     *
     * Disparity specifies the difference between columns
     * of current pixel on the right image
     * and corresponding one on the left image.
     */
    ULONG disparity;
};

/**
 * \brief Edge is an ordered pair of two Node instances.
 */
struct Edge
{
    /**
     * \brief Start Node of the directed Edge.
     */
    struct Node node;
    /**
     * \brief Start Node of the directed Edge.
     */
    struct Node neighbor;
};

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
}
}
#endif

#endif
