/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/layouts/tile_clocking.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/tile_clocking.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Clock zones have positive dimensions", "[tile-clocking]")
{
    CHECK_THROWS_AS((tile_clocking{0, 1}), std::invalid_argument);
    CHECK_THROWS_AS((tile_clocking{1, 0}), std::invalid_argument);

    tile_clocking clk{2, 3};
    CHECK_THROWS_AS(clk.set_tile_size_x(0), std::invalid_argument);
    CHECK_THROWS_AS(clk.set_tile_size_y(0), std::invalid_argument);
    CHECK(clk.get_tile_size_x() == 2);
    CHECK(clk.get_tile_size_y() == 3);
    CHECK(clk.is_clocking_scheme(clocking::OPEN_NAME));
    CHECK(clk.num_clocks() == 4);
}

TEST_CASE("Tile clocking equality compares dimensions and scheme names", "[tile-clocking]")
{
    tile_clocking original{2, 3};
    original.replace_clocking_scheme(clocking::twoddwave());
    auto copy = original;
    copy.assign_clock_number({1, 1}, 3);
    CHECK(copy == original);

    auto detached_scheme = original.get_clocking_scheme();
    detached_scheme.override_clock_number(0, 0, 3);
    CHECK(original.get_clock_number({0, 0}) == 0);

    copy.replace_clocking_scheme(clocking::twoddwave(clocking::num_clks::THREE));
    CHECK(copy == original);
    copy.replace_clocking_scheme(clocking::use());
    CHECK(copy != original);
    copy = original;
    copy.set_tile_size_x(3);
    CHECK(copy != original);
    copy = original;
    copy.set_tile_size_y(4);
    CHECK(copy != original);
}

TEST_CASE("Clock zones are tiles on every layer", "[tile-clocking]")
{
    // a layout provides the geometry that clocking::twoddwave needs
    const inml::layout lyt{{4, 4}, clocking::twoddwave(), "", 2, 2};

    CHECK(lyt.get_clock_zone({3, 2}) == coords::offset{1, 1});
    CHECK(lyt.get_clock_zone({3, 2, 1}) == coords::offset{1, 1});

    CHECK(lyt.get_clock_number({0, 0}) == 0);
    CHECK(lyt.get_clock_number({1, 1}) == 0);
    CHECK(lyt.get_clock_number({2, 0}) == 1);
    CHECK(lyt.get_clock_number({0, 3}) == 1);
    CHECK(lyt.get_clock_number({3, 3}) == 2);
    CHECK(lyt.get_clock_number({4, 1}) == 2);
    CHECK(lyt.get_clock_number({4, 3}) == 3);
    CHECK(lyt.get_clock_number({4, 4}) == 0);

    SECTION("overridden clock numbers cover the whole zone")
    {
        auto overridden = lyt;
        overridden.assign_clock_number({1, 1}, 3);

        CHECK(overridden.get_clock_number({2, 2}) == 3);
        CHECK(overridden.get_clock_number({3, 3}) == 3);
        CHECK(!overridden.is_regularly_clocked());
        CHECK(lyt.get_clock_number({2, 2}) == 2);
    }
}
