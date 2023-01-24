//
// Created by marcel on 18.01.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_transformation/crossing_reduction.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/cover.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>

#include <type_traits>

using namespace fiction;

TEMPLATE_TEST_CASE("Node positions", "[crossing-reduction]", mockturtle::aig_network, mockturtle::mig_network,
                   mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   mockturtle::cover_network)
{
    TestType ntk{};

    const auto p1 = ntk.create_pi();
    const auto p2 = ntk.create_pi();
    const auto p3 = ntk.create_pi();
    const auto n1 = ntk.create_and(p1, p2);
    const auto n2 = ntk.create_and(p3, n1);
    const auto n3 = ntk.create_and(p1, n2);
    ntk.create_po(n3);

    const mockturtle::rank_view rank_ntk{ntk};

    REQUIRE(rank_ntk.level(rank_ntk.get_node(p1)) == 0);
    REQUIRE(rank_ntk.level(rank_ntk.get_node(p2)) == 0);
    REQUIRE(rank_ntk.level(rank_ntk.get_node(p3)) == 0);
    REQUIRE(rank_ntk.level(rank_ntk.get_node(n1)) == 1);
    REQUIRE(rank_ntk.level(rank_ntk.get_node(n2)) == 2);
    REQUIRE(rank_ntk.level(rank_ntk.get_node(n3)) == 3);

    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p1)).y == 0);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p1)).x == 0);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p2)).y == 0);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p2)).x == 1);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p3)).y == 0);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(p3)).x == 2);

    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n1)).y == 1);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n1)).x == 0);

    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n2)).y == 2);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n2)).x == 0);

    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n3)).y == 3);
    CHECK(detail::get_node_pos(rank_ntk, rank_ntk.get_node(n3)).x == 0);
}

TEMPLATE_TEST_CASE("Empty network straight-line crossing number", "[crossing-reduction]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   mockturtle::cover_network)
{
    const mockturtle::rank_view<TestType> rank_ntk{};

    CHECK(detail::number_of_crossings(rank_ntk) == 0);
}

TEMPLATE_TEST_CASE("Straight-line crossing number (simple network without crossings)", "[crossing-reduction]",
                   mockturtle::aig_network, mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network,
                   mockturtle::klut_network, mockturtle::cover_network)
{
    TestType ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto a1 = ntk.create_and(x1, x2);
    ntk.create_po(a1);

    mockturtle::rank_view<TestType> rank_ntk{ntk};

    const auto n_x1 = rank_ntk.get_node(x1);
    const auto n_x2 = rank_ntk.get_node(x2);
    const auto n_a1 = rank_ntk.get_node(a1);

    // validate non-crossings
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    // validate non-crossings with swapped inputs
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));

    // count crossings
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 0, 1) == 0);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 0) == 0);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 0);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 0);
    CHECK(detail::number_of_crossings(rank_ntk) == 0);

    // there should be no crossings in the network no matter the order of the inputs
    rank_ntk.swap(n_x1, n_x2);

    // validate non-crossings
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    // validate non-crossings with swapped inputs
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));

    // count crossings
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 0, 1) == 0);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 0) == 0);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 0);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 0);
    CHECK(detail::number_of_crossings(rank_ntk) == 0);
}

TEMPLATE_TEST_CASE("Straight-line crossing number (simple network with 1 crossing)", "[crossing-reduction]",
                   mockturtle::aig_network, mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network,
                   mockturtle::klut_network, mockturtle::cover_network)
{
    TestType ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(x1, x2);
    ntk.create_po(a1);
    ntk.create_po(o1);

    mockturtle::rank_view<TestType> rank_ntk{ntk};

    const auto n_x1 = rank_ntk.get_node(x1);
    const auto n_x2 = rank_ntk.get_node(x2);
    const auto n_a1 = rank_ntk.get_node(a1);
    const auto n_o1 = rank_ntk.get_node(o1);

    // validate non-crossings
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    // validate non-crossings with swapped inputs
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_a1}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_a1}));

    // validate crossings
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_o1}, {n_x2, n_a1}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_a1}, {n_x1, n_o1}));

    // count crossings
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 0, 1) == 1);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 0) == 1);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 1);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 1);
    CHECK(detail::number_of_crossings(rank_ntk) == 1);

    // there should be 1 crossing in the network no matter the order of the inputs
    rank_ntk.swap(n_x1, n_x2);

    // validate crossings
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_a1}, {n_x2, n_o1}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_o1}, {n_x1, n_a1}));

    // count crossings
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 0, 1) == 1);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 0) == 1);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 1);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 1);
    CHECK(detail::number_of_crossings(rank_ntk) == 1);

    const auto number_of_crossings_before_reduction = detail::number_of_crossings(rank_ntk);

    const auto reduced = crossing_reduction(rank_ntk);

    const auto number_of_crossings_after_reduction = detail::number_of_crossings(reduced);

    CHECK(number_of_crossings_after_reduction <= number_of_crossings_before_reduction);
}

