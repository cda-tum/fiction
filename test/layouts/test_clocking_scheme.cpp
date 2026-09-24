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
 * @brief Tests for `fiction/layouts/clocking_scheme.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("3-phase open clocking", "[clocking-scheme]")
{
    auto open3 = clocking::open(clocking::num_clks::THREE);

    CHECK(open3.num_clocks() == 3u);
    CHECK(open3.max_in_degree() == clocking::scheme::UNBOUNDED);
    CHECK(open3.max_out_degree() == clocking::scheme::UNBOUNDED);
    CHECK(!open3.is_regular());

    CHECK(open3(0, 0) == 0);
    CHECK(open3(0, 1) == 0);
    CHECK(open3(0, 2) == 0);
    CHECK(open3(0, 3) == 0);
    CHECK(open3(1, 0) == 0);
    CHECK(open3(1, 1) == 0);
    CHECK(open3(1, 2) == 0);
    CHECK(open3(1, 3) == 0);
    CHECK(open3(2, 0) == 0);
    CHECK(open3(2, 1) == 0);
    CHECK(open3(2, 2) == 0);
    CHECK(open3(2, 3) == 0);
    CHECK(open3(3, 0) == 0);
    CHECK(open3(3, 1) == 0);
    CHECK(open3(3, 2) == 0);
    CHECK(open3(3, 3) == 0);

    open3.override_clock_number(0, 0, 0);
    open3.override_clock_number(0, 1, 1);
    open3.override_clock_number(0, 2, 2);
    open3.override_clock_number(0, 3, 3);
    open3.override_clock_number(0, 4, 4);
    open3.override_clock_number(0, 5, 5);
    open3.override_clock_number(0, 6, 6);

    CHECK(!open3.is_regular());

    CHECK(open3(0, 0) == 0);
    CHECK(open3(0, 1) == 1);
    CHECK(open3(0, 2) == 2);
    CHECK(open3(0, 3) == 0);
    CHECK(open3(0, 4) == 1);
    CHECK(open3(0, 5) == 2);
    CHECK(open3(0, 6) == 0);
    CHECK(open3(1, 0) == 0);
    CHECK(open3(1, 1) == 0);
    CHECK(open3(1, 2) == 0);
    CHECK(open3(1, 3) == 0);
    CHECK(open3(2, 0) == 0);
    CHECK(open3(2, 1) == 0);
    CHECK(open3(2, 2) == 0);
    CHECK(open3(2, 3) == 0);
    CHECK(open3(3, 0) == 0);
    CHECK(open3(3, 1) == 0);
    CHECK(open3(3, 2) == 0);
    CHECK(open3(3, 3) == 0);
}

TEST_CASE("4-phase open clocking", "[clocking-scheme]")
{
    auto open4 = clocking::open(clocking::num_clks::FOUR);

    CHECK(open4.num_clocks() == 4u);
    CHECK(open4.max_in_degree() == clocking::scheme::UNBOUNDED);
    CHECK(open4.max_out_degree() == clocking::scheme::UNBOUNDED);
    CHECK(!open4.is_regular());

    CHECK(open4(0, 0) == 0);
    CHECK(open4(0, 1) == 0);
    CHECK(open4(0, 2) == 0);
    CHECK(open4(0, 3) == 0);
    CHECK(open4(1, 0) == 0);
    CHECK(open4(1, 1) == 0);
    CHECK(open4(1, 2) == 0);
    CHECK(open4(1, 3) == 0);
    CHECK(open4(2, 0) == 0);
    CHECK(open4(2, 1) == 0);
    CHECK(open4(2, 2) == 0);
    CHECK(open4(2, 3) == 0);
    CHECK(open4(3, 0) == 0);
    CHECK(open4(3, 1) == 0);
    CHECK(open4(3, 2) == 0);
    CHECK(open4(3, 3) == 0);

    open4.override_clock_number(0, 0, 0);
    open4.override_clock_number(0, 1, 1);
    open4.override_clock_number(0, 2, 2);
    open4.override_clock_number(0, 3, 3);
    open4.override_clock_number(0, 4, 4);
    open4.override_clock_number(0, 5, 5);
    open4.override_clock_number(0, 6, 6);

    CHECK(!open4.is_regular());

    CHECK(open4(0, 0) == 0);
    CHECK(open4(0, 1) == 1);
    CHECK(open4(0, 2) == 2);
    CHECK(open4(0, 3) == 3);
    CHECK(open4(0, 4) == 0);
    CHECK(open4(0, 5) == 1);
    CHECK(open4(0, 6) == 2);
    CHECK(open4(1, 0) == 0);
    CHECK(open4(1, 1) == 0);
    CHECK(open4(1, 2) == 0);
    CHECK(open4(1, 3) == 0);
    CHECK(open4(2, 0) == 0);
    CHECK(open4(2, 1) == 0);
    CHECK(open4(2, 2) == 0);
    CHECK(open4(2, 3) == 0);
    CHECK(open4(3, 0) == 0);
    CHECK(open4(3, 1) == 0);
    CHECK(open4(3, 2) == 0);
    CHECK(open4(3, 3) == 0);
}

TEST_CASE("3-phase columnar clocking", "[clocking-scheme]")
{
    const auto columnar3 = clocking::columnar(clocking::num_clks::THREE);

    CHECK(columnar3.num_clocks() == 3u);
    CHECK(columnar3.max_in_degree() == 3u);
    CHECK(columnar3.max_out_degree() == 2u);
    CHECK(columnar3.is_regular());

    CHECK(columnar3(0, 0) == 0);
    CHECK(columnar3(0, 1) == 0);
    CHECK(columnar3(0, 2) == 0);
    CHECK(columnar3(1, 0) == 1);
    CHECK(columnar3(1, 1) == 1);
    CHECK(columnar3(1, 2) == 1);
    CHECK(columnar3(2, 0) == 2);
    CHECK(columnar3(2, 1) == 2);
    CHECK(columnar3(2, 2) == 2);

    CHECK(columnar3(0 + 3, 0) == 0);
    CHECK(columnar3(0 + 3, 1) == 0);
    CHECK(columnar3(0 + 3, 2) == 0);
    CHECK(columnar3(1 + 3, 0) == 1);
    CHECK(columnar3(1 + 3, 1) == 1);
    CHECK(columnar3(1 + 3, 2) == 1);
    CHECK(columnar3(2 + 3, 0) == 2);
    CHECK(columnar3(2 + 3, 1) == 2);
    CHECK(columnar3(2 + 3, 2) == 2);

    CHECK(columnar3(0, 0 + 3) == 0);
    CHECK(columnar3(0, 1 + 3) == 0);
    CHECK(columnar3(0, 2 + 3) == 0);
    CHECK(columnar3(1, 0 + 3) == 1);
    CHECK(columnar3(1, 1 + 3) == 1);
    CHECK(columnar3(1, 2 + 3) == 1);
    CHECK(columnar3(2, 0 + 3) == 2);
    CHECK(columnar3(2, 1 + 3) == 2);
    CHECK(columnar3(2, 2 + 3) == 2);

    CHECK(columnar3(0 + 3, 0 + 3) == 0);
    CHECK(columnar3(0 + 3, 1 + 3) == 0);
    CHECK(columnar3(0 + 3, 2 + 3) == 0);
    CHECK(columnar3(1 + 3, 0 + 3) == 1);
    CHECK(columnar3(1 + 3, 1 + 3) == 1);
    CHECK(columnar3(1 + 3, 2 + 3) == 1);
    CHECK(columnar3(2 + 3, 0 + 3) == 2);
    CHECK(columnar3(2 + 3, 1 + 3) == 2);
    CHECK(columnar3(2 + 3, 2 + 3) == 2);
}

TEST_CASE("4-phase columnar clocking", "[clocking-scheme]")
{
    const auto columnar4 = clocking::columnar(clocking::num_clks::FOUR);

    CHECK(columnar4.num_clocks() == 4u);
    CHECK(columnar4.max_in_degree() == 3u);
    CHECK(columnar4.max_out_degree() == 2u);
    CHECK(columnar4.is_regular());

    CHECK(columnar4(0, 0) == 0);
    CHECK(columnar4(0, 1) == 0);
    CHECK(columnar4(0, 2) == 0);
    CHECK(columnar4(0, 3) == 0);
    CHECK(columnar4(1, 0) == 1);
    CHECK(columnar4(1, 1) == 1);
    CHECK(columnar4(1, 2) == 1);
    CHECK(columnar4(1, 3) == 1);
    CHECK(columnar4(2, 0) == 2);
    CHECK(columnar4(2, 1) == 2);
    CHECK(columnar4(2, 2) == 2);
    CHECK(columnar4(2, 3) == 2);
    CHECK(columnar4(3, 0) == 3);
    CHECK(columnar4(3, 1) == 3);
    CHECK(columnar4(3, 2) == 3);
    CHECK(columnar4(3, 3) == 3);

    CHECK(columnar4(0 + 4, 0) == 0);
    CHECK(columnar4(0 + 4, 1) == 0);
    CHECK(columnar4(0 + 4, 2) == 0);
    CHECK(columnar4(0 + 4, 3) == 0);
    CHECK(columnar4(1 + 4, 0) == 1);
    CHECK(columnar4(1 + 4, 1) == 1);
    CHECK(columnar4(1 + 4, 2) == 1);
    CHECK(columnar4(1 + 4, 3) == 1);
    CHECK(columnar4(2 + 4, 0) == 2);
    CHECK(columnar4(2 + 4, 1) == 2);
    CHECK(columnar4(2 + 4, 2) == 2);
    CHECK(columnar4(2 + 4, 3) == 2);
    CHECK(columnar4(3 + 4, 0) == 3);
    CHECK(columnar4(3 + 4, 1) == 3);
    CHECK(columnar4(3 + 4, 2) == 3);
    CHECK(columnar4(3 + 4, 3) == 3);

    CHECK(columnar4(0, 0 + 4) == 0);
    CHECK(columnar4(0, 1 + 4) == 0);
    CHECK(columnar4(0, 2 + 4) == 0);
    CHECK(columnar4(0, 3 + 4) == 0);
    CHECK(columnar4(1, 0 + 4) == 1);
    CHECK(columnar4(1, 1 + 4) == 1);
    CHECK(columnar4(1, 2 + 4) == 1);
    CHECK(columnar4(1, 3 + 4) == 1);
    CHECK(columnar4(2, 0 + 4) == 2);
    CHECK(columnar4(2, 1 + 4) == 2);
    CHECK(columnar4(2, 2 + 4) == 2);
    CHECK(columnar4(2, 3 + 4) == 2);
    CHECK(columnar4(3, 0 + 4) == 3);
    CHECK(columnar4(3, 1 + 4) == 3);
    CHECK(columnar4(3, 2 + 4) == 3);
    CHECK(columnar4(3, 3 + 4) == 3);

    CHECK(columnar4(0 + 4, 0 + 4) == 0);
    CHECK(columnar4(0 + 4, 1 + 4) == 0);
    CHECK(columnar4(0 + 4, 2 + 4) == 0);
    CHECK(columnar4(0 + 4, 3 + 4) == 0);
    CHECK(columnar4(1 + 4, 0 + 4) == 1);
    CHECK(columnar4(1 + 4, 1 + 4) == 1);
    CHECK(columnar4(1 + 4, 2 + 4) == 1);
    CHECK(columnar4(1 + 4, 3 + 4) == 1);
    CHECK(columnar4(2 + 4, 0 + 4) == 2);
    CHECK(columnar4(2 + 4, 1 + 4) == 2);
    CHECK(columnar4(2 + 4, 2 + 4) == 2);
    CHECK(columnar4(2 + 4, 3 + 4) == 2);
    CHECK(columnar4(3 + 4, 0 + 4) == 3);
    CHECK(columnar4(3 + 4, 1 + 4) == 3);
    CHECK(columnar4(3 + 4, 2 + 4) == 3);
    CHECK(columnar4(3 + 4, 3 + 4) == 3);
}

TEST_CASE("3-phase row clocking", "[clocking-scheme]")
{
    const auto row3 = clocking::row(clocking::num_clks::THREE);

    CHECK(row3.num_clocks() == 3u);
    CHECK(row3.max_in_degree() == 3u);
    CHECK(row3.max_out_degree() == 2u);
    CHECK(row3.is_regular());

    CHECK(row3(0, 0) == 0);
    CHECK(row3(0, 1) == 1);
    CHECK(row3(0, 2) == 2);
    CHECK(row3(1, 0) == 0);
    CHECK(row3(1, 1) == 1);
    CHECK(row3(1, 2) == 2);
    CHECK(row3(2, 0) == 0);
    CHECK(row3(2, 1) == 1);
    CHECK(row3(2, 2) == 2);

    CHECK(row3(0 + 3, 0) == 0);
    CHECK(row3(0 + 3, 1) == 1);
    CHECK(row3(0 + 3, 2) == 2);
    CHECK(row3(1 + 3, 0) == 0);
    CHECK(row3(1 + 3, 1) == 1);
    CHECK(row3(1 + 3, 2) == 2);
    CHECK(row3(2 + 3, 0) == 0);
    CHECK(row3(2 + 3, 1) == 1);
    CHECK(row3(2 + 3, 2) == 2);

    CHECK(row3(0, 0 + 3) == 0);
    CHECK(row3(0, 1 + 3) == 1);
    CHECK(row3(0, 2 + 3) == 2);
    CHECK(row3(1, 0 + 3) == 0);
    CHECK(row3(1, 1 + 3) == 1);
    CHECK(row3(1, 2 + 3) == 2);
    CHECK(row3(2, 0 + 3) == 0);
    CHECK(row3(2, 1 + 3) == 1);
    CHECK(row3(2, 2 + 3) == 2);

    CHECK(row3(0 + 3, 0 + 3) == 0);
    CHECK(row3(0 + 3, 1 + 3) == 1);
    CHECK(row3(0 + 3, 2 + 3) == 2);
    CHECK(row3(1 + 3, 0 + 3) == 0);
    CHECK(row3(1 + 3, 1 + 3) == 1);
    CHECK(row3(1 + 3, 2 + 3) == 2);
    CHECK(row3(2 + 3, 0 + 3) == 0);
    CHECK(row3(2 + 3, 1 + 3) == 1);
    CHECK(row3(2 + 3, 2 + 3) == 2);
}

TEST_CASE("4-phase row clocking", "[clocking-scheme]")
{
    const auto row4 = clocking::row(clocking::num_clks::FOUR);

    CHECK(row4.num_clocks() == 4u);
    CHECK(row4.max_in_degree() == 3u);
    CHECK(row4.max_out_degree() == 2u);
    CHECK(row4.is_regular());

    CHECK(row4(0, 0) == 0);
    CHECK(row4(0, 1) == 1);
    CHECK(row4(0, 2) == 2);
    CHECK(row4(0, 3) == 3);
    CHECK(row4(1, 0) == 0);
    CHECK(row4(1, 1) == 1);
    CHECK(row4(1, 2) == 2);
    CHECK(row4(1, 3) == 3);
    CHECK(row4(2, 0) == 0);
    CHECK(row4(2, 1) == 1);
    CHECK(row4(2, 2) == 2);
    CHECK(row4(2, 3) == 3);
    CHECK(row4(3, 0) == 0);
    CHECK(row4(3, 1) == 1);
    CHECK(row4(3, 2) == 2);
    CHECK(row4(3, 3) == 3);

    CHECK(row4(0 + 4, 0) == 0);
    CHECK(row4(0 + 4, 1) == 1);
    CHECK(row4(0 + 4, 2) == 2);
    CHECK(row4(0 + 4, 3) == 3);
    CHECK(row4(1 + 4, 0) == 0);
    CHECK(row4(1 + 4, 1) == 1);
    CHECK(row4(1 + 4, 2) == 2);
    CHECK(row4(1 + 4, 3) == 3);
    CHECK(row4(2 + 4, 0) == 0);
    CHECK(row4(2 + 4, 1) == 1);
    CHECK(row4(2 + 4, 2) == 2);
    CHECK(row4(2 + 4, 3) == 3);
    CHECK(row4(3 + 4, 0) == 0);
    CHECK(row4(3 + 4, 1) == 1);
    CHECK(row4(3 + 4, 2) == 2);
    CHECK(row4(3 + 4, 3) == 3);

    CHECK(row4(0, 0 + 4) == 0);
    CHECK(row4(0, 1 + 4) == 1);
    CHECK(row4(0, 2 + 4) == 2);
    CHECK(row4(0, 3 + 4) == 3);
    CHECK(row4(1, 0 + 4) == 0);
    CHECK(row4(1, 1 + 4) == 1);
    CHECK(row4(1, 2 + 4) == 2);
    CHECK(row4(1, 3 + 4) == 3);
    CHECK(row4(2, 0 + 4) == 0);
    CHECK(row4(2, 1 + 4) == 1);
    CHECK(row4(2, 2 + 4) == 2);
    CHECK(row4(2, 3 + 4) == 3);
    CHECK(row4(3, 0 + 4) == 0);
    CHECK(row4(3, 1 + 4) == 1);
    CHECK(row4(3, 2 + 4) == 2);
    CHECK(row4(3, 3 + 4) == 3);

    CHECK(row4(0 + 4, 0 + 4) == 0);
    CHECK(row4(0 + 4, 1 + 4) == 1);
    CHECK(row4(0 + 4, 2 + 4) == 2);
    CHECK(row4(0 + 4, 3 + 4) == 3);
    CHECK(row4(1 + 4, 0 + 4) == 0);
    CHECK(row4(1 + 4, 1 + 4) == 1);
    CHECK(row4(1 + 4, 2 + 4) == 2);
    CHECK(row4(1 + 4, 3 + 4) == 3);
    CHECK(row4(2 + 4, 0 + 4) == 0);
    CHECK(row4(2 + 4, 1 + 4) == 1);
    CHECK(row4(2 + 4, 2 + 4) == 2);
    CHECK(row4(2 + 4, 3 + 4) == 3);
    CHECK(row4(3 + 4, 0 + 4) == 0);
    CHECK(row4(3 + 4, 1 + 4) == 1);
    CHECK(row4(3 + 4, 2 + 4) == 2);
    CHECK(row4(3 + 4, 3 + 4) == 3);
}

TEST_CASE("3-phase 2DDWave", "[clocking-scheme]")
{
    const auto twoddwave3 = clocking::twoddwave(clocking::num_clks::THREE);

    CHECK(twoddwave3.num_clocks() == 3u);
    CHECK(twoddwave3.max_in_degree() == 2u);
    CHECK(twoddwave3.max_out_degree() == 2u);
    CHECK(twoddwave3.is_regular());

    CHECK(twoddwave3(0, 0) == 0);
    CHECK(twoddwave3(0, 1) == 1);
    CHECK(twoddwave3(0, 2) == 2);
    CHECK(twoddwave3(1, 0) == 1);
    CHECK(twoddwave3(1, 1) == 2);
    CHECK(twoddwave3(1, 2) == 0);
    CHECK(twoddwave3(2, 0) == 2);
    CHECK(twoddwave3(2, 1) == 0);
    CHECK(twoddwave3(2, 2) == 1);

    CHECK(twoddwave3(0 + 3, 0) == 0);
    CHECK(twoddwave3(0 + 3, 1) == 1);
    CHECK(twoddwave3(0 + 3, 2) == 2);
    CHECK(twoddwave3(1 + 3, 0) == 1);
    CHECK(twoddwave3(1 + 3, 1) == 2);
    CHECK(twoddwave3(1 + 3, 2) == 0);
    CHECK(twoddwave3(2 + 3, 0) == 2);
    CHECK(twoddwave3(2 + 3, 1) == 0);
    CHECK(twoddwave3(2 + 3, 2) == 1);

    CHECK(twoddwave3(0, 0 + 3) == 0);
    CHECK(twoddwave3(0, 1 + 3) == 1);
    CHECK(twoddwave3(0, 2 + 3) == 2);
    CHECK(twoddwave3(1, 0 + 3) == 1);
    CHECK(twoddwave3(1, 1 + 3) == 2);
    CHECK(twoddwave3(1, 2 + 3) == 0);
    CHECK(twoddwave3(2, 0 + 3) == 2);
    CHECK(twoddwave3(2, 1 + 3) == 0);
    CHECK(twoddwave3(2, 2 + 3) == 1);

    CHECK(twoddwave3(0 + 3, 0 + 3) == 0);
    CHECK(twoddwave3(0 + 3, 1 + 3) == 1);
    CHECK(twoddwave3(0 + 3, 2 + 3) == 2);
    CHECK(twoddwave3(1 + 3, 0 + 3) == 1);
    CHECK(twoddwave3(1 + 3, 1 + 3) == 2);
    CHECK(twoddwave3(1 + 3, 2 + 3) == 0);
    CHECK(twoddwave3(2 + 3, 0 + 3) == 2);
    CHECK(twoddwave3(2 + 3, 1 + 3) == 0);
    CHECK(twoddwave3(2 + 3, 2 + 3) == 1);
}

TEST_CASE("4-phase 2DDWave", "[clocking-scheme]")
{
    const auto twoddwave4 = clocking::twoddwave(clocking::num_clks::FOUR);

    CHECK(twoddwave4.num_clocks() == 4u);
    CHECK(twoddwave4.max_in_degree() == 2u);
    CHECK(twoddwave4.max_out_degree() == 2u);
    CHECK(twoddwave4.is_regular());

    CHECK(twoddwave4(0, 0) == 0);
    CHECK(twoddwave4(0, 1) == 1);
    CHECK(twoddwave4(0, 2) == 2);
    CHECK(twoddwave4(0, 3) == 3);
    CHECK(twoddwave4(1, 0) == 1);
    CHECK(twoddwave4(1, 1) == 2);
    CHECK(twoddwave4(1, 2) == 3);
    CHECK(twoddwave4(1, 3) == 0);
    CHECK(twoddwave4(2, 0) == 2);
    CHECK(twoddwave4(2, 1) == 3);
    CHECK(twoddwave4(2, 2) == 0);
    CHECK(twoddwave4(2, 3) == 1);
    CHECK(twoddwave4(3, 0) == 3);
    CHECK(twoddwave4(3, 1) == 0);
    CHECK(twoddwave4(3, 2) == 1);
    CHECK(twoddwave4(3, 3) == 2);

    CHECK(twoddwave4(0 + 4, 0) == 0);
    CHECK(twoddwave4(0 + 4, 1) == 1);
    CHECK(twoddwave4(0 + 4, 2) == 2);
    CHECK(twoddwave4(0 + 4, 3) == 3);
    CHECK(twoddwave4(1 + 4, 0) == 1);
    CHECK(twoddwave4(1 + 4, 1) == 2);
    CHECK(twoddwave4(1 + 4, 2) == 3);
    CHECK(twoddwave4(1 + 4, 3) == 0);
    CHECK(twoddwave4(2 + 4, 0) == 2);
    CHECK(twoddwave4(2 + 4, 1) == 3);
    CHECK(twoddwave4(2 + 4, 2) == 0);
    CHECK(twoddwave4(2 + 4, 3) == 1);
    CHECK(twoddwave4(3 + 4, 0) == 3);
    CHECK(twoddwave4(3 + 4, 1) == 0);
    CHECK(twoddwave4(3 + 4, 2) == 1);
    CHECK(twoddwave4(3 + 4, 3) == 2);

    CHECK(twoddwave4(0, 0 + 4) == 0);
    CHECK(twoddwave4(0, 1 + 4) == 1);
    CHECK(twoddwave4(0, 2 + 4) == 2);
    CHECK(twoddwave4(0, 3 + 4) == 3);
    CHECK(twoddwave4(1, 0 + 4) == 1);
    CHECK(twoddwave4(1, 1 + 4) == 2);
    CHECK(twoddwave4(1, 2 + 4) == 3);
    CHECK(twoddwave4(1, 3 + 4) == 0);
    CHECK(twoddwave4(2, 0 + 4) == 2);
    CHECK(twoddwave4(2, 1 + 4) == 3);
    CHECK(twoddwave4(2, 2 + 4) == 0);
    CHECK(twoddwave4(2, 3 + 4) == 1);
    CHECK(twoddwave4(3, 0 + 4) == 3);
    CHECK(twoddwave4(3, 1 + 4) == 0);
    CHECK(twoddwave4(3, 2 + 4) == 1);
    CHECK(twoddwave4(3, 3 + 4) == 2);

    CHECK(twoddwave4(0 + 4, 0 + 4) == 0);
    CHECK(twoddwave4(0 + 4, 1 + 4) == 1);
    CHECK(twoddwave4(0 + 4, 2 + 4) == 2);
    CHECK(twoddwave4(0 + 4, 3 + 4) == 3);
    CHECK(twoddwave4(1 + 4, 0 + 4) == 1);
    CHECK(twoddwave4(1 + 4, 1 + 4) == 2);
    CHECK(twoddwave4(1 + 4, 2 + 4) == 3);
    CHECK(twoddwave4(1 + 4, 3 + 4) == 0);
    CHECK(twoddwave4(2 + 4, 0 + 4) == 2);
    CHECK(twoddwave4(2 + 4, 1 + 4) == 3);
    CHECK(twoddwave4(2 + 4, 2 + 4) == 0);
    CHECK(twoddwave4(2 + 4, 3 + 4) == 1);
    CHECK(twoddwave4(3 + 4, 0 + 4) == 3);
    CHECK(twoddwave4(3 + 4, 1 + 4) == 0);
    CHECK(twoddwave4(3 + 4, 2 + 4) == 1);
    CHECK(twoddwave4(3 + 4, 3 + 4) == 2);
}

TEST_CASE("3-phase 2DDWaveHex: odd row", "[clocking-scheme]")
{
    const auto twoddwave_hex_3 = clocking::twoddwave_hex(clocking::hex_arrangement::ODD_ROW, clocking::num_clks::THREE);

    CHECK(twoddwave_hex_3.num_clocks() == 3u);
    CHECK(twoddwave_hex_3.max_in_degree() == 2u);
    CHECK(twoddwave_hex_3.max_out_degree() == 2u);
    CHECK(twoddwave_hex_3.is_regular());

    CHECK(twoddwave_hex_3(0, 0) == 0);
    CHECK(twoddwave_hex_3(0, 1) == 1);
    CHECK(twoddwave_hex_3(0, 2) == 1);
    CHECK(twoddwave_hex_3(0, 3) == 2);
    CHECK(twoddwave_hex_3(0, 4) == 2);
    CHECK(twoddwave_hex_3(0, 5) == 0);
    CHECK(twoddwave_hex_3(1, 0) == 1);
    CHECK(twoddwave_hex_3(1, 1) == 2);
    CHECK(twoddwave_hex_3(1, 2) == 2);
    CHECK(twoddwave_hex_3(1, 3) == 0);
    CHECK(twoddwave_hex_3(1, 4) == 0);
    CHECK(twoddwave_hex_3(1, 5) == 1);
    CHECK(twoddwave_hex_3(2, 0) == 2);
    CHECK(twoddwave_hex_3(2, 1) == 0);
    CHECK(twoddwave_hex_3(2, 2) == 0);
    CHECK(twoddwave_hex_3(2, 3) == 1);
    CHECK(twoddwave_hex_3(2, 4) == 1);
    CHECK(twoddwave_hex_3(2, 5) == 2);

    CHECK(twoddwave_hex_3(0 + 3, 0) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 1) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 2) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 3) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 4) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 5) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 0) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 1) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 2) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 3) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 4) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 5) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 0) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 1) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 2) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 3) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 4) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 5) == 2);

    CHECK(twoddwave_hex_3(0, 0 + 6) == 0);
    CHECK(twoddwave_hex_3(0, 1 + 6) == 1);
    CHECK(twoddwave_hex_3(0, 2 + 6) == 1);
    CHECK(twoddwave_hex_3(0, 3 + 6) == 2);
    CHECK(twoddwave_hex_3(0, 4 + 6) == 2);
    CHECK(twoddwave_hex_3(0, 5 + 6) == 0);
    CHECK(twoddwave_hex_3(1, 0 + 6) == 1);
    CHECK(twoddwave_hex_3(1, 1 + 6) == 2);
    CHECK(twoddwave_hex_3(1, 2 + 6) == 2);
    CHECK(twoddwave_hex_3(1, 3 + 6) == 0);
    CHECK(twoddwave_hex_3(1, 4 + 6) == 0);
    CHECK(twoddwave_hex_3(1, 5 + 6) == 1);
    CHECK(twoddwave_hex_3(2, 0 + 6) == 2);
    CHECK(twoddwave_hex_3(2, 1 + 6) == 0);
    CHECK(twoddwave_hex_3(2, 2 + 6) == 0);
    CHECK(twoddwave_hex_3(2, 3 + 6) == 1);
    CHECK(twoddwave_hex_3(2, 4 + 6) == 1);
    CHECK(twoddwave_hex_3(2, 5 + 6) == 2);

    CHECK(twoddwave_hex_3(0 + 3, 0 + 6) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 1 + 6) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 2 + 6) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 3 + 6) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 4 + 6) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 5 + 6) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 0 + 6) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 1 + 6) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 2 + 6) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 3 + 6) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 4 + 6) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 5 + 6) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 0 + 6) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 1 + 6) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 2 + 6) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 3 + 6) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 4 + 6) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 5 + 6) == 2);
}

TEST_CASE("3-phase 2DDWaveHex: even row", "[clocking-scheme]")
{
    const auto twoddwave_hex_3 =
        clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_ROW, clocking::num_clks::THREE);

    CHECK(twoddwave_hex_3.num_clocks() == 3u);
    CHECK(twoddwave_hex_3.max_in_degree() == 2u);
    CHECK(twoddwave_hex_3.max_out_degree() == 2u);
    CHECK(twoddwave_hex_3.is_regular());

    CHECK(twoddwave_hex_3(0, 0) == 0);
    CHECK(twoddwave_hex_3(0, 1) == 0);
    CHECK(twoddwave_hex_3(0, 2) == 1);
    CHECK(twoddwave_hex_3(0, 3) == 1);
    CHECK(twoddwave_hex_3(0, 4) == 2);
    CHECK(twoddwave_hex_3(0, 5) == 2);
    CHECK(twoddwave_hex_3(1, 0) == 1);
    CHECK(twoddwave_hex_3(1, 1) == 1);
    CHECK(twoddwave_hex_3(1, 2) == 2);
    CHECK(twoddwave_hex_3(1, 3) == 2);
    CHECK(twoddwave_hex_3(1, 4) == 0);
    CHECK(twoddwave_hex_3(1, 5) == 0);
    CHECK(twoddwave_hex_3(2, 0) == 2);
    CHECK(twoddwave_hex_3(2, 1) == 2);
    CHECK(twoddwave_hex_3(2, 2) == 0);
    CHECK(twoddwave_hex_3(2, 3) == 0);
    CHECK(twoddwave_hex_3(2, 4) == 1);
    CHECK(twoddwave_hex_3(2, 5) == 1);

    CHECK(twoddwave_hex_3(0 + 3, 0) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 1) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 2) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 3) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 4) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 5) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 0) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 1) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 2) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 3) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 4) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 5) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 0) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 1) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 2) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 3) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 4) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 5) == 1);

    CHECK(twoddwave_hex_3(0, 0 + 6) == 0);
    CHECK(twoddwave_hex_3(0, 1 + 6) == 0);
    CHECK(twoddwave_hex_3(0, 2 + 6) == 1);
    CHECK(twoddwave_hex_3(0, 3 + 6) == 1);
    CHECK(twoddwave_hex_3(0, 4 + 6) == 2);
    CHECK(twoddwave_hex_3(0, 5 + 6) == 2);
    CHECK(twoddwave_hex_3(1, 0 + 6) == 1);
    CHECK(twoddwave_hex_3(1, 1 + 6) == 1);
    CHECK(twoddwave_hex_3(1, 2 + 6) == 2);
    CHECK(twoddwave_hex_3(1, 3 + 6) == 2);
    CHECK(twoddwave_hex_3(1, 4 + 6) == 0);
    CHECK(twoddwave_hex_3(1, 5 + 6) == 0);
    CHECK(twoddwave_hex_3(2, 0 + 6) == 2);
    CHECK(twoddwave_hex_3(2, 1 + 6) == 2);
    CHECK(twoddwave_hex_3(2, 2 + 6) == 0);
    CHECK(twoddwave_hex_3(2, 3 + 6) == 0);
    CHECK(twoddwave_hex_3(2, 4 + 6) == 1);
    CHECK(twoddwave_hex_3(2, 5 + 6) == 1);

    CHECK(twoddwave_hex_3(0 + 3, 0 + 6) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 1 + 6) == 0);
    CHECK(twoddwave_hex_3(0 + 3, 2 + 6) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 3 + 6) == 1);
    CHECK(twoddwave_hex_3(0 + 3, 4 + 6) == 2);
    CHECK(twoddwave_hex_3(0 + 3, 5 + 6) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 0 + 6) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 1 + 6) == 1);
    CHECK(twoddwave_hex_3(1 + 3, 2 + 6) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 3 + 6) == 2);
    CHECK(twoddwave_hex_3(1 + 3, 4 + 6) == 0);
    CHECK(twoddwave_hex_3(1 + 3, 5 + 6) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 0 + 6) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 1 + 6) == 2);
    CHECK(twoddwave_hex_3(2 + 3, 2 + 6) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 3 + 6) == 0);
    CHECK(twoddwave_hex_3(2 + 3, 4 + 6) == 1);
    CHECK(twoddwave_hex_3(2 + 3, 5 + 6) == 1);
}

TEST_CASE("3-phase 2DDWaveHex: odd column", "[clocking-scheme]")
{
    const auto twoddwave_hex_3 =
        clocking::twoddwave_hex(clocking::hex_arrangement::ODD_COLUMN, clocking::num_clks::THREE);

    CHECK(twoddwave_hex_3.num_clocks() == 3u);
    CHECK(twoddwave_hex_3.max_in_degree() == 2u);
    CHECK(twoddwave_hex_3.max_out_degree() == 2u);
    CHECK(twoddwave_hex_3.is_regular());

    CHECK(twoddwave_hex_3(0, 0) == 0);
    CHECK(twoddwave_hex_3(0, 1) == 1);
    CHECK(twoddwave_hex_3(0, 2) == 2);
    CHECK(twoddwave_hex_3(1, 0) == 1);
    CHECK(twoddwave_hex_3(1, 1) == 2);
    CHECK(twoddwave_hex_3(1, 2) == 0);
    CHECK(twoddwave_hex_3(2, 0) == 1);
    CHECK(twoddwave_hex_3(2, 1) == 2);
    CHECK(twoddwave_hex_3(2, 2) == 0);
    CHECK(twoddwave_hex_3(3, 0) == 2);
    CHECK(twoddwave_hex_3(3, 1) == 0);
    CHECK(twoddwave_hex_3(3, 2) == 1);
    CHECK(twoddwave_hex_3(4, 0) == 2);
    CHECK(twoddwave_hex_3(4, 1) == 0);
    CHECK(twoddwave_hex_3(4, 2) == 1);
    CHECK(twoddwave_hex_3(5, 0) == 0);
    CHECK(twoddwave_hex_3(5, 1) == 1);
    CHECK(twoddwave_hex_3(5, 2) == 2);

    CHECK(twoddwave_hex_3(0 + 6, 0) == 0);
    CHECK(twoddwave_hex_3(0 + 6, 1) == 1);
    CHECK(twoddwave_hex_3(0 + 6, 2) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 0) == 1);
    CHECK(twoddwave_hex_3(1 + 6, 1) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 2) == 0);
    CHECK(twoddwave_hex_3(2 + 6, 0) == 1);
    CHECK(twoddwave_hex_3(2 + 6, 1) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 2) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 0) == 2);
    CHECK(twoddwave_hex_3(3 + 6, 1) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 2) == 1);
    CHECK(twoddwave_hex_3(4 + 6, 0) == 2);
    CHECK(twoddwave_hex_3(4 + 6, 1) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 2) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 0) == 0);
    CHECK(twoddwave_hex_3(5 + 6, 1) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 2) == 2);

    CHECK(twoddwave_hex_3(0, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(0, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(0, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(1, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(1, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(1, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(2, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(2, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(2, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(3, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(3, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(3, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(4, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(4, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(4, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(5, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(5, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(5, 2 + 3) == 2);

    CHECK(twoddwave_hex_3(0 + 6, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(0 + 6, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(0 + 6, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(1 + 6, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(2 + 6, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(2 + 6, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(3 + 6, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(4 + 6, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(4 + 6, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(5 + 6, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 2 + 3) == 2);
}

TEST_CASE("3-phase 2DDWaveHex: even column", "[clocking-scheme]")
{
    const auto twoddwave_hex_3 =
        clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_COLUMN, clocking::num_clks::THREE);

    CHECK(twoddwave_hex_3.num_clocks() == 3u);
    CHECK(twoddwave_hex_3.max_in_degree() == 2u);
    CHECK(twoddwave_hex_3.max_out_degree() == 2u);
    CHECK(twoddwave_hex_3.is_regular());

    CHECK(twoddwave_hex_3(0, 0) == 0);
    CHECK(twoddwave_hex_3(0, 1) == 1);
    CHECK(twoddwave_hex_3(0, 2) == 2);
    CHECK(twoddwave_hex_3(1, 0) == 0);
    CHECK(twoddwave_hex_3(1, 1) == 1);
    CHECK(twoddwave_hex_3(1, 2) == 2);
    CHECK(twoddwave_hex_3(2, 0) == 1);
    CHECK(twoddwave_hex_3(2, 1) == 2);
    CHECK(twoddwave_hex_3(2, 2) == 0);
    CHECK(twoddwave_hex_3(3, 0) == 1);
    CHECK(twoddwave_hex_3(3, 1) == 2);
    CHECK(twoddwave_hex_3(3, 2) == 0);
    CHECK(twoddwave_hex_3(4, 0) == 2);
    CHECK(twoddwave_hex_3(4, 1) == 0);
    CHECK(twoddwave_hex_3(4, 2) == 1);
    CHECK(twoddwave_hex_3(5, 0) == 2);
    CHECK(twoddwave_hex_3(5, 1) == 0);
    CHECK(twoddwave_hex_3(5, 2) == 1);

    CHECK(twoddwave_hex_3(0 + 6, 0) == 0);
    CHECK(twoddwave_hex_3(0 + 6, 1) == 1);
    CHECK(twoddwave_hex_3(0 + 6, 2) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 0) == 0);
    CHECK(twoddwave_hex_3(1 + 6, 1) == 1);
    CHECK(twoddwave_hex_3(1 + 6, 2) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 0) == 1);
    CHECK(twoddwave_hex_3(2 + 6, 1) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 2) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 0) == 1);
    CHECK(twoddwave_hex_3(3 + 6, 1) == 2);
    CHECK(twoddwave_hex_3(3 + 6, 2) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 0) == 2);
    CHECK(twoddwave_hex_3(4 + 6, 1) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 2) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 0) == 2);
    CHECK(twoddwave_hex_3(5 + 6, 1) == 0);
    CHECK(twoddwave_hex_3(5 + 6, 2) == 1);

    CHECK(twoddwave_hex_3(0, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(0, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(0, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(1, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(1, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(1, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(2, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(2, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(2, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(3, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(3, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(3, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(4, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(4, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(4, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(5, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(5, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(5, 2 + 3) == 1);

    CHECK(twoddwave_hex_3(0 + 6, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(0 + 6, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(0 + 6, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(1 + 6, 0 + 3) == 0);
    CHECK(twoddwave_hex_3(1 + 6, 1 + 3) == 1);
    CHECK(twoddwave_hex_3(1 + 6, 2 + 3) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(2 + 6, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(2 + 6, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(3 + 6, 0 + 3) == 1);
    CHECK(twoddwave_hex_3(3 + 6, 1 + 3) == 2);
    CHECK(twoddwave_hex_3(3 + 6, 2 + 3) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(4 + 6, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(4 + 6, 2 + 3) == 1);
    CHECK(twoddwave_hex_3(5 + 6, 0 + 3) == 2);
    CHECK(twoddwave_hex_3(5 + 6, 1 + 3) == 0);
    CHECK(twoddwave_hex_3(5 + 6, 2 + 3) == 1);
}

TEST_CASE("4-phase 2DDWaveHex: odd row", "[clocking-scheme]")
{
    const auto twoddwave_hex_4 = clocking::twoddwave_hex(clocking::hex_arrangement::ODD_ROW, clocking::num_clks::FOUR);

    CHECK(twoddwave_hex_4.num_clocks() == 4u);
    CHECK(twoddwave_hex_4.max_in_degree() == 2u);
    CHECK(twoddwave_hex_4.max_out_degree() == 2u);
    CHECK(twoddwave_hex_4.is_regular());

    /** @brief Clock numbers for one complete period of this topology. */
    static constexpr std::array<std::array<uint8_t, 8>, 4> expected{{{{0, 1, 1, 2, 2, 3, 3, 0}},
                                                                     {{1, 2, 2, 3, 3, 0, 0, 1}},
                                                                     {{2, 3, 3, 0, 0, 1, 1, 2}},
                                                                     {{3, 0, 0, 1, 1, 2, 2, 3}}}};
    for (uint32_t x = 0; x < 4; ++x)
    {
        for (uint32_t y = 0; y < 8; ++y)
        {
            CAPTURE(x, y);
            CHECK(twoddwave_hex_4(x, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 4, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x, y + 8) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 4, y + 8) == expected.at(x).at(y));
        }
    }
}

TEST_CASE("4-phase 2DDWaveHex: even row", "[clocking-scheme]")
{
    const auto twoddwave_hex_4 = clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_ROW, clocking::num_clks::FOUR);

    CHECK(twoddwave_hex_4.num_clocks() == 4u);
    CHECK(twoddwave_hex_4.max_in_degree() == 2u);
    CHECK(twoddwave_hex_4.max_out_degree() == 2u);
    CHECK(twoddwave_hex_4.is_regular());

    /** @brief Clock numbers for one complete period of this topology. */
    static constexpr std::array<std::array<uint8_t, 8>, 4> expected{{{{0, 0, 1, 1, 2, 2, 3, 3}},
                                                                     {{1, 1, 2, 2, 3, 3, 0, 0}},
                                                                     {{2, 2, 3, 3, 0, 0, 1, 1}},
                                                                     {{3, 3, 0, 0, 1, 1, 2, 2}}}};
    for (uint32_t x = 0; x < 4; ++x)
    {
        for (uint32_t y = 0; y < 8; ++y)
        {
            CAPTURE(x, y);
            CHECK(twoddwave_hex_4(x, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 4, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x, y + 8) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 4, y + 8) == expected.at(x).at(y));
        }
    }
}

TEST_CASE("4-phase 2DDWaveHex: odd column", "[clocking-scheme]")
{
    const auto twoddwave_hex_4 =
        clocking::twoddwave_hex(clocking::hex_arrangement::ODD_COLUMN, clocking::num_clks::FOUR);

    CHECK(twoddwave_hex_4.num_clocks() == 4u);
    CHECK(twoddwave_hex_4.max_in_degree() == 2u);
    CHECK(twoddwave_hex_4.max_out_degree() == 2u);
    CHECK(twoddwave_hex_4.is_regular());

    /** @brief Clock numbers for one complete period of this topology. */
    static constexpr std::array<std::array<uint8_t, 4>, 8> expected{{{{0, 1, 2, 3}},
                                                                     {{1, 2, 3, 0}},
                                                                     {{1, 2, 3, 0}},
                                                                     {{2, 3, 0, 1}},
                                                                     {{2, 3, 0, 1}},
                                                                     {{3, 0, 1, 2}},
                                                                     {{3, 0, 1, 2}},
                                                                     {{0, 1, 2, 3}}}};
    for (uint32_t x = 0; x < 8; ++x)
    {
        for (uint32_t y = 0; y < 4; ++y)
        {
            CAPTURE(x, y);
            CHECK(twoddwave_hex_4(x, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 8, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x, y + 4) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 8, y + 4) == expected.at(x).at(y));
        }
    }
}

TEST_CASE("4-phase 2DDWaveHex: even column", "[clocking-scheme]")
{
    const auto twoddwave_hex_4 =
        clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_COLUMN, clocking::num_clks::FOUR);

    CHECK(twoddwave_hex_4.num_clocks() == 4u);
    CHECK(twoddwave_hex_4.max_in_degree() == 2u);
    CHECK(twoddwave_hex_4.max_out_degree() == 2u);
    CHECK(twoddwave_hex_4.is_regular());

    /** @brief Clock numbers for one complete period of this topology. */
    static constexpr std::array<std::array<uint8_t, 4>, 8> expected{{{{0, 1, 2, 3}},
                                                                     {{0, 1, 2, 3}},
                                                                     {{1, 2, 3, 0}},
                                                                     {{1, 2, 3, 0}},
                                                                     {{2, 3, 0, 1}},
                                                                     {{2, 3, 0, 1}},
                                                                     {{3, 0, 1, 2}},
                                                                     {{3, 0, 1, 2}}}};
    for (uint32_t x = 0; x < 8; ++x)
    {
        for (uint32_t y = 0; y < 4; ++y)
        {
            CAPTURE(x, y);
            CHECK(twoddwave_hex_4(x, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 8, y) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x, y + 4) == expected.at(x).at(y));
            CHECK(twoddwave_hex_4(x + 8, y + 4) == expected.at(x).at(y));
        }
    }
}

TEST_CASE("4-phase USE", "[clocking-scheme]")
{
    const auto use4 = clocking::use();

    CHECK(use4.num_clocks() == 4u);
    CHECK(use4.max_in_degree() == 2u);
    CHECK(use4.max_out_degree() == 2u);
    CHECK(use4.is_regular());

    CHECK(use4(0, 0) == 0);
    CHECK(use4(0, 1) == 3);
    CHECK(use4(0, 2) == 2);
    CHECK(use4(0, 3) == 1);
    CHECK(use4(1, 0) == 1);
    CHECK(use4(1, 1) == 2);
    CHECK(use4(1, 2) == 3);
    CHECK(use4(1, 3) == 0);
    CHECK(use4(2, 0) == 2);
    CHECK(use4(2, 1) == 1);
    CHECK(use4(2, 2) == 0);
    CHECK(use4(2, 3) == 3);
    CHECK(use4(3, 0) == 3);
    CHECK(use4(3, 1) == 0);
    CHECK(use4(3, 2) == 1);
    CHECK(use4(3, 3) == 2);

    CHECK(use4(0 + 4, 0) == 0);
    CHECK(use4(0 + 4, 1) == 3);
    CHECK(use4(0 + 4, 2) == 2);
    CHECK(use4(0 + 4, 3) == 1);
    CHECK(use4(1 + 4, 0) == 1);
    CHECK(use4(1 + 4, 1) == 2);
    CHECK(use4(1 + 4, 2) == 3);
    CHECK(use4(1 + 4, 3) == 0);
    CHECK(use4(2 + 4, 0) == 2);
    CHECK(use4(2 + 4, 1) == 1);
    CHECK(use4(2 + 4, 2) == 0);
    CHECK(use4(2 + 4, 3) == 3);
    CHECK(use4(3 + 4, 0) == 3);
    CHECK(use4(3 + 4, 1) == 0);
    CHECK(use4(3 + 4, 2) == 1);
    CHECK(use4(3 + 4, 3) == 2);

    CHECK(use4(0, 0 + 4) == 0);
    CHECK(use4(0, 1 + 4) == 3);
    CHECK(use4(0, 2 + 4) == 2);
    CHECK(use4(0, 3 + 4) == 1);
    CHECK(use4(1, 0 + 4) == 1);
    CHECK(use4(1, 1 + 4) == 2);
    CHECK(use4(1, 2 + 4) == 3);
    CHECK(use4(1, 3 + 4) == 0);
    CHECK(use4(2, 0 + 4) == 2);
    CHECK(use4(2, 1 + 4) == 1);
    CHECK(use4(2, 2 + 4) == 0);
    CHECK(use4(2, 3 + 4) == 3);
    CHECK(use4(3, 0 + 4) == 3);
    CHECK(use4(3, 1 + 4) == 0);
    CHECK(use4(3, 2 + 4) == 1);
    CHECK(use4(3, 3 + 4) == 2);

    CHECK(use4(0 + 4, 0 + 4) == 0);
    CHECK(use4(0 + 4, 1 + 4) == 3);
    CHECK(use4(0 + 4, 2 + 4) == 2);
    CHECK(use4(0 + 4, 3 + 4) == 1);
    CHECK(use4(1 + 4, 0 + 4) == 1);
    CHECK(use4(1 + 4, 1 + 4) == 2);
    CHECK(use4(1 + 4, 2 + 4) == 3);
    CHECK(use4(1 + 4, 3 + 4) == 0);
    CHECK(use4(2 + 4, 0 + 4) == 2);
    CHECK(use4(2 + 4, 1 + 4) == 1);
    CHECK(use4(2 + 4, 2 + 4) == 0);
    CHECK(use4(2 + 4, 3 + 4) == 3);
    CHECK(use4(3 + 4, 0 + 4) == 3);
    CHECK(use4(3 + 4, 1 + 4) == 0);
    CHECK(use4(3 + 4, 2 + 4) == 1);
    CHECK(use4(3 + 4, 3 + 4) == 2);
}

TEST_CASE("4-phase RES", "[clocking-scheme]")
{
    const auto res4 = clocking::res();

    CHECK(res4.num_clocks() == 4u);
    CHECK(res4.max_in_degree() == 3u);
    CHECK(res4.max_out_degree() == 3u);
    CHECK(res4.is_regular());

    CHECK(res4(0, 0) == 3);
    CHECK(res4(0, 1) == 0);
    CHECK(res4(0, 2) == 1);
    CHECK(res4(0, 3) == 0);
    CHECK(res4(1, 0) == 0);
    CHECK(res4(1, 1) == 1);
    CHECK(res4(1, 2) == 2);
    CHECK(res4(1, 3) == 3);
    CHECK(res4(2, 0) == 1);
    CHECK(res4(2, 1) == 0);
    CHECK(res4(2, 2) == 3);
    CHECK(res4(2, 3) == 2);
    CHECK(res4(3, 0) == 2);
    CHECK(res4(3, 1) == 3);
    CHECK(res4(3, 2) == 0);
    CHECK(res4(3, 3) == 1);

    CHECK(res4(0 + 4, 0) == 3);
    CHECK(res4(0 + 4, 1) == 0);
    CHECK(res4(0 + 4, 2) == 1);
    CHECK(res4(0 + 4, 3) == 0);
    CHECK(res4(1 + 4, 0) == 0);
    CHECK(res4(1 + 4, 1) == 1);
    CHECK(res4(1 + 4, 2) == 2);
    CHECK(res4(1 + 4, 3) == 3);
    CHECK(res4(2 + 4, 0) == 1);
    CHECK(res4(2 + 4, 1) == 0);
    CHECK(res4(2 + 4, 2) == 3);
    CHECK(res4(2 + 4, 3) == 2);
    CHECK(res4(3 + 4, 0) == 2);
    CHECK(res4(3 + 4, 1) == 3);
    CHECK(res4(3 + 4, 2) == 0);
    CHECK(res4(3 + 4, 3) == 1);

    CHECK(res4(0, 0 + 4) == 3);
    CHECK(res4(0, 1 + 4) == 0);
    CHECK(res4(0, 2 + 4) == 1);
    CHECK(res4(0, 3 + 4) == 0);
    CHECK(res4(1, 0 + 4) == 0);
    CHECK(res4(1, 1 + 4) == 1);
    CHECK(res4(1, 2 + 4) == 2);
    CHECK(res4(1, 3 + 4) == 3);
    CHECK(res4(2, 0 + 4) == 1);
    CHECK(res4(2, 1 + 4) == 0);
    CHECK(res4(2, 2 + 4) == 3);
    CHECK(res4(2, 3 + 4) == 2);
    CHECK(res4(3, 0 + 4) == 2);
    CHECK(res4(3, 1 + 4) == 3);
    CHECK(res4(3, 2 + 4) == 0);
    CHECK(res4(3, 3 + 4) == 1);

    CHECK(res4(0 + 4, 0 + 4) == 3);
    CHECK(res4(0 + 4, 1 + 4) == 0);
    CHECK(res4(0 + 4, 2 + 4) == 1);
    CHECK(res4(0 + 4, 3 + 4) == 0);
    CHECK(res4(1 + 4, 0 + 4) == 0);
    CHECK(res4(1 + 4, 1 + 4) == 1);
    CHECK(res4(1 + 4, 2 + 4) == 2);
    CHECK(res4(1 + 4, 3 + 4) == 3);
    CHECK(res4(2 + 4, 0 + 4) == 1);
    CHECK(res4(2 + 4, 1 + 4) == 0);
    CHECK(res4(2 + 4, 2 + 4) == 3);
    CHECK(res4(2 + 4, 3 + 4) == 2);
    CHECK(res4(3 + 4, 0 + 4) == 2);
    CHECK(res4(3 + 4, 1 + 4) == 3);
    CHECK(res4(3 + 4, 2 + 4) == 0);
    CHECK(res4(3 + 4, 3 + 4) == 1);
}

TEST_CASE("4-phase CFE", "[clocking-scheme]")
{
    const auto cfe4 = clocking::cfe();

    CHECK(cfe4.num_clocks() == 4u);
    CHECK(cfe4.max_in_degree() == 3u);
    CHECK(cfe4.max_out_degree() == 3u);
    CHECK(cfe4.is_regular());

    CHECK(cfe4(0, 0) == 0);
    CHECK(cfe4(0, 1) == 3);
    CHECK(cfe4(0, 2) == 0);
    CHECK(cfe4(0, 3) == 3);
    CHECK(cfe4(1, 0) == 1);
    CHECK(cfe4(1, 1) == 2);
    CHECK(cfe4(1, 2) == 1);
    CHECK(cfe4(1, 3) == 2);
    CHECK(cfe4(2, 0) == 0);
    CHECK(cfe4(2, 1) == 3);
    CHECK(cfe4(2, 2) == 0);
    CHECK(cfe4(2, 3) == 3);
    CHECK(cfe4(3, 0) == 1);
    CHECK(cfe4(3, 1) == 2);
    CHECK(cfe4(3, 2) == 1);
    CHECK(cfe4(3, 3) == 2);

    CHECK(cfe4(0 + 4, 0) == 0);
    CHECK(cfe4(0 + 4, 1) == 3);
    CHECK(cfe4(0 + 4, 2) == 0);
    CHECK(cfe4(0 + 4, 3) == 3);
    CHECK(cfe4(1 + 4, 0) == 1);
    CHECK(cfe4(1 + 4, 1) == 2);
    CHECK(cfe4(1 + 4, 2) == 1);
    CHECK(cfe4(1 + 4, 3) == 2);
    CHECK(cfe4(2 + 4, 0) == 0);
    CHECK(cfe4(2 + 4, 1) == 3);
    CHECK(cfe4(2 + 4, 2) == 0);
    CHECK(cfe4(2 + 4, 3) == 3);
    CHECK(cfe4(3 + 4, 0) == 1);
    CHECK(cfe4(3 + 4, 1) == 2);
    CHECK(cfe4(3 + 4, 2) == 1);
    CHECK(cfe4(3 + 4, 3) == 2);

    CHECK(cfe4(0, 0 + 4) == 0);
    CHECK(cfe4(0, 1 + 4) == 3);
    CHECK(cfe4(0, 2 + 4) == 0);
    CHECK(cfe4(0, 3 + 4) == 3);
    CHECK(cfe4(1, 0 + 4) == 1);
    CHECK(cfe4(1, 1 + 4) == 2);
    CHECK(cfe4(1, 2 + 4) == 1);
    CHECK(cfe4(1, 3 + 4) == 2);
    CHECK(cfe4(2, 0 + 4) == 0);
    CHECK(cfe4(2, 1 + 4) == 3);
    CHECK(cfe4(2, 2 + 4) == 0);
    CHECK(cfe4(2, 3 + 4) == 3);
    CHECK(cfe4(3, 0 + 4) == 1);
    CHECK(cfe4(3, 1 + 4) == 2);
    CHECK(cfe4(3, 2 + 4) == 1);
    CHECK(cfe4(3, 3 + 4) == 2);

    CHECK(cfe4(0 + 4, 0 + 4) == 0);
    CHECK(cfe4(0 + 4, 1 + 4) == 3);
    CHECK(cfe4(0 + 4, 2 + 4) == 0);
    CHECK(cfe4(0 + 4, 3 + 4) == 3);
    CHECK(cfe4(1 + 4, 0 + 4) == 1);
    CHECK(cfe4(1 + 4, 1 + 4) == 2);
    CHECK(cfe4(1 + 4, 2 + 4) == 1);
    CHECK(cfe4(1 + 4, 3 + 4) == 2);
    CHECK(cfe4(2 + 4, 0 + 4) == 0);
    CHECK(cfe4(2 + 4, 1 + 4) == 3);
    CHECK(cfe4(2 + 4, 2 + 4) == 0);
    CHECK(cfe4(2 + 4, 3 + 4) == 3);
    CHECK(cfe4(3 + 4, 0 + 4) == 1);
    CHECK(cfe4(3 + 4, 1 + 4) == 2);
    CHECK(cfe4(3 + 4, 2 + 4) == 1);
    CHECK(cfe4(3 + 4, 3 + 4) == 2);
}

TEST_CASE("4-phase Ripple", "[clocking-scheme]")
{
    const auto ripple4 = clocking::ripple();

    CHECK(ripple4.num_clocks() == 4u);
    CHECK(ripple4.max_in_degree() == 3u);
    CHECK(ripple4.max_out_degree() == 3u);
    CHECK(ripple4.is_regular());

    CHECK(ripple4(0, 0) == 0);
    CHECK(ripple4(0, 1) == 3);
    CHECK(ripple4(0, 2) == 0);
    CHECK(ripple4(0, 3) == 3);
    CHECK(ripple4(1, 0) == 1);
    CHECK(ripple4(1, 1) == 2);
    CHECK(ripple4(1, 2) == 1);
    CHECK(ripple4(1, 3) == 2);
    CHECK(ripple4(2, 0) == 2);
    CHECK(ripple4(2, 1) == 1);
    CHECK(ripple4(2, 2) == 2);
    CHECK(ripple4(2, 3) == 1);
    CHECK(ripple4(3, 0) == 3);
    CHECK(ripple4(3, 1) == 0);
    CHECK(ripple4(3, 2) == 3);
    CHECK(ripple4(3, 3) == 0);

    CHECK(ripple4(0 + 4, 0) == 0);
    CHECK(ripple4(0 + 4, 1) == 3);
    CHECK(ripple4(0 + 4, 2) == 0);
    CHECK(ripple4(0 + 4, 3) == 3);
    CHECK(ripple4(1 + 4, 0) == 1);
    CHECK(ripple4(1 + 4, 1) == 2);
    CHECK(ripple4(1 + 4, 2) == 1);
    CHECK(ripple4(1 + 4, 3) == 2);
    CHECK(ripple4(2 + 4, 0) == 2);
    CHECK(ripple4(2 + 4, 1) == 1);
    CHECK(ripple4(2 + 4, 2) == 2);
    CHECK(ripple4(2 + 4, 3) == 1);
    CHECK(ripple4(3 + 4, 0) == 3);
    CHECK(ripple4(3 + 4, 1) == 0);
    CHECK(ripple4(3 + 4, 2) == 3);
    CHECK(ripple4(3 + 4, 3) == 0);

    CHECK(ripple4(0, 0 + 4) == 0);
    CHECK(ripple4(0, 1 + 4) == 3);
    CHECK(ripple4(0, 2 + 4) == 0);
    CHECK(ripple4(0, 3 + 4) == 3);
    CHECK(ripple4(1, 0 + 4) == 1);
    CHECK(ripple4(1, 1 + 4) == 2);
    CHECK(ripple4(1, 2 + 4) == 1);
    CHECK(ripple4(1, 3 + 4) == 2);
    CHECK(ripple4(2, 0 + 4) == 2);
    CHECK(ripple4(2, 1 + 4) == 1);
    CHECK(ripple4(2, 2 + 4) == 2);
    CHECK(ripple4(2, 3 + 4) == 1);
    CHECK(ripple4(3, 0 + 4) == 3);
    CHECK(ripple4(3, 1 + 4) == 0);
    CHECK(ripple4(3, 2 + 4) == 3);
    CHECK(ripple4(3, 3 + 4) == 0);

    CHECK(ripple4(0 + 4, 0 + 4) == 0);
    CHECK(ripple4(0 + 4, 1 + 4) == 3);
    CHECK(ripple4(0 + 4, 2 + 4) == 0);
    CHECK(ripple4(0 + 4, 3 + 4) == 3);
    CHECK(ripple4(1 + 4, 0 + 4) == 1);
    CHECK(ripple4(1 + 4, 1 + 4) == 2);
    CHECK(ripple4(1 + 4, 2 + 4) == 1);
    CHECK(ripple4(1 + 4, 3 + 4) == 2);
    CHECK(ripple4(2 + 4, 0 + 4) == 2);
    CHECK(ripple4(2 + 4, 1 + 4) == 1);
    CHECK(ripple4(2 + 4, 2 + 4) == 2);
    CHECK(ripple4(2 + 4, 3 + 4) == 1);
    CHECK(ripple4(3 + 4, 0 + 4) == 3);
    CHECK(ripple4(3 + 4, 1 + 4) == 0);
    CHECK(ripple4(3 + 4, 2 + 4) == 3);
    CHECK(ripple4(3 + 4, 3 + 4) == 0);
}

TEST_CASE("4-phase SRS", "[clocking-scheme]")
{
    const auto srs4 = clocking::srs();

    CHECK(srs4.num_clocks() == 4u);
    CHECK(srs4.max_in_degree() == 3u);
    CHECK(srs4.max_out_degree() == 3u);
    CHECK(srs4.is_regular());

    CHECK(srs4(0, 0) == 1);
    CHECK(srs4(0, 1) == 2);
    CHECK(srs4(0, 2) == 3);
    CHECK(srs4(0, 3) == 0);
    CHECK(srs4(1, 0) == 0);
    CHECK(srs4(1, 1) == 3);
    CHECK(srs4(1, 2) == 2);
    CHECK(srs4(1, 3) == 1);
    CHECK(srs4(2, 0) == 3);
    CHECK(srs4(2, 1) == 0);
    CHECK(srs4(2, 2) == 3);
    CHECK(srs4(2, 3) == 2);
    CHECK(srs4(3, 0) == 2);
    CHECK(srs4(3, 1) == 1);
    CHECK(srs4(3, 2) == 0);
    CHECK(srs4(3, 3) == 1);

    CHECK(srs4(0 + 4, 0) == 1);
    CHECK(srs4(0 + 4, 1) == 2);
    CHECK(srs4(0 + 4, 2) == 3);
    CHECK(srs4(0 + 4, 3) == 0);
    CHECK(srs4(1 + 4, 0) == 0);
    CHECK(srs4(1 + 4, 1) == 3);
    CHECK(srs4(1 + 4, 2) == 2);
    CHECK(srs4(1 + 4, 3) == 1);
    CHECK(srs4(2 + 4, 0) == 3);
    CHECK(srs4(2 + 4, 1) == 0);
    CHECK(srs4(2 + 4, 2) == 3);
    CHECK(srs4(2 + 4, 3) == 2);
    CHECK(srs4(3 + 4, 0) == 2);
    CHECK(srs4(3 + 4, 1) == 1);
    CHECK(srs4(3 + 4, 2) == 0);
    CHECK(srs4(3 + 4, 3) == 1);

    CHECK(srs4(0, 0 + 4) == 1);
    CHECK(srs4(0, 1 + 4) == 2);
    CHECK(srs4(0, 2 + 4) == 3);
    CHECK(srs4(0, 3 + 4) == 0);
    CHECK(srs4(1, 0 + 4) == 0);
    CHECK(srs4(1, 1 + 4) == 3);
    CHECK(srs4(1, 2 + 4) == 2);
    CHECK(srs4(1, 3 + 4) == 1);
    CHECK(srs4(2, 0 + 4) == 3);
    CHECK(srs4(2, 1 + 4) == 0);
    CHECK(srs4(2, 2 + 4) == 3);
    CHECK(srs4(2, 3 + 4) == 2);
    CHECK(srs4(3, 0 + 4) == 2);
    CHECK(srs4(3, 1 + 4) == 1);
    CHECK(srs4(3, 2 + 4) == 0);
    CHECK(srs4(3, 3 + 4) == 1);

    CHECK(srs4(0 + 4, 0 + 4) == 1);
    CHECK(srs4(0 + 4, 1 + 4) == 2);
    CHECK(srs4(0 + 4, 2 + 4) == 3);
    CHECK(srs4(0 + 4, 3 + 4) == 0);
    CHECK(srs4(1 + 4, 0 + 4) == 0);
    CHECK(srs4(1 + 4, 1 + 4) == 3);
    CHECK(srs4(1 + 4, 2 + 4) == 2);
    CHECK(srs4(1 + 4, 3 + 4) == 1);
    CHECK(srs4(2 + 4, 0 + 4) == 3);
    CHECK(srs4(2 + 4, 1 + 4) == 0);
    CHECK(srs4(2 + 4, 2 + 4) == 3);
    CHECK(srs4(2 + 4, 3 + 4) == 2);
    CHECK(srs4(3 + 4, 0 + 4) == 2);
    CHECK(srs4(3 + 4, 1 + 4) == 1);
    CHECK(srs4(3 + 4, 2 + 4) == 0);
    CHECK(srs4(3 + 4, 3 + 4) == 1);
}

TEST_CASE("3-phase BANCS", "[clocking-scheme]")
{
    const auto bancs3 = clocking::bancs();

    CHECK(bancs3.num_clocks() == 3u);
    CHECK(bancs3.max_in_degree() == 2u);
    CHECK(bancs3.max_out_degree() == 2u);
    CHECK(bancs3.is_regular());

    CHECK(bancs3(0, 0) == 0);
    CHECK(bancs3(0, 1) == 2);
    CHECK(bancs3(0, 2) == 2);
    CHECK(bancs3(0, 3) == 1);
    CHECK(bancs3(0, 4) == 1);
    CHECK(bancs3(0, 5) == 0);
    CHECK(bancs3(1, 0) == 1);
    CHECK(bancs3(1, 1) == 1);
    CHECK(bancs3(1, 2) == 0);
    CHECK(bancs3(1, 3) == 0);
    CHECK(bancs3(1, 4) == 2);
    CHECK(bancs3(1, 5) == 2);
    CHECK(bancs3(2, 0) == 2);
    CHECK(bancs3(2, 1) == 0);
    CHECK(bancs3(2, 2) == 1);
    CHECK(bancs3(2, 3) == 2);
    CHECK(bancs3(2, 4) == 0);
    CHECK(bancs3(2, 5) == 1);

    CHECK(bancs3(0 + 3, 0) == 0);
    CHECK(bancs3(0 + 3, 1) == 2);
    CHECK(bancs3(0 + 3, 2) == 2);
    CHECK(bancs3(0 + 3, 3) == 1);
    CHECK(bancs3(0 + 3, 4) == 1);
    CHECK(bancs3(0 + 3, 5) == 0);
    CHECK(bancs3(1 + 3, 0) == 1);
    CHECK(bancs3(1 + 3, 1) == 1);
    CHECK(bancs3(1 + 3, 2) == 0);
    CHECK(bancs3(1 + 3, 3) == 0);
    CHECK(bancs3(1 + 3, 4) == 2);
    CHECK(bancs3(1 + 3, 5) == 2);
    CHECK(bancs3(2 + 3, 0) == 2);
    CHECK(bancs3(2 + 3, 1) == 0);
    CHECK(bancs3(2 + 3, 2) == 1);
    CHECK(bancs3(2 + 3, 3) == 2);
    CHECK(bancs3(2 + 3, 4) == 0);
    CHECK(bancs3(2 + 3, 5) == 1);

    CHECK(bancs3(0, 0 + 6) == 0);
    CHECK(bancs3(0, 1 + 6) == 2);
    CHECK(bancs3(0, 2 + 6) == 2);
    CHECK(bancs3(0, 3 + 6) == 1);
    CHECK(bancs3(0, 4 + 6) == 1);
    CHECK(bancs3(0, 5 + 6) == 0);
    CHECK(bancs3(1, 0 + 6) == 1);
    CHECK(bancs3(1, 1 + 6) == 1);
    CHECK(bancs3(1, 2 + 6) == 0);
    CHECK(bancs3(1, 3 + 6) == 0);
    CHECK(bancs3(1, 4 + 6) == 2);
    CHECK(bancs3(1, 5 + 6) == 2);
    CHECK(bancs3(2, 0 + 6) == 2);
    CHECK(bancs3(2, 1 + 6) == 0);
    CHECK(bancs3(2, 2 + 6) == 1);
    CHECK(bancs3(2, 3 + 6) == 2);
    CHECK(bancs3(2, 4 + 6) == 0);
    CHECK(bancs3(2, 5 + 6) == 1);

    CHECK(bancs3(0 + 3, 0 + 6) == 0);
    CHECK(bancs3(0 + 3, 1 + 6) == 2);
    CHECK(bancs3(0 + 3, 2 + 6) == 2);
    CHECK(bancs3(0 + 3, 3 + 6) == 1);
    CHECK(bancs3(0 + 3, 4 + 6) == 1);
    CHECK(bancs3(0 + 3, 5 + 6) == 0);
    CHECK(bancs3(1 + 3, 0 + 6) == 1);
    CHECK(bancs3(1 + 3, 1 + 6) == 1);
    CHECK(bancs3(1 + 3, 2 + 6) == 0);
    CHECK(bancs3(1 + 3, 3 + 6) == 0);
    CHECK(bancs3(1 + 3, 4 + 6) == 2);
    CHECK(bancs3(1 + 3, 5 + 6) == 2);
    CHECK(bancs3(2 + 3, 0 + 6) == 2);
    CHECK(bancs3(2 + 3, 1 + 6) == 0);
    CHECK(bancs3(2 + 3, 2 + 6) == 1);
    CHECK(bancs3(2 + 3, 3 + 6) == 2);
    CHECK(bancs3(2 + 3, 4 + 6) == 0);
    CHECK(bancs3(2 + 3, 5 + 6) == 1);
}

TEST_CASE("Override clocking", "[clocking-scheme]")
{
    auto twoddwave4 = clocking::twoddwave();

    CHECK(twoddwave4.is_regular());

    twoddwave4.override_clock_number(0, 0, 0);

    CHECK(!twoddwave4.is_regular());

    twoddwave4.override_clock_number(0, 1, 1);
    twoddwave4.override_clock_number(0, 2, 2);
    twoddwave4.override_clock_number(0, 3, 3);
    twoddwave4.override_clock_number(0, 4, 4);
    twoddwave4.override_clock_number(0, 5, 5);
    twoddwave4.override_clock_number(0, 6, 6);

    CHECK(twoddwave4(0, 0) == 0);
    CHECK(twoddwave4(0, 1) == 1);
    CHECK(twoddwave4(0, 2) == 2);
    CHECK(twoddwave4(0, 3) == 3);
    CHECK(twoddwave4(0, 4) == 0);
    CHECK(twoddwave4(0, 5) == 1);
    CHECK(twoddwave4(0, 6) == 2);
    CHECK(twoddwave4(1, 0) == 1);
    CHECK(twoddwave4(1, 1) == 2);
    CHECK(twoddwave4(1, 2) == 3);
    CHECK(twoddwave4(1, 3) == 0);
    CHECK(twoddwave4(2, 0) == 2);
    CHECK(twoddwave4(2, 1) == 3);
    CHECK(twoddwave4(2, 2) == 0);
    CHECK(twoddwave4(2, 3) == 1);
    CHECK(twoddwave4(3, 0) == 3);
    CHECK(twoddwave4(3, 1) == 0);
    CHECK(twoddwave4(3, 2) == 1);
    CHECK(twoddwave4(3, 3) == 2);
}

TEST_CASE("4-phase ESR", "[clocking-scheme]")
{
    const auto esr4 = clocking::esr();

    CHECK(esr4.num_clocks() == 4u);
    CHECK(esr4.max_in_degree() == 3u);
    CHECK(esr4.max_out_degree() == 3u);
    CHECK(esr4.is_regular());

    CHECK(esr4(0, 0) == 3);
    CHECK(esr4(0, 1) == 0);
    CHECK(esr4(0, 2) == 1);
    CHECK(esr4(0, 3) == 0);
    CHECK(esr4(1, 0) == 0);
    CHECK(esr4(1, 1) == 1);
    CHECK(esr4(1, 2) == 2);
    CHECK(esr4(1, 3) == 3);
    CHECK(esr4(2, 0) == 1);
    CHECK(esr4(2, 1) == 2);
    CHECK(esr4(2, 2) == 3);
    CHECK(esr4(2, 3) == 2);
    CHECK(esr4(3, 0) == 2);
    CHECK(esr4(3, 1) == 3);
    CHECK(esr4(3, 2) == 0);
    CHECK(esr4(3, 3) == 1);

    CHECK(esr4(0 + 4, 0) == 3);
    CHECK(esr4(0 + 4, 1) == 0);
    CHECK(esr4(0 + 4, 2) == 1);
    CHECK(esr4(0 + 4, 3) == 0);
    CHECK(esr4(1 + 4, 0) == 0);
    CHECK(esr4(1 + 4, 1) == 1);
    CHECK(esr4(1 + 4, 2) == 2);
    CHECK(esr4(1 + 4, 3) == 3);
    CHECK(esr4(2 + 4, 0) == 1);
    CHECK(esr4(2 + 4, 1) == 2);
    CHECK(esr4(2 + 4, 2) == 3);
    CHECK(esr4(2 + 4, 3) == 2);
    CHECK(esr4(3 + 4, 0) == 2);
    CHECK(esr4(3 + 4, 1) == 3);
    CHECK(esr4(3 + 4, 2) == 0);
    CHECK(esr4(3 + 4, 3) == 1);

    CHECK(esr4(0, 0 + 4) == 3);
    CHECK(esr4(0, 1 + 4) == 0);
    CHECK(esr4(0, 2 + 4) == 1);
    CHECK(esr4(0, 3 + 4) == 0);
    CHECK(esr4(1, 0 + 4) == 0);
    CHECK(esr4(1, 1 + 4) == 1);
    CHECK(esr4(1, 2 + 4) == 2);
    CHECK(esr4(1, 3 + 4) == 3);
    CHECK(esr4(2, 0 + 4) == 1);
    CHECK(esr4(2, 1 + 4) == 2);
    CHECK(esr4(2, 2 + 4) == 3);
    CHECK(esr4(2, 3 + 4) == 2);
    CHECK(esr4(3, 0 + 4) == 2);
    CHECK(esr4(3, 1 + 4) == 3);
    CHECK(esr4(3, 2 + 4) == 0);
    CHECK(esr4(3, 3 + 4) == 1);

    CHECK(esr4(0 + 4, 0 + 4) == 3);
    CHECK(esr4(0 + 4, 1 + 4) == 0);
    CHECK(esr4(0 + 4, 2 + 4) == 1);
    CHECK(esr4(0 + 4, 3 + 4) == 0);
    CHECK(esr4(1 + 4, 0 + 4) == 0);
    CHECK(esr4(1 + 4, 1 + 4) == 1);
    CHECK(esr4(1 + 4, 2 + 4) == 2);
    CHECK(esr4(1 + 4, 3 + 4) == 3);
    CHECK(esr4(2 + 4, 0 + 4) == 1);
    CHECK(esr4(2 + 4, 1 + 4) == 2);
    CHECK(esr4(2 + 4, 2 + 4) == 3);
    CHECK(esr4(2 + 4, 3 + 4) == 2);
    CHECK(esr4(3 + 4, 0 + 4) == 2);
    CHECK(esr4(3 + 4, 1 + 4) == 3);
    CHECK(esr4(3 + 4, 2 + 4) == 0);
    CHECK(esr4(3 + 4, 3 + 4) == 1);
}

TEST_CASE("Clocking lookup", "[clocking-scheme]")
{
    using clk_lyt = gate_level_layout<cartesian_layout<coords::offset>>;

    auto check = [](const std::vector<std::string>& vec, const auto& name)
    {
        for (const auto& n : vec)
        {
            auto cs = clocking::get_scheme<clk_lyt>(n);
            REQUIRE(cs.has_value());
            CHECK(cs->name() == name);
        }
    };

    check({"open", "OPEN", "oPeN", "OpEn"}, clocking::OPEN_NAME);
    check({"columnar", "COLUMNAR", "CoLumNar", "COLUMnar"}, clocking::COLUMNAR_NAME);
    check({"row", "ROW", "RoW", "rOw"}, clocking::ROW_NAME);
    check({"2DDwave", "2DdWaVe", "2ddwave", "2DDWAVE", "2DDWave"}, clocking::TWODDWAVE_NAME);
    check({"2DDwavehex", "2DdWaVeHeX", "2ddwavehex", "2DDWAVEHEX", "2DDWaveHex"}, clocking::TWODDWAVE_NAME);
    check({"use", "USE", "uSe", "UsE"}, clocking::USE_NAME);
    check({"res", "RES", "rEs", "ReS"}, clocking::RES_NAME);
    check({"esr", "ESR", "eSr", "EsR"}, clocking::ESR_NAME);
    check({"cfe", "CFE", "cFe", "CfE"}, clocking::CFE_NAME);
    check({"ripple", "RIPPLE", "RiPpLe", "RIppLE"}, clocking::RIPPLE_NAME);
    check({"srs", "SRS", "sRs", "SrS"}, clocking::SRS_NAME);
    check({"bancs", "BANCS", "BaNCs", "banCS"}, clocking::BANCS_NAME);

    check({"bancs3", "BANCS3"}, clocking::BANCS_NAME);

    CHECK(clocking::get_scheme<clk_lyt>("2DDWAVEHEX3") == clocking::twoddwave(clocking::num_clks::THREE));
    CHECK(clocking::get_scheme<clk_lyt>("BANCS3") == clocking::bancs());

    CHECK(!clocking::get_scheme<clk_lyt>("").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("Column").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("Rows").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("TwoDDWave").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("2DDWave6").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("SUE").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("SER").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("ERS").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("CEF").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("RPIPLE").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("SSR").has_value());
    CHECK(!clocking::get_scheme<clk_lyt>("BNCS").has_value());
}

TEST_CASE("Linear schemes", "[clocking-scheme]")
{
    using clk_lyt = gate_level_layout<cartesian_layout<coords::offset>>;

    auto check_linear_scheme = [](const auto& name, bool expected)
    {
        auto cs = clocking::get_scheme<clk_lyt>(name);
        REQUIRE(cs.has_value());
        CHECK(clocking::is_linear(*cs) == expected);
    };

    // Linear clocking schemes
    check_linear_scheme(clocking::COLUMNAR_NAME, true);
    check_linear_scheme(clocking::ROW_NAME, true);
    check_linear_scheme(clocking::TWODDWAVE_NAME, true);
    check_linear_scheme(clocking::TWODDWAVE_HEX_NAME, true);

    // Non-linear clocking schemes
    check_linear_scheme(clocking::OPEN_NAME, false);
    check_linear_scheme(clocking::USE_NAME, false);
    check_linear_scheme(clocking::RES_NAME, false);
    check_linear_scheme(clocking::ESR_NAME, false);
    check_linear_scheme(clocking::CFE_NAME, false);
    check_linear_scheme(clocking::RIPPLE_NAME, false);
    check_linear_scheme(clocking::SRS_NAME, false);
    check_linear_scheme(clocking::BANCS_NAME, false);
}

TEST_CASE("Clocking scheme construction validates its cutout", "[clocking-scheme]")
{
    using cutout = std::vector<std::vector<clocking::scheme::clock_number>>;

    CHECK_THROWS_AS((clocking::scheme{"EMPTY", cutout{}, 4u, 2u, 2u}), std::invalid_argument);
    CHECK_THROWS_AS((clocking::scheme{"EMPTY", cutout{{}}, 4u, 2u, 2u}), std::invalid_argument);
    CHECK_THROWS_AS((clocking::scheme{"RAGGED", cutout{{0, 1}, {2}}, 4u, 2u, 2u}), std::invalid_argument);
    CHECK_THROWS_AS((clocking::scheme{"RANGE", cutout{{0, 1, 2, 3}}, 3u, 2u, 2u}), std::invalid_argument);
    CHECK_THROWS_AS((clocking::scheme{"FIVE", cutout{{0, 1, 2, 3, 4}}, 5u, 2u, 2u}), std::invalid_argument);
    CHECK_THROWS_AS((clocking::scheme{"TWO", cutout{{0, 1}}, 2u, 2u, 2u}), std::invalid_argument);

    const clocking::scheme custom{"CUSTOM", cutout{{0, 1}, {2, 0}}, 3u, 1u, 2u};

    CHECK(custom.name() == "CUSTOM");
    CHECK(custom.num_clocks() == 3u);
    CHECK(custom.max_in_degree() == 1u);
    CHECK(custom.max_out_degree() == 2u);
    CHECK(custom.is_regular());
    CHECK(custom(1, 1) == 0);
    CHECK(custom(0, 3) == 2);
}

TEST_CASE("Clocking schemes are copyable, assignable, and comparable", "[clocking-scheme]")
{
    auto original = clocking::twoddwave();
    auto copy     = original;

    CHECK(copy == original);

    copy.override_clock_number(0, 0, 3);

    CHECK(copy != original);
    CHECK(copy(0, 0) == 3);
    CHECK(original(0, 0) == 0);
    CHECK(!copy.is_regular());
    CHECK(original.is_regular());

    original.override_clock_number(0, 0, 7);  // stored modulo the phase count

    CHECK(copy == original);

    copy = clocking::use();

    CHECK(copy.name() == clocking::USE_NAME);
    CHECK(copy != clocking::twoddwave());
    CHECK(clocking::twoddwave(clocking::num_clks::THREE) != clocking::twoddwave());
    CHECK(clocking::twoddwave_hex(clocking::hex_arrangement::ODD_ROW) !=
          clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_ROW));
    CHECK(clocking::res() != clocking::esr());
}

TEST_CASE("Clocking schemes repeat at negative coordinates", "[clocking-scheme]")
{
    const auto columnar3 = clocking::columnar(clocking::num_clks::THREE);

    CHECK(columnar3(-1, 0) == 2);
    CHECK(columnar3(-2, 0) == 1);
    CHECK(columnar3(-3, 0) == 0);
    CHECK(columnar3(-1, -1) == 2);

    const auto bancs3 = clocking::bancs();

    CHECK(bancs3(0, -1) == bancs3(0, 5));
    CHECK(bancs3(-1, -1) == bancs3(2, 5));
    CHECK(bancs3(-3, -6) == bancs3(0, 0));

    const auto twoddwave4 = clocking::twoddwave();

    CHECK(twoddwave4(-1, 0) == 3);
    CHECK(twoddwave4(0, -1) == 3);
    CHECK(twoddwave4(-1, -1) == 2);
}

TEST_CASE("Clocking lookup by phase count and hexagonal arrangement", "[clocking-scheme]")
{
    const auto phases = [](const std::string& name) -> std::optional<unsigned>
    {
        const auto scm = clocking::get_scheme(name);
        return scm.has_value() ? std::optional<unsigned>{scm->num_clocks()} : std::nullopt;
    };

    for (const auto* const name : {"OPEN", "COLUMNAR", "ROW", "2DDWAVE", "2DDWAVEHEX"})
    {
        INFO(name);
        CHECK(phases(name) == 4u);
        CHECK(phases(std::string{name} + "3") == 3u);
        CHECK(phases(std::string{name} + "4") == 4u);
    }

    for (const auto* const name : {"USE", "RES", "ESR", "CFE", "RIPPLE", "SRS"})
    {
        INFO(name);
        CHECK(phases(std::string{name} + "4") == 4u);
        CHECK(!clocking::get_scheme(std::string{name} + "3").has_value());
    }

    CHECK(phases("BANCS") == 3u);
    CHECK(phases("bancs3") == 3u);
    CHECK(!clocking::get_scheme("BANCS4").has_value());
    CHECK(!clocking::get_scheme("3").has_value());

    CHECK(clocking::get_scheme("2DDWaveHex3") == clocking::twoddwave(clocking::num_clks::THREE));
    CHECK(clocking::get_scheme("2DDWaveHex", clocking::hex_arrangement::EVEN_COLUMN) ==
          clocking::twoddwave_hex(clocking::hex_arrangement::EVEN_COLUMN));
    CHECK(clocking::get_scheme<gate_level_layout<hexagonal_layout<coords::offset, odd_column_hex>>>("2DDWaveHex3") ==
          clocking::twoddwave_hex(clocking::hex_arrangement::ODD_COLUMN, clocking::num_clks::THREE));
}
