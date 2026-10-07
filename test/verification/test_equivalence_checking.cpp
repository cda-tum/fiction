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
 * @brief Tests for `fiction/verification/equivalence_checking.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/equivalence_checking.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::verification;

template <typename Spec, typename Impl>
void check_for_strong_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    const auto equiv = equivalence_checking(spec, impl, &st);

    CHECK(equiv == eq_type::STRONG);
    CHECK(st.eq == eq_type::STRONG);
    CHECK(st.counter_example.empty());
    CHECK(st.spec_drv_stats.drvs == 0);
    CHECK(st.impl_drv_stats.drvs == 0);
    CHECK(st.tp_diff == 0);
}

template <typename Spec, typename Impl>
void check_for_weak_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    const auto equiv = equivalence_checking(spec, impl, &st);

    CHECK(equiv == eq_type::WEAK);
    CHECK(st.eq == eq_type::WEAK);
    CHECK(st.counter_example.empty());
    CHECK(st.spec_drv_stats.drvs == 0);
    CHECK(st.impl_drv_stats.drvs == 0);
    CHECK(st.tp_diff > 0);
}

template <typename Spec, typename Impl>
void check_for_no_equiv(const Spec& spec, const Impl& impl)
{
    equivalence_checking_stats st{};

    const auto equiv = equivalence_checking(spec, impl, &st);

    CHECK(equiv == eq_type::NO);
    CHECK(((!st.counter_example.empty()) || (st.spec_drv_stats.drvs != 0) || (st.impl_drv_stats.drvs != 0)));
}

TEST_CASE("Network-network equivalence", "[equiv]")
{
    check_for_strong_equiv(mockturtle::aig_network{}, mockturtle::aig_network{});
    check_for_strong_equiv(mockturtle::mig_network{}, mockturtle::mig_network{});
    check_for_strong_equiv(mockturtle::aig_network{}, mockturtle::mig_network{});

    check_for_strong_equiv(blueprints::maj4_network<mockturtle::aig_network>(),
                           blueprints::maj4_network<mockturtle::mig_network>());
    check_for_strong_equiv(blueprints::maj4_network<mockturtle::mig_network>(),
                           blueprints::maj4_network<mockturtle::xag_network>());
    check_for_strong_equiv(blueprints::maj4_network<mockturtle::xag_network>(),
                           blueprints::maj4_network<technology_network>());
    check_for_strong_equiv(blueprints::maj4_network<technology_network>(),
                           blueprints::maj4_network<mockturtle::aig_network>());
}

TEST_CASE("Network-layout equivalence", "[equiv]")
{
    SECTION("Cartesian layout")
    {
        using gate_lyt = gate_level_layout<cartesian_layout>;

        check_for_strong_equiv(mockturtle::aig_network{}, gate_lyt{});
        check_for_strong_equiv(mockturtle::mig_network{}, gate_lyt{});

        check_for_strong_equiv(blueprints::and_or_network<mockturtle::aig_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::mig_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
        check_for_strong_equiv(blueprints::and_or_network<technology_network>(),
                               blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
    }
    SECTION("Hexagonal layout")
    {
        using gate_layout = gate_level_layout<hexagonal_layout>;

        check_for_strong_equiv(mockturtle::aig_network{}, gate_layout{arrangement::EVEN_ROW});
        check_for_strong_equiv(mockturtle::mig_network{}, gate_layout{arrangement::EVEN_ROW});

        check_for_strong_equiv(blueprints::and_or_network<mockturtle::aig_network>(),
                               blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_COLUMN));
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::mig_network>(),
                               blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::ODD_COLUMN));
        check_for_strong_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                               blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
        check_for_strong_equiv(blueprints::and_or_network<technology_network>(),
                               blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::ODD_ROW));
    }
}

TEST_CASE("Layout-layout equivalence", "[equiv]")
{
    SECTION("TP == 1/1")
    {
        check_for_strong_equiv(cart_gate_clk_lyt{}, cart_gate_clk_lyt{});
        check_for_strong_equiv(cart_gate_clk_lyt{}, hex_gate_clk_lyt{arrangement::EVEN_COLUMN});
        check_for_strong_equiv(cart_gate_clk_lyt{}, hex_gate_clk_lyt{arrangement::ODD_COLUMN});
        check_for_strong_equiv(cart_gate_clk_lyt{}, hex_gate_clk_lyt{arrangement::EVEN_ROW});
        check_for_strong_equiv(cart_gate_clk_lyt{}, hex_gate_clk_lyt{arrangement::ODD_ROW});

        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_COLUMN));
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_gate_clk_lyt>(arrangement::ODD_COLUMN));
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
        check_for_strong_equiv(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>(),
                               blueprints::xor_maj_gate_layout<hex_gate_clk_lyt>(arrangement::ODD_ROW));
    }
    SECTION("TP == 1/2")
    {
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::EVEN_COLUMN));
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::ODD_COLUMN));
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
        check_for_strong_equiv(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>(),
                               blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::ODD_ROW));
    }
}

TEST_CASE("Weak equivalence", "[equiv]")
{
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
                         blueprints::unbalanced_and_layout<cart_gate_clk_lyt>());
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::mig_network>(),
                         blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::EVEN_COLUMN));
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::xag_network>(),
                         blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::ODD_COLUMN));
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<mockturtle::klut_network>(),
                         blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
    check_for_weak_equiv(blueprints::one_to_five_path_difference_network<technology_network>(),
                         blueprints::unbalanced_and_layout<hex_gate_clk_lyt>(arrangement::ODD_ROW));
}

TEST_CASE("No equivalence", "[equiv]")
{
    check_for_no_equiv(blueprints::and_or_network<mockturtle::xag_network>(),
                       blueprints::half_adder_network<technology_network>());
    check_for_no_equiv(blueprints::full_adder_network<mockturtle::aig_network>(),
                       blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>());
    check_for_no_equiv(blueprints::half_adder_network<mockturtle::mig_network>(),
                       blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
    check_for_no_equiv(blueprints::and_not_gate_layout<hex_gate_clk_lyt>(arrangement::ODD_ROW),
                       blueprints::and_or_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_COLUMN));
}

TEST_CASE("Physical equivalence matches named primary interfaces", "[equiv][placed-objects]")
{
    mockturtle::names_view<mockturtle::klut_network> left{}, right{};
    const auto                                       a = left.create_pi("a");
    const auto                                       b = left.create_pi("b");
    left.create_po(left.create_lt(a, b), "compare");
    left.create_po(a, "pass");
    const auto rb = right.create_pi("b");
    const auto ra = right.create_pi("a");
    right.create_po(ra, "pass");
    right.create_po(right.create_lt(ra, rb), "compare");
    CHECK(equivalence_checking(left, right) == eq_type::STRONG);
    right.create_pi("extra");
    CHECK(equivalence_checking(left, right) == eq_type::NO);
}

TEST_CASE("Physical equivalence extracts placed objects and rejects physical defects", "[equiv][placed-objects]")
{
    mockturtle::names_view<mockturtle::klut_network> spec{};
    const auto                                       a = spec.create_pi("a");
    spec.create_po(a, "result");
    gate_level_layout<cartesian_layout> lyt{{1, 2}, clocking::twoddwave()};
    const auto                          pi = lyt.create_pi("a", {0, 0});
    const auto                          po = lyt.create_po(pi, "result", {0, 1});
    CHECK(equivalence_checking(spec, lyt) == eq_type::STRONG);
    lyt.move_node(po, {0, 10});
    CHECK(equivalence_checking(spec, lyt) == eq_type::NO);
    lyt.disconnect({po, 0});
    CHECK(equivalence_checking(spec, lyt) == eq_type::NO);
}

TEST_CASE("Physical equivalence retains wire throughput", "[equiv][placed-objects]")
{
    mockturtle::names_view<mockturtle::klut_network> spec{};
    const auto                                       a = spec.create_pi("a");
    const auto                                       b = spec.create_pi("b");
    spec.create_po(spec.create_and(a, b), "result");
    gate_level_layout<cartesian_layout> lyt{{6, 2}, clocking::twoddwave()};
    auto                                path       = lyt.create_pi("a", {0, 0});
    const auto                          short_path = lyt.create_pi("b", {3, 1});
    for (int64_t x = 1; x <= 4; ++x)
    {
        path = lyt.create_buf(path, {x, 0});
    }
    const auto gate = lyt.create_and(path, short_path, {4, 1});
    lyt.create_po(gate, "result", {5, 1});
    equivalence_checking_stats stats{};
    CHECK(equivalence_checking(spec, lyt, &stats) == eq_type::WEAK);
    CHECK(stats.tp_impl == 2);
    CHECK(stats.tp_diff == 1);
}

TEST_CASE("Physical equivalence returns NO for a clocked required cycle", "[equiv][placed-objects]")
{
    mockturtle::names_view<mockturtle::klut_network> spec{};
    spec.create_po(spec.create_pi("a"), "result");
    gate_level_layout<cartesian_layout> lyt{{3, 3}};
    const auto                          pi = lyt.create_pi("a", {0, 1});
    kitty::dynamic_truth_table          function{2};
    kitty::create_from_hex_string(function, "8");
    const auto gate   = lyt.create_node({pi}, function, {1, 1});
    const auto first  = lyt.create_buf(gate, {2, 1});
    const auto second = lyt.create_buf(first, {2, 2});
    const auto third  = lyt.create_buf(second, {1, 2});
    lyt.connect(third, {gate, 1});
    lyt.create_po(third, "result", {0, 2});
    lyt.assign_clock_number({0, 1}, 3);
    lyt.assign_clock_number({1, 1}, 0);
    lyt.assign_clock_number({2, 1}, 1);
    lyt.assign_clock_number({2, 2}, 2);
    lyt.assign_clock_number({1, 2}, 3);
    lyt.assign_clock_number({0, 2}, 0);
    equivalence_checking_stats stats{};
    CHECK(equivalence_checking(spec, lyt, &stats) == eq_type::NO);
    CHECK(stats.impl_drv_stats.drvs == 0);
    CHECK(stats.eq == eq_type::NO);
}
