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
#include <boost/test/unit_test.hpp>

#include <image.hpp>
#include <indexing.hpp>

BOOST_AUTO_TEST_SUITE(ImageTest)

using sp::image::Image;
using sp::indexing::pixel_index;
using sp::indexing::pixel_value;
using sp::types::ULONG;

BOOST_AUTO_TEST_CASE(create_image)
{
    struct Image image{2, 1, 1, {0, 1}};
    BOOST_CHECK_EQUAL(image.width, 2);
    BOOST_CHECK_EQUAL(image.height, 1);
    BOOST_CHECK_EQUAL(image.max_value, 1);
    BOOST_CHECK(image.data == std::vector<ULONG>({0, 1}));
}

BOOST_AUTO_TEST_CASE(check_image_valid)
{
    struct Image image{2, 1, 1, {0, 1}};
    BOOST_CHECK(image_valid(image));
}

BOOST_AUTO_TEST_CASE(image_invalid_value)
{
    struct Image image{2, 1, 1, {0, 2}};
    BOOST_CHECK(!image_valid(image));
}

BOOST_AUTO_TEST_CASE(image_invalid_max_value)
{
    struct Image image{2, 1, 0, {0, 0}};
    BOOST_CHECK(!image_valid(image));
}

BOOST_AUTO_TEST_CASE(image_invalid_size)
{
    struct Image image{0, 0, 1, {}};
    BOOST_CHECK(!image_valid(image));
}

BOOST_AUTO_TEST_CASE(check_pixel_index)
{
    struct Image image{3, 2, 5, {5, 4, 3, 2, 1, 0}};
    BOOST_CHECK(image_valid(image));
    BOOST_CHECK_EQUAL(image.width, 3);
    BOOST_CHECK_EQUAL(image.height, 2);
    BOOST_CHECK_EQUAL(pixel_index(image, {0, 0}), 0);
    BOOST_CHECK_EQUAL(pixel_index(image, {1, 0}), 1);
    BOOST_CHECK_EQUAL(pixel_index(image, {2, 0}), 2);
    BOOST_CHECK_EQUAL(pixel_index(image, {0, 1}), 3);
    BOOST_CHECK_EQUAL(pixel_index(image, {1, 1}), 4);
    BOOST_CHECK_EQUAL(pixel_index(image, {2, 1}), 5);
}

BOOST_AUTO_TEST_CASE(check_pixel_value)
{
    struct Image image{3, 2, 5, {5, 4, 3, 2, 1, 0}};
    BOOST_CHECK(image_valid(image));
    BOOST_CHECK_EQUAL(image.width, 3);
    BOOST_CHECK_EQUAL(image.height, 2);
    BOOST_CHECK_EQUAL(pixel_value(image, {0, 0}), 5);
    BOOST_CHECK_EQUAL(pixel_value(image, {1, 0}), 4);
    BOOST_CHECK_EQUAL(pixel_value(image, {2, 0}), 3);
    BOOST_CHECK_EQUAL(pixel_value(image, {0, 1}), 2);
    BOOST_CHECK_EQUAL(pixel_value(image, {1, 1}), 1);
    BOOST_CHECK_EQUAL(pixel_value(image, {2, 1}), 0);
}

BOOST_AUTO_TEST_SUITE_END()
