/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 12.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>

#include <string>
#include <vector>

using namespace fiction;

TEST_CASE("3-phase open clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    auto open3 = layouts::clocking::open<clk_lyt>(layouts::clocking::num_clks::THREE);

    CHECK(open3.num_clocks == 3u);
    CHECK(open3.max_in_degree == 3u);
    CHECK(open3.max_out_degree == 3u);
    CHECK(!open3.is_regular());

    CHECK(open3({0, 0}) == 0);
    CHECK(open3({0, 1}) == 0);
    CHECK(open3({0, 2}) == 0);
    CHECK(open3({0, 3}) == 0);
    CHECK(open3({1, 0}) == 0);
    CHECK(open3({1, 1}) == 0);
    CHECK(open3({1, 2}) == 0);
    CHECK(open3({1, 3}) == 0);
    CHECK(open3({2, 0}) == 0);
    CHECK(open3({2, 1}) == 0);
    CHECK(open3({2, 2}) == 0);
    CHECK(open3({2, 3}) == 0);
    CHECK(open3({3, 0}) == 0);
    CHECK(open3({3, 1}) == 0);
    CHECK(open3({3, 2}) == 0);
    CHECK(open3({3, 3}) == 0);

    open3.override_clock_number({0, 0}, 0);
    open3.override_clock_number({0, 1}, 1);
    open3.override_clock_number({0, 2}, 2);
    open3.override_clock_number({0, 3}, 3);
    open3.override_clock_number({0, 4}, 4);
    open3.override_clock_number({0, 5}, 5);
    open3.override_clock_number({0, 6}, 6);

    CHECK(!open3.is_regular());

    CHECK(open3({0, 0}) == 0);
    CHECK(open3({0, 1}) == 1);
    CHECK(open3({0, 2}) == 2);
    CHECK(open3({0, 3}) == 0);
    CHECK(open3({0, 4}) == 1);
    CHECK(open3({0, 5}) == 2);
    CHECK(open3({0, 6}) == 0);
    CHECK(open3({1, 0}) == 0);
    CHECK(open3({1, 1}) == 0);
    CHECK(open3({1, 2}) == 0);
    CHECK(open3({1, 3}) == 0);
    CHECK(open3({2, 0}) == 0);
    CHECK(open3({2, 1}) == 0);
    CHECK(open3({2, 2}) == 0);
    CHECK(open3({2, 3}) == 0);
    CHECK(open3({3, 0}) == 0);
    CHECK(open3({3, 1}) == 0);
    CHECK(open3({3, 2}) == 0);
    CHECK(open3({3, 3}) == 0);
}

TEST_CASE("4-phase open clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    auto open4 = layouts::clocking::open<clk_lyt>(layouts::clocking::num_clks::FOUR);

    CHECK(open4.num_clocks == 4u);
    CHECK(open4.max_in_degree == 3u);
    CHECK(open4.max_out_degree == 3u);
    CHECK(!open4.is_regular());

    CHECK(open4({0, 0}) == 0);
    CHECK(open4({0, 1}) == 0);
    CHECK(open4({0, 2}) == 0);
    CHECK(open4({0, 3}) == 0);
    CHECK(open4({1, 0}) == 0);
    CHECK(open4({1, 1}) == 0);
    CHECK(open4({1, 2}) == 0);
    CHECK(open4({1, 3}) == 0);
    CHECK(open4({2, 0}) == 0);
    CHECK(open4({2, 1}) == 0);
    CHECK(open4({2, 2}) == 0);
    CHECK(open4({2, 3}) == 0);
    CHECK(open4({3, 0}) == 0);
    CHECK(open4({3, 1}) == 0);
    CHECK(open4({3, 2}) == 0);
    CHECK(open4({3, 3}) == 0);

    open4.override_clock_number({0, 0}, 0);
    open4.override_clock_number({0, 1}, 1);
    open4.override_clock_number({0, 2}, 2);
    open4.override_clock_number({0, 3}, 3);
    open4.override_clock_number({0, 4}, 4);
    open4.override_clock_number({0, 5}, 5);
    open4.override_clock_number({0, 6}, 6);

    CHECK(!open4.is_regular());

    CHECK(open4({0, 0}) == 0);
    CHECK(open4({0, 1}) == 1);
    CHECK(open4({0, 2}) == 2);
    CHECK(open4({0, 3}) == 3);
    CHECK(open4({0, 4}) == 0);
    CHECK(open4({0, 5}) == 1);
    CHECK(open4({0, 6}) == 2);
    CHECK(open4({1, 0}) == 0);
    CHECK(open4({1, 1}) == 0);
    CHECK(open4({1, 2}) == 0);
    CHECK(open4({1, 3}) == 0);
    CHECK(open4({2, 0}) == 0);
    CHECK(open4({2, 1}) == 0);
    CHECK(open4({2, 2}) == 0);
    CHECK(open4({2, 3}) == 0);
    CHECK(open4({3, 0}) == 0);
    CHECK(open4({3, 1}) == 0);
    CHECK(open4({3, 2}) == 0);
    CHECK(open4({3, 3}) == 0);
}

TEST_CASE("3-phase columnar clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto columnar3 = layouts::clocking::columnar<clk_lyt>(layouts::clocking::num_clks::THREE);

    CHECK(columnar3.num_clocks == 3u);
    CHECK(columnar3.max_in_degree == 3u);
    CHECK(columnar3.max_out_degree == 2u);
    CHECK(columnar3.is_regular());

    CHECK(columnar3({0, 0}) == 0);
    CHECK(columnar3({0, 1}) == 0);
    CHECK(columnar3({0, 2}) == 0);
    CHECK(columnar3({1, 0}) == 1);
    CHECK(columnar3({1, 1}) == 1);
    CHECK(columnar3({1, 2}) == 1);
    CHECK(columnar3({2, 0}) == 2);
    CHECK(columnar3({2, 1}) == 2);
    CHECK(columnar3({2, 2}) == 2);

    CHECK(columnar3({0 + 3, 0}) == 0);
    CHECK(columnar3({0 + 3, 1}) == 0);
    CHECK(columnar3({0 + 3, 2}) == 0);
    CHECK(columnar3({1 + 3, 0}) == 1);
    CHECK(columnar3({1 + 3, 1}) == 1);
    CHECK(columnar3({1 + 3, 2}) == 1);
    CHECK(columnar3({2 + 3, 0}) == 2);
    CHECK(columnar3({2 + 3, 1}) == 2);
    CHECK(columnar3({2 + 3, 2}) == 2);

    CHECK(columnar3({0, 0 + 3}) == 0);
    CHECK(columnar3({0, 1 + 3}) == 0);
    CHECK(columnar3({0, 2 + 3}) == 0);
    CHECK(columnar3({1, 0 + 3}) == 1);
    CHECK(columnar3({1, 1 + 3}) == 1);
    CHECK(columnar3({1, 2 + 3}) == 1);
    CHECK(columnar3({2, 0 + 3}) == 2);
    CHECK(columnar3({2, 1 + 3}) == 2);
    CHECK(columnar3({2, 2 + 3}) == 2);

    CHECK(columnar3({0 + 3, 0 + 3}) == 0);
    CHECK(columnar3({0 + 3, 1 + 3}) == 0);
    CHECK(columnar3({0 + 3, 2 + 3}) == 0);
    CHECK(columnar3({1 + 3, 0 + 3}) == 1);
    CHECK(columnar3({1 + 3, 1 + 3}) == 1);
    CHECK(columnar3({1 + 3, 2 + 3}) == 1);
    CHECK(columnar3({2 + 3, 0 + 3}) == 2);
    CHECK(columnar3({2 + 3, 1 + 3}) == 2);
    CHECK(columnar3({2 + 3, 2 + 3}) == 2);
}

TEST_CASE("4-phase columnar clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto columnar4 = layouts::clocking::columnar<clk_lyt>(layouts::clocking::num_clks::FOUR);

    CHECK(columnar4.num_clocks == 4u);
    CHECK(columnar4.max_in_degree == 3u);
    CHECK(columnar4.max_out_degree == 2u);
    CHECK(columnar4.is_regular());

    CHECK(columnar4({0, 0}) == 0);
    CHECK(columnar4({0, 1}) == 0);
    CHECK(columnar4({0, 2}) == 0);
    CHECK(columnar4({0, 3}) == 0);
    CHECK(columnar4({1, 0}) == 1);
    CHECK(columnar4({1, 1}) == 1);
    CHECK(columnar4({1, 2}) == 1);
    CHECK(columnar4({1, 3}) == 1);
    CHECK(columnar4({2, 0}) == 2);
    CHECK(columnar4({2, 1}) == 2);
    CHECK(columnar4({2, 2}) == 2);
    CHECK(columnar4({2, 3}) == 2);
    CHECK(columnar4({3, 0}) == 3);
    CHECK(columnar4({3, 1}) == 3);
    CHECK(columnar4({3, 2}) == 3);
    CHECK(columnar4({3, 3}) == 3);

    CHECK(columnar4({0 + 4, 0}) == 0);
    CHECK(columnar4({0 + 4, 1}) == 0);
    CHECK(columnar4({0 + 4, 2}) == 0);
    CHECK(columnar4({0 + 4, 3}) == 0);
    CHECK(columnar4({1 + 4, 0}) == 1);
    CHECK(columnar4({1 + 4, 1}) == 1);
    CHECK(columnar4({1 + 4, 2}) == 1);
    CHECK(columnar4({1 + 4, 3}) == 1);
    CHECK(columnar4({2 + 4, 0}) == 2);
    CHECK(columnar4({2 + 4, 1}) == 2);
    CHECK(columnar4({2 + 4, 2}) == 2);
    CHECK(columnar4({2 + 4, 3}) == 2);
    CHECK(columnar4({3 + 4, 0}) == 3);
    CHECK(columnar4({3 + 4, 1}) == 3);
    CHECK(columnar4({3 + 4, 2}) == 3);
    CHECK(columnar4({3 + 4, 3}) == 3);

    CHECK(columnar4({0, 0 + 4}) == 0);
    CHECK(columnar4({0, 1 + 4}) == 0);
    CHECK(columnar4({0, 2 + 4}) == 0);
    CHECK(columnar4({0, 3 + 4}) == 0);
    CHECK(columnar4({1, 0 + 4}) == 1);
    CHECK(columnar4({1, 1 + 4}) == 1);
    CHECK(columnar4({1, 2 + 4}) == 1);
    CHECK(columnar4({1, 3 + 4}) == 1);
    CHECK(columnar4({2, 0 + 4}) == 2);
    CHECK(columnar4({2, 1 + 4}) == 2);
    CHECK(columnar4({2, 2 + 4}) == 2);
    CHECK(columnar4({2, 3 + 4}) == 2);
    CHECK(columnar4({3, 0 + 4}) == 3);
    CHECK(columnar4({3, 1 + 4}) == 3);
    CHECK(columnar4({3, 2 + 4}) == 3);
    CHECK(columnar4({3, 3 + 4}) == 3);

    CHECK(columnar4({0 + 4, 0 + 4}) == 0);
    CHECK(columnar4({0 + 4, 1 + 4}) == 0);
    CHECK(columnar4({0 + 4, 2 + 4}) == 0);
    CHECK(columnar4({0 + 4, 3 + 4}) == 0);
    CHECK(columnar4({1 + 4, 0 + 4}) == 1);
    CHECK(columnar4({1 + 4, 1 + 4}) == 1);
    CHECK(columnar4({1 + 4, 2 + 4}) == 1);
    CHECK(columnar4({1 + 4, 3 + 4}) == 1);
    CHECK(columnar4({2 + 4, 0 + 4}) == 2);
    CHECK(columnar4({2 + 4, 1 + 4}) == 2);
    CHECK(columnar4({2 + 4, 2 + 4}) == 2);
    CHECK(columnar4({2 + 4, 3 + 4}) == 2);
    CHECK(columnar4({3 + 4, 0 + 4}) == 3);
    CHECK(columnar4({3 + 4, 1 + 4}) == 3);
    CHECK(columnar4({3 + 4, 2 + 4}) == 3);
    CHECK(columnar4({3 + 4, 3 + 4}) == 3);
}

