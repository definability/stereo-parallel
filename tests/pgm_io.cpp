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
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <image.hpp>
#include <pgm_io.hpp>

BOOST_AUTO_TEST_SUITE(PGM_IO_test)

BOOST_AUTO_TEST_CASE(read_image)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    # Grayscale image
    P2
    # 3 columns and 2 rows
    3 2
    10 # Another pretty comment before a blank line

    0 1 2
    3 4 5
    )image"};
    image_content >> pgm_io;
    BOOST_CHECK(pgm_io.get_image());
    BOOST_CHECK_EQUAL(pgm_io.get_image()->width, 3);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->height, 2);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->max_value, 10);

    struct Image image{*pgm_io.get_image()};
    BOOST_CHECK_EQUAL(get_pixel_value(image, {0, 0}), 0);
    BOOST_CHECK_EQUAL(get_pixel_value(image, {0, 1}), 1);
    BOOST_CHECK_EQUAL(get_pixel_value(image, {0, 2}), 2);
    BOOST_CHECK_EQUAL(get_pixel_value(image, {1, 0}), 3);
    BOOST_CHECK_EQUAL(get_pixel_value(image, {1, 1}), 4);
    BOOST_CHECK_EQUAL(get_pixel_value(image, {1, 2}), 5);
}

BOOST_AUTO_TEST_CASE(read_wrong_format_name)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P
    3 2
    10
    0 1 2
    3 4 5
    )image"};
    image_content >> pgm_io;
    BOOST_CHECK(!pgm_io.get_image());
    BOOST_CHECK(!image_content);
}

BOOST_AUTO_TEST_CASE(read_wrong_max_value)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P2
    3 2
    65537
    0 1 2
    3 4 5
    )image"};
    image_content >> pgm_io;
    BOOST_CHECK(!pgm_io.get_image());
    BOOST_CHECK(!image_content);
}

BOOST_AUTO_TEST_CASE(read_wrong_data)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P2
    3 2
    -1
    0 1 2
    3 4
    )image"};
    image_content >> pgm_io;
    BOOST_CHECK(!pgm_io.get_image());
    BOOST_CHECK(!image_content);
}

BOOST_AUTO_TEST_CASE(write_image)
{
    std::shared_ptr<struct Image> image = std::make_shared<struct Image>(
        Image{3, 2, 5, {0, 1, 2, 3, 4, 5}});
    PGM_IO pgm_io{image};

    std::ostringstream image_content;
    image_content << pgm_io;

    BOOST_CHECK_EQUAL(image_content.str(),
R"image(P2
3 2
5
0 1 2
3 4 5
)image");
}

BOOST_AUTO_TEST_CASE(write_no_image)
{
    PGM_IO pgm_io;

    std::ostringstream image_content;
    image_content << pgm_io;

    BOOST_CHECK_EQUAL(image_content.str(), "");
}

BOOST_AUTO_TEST_CASE(read_write_image)
{
    PGM_IO pgm_io;
    std::string image_string{R"image(P2
3 2
5
0 1 2
3 4 5
)image"};

    std::istringstream image_input{image_string};
    image_input >> pgm_io;
    BOOST_CHECK(pgm_io.get_image());
    BOOST_CHECK_EQUAL(pgm_io.get_image()->width, 3);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->height, 2);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->max_value, 5);

    std::ostringstream image_output;
    image_output << pgm_io;

    BOOST_CHECK_EQUAL(image_output.str(), image_string);
}

BOOST_AUTO_TEST_CASE(read_write_imagelong)
{
    PGM_IO pgm_io;
    std::string image_string{R"image(P2
12 3
65536
65536 65536 65536 65536 65536 65536 65536 65536 65536 65536 65536
65536
1 2 3 4 5 6 7 8 9 10 11
12
0 1 100 1000 10000 5 50 500 5000 50000 65535
0
)image"};

    std::istringstream image_input{image_string};
    image_input >> pgm_io;
    BOOST_CHECK(pgm_io.get_image());
    BOOST_CHECK_EQUAL(pgm_io.get_image()->width, 12);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->height, 3);
    BOOST_CHECK_EQUAL(pgm_io.get_image()->max_value, 65536);

    std::ostringstream image_output;
    image_output << pgm_io;

    BOOST_CHECK_EQUAL(image_output.str(), image_string);
}

BOOST_AUTO_TEST_SUITE_END()
