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

#include <constraint_graph.hpp>
#include <disparity_graph.hpp>
#include <image.hpp>
#include <indexing.hpp>
#include <lowest_penalties.hpp>
#include <pgm_io.hpp>

BOOST_AUTO_TEST_SUITE(ConstraintGraphTest)

BOOST_AUTO_TEST_CASE(check_nodes_indexing)
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
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 1};

    BOOST_CHECK_EQUAL(node_index(constraint_graph.disparity_graph, {{0, 0}, 0}), 0);
    BOOST_CHECK_EQUAL(node_index(constraint_graph.disparity_graph, {{0, 0}, 2}), 2);
    BOOST_CHECK_EQUAL(node_index(constraint_graph.disparity_graph, {{0, 1}, 0}), 3);
    BOOST_CHECK_EQUAL(node_index(constraint_graph.disparity_graph, {{0, 1}, 2}), 5);
    BOOST_CHECK_EQUAL(node_index(constraint_graph.disparity_graph, {{1, 0}, 0}), 6);
}

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

    struct DisparityGraph disparity_graph{image, image, 3, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 1};
    BOOST_CHECK_CLOSE(constraint_graph.threshold, 1, 1);

    struct Node node{{0, 0}, 0};
    for (
        node.pixel.x = 0;
        node.pixel.x < disparity_graph.right.width;
        ++node.pixel.x
    )
    {
        for (
            node.pixel.y = 0;
            node.pixel.y < disparity_graph.right.height;
            ++node.pixel.y
        )
        {
            for (
                node.disparity = 0;
                node.disparity < disparity_graph.disparity_levels;
                ++node.disparity
            )
            {
                BOOST_CHECK_EQUAL(
                    is_node_available(constraint_graph, node),
                    node.pixel.x + node.disparity
                    < disparity_graph.left.width
                );
            }
        }
    }
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
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 128 * 128};
    BOOST_CHECK_CLOSE(constraint_graph.threshold, 128 * 128, 1);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 0}), 0, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 0}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 1}), 127 * 127, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 1}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 2}), 256 * 256, 1);
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 0}, 2}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 0}), 0, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 1}, 0}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 1}), 129 * 129, 1);
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 1}, 1}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 2}), 256 * 256, 1);
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 1}, 2}));
}

BOOST_AUTO_TEST_CASE(check_disparity_loop)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    3 2
    10
    8 7 10
    0 0 0
    )image"};
    std::istringstream right_image_content{R"image(
    P2
    3 2
    10
    10 0 0
    0 0 0
    )image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 15};
    BOOST_CHECK_CLOSE(constraint_graph.threshold, 15, 1);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 0}), 4, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 0}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 1}), 9, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 1}));
}

/**
 * When minimal node penalty
 * is calculated with disparity left from the previous iteration,
 * it's possible that on the last column the value will be wrong,
 * because, if disparity is more than zero,
 * this will cause index to go out of bounds
 * of penalties of nodes of the row.
 * So, the penalty of the cell with the one from the next row
 * will be initial minimal value.
 * This may lead to bad bugs that are really hard to find.
 */
BOOST_AUTO_TEST_CASE(check_minimal_node_value_calculation)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    3 2
    1
    0 0 1
    0 0 0
    )image"};
    std::istringstream right_image_content{R"image(
    P2
    3 2
    1
    0 0 0
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
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 0.5};
    BOOST_CHECK_CLOSE(constraint_graph.threshold, 0.5, 1);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{2, 0}, 0}), 1, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{2, 0}, 0}));
}

BOOST_AUTO_TEST_CASE(check_removal)
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
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 16};

    BOOST_CHECK_CLOSE(constraint_graph.threshold, 16, 1);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 0}), 36, 1);
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 0}, 0}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 1}), 25, 1);
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 0}, 1}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 2}), 0, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 2}));

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 0}, 0}), 4, 1);
    BOOST_CHECK(is_node_available(constraint_graph, {{1, 0}, 0}));

    BOOST_CHECK(solve_csp(&constraint_graph));

    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 0}, 0}));
    BOOST_CHECK(!is_node_available(constraint_graph, {{0, 0}, 1}));
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 2}));

    BOOST_CHECK(!is_node_available(constraint_graph, {{1, 0}, 0}));
    BOOST_CHECK(is_node_available(constraint_graph, {{1, 0}, 1}));

    BOOST_CHECK(!is_edge_available(constraint_graph, {{{0, 0}, 0}, {{1, 0}, 0}}));
    BOOST_CHECK(!is_edge_available(constraint_graph, {{{0, 0}, 1}, {{1, 0}, 0}}));
    BOOST_CHECK(!is_edge_available(constraint_graph, {{{0, 0}, 2}, {{1, 0}, 0}}));
}

BOOST_AUTO_TEST_CASE(check_unconstrained_disparities)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"image(
    P2
    4 3
    1
    0 0 0 0
    0 0 0 0
    0 0 0 0
    )image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, left_image, 4, 1, 1};
    struct LowestPenalties lowest_penalties{disparity_graph};
    struct ConstraintGraph constraint_graph{disparity_graph, lowest_penalties, 9};
    BOOST_CHECK_CLOSE(constraint_graph.threshold, 9, 1);
    BOOST_CHECK(solve_csp(&constraint_graph));

    make_node_unavailable(&constraint_graph, {{1, 0}, 0});

    make_node_unavailable(&constraint_graph, {{1, 1}, 1});
    make_node_unavailable(&constraint_graph, {{1, 1}, 2});

    make_node_unavailable(&constraint_graph, {{0, 0}, 1});
    make_node_unavailable(&constraint_graph, {{0, 0}, 2});
    make_node_unavailable(&constraint_graph, {{0, 0}, 3});

    make_node_unavailable(&constraint_graph, {{1, 2}, 0});
    make_node_unavailable(&constraint_graph, {{1, 2}, 1});

    BOOST_CHECK(solve_csp(&constraint_graph));
    BOOST_CHECK(is_node_available(constraint_graph, {{0, 0}, 0}));
    BOOST_CHECK(is_node_available(constraint_graph, {{1, 0}, 2}));
    BOOST_CHECK(is_node_available(constraint_graph, {{2, 0}, 1}));
    BOOST_CHECK(is_node_available(constraint_graph, {{1, 1}, 0}));
}

BOOST_AUTO_TEST_SUITE_END()