TEST_CASE("3-phase row clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto row3 = layouts::clocking::row<clk_lyt>(layouts::clocking::num_clks::THREE);

    CHECK(row3.num_clocks == 3u);
    CHECK(row3.max_in_degree == 3u);
    CHECK(row3.max_out_degree == 2u);
    CHECK(row3.is_regular());

    CHECK(row3({0, 0}) == 0);
    CHECK(row3({0, 1}) == 1);
    CHECK(row3({0, 2}) == 2);
    CHECK(row3({1, 0}) == 0);
    CHECK(row3({1, 1}) == 1);
    CHECK(row3({1, 2}) == 2);
    CHECK(row3({2, 0}) == 0);
    CHECK(row3({2, 1}) == 1);
    CHECK(row3({2, 2}) == 2);

    CHECK(row3({0 + 3, 0}) == 0);
    CHECK(row3({0 + 3, 1}) == 1);
    CHECK(row3({0 + 3, 2}) == 2);
    CHECK(row3({1 + 3, 0}) == 0);
    CHECK(row3({1 + 3, 1}) == 1);
    CHECK(row3({1 + 3, 2}) == 2);
    CHECK(row3({2 + 3, 0}) == 0);
    CHECK(row3({2 + 3, 1}) == 1);
    CHECK(row3({2 + 3, 2}) == 2);

    CHECK(row3({0, 0 + 3}) == 0);
    CHECK(row3({0, 1 + 3}) == 1);
    CHECK(row3({0, 2 + 3}) == 2);
    CHECK(row3({1, 0 + 3}) == 0);
    CHECK(row3({1, 1 + 3}) == 1);
    CHECK(row3({1, 2 + 3}) == 2);
    CHECK(row3({2, 0 + 3}) == 0);
    CHECK(row3({2, 1 + 3}) == 1);
    CHECK(row3({2, 2 + 3}) == 2);

    CHECK(row3({0 + 3, 0 + 3}) == 0);
    CHECK(row3({0 + 3, 1 + 3}) == 1);
    CHECK(row3({0 + 3, 2 + 3}) == 2);
    CHECK(row3({1 + 3, 0 + 3}) == 0);
    CHECK(row3({1 + 3, 1 + 3}) == 1);
    CHECK(row3({1 + 3, 2 + 3}) == 2);
    CHECK(row3({2 + 3, 0 + 3}) == 0);
    CHECK(row3({2 + 3, 1 + 3}) == 1);
    CHECK(row3({2 + 3, 2 + 3}) == 2);
}

TEST_CASE("4-phase row clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto row4 = layouts::clocking::row<clk_lyt>(layouts::clocking::num_clks::FOUR);

    CHECK(row4.num_clocks == 4u);
    CHECK(row4.max_in_degree == 3u);
    CHECK(row4.max_out_degree == 2u);
    CHECK(row4.is_regular());

    CHECK(row4({0, 0}) == 0);
    CHECK(row4({0, 1}) == 1);
    CHECK(row4({0, 2}) == 2);
    CHECK(row4({0, 3}) == 3);
    CHECK(row4({1, 0}) == 0);
    CHECK(row4({1, 1}) == 1);
    CHECK(row4({1, 2}) == 2);
    CHECK(row4({1, 3}) == 3);
    CHECK(row4({2, 0}) == 0);
    CHECK(row4({2, 1}) == 1);
    CHECK(row4({2, 2}) == 2);
    CHECK(row4({2, 3}) == 3);
    CHECK(row4({3, 0}) == 0);
    CHECK(row4({3, 1}) == 1);
    CHECK(row4({3, 2}) == 2);
    CHECK(row4({3, 3}) == 3);

    CHECK(row4({0 + 4, 0}) == 0);
    CHECK(row4({0 + 4, 1}) == 1);
    CHECK(row4({0 + 4, 2}) == 2);
    CHECK(row4({0 + 4, 3}) == 3);
    CHECK(row4({1 + 4, 0}) == 0);
    CHECK(row4({1 + 4, 1}) == 1);
    CHECK(row4({1 + 4, 2}) == 2);
    CHECK(row4({1 + 4, 3}) == 3);
    CHECK(row4({2 + 4, 0}) == 0);
    CHECK(row4({2 + 4, 1}) == 1);
    CHECK(row4({2 + 4, 2}) == 2);
    CHECK(row4({2 + 4, 3}) == 3);
    CHECK(row4({3 + 4, 0}) == 0);
    CHECK(row4({3 + 4, 1}) == 1);
    CHECK(row4({3 + 4, 2}) == 2);
    CHECK(row4({3 + 4, 3}) == 3);

    CHECK(row4({0, 0 + 4}) == 0);
    CHECK(row4({0, 1 + 4}) == 1);
    CHECK(row4({0, 2 + 4}) == 2);
    CHECK(row4({0, 3 + 4}) == 3);
    CHECK(row4({1, 0 + 4}) == 0);
    CHECK(row4({1, 1 + 4}) == 1);
    CHECK(row4({1, 2 + 4}) == 2);
    CHECK(row4({1, 3 + 4}) == 3);
    CHECK(row4({2, 0 + 4}) == 0);
    CHECK(row4({2, 1 + 4}) == 1);
    CHECK(row4({2, 2 + 4}) == 2);
    CHECK(row4({2, 3 + 4}) == 3);
    CHECK(row4({3, 0 + 4}) == 0);
    CHECK(row4({3, 1 + 4}) == 1);
    CHECK(row4({3, 2 + 4}) == 2);
    CHECK(row4({3, 3 + 4}) == 3);

    CHECK(row4({0 + 4, 0 + 4}) == 0);
    CHECK(row4({0 + 4, 1 + 4}) == 1);
    CHECK(row4({0 + 4, 2 + 4}) == 2);
    CHECK(row4({0 + 4, 3 + 4}) == 3);
    CHECK(row4({1 + 4, 0 + 4}) == 0);
    CHECK(row4({1 + 4, 1 + 4}) == 1);
    CHECK(row4({1 + 4, 2 + 4}) == 2);
    CHECK(row4({1 + 4, 3 + 4}) == 3);
    CHECK(row4({2 + 4, 0 + 4}) == 0);
    CHECK(row4({2 + 4, 1 + 4}) == 1);
    CHECK(row4({2 + 4, 2 + 4}) == 2);
    CHECK(row4({2 + 4, 3 + 4}) == 3);
    CHECK(row4({3 + 4, 0 + 4}) == 0);
    CHECK(row4({3 + 4, 1 + 4}) == 1);
    CHECK(row4({3 + 4, 2 + 4}) == 2);
    CHECK(row4({3 + 4, 3 + 4}) == 3);
}

TEST_CASE("3-phase 2DDWave", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto twoddwave3 = layouts::clocking::twoddwave<clk_lyt>(layouts::clocking::num_clks::THREE);

    CHECK(twoddwave3.num_clocks == 3u);
    CHECK(twoddwave3.max_in_degree == 2u);
    CHECK(twoddwave3.max_out_degree == 2u);
    CHECK(twoddwave3.is_regular());

    CHECK(twoddwave3({0, 0}) == 0);
    CHECK(twoddwave3({0, 1}) == 1);
    CHECK(twoddwave3({0, 2}) == 2);
    CHECK(twoddwave3({1, 0}) == 1);
    CHECK(twoddwave3({1, 1}) == 2);
    CHECK(twoddwave3({1, 2}) == 0);
    CHECK(twoddwave3({2, 0}) == 2);
    CHECK(twoddwave3({2, 1}) == 0);
    CHECK(twoddwave3({2, 2}) == 1);

    CHECK(twoddwave3({0 + 3, 0}) == 0);
    CHECK(twoddwave3({0 + 3, 1}) == 1);
    CHECK(twoddwave3({0 + 3, 2}) == 2);
    CHECK(twoddwave3({1 + 3, 0}) == 1);
    CHECK(twoddwave3({1 + 3, 1}) == 2);
    CHECK(twoddwave3({1 + 3, 2}) == 0);
    CHECK(twoddwave3({2 + 3, 0}) == 2);
    CHECK(twoddwave3({2 + 3, 1}) == 0);
    CHECK(twoddwave3({2 + 3, 2}) == 1);

    CHECK(twoddwave3({0, 0 + 3}) == 0);
    CHECK(twoddwave3({0, 1 + 3}) == 1);
    CHECK(twoddwave3({0, 2 + 3}) == 2);
    CHECK(twoddwave3({1, 0 + 3}) == 1);
    CHECK(twoddwave3({1, 1 + 3}) == 2);
    CHECK(twoddwave3({1, 2 + 3}) == 0);
    CHECK(twoddwave3({2, 0 + 3}) == 2);
    CHECK(twoddwave3({2, 1 + 3}) == 0);
    CHECK(twoddwave3({2, 2 + 3}) == 1);

    CHECK(twoddwave3({0 + 3, 0 + 3}) == 0);
    CHECK(twoddwave3({0 + 3, 1 + 3}) == 1);
    CHECK(twoddwave3({0 + 3, 2 + 3}) == 2);
    CHECK(twoddwave3({1 + 3, 0 + 3}) == 1);
    CHECK(twoddwave3({1 + 3, 1 + 3}) == 2);
    CHECK(twoddwave3({1 + 3, 2 + 3}) == 0);
    CHECK(twoddwave3({2 + 3, 0 + 3}) == 2);
    CHECK(twoddwave3({2 + 3, 1 + 3}) == 0);
    CHECK(twoddwave3({2 + 3, 2 + 3}) == 1);
}

TEST_CASE("4-phase 2DDWave", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto twoddwave4 = layouts::clocking::twoddwave<clk_lyt>(layouts::clocking::num_clks::FOUR);

    CHECK(twoddwave4.num_clocks == 4u);
    CHECK(twoddwave4.max_in_degree == 2u);
    CHECK(twoddwave4.max_out_degree == 2u);
    CHECK(twoddwave4.is_regular());

    CHECK(twoddwave4({0, 0}) == 0);
    CHECK(twoddwave4({0, 1}) == 1);
    CHECK(twoddwave4({0, 2}) == 2);
    CHECK(twoddwave4({0, 3}) == 3);
    CHECK(twoddwave4({1, 0}) == 1);
    CHECK(twoddwave4({1, 1}) == 2);
    CHECK(twoddwave4({1, 2}) == 3);
    CHECK(twoddwave4({1, 3}) == 0);
    CHECK(twoddwave4({2, 0}) == 2);
    CHECK(twoddwave4({2, 1}) == 3);
    CHECK(twoddwave4({2, 2}) == 0);
    CHECK(twoddwave4({2, 3}) == 1);
    CHECK(twoddwave4({3, 0}) == 3);
    CHECK(twoddwave4({3, 1}) == 0);
    CHECK(twoddwave4({3, 2}) == 1);
    CHECK(twoddwave4({3, 3}) == 2);

    CHECK(twoddwave4({0 + 4, 0}) == 0);
    CHECK(twoddwave4({0 + 4, 1}) == 1);
    CHECK(twoddwave4({0 + 4, 2}) == 2);
    CHECK(twoddwave4({0 + 4, 3}) == 3);
    CHECK(twoddwave4({1 + 4, 0}) == 1);
    CHECK(twoddwave4({1 + 4, 1}) == 2);
    CHECK(twoddwave4({1 + 4, 2}) == 3);
    CHECK(twoddwave4({1 + 4, 3}) == 0);
    CHECK(twoddwave4({2 + 4, 0}) == 2);
    CHECK(twoddwave4({2 + 4, 1}) == 3);
    CHECK(twoddwave4({2 + 4, 2}) == 0);
    CHECK(twoddwave4({2 + 4, 3}) == 1);
    CHECK(twoddwave4({3 + 4, 0}) == 3);
    CHECK(twoddwave4({3 + 4, 1}) == 0);
    CHECK(twoddwave4({3 + 4, 2}) == 1);
    CHECK(twoddwave4({3 + 4, 3}) == 2);

    CHECK(twoddwave4({0, 0 + 4}) == 0);
    CHECK(twoddwave4({0, 1 + 4}) == 1);
    CHECK(twoddwave4({0, 2 + 4}) == 2);
    CHECK(twoddwave4({0, 3 + 4}) == 3);
    CHECK(twoddwave4({1, 0 + 4}) == 1);
    CHECK(twoddwave4({1, 1 + 4}) == 2);
    CHECK(twoddwave4({1, 2 + 4}) == 3);
    CHECK(twoddwave4({1, 3 + 4}) == 0);
    CHECK(twoddwave4({2, 0 + 4}) == 2);
    CHECK(twoddwave4({2, 1 + 4}) == 3);
    CHECK(twoddwave4({2, 2 + 4}) == 0);
    CHECK(twoddwave4({2, 3 + 4}) == 1);
    CHECK(twoddwave4({3, 0 + 4}) == 3);
    CHECK(twoddwave4({3, 1 + 4}) == 0);
    CHECK(twoddwave4({3, 2 + 4}) == 1);
    CHECK(twoddwave4({3, 3 + 4}) == 2);

    CHECK(twoddwave4({0 + 4, 0 + 4}) == 0);
    CHECK(twoddwave4({0 + 4, 1 + 4}) == 1);
    CHECK(twoddwave4({0 + 4, 2 + 4}) == 2);
    CHECK(twoddwave4({0 + 4, 3 + 4}) == 3);
    CHECK(twoddwave4({1 + 4, 0 + 4}) == 1);
    CHECK(twoddwave4({1 + 4, 1 + 4}) == 2);
    CHECK(twoddwave4({1 + 4, 2 + 4}) == 3);
    CHECK(twoddwave4({1 + 4, 3 + 4}) == 0);
    CHECK(twoddwave4({2 + 4, 0 + 4}) == 2);
    CHECK(twoddwave4({2 + 4, 1 + 4}) == 3);
    CHECK(twoddwave4({2 + 4, 2 + 4}) == 0);
    CHECK(twoddwave4({2 + 4, 3 + 4}) == 1);
    CHECK(twoddwave4({3 + 4, 0 + 4}) == 3);
    CHECK(twoddwave4({3 + 4, 1 + 4}) == 0);
    CHECK(twoddwave4({3 + 4, 2 + 4}) == 1);
    CHECK(twoddwave4({3 + 4, 3 + 4}) == 2);
}