TEMPLATE_TEST_CASE("Three layer lattice straight-line crossing number", "[crossing-reduction]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   mockturtle::cover_network)
{
    mockturtle::rank_view<TestType> rank_ntk{};

    // rank 1
    const auto x1 = rank_ntk.create_pi();
    const auto x2 = rank_ntk.create_pi();
    const auto x3 = rank_ntk.create_pi();

    // rank 2
    const auto x4 = rank_ntk.create_and(x1, x2);
    const auto x5 = rank_ntk.create_and(x1, x3);
    const auto x6 = rank_ntk.create_and(x2, x3);

    // rank 3
    const auto x7 = rank_ntk.create_and(x4, x5);
    const auto x8 = rank_ntk.create_and(x4, x6);
    const auto x9 = rank_ntk.create_and(x5, x6);

    // outputs
    rank_ntk.create_po(x7);
    rank_ntk.create_po(x8);
    rank_ntk.create_po(x9);

    // nodes of first rank
    const auto n_x1 = rank_ntk.get_node(x1);
    const auto n_x2 = rank_ntk.get_node(x2);
    const auto n_x3 = rank_ntk.get_node(x3);

    // nodes of second rank
    const auto n_x4 = rank_ntk.get_node(x4);
    const auto n_x5 = rank_ntk.get_node(x5);
    const auto n_x6 = rank_ntk.get_node(x6);

    // nodes of third rank
    const auto n_x7 = rank_ntk.get_node(x7);
    const auto n_x8 = rank_ntk.get_node(x8);
    const auto n_x9 = rank_ntk.get_node(x9);

    // validate rank positions
    REQUIRE(rank_ntk.rank_position(n_x1) == 0);
    REQUIRE(rank_ntk.rank_position(n_x2) == 1);
    REQUIRE(rank_ntk.rank_position(n_x3) == 2);
    REQUIRE(rank_ntk.rank_position(n_x4) == 0);
    REQUIRE(rank_ntk.rank_position(n_x5) == 1);
    REQUIRE(rank_ntk.rank_position(n_x6) == 2);
    REQUIRE(rank_ntk.rank_position(n_x7) == 0);
    REQUIRE(rank_ntk.rank_position(n_x8) == 1);
    REQUIRE(rank_ntk.rank_position(n_x9) == 2);

    // check crossings
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x5}, {n_x2, n_x4}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x3, n_x5}, {n_x2, n_x6}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x4, n_x8}, {n_x5, n_x7}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x6, n_x8}, {n_x5, n_x9}));
    // check crossings with swapped inputs
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x4}, {n_x1, n_x5}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x6}, {n_x3, n_x5}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x5, n_x7}, {n_x4, n_x8}));
    CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x5, n_x9}, {n_x6, n_x8}));

    // validate non-crossings
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x1, n_x5}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x2, n_x6}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x2, n_x4}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x1, n_x4}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x3, n_x6}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x3, n_x5}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x4}, {n_x4, n_x8}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x4}, {n_x5, n_x7}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x4}, {n_x6, n_x8}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x4}, {n_x5, n_x9}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x3, n_x5}, {n_x4, n_x8}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x3, n_x5}, {n_x5, n_x7}));
    CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x3, n_x5}, {n_x6, n_x8}));

    // count crossings
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 0, 1) == 2);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 0) == 2);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 1, 2) == 2);
    CHECK(detail::number_of_crossings_between_ranks(rank_ntk, 2, 1) == 2);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 2);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 4);
    CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 2) == 2);
    CHECK(detail::number_of_crossings(rank_ntk) == 4);

    const auto number_of_crossings_before_reduction = detail::number_of_crossings(rank_ntk);

    const auto reduced = crossing_reduction(rank_ntk);

    const auto number_of_crossings_after_reduction = detail::number_of_crossings(reduced);

    CHECK(number_of_crossings_after_reduction <= number_of_crossings_before_reduction);
}

