//
// Created by marcel on 16.01.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/placement_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/traits.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Traits and construction", "[placement-layout]",
    (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>))
{
    using Ntk = technology_network;
    const Ntk ntk{};

    REQUIRE(is_gate_level_layout_v<TestType>);
    CHECK(!has_initialize_random_placement_v<TestType>);
    CHECK(!has_random_tile_v<TestType>);
    CHECK(!has_random_node_v<TestType>);

    const TestType lyt{};

    using place_layout = placement_layout<TestType, Ntk>;
    CHECK(is_gate_level_layout_v<place_layout>);
    CHECK(has_initialize_random_placement_v<place_layout>);
    CHECK(has_random_tile_v<place_layout>);
    CHECK(has_random_node_v<place_layout>);

    const place_layout place_lyt_from_lyt{lyt, ntk};

    using place_place_layout = placement_layout<place_layout, Ntk>;
    CHECK(is_gate_level_layout_v<place_place_layout>);
    CHECK(has_initialize_random_placement_v<place_place_layout>);
    CHECK(has_random_tile_v<place_place_layout>);
    CHECK(has_random_node_v<place_place_layout>);
}

TEMPLATE_TEST_CASE(
    "Random initial placement", "[placement-layout]",
    (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>))
{
    using Ntk      = technology_network;
    const auto ntk = blueprints::unbalanced_and_inv_network<Ntk>();

    REQUIRE(ntk.size() == 6u);

    const TestType lyt{{5, 5}};

    placement_layout place_lyt{lyt, ntk};
    place_lyt.initialize_random_placement();

    place_lyt.foreach_placed_node([&place_lyt](const auto&, const auto& c) { CHECK(place_lyt.is_within_bounds(c)); });
}

TEMPLATE_TEST_CASE(
    "Random tile sampling", "[placement-layout]",
    (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>))
{
    using Ntk = technology_network;
    const Ntk ntk{};

    const TestType lyt{{10, 20}};

    const placement_layout place_lyt{lyt, ntk};

    // sample 100 tiles and check if they are all within the layout bounds
    for (auto i = 0u; i < 100u; ++i)
    {
        const auto t = place_lyt.random_tile();
        CHECK(place_lyt.is_within_bounds(t));
    }
}

TEMPLATE_TEST_CASE(
    "Random node sampling", "[placement-layout]",
    (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>))
{
    using Ntk      = technology_network;
    const auto ntk = blueprints::full_adder_network<Ntk>();

    const TestType lyt{{10, 20}};

    const placement_layout place_lyt{lyt, ntk};

    // sample 100 nodes and check if they are all contained within the network
    for (auto i = 0u; i < 100u; ++i)
    {
        const auto n = place_lyt.random_node();
        CHECK(!ntk.is_constant(n));
        CHECK((ntk.is_pi(n) || ntk.is_function(n)));
        CHECK(n < ntk.size());
    }
}

TEMPLATE_TEST_CASE(
    "Node swapping", "[placement-layout]", (gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (gate_level_layout<synchronization_element_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>),
    (gate_level_layout<clocked_layout<hexagonal_layout<offset::ucoord_t>>>))
{
    using Ntk      = technology_network;
    const auto ntk = blueprints::full_adder_network<Ntk>();

    const TestType lyt{{10, 20}};

    placement_layout place_lyt{lyt, ntk};

    place_lyt.initialize_random_placement();

    for (auto i = 0u; i < 100u; ++i)
    {
        const auto n = place_lyt.random_node();
        const auto t = place_lyt.random_tile();

        const auto n_old_t = place_lyt.get_node_tile(n);
        const auto t_old_n = place_lyt.get_tile_node(t);

        place_lyt.swap_node_and_tile(n, t);

        CHECK(place_lyt.get_node_tile(n) == t);
        CHECK(place_lyt.get_tile_node(t) == n);
        CHECK(place_lyt.get_node_tile(t_old_n) == n_old_t);
        CHECK(place_lyt.get_tile_node(n_old_t) == t_old_n);
    }
}
