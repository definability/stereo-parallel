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
/**
 * @file
 */
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <types.hpp>

namespace sp::image
{

using sp::types::ULONG;
using sp::types::ULONG_ARRAY;

/**
 * \brief Structure to represent image on both CPU and GPU.
 */
struct Image
{
    /**
     * \brief Width of the image in pixels.
     */
    ULONG width;
    /**
     * \brief Height of the image in pixels.
     */
    ULONG height;
    /**
     * \brief Maximal intensity contained in Image::data array.
     */
    ULONG max_value;
    /**
     * \brief Data contained in the image.
     *
     * Image is represented as a grid with intensities of pixels.
     * Intensity is a non-negative integer.
     *
     * In order to increase performance
     * 1D array is used with row-major order:
     * first, we put elements of the first row one-by-one,
     * then the second one, and so on
     *
     * \f[
     *  \begin{bmatrix}
     *      a_{0 0} & a_{0 1} \\
     *      a_{1 0} & a_{1 1} \\
     *  \end{bmatrix}
     *  \mapsto
     *  \begin{bmatrix}
     *      a_{0 0} & a_{0 1} &
     *      a_{1 0} & a_{1 1}
     *  \end{bmatrix}
     * \f]
     */
    ULONG_ARRAY data;
};

/**
 * \brief Check validity of an image.
 *
 * Image should contain at least one pixel,
 * maximal intensity should be greater than zero,
 * and neither intensity should exceed specified maximal value.
 */
bool image_valid(const struct Image& image);

}

#endif
