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
#include <image.hpp>

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
namespace sp
{
namespace image
{
#endif

BOOL image_valid(const struct Image* image)
{
    if (image->max_value == 0 || image->width == 0 || image->height == 0)
    {
        return false;
    }
    for (unsigned i = 0; i < image->width * image->height; ++i)
    {
        if (image->data[i] > image->max_value)
        {
            return false;
        }
    }
    return true;
}

#if !defined(__OPENCL_C_VERSION__) && !defined(__CUDA_ARCH__)
}
}
#endif
