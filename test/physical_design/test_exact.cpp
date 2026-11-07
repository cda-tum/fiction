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
 * @brief Tests for `fiction/physical_design/exact.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>

#if (FICTION_Z3_SOLVER)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/network_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/exact.hpp>
#include <fiction/physical_design/surface_analysis.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/critical_path_length_and_throughput.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <concepts>
#include <cstdint>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::inml;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;
using namespace fiction::qca;
using namespace fiction::sidb;
using namespace fiction::synthesis;
using namespace fiction::verification;

static_assert(std::same_as<decltype(&exact<cartesian_layout>),
                           std::optional<cartesian_gate_layout> (*)(const named_technology_network&,
                                                                    const exact_physical_design_params&,
                                                                    exact_physical_design_stats*)>);
static_assert(std::same_as<decltype(&exact_with_blacklist<cartesian_layout>),
                           std::optional<cartesian_gate_layout> (*)(
                               const named_technology_network&, const surface_black_list<port_direction>&,
                               exact_physical_design_params, exact_physical_design_stats*)>);

namespace
{

/**
 * @brief Check statistics against the resulting layout.
 * @tparam Lyt Gate-level layout type.
 * @param st Reported physical-design statistics.
 * @param lyt Resulting layout.
 */
template <typename Lyt>
void check_stats(const exact_physical_design_stats& st, const Lyt& lyt)
{
    CHECK(st.time_total >= decltype(st.time_total)::zero());
    CHECK(st.x_size == lyt.width());
    CHECK(st.y_size == lyt.height());
    CHECK(st.num_gates == lyt.num_gates());
    CHECK(st.num_wires == lyt.num_wires());
    CHECK(st.num_crossings == lyt.num_crossings());
}

template <typename Lyt>
void check_tp(const Lyt& lyt, const uint64_t tp)
{
    const auto cp_tp = critical_path_length_and_throughput(lyt);

    CHECK(cp_tp.throughput >= tp);  // >= because Z3 might behave differently on different operating systems
}

template <typename Lyt>
void check_drvs(const Lyt& lyt)
{
    std::stringstream           ss{};
    const gate_level_drv_params ps{.out = &ss};
    gate_level_drv_stats        st{};
    gate_level_drvs(lyt, ps, &st);

    REQUIRE(st.drvs == 0);
}

template <typename Lyt, typename Ntk>
Lyt generate_layout(const Ntk& ntk, exact_physical_design_params ps, const std::optional<arrangement> a = std::nullopt)
{
    exact_physical_design_stats stats{};

    ps.layout_arrangement = a;

    const auto layout =
        exact<typename Lyt::coordinate_layout>(convert_network<named_technology_network>(ntk), ps, &stats);

    REQUIRE(layout.has_value());

    check_drvs(layout.value());          // NOLINT(bugprone-unchecked-optional-access)
    check_stats(stats, layout.value());  // NOLINT(bugprone-unchecked-optional-access): REQUIRE guards the value.

    return layout.value();  // NOLINT(bugprone-unchecked-optional-access)
}
template <typename Lyt, typename Ntk>
Lyt generate_layout_with_black_list(const Ntk& ntk, const surface_black_list<port_direction>& black_list,
                                    const exact_physical_design_params& ps)
{
    exact_physical_design_stats stats{};

    const auto layout = exact_with_blacklist<typename Lyt::coordinate_layout>(
        convert_network<named_technology_network>(ntk), black_list, ps, &stats);

    REQUIRE(layout.has_value());

    check_drvs(layout.value());          // NOLINT(bugprone-unchecked-optional-access)
    check_stats(stats, layout.value());  // NOLINT(bugprone-unchecked-optional-access): REQUIRE guards the value.

    return layout.value();  // NOLINT(bugprone-unchecked-optional-access)
}

template <typename Ntk, typename Lyt>
void check_io_names(const Ntk& ntk, const Lyt& lyt)
{
    if constexpr (mockturtle::has_get_name_v<Ntk>)
    {
        for (auto i = 0u; i < ntk.num_pis(); ++i)
        {
            CHECK_THAT(lyt.get_input_name(i), Catch::Matchers::Equals(ntk.get_name(ntk.make_signal(ntk.pi_at(i)))));
        }
    }
    if constexpr (mockturtle::has_get_output_name_v<Ntk>)
    {
        for (auto i = 0u; i < ntk.num_pos(); ++i)
        {
            CHECK_THAT(lyt.get_output_name(i), Catch::Matchers::Equals(ntk.get_output_name(i)));
        }
    }
}

/**
 * @brief Checks the physical layout, logic, gate library, and terminal names.
 * @tparam Lib Gate library type.
 * @tparam GateLyt Gate-level layout type.
 * @tparam Ntk Network type.
 * @param ntk Network to place and route.
 * @param ps Physical-design parameters.
 * @param a Layout arrangement.
 */
template <typename Lib, typename GateLyt, typename Ntk>
void check_with_gate_library(const Ntk& ntk, const exact_physical_design_params& ps,
                             const std::optional<arrangement> a = std::nullopt)
{
    const auto layout = generate_layout<GateLyt>(ntk, ps, a);

    check_eq(ntk, layout);
    check_tp(layout, 1);
    CHECK_NOTHROW(apply_gate_library<Lib>(layout));
    check_io_names(ntk, layout);
}

template <typename Lyt, typename Ntk>
void check_without_gate_library(const Ntk& ntk, const exact_physical_design_params& ps,
                                const std::optional<arrangement> a = std::nullopt)
{
    const auto layout = generate_layout<Lyt>(ntk, ps, a);

    check_eq(ntk, layout);
    check_tp(layout, 1);
}

/**
 * @brief Checks whether each inverter has opposite incoming and outgoing signals.
 * @tparam Lyt Native gate-level layout type.
 * @param lyt Layout to inspect.
 * @return Whether every inverter is straight.
 */
template <typename Lyt>
bool has_straight_inverters(const Lyt& lyt)
{
    bool only_straight_inverters = true;
    lyt.foreach_gate(
        [&lyt, &only_straight_inverters](const auto& g)
        {
            if (lyt.is_inv(g) && !lyt.has_opposite_incoming_and_outgoing_signals(lyt.get_tile(g)))
            {
                only_straight_inverters = false;
                return false;  // break loop
            }

            return true;  // continue
        });

    return only_straight_inverters;
}

}  // namespace

TEST_CASE("Straight inverter validation", "[exact]")
{
    cartesian_gate_layout lyt{cartesian_gate_layout::extent{3, 2, 1}, clocking::twoddwave()};
    const auto            input    = lyt.create_pi("a", {0, 0});
    const auto            inverter = lyt.create_not(input, {1, 0});
    const auto            output   = lyt.create_po(inverter, "f", {2, 0});
    CHECK(has_straight_inverters(lyt));
    lyt.move_object(output, {1, 1});
    CHECK_FALSE(has_straight_inverters(lyt));
}

TEST_CASE("Exact Cartesian physical design", "[exact]")
{
    SECTION("Open clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "Open", .crossings = true});
    }
    SECTION("2DDWave clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "2DDWave", .crossings = true});
    }
    SECTION("USE clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "USE", .crossings = true});
    }
    SECTION("RES clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "RES", .crossings = true});
    }
    SECTION("ESR clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "ESR", .crossings = true});
    }
    SECTION("CFE clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "CFE", .crossings = true});
    }
    SECTION("SRS clocking")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "SRS", .crossings = true});
    }
    SECTION("Border I/O")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::and_or_network<mockturtle::mig_network>(),
            exact_physical_design_params{.scheme = "2DDWave", .crossings = true, .border_io = true});
    }
    SECTION("Planar")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
            exact_physical_design_params{.scheme = "2DDWave"});
    }
    SECTION("Straight inverters")
    {
        CHECK(has_straight_inverters(generate_layout<cartesian_gate_layout>(
            blueprints::inverter_network<technology_network>(),
            exact_physical_design_params{.scheme = "USE", .straight_inverters = true})));
        CHECK(has_straight_inverters(generate_layout<cartesian_gate_layout>(
            blueprints::inverter_network<technology_network>(),
            exact_physical_design_params{.scheme = "Open", .straight_inverters = true})));
    }
    SECTION("Global synchronization")
    {
        SECTION("enabled")
        {
            check_tp(generate_layout<cartesian_gate_layout>(
                         blueprints::one_to_five_path_difference_network<technology_network>(),
                         exact_physical_design_params{.scheme = "USE"}),
                     1);
        }
        SECTION("disabled")
        {
            check_tp(generate_layout<cartesian_gate_layout>(
                         blueprints::one_to_five_path_difference_network<technology_network>(),
                         exact_physical_design_params{.scheme = "USE", .desynchronize = true}),
                     2);
        }
    }
    SECTION("Blacklist gates & wires")
    {
        SECTION("Without port info")
        {
            surface_black_list<port_direction> black_list{};
            black_list[{2, 2}].insert({create_and_tt(), {}});
            black_list[{2, 2}].insert({create_id_tt(), {}});
            black_list[{1, 2}].insert({create_or_tt(), {}});
            black_list[{2, 0}].insert({create_id_tt(), {}});
            const auto lyt = generate_layout_with_black_list<cartesian_gate_layout>(
                blueprints::and_or_network<technology_network>(), black_list,
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true});

            check_eq(blueprints::and_or_network<technology_network>(), lyt);

            /** @brief Object at the position blacklisted for AND gates and wires. */
            const auto shared_object = lyt.find_object({2, 2});
            /** @brief Object at the position blacklisted for OR gates. */
            const auto or_object = lyt.find_object({1, 2});
            /** @brief Object at the second position blacklisted for wires. */
            const auto wire_object = lyt.find_object({2, 0});
            CHECK((!shared_object.has_value() || !lyt.is_and(*shared_object)));
            CHECK((!shared_object.has_value() || !lyt.is_wire(*shared_object)));
            CHECK((!or_object.has_value() || !lyt.is_or(*or_object)));
            CHECK((!wire_object.has_value() || !lyt.is_wire(*wire_object)));
        }
        SECTION("With port info")
        {
            const port_list<port_direction> gate_ports{
                {port_direction{port_direction::cardinal::NORTH}, port_direction{port_direction::cardinal::WEST}},
                {port_direction{port_direction::cardinal::SOUTH}}};
            const port_list<port_direction>    wire_ports{{port_direction{port_direction::cardinal::NORTH}},
                                                          {port_direction{port_direction::cardinal::SOUTH}}};
            surface_black_list<port_direction> black_list{};
            black_list[{2, 2}].insert({create_and_tt(), {gate_ports}});
            black_list[{2, 2}].insert({create_or_tt(), {gate_ports}});
            black_list[{2, 2}].insert({create_id_tt(), {wire_ports}});
            const auto lyt = generate_layout_with_black_list<cartesian_gate_layout>(
                blueprints::and_or_network<technology_network>(), black_list,
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true});

            check_eq(blueprints::and_or_network<technology_network>(), lyt);

            /** @brief Object whose type-specific port pattern is blacklisted. */
            const auto object = lyt.find_object({2, 2});
            CHECK((!object.has_value() || !lyt.is_and(*object) ||
                   !(lyt.has_northern_incoming_signal({2, 2}) && lyt.has_western_incoming_signal({2, 2}) &&
                     lyt.has_southern_outgoing_signal({2, 2}))));

            CHECK((!object.has_value() || !lyt.is_or(*object) ||
                   !(lyt.has_northern_incoming_signal({2, 2}) && lyt.has_western_incoming_signal({2, 2}) &&
                     lyt.has_southern_outgoing_signal({2, 2}))));

            CHECK((!object.has_value() || !lyt.is_wire(*object) ||
                   !(lyt.has_northern_incoming_signal({2, 2}) && lyt.has_southern_outgoing_signal({2, 2}))));
        }
    }
    SECTION("Synchronization elements")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
            exact_physical_design_params{.scheme = "USE", .border_io = true, .synchronization_elements = true});
    }
    SECTION("Minimize wires")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
            exact_physical_design_params{.scheme = "RES", .crossings = true, .minimize_wires = true});
    }
    SECTION("Minimize crossings")
    {
        check_with_gate_library<qca_one_library, cartesian_gate_layout>(
            blueprints::one_to_five_path_difference_network<mockturtle::aig_network>(),
            exact_physical_design_params{.scheme = "RES", .crossings = true, .minimize_crossings = true});
    }
}

TEST_CASE("Exact shifted Cartesian physical design", "[exact]")
{
    SECTION("odd col")
    {
        constexpr auto arr = arrangement::ODD_COLUMN;

        SECTION("Technology constraints: ToPoliNano")
        {
            check_with_gate_library<topolinano_library, shifted_cartesian_gate_layout>(
                blueprints::topolinano_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme               = "Columnar",
                                             .crossings            = true,
                                             .border_io            = true,
                                             .technology_specifics = technology_constraints::TOPOLINANO},
                arr);
        }
    }
}

TEST_CASE("Exact hexagonal physical design", "[exact]")
{
    SECTION("odd row")
    {
        constexpr auto arr = arrangement::ODD_ROW;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Open", .crossings = true}, arr);
        }
        SECTION("Row clocking")
        {
            check_with_gate_library<bestagon_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Row", .crossings = true, .border_io = true}, arr);

            check_with_gate_library<bestagon_library, hexagonal_gate_layout>(
                blueprints::nand_xnor_network<technology_network>(),
                exact_physical_design_params{.scheme = "Row", .crossings = true, .border_io = true}, arr);
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true}, arr);
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true, .border_io = true}, arr);
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
                exact_physical_design_params{.scheme = "2DDWave"}, arr);
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "USE", .straight_inverters = true}, arr)));
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "Open", .straight_inverters = true}, arr)));
        }
    }
    SECTION("even row")
    {
        constexpr auto arr = arrangement::EVEN_ROW;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Open", .crossings = true}, arr);
        }
        SECTION("Row clocking")
        {
            check_with_gate_library<bestagon_library, hexagonal_gate_layout>(
                blueprints::unbalanced_and_inv_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Row", .crossings = true, .border_io = true}, arr);
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true}, arr);
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true, .border_io = true}, arr);
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
                exact_physical_design_params{.scheme = "2DDWave"}, arr);
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "USE", .straight_inverters = true}, arr)));
        }
    }
    SECTION("odd column")
    {
        constexpr auto arr = arrangement::ODD_COLUMN;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Open", .crossings = true}, arr);
        }
        SECTION("Columnar clocking")
        {
            check_without_gate_library<hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Columnar", .crossings = true, .border_io = true}, arr);
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true}, arr);
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true, .border_io = true}, arr);
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
                exact_physical_design_params{.scheme = "2DDWave"}, arr);
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "USE", .straight_inverters = true}, arr)));
        }
    }
    SECTION("even column")
    {
        constexpr auto arr = arrangement::EVEN_COLUMN;

        SECTION("Open clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Open", .crossings = true}, arr);
        }
        SECTION("Columnar clocking")
        {
            check_without_gate_library<hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "Columnar", .crossings = true, .border_io = true}, arr);
        }
        SECTION("2DDWave clocking")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true}, arr);
        }
        SECTION("Border I/O")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::and_or_network<mockturtle::mig_network>(),
                exact_physical_design_params{.scheme = "2DDWave", .crossings = true, .border_io = true}, arr);
        }
        SECTION("Planar")
        {
            check_with_gate_library<qca_one_library, hexagonal_gate_layout>(
                blueprints::unbalanced_and_inv_network<mockturtle::aig_network>(),
                exact_physical_design_params{.scheme = "2DDWave"}, arr);
        }
        SECTION("Straight inverters")
        {
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "USE", .straight_inverters = true}, arr)));
            CHECK(has_straight_inverters(generate_layout<hexagonal_gate_layout>(
                blueprints::inverter_network<technology_network>(),
                exact_physical_design_params{.scheme = "Open", .straight_inverters = true}, arr)));
        }
    }
}

TEST_CASE("High degree input networks", "[exact]")
{
    CHECK_THROWS_AS(exact<cartesian_layout>(
                        convert_network<named_technology_network>(blueprints::maj1_network<mockturtle::mig_network>()),
                        exact_physical_design_params{.scheme = "2DDWave"}),
                    high_degree_fanin_exception);
    CHECK_THROWS_AS(exact<cartesian_layout>(
                        convert_network<named_technology_network>(blueprints::maj1_network<mockturtle::mig_network>()),
                        exact_physical_design_params{.scheme = "USE"}),
                    high_degree_fanin_exception);

    CHECK_NOTHROW(exact<cartesian_layout>(
        convert_network<named_technology_network>(blueprints::maj1_network<mockturtle::mig_network>()),
        exact_physical_design_params{.scheme = "RES"}));
}

TEST_CASE("Exact physical design with upper bounds", "[exact]")
{
    auto upper_bound_config = exact_physical_design_params{.scheme = "2DDWave", .crossings = true};

    SECTION("total area")
    {
        upper_bound_config.upper_bound_area = 5u;  // allow only 5 tiles total; this will fail (and is tested for)

        const auto half_adder = blueprints::half_adder_network<mockturtle::aig_network>();
        const auto layout =
            exact<cartesian_layout>(convert_network<named_technology_network>(half_adder), upper_bound_config);

        // since a half adder cannot be synthesized on just 5 tiles, layout should not have a value
        CHECK(!layout.has_value());
    }
    SECTION("individual bounds")
    {
        upper_bound_config.upper_bound_y = 3u;  // allow only 3 tiles in y direction; this will work

        const auto mux    = blueprints::mux21_network<technology_network>();
        auto       layout = exact<cartesian_layout>(mux, upper_bound_config);

        REQUIRE(layout.has_value());

        CHECK(layout->height() <= 4);  // NOLINT(bugprone-unchecked-optional-access): REQUIRE checks the value.

        upper_bound_config.upper_bound_x = 2u;  // additionally, allow only 2 tiles in x direction; this will now fail

        layout = exact<cartesian_layout>(mux, upper_bound_config);

        CHECK(!layout.has_value());
    }
}

TEST_CASE("Exact physical design timeout", "[exact]")
{
    auto timeout_config    = exact_physical_design_params{.scheme = "USE", .crossings = true};
    timeout_config.timeout = 1u;  // allow one millisecond to find a solution

    const auto half_adder = blueprints::half_adder_network<mockturtle::aig_network>();
    const auto layout = exact<cartesian_layout>(convert_network<named_technology_network>(half_adder), timeout_config);

    // The one-millisecond budget is insufficient for placing and routing the half adder.
    CHECK(!layout.has_value());
}

TEST_CASE("Name conservation after exact physical design", "[exact]")
{
    auto maj = blueprints::maj1_network<named_mig_network>();
    maj.set_network_name("maj");

    auto params        = exact_physical_design_params{.scheme = "RES"};
    params.num_threads = GENERATE(1u, 2u);
    CAPTURE(params.num_threads);
    const auto layout = exact<cartesian_layout>(convert_network<named_technology_network>(maj), params);

    REQUIRE(layout.has_value());

    /** @brief Placed layout whose names are checked. */
    const auto& result = *layout;  // NOLINT(bugprone-unchecked-optional-access): REQUIRE passed.

    // network name
    CHECK_THAT(result.get_layout_name(), Catch::Matchers::Equals("maj"));

    // PI names
    CHECK_THAT(result.get_name(result.pi_at(0)), Catch::Matchers::Equals("a"));
    CHECK_THAT(result.get_name(result.pi_at(1)), Catch::Matchers::Equals("b"));
    CHECK_THAT(result.get_name(result.pi_at(2)), Catch::Matchers::Equals("c"));

    // PO names
    CHECK_THAT(result.get_output_name(0), Catch::Matchers::Equals("f"));
}

TEST_CASE("Exact physical design reports progress", "[exact]")
{
    /** @brief Selects the ordinary or empty-blacklist entry for the same progress contract. */
    const bool with_blacklist = GENERATE(false, true);
    const auto ntk            = blueprints::and_or_network<technology_network>();

    auto params = exact_physical_design_params{.scheme = "2DDWave"};

    params.num_threads = GENERATE(1u, 2u);
    CAPTURE(with_blacklist, params.num_threads);

    // Repeated starts cover workers finding a layout while another worker initializes its context.
    for (unsigned trial = 0; trial < (params.num_threads > 1 ? 8u : 1u); ++trial)
    {
        CAPTURE(trial);
        progress_recorder rec{};
        params.on_progress = rec.callback();

        exact_physical_design_stats stats{};

        /** @brief Placement result from the selected exact entry point. */
        const auto layout = with_blacklist ? exact_with_blacklist<cartesian_layout>(ntk, {}, params, &stats) :
                                             exact<cartesian_layout>(ntk, params, &stats);

        CHECK(static_cast<bool>(params.on_progress));

        REQUIRE(layout.has_value());
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access): REQUIRE guards the access
        check_eq(ntk, *layout);

        // the number of aspect ratios is unknown in advance
        CHECK(rec.is_consistent("aspect ratios"));
        CHECK(rec.final_count("aspect ratios") == stats.num_aspect_ratios);
    }
}

TEST_CASE("Exact physical design requires an arrangement for shifted Cartesian and hexagonal layouts", "[exact]")
{
    const auto ntk = convert_network<named_technology_network>(blueprints::and_or_network<mockturtle::mig_network>());

    CHECK_THROWS_AS(exact<shifted_cartesian_layout>(ntk), std::invalid_argument);
    CHECK_THROWS_AS(exact<hexagonal_layout>(ntk), std::invalid_argument);
    CHECK_THROWS_AS(exact_with_blacklist<hexagonal_layout>(ntk, {}), std::invalid_argument);
}

TEMPLATE_TEST_CASE("Named specification conversion for exact", "[physical-network]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network)
{
    mockturtle::names_view<TestType> source{};
    source.set_network_name("named_spec");
    const auto a = source.create_pi("left");
    const auto b = source.create_pi("right");
    source.create_po(source.create_and(a, b), "result");
    const auto ntk = fiction::synthesis::convert_network<named_technology_network>(source);
    CHECK_THAT(ntk.get_network_name(), Catch::Matchers::Equals("named_spec"));
    CHECK_THAT(ntk.get_name(ntk.make_signal(ntk.pi_at(0))), Catch::Matchers::Equals("left"));
    CHECK_THAT(ntk.get_name(ntk.make_signal(ntk.pi_at(1))), Catch::Matchers::Equals("right"));
    CHECK_THAT(ntk.get_output_name(0), Catch::Matchers::Equals("result"));
    const auto result = exact<cartesian_layout>(ntk);
    REQUIRE(result.has_value());
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): REQUIRE guards the access
    const auto& lyt = result.value();
    CHECK_THAT(lyt.get_layout_name(), Catch::Matchers::Equals("named_spec"));
    CHECK_THAT(lyt.get_name(lyt.pi_at(0)), Catch::Matchers::Equals("left"));
    CHECK_THAT(lyt.get_name(lyt.pi_at(1)), Catch::Matchers::Equals("right"));
    CHECK_THAT(lyt.get_output_name(0), Catch::Matchers::Equals("result"));
    const auto extracted = extract_layout_network(lyt);
    CHECK_THAT(extracted.get_network_name(), Catch::Matchers::Equals("named_spec"));
    CHECK_THAT(extracted.get_name(extracted.make_signal(extracted.pi_at(0))), Catch::Matchers::Equals("left"));
    CHECK_THAT(extracted.get_name(extracted.make_signal(extracted.pi_at(1))), Catch::Matchers::Equals("right"));
    CHECK_THAT(extracted.get_output_name(0), Catch::Matchers::Equals("result"));
    check_eq(source, extracted);
}

#else  // FICTION_Z3_SOLVER

TEST_CASE("Exact physical design", "[exact]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_Z3_SOLVER
