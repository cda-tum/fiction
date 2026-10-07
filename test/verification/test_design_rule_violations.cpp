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
 * @brief Tests for `fiction/verification/design_rule_violations.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <cstddef>
#include <sstream>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::verification;

/** @brief Checks design rules without printing. @tparam Lyt Layout type. @param lyt Layout. @return DRV statistics. */
template <typename Lyt>
gate_level_drv_stats get_drvs(const Lyt& lyt)
{
    gate_level_drv_params ps{};
    gate_level_drv_stats  st{};

    // suppress standard output
    std::stringstream ss{};
    ps.out = &ss;

    gate_level_drvs(lyt, ps, &st);

    return st;
}

/** @brief Checks issue counts. @tparam Lyt Layout type. @param lyt Layout. @param num_drvs Expected DRVs. @param
 * num_warnings Expected warnings. */
template <typename Lyt>
void check_for_drvs(const Lyt& lyt, const std::size_t num_drvs, const std::size_t num_warnings)
{
    const auto st = get_drvs(lyt);

    CHECK(st.drvs == num_drvs);
    CHECK(st.warnings == num_warnings);
}

TEST_CASE("Intact layouts", "[drv]")
{
    // empty layouts
    check_for_drvs(cart_gate_clk_lyt{}, 0, 0);
    check_for_drvs(hex_gate_clk_lyt{arrangement::EVEN_COLUMN}, 0, 0);
    check_for_drvs(hex_gate_clk_lyt{arrangement::ODD_COLUMN}, 0, 0);
    check_for_drvs(hex_gate_clk_lyt{arrangement::EVEN_ROW}, 0, 0);
    check_for_drvs(hex_gate_clk_lyt{arrangement::ODD_ROW}, 0, 0);

    // Cartesian gate layouts
    check_for_drvs(blueprints::and_or_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::and_not_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::or_not_gate_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::crossing_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::fanout_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(), 0, 0);
    check_for_drvs(blueprints::se_gate_layout<cart_gate_clk_lyt>(), 0, 0);
}

TEST_CASE("Warnings", "[drv]")
{
    // PI tile (1,1) is not at a border
    check_for_drvs(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(), 0, 1);
}

TEST_CASE("DRVs", "[drv]")
{
    check_for_drvs(blueprints::non_structural_all_function_gate_layout<cart_gate_clk_lyt>(), 46, 1);
}

TEST_CASE("DRVs inspect declared input holes", "[drv][placed-objects]")
{
    cart_gate_clk_lyt          lyt{{1, 3}, clocking::twoddwave()};
    const auto                 pi = lyt.create_pi("a", {0, 0});
    kitty::dynamic_truth_table function{2};
    kitty::create_from_hex_string(function, "8");
    const auto gate = lyt.create_node({pi}, function, {0, 1});
    lyt.disconnect({gate.object, 0});
    lyt.connect(pi, {gate.object, 1});
    lyt.create_po(gate, "result", {0, 2});
    const auto stats = get_drvs(lyt);
    CHECK(stats.drvs == 1);
    CHECK(stats.report["Missing connections"].size() == 1);
    lyt.connect(pi, {gate.object, 0});
    CHECK(get_drvs(lyt).drvs == 0);
}

TEST_CASE("DRVs visit objects outside the zero-origin extent", "[drv][placed-objects]")
{
    cart_gate_clk_lyt lyt{{1, 1}};
    const auto        pi = lyt.create_pi("a", {-10, -2});
    lyt.create_po(pi, "result", {10, -2});
    const auto stats = get_drvs(lyt);
    CHECK(stats.report["Non adjacent connections"].size() == 2);
    CHECK(stats.drvs >= 1);
}
