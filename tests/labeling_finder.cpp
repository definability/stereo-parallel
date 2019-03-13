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

#include <labeling_finder.hpp>
#include <constraint_graph.hpp>
#include <disparity_graph.hpp>
#include <image.hpp>
#include <lowest_penalties.hpp>
#include <pgm_io.hpp>

BOOST_AUTO_TEST_SUITE(LabelingFinder)

BOOST_AUTO_TEST_CASE(check_black_images)
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

    struct DisparityGraph disparity_graph{image, image, 2, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};
    auto pixel_penalties = fetch_pixel_available_penalties(
        disparity_graph,
        {0, 0},
        0
    );
    BOOST_CHECK_EQUAL(pixel_penalties.size(), 1);
    BOOST_CHECK_CLOSE(pixel_penalties[0], 0, 1);

    auto edge_penalties = fetch_edge_available_penalties(
        disparity_graph,
        {{{0, 0}, 0}, {{0, 1}, 0}},
        0
    );
    BOOST_CHECK_EQUAL(edge_penalties.size(), 2);
    BOOST_CHECK_CLOSE(edge_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(edge_penalties[1], 1, 1);

    auto available_penalties = fetch_available_penalties(lowest_penalties);
    BOOST_CHECK_EQUAL(available_penalties.size(), 2);
    BOOST_CHECK_CLOSE(available_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(available_penalties[1], 1, 1);

    FLOAT threshold = calculate_minimal_consistent_threshold(
        lowest_penalties,
        disparity_graph,
        available_penalties
    );
    BOOST_CHECK_CLOSE(threshold, 0, 1);

    struct ConstraintGraph constraint_graph{
        disparity_graph,
        lowest_penalties,
        threshold
    };
    BOOST_TEST(solve_csp(&constraint_graph));
    BOOST_CHECK_EQUAL(find_labeling(&constraint_graph), &constraint_graph);
}

BOOST_AUTO_TEST_CASE(check_equal_images)
{
    PGM_IO pgm_io;
    std::istringstream image_content{R"image(
    P2
    3 2
    256
    0   127 256
    256 127 0
    )image"};

    image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{image, image, 3, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};

    auto pixel_penalties = fetch_pixel_available_penalties(
        disparity_graph,
        {1, 0},
        0
    );
    BOOST_CHECK_EQUAL(pixel_penalties.size(), 2);
    BOOST_CHECK_CLOSE(pixel_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(pixel_penalties[1], 129 * 129, 1);

    auto edge_penalties = fetch_edge_available_penalties(
        disparity_graph,
        {{{0, 0}, 0}, {{0, 1}, 0}},
        0
    );
    BOOST_CHECK_EQUAL(edge_penalties.size(), 3);
    BOOST_CHECK_CLOSE(edge_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(edge_penalties[1], 1, 1);
    BOOST_CHECK_CLOSE(edge_penalties[2], 4, 1);

    auto available_penalties = fetch_available_penalties(lowest_penalties);
    BOOST_CHECK_EQUAL(available_penalties.size(), 6);
    BOOST_CHECK_CLOSE(available_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(available_penalties[1], 1, 1);
    BOOST_CHECK_CLOSE(available_penalties[2], 4, 1);
    BOOST_CHECK_CLOSE(available_penalties[3], 127 * 127, 1);
    BOOST_CHECK_CLOSE(available_penalties[4], 129 * 129, 1);
    BOOST_CHECK_CLOSE(available_penalties[5], 256 * 256, 1);

    FLOAT threshold = calculate_minimal_consistent_threshold(
        lowest_penalties,
        disparity_graph,
        available_penalties
    );
    BOOST_CHECK_CLOSE(threshold, 0, 1);

    struct ConstraintGraph constraint_graph{
        disparity_graph,
        lowest_penalties,
        threshold
    };
    BOOST_TEST(solve_csp(&constraint_graph));
    BOOST_CHECK_EQUAL(find_labeling(&constraint_graph), &constraint_graph);
}

BOOST_AUTO_TEST_CASE(basic_check)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    3 2
    10
    4 5 10
    0 0 0
    )image"};
    std::istringstream right_image_content{R"image(
    P2
    3 2
    10
    10 7 0
    0 0 0
    )image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};

    auto pixel_penalties = fetch_pixel_available_penalties(
        disparity_graph,
        {0, 0},
        0
    );
    BOOST_CHECK_EQUAL(pixel_penalties.size(), 3);
    BOOST_CHECK_CLOSE(pixel_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(pixel_penalties[1], 25, 1);
    BOOST_CHECK_CLOSE(pixel_penalties[2], 36, 1);

    auto edge_penalties = fetch_edge_available_penalties(
        disparity_graph,
        {{{0, 0}, 0}, {{0, 1}, 0}},
        0
    );
    BOOST_CHECK_EQUAL(edge_penalties.size(), 3);
    BOOST_CHECK_CLOSE(edge_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(edge_penalties[1], 1, 1);
    BOOST_CHECK_CLOSE(edge_penalties[2], 4, 1);

    auto available_penalties = fetch_available_penalties(lowest_penalties);
    BOOST_CHECK_EQUAL(available_penalties.size(), 6);
    BOOST_CHECK_CLOSE(available_penalties[0], 0, 1);
    BOOST_CHECK_CLOSE(available_penalties[1], 1, 1);
    BOOST_CHECK_CLOSE(available_penalties[2], 4, 1);
    BOOST_CHECK_CLOSE(available_penalties[3], 5, 1);
    BOOST_CHECK_CLOSE(available_penalties[4], 25, 1);
    BOOST_CHECK_CLOSE(available_penalties[5], 36, 1);

    FLOAT threshold = calculate_minimal_consistent_threshold(
        lowest_penalties,
        disparity_graph,
        available_penalties
    );
    BOOST_CHECK_CLOSE(threshold, 5, 1);

    struct ConstraintGraph constraint_graph{
        disparity_graph,
        lowest_penalties,
        threshold
    };
    BOOST_TEST(solve_csp(&constraint_graph));
    BOOST_CHECK_EQUAL(find_labeling(&constraint_graph), &constraint_graph);
}

BOOST_AUTO_TEST_SUITE_END()
