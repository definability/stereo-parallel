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

#include <lowest_penalties.hpp>
#include <disparity_graph.hpp>
#include <image.hpp>
#include <pgm_io.hpp>

BOOST_AUTO_TEST_SUITE(LowestPenaltiesTest)

BOOST_AUTO_TEST_CASE(check_pixels_indexing)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P2
    3 2
    2
    0 0 0
    0 0 0
    )image"};

    image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{image, image, 3, 1, 1};
    struct LowestPenalties lowest_penalties{disparity2lowest(disparity_graph)};
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {0, 0}), 0);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {0, 1}), 1);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {1, 0}), 2);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {1, 1}), 3);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {2, 0}), 4);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.disparity_graph, {2, 1}), 5);
}

BOOST_AUTO_TEST_CASE(check_neighborhoods_indexing)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P2
    3 2
    2
    0 0 0
    0 0 0
    )image"};

    image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{image, image, 3, 1, 1};
    struct LowestPenalties lowest_penalties{disparity2lowest(disparity_graph)};
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 0}, 0),
        0
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {0, 0}, {1, 0}),
        0
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 0}, 1),
        1
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 0}, 2),
        2
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {0, 0}, {0, 1}),
        2
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 0}, 3),
        3
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 1}, 0),
        4
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {0, 1}, {1, 1}),
        4
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 1}, 1),
        5
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 1}, 2),
        6
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {0, 1}, 3),
        7
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {0, 1}, {0, 0}),
        7
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {0, 1}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 0}, 0),
        8
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 0}, {2, 0}),
        8
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 0}, 1),
        9
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 0}, {0, 0}),
        9
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 0}, 2),
        10
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 0}, {1, 1}),
        10
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 0}, 3),
        11
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 1}, 0),
        12
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 1}, {2, 1}),
        12
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 1}, 1),
        13
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 1}, {0, 1}),
        13
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 1}, 2),
        14
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {1, 1}, 3),
        15
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {1, 1}, {1, 0}),
        15
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {1, 1}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 0}, 0),
        16
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 0}, 1),
        17
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {2, 0}, {1, 0}),
        17
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 0}, 2),
        18
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {2, 0}, {2, 1}),
        18
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 0}, 3),
        19
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 1}, 0),
        20
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 1}, 1),
        21
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {2, 1}, {1, 1}),
        21
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 1}, 2),
        22
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.disparity_graph, {2, 1}, 3),
        23
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.disparity_graph, {2, 1}, {2, 0}),
        23
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.disparity_graph, {2, 1}, 3)
    );
}

BOOST_AUTO_TEST_SUITE_END()
