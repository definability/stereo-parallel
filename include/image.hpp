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
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <memory>

/**
 * \brief Unsigned long type alias
 * to use the same name on CPU and GPU.
 */
using ULONG = unsigned long;
/**
 * \brief Unsigned long pointer type alias
 * to use the same name on CPU and GPU.
 */
using ULONG_PTR = std::shared_ptr<ULONG>;

/**
 * \brief Structure to represent image on both CPU and GPU.
 */
struct Image {
    /**
     * \brief Width of the image in pixels.
     */
    ULONG width;
    /**
     * \brief Height of the image in pixels.
     */
    ULONG height;
    /**
     * \brief Data contained in the image.
     *
     * Image is represented as a grid with intensities of pixels.
     * Intensity is a non-negative integer.
     */
    ULONG_PTR data;
};

#endif
