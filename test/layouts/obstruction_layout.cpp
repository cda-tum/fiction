//
// Created by marcel on 02.02.22.
//

#include "catch.hpp"
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
        CHECK(!has_is_obstructed_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_coordinate_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_coordinate_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        REQUIRE(is_gate_level_layout_v<layout>);
        CHECK(!has_is_obstructed_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_gate_level_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_gate_level_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
    SECTION("Cell-level layout")
    {
        using layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        REQUIRE(is_cell_level_layout_v<layout>);
        CHECK(!has_is_obstructed_v<layout>);

        layout lyt{};

        using obstr_layout = obstruction_layout<layout>;
        CHECK(is_cell_level_layout_v<obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_layout>);

        obstr_layout obstr_lyt{};
        obstr_layout obstr_lyt_from_lyt{lyt};

        using obstr_obstr_layout = obstruction_layout<obstr_layout>;
        CHECK(is_cell_level_layout_v<obstr_obstr_layout>);
        CHECK(has_is_obstructed_v<obstr_obstr_layout>);

        obstr_obstr_layout obstr_obstr_lyt_from_lyt{obstr_lyt};
    }
}

TEST_CASE("Obstruction", "[obstruction-layout]")
{
    SECTION("Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        const layout lyt{{4, 4}};

        const obstruction_layout obstr_lyt{lyt};

        obstr_lyt.foreach_coordinate([&obstr_lyt](const auto& c) { CHECK(!obstr_lyt.is_obstructed(c)); });
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::xor_maj_gate_layout<layout>();

        const obstruction_layout obstr_lyt{lyt};

        CHECK(obstr_lyt.is_obstructed({1, 1}));
        CHECK(obstr_lyt.is_obstructed({2, 0}));
        CHECK(obstr_lyt.is_obstructed({3, 1}));
        CHECK(obstr_lyt.is_obstructed({2, 1}));
        CHECK(obstr_lyt.is_obstructed({1, 0}));
        CHECK(obstr_lyt.is_obstructed({2, 2}));
        CHECK(obstr_lyt.is_obstructed({0, 0}));

        CHECK(!obstr_lyt.is_obstructed({0, 1}));
        CHECK(!obstr_lyt.is_obstructed({0, 2}));
        CHECK(!obstr_lyt.is_obstructed({1, 2}));
        CHECK(!obstr_lyt.is_obstructed({3, 0}));
        CHECK(!obstr_lyt.is_obstructed({3, 2}));
    }
    SECTION("Cell-level layout")
    {
        using layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

        const auto lyt = blueprints::single_layer_qca_and_gate<layout>();

        const obstruction_layout obstr_lyt{lyt};

        CHECK(obstr_lyt.is_obstructed({0, 2}));
        CHECK(obstr_lyt.is_obstructed({2, 4}));
        CHECK(obstr_lyt.is_obstructed({2, 0}));
        CHECK(obstr_lyt.is_obstructed({2, 1}));
        CHECK(obstr_lyt.is_obstructed({2, 2}));
        CHECK(obstr_lyt.is_obstructed({2, 3}));
        CHECK(obstr_lyt.is_obstructed({1, 2}));
        CHECK(obstr_lyt.is_obstructed({3, 2}));
        CHECK(obstr_lyt.is_obstructed({4, 2}));

        CHECK(!obstr_lyt.is_obstructed({0, 0}));
        CHECK(!obstr_lyt.is_obstructed({0, 1}));
        CHECK(!obstr_lyt.is_obstructed({0, 3}));
        CHECK(!obstr_lyt.is_obstructed({0, 4}));
        CHECK(!obstr_lyt.is_obstructed({1, 0}));
        CHECK(!obstr_lyt.is_obstructed({1, 1}));
        CHECK(!obstr_lyt.is_obstructed({1, 3}));
        CHECK(!obstr_lyt.is_obstructed({1, 4}));
        CHECK(!obstr_lyt.is_obstructed({3, 0}));
        CHECK(!obstr_lyt.is_obstructed({3, 1}));
        CHECK(!obstr_lyt.is_obstructed({3, 3}));
        CHECK(!obstr_lyt.is_obstructed({3, 4}));
        CHECK(!obstr_lyt.is_obstructed({4, 0}));
        CHECK(!obstr_lyt.is_obstructed({4, 1}));
        CHECK(!obstr_lyt.is_obstructed({4, 3}));
        CHECK(!obstr_lyt.is_obstructed({4, 4}));
    }
}
