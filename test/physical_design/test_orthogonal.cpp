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

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <optional>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;
using namespace fiction::qca;
using namespace fiction::synthesis;

TEST_CASE("East-south coloring", "[orthogonal]")
{
    const auto check = [](const auto& ntk)
    {
        auto container = physical_design::detail::east_south_edge_coloring(ntk);
        CHECK(physical_design::detail::is_east_south_colored(container.color_ntk));
    };

    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::se_coloring_corner_case_network<technology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<technology_network>(
        blueprints::fanout_substitution_corner_case_network<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::nary_operation_network<technology_network>())});
    check(mockturtle::fanout_view{fanout_substitution<technology_network>(blueprints::clpl<technology_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::half_adder_network<mockturtle::mig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::full_adder_network<mockturtle::mig_network>())});
}

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

    auto layout = orthogonal<Lyt>(ntk, ps, &stats);

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

TEST_CASE("Layout equivalence", "[algorithms]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        check_ortho_equiv_all<gate_layout>();
    }
    SECTION("Hexagonal layouts")
    {
        using gate_layout = gate_level_layout<hexagonal_layout>;

        const auto a =
            GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

        check_ortho_equiv_all<gate_layout>(a);
    }
}

TEST_CASE("Gate library application", "[orthogonal]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    const auto check = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal<gate_layout>(ntk, {}, &stats);

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

TEST_CASE("Name conservation after orthogonal physical design", "[orthogonal]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("maj");

    const auto layout = orthogonal<gate_layout>(maj);

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}

TEST_CASE("Orthogonal physical design reports progress", "[orthogonal]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    const auto ntk = blueprints::mux21_network<technology_network>();

    progress_recorder                 rec{};
    orthogonal_physical_design_params params{};
    params.on_progress = rec.callback();

    const auto layout = orthogonal<gate_layout>(ntk, params);

    check_eq(ntk, layout);

    CHECK(rec.is_consistent("placing gates"));
    CHECK(rec.final_count("placing gates") > 0);
}

TEST_CASE("Orthogonal physical design requires an arrangement for hexagonal layouts", "[orthogonal]")
{
    using gate_layout = gate_level_layout<hexagonal_layout>;

    CHECK_THROWS_AS(orthogonal<gate_layout>(blueprints::and_or_network<technology_network>()), std::invalid_argument);
}

TEST_CASE("Orthogonal physical design of a network without primary inputs", "[orthogonal]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    technology_network ntk{};
    ntk.create_po(ntk.get_constant(false));

    CHECK_NOTHROW(orthogonal<gate_layout>(ntk));
}
