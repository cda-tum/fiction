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

#include <fiction/types.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <sstream>

using namespace fiction;
using namespace fiction::verification;

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
    check_for_drvs(hex_even_col_gate_clk_lyt{}, 0, 0);
    check_for_drvs(hex_odd_col_gate_clk_lyt{}, 0, 0);
    check_for_drvs(hex_even_row_gate_clk_lyt{}, 0, 0);
    check_for_drvs(hex_odd_row_gate_clk_lyt{}, 0, 0);

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
    check_for_drvs(blueprints::non_structural_all_function_gate_layout<cart_gate_clk_lyt>(), 50, 1);
}
