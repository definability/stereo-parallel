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
#include <pgm_io.hpp>

BOOST_AUTO_TEST_SUITE(DisparityGraphTest)

BOOST_AUTO_TEST_CASE(check_nodes_existence)
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

    for (ULONG y = 0; y < 2; ++y)
    {
        BOOST_CHECK(node_exists(disparity_graph, {{0, y}, 0}));
        BOOST_CHECK(node_exists(disparity_graph, {{1, y}, 0}));
        BOOST_CHECK(node_exists(disparity_graph, {{2, y}, 0}));

        BOOST_CHECK(node_exists(disparity_graph, {{0, y}, 1}));
        BOOST_CHECK(node_exists(disparity_graph, {{1, y}, 1}));

        BOOST_CHECK(!node_exists(disparity_graph, {{0, y}, 2}));
        BOOST_CHECK(!node_exists(disparity_graph, {{1, y}, 2}));

        BOOST_CHECK(!node_exists(disparity_graph, {{2, y}, 1}));
    }
    BOOST_CHECK(!node_exists(disparity_graph, {{0, 2}, 0}));
    BOOST_CHECK(!node_exists(disparity_graph, {{3, 0}, 2}));
    BOOST_CHECK(!node_exists(disparity_graph, {{3, 2}, 2}));
}

BOOST_AUTO_TEST_CASE(check_reparametrization_indexing)
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

    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 0}, 0), 0);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 0}, 0), 1);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 1}, 0), 2);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 2}, 0), 4);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 2}, 0), 5);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 0}, 1), 6);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 0}, 1), 7);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 1}, 1), 8);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 1}, 1), 9);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 2}, 1), 10);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 2}, 1), 11);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 0}, 2), 12);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 0}, 0}, 3), 18);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{0, 1}, 2}, 3), 23);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{1, 0}, 0}, 0), 24);
    BOOST_CHECK_EQUAL(reparametrization_index_fast(disparity_graph, {{1, 1}, 2}, 3), 47);
}

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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 1};

    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 0}, {1, 0}));
    BOOST_CHECK(neighborhood_exists(disparity_graph, {0, 0}, {0, 1}));

    BOOST_CHECK(neighborhood_exists(disparity_graph, {1, 0}, {2, 0}));

    BOOST_CHECK(neighborhood_exists(disparity_graph, {1, 0}, {0, 0}));
    BOOST_CHECK(neighborhood_exists(disparity_graph, {1, 0}, {1, 1}));

    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {0, 0}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {1, 1}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {0, 2}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {0, 0}, {2, 0}));

    BOOST_CHECK(!neighborhood_exists(disparity_graph, {1, 0}, {1, 0}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {1, 0}, {0, 1}));
    BOOST_CHECK(!neighborhood_exists(disparity_graph, {1, 0}, {2, 1}));
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

    struct DisparityGraph disparity_graph{left_image, right_image, 2, 1, 1};

    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 0}}));
    BOOST_CHECK(edge_exists(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 1}}));

    BOOST_CHECK(edge_exists(disparity_graph, {{{1, 0}, 0}, {{0, 0}, 0}}));
    BOOST_CHECK(edge_exists(disparity_graph, {{{1, 0}, 1}, {{0, 0}, 0}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 0}, 2}, {{1, 0}, 1}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 1}, {{1, 0}, 2}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 2}, {{1, 0}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 0}, 0}, {{1, 0}, 2}}));
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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 1};

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 0}, 2}, {{1, 0}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 0}, 0}, {{0, 0}, 2}}));

    BOOST_CHECK(!edge_exists(disparity_graph, {{{0, 1}, 2}, {{1, 1}, 0}}));
    BOOST_CHECK(!edge_exists(disparity_graph, {{{1, 1}, 0}, {{0, 1}, 2}}));
}

BOOST_AUTO_TEST_CASE(check_initial_edges_penalties)
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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 1};

    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 0}}),
        0.0,
        0.5
    );
    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{0, 0}, 0}, {{0, 1}, 1}}),
        1.0,
        0.5
    );

    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 1}, {{2, 0}, 0}}),
        1.0,
        0.5
    );

    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 0}, {{0, 0}, 2}}),
        4.0,
        0.5
    );
    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 2}, {{0, 1}, 0}}),
        4.0,
        0.5
    );
    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 2}, {{1, 1}, 2}}),
        0.0,
        0.5
    );
}

BOOST_AUTO_TEST_CASE(check_initial_nodes_penalties)
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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 1};

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 0}), 1.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 1}), 0.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 2}), 1.0, 0.5);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 0}, 0}), 1.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 0}, 1}), 4.0, 0.5);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{2, 0}, 0}), 0.0, 0.5);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 0}), 0.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 1}), 4.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 1}, 2}), 1.0, 0.5);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 1}, 0}), 0.0, 0.5);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 1}, 1}), 1.0, 0.5);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{2, 1}, 0}), 0.0, 0.5);
}

BOOST_AUTO_TEST_CASE(check_edges_penalties)
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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 1, 10};

    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{0, 0}, 0}, {1, 0})
    ] = -2;

    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 0}}),
        2,
        1
    );
    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{0, 0}, 0}, {{1, 0}, 1}}),
        12,
        1
    );

    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 0}, {{0, 0}, 0}}),
        2,
        1
    );
    BOOST_CHECK_CLOSE(
        edge_penalty(disparity_graph, {{{1, 0}, 0}, {{0, 0}, 1}}),
        10,
        1
    );
}

BOOST_AUTO_TEST_CASE(check_nodes_penalties)
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

    struct DisparityGraph disparity_graph{left_image, right_image, 3, 10, 1};

    disparity_graph.reparametrization[
        reparametrization_index(disparity_graph, {{0, 0}, 0}, {1, 0})
    ] = -2;

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 0}), 8, 1);
    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{0, 0}, 1}), 0, 1);

    BOOST_CHECK_CLOSE(node_penalty(disparity_graph, {{1, 0}, 0}), 10, 1);
}

BOOST_AUTO_TEST_SUITE_END()
