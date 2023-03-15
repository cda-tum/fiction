//
// Created by marcel on 02.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/traits.hpp>

using namespace fiction;

TEST_CASE("Traits and construction", "[obstruction-layout]")
{
    SECTION("Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        REQUIRE(is_coordinate_layout_v<layout>);
        CHECK(!has_is_obstructed_coordinate_v<layout>);
        CHECK(!has_is_obstructed_connection_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_coordinate_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_coordinate_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        REQUIRE(is_gate_level_layout_v<layout>);
        CHECK(!has_is_obstructed_coordinate_v<layout>);
        CHECK(!has_is_obstructed_connection_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_coordinate_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_coordinate_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
    SECTION("Cell-level layout")
    {
        using layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        REQUIRE(is_cell_level_layout_v<layout>);
        CHECK(!has_is_obstructed_coordinate_v<layout>);
        CHECK(!has_is_obstructed_connection_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_coordinate_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_coordinate_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_coordinate_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_connection_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
}

TEST_CASE("Coordinate obstruction", "[obstruction-layout]")
{
    SECTION("Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        const layout lyt{{4, 4}};

        obstruction_layout obstr_lyt{lyt};

        obstr_lyt.foreach_coordinate([&obstr_lyt](const auto& c) { CHECK(!obstr_lyt.is_obstructed_coordinate(c)); });

        // add artificial obstruction
        obstr_lyt.obstruct_coordinate({0, 0});
        obstr_lyt.obstruct_coordinate({1, 0});
        obstr_lyt.obstruct_coordinate({2, 0});
        obstr_lyt.obstruct_coordinate({3, 0});
        obstr_lyt.obstruct_coordinate({4, 0});

        CHECK(obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({4, 0}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_coordinate({0, 0});
        obstr_lyt.clear_obstructed_coordinate({1, 0});
        obstr_lyt.clear_obstructed_coordinate({2, 0});

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({4, 0}));

        // remove all obstructions
        obstr_lyt.clear_obstructed_coordinates();

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 0}));
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::xor_maj_gate_layout<layout>();

        obstruction_layout obstr_lyt{lyt};

        CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({0, 0}));

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 2}));

        // add artificial obstructions
        obstr_lyt.obstruct_coordinate({0, 1});
        obstr_lyt.obstruct_coordinate({1, 2});
        obstr_lyt.obstruct_coordinate({3, 2});

        CHECK(obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 2}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 2}));

        // remove some manually added obstructions
        obstr_lyt.clear_obstructed_coordinate({0, 1});
        obstr_lyt.clear_obstructed_coordinate({1, 2});

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 2}));

        // removing an obstruction that was not added manually should not change anything
        obstr_lyt.clear_obstructed_coordinate({1, 1});
        obstr_lyt.clear_obstructed_coordinate({2, 0});
        obstr_lyt.clear_obstructed_coordinate({3, 1});

        CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 1}));

        // remove all obstructions
        obstr_lyt.clear_obstructed_coordinates();

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 2}));
    }
    SECTION("Cell-level layout")
    {
        using layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::single_layer_qca_and_gate<layout>();

        obstruction_layout obstr_lyt{lyt};

        CHECK(obstr_lyt.is_obstructed_coordinate({0, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 4}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 3}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({3, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({4, 2}));

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 4}));

        // add artificial obstructions
        obstr_lyt.obstruct_coordinate({0, 0});
        obstr_lyt.obstruct_coordinate({0, 1});
        obstr_lyt.obstruct_coordinate({0, 3});
        obstr_lyt.obstruct_coordinate({0, 4});
        obstr_lyt.obstruct_coordinate({1, 0});
        obstr_lyt.obstruct_coordinate({1, 1});
        obstr_lyt.obstruct_coordinate({1, 3});
        obstr_lyt.obstruct_coordinate({1, 4});

        CHECK(obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({0, 3}));
        CHECK(obstr_lyt.is_obstructed_coordinate({0, 4}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 3}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 4}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_coordinate({0, 0});
        obstr_lyt.clear_obstructed_coordinate({0, 1});
        obstr_lyt.clear_obstructed_coordinate({0, 3});
        obstr_lyt.clear_obstructed_coordinate({0, 4});

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 4}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 3}));
        CHECK(obstr_lyt.is_obstructed_coordinate({1, 4}));

        // removing an obstruction that was not added manually should not change anything
        obstr_lyt.clear_obstructed_coordinate({0, 2});
        obstr_lyt.clear_obstructed_coordinate({2, 4});
        obstr_lyt.clear_obstructed_coordinate({2, 0});
        obstr_lyt.clear_obstructed_coordinate({2, 1});
        obstr_lyt.clear_obstructed_coordinate({2, 2});

        CHECK(obstr_lyt.is_obstructed_coordinate({0, 2}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 4}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 1}));
        CHECK(obstr_lyt.is_obstructed_coordinate({2, 2}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_coordinates();

        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({0, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({1, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({3, 4}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 0}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 1}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 3}));
        CHECK(!obstr_lyt.is_obstructed_coordinate({4, 4}));
    }
}

TEST_CASE("Connection obstruction", "[obstruction-layout]")
{
    SECTION("Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        const layout lyt{{4, 4}};

        obstruction_layout obstr_lyt{lyt};

        obstr_lyt.foreach_coordinate(
            [&obstr_lyt](const auto& c)
            {
                obstr_lyt.foreach_adjacent_coordinate(c, [&obstr_lyt, &c](const auto& ac)
                                                      { CHECK(!obstr_lyt.is_obstructed_connection(c, ac)); });
            });

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::xor_maj_gate_layout<layout>();

        obstruction_layout obstr_lyt{lyt};

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({3, 1}, {2, 1}));

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {1, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {1, 0}));

        CHECK(obstr_lyt.is_obstructed_connection({2, 1}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // removing an obstruction that was not added manually should not change anything
        obstr_lyt.clear_obstructed_connection({1, 1}, {2, 1});
        obstr_lyt.clear_obstructed_connection({2, 0}, {2, 1});
        obstr_lyt.clear_obstructed_connection({3, 1}, {2, 1});

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({3, 1}, {2, 1}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
    SECTION("Cell-level layout")
    {
        using layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::single_layer_qca_and_gate<layout>();

        obstruction_layout obstr_lyt{lyt};

        obstr_lyt.foreach_coordinate(
            [&obstr_lyt](const auto& c)
            {
                obstr_lyt.foreach_adjacent_coordinate(c, [&obstr_lyt, &c](const auto& ac)
                                                      { CHECK(!obstr_lyt.is_obstructed_connection(c, ac)); });
            });

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
}