TEMPLATE_TEST_CASE("Straight-line crossing number with fanins in different levels", "[crossing-reduction]",
                   mockturtle::aig_network, mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network,
                   mockturtle::klut_network, mockturtle::cover_network)
{
    mockturtle::rank_view<TestType> rank_ntk{};

    SECTION("Asymmetrically stacked nodes")
    {
        // rank 1
        const auto x1 = rank_ntk.create_pi();
        const auto x2 = rank_ntk.create_pi();
        const auto x3 = rank_ntk.create_pi();

        // rank 2
        const auto x4 = rank_ntk.create_and(x2, x3);

        // rank 3
        const auto x5 = rank_ntk.create_and(x1, x4);

        // rank 4
        const auto x6 = rank_ntk.create_and(x2, x5);

        // output
        rank_ntk.create_po(x6);

        // nodes
        const auto n_x1 = rank_ntk.get_node(x1);
        const auto n_x2 = rank_ntk.get_node(x2);
        const auto n_x3 = rank_ntk.get_node(x3);
        const auto n_x4 = rank_ntk.get_node(x4);
        const auto n_x5 = rank_ntk.get_node(x5);
        const auto n_x6 = rank_ntk.get_node(x6);

        // validate rank positions
        REQUIRE(rank_ntk.rank_position(n_x1) == 0);
        REQUIRE(rank_ntk.rank_position(n_x2) == 1);
        REQUIRE(rank_ntk.rank_position(n_x3) == 2);
        REQUIRE(rank_ntk.rank_position(n_x4) == 0);
        REQUIRE(rank_ntk.rank_position(n_x5) == 0);
        REQUIRE(rank_ntk.rank_position(n_x6) == 0);

        // validate non-crossings across ranks
        CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x5}, {n_x2, n_x6}));
        CHECK(!detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x6}, {n_x1, n_x5}));

        // no crossings since parallel lines are not considered crossings
        CHECK(detail::number_of_crossings(rank_ntk) == 0);
    }
    SECTION("Symmetrically stacked nodes")
    {
        // rank 1
        const auto x1 = rank_ntk.create_pi();
        const auto x2 = rank_ntk.create_pi();

        // rank 2
        const auto x3 = rank_ntk.create_and(x1, x2);
        const auto x4 = rank_ntk.create_or(x1, x2);

        // rank 3
        const auto x5 = rank_ntk.create_and(x3, x2);
        const auto x6 = rank_ntk.create_and(x4, x1);

        // outputs
        rank_ntk.create_po(x5);
        rank_ntk.create_po(x6);

        // nodes
        const auto n_x1 = rank_ntk.get_node(x1);
        const auto n_x2 = rank_ntk.get_node(x2);
        const auto n_x3 = rank_ntk.get_node(x3);
        const auto n_x4 = rank_ntk.get_node(x4);
        const auto n_x5 = rank_ntk.get_node(x5);
        const auto n_x6 = rank_ntk.get_node(x6);

        // validate rank positions
        REQUIRE(rank_ntk.rank_position(n_x1) == 0);
        REQUIRE(rank_ntk.rank_position(n_x2) == 1);
        REQUIRE(rank_ntk.rank_position(n_x3) == 0);
        REQUIRE(rank_ntk.rank_position(n_x4) == 1);
        REQUIRE(rank_ntk.rank_position(n_x5) == 0);
        REQUIRE(rank_ntk.rank_position(n_x6) == 1);

        // check crossings (adjacent)
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x2, n_x3}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x3}, {n_x1, n_x4}));
        // check crossings (non-adjacent)
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x6}, {n_x2, n_x3}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x3}, {n_x1, n_x6}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x4}, {n_x2, n_x5}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x5}, {n_x1, n_x4}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x1, n_x6}, {n_x2, n_x5}));
        CHECK(detail::is_straight_line_crossing(rank_ntk, {n_x2, n_x5}, {n_x1, n_x6}));

        // validate number of crossings
        CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 0) == 1);
        CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 1) == 1);
        CHECK(detail::number_of_crossings_with_adjacent_ranks(rank_ntk, 2) == 0);

        // only 1 crossing is caused by adjacent ranks
        CHECK(detail::number_of_crossings(rank_ntk) == 1);
    }
}

TEMPLATE_TEST_CASE("Crossing reduction", "[crossing-reduction]", mockturtle::aig_network, mockturtle::mig_network,
                   mockturtle::xag_network, mockturtle::xmg_network)
{
    const auto ntk = blueprints::maj4_network<TestType>();

    const auto number_of_crossings_before_reduction = detail::number_of_crossings(mockturtle::rank_view{ntk});

    const auto reduced = crossing_reduction(ntk);

    const auto number_of_crossings_after_reduction = detail::number_of_crossings(reduced);

    CHECK(number_of_crossings_after_reduction <= number_of_crossings_before_reduction);
    CHECK(reduced.size() == ntk.size());
}
