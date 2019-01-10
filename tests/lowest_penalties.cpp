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

#include <disparity_graph.hpp>
#include <image.hpp>
#include <lowest_penalties.hpp>
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
    struct LowestPenalties lowest_penalties{disparity_graph};
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {0, 0}), 0);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {0, 1}), 1);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {1, 0}), 2);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {1, 1}), 3);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {2, 0}), 4);
    BOOST_CHECK_EQUAL(pixel_index(lowest_penalties.graph, {2, 1}), 5);
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
    struct LowestPenalties lowest_penalties{disparity_graph};
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 0}, 0),
        0
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {0, 0}, {1, 0}),
        0
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {0, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 0}, 1),
        1
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {0, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 0}, 2),
        2
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {0, 0}, {0, 1}),
        2
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {0, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 0}, 3),
        3
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {0, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 1}, 0),
        4
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {0, 1}, {1, 1}),
        4
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {0, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 1}, 1),
        5
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {0, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 1}, 2),
        6
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {0, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {0, 1}, 3),
        7
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {0, 1}, {0, 0}),
        7
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {0, 1}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 0}, 0),
        8
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 0}, {2, 0}),
        8
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 0}, 1),
        9
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 0}, {0, 0}),
        9
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 0}, 2),
        10
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 0}, {1, 1}),
        10
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 0}, 3),
        11
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {1, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 1}, 0),
        12
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 1}, {2, 1}),
        12
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 1}, 1),
        13
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 1}, {0, 1}),
        13
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 1}, 2),
        14
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {1, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {1, 1}, 3),
        15
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {1, 1}, {1, 0}),
        15
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {1, 1}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 0}, 0),
        16
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {2, 0}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 0}, 1),
        17
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {2, 0}, {1, 0}),
        17
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {2, 0}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 0}, 2),
        18
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {2, 0}, {2, 1}),
        18
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {2, 0}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 0}, 3),
        19
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {2, 0}, 3)
    );

    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 1}, 0),
        20
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {2, 1}, 0)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 1}, 1),
        21
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {2, 1}, {1, 1}),
        21
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {2, 1}, 1)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 1}, 2),
        22
    );
    BOOST_CHECK(
        !neighborhood_exists_fast(lowest_penalties.graph, {2, 1}, 2)
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index_fast(lowest_penalties.graph, {2, 1}, 3),
        23
    );
    BOOST_CHECK_EQUAL(
        neighborhood_index(lowest_penalties.graph, {2, 1}, {2, 0}),
        23
    );
    BOOST_CHECK(
        neighborhood_exists_fast(lowest_penalties.graph, {2, 1}, 3)
    );
}

BOOST_AUTO_TEST_CASE(check_lowest_pixel_penalty)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    3 2
    4
    1 2 0
    3 4 3
    )image"};
    std::istringstream right_image_content{R"image(
    P2
    3 2
    4
    0 1 2
    4 3 0
    )image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2, 10, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};

    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {0, 0}), 10, 1);
    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {1, 0}), 10, 1);
    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {2, 0}), 40, 1);

    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {0, 1}), 0, 1);
    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {1, 1}), 0, 1);
    BOOST_CHECK_CLOSE(lowest_pixel_penalty(lowest_penalties, {2, 1}), 90, 1);
}

BOOST_AUTO_TEST_CASE(check_lowest_neighborhood_penalty)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    3 2
    4
    1 2 0
    3 4 3
    )image"};
    std::istringstream right_image_content{R"image(
    P2
    3 2
    4
    0 1 2
    4 3 0
    )image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2, 1, 10};

    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{1, 0}, 0}, {1, 1})
    ] = 0.5;

    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{1, 0}, 0}, {2, 0})
    ] = -2;
    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{2, 0}, 1}, {1, 0})
    ] = -2;

    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{2, 0}, 0}, {2, 1})
    ] = -1;
    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{2, 1}, 0}, {2, 0})
    ] = -1;

    struct LowestPenalties lowest_penalties{disparity_graph};

    /**
     * This should not affect the final weights,
     * because it's modified after minimal penalties calculation.
     */
    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{1, 1}, 0}, {1, 0})
    ] = 0.5;

    BOOST_CHECK_CLOSE(
        lowest_neighborhood_penalty_fast(lowest_penalties, {0, 0}, {1, 0}),
        0,
        1
    );
    BOOST_CHECK_CLOSE(
        lowest_neighborhood_penalty_fast(lowest_penalties, {0, 0}, {0, 1}),
        0,
        1
    );

    BOOST_CHECK_CLOSE(
        lowest_neighborhood_penalty_fast(lowest_penalties, {1, 0}, {1, 1}),
        -0.5,
        1
    );

    BOOST_CHECK_CLOSE(
        lowest_neighborhood_penalty_fast(lowest_penalties, {1, 0}, {2, 0}),
        2,
        1
    );

    BOOST_CHECK_CLOSE(
        lowest_neighborhood_penalty_fast(lowest_penalties, {2, 0}, {2, 1}),
        2,
        1
    );
}

BOOST_AUTO_TEST_SUITE_END()