TEST_CASE("3-phase 2DDWaveHex", "[clocking-scheme]")
{
    SECTION("odd row")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_row_hex>>;

        const auto twoddwave_hex_3 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::THREE);

        CHECK(twoddwave_hex_3.num_clocks == 3u);
        CHECK(twoddwave_hex_3.max_in_degree == 2u);
        CHECK(twoddwave_hex_3.max_out_degree == 2u);
        CHECK(twoddwave_hex_3.is_regular());

        CHECK(twoddwave_hex_3({0, 0}) == 0);
        CHECK(twoddwave_hex_3({0, 1}) == 1);
        CHECK(twoddwave_hex_3({0, 2}) == 1);
        CHECK(twoddwave_hex_3({0, 3}) == 2);
        CHECK(twoddwave_hex_3({0, 4}) == 2);
        CHECK(twoddwave_hex_3({0, 5}) == 0);
        CHECK(twoddwave_hex_3({1, 0}) == 1);
        CHECK(twoddwave_hex_3({1, 1}) == 2);
        CHECK(twoddwave_hex_3({1, 2}) == 2);
        CHECK(twoddwave_hex_3({1, 3}) == 0);
        CHECK(twoddwave_hex_3({1, 4}) == 0);
        CHECK(twoddwave_hex_3({1, 5}) == 1);
        CHECK(twoddwave_hex_3({2, 0}) == 2);
        CHECK(twoddwave_hex_3({2, 1}) == 0);
        CHECK(twoddwave_hex_3({2, 2}) == 0);
        CHECK(twoddwave_hex_3({2, 3}) == 1);
        CHECK(twoddwave_hex_3({2, 4}) == 1);
        CHECK(twoddwave_hex_3({2, 5}) == 2);

        CHECK(twoddwave_hex_3({0 + 3, 0}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 1}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 2}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 3}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 4}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 5}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 0}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 1}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 2}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 3}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 4}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 5}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 0}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 1}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 2}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 3}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 4}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 5}) == 2);

        CHECK(twoddwave_hex_3({0, 0 + 6}) == 0);
        CHECK(twoddwave_hex_3({0, 1 + 6}) == 1);
        CHECK(twoddwave_hex_3({0, 2 + 6}) == 1);
        CHECK(twoddwave_hex_3({0, 3 + 6}) == 2);
        CHECK(twoddwave_hex_3({0, 4 + 6}) == 2);
        CHECK(twoddwave_hex_3({0, 5 + 6}) == 0);
        CHECK(twoddwave_hex_3({1, 0 + 6}) == 1);
        CHECK(twoddwave_hex_3({1, 1 + 6}) == 2);
        CHECK(twoddwave_hex_3({1, 2 + 6}) == 2);
        CHECK(twoddwave_hex_3({1, 3 + 6}) == 0);
        CHECK(twoddwave_hex_3({1, 4 + 6}) == 0);
        CHECK(twoddwave_hex_3({1, 5 + 6}) == 1);
        CHECK(twoddwave_hex_3({2, 0 + 6}) == 2);
        CHECK(twoddwave_hex_3({2, 1 + 6}) == 0);
        CHECK(twoddwave_hex_3({2, 2 + 6}) == 0);
        CHECK(twoddwave_hex_3({2, 3 + 6}) == 1);
        CHECK(twoddwave_hex_3({2, 4 + 6}) == 1);
        CHECK(twoddwave_hex_3({2, 5 + 6}) == 2);

        CHECK(twoddwave_hex_3({0 + 3, 0 + 6}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 1 + 6}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 2 + 6}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 3 + 6}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 4 + 6}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 5 + 6}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 0 + 6}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 1 + 6}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 2 + 6}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 3 + 6}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 4 + 6}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 5 + 6}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 0 + 6}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 1 + 6}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 2 + 6}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 3 + 6}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 4 + 6}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 5 + 6}) == 2);
    }
    SECTION("even row")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_row_hex>>;

        const auto twoddwave_hex_3 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::THREE);

        CHECK(twoddwave_hex_3.num_clocks == 3u);
        CHECK(twoddwave_hex_3.max_in_degree == 2u);
        CHECK(twoddwave_hex_3.max_out_degree == 2u);
        CHECK(twoddwave_hex_3.is_regular());

        CHECK(twoddwave_hex_3({0, 0}) == 0);
        CHECK(twoddwave_hex_3({0, 1}) == 0);
        CHECK(twoddwave_hex_3({0, 2}) == 1);
        CHECK(twoddwave_hex_3({0, 3}) == 1);
        CHECK(twoddwave_hex_3({0, 4}) == 2);
        CHECK(twoddwave_hex_3({0, 5}) == 2);
        CHECK(twoddwave_hex_3({1, 0}) == 1);
        CHECK(twoddwave_hex_3({1, 1}) == 1);
        CHECK(twoddwave_hex_3({1, 2}) == 2);
        CHECK(twoddwave_hex_3({1, 3}) == 2);
        CHECK(twoddwave_hex_3({1, 4}) == 0);
        CHECK(twoddwave_hex_3({1, 5}) == 0);
        CHECK(twoddwave_hex_3({2, 0}) == 2);
        CHECK(twoddwave_hex_3({2, 1}) == 2);
        CHECK(twoddwave_hex_3({2, 2}) == 0);
        CHECK(twoddwave_hex_3({2, 3}) == 0);
        CHECK(twoddwave_hex_3({2, 4}) == 1);
        CHECK(twoddwave_hex_3({2, 5}) == 1);

        CHECK(twoddwave_hex_3({0 + 3, 0}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 1}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 2}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 3}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 4}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 5}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 0}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 1}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 2}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 3}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 4}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 5}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 0}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 1}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 2}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 3}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 4}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 5}) == 1);

        CHECK(twoddwave_hex_3({0, 0 + 6}) == 0);
        CHECK(twoddwave_hex_3({0, 1 + 6}) == 0);
        CHECK(twoddwave_hex_3({0, 2 + 6}) == 1);
        CHECK(twoddwave_hex_3({0, 3 + 6}) == 1);
        CHECK(twoddwave_hex_3({0, 4 + 6}) == 2);
        CHECK(twoddwave_hex_3({0, 5 + 6}) == 2);
        CHECK(twoddwave_hex_3({1, 0 + 6}) == 1);
        CHECK(twoddwave_hex_3({1, 1 + 6}) == 1);
        CHECK(twoddwave_hex_3({1, 2 + 6}) == 2);
        CHECK(twoddwave_hex_3({1, 3 + 6}) == 2);
        CHECK(twoddwave_hex_3({1, 4 + 6}) == 0);
        CHECK(twoddwave_hex_3({1, 5 + 6}) == 0);
        CHECK(twoddwave_hex_3({2, 0 + 6}) == 2);
        CHECK(twoddwave_hex_3({2, 1 + 6}) == 2);
        CHECK(twoddwave_hex_3({2, 2 + 6}) == 0);
        CHECK(twoddwave_hex_3({2, 3 + 6}) == 0);
        CHECK(twoddwave_hex_3({2, 4 + 6}) == 1);
        CHECK(twoddwave_hex_3({2, 5 + 6}) == 1);

        CHECK(twoddwave_hex_3({0 + 3, 0 + 6}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 1 + 6}) == 0);
        CHECK(twoddwave_hex_3({0 + 3, 2 + 6}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 3 + 6}) == 1);
        CHECK(twoddwave_hex_3({0 + 3, 4 + 6}) == 2);
        CHECK(twoddwave_hex_3({0 + 3, 5 + 6}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 0 + 6}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 1 + 6}) == 1);
        CHECK(twoddwave_hex_3({1 + 3, 2 + 6}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 3 + 6}) == 2);
        CHECK(twoddwave_hex_3({1 + 3, 4 + 6}) == 0);
        CHECK(twoddwave_hex_3({1 + 3, 5 + 6}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 0 + 6}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 1 + 6}) == 2);
        CHECK(twoddwave_hex_3({2 + 3, 2 + 6}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 3 + 6}) == 0);
        CHECK(twoddwave_hex_3({2 + 3, 4 + 6}) == 1);
        CHECK(twoddwave_hex_3({2 + 3, 5 + 6}) == 1);
    }
    SECTION("odd column")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_column_hex>>;

        const auto twoddwave_hex_3 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::THREE);

        CHECK(twoddwave_hex_3.num_clocks == 3u);
        CHECK(twoddwave_hex_3.max_in_degree == 2u);
        CHECK(twoddwave_hex_3.max_out_degree == 2u);
        CHECK(twoddwave_hex_3.is_regular());

        CHECK(twoddwave_hex_3({0, 0}) == 0);
        CHECK(twoddwave_hex_3({0, 1}) == 1);
        CHECK(twoddwave_hex_3({0, 2}) == 2);
        CHECK(twoddwave_hex_3({1, 0}) == 1);
        CHECK(twoddwave_hex_3({1, 1}) == 2);
        CHECK(twoddwave_hex_3({1, 2}) == 0);
        CHECK(twoddwave_hex_3({2, 0}) == 1);
        CHECK(twoddwave_hex_3({2, 1}) == 2);
        CHECK(twoddwave_hex_3({2, 2}) == 0);
        CHECK(twoddwave_hex_3({3, 0}) == 2);
        CHECK(twoddwave_hex_3({3, 1}) == 0);
        CHECK(twoddwave_hex_3({3, 2}) == 1);
        CHECK(twoddwave_hex_3({4, 0}) == 2);
        CHECK(twoddwave_hex_3({4, 1}) == 0);
        CHECK(twoddwave_hex_3({4, 2}) == 1);
        CHECK(twoddwave_hex_3({5, 0}) == 0);
        CHECK(twoddwave_hex_3({5, 1}) == 1);
        CHECK(twoddwave_hex_3({5, 2}) == 2);

        CHECK(twoddwave_hex_3({0 + 6, 0}) == 0);
        CHECK(twoddwave_hex_3({0 + 6, 1}) == 1);
        CHECK(twoddwave_hex_3({0 + 6, 2}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 0}) == 1);
        CHECK(twoddwave_hex_3({1 + 6, 1}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 2}) == 0);
        CHECK(twoddwave_hex_3({2 + 6, 0}) == 1);
        CHECK(twoddwave_hex_3({2 + 6, 1}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 2}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 0}) == 2);
        CHECK(twoddwave_hex_3({3 + 6, 1}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 2}) == 1);
        CHECK(twoddwave_hex_3({4 + 6, 0}) == 2);
        CHECK(twoddwave_hex_3({4 + 6, 1}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 2}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 0}) == 0);
        CHECK(twoddwave_hex_3({5 + 6, 1}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 2}) == 2);

        CHECK(twoddwave_hex_3({0, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({0, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({0, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({1, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({1, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({1, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({2, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({2, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({2, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({3, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({3, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({3, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({4, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({4, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({4, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({5, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({5, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({5, 2 + 3}) == 2);

        CHECK(twoddwave_hex_3({0 + 6, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({0 + 6, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({0 + 6, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({1 + 6, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({2 + 6, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({2 + 6, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({3 + 6, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({4 + 6, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({4 + 6, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({5 + 6, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 2 + 3}) == 2);
    }
    SECTION("even column")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_column_hex>>;

        const auto twoddwave_hex_3 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::THREE);

        CHECK(twoddwave_hex_3.num_clocks == 3u);
        CHECK(twoddwave_hex_3.max_in_degree == 2u);
        CHECK(twoddwave_hex_3.max_out_degree == 2u);
        CHECK(twoddwave_hex_3.is_regular());

        CHECK(twoddwave_hex_3({0, 0}) == 0);
        CHECK(twoddwave_hex_3({0, 1}) == 1);
        CHECK(twoddwave_hex_3({0, 2}) == 2);
        CHECK(twoddwave_hex_3({1, 0}) == 0);
        CHECK(twoddwave_hex_3({1, 1}) == 1);
        CHECK(twoddwave_hex_3({1, 2}) == 2);
        CHECK(twoddwave_hex_3({2, 0}) == 1);
        CHECK(twoddwave_hex_3({2, 1}) == 2);
        CHECK(twoddwave_hex_3({2, 2}) == 0);
        CHECK(twoddwave_hex_3({3, 0}) == 1);
        CHECK(twoddwave_hex_3({3, 1}) == 2);
        CHECK(twoddwave_hex_3({3, 2}) == 0);
        CHECK(twoddwave_hex_3({4, 0}) == 2);
        CHECK(twoddwave_hex_3({4, 1}) == 0);
        CHECK(twoddwave_hex_3({4, 2}) == 1);
        CHECK(twoddwave_hex_3({5, 0}) == 2);
        CHECK(twoddwave_hex_3({5, 1}) == 0);
        CHECK(twoddwave_hex_3({5, 2}) == 1);

        CHECK(twoddwave_hex_3({0 + 6, 0}) == 0);
        CHECK(twoddwave_hex_3({0 + 6, 1}) == 1);
        CHECK(twoddwave_hex_3({0 + 6, 2}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 0}) == 0);
        CHECK(twoddwave_hex_3({1 + 6, 1}) == 1);
        CHECK(twoddwave_hex_3({1 + 6, 2}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 0}) == 1);
        CHECK(twoddwave_hex_3({2 + 6, 1}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 2}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 0}) == 1);
        CHECK(twoddwave_hex_3({3 + 6, 1}) == 2);
        CHECK(twoddwave_hex_3({3 + 6, 2}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 0}) == 2);
        CHECK(twoddwave_hex_3({4 + 6, 1}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 2}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 0}) == 2);
        CHECK(twoddwave_hex_3({5 + 6, 1}) == 0);
        CHECK(twoddwave_hex_3({5 + 6, 2}) == 1);

        CHECK(twoddwave_hex_3({0, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({0, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({0, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({1, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({1, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({1, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({2, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({2, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({2, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({3, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({3, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({3, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({4, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({4, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({4, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({5, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({5, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({5, 2 + 3}) == 1);

        CHECK(twoddwave_hex_3({0 + 6, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({0 + 6, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({0 + 6, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({1 + 6, 0 + 3}) == 0);
        CHECK(twoddwave_hex_3({1 + 6, 1 + 3}) == 1);
        CHECK(twoddwave_hex_3({1 + 6, 2 + 3}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({2 + 6, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({2 + 6, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({3 + 6, 0 + 3}) == 1);
        CHECK(twoddwave_hex_3({3 + 6, 1 + 3}) == 2);
        CHECK(twoddwave_hex_3({3 + 6, 2 + 3}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({4 + 6, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({4 + 6, 2 + 3}) == 1);
        CHECK(twoddwave_hex_3({5 + 6, 0 + 3}) == 2);
        CHECK(twoddwave_hex_3({5 + 6, 1 + 3}) == 0);
        CHECK(twoddwave_hex_3({5 + 6, 2 + 3}) == 1);
    }
}

TEST_CASE("4-phase 2DDWaveHex", "[clocking-scheme]")
{
    SECTION("odd row")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_row_hex>>;

        const auto twoddwave_hex_4 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::FOUR);

        CHECK(twoddwave_hex_4.num_clocks == 4u);
        CHECK(twoddwave_hex_4.max_in_degree == 2u);
        CHECK(twoddwave_hex_4.max_out_degree == 2u);
        CHECK(twoddwave_hex_4.is_regular());

        CHECK(twoddwave_hex_4({0, 0}) == 0);
        CHECK(twoddwave_hex_4({0, 1}) == 1);
        CHECK(twoddwave_hex_4({0, 2}) == 1);
        CHECK(twoddwave_hex_4({0, 3}) == 2);
        CHECK(twoddwave_hex_4({0, 4}) == 2);
        CHECK(twoddwave_hex_4({0, 5}) == 3);
        CHECK(twoddwave_hex_4({0, 6}) == 3);
        CHECK(twoddwave_hex_4({0, 7}) == 0);
        CHECK(twoddwave_hex_4({1, 0}) == 1);
        CHECK(twoddwave_hex_4({1, 1}) == 2);
        CHECK(twoddwave_hex_4({1, 2}) == 2);
        CHECK(twoddwave_hex_4({1, 3}) == 3);
        CHECK(twoddwave_hex_4({1, 4}) == 3);
        CHECK(twoddwave_hex_4({1, 5}) == 0);
        CHECK(twoddwave_hex_4({1, 6}) == 0);
        CHECK(twoddwave_hex_4({1, 7}) == 1);
        CHECK(twoddwave_hex_4({2, 0}) == 2);
        CHECK(twoddwave_hex_4({2, 1}) == 3);
        CHECK(twoddwave_hex_4({2, 2}) == 3);
        CHECK(twoddwave_hex_4({2, 3}) == 0);
        CHECK(twoddwave_hex_4({2, 4}) == 0);
        CHECK(twoddwave_hex_4({2, 5}) == 1);
        CHECK(twoddwave_hex_4({2, 6}) == 1);
        CHECK(twoddwave_hex_4({2, 7}) == 2);
        CHECK(twoddwave_hex_4({3, 0}) == 3);
        CHECK(twoddwave_hex_4({3, 1}) == 0);
        CHECK(twoddwave_hex_4({3, 2}) == 0);
        CHECK(twoddwave_hex_4({3, 3}) == 1);
        CHECK(twoddwave_hex_4({3, 4}) == 1);
        CHECK(twoddwave_hex_4({3, 5}) == 2);
        CHECK(twoddwave_hex_4({3, 6}) == 2);
        CHECK(twoddwave_hex_4({3, 7}) == 3);

        CHECK(twoddwave_hex_4({0 + 4, 0}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 1}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 2}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 3}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 4}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 5}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 6}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 7}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 0}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 1}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 2}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 3}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 4}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 5}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 6}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 7}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 0}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 1}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 2}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 3}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 4}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 5}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 6}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 7}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 0}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 1}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 2}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 3}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 4}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 5}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 6}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 7}) == 3);

        CHECK(twoddwave_hex_4({0, 0 + 8}) == 0);
        CHECK(twoddwave_hex_4({0, 1 + 8}) == 1);
        CHECK(twoddwave_hex_4({0, 2 + 8}) == 1);
        CHECK(twoddwave_hex_4({0, 3 + 8}) == 2);
        CHECK(twoddwave_hex_4({0, 4 + 8}) == 2);
        CHECK(twoddwave_hex_4({0, 5 + 8}) == 3);
        CHECK(twoddwave_hex_4({0, 6 + 8}) == 3);
        CHECK(twoddwave_hex_4({0, 7 + 8}) == 0);
        CHECK(twoddwave_hex_4({1, 0 + 8}) == 1);
        CHECK(twoddwave_hex_4({1, 1 + 8}) == 2);
        CHECK(twoddwave_hex_4({1, 2 + 8}) == 2);
        CHECK(twoddwave_hex_4({1, 3 + 8}) == 3);
        CHECK(twoddwave_hex_4({1, 4 + 8}) == 3);
        CHECK(twoddwave_hex_4({1, 5 + 8}) == 0);
        CHECK(twoddwave_hex_4({1, 6 + 8}) == 0);
        CHECK(twoddwave_hex_4({1, 7 + 8}) == 1);
        CHECK(twoddwave_hex_4({2, 0 + 8}) == 2);
        CHECK(twoddwave_hex_4({2, 1 + 8}) == 3);
        CHECK(twoddwave_hex_4({2, 2 + 8}) == 3);
        CHECK(twoddwave_hex_4({2, 3 + 8}) == 0);
        CHECK(twoddwave_hex_4({2, 4 + 8}) == 0);
        CHECK(twoddwave_hex_4({2, 5 + 8}) == 1);
        CHECK(twoddwave_hex_4({2, 6 + 8}) == 1);
        CHECK(twoddwave_hex_4({2, 7 + 8}) == 2);
        CHECK(twoddwave_hex_4({3, 0 + 8}) == 3);
        CHECK(twoddwave_hex_4({3, 1 + 8}) == 0);
        CHECK(twoddwave_hex_4({3, 2 + 8}) == 0);
        CHECK(twoddwave_hex_4({3, 3 + 8}) == 1);
        CHECK(twoddwave_hex_4({3, 4 + 8}) == 1);
        CHECK(twoddwave_hex_4({3, 5 + 8}) == 2);
        CHECK(twoddwave_hex_4({3, 6 + 8}) == 2);
        CHECK(twoddwave_hex_4({3, 7 + 8}) == 3);

        CHECK(twoddwave_hex_4({0 + 4, 0 + 8}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 1 + 8}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 2 + 8}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 3 + 8}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 4 + 8}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 5 + 8}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 6 + 8}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 7 + 8}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 0 + 8}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 1 + 8}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 2 + 8}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 3 + 8}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 4 + 8}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 5 + 8}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 6 + 8}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 7 + 8}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 0 + 8}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 1 + 8}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 2 + 8}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 3 + 8}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 4 + 8}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 5 + 8}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 6 + 8}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 7 + 8}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 0 + 8}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 1 + 8}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 2 + 8}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 3 + 8}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 4 + 8}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 5 + 8}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 6 + 8}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 7 + 8}) == 3);
    }
    SECTION("even row")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_row_hex>>;

        const auto twoddwave_hex_4 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::FOUR);

        CHECK(twoddwave_hex_4.num_clocks == 4u);
        CHECK(twoddwave_hex_4.max_in_degree == 2u);
        CHECK(twoddwave_hex_4.max_out_degree == 2u);
        CHECK(twoddwave_hex_4.is_regular());

        CHECK(twoddwave_hex_4({0, 0}) == 0);
        CHECK(twoddwave_hex_4({0, 1}) == 0);
        CHECK(twoddwave_hex_4({0, 2}) == 1);
        CHECK(twoddwave_hex_4({0, 3}) == 1);
        CHECK(twoddwave_hex_4({0, 4}) == 2);
        CHECK(twoddwave_hex_4({0, 5}) == 2);
        CHECK(twoddwave_hex_4({0, 6}) == 3);
        CHECK(twoddwave_hex_4({0, 7}) == 3);
        CHECK(twoddwave_hex_4({1, 0}) == 1);
        CHECK(twoddwave_hex_4({1, 1}) == 1);
        CHECK(twoddwave_hex_4({1, 2}) == 2);
        CHECK(twoddwave_hex_4({1, 3}) == 2);
        CHECK(twoddwave_hex_4({1, 4}) == 3);
        CHECK(twoddwave_hex_4({1, 5}) == 3);
        CHECK(twoddwave_hex_4({1, 6}) == 0);
        CHECK(twoddwave_hex_4({1, 7}) == 0);
        CHECK(twoddwave_hex_4({2, 0}) == 2);
        CHECK(twoddwave_hex_4({2, 1}) == 2);
        CHECK(twoddwave_hex_4({2, 2}) == 3);
        CHECK(twoddwave_hex_4({2, 3}) == 3);
        CHECK(twoddwave_hex_4({2, 4}) == 0);
        CHECK(twoddwave_hex_4({2, 5}) == 0);
        CHECK(twoddwave_hex_4({2, 6}) == 1);
        CHECK(twoddwave_hex_4({2, 7}) == 1);
        CHECK(twoddwave_hex_4({3, 0}) == 3);
        CHECK(twoddwave_hex_4({3, 1}) == 3);
        CHECK(twoddwave_hex_4({3, 2}) == 0);
        CHECK(twoddwave_hex_4({3, 3}) == 0);
        CHECK(twoddwave_hex_4({3, 4}) == 1);
        CHECK(twoddwave_hex_4({3, 5}) == 1);
        CHECK(twoddwave_hex_4({3, 6}) == 2);
        CHECK(twoddwave_hex_4({3, 7}) == 2);

        CHECK(twoddwave_hex_4({0 + 4, 0}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 1}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 2}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 3}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 4}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 5}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 6}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 7}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 0}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 1}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 2}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 3}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 4}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 5}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 6}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 7}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 0}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 1}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 2}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 3}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 4}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 5}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 6}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 7}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 0}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 1}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 2}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 3}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 4}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 5}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 6}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 7}) == 2);

        CHECK(twoddwave_hex_4({0, 0 + 8}) == 0);
        CHECK(twoddwave_hex_4({0, 1 + 8}) == 0);
        CHECK(twoddwave_hex_4({0, 2 + 8}) == 1);
        CHECK(twoddwave_hex_4({0, 3 + 8}) == 1);
        CHECK(twoddwave_hex_4({0, 4 + 8}) == 2);
        CHECK(twoddwave_hex_4({0, 5 + 8}) == 2);
        CHECK(twoddwave_hex_4({0, 6 + 8}) == 3);
        CHECK(twoddwave_hex_4({0, 7 + 8}) == 3);
        CHECK(twoddwave_hex_4({1, 0 + 8}) == 1);
        CHECK(twoddwave_hex_4({1, 1 + 8}) == 1);
        CHECK(twoddwave_hex_4({1, 2 + 8}) == 2);
        CHECK(twoddwave_hex_4({1, 3 + 8}) == 2);
        CHECK(twoddwave_hex_4({1, 4 + 8}) == 3);
        CHECK(twoddwave_hex_4({1, 5 + 8}) == 3);
        CHECK(twoddwave_hex_4({1, 6 + 8}) == 0);
        CHECK(twoddwave_hex_4({1, 7 + 8}) == 0);
        CHECK(twoddwave_hex_4({2, 0 + 8}) == 2);
        CHECK(twoddwave_hex_4({2, 1 + 8}) == 2);
        CHECK(twoddwave_hex_4({2, 2 + 8}) == 3);
        CHECK(twoddwave_hex_4({2, 3 + 8}) == 3);
        CHECK(twoddwave_hex_4({2, 4 + 8}) == 0);
        CHECK(twoddwave_hex_4({2, 5 + 8}) == 0);
        CHECK(twoddwave_hex_4({2, 6 + 8}) == 1);
        CHECK(twoddwave_hex_4({2, 7 + 8}) == 1);
        CHECK(twoddwave_hex_4({3, 0 + 8}) == 3);
        CHECK(twoddwave_hex_4({3, 1 + 8}) == 3);
        CHECK(twoddwave_hex_4({3, 2 + 8}) == 0);
        CHECK(twoddwave_hex_4({3, 3 + 8}) == 0);
        CHECK(twoddwave_hex_4({3, 4 + 8}) == 1);
        CHECK(twoddwave_hex_4({3, 5 + 8}) == 1);
        CHECK(twoddwave_hex_4({3, 6 + 8}) == 2);
        CHECK(twoddwave_hex_4({3, 7 + 8}) == 2);

        CHECK(twoddwave_hex_4({0 + 4, 0 + 8}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 1 + 8}) == 0);
        CHECK(twoddwave_hex_4({0 + 4, 2 + 8}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 3 + 8}) == 1);
        CHECK(twoddwave_hex_4({0 + 4, 4 + 8}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 5 + 8}) == 2);
        CHECK(twoddwave_hex_4({0 + 4, 6 + 8}) == 3);
        CHECK(twoddwave_hex_4({0 + 4, 7 + 8}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 0 + 8}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 1 + 8}) == 1);
        CHECK(twoddwave_hex_4({1 + 4, 2 + 8}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 3 + 8}) == 2);
        CHECK(twoddwave_hex_4({1 + 4, 4 + 8}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 5 + 8}) == 3);
        CHECK(twoddwave_hex_4({1 + 4, 6 + 8}) == 0);
        CHECK(twoddwave_hex_4({1 + 4, 7 + 8}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 0 + 8}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 1 + 8}) == 2);
        CHECK(twoddwave_hex_4({2 + 4, 2 + 8}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 3 + 8}) == 3);
        CHECK(twoddwave_hex_4({2 + 4, 4 + 8}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 5 + 8}) == 0);
        CHECK(twoddwave_hex_4({2 + 4, 6 + 8}) == 1);
        CHECK(twoddwave_hex_4({2 + 4, 7 + 8}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 0 + 8}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 1 + 8}) == 3);
        CHECK(twoddwave_hex_4({3 + 4, 2 + 8}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 3 + 8}) == 0);
        CHECK(twoddwave_hex_4({3 + 4, 4 + 8}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 5 + 8}) == 1);
        CHECK(twoddwave_hex_4({3 + 4, 6 + 8}) == 2);
        CHECK(twoddwave_hex_4({3 + 4, 7 + 8}) == 2);
    }
    SECTION("odd column")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_column_hex>>;

        const auto twoddwave_hex_4 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::FOUR);

        CHECK(twoddwave_hex_4.num_clocks == 4u);
        CHECK(twoddwave_hex_4.max_in_degree == 2u);
        CHECK(twoddwave_hex_4.max_out_degree == 2u);
        CHECK(twoddwave_hex_4.is_regular());

        CHECK(twoddwave_hex_4({0, 0}) == 0);
        CHECK(twoddwave_hex_4({0, 1}) == 1);
        CHECK(twoddwave_hex_4({0, 2}) == 2);
        CHECK(twoddwave_hex_4({0, 3}) == 3);
        CHECK(twoddwave_hex_4({1, 0}) == 1);
        CHECK(twoddwave_hex_4({1, 1}) == 2);
        CHECK(twoddwave_hex_4({1, 2}) == 3);
        CHECK(twoddwave_hex_4({1, 3}) == 0);
        CHECK(twoddwave_hex_4({2, 0}) == 1);
        CHECK(twoddwave_hex_4({2, 1}) == 2);
        CHECK(twoddwave_hex_4({2, 2}) == 3);
        CHECK(twoddwave_hex_4({2, 3}) == 0);
        CHECK(twoddwave_hex_4({3, 0}) == 2);
        CHECK(twoddwave_hex_4({3, 1}) == 3);
        CHECK(twoddwave_hex_4({3, 2}) == 0);
        CHECK(twoddwave_hex_4({3, 3}) == 1);
        CHECK(twoddwave_hex_4({4, 0}) == 2);
        CHECK(twoddwave_hex_4({4, 1}) == 3);
        CHECK(twoddwave_hex_4({4, 2}) == 0);
        CHECK(twoddwave_hex_4({4, 3}) == 1);
        CHECK(twoddwave_hex_4({5, 0}) == 3);
        CHECK(twoddwave_hex_4({5, 1}) == 0);
        CHECK(twoddwave_hex_4({5, 2}) == 1);
        CHECK(twoddwave_hex_4({5, 3}) == 2);
        CHECK(twoddwave_hex_4({6, 0}) == 3);
        CHECK(twoddwave_hex_4({6, 1}) == 0);
        CHECK(twoddwave_hex_4({6, 2}) == 1);
        CHECK(twoddwave_hex_4({6, 3}) == 2);
        CHECK(twoddwave_hex_4({7, 0}) == 0);
        CHECK(twoddwave_hex_4({7, 1}) == 1);
        CHECK(twoddwave_hex_4({7, 2}) == 2);
        CHECK(twoddwave_hex_4({7, 3}) == 3);

        CHECK(twoddwave_hex_4({0 + 8, 0}) == 0);
        CHECK(twoddwave_hex_4({0 + 8, 1}) == 1);
        CHECK(twoddwave_hex_4({0 + 8, 2}) == 2);
        CHECK(twoddwave_hex_4({0 + 8, 3}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 0}) == 1);
        CHECK(twoddwave_hex_4({1 + 8, 1}) == 2);
        CHECK(twoddwave_hex_4({1 + 8, 2}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 3}) == 0);
        CHECK(twoddwave_hex_4({2 + 8, 0}) == 1);
        CHECK(twoddwave_hex_4({2 + 8, 1}) == 2);
        CHECK(twoddwave_hex_4({2 + 8, 2}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 3}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 0}) == 2);
        CHECK(twoddwave_hex_4({3 + 8, 1}) == 3);
        CHECK(twoddwave_hex_4({3 + 8, 2}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 3}) == 1);
        CHECK(twoddwave_hex_4({4 + 8, 0}) == 2);
        CHECK(twoddwave_hex_4({4 + 8, 1}) == 3);
        CHECK(twoddwave_hex_4({4 + 8, 2}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 3}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 0}) == 3);
        CHECK(twoddwave_hex_4({5 + 8, 1}) == 0);
        CHECK(twoddwave_hex_4({5 + 8, 2}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 3}) == 2);
        CHECK(twoddwave_hex_4({6 + 8, 0}) == 3);
        CHECK(twoddwave_hex_4({6 + 8, 1}) == 0);
        CHECK(twoddwave_hex_4({6 + 8, 2}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 3}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 0}) == 0);
        CHECK(twoddwave_hex_4({7 + 8, 1}) == 1);
        CHECK(twoddwave_hex_4({7 + 8, 2}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 3}) == 3);

        CHECK(twoddwave_hex_4({0, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({0, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({0, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({0, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({1, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({1, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({1, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({1, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({2, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({2, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({2, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({2, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({3, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({3, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({3, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({3, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({4, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({4, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({4, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({4, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({5, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({5, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({5, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({5, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({6, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({6, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({6, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({6, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({7, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({7, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({7, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({7, 3 + 4}) == 3);

        CHECK(twoddwave_hex_4({0 + 8, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({0 + 8, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({0 + 8, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({0 + 8, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({1 + 8, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({1 + 8, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({2 + 8, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({2 + 8, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({2 + 8, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({3 + 8, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({3 + 8, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({4 + 8, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({4 + 8, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({4 + 8, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({5 + 8, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({5 + 8, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({6 + 8, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({6 + 8, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({6 + 8, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({7 + 8, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({7 + 8, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 3 + 4}) == 3);
    }
    SECTION("even column")
    {
        using clk_lyt =
            layouts::clocked_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_column_hex>>;

        const auto twoddwave_hex_4 = layouts::clocking::twoddwave_hex<clk_lyt>(layouts::clocking::num_clks::FOUR);

        CHECK(twoddwave_hex_4.num_clocks == 4u);
        CHECK(twoddwave_hex_4.max_in_degree == 2u);
        CHECK(twoddwave_hex_4.max_out_degree == 2u);
        CHECK(twoddwave_hex_4.is_regular());

        CHECK(twoddwave_hex_4({0, 0}) == 0);
        CHECK(twoddwave_hex_4({0, 1}) == 1);
        CHECK(twoddwave_hex_4({0, 2}) == 2);
        CHECK(twoddwave_hex_4({0, 3}) == 3);
        CHECK(twoddwave_hex_4({1, 0}) == 0);
        CHECK(twoddwave_hex_4({1, 1}) == 1);
        CHECK(twoddwave_hex_4({1, 2}) == 2);
        CHECK(twoddwave_hex_4({1, 3}) == 3);
        CHECK(twoddwave_hex_4({2, 0}) == 1);
        CHECK(twoddwave_hex_4({2, 1}) == 2);
        CHECK(twoddwave_hex_4({2, 2}) == 3);
        CHECK(twoddwave_hex_4({2, 3}) == 0);
        CHECK(twoddwave_hex_4({3, 0}) == 1);
        CHECK(twoddwave_hex_4({3, 1}) == 2);
        CHECK(twoddwave_hex_4({3, 2}) == 3);
        CHECK(twoddwave_hex_4({3, 3}) == 0);
        CHECK(twoddwave_hex_4({4, 0}) == 2);
        CHECK(twoddwave_hex_4({4, 1}) == 3);
        CHECK(twoddwave_hex_4({4, 2}) == 0);
        CHECK(twoddwave_hex_4({4, 3}) == 1);
        CHECK(twoddwave_hex_4({5, 0}) == 2);
        CHECK(twoddwave_hex_4({5, 1}) == 3);
        CHECK(twoddwave_hex_4({5, 2}) == 0);
        CHECK(twoddwave_hex_4({5, 3}) == 1);
        CHECK(twoddwave_hex_4({6, 0}) == 3);
        CHECK(twoddwave_hex_4({6, 1}) == 0);
        CHECK(twoddwave_hex_4({6, 2}) == 1);
        CHECK(twoddwave_hex_4({6, 3}) == 2);
        CHECK(twoddwave_hex_4({7, 0}) == 3);
        CHECK(twoddwave_hex_4({7, 1}) == 0);
        CHECK(twoddwave_hex_4({7, 2}) == 1);
        CHECK(twoddwave_hex_4({7, 3}) == 2);

        CHECK(twoddwave_hex_4({0 + 8, 0}) == 0);
        CHECK(twoddwave_hex_4({0 + 8, 1}) == 1);
        CHECK(twoddwave_hex_4({0 + 8, 2}) == 2);
        CHECK(twoddwave_hex_4({0 + 8, 3}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 0}) == 0);
        CHECK(twoddwave_hex_4({1 + 8, 1}) == 1);
        CHECK(twoddwave_hex_4({1 + 8, 2}) == 2);
        CHECK(twoddwave_hex_4({1 + 8, 3}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 0}) == 1);
        CHECK(twoddwave_hex_4({2 + 8, 1}) == 2);
        CHECK(twoddwave_hex_4({2 + 8, 2}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 3}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 0}) == 1);
        CHECK(twoddwave_hex_4({3 + 8, 1}) == 2);
        CHECK(twoddwave_hex_4({3 + 8, 2}) == 3);
        CHECK(twoddwave_hex_4({3 + 8, 3}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 0}) == 2);
        CHECK(twoddwave_hex_4({4 + 8, 1}) == 3);
        CHECK(twoddwave_hex_4({4 + 8, 2}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 3}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 0}) == 2);
        CHECK(twoddwave_hex_4({5 + 8, 1}) == 3);
        CHECK(twoddwave_hex_4({5 + 8, 2}) == 0);
        CHECK(twoddwave_hex_4({5 + 8, 3}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 0}) == 3);
        CHECK(twoddwave_hex_4({6 + 8, 1}) == 0);
        CHECK(twoddwave_hex_4({6 + 8, 2}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 3}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 0}) == 3);
        CHECK(twoddwave_hex_4({7 + 8, 1}) == 0);
        CHECK(twoddwave_hex_4({7 + 8, 2}) == 1);
        CHECK(twoddwave_hex_4({7 + 8, 3}) == 2);

        CHECK(twoddwave_hex_4({0, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({0, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({0, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({0, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({1, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({1, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({1, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({1, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({2, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({2, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({2, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({2, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({3, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({3, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({3, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({3, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({4, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({4, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({4, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({4, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({5, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({5, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({5, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({5, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({6, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({6, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({6, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({6, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({7, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({7, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({7, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({7, 3 + 4}) == 2);

        CHECK(twoddwave_hex_4({0 + 8, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({0 + 8, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({0 + 8, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({0 + 8, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({1 + 8, 0 + 4}) == 0);
        CHECK(twoddwave_hex_4({1 + 8, 1 + 4}) == 1);
        CHECK(twoddwave_hex_4({1 + 8, 2 + 4}) == 2);
        CHECK(twoddwave_hex_4({1 + 8, 3 + 4}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({2 + 8, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({2 + 8, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({2 + 8, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({3 + 8, 0 + 4}) == 1);
        CHECK(twoddwave_hex_4({3 + 8, 1 + 4}) == 2);
        CHECK(twoddwave_hex_4({3 + 8, 2 + 4}) == 3);
        CHECK(twoddwave_hex_4({3 + 8, 3 + 4}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({4 + 8, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({4 + 8, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({4 + 8, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({5 + 8, 0 + 4}) == 2);
        CHECK(twoddwave_hex_4({5 + 8, 1 + 4}) == 3);
        CHECK(twoddwave_hex_4({5 + 8, 2 + 4}) == 0);
        CHECK(twoddwave_hex_4({5 + 8, 3 + 4}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({6 + 8, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({6 + 8, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({6 + 8, 3 + 4}) == 2);
        CHECK(twoddwave_hex_4({7 + 8, 0 + 4}) == 3);
        CHECK(twoddwave_hex_4({7 + 8, 1 + 4}) == 0);
        CHECK(twoddwave_hex_4({7 + 8, 2 + 4}) == 1);
        CHECK(twoddwave_hex_4({7 + 8, 3 + 4}) == 2);
    }
}

TEST_CASE("4-phase USE", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto use4 = layouts::clocking::use<clk_lyt>();

    CHECK(use4.num_clocks == 4u);
    CHECK(use4.max_in_degree == 2u);
    CHECK(use4.max_out_degree == 2u);
    CHECK(use4.is_regular());

    CHECK(use4({0, 0}) == 0);
    CHECK(use4({0, 1}) == 3);
    CHECK(use4({0, 2}) == 2);
    CHECK(use4({0, 3}) == 1);
    CHECK(use4({1, 0}) == 1);
    CHECK(use4({1, 1}) == 2);
    CHECK(use4({1, 2}) == 3);
    CHECK(use4({1, 3}) == 0);
    CHECK(use4({2, 0}) == 2);
    CHECK(use4({2, 1}) == 1);
    CHECK(use4({2, 2}) == 0);
    CHECK(use4({2, 3}) == 3);
    CHECK(use4({3, 0}) == 3);
    CHECK(use4({3, 1}) == 0);
    CHECK(use4({3, 2}) == 1);
    CHECK(use4({3, 3}) == 2);

    CHECK(use4({0 + 4, 0}) == 0);
    CHECK(use4({0 + 4, 1}) == 3);
    CHECK(use4({0 + 4, 2}) == 2);
    CHECK(use4({0 + 4, 3}) == 1);
    CHECK(use4({1 + 4, 0}) == 1);
    CHECK(use4({1 + 4, 1}) == 2);
    CHECK(use4({1 + 4, 2}) == 3);
    CHECK(use4({1 + 4, 3}) == 0);
    CHECK(use4({2 + 4, 0}) == 2);
    CHECK(use4({2 + 4, 1}) == 1);
    CHECK(use4({2 + 4, 2}) == 0);
    CHECK(use4({2 + 4, 3}) == 3);
    CHECK(use4({3 + 4, 0}) == 3);
    CHECK(use4({3 + 4, 1}) == 0);
    CHECK(use4({3 + 4, 2}) == 1);
    CHECK(use4({3 + 4, 3}) == 2);

    CHECK(use4({0, 0 + 4}) == 0);
    CHECK(use4({0, 1 + 4}) == 3);
    CHECK(use4({0, 2 + 4}) == 2);
    CHECK(use4({0, 3 + 4}) == 1);
    CHECK(use4({1, 0 + 4}) == 1);
    CHECK(use4({1, 1 + 4}) == 2);
    CHECK(use4({1, 2 + 4}) == 3);
    CHECK(use4({1, 3 + 4}) == 0);
    CHECK(use4({2, 0 + 4}) == 2);
    CHECK(use4({2, 1 + 4}) == 1);
    CHECK(use4({2, 2 + 4}) == 0);
    CHECK(use4({2, 3 + 4}) == 3);
    CHECK(use4({3, 0 + 4}) == 3);
    CHECK(use4({3, 1 + 4}) == 0);
    CHECK(use4({3, 2 + 4}) == 1);
    CHECK(use4({3, 3 + 4}) == 2);

    CHECK(use4({0 + 4, 0 + 4}) == 0);
    CHECK(use4({0 + 4, 1 + 4}) == 3);
    CHECK(use4({0 + 4, 2 + 4}) == 2);
    CHECK(use4({0 + 4, 3 + 4}) == 1);
    CHECK(use4({1 + 4, 0 + 4}) == 1);
    CHECK(use4({1 + 4, 1 + 4}) == 2);
    CHECK(use4({1 + 4, 2 + 4}) == 3);
    CHECK(use4({1 + 4, 3 + 4}) == 0);
    CHECK(use4({2 + 4, 0 + 4}) == 2);
    CHECK(use4({2 + 4, 1 + 4}) == 1);
    CHECK(use4({2 + 4, 2 + 4}) == 0);
    CHECK(use4({2 + 4, 3 + 4}) == 3);
    CHECK(use4({3 + 4, 0 + 4}) == 3);
    CHECK(use4({3 + 4, 1 + 4}) == 0);
    CHECK(use4({3 + 4, 2 + 4}) == 1);
    CHECK(use4({3 + 4, 3 + 4}) == 2);
}

TEST_CASE("4-phase RES", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto res4 = layouts::clocking::res<clk_lyt>();

    CHECK(res4.num_clocks == 4u);
    CHECK(res4.max_in_degree == 3u);
    CHECK(res4.max_out_degree == 3u);
    CHECK(res4.is_regular());

    CHECK(res4({0, 0}) == 3);
    CHECK(res4({0, 1}) == 0);
    CHECK(res4({0, 2}) == 1);
    CHECK(res4({0, 3}) == 0);
    CHECK(res4({1, 0}) == 0);
    CHECK(res4({1, 1}) == 1);
    CHECK(res4({1, 2}) == 2);
    CHECK(res4({1, 3}) == 3);
    CHECK(res4({2, 0}) == 1);
    CHECK(res4({2, 1}) == 0);
    CHECK(res4({2, 2}) == 3);
    CHECK(res4({2, 3}) == 2);
    CHECK(res4({3, 0}) == 2);
    CHECK(res4({3, 1}) == 3);
    CHECK(res4({3, 2}) == 0);
    CHECK(res4({3, 3}) == 1);

    CHECK(res4({0 + 4, 0}) == 3);
    CHECK(res4({0 + 4, 1}) == 0);
    CHECK(res4({0 + 4, 2}) == 1);
    CHECK(res4({0 + 4, 3}) == 0);
    CHECK(res4({1 + 4, 0}) == 0);
    CHECK(res4({1 + 4, 1}) == 1);
    CHECK(res4({1 + 4, 2}) == 2);
    CHECK(res4({1 + 4, 3}) == 3);
    CHECK(res4({2 + 4, 0}) == 1);
    CHECK(res4({2 + 4, 1}) == 0);
    CHECK(res4({2 + 4, 2}) == 3);
    CHECK(res4({2 + 4, 3}) == 2);
    CHECK(res4({3 + 4, 0}) == 2);
    CHECK(res4({3 + 4, 1}) == 3);
    CHECK(res4({3 + 4, 2}) == 0);
    CHECK(res4({3 + 4, 3}) == 1);

    CHECK(res4({0, 0 + 4}) == 3);
    CHECK(res4({0, 1 + 4}) == 0);
    CHECK(res4({0, 2 + 4}) == 1);
    CHECK(res4({0, 3 + 4}) == 0);
    CHECK(res4({1, 0 + 4}) == 0);
    CHECK(res4({1, 1 + 4}) == 1);
    CHECK(res4({1, 2 + 4}) == 2);
    CHECK(res4({1, 3 + 4}) == 3);
    CHECK(res4({2, 0 + 4}) == 1);
    CHECK(res4({2, 1 + 4}) == 0);
    CHECK(res4({2, 2 + 4}) == 3);
    CHECK(res4({2, 3 + 4}) == 2);
    CHECK(res4({3, 0 + 4}) == 2);
    CHECK(res4({3, 1 + 4}) == 3);
    CHECK(res4({3, 2 + 4}) == 0);
    CHECK(res4({3, 3 + 4}) == 1);

    CHECK(res4({0 + 4, 0 + 4}) == 3);
    CHECK(res4({0 + 4, 1 + 4}) == 0);
    CHECK(res4({0 + 4, 2 + 4}) == 1);
    CHECK(res4({0 + 4, 3 + 4}) == 0);
    CHECK(res4({1 + 4, 0 + 4}) == 0);
    CHECK(res4({1 + 4, 1 + 4}) == 1);
    CHECK(res4({1 + 4, 2 + 4}) == 2);
    CHECK(res4({1 + 4, 3 + 4}) == 3);
    CHECK(res4({2 + 4, 0 + 4}) == 1);
    CHECK(res4({2 + 4, 1 + 4}) == 0);
    CHECK(res4({2 + 4, 2 + 4}) == 3);
    CHECK(res4({2 + 4, 3 + 4}) == 2);
    CHECK(res4({3 + 4, 0 + 4}) == 2);
    CHECK(res4({3 + 4, 1 + 4}) == 3);
    CHECK(res4({3 + 4, 2 + 4}) == 0);
    CHECK(res4({3 + 4, 3 + 4}) == 1);
}

TEST_CASE("4-phase CFE", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto cfe4 = layouts::clocking::cfe<clk_lyt>();

    CHECK(cfe4.num_clocks == 4u);
    CHECK(cfe4.max_in_degree == 3u);
    CHECK(cfe4.max_out_degree == 3u);
    CHECK(cfe4.is_regular());

    CHECK(cfe4({0, 0}) == 0);
    CHECK(cfe4({0, 1}) == 3);
    CHECK(cfe4({0, 2}) == 0);
    CHECK(cfe4({0, 3}) == 3);
    CHECK(cfe4({1, 0}) == 1);
    CHECK(cfe4({1, 1}) == 2);
    CHECK(cfe4({1, 2}) == 1);
    CHECK(cfe4({1, 3}) == 2);
    CHECK(cfe4({2, 0}) == 0);
    CHECK(cfe4({2, 1}) == 3);
    CHECK(cfe4({2, 2}) == 0);
    CHECK(cfe4({2, 3}) == 3);
    CHECK(cfe4({3, 0}) == 1);
    CHECK(cfe4({3, 1}) == 2);
    CHECK(cfe4({3, 2}) == 1);
    CHECK(cfe4({3, 3}) == 2);

    CHECK(cfe4({0 + 4, 0}) == 0);
    CHECK(cfe4({0 + 4, 1}) == 3);
    CHECK(cfe4({0 + 4, 2}) == 0);
    CHECK(cfe4({0 + 4, 3}) == 3);
    CHECK(cfe4({1 + 4, 0}) == 1);
    CHECK(cfe4({1 + 4, 1}) == 2);
    CHECK(cfe4({1 + 4, 2}) == 1);
    CHECK(cfe4({1 + 4, 3}) == 2);
    CHECK(cfe4({2 + 4, 0}) == 0);
    CHECK(cfe4({2 + 4, 1}) == 3);
    CHECK(cfe4({2 + 4, 2}) == 0);
    CHECK(cfe4({2 + 4, 3}) == 3);
    CHECK(cfe4({3 + 4, 0}) == 1);
    CHECK(cfe4({3 + 4, 1}) == 2);
    CHECK(cfe4({3 + 4, 2}) == 1);
    CHECK(cfe4({3 + 4, 3}) == 2);

    CHECK(cfe4({0, 0 + 4}) == 0);
    CHECK(cfe4({0, 1 + 4}) == 3);
    CHECK(cfe4({0, 2 + 4}) == 0);
    CHECK(cfe4({0, 3 + 4}) == 3);
    CHECK(cfe4({1, 0 + 4}) == 1);
    CHECK(cfe4({1, 1 + 4}) == 2);
    CHECK(cfe4({1, 2 + 4}) == 1);
    CHECK(cfe4({1, 3 + 4}) == 2);
    CHECK(cfe4({2, 0 + 4}) == 0);
    CHECK(cfe4({2, 1 + 4}) == 3);
    CHECK(cfe4({2, 2 + 4}) == 0);
    CHECK(cfe4({2, 3 + 4}) == 3);
    CHECK(cfe4({3, 0 + 4}) == 1);
    CHECK(cfe4({3, 1 + 4}) == 2);
    CHECK(cfe4({3, 2 + 4}) == 1);
    CHECK(cfe4({3, 3 + 4}) == 2);

    CHECK(cfe4({0 + 4, 0 + 4}) == 0);
    CHECK(cfe4({0 + 4, 1 + 4}) == 3);
    CHECK(cfe4({0 + 4, 2 + 4}) == 0);
    CHECK(cfe4({0 + 4, 3 + 4}) == 3);
    CHECK(cfe4({1 + 4, 0 + 4}) == 1);
    CHECK(cfe4({1 + 4, 1 + 4}) == 2);
    CHECK(cfe4({1 + 4, 2 + 4}) == 1);
    CHECK(cfe4({1 + 4, 3 + 4}) == 2);
    CHECK(cfe4({2 + 4, 0 + 4}) == 0);
    CHECK(cfe4({2 + 4, 1 + 4}) == 3);
    CHECK(cfe4({2 + 4, 2 + 4}) == 0);
    CHECK(cfe4({2 + 4, 3 + 4}) == 3);
    CHECK(cfe4({3 + 4, 0 + 4}) == 1);
    CHECK(cfe4({3 + 4, 1 + 4}) == 2);
    CHECK(cfe4({3 + 4, 2 + 4}) == 1);
    CHECK(cfe4({3 + 4, 3 + 4}) == 2);
}

TEST_CASE("4-phase Ripple", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto ripple4 = layouts::clocking::ripple<clk_lyt>();

    CHECK(ripple4.num_clocks == 4u);
    CHECK(ripple4.max_in_degree == 3u);
    CHECK(ripple4.max_out_degree == 3u);
    CHECK(ripple4.is_regular());

    CHECK(ripple4({0, 0}) == 0);
    CHECK(ripple4({0, 1}) == 3);
    CHECK(ripple4({0, 2}) == 0);
    CHECK(ripple4({0, 3}) == 3);
    CHECK(ripple4({1, 0}) == 1);
    CHECK(ripple4({1, 1}) == 2);
    CHECK(ripple4({1, 2}) == 1);
    CHECK(ripple4({1, 3}) == 2);
    CHECK(ripple4({2, 0}) == 2);
    CHECK(ripple4({2, 1}) == 1);
    CHECK(ripple4({2, 2}) == 2);
    CHECK(ripple4({2, 3}) == 1);
    CHECK(ripple4({3, 0}) == 3);
    CHECK(ripple4({3, 1}) == 0);
    CHECK(ripple4({3, 2}) == 3);
    CHECK(ripple4({3, 3}) == 0);

    CHECK(ripple4({0 + 4, 0}) == 0);
    CHECK(ripple4({0 + 4, 1}) == 3);
    CHECK(ripple4({0 + 4, 2}) == 0);
    CHECK(ripple4({0 + 4, 3}) == 3);
    CHECK(ripple4({1 + 4, 0}) == 1);
    CHECK(ripple4({1 + 4, 1}) == 2);
    CHECK(ripple4({1 + 4, 2}) == 1);
    CHECK(ripple4({1 + 4, 3}) == 2);
    CHECK(ripple4({2 + 4, 0}) == 2);
    CHECK(ripple4({2 + 4, 1}) == 1);
    CHECK(ripple4({2 + 4, 2}) == 2);
    CHECK(ripple4({2 + 4, 3}) == 1);
    CHECK(ripple4({3 + 4, 0}) == 3);
    CHECK(ripple4({3 + 4, 1}) == 0);
    CHECK(ripple4({3 + 4, 2}) == 3);
    CHECK(ripple4({3 + 4, 3}) == 0);

    CHECK(ripple4({0, 0 + 4}) == 0);
    CHECK(ripple4({0, 1 + 4}) == 3);
    CHECK(ripple4({0, 2 + 4}) == 0);
    CHECK(ripple4({0, 3 + 4}) == 3);
    CHECK(ripple4({1, 0 + 4}) == 1);
    CHECK(ripple4({1, 1 + 4}) == 2);
    CHECK(ripple4({1, 2 + 4}) == 1);
    CHECK(ripple4({1, 3 + 4}) == 2);
    CHECK(ripple4({2, 0 + 4}) == 2);
    CHECK(ripple4({2, 1 + 4}) == 1);
    CHECK(ripple4({2, 2 + 4}) == 2);
    CHECK(ripple4({2, 3 + 4}) == 1);
    CHECK(ripple4({3, 0 + 4}) == 3);
    CHECK(ripple4({3, 1 + 4}) == 0);
    CHECK(ripple4({3, 2 + 4}) == 3);
    CHECK(ripple4({3, 3 + 4}) == 0);

    CHECK(ripple4({0 + 4, 0 + 4}) == 0);
    CHECK(ripple4({0 + 4, 1 + 4}) == 3);
    CHECK(ripple4({0 + 4, 2 + 4}) == 0);
    CHECK(ripple4({0 + 4, 3 + 4}) == 3);
    CHECK(ripple4({1 + 4, 0 + 4}) == 1);
    CHECK(ripple4({1 + 4, 1 + 4}) == 2);
    CHECK(ripple4({1 + 4, 2 + 4}) == 1);
    CHECK(ripple4({1 + 4, 3 + 4}) == 2);
    CHECK(ripple4({2 + 4, 0 + 4}) == 2);
    CHECK(ripple4({2 + 4, 1 + 4}) == 1);
    CHECK(ripple4({2 + 4, 2 + 4}) == 2);
    CHECK(ripple4({2 + 4, 3 + 4}) == 1);
    CHECK(ripple4({3 + 4, 0 + 4}) == 3);
    CHECK(ripple4({3 + 4, 1 + 4}) == 0);
    CHECK(ripple4({3 + 4, 2 + 4}) == 3);
    CHECK(ripple4({3 + 4, 3 + 4}) == 0);
}

TEST_CASE("4-phase SRS", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto srs4 = layouts::clocking::srs<clk_lyt>();

    CHECK(srs4.num_clocks == 4u);
    CHECK(srs4.max_in_degree == 3u);
    CHECK(srs4.max_out_degree == 3u);
    CHECK(srs4.is_regular());

    CHECK(srs4({0, 0}) == 1);
    CHECK(srs4({0, 1}) == 2);
    CHECK(srs4({0, 2}) == 3);
    CHECK(srs4({0, 3}) == 0);
    CHECK(srs4({1, 0}) == 0);
    CHECK(srs4({1, 1}) == 3);
    CHECK(srs4({1, 2}) == 2);
    CHECK(srs4({1, 3}) == 1);
    CHECK(srs4({2, 0}) == 3);
    CHECK(srs4({2, 1}) == 0);
    CHECK(srs4({2, 2}) == 3);
    CHECK(srs4({2, 3}) == 2);
    CHECK(srs4({3, 0}) == 2);
    CHECK(srs4({3, 1}) == 1);
    CHECK(srs4({3, 2}) == 0);
    CHECK(srs4({3, 3}) == 1);

    CHECK(srs4({0 + 4, 0}) == 1);
    CHECK(srs4({0 + 4, 1}) == 2);
    CHECK(srs4({0 + 4, 2}) == 3);
    CHECK(srs4({0 + 4, 3}) == 0);
    CHECK(srs4({1 + 4, 0}) == 0);
    CHECK(srs4({1 + 4, 1}) == 3);
    CHECK(srs4({1 + 4, 2}) == 2);
    CHECK(srs4({1 + 4, 3}) == 1);
    CHECK(srs4({2 + 4, 0}) == 3);
    CHECK(srs4({2 + 4, 1}) == 0);
    CHECK(srs4({2 + 4, 2}) == 3);
    CHECK(srs4({2 + 4, 3}) == 2);
    CHECK(srs4({3 + 4, 0}) == 2);
    CHECK(srs4({3 + 4, 1}) == 1);
    CHECK(srs4({3 + 4, 2}) == 0);
    CHECK(srs4({3 + 4, 3}) == 1);

    CHECK(srs4({0, 0 + 4}) == 1);
    CHECK(srs4({0, 1 + 4}) == 2);
    CHECK(srs4({0, 2 + 4}) == 3);
    CHECK(srs4({0, 3 + 4}) == 0);
    CHECK(srs4({1, 0 + 4}) == 0);
    CHECK(srs4({1, 1 + 4}) == 3);
    CHECK(srs4({1, 2 + 4}) == 2);
    CHECK(srs4({1, 3 + 4}) == 1);
    CHECK(srs4({2, 0 + 4}) == 3);
    CHECK(srs4({2, 1 + 4}) == 0);
    CHECK(srs4({2, 2 + 4}) == 3);
    CHECK(srs4({2, 3 + 4}) == 2);
    CHECK(srs4({3, 0 + 4}) == 2);
    CHECK(srs4({3, 1 + 4}) == 1);
    CHECK(srs4({3, 2 + 4}) == 0);
    CHECK(srs4({3, 3 + 4}) == 1);

    CHECK(srs4({0 + 4, 0 + 4}) == 1);
    CHECK(srs4({0 + 4, 1 + 4}) == 2);
    CHECK(srs4({0 + 4, 2 + 4}) == 3);
    CHECK(srs4({0 + 4, 3 + 4}) == 0);
    CHECK(srs4({1 + 4, 0 + 4}) == 0);
    CHECK(srs4({1 + 4, 1 + 4}) == 3);
    CHECK(srs4({1 + 4, 2 + 4}) == 2);
    CHECK(srs4({1 + 4, 3 + 4}) == 1);
    CHECK(srs4({2 + 4, 0 + 4}) == 3);
    CHECK(srs4({2 + 4, 1 + 4}) == 0);
    CHECK(srs4({2 + 4, 2 + 4}) == 3);
    CHECK(srs4({2 + 4, 3 + 4}) == 2);
    CHECK(srs4({3 + 4, 0 + 4}) == 2);
    CHECK(srs4({3 + 4, 1 + 4}) == 1);
    CHECK(srs4({3 + 4, 2 + 4}) == 0);
    CHECK(srs4({3 + 4, 3 + 4}) == 1);
}

TEST_CASE("3-phase BANCS", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto bancs3 = layouts::clocking::bancs<clk_lyt>();

    CHECK(bancs3.num_clocks == 3u);
    CHECK(bancs3.max_in_degree == 2u);
    CHECK(bancs3.max_out_degree == 2u);
    CHECK(bancs3.is_regular());

    CHECK(bancs3({0, 0}) == 0);
    CHECK(bancs3({0, 1}) == 2);
    CHECK(bancs3({0, 2}) == 2);
    CHECK(bancs3({0, 3}) == 1);
    CHECK(bancs3({0, 4}) == 1);
    CHECK(bancs3({0, 5}) == 0);
    CHECK(bancs3({1, 0}) == 1);
    CHECK(bancs3({1, 1}) == 1);
    CHECK(bancs3({1, 2}) == 0);
    CHECK(bancs3({1, 3}) == 0);
    CHECK(bancs3({1, 4}) == 2);
    CHECK(bancs3({1, 5}) == 2);
    CHECK(bancs3({2, 0}) == 2);
    CHECK(bancs3({2, 1}) == 0);
    CHECK(bancs3({2, 2}) == 1);
    CHECK(bancs3({2, 3}) == 2);
    CHECK(bancs3({2, 4}) == 0);
    CHECK(bancs3({2, 5}) == 1);

    CHECK(bancs3({0 + 3, 0}) == 0);
    CHECK(bancs3({0 + 3, 1}) == 2);
    CHECK(bancs3({0 + 3, 2}) == 2);
    CHECK(bancs3({0 + 3, 3}) == 1);
    CHECK(bancs3({0 + 3, 4}) == 1);
    CHECK(bancs3({0 + 3, 5}) == 0);
    CHECK(bancs3({1 + 3, 0}) == 1);
    CHECK(bancs3({1 + 3, 1}) == 1);
    CHECK(bancs3({1 + 3, 2}) == 0);
    CHECK(bancs3({1 + 3, 3}) == 0);
    CHECK(bancs3({1 + 3, 4}) == 2);
    CHECK(bancs3({1 + 3, 5}) == 2);
    CHECK(bancs3({2 + 3, 0}) == 2);
    CHECK(bancs3({2 + 3, 1}) == 0);
    CHECK(bancs3({2 + 3, 2}) == 1);
    CHECK(bancs3({2 + 3, 3}) == 2);
    CHECK(bancs3({2 + 3, 4}) == 0);
    CHECK(bancs3({2 + 3, 5}) == 1);

    CHECK(bancs3({0, 0 + 6}) == 0);
    CHECK(bancs3({0, 1 + 6}) == 2);
    CHECK(bancs3({0, 2 + 6}) == 2);
    CHECK(bancs3({0, 3 + 6}) == 1);
    CHECK(bancs3({0, 4 + 6}) == 1);
    CHECK(bancs3({0, 5 + 6}) == 0);
    CHECK(bancs3({1, 0 + 6}) == 1);
    CHECK(bancs3({1, 1 + 6}) == 1);
    CHECK(bancs3({1, 2 + 6}) == 0);
    CHECK(bancs3({1, 3 + 6}) == 0);
    CHECK(bancs3({1, 4 + 6}) == 2);
    CHECK(bancs3({1, 5 + 6}) == 2);
    CHECK(bancs3({2, 0 + 6}) == 2);
    CHECK(bancs3({2, 1 + 6}) == 0);
    CHECK(bancs3({2, 2 + 6}) == 1);
    CHECK(bancs3({2, 3 + 6}) == 2);
    CHECK(bancs3({2, 4 + 6}) == 0);
    CHECK(bancs3({2, 5 + 6}) == 1);

    CHECK(bancs3({0 + 3, 0 + 6}) == 0);
    CHECK(bancs3({0 + 3, 1 + 6}) == 2);
    CHECK(bancs3({0 + 3, 2 + 6}) == 2);
    CHECK(bancs3({0 + 3, 3 + 6}) == 1);
    CHECK(bancs3({0 + 3, 4 + 6}) == 1);
    CHECK(bancs3({0 + 3, 5 + 6}) == 0);
    CHECK(bancs3({1 + 3, 0 + 6}) == 1);
    CHECK(bancs3({1 + 3, 1 + 6}) == 1);
    CHECK(bancs3({1 + 3, 2 + 6}) == 0);
    CHECK(bancs3({1 + 3, 3 + 6}) == 0);
    CHECK(bancs3({1 + 3, 4 + 6}) == 2);
    CHECK(bancs3({1 + 3, 5 + 6}) == 2);
    CHECK(bancs3({2 + 3, 0 + 6}) == 2);
    CHECK(bancs3({2 + 3, 1 + 6}) == 0);
    CHECK(bancs3({2 + 3, 2 + 6}) == 1);
    CHECK(bancs3({2 + 3, 3 + 6}) == 2);
    CHECK(bancs3({2 + 3, 4 + 6}) == 0);
    CHECK(bancs3({2 + 3, 5 + 6}) == 1);
}

TEST_CASE("Override clocking", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    auto twoddwave4 = layouts::clocking::twoddwave<clk_lyt>();

    CHECK(twoddwave4.is_regular());

    twoddwave4.override_clock_number({0, 0}, 0);

    CHECK(!twoddwave4.is_regular());

    twoddwave4.override_clock_number({0, 1}, 1);
    twoddwave4.override_clock_number({0, 2}, 2);
    twoddwave4.override_clock_number({0, 3}, 3);
    twoddwave4.override_clock_number({0, 4}, 4);
    twoddwave4.override_clock_number({0, 5}, 5);
    twoddwave4.override_clock_number({0, 6}, 6);

    CHECK(twoddwave4({0, 0}) == 0);
    CHECK(twoddwave4({0, 1}) == 1);
    CHECK(twoddwave4({0, 2}) == 2);
    CHECK(twoddwave4({0, 3}) == 3);
    CHECK(twoddwave4({0, 4}) == 0);
    CHECK(twoddwave4({0, 5}) == 1);
    CHECK(twoddwave4({0, 6}) == 2);
    CHECK(twoddwave4({1, 0}) == 1);
    CHECK(twoddwave4({1, 1}) == 2);
    CHECK(twoddwave4({1, 2}) == 3);
    CHECK(twoddwave4({1, 3}) == 0);
    CHECK(twoddwave4({2, 0}) == 2);
    CHECK(twoddwave4({2, 1}) == 3);
    CHECK(twoddwave4({2, 2}) == 0);
    CHECK(twoddwave4({2, 3}) == 1);
    CHECK(twoddwave4({3, 0}) == 3);
    CHECK(twoddwave4({3, 1}) == 0);
    CHECK(twoddwave4({3, 2}) == 1);
    CHECK(twoddwave4({3, 3}) == 2);
}

TEST_CASE("4-phase ESR", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    const auto esr4 = layouts::clocking::esr<clk_lyt>();

    CHECK(esr4.num_clocks == 4u);
    CHECK(esr4.max_in_degree == 3u);
    CHECK(esr4.max_out_degree == 3u);
    CHECK(esr4.is_regular());

    CHECK(esr4({0, 0}) == 3);
    CHECK(esr4({0, 1}) == 0);
    CHECK(esr4({0, 2}) == 1);
    CHECK(esr4({0, 3}) == 0);
    CHECK(esr4({1, 0}) == 0);
    CHECK(esr4({1, 1}) == 1);
    CHECK(esr4({1, 2}) == 2);
    CHECK(esr4({1, 3}) == 3);
    CHECK(esr4({2, 0}) == 1);
    CHECK(esr4({2, 1}) == 2);
    CHECK(esr4({2, 2}) == 3);
    CHECK(esr4({2, 3}) == 2);
    CHECK(esr4({3, 0}) == 2);
    CHECK(esr4({3, 1}) == 3);
    CHECK(esr4({3, 2}) == 0);
    CHECK(esr4({3, 3}) == 1);

    CHECK(esr4({0 + 4, 0}) == 3);
    CHECK(esr4({0 + 4, 1}) == 0);
    CHECK(esr4({0 + 4, 2}) == 1);
    CHECK(esr4({0 + 4, 3}) == 0);
    CHECK(esr4({1 + 4, 0}) == 0);
    CHECK(esr4({1 + 4, 1}) == 1);
    CHECK(esr4({1 + 4, 2}) == 2);
    CHECK(esr4({1 + 4, 3}) == 3);
    CHECK(esr4({2 + 4, 0}) == 1);
    CHECK(esr4({2 + 4, 1}) == 2);
    CHECK(esr4({2 + 4, 2}) == 3);
    CHECK(esr4({2 + 4, 3}) == 2);
    CHECK(esr4({3 + 4, 0}) == 2);
    CHECK(esr4({3 + 4, 1}) == 3);
    CHECK(esr4({3 + 4, 2}) == 0);
    CHECK(esr4({3 + 4, 3}) == 1);

    CHECK(esr4({0, 0 + 4}) == 3);
    CHECK(esr4({0, 1 + 4}) == 0);
    CHECK(esr4({0, 2 + 4}) == 1);
    CHECK(esr4({0, 3 + 4}) == 0);
    CHECK(esr4({1, 0 + 4}) == 0);
    CHECK(esr4({1, 1 + 4}) == 1);
    CHECK(esr4({1, 2 + 4}) == 2);
    CHECK(esr4({1, 3 + 4}) == 3);
    CHECK(esr4({2, 0 + 4}) == 1);
    CHECK(esr4({2, 1 + 4}) == 2);
    CHECK(esr4({2, 2 + 4}) == 3);
    CHECK(esr4({2, 3 + 4}) == 2);
    CHECK(esr4({3, 0 + 4}) == 2);
    CHECK(esr4({3, 1 + 4}) == 3);
    CHECK(esr4({3, 2 + 4}) == 0);
    CHECK(esr4({3, 3 + 4}) == 1);

    CHECK(esr4({0 + 4, 0 + 4}) == 3);
    CHECK(esr4({0 + 4, 1 + 4}) == 0);
    CHECK(esr4({0 + 4, 2 + 4}) == 1);
    CHECK(esr4({0 + 4, 3 + 4}) == 0);
    CHECK(esr4({1 + 4, 0 + 4}) == 0);
    CHECK(esr4({1 + 4, 1 + 4}) == 1);
    CHECK(esr4({1 + 4, 2 + 4}) == 2);
    CHECK(esr4({1 + 4, 3 + 4}) == 3);
    CHECK(esr4({2 + 4, 0 + 4}) == 1);
    CHECK(esr4({2 + 4, 1 + 4}) == 2);
    CHECK(esr4({2 + 4, 2 + 4}) == 3);
    CHECK(esr4({2 + 4, 3 + 4}) == 2);
    CHECK(esr4({3 + 4, 0 + 4}) == 2);
    CHECK(esr4({3 + 4, 1 + 4}) == 3);
    CHECK(esr4({3 + 4, 2 + 4}) == 0);
    CHECK(esr4({3 + 4, 3 + 4}) == 1);
}

TEST_CASE("Clocking lookup", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    auto check = [](const std::vector<std::string>& vec, const auto& name)
    {
        for (const auto& n : vec)
        {
            auto cs = layouts::clocking::get_scheme<clk_lyt>(n);
            REQUIRE(cs.has_value());
            CHECK(*cs == name);
        }
    };

    check({"open", "OPEN", "oPeN", "OpEn"}, layouts::clocking::OPEN_NAME);
    check({"columnar", "COLUMNAR", "CoLumNar", "COLUMnar"}, layouts::clocking::COLUMNAR_NAME);
    check({"row", "ROW", "RoW", "rOw"}, layouts::clocking::ROW_NAME);
    check({"2DDwave", "2DdWaVe", "2ddwave", "2DDWAVE", "2DDWave"}, layouts::clocking::TWODDWAVE_NAME);
    check({"2DDwavehex", "2DdWaVeHeX", "2ddwavehex", "2DDWAVEHEX", "2DDWaveHex"}, layouts::clocking::TWODDWAVE_NAME);
    check({"use", "USE", "uSe", "UsE"}, layouts::clocking::USE_NAME);
    check({"res", "RES", "rEs", "ReS"}, layouts::clocking::RES_NAME);
    check({"esr", "ESR", "eSr", "EsR"}, layouts::clocking::ESR_NAME);
    check({"cfe", "CFE", "cFe", "CfE"}, layouts::clocking::CFE_NAME);
    check({"ripple", "RIPPLE", "RiPpLe", "RIppLE"}, layouts::clocking::RIPPLE_NAME);
    check({"srs", "SRS", "sRs", "SrS"}, layouts::clocking::SRS_NAME);
    check({"bancs", "BANCS", "BaNCs", "banCS"}, layouts::clocking::BANCS_NAME);

    CHECK(!layouts::clocking::get_scheme<clk_lyt>("").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("Column").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("Rows").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("TwoDDWave").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("2DDWave6").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("SUE").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("SER").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("ERS").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("CEF").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("RPIPLE").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("SSR").has_value());
    CHECK(!layouts::clocking::get_scheme<clk_lyt>("BNCS").has_value());
}

TEST_CASE("Linear schemes", "[clocking-scheme]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;

    auto check_linear_scheme = [](const auto& name, bool expected)
    {
        auto cs = layouts::clocking::get_scheme<clk_lyt>(name);
        REQUIRE(cs.has_value());
        CHECK(layouts::clocking::is_linear<clk_lyt>(*cs) == expected);
    };

    // Linear clocking schemes
    check_linear_scheme(layouts::clocking::COLUMNAR_NAME, true);
    check_linear_scheme(layouts::clocking::ROW_NAME, true);
    check_linear_scheme(layouts::clocking::TWODDWAVE_NAME, true);
    check_linear_scheme(layouts::clocking::TWODDWAVE_HEX_NAME, true);

    // Non-linear clocking schemes
    check_linear_scheme(layouts::clocking::OPEN_NAME, false);
    check_linear_scheme(layouts::clocking::USE_NAME, false);
    check_linear_scheme(layouts::clocking::RES_NAME, false);
    check_linear_scheme(layouts::clocking::ESR_NAME, false);
    check_linear_scheme(layouts::clocking::CFE_NAME, false);
    check_linear_scheme(layouts::clocking::RIPPLE_NAME, false);
    check_linear_scheme(layouts::clocking::SRS_NAME, false);
    check_linear_scheme(layouts::clocking::BANCS_NAME, false);
}
