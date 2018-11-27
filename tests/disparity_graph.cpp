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
#include <pgm_io.hpp>
#include <image.hpp>

#include <cmath>

BOOST_AUTO_TEST_SUITE(DisparityGraphTest)

BOOST_AUTO_TEST_CASE(check_neighborhood)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 0}, {0, 1}));
    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 0}, {1, 0}));

    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 1}, {0, 2}));

    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 1}, {0, 0}));
    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 1}, {1, 1}));

    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {0, 0}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {1, 1}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {2, 0}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {0, 2}));

    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 1}, {0, 1}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 1}, {1, 0}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 1}, {1, 2}));
}

BOOST_AUTO_TEST_CASE(check_edges_existence)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 1};

    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 0}}));
    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 1}}));

    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 1}, 0}, {{0, 0}, 0}}));
    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 1}, 1}, {{0, 0}, 0}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 1}, 2}, {{0, 1}, 1}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 1}, {{0, 1}, 2}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 2}, {{0, 1}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 1}, 0}, {{0, 1}, 2}}));
}

BOOST_AUTO_TEST_CASE(check_continuity_constraint)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 2}, {{0, 1}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 1}, 0}, {{0, 0}, 2}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 0}, 2}, {{1, 1}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 1}, 0}, {{1, 0}, 2}}));
}

BOOST_AUTO_TEST_CASE(
    check_initial_edges_penalties,
    *boost::unit_test::tolerance(0.5)
)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    // No floats? :O
    // https://www.boost.org/doc/libs/master/libs/test/doc/html/boost_test/testing_tools/extended_comparison/floating_point.html
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 0}}) - 0.0) < 0.5);
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 1}}) - 1.0) < 0.5);

    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 1}, {{0, 2}, 0}}) - 1.0) < 0.5);

    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 0}, {{0, 0}, 2}}) - 4.0) < 0.5);
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 2}, {{1, 0}, 0}}) - 4.0) < 0.5);
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 2}, {{1, 1}, 2}}) - 0.0) < 0.5);
}

BOOST_AUTO_TEST_CASE(
    check_initial_nodes_penalties,
    *boost::unit_test::tolerance(0.5)
)
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 0}, 0}) - 1.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 0}, 1}) - 0.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 0}, 2}) - 1.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 1}, 0}) - 1.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 1}, 1}) - 4.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 2}, 0}) - 0.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 0}, 0}) - 0.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 0}, 1}) - 4.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 0}, 2}) - 1.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 1}, 0}) - 0.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 1}, 1}) - 1.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{1, 2}, 0}) - 0.0) < 0.5);
}

BOOST_AUTO_TEST_CASE(check_edges_penalties, *boost::unit_test::tolerance(0.5))
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    disparity_graph.potentials[
        potential_index(disparity_graph, {{0, 0}, 0}, {0, 1})
    ] = -2.0;

    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 0}}) - -2.0) < 0.5);
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 1}}) - -1.0) < 0.5);

    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 0}, {{0, 0}, 0}}) - -2.0) < 0.5);
    BOOST_CHECK(fabs(edge_penalty(disparity_graph, {{{0, 1}, 0}, {{0, 0}, 1}}) - 1.0) < 0.5);
}

BOOST_AUTO_TEST_CASE(check_nodes_penalties, *boost::unit_test::tolerance(0.5))
{
    PGM_IO pgm_io;
    std::istringstream left_image_content{R"left_image(
    P2
    3 2
    2
    0 1 2
    2 0 1
    )left_image"};
    std::istringstream right_image_content{R"right_image(
    P2
    3 2
    2
    1 0 2
    2 0 1
    )right_image"};

    left_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image left_image{*pgm_io.get_image()};

    right_image_content >> pgm_io;
    BOOST_REQUIRE(pgm_io.get_image());
    struct Image right_image{*pgm_io.get_image()};

    struct DisparityGraph disparity_graph{left_image, right_image, 2};

    disparity_graph.potentials[
        potential_index(disparity_graph, {{0, 0}, 0}, {0, 1})
    ] = -2.0;

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 0}, 0}) - 3.0) < 0.5);
    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 0}, 1}) - 0.0) < 0.5);

    BOOST_CHECK(fabs(node_penalty(disparity_graph, {{0, 1}, 0}) - 1.0) < 0.5);
}

BOOST_AUTO_TEST_SUITE_END()
