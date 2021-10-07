//
// Created by marcel on 12.05.21.
//

#include "catch.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <string>
#include <vector>

using namespace fiction;

TEST_CASE("3-phase 2DDWave", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto twoddwave3 = twoddwave_3_clocking<clk_lyt>();

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
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto twoddwave4 = twoddwave_4_clocking<clk_lyt>();

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

TEST_CASE("4-phase USE", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto use4 = use_4_clocking<clk_lyt>();

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
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto res4 = res_4_clocking<clk_lyt>();

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

TEST_CASE("3-phase BANCS", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto bancs3 = bancs_3_clocking<clk_lyt>();

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

TEST_CASE("3-phase ToPoliNano", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto topolinano3 = topolinano_3_clocking<clk_lyt>();

    CHECK(topolinano3.is_regular());

    CHECK(topolinano3({0, 0}) == 0);
    CHECK(topolinano3({0, 1}) == 0);
    CHECK(topolinano3({0, 2}) == 0);
    CHECK(topolinano3({1, 0}) == 1);
    CHECK(topolinano3({1, 1}) == 1);
    CHECK(topolinano3({1, 2}) == 1);
    CHECK(topolinano3({2, 0}) == 2);
    CHECK(topolinano3({2, 1}) == 2);
    CHECK(topolinano3({2, 2}) == 2);

    CHECK(topolinano3({0 + 3, 0}) == 0);
    CHECK(topolinano3({0 + 3, 1}) == 0);
    CHECK(topolinano3({0 + 3, 2}) == 0);
    CHECK(topolinano3({1 + 3, 0}) == 1);
    CHECK(topolinano3({1 + 3, 1}) == 1);
    CHECK(topolinano3({1 + 3, 2}) == 1);
    CHECK(topolinano3({2 + 3, 0}) == 2);
    CHECK(topolinano3({2 + 3, 1}) == 2);
    CHECK(topolinano3({2 + 3, 2}) == 2);

    CHECK(topolinano3({0, 0 + 3}) == 0);
    CHECK(topolinano3({0, 1 + 3}) == 0);
    CHECK(topolinano3({0, 2 + 3}) == 0);
    CHECK(topolinano3({1, 0 + 3}) == 1);
    CHECK(topolinano3({1, 1 + 3}) == 1);
    CHECK(topolinano3({1, 2 + 3}) == 1);
    CHECK(topolinano3({2, 0 + 3}) == 2);
    CHECK(topolinano3({2, 1 + 3}) == 2);
    CHECK(topolinano3({2, 2 + 3}) == 2);

    CHECK(topolinano3({0 + 3, 0 + 3}) == 0);
    CHECK(topolinano3({0 + 3, 1 + 3}) == 0);
    CHECK(topolinano3({0 + 3, 2 + 3}) == 0);
    CHECK(topolinano3({1 + 3, 0 + 3}) == 1);
    CHECK(topolinano3({1 + 3, 1 + 3}) == 1);
    CHECK(topolinano3({1 + 3, 2 + 3}) == 1);
    CHECK(topolinano3({2 + 3, 0 + 3}) == 2);
    CHECK(topolinano3({2 + 3, 1 + 3}) == 2);
    CHECK(topolinano3({2 + 3, 2 + 3}) == 2);
}

TEST_CASE("4-phase ToPoliNano", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    const auto topolinano4 = topolinano_4_clocking<clk_lyt>();

    CHECK(topolinano4.is_regular());

    CHECK(topolinano4({0, 0}) == 0);
    CHECK(topolinano4({0, 1}) == 0);
    CHECK(topolinano4({0, 2}) == 0);
    CHECK(topolinano4({0, 3}) == 0);
    CHECK(topolinano4({1, 0}) == 1);
    CHECK(topolinano4({1, 1}) == 1);
    CHECK(topolinano4({1, 2}) == 1);
    CHECK(topolinano4({1, 3}) == 1);
    CHECK(topolinano4({2, 0}) == 2);
    CHECK(topolinano4({2, 1}) == 2);
    CHECK(topolinano4({2, 2}) == 2);
    CHECK(topolinano4({2, 3}) == 2);
    CHECK(topolinano4({3, 0}) == 3);
    CHECK(topolinano4({3, 1}) == 3);
    CHECK(topolinano4({3, 2}) == 3);
    CHECK(topolinano4({3, 3}) == 3);

    CHECK(topolinano4({0 + 4, 0}) == 0);
    CHECK(topolinano4({0 + 4, 1}) == 0);
    CHECK(topolinano4({0 + 4, 2}) == 0);
    CHECK(topolinano4({0 + 4, 3}) == 0);
    CHECK(topolinano4({1 + 4, 0}) == 1);
    CHECK(topolinano4({1 + 4, 1}) == 1);
    CHECK(topolinano4({1 + 4, 2}) == 1);
    CHECK(topolinano4({1 + 4, 3}) == 1);
    CHECK(topolinano4({2 + 4, 0}) == 2);
    CHECK(topolinano4({2 + 4, 1}) == 2);
    CHECK(topolinano4({2 + 4, 2}) == 2);
    CHECK(topolinano4({2 + 4, 3}) == 2);
    CHECK(topolinano4({3 + 4, 0}) == 3);
    CHECK(topolinano4({3 + 4, 1}) == 3);
    CHECK(topolinano4({3 + 4, 2}) == 3);
    CHECK(topolinano4({3 + 4, 3}) == 3);

    CHECK(topolinano4({0, 0 + 4}) == 0);
    CHECK(topolinano4({0, 1 + 4}) == 0);
    CHECK(topolinano4({0, 2 + 4}) == 0);
    CHECK(topolinano4({0, 3 + 4}) == 0);
    CHECK(topolinano4({1, 0 + 4}) == 1);
    CHECK(topolinano4({1, 1 + 4}) == 1);
    CHECK(topolinano4({1, 2 + 4}) == 1);
    CHECK(topolinano4({1, 3 + 4}) == 1);
    CHECK(topolinano4({2, 0 + 4}) == 2);
    CHECK(topolinano4({2, 1 + 4}) == 2);
    CHECK(topolinano4({2, 2 + 4}) == 2);
    CHECK(topolinano4({2, 3 + 4}) == 2);
    CHECK(topolinano4({3, 0 + 4}) == 3);
    CHECK(topolinano4({3, 1 + 4}) == 3);
    CHECK(topolinano4({3, 2 + 4}) == 3);
    CHECK(topolinano4({3, 3 + 4}) == 3);

    CHECK(topolinano4({0 + 4, 0 + 4}) == 0);
    CHECK(topolinano4({0 + 4, 1 + 4}) == 0);
    CHECK(topolinano4({0 + 4, 2 + 4}) == 0);
    CHECK(topolinano4({0 + 4, 3 + 4}) == 0);
    CHECK(topolinano4({1 + 4, 0 + 4}) == 1);
    CHECK(topolinano4({1 + 4, 1 + 4}) == 1);
    CHECK(topolinano4({1 + 4, 2 + 4}) == 1);
    CHECK(topolinano4({1 + 4, 3 + 4}) == 1);
    CHECK(topolinano4({2 + 4, 0 + 4}) == 2);
    CHECK(topolinano4({2 + 4, 1 + 4}) == 2);
    CHECK(topolinano4({2 + 4, 2 + 4}) == 2);
    CHECK(topolinano4({2 + 4, 3 + 4}) == 2);
    CHECK(topolinano4({3 + 4, 0 + 4}) == 3);
    CHECK(topolinano4({3 + 4, 1 + 4}) == 3);
    CHECK(topolinano4({3 + 4, 2 + 4}) == 3);
    CHECK(topolinano4({3 + 4, 3 + 4}) == 3);
}

TEST_CASE("3-phase open clocking", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    auto open3 = open_3_clocking<clk_lyt>();

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
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    auto open4 = open_4_clocking<clk_lyt>();

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

TEST_CASE("Override clocking", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    auto twoddwave4 = twoddwave_4_clocking<clk_lyt>();

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

TEST_CASE("Clocking lookup", "[clocking-scheme]")
{
    using clk_lyt = clocked_layout<cartesian_layout<cartesian::ucoord_t>>;

    auto check = [](const std::vector<std::string>& vec, const auto& name)
    {
        for (const auto& n : vec)
        {
            auto cs = get_clocking_scheme<clk_lyt>(n);
            REQUIRE(cs.has_value());
            CHECK(cs == name);
        }
    };

    check({"open3", "OPEN3", "oPeN3", "OpEn3"}, clock_name::open3);
    check({"open4", "OPEN4", "oPeN4", "OpEn4", "open", "OPEN"}, clock_name::open4);
    check({"2ddwave3", "2DDwave3", "2ddWAVE3", "2dDwAvE3", "2DdWaVe3", "DIAG3", "diag3"}, clock_name::twoddwave3);
    check({"2ddwave4", "2DdWaVe4", "DIAG4", "diag4", "2ddwave", "2DDWAVE", "2DDWave"}, clock_name::twoddwave4);
    check({"use", "USE", "uSe", "UsE"}, clock_name::use);
    check({"res", "RES", "rEs", "ReS"}, clock_name::res);
    check({"bancs", "BANCS", "BaNCs", "banCS"}, clock_name::bancs);
    check({"topolinano3", "TOPOLINANO3", "ToPoliNano3", "topolinano"}, clock_name::topolinano3);
    check({"topolinano4", "TOPOLINANO4", "ToPoliNano4"}, clock_name::topolinano4);

    CHECK(!get_clocking_scheme<clk_lyt>("").has_value());
    CHECK(!get_clocking_scheme<clk_lyt>("TwoDDWave").has_value());
    CHECK(!get_clocking_scheme<clk_lyt>("BNCS").has_value());
    CHECK(!get_clocking_scheme<clk_lyt>("ToPolino").has_value());
}
