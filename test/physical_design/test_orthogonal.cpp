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
 * @brief Tests for `fiction/physical_design/orthogonal.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <concepts>
#include <optional>
#include <sstream>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;
using namespace fiction::qca;
using namespace fiction::synthesis;
using namespace fiction::verification;

static_assert(std::same_as<decltype(&orthogonal<cartesian_gate_layout>),
                           cartesian_gate_layout (*)(const tec_nt&, orthogonal_physical_design_params,
                                                     orthogonal_physical_design_stats*)>);

void check_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

template <typename Lyt, typename Ntk>
void check_ortho_equiv(const Ntk& ntk, const std::optional<arrangement> a = std::nullopt)
{
    orthogonal_physical_design_stats  stats{};
    orthogonal_physical_design_params ps{};
    ps.layout_arrangement = a;

    auto layout = orthogonal<Lyt>(convert_network<tec_nt>(ntk), ps, &stats);

    check_stats(stats);
    check_eq(ntk, layout);
}

template <typename Lyt>
void check_ortho_equiv_all(const std::optional<arrangement> a = std::nullopt)
{
    check_ortho_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::nary_operation_network<technology_network>(), a);
    check_ortho_equiv<Lyt>(blueprints::clpl<technology_network>(), a);

    // constant input network
    check_ortho_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>(), a);

    // multi-output network
    check_ortho_equiv<Lyt>(blueprints::multi_output_network<technology_network>(), a);
}

TEST_CASE("East-south coloring", "[orthogonal]")
{
    const auto check = [](const auto& ntk)
    {
        auto container = physical_design::detail::east_south_edge_coloring(mockturtle::topo_view{ntk});
        CHECK(physical_design::detail::is_east_south_colored(container.color_ntk));
    };

    check(mockturtle::fanout_view{
        fanout_substitution<tec_nt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{fanout_substitution<tec_nt>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{fanout_substitution<tec_nt>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<tec_nt>(blueprints::se_coloring_corner_case_network<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<tec_nt>(blueprints::fanout_substitution_corner_case_network<technology_network>())});
    check(
        mockturtle::fanout_view{fanout_substitution<tec_nt>(blueprints::nary_operation_network<technology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<tec_nt>(blueprints::clpl<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<tec_nt>(blueprints::half_adder_network<mockturtle::mig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<tec_nt>(blueprints::full_adder_network<mockturtle::mig_network>())});
}

TEST_CASE("Orthogonal physical design of a network without primary inputs", "[orthogonal]")
{
    tec_nt ntk{};
    ntk.create_po(ntk.get_constant(false));

    const auto lyt = orthogonal<cartesian_gate_layout>(ntk);
    CHECK(lyt.num_pis() == 0);
    CHECK(lyt.num_pos() == 1);
    check_eq(ntk, extract_layout_network(lyt));
}

TEST_CASE("Layout equivalence", "[orthogonal]")
{
    SECTION("Cartesian layouts")
    {
        check_ortho_equiv_all<cartesian_gate_layout>();
    }
    SECTION("Hexagonal layouts")
    {
        const auto a =
            GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

        check_ortho_equiv_all<hexagonal_gate_layout>(a);
    }
}

TEST_CASE("Repeated gate inputs have valid physical routing", "[orthogonal]")
{
    tec_nt     ntk{};
    const auto a        = ntk.create_pi();
    const auto function = GENERATE(0, 1, 2);
    /** @brief Gate with repeated input ports for the selected Boolean function. */
    const auto output = [&ntk, a, function]()
    {
        if (function == 0)
        {
            return ntk.create_and(a, a);
        }
        if (function == 1)
        {
            return ntk.create_or(a, a);
        }
        return ntk.create_xor(a, a);
    }();
    if (GENERATE(false, true))
    {
        ntk.create_po(a);
    }
    ntk.create_po(output);

    const auto layout = orthogonal<gate_layout<cartesian_layout>>(ntk);
    check_eq(ntk, layout);
    std::ostringstream    report{};
    gate_level_drv_params params{};
    params.out = &report;
    gate_level_drv_stats stats{};
    gate_level_drvs(layout, params, &stats);
    CHECK(stats.drvs == 0);
}

TEST_CASE("Orthogonal placement preserves noncommutative input order", "[orthogonal]")
{
    tec_nt     ntk{};
    const auto a      = ntk.create_pi();
    const auto b      = ntk.create_pi();
    const auto unused = ntk.create_pi();
    ntk.create_po(ntk.create_lt(b, a));
    const auto lyt = orthogonal<cartesian_gate_layout>(ntk);
    CHECK(lyt.num_pis() == 3);
    CHECK(lyt.is_pi(lyt.pi_at(2)));
    const auto extracted = extract_layout_network(lyt);
    CHECK(extracted.num_pis() == 3);
    check_eq(ntk, extract_layout_network(lyt));
    CHECK(ntk.is_pi(ntk.get_node(unused)));
}

TEST_CASE("Name conservation after orthogonal physical design", "[orthogonal]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("maj");

    const auto layout = orthogonal<cartesian_gate_layout>(convert_network<tec_nt>(maj));

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}

TEST_CASE("Orthogonal physical design requires an arrangement for hexagonal layouts", "[orthogonal]")
{
    CHECK_THROWS_AS(orthogonal<hexagonal_gate_layout>(blueprints::and_or_network<technology_network>()),
                    std::invalid_argument);
}

TEST_CASE("Gate library application", "[orthogonal]")
{
    const auto check = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal<cartesian_gate_layout>(convert_network<tec_nt>(ntk), {}, &stats);

        CHECK_NOTHROW(apply_gate_library<qca_one_library>(layout));
    };

    check(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check(blueprints::maj1_network<mockturtle::aig_network>());
    check(blueprints::maj4_network<mockturtle::aig_network>());
    check(blueprints::se_coloring_corner_case_network<technology_network>());
    check(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check(blueprints::clpl<technology_network>());
    check(blueprints::half_adder_network<mockturtle::mig_network>());

    // constant input network
    check(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Orthogonal physical design reports progress", "[orthogonal]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    progress_recorder                 rec{};
    orthogonal_physical_design_params params{};
    params.on_progress = rec.callback();

    const auto layout = orthogonal<cartesian_gate_layout>(ntk, params);

    check_eq(ntk, layout);

    CHECK(rec.is_consistent("placing gates"));
    CHECK(rec.final_count("placing gates") > 0);
}

TEMPLATE_TEST_CASE("Named specification conversion for orthogonal", "[physical-network]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network)
{
    mockturtle::names_view<TestType> source{};
    source.set_network_name("named_spec");
    const auto a = source.create_pi("left");
    const auto b = source.create_pi("right");
    source.create_po(source.create_and(a, b), "result");
    const auto ntk = fiction::synthesis::convert_network<tec_nt>(source);
    CHECK(ntk.get_network_name() == "named_spec");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(0))) == "left");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(1))) == "right");
    CHECK(ntk.get_output_name(0) == "result");
    const auto lyt = orthogonal<cartesian_gate_layout>(ntk);
    CHECK(lyt.get_layout_name() == "named_spec");
    CHECK(lyt.get_name(lyt.pi_at(0)) == "left");
    CHECK(lyt.get_name(lyt.pi_at(1)) == "right");
    CHECK(lyt.get_output_name(0) == "result");
    const auto extracted = extract_layout_network(lyt);
    CHECK(extracted.get_network_name() == "named_spec");
    CHECK(extracted.get_name(extracted.make_signal(extracted.pi_at(0))) == "left");
    CHECK(extracted.get_name(extracted.make_signal(extracted.pi_at(1))) == "right");
    CHECK(extracted.get_output_name(0) == "result");
    check_eq(source, extracted);
    const auto roundtrip = orthogonal<cartesian_gate_layout>(convert_network<tec_nt>(extracted));
    CHECK(roundtrip.get_layout_name() == "named_spec");
    CHECK(roundtrip.get_name(roundtrip.pi_at(0)) == "left");
    CHECK(roundtrip.get_name(roundtrip.pi_at(1)) == "right");
    CHECK(roundtrip.get_output_name(0) == "result");
    check_eq(extracted, roundtrip);
}
