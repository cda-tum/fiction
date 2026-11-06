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
 * @brief Tests for `fiction/synthesis/network_conversion.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <array>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;

/**
 * @brief Checks whether a source can convert to the named technology network.
 * @tparam Ntk Source network type.
 */
template <typename Ntk>
concept technology_convertible = requires(const Ntk& ntk) { convert_network<named_technology_network>(ntk); };

static_assert(technology_convertible<mockturtle::aig_network>);
static_assert(!technology_convertible<int>);

/**
 * @brief Compares every output truth table for networks with the same inputs.
 * @tparam Spec Specification network type.
 * @tparam Impl Implementation network type.
 * @param spec Specification network.
 * @param impl Implementation network.
 */
template <typename Spec, typename Impl>
void check_simulated_eq(const Spec& spec, const Impl& impl)
{
    REQUIRE(spec.num_pis() == impl.num_pis());
    const mockturtle::default_simulator<kitty::dynamic_truth_table> simulator{spec.num_pis()};
    const auto expected = mockturtle::simulate<kitty::dynamic_truth_table>(spec, simulator);
    const auto actual   = mockturtle::simulate<kitty::dynamic_truth_table>(impl, simulator);
    REQUIRE(actual.size() == expected.size());
    for (auto i = 0u; i < actual.size(); ++i)
    {
        CAPTURE(i);
        CHECK(actual[i] == expected[i]);
    }
}

/**
 * @brief Checks Boolean behavior in every supported destination type.
 * @tparam Ntk Source network type.
 * @param ntk Source network.
 */
template <typename Ntk>
void to_x(const Ntk& ntk)
{
    {
        INFO("MIG destination");
        const auto converted_mig = convert_network<mockturtle::mig_network>(ntk);

        check_eq(ntk, converted_mig);
        check_simulated_eq(ntk, converted_mig);
    }

    {
        INFO("AIG destination");
        const auto converted_aig = convert_network<mockturtle::aig_network>(ntk);

        check_eq(ntk, converted_aig);
        check_simulated_eq(ntk, converted_aig);
    }

    {
        INFO("XAG destination");
        const auto converted_xag = convert_network<mockturtle::xag_network>(ntk);

        check_eq(ntk, converted_xag);
        check_simulated_eq(ntk, converted_xag);
    }

    {
        INFO("TEC destination");
        const auto converted_tec = convert_network<technology_network>(ntk);

        check_eq(ntk, converted_tec);
        check_simulated_eq(ntk, converted_tec);
    }
}

TEMPLATE_TEST_CASE("Simple network conversion", "[network-conversion]", mockturtle::mig_network,
                   mockturtle::aig_network, mockturtle::xag_network, technology_network)
{
    to_x(blueprints::maj1_network<TestType>());
}

TEST_CASE("Name conservation after network conversion", "[network-conversion]")
{
    auto maj = blueprints::maj1_network<named_mig_network>();
    maj.set_network_name("maj");

    const auto converted_maj = convert_network<named_technology_network>(maj);

    // network name
    CHECK(converted_maj.get_network_name() == "maj");

    // PI names
    REQUIRE(converted_maj.num_pis() == 3u);
    CHECK(converted_maj.get_name(converted_maj.make_signal(converted_maj.pi_at(0))) == "a");
    CHECK(converted_maj.get_name(converted_maj.make_signal(converted_maj.pi_at(1))) == "b");
    CHECK(converted_maj.get_name(converted_maj.make_signal(converted_maj.pi_at(2))) == "c");

    // PO names
    REQUIRE(converted_maj.num_pos() == 1u);
    CHECK(converted_maj.get_output_name(0) == "f");
}

// technology_network uses explicit inverter nodes instead of complemented signals.
TEMPLATE_TEST_CASE("Constant inverted signal recovery conversion", "[network-conversion]", mockturtle::mig_network,
                   mockturtle::aig_network, mockturtle::xag_network)
{
    to_x(blueprints::constant_gate_input_maj_network<TestType>());
}

TEST_CASE("Technology network inverters and buffers survive conversion", "[network-conversion]")
{
    // a multiplexer whose select signal is inverted by an inverter node and whose output passes a buffer,
    // as the readers build technology networks
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();
    const auto s = tec.create_pi();

    const auto not_s = tec.create_not(s);
    const auto mux   = tec.create_or(tec.create_and(a, not_s), tec.create_and(b, s));

    tec.create_po(tec.create_buf(mux));

    to_x(tec);
}

TEMPLATE_TEST_CASE("Complex network conversion", "[network-conversion]", mockturtle::mig_network,
                   mockturtle::aig_network, mockturtle::xag_network, technology_network)
{
    to_x(blueprints::maj4_network<TestType>());
    to_x(blueprints::nary_operation_network<TestType>());
}

TEST_CASE("Multiple conversions preserve network size and logic", "[network-conversion]")
{
    const auto tec = blueprints::se_coloring_corner_case_network<technology_network>();

    const auto converted = convert_network<technology_network>(
        convert_network<technology_network>(convert_network<technology_network>(tec)));

    CHECK(tec.size() == converted.size());
    check_simulated_eq(tec, converted);

    const auto aig           = blueprints::maj4_network<mockturtle::aig_network>();
    const auto converted_aig = convert_network<technology_network>(aig);

    const auto converted_converted_aig = convert_network<technology_network>(
        convert_network<technology_network>(convert_network<technology_network>(converted_aig)));

    CHECK(converted_aig.size() == converted_converted_aig.size());
    check_simulated_eq(converted_aig, converted_converted_aig);
    check_simulated_eq(aig, converted_converted_aig);
}

TEST_CASE("Conversion preserves size and logic after fanout substitution", "[network-conversion]")
{
    const auto aig                       = blueprints::maj4_network<mockturtle::aig_network>();
    const auto substituted_aig           = fanout_substitution<technology_network>(aig);
    const auto converted_substituted_aig = convert_network<technology_network>(substituted_aig);
    CHECK(substituted_aig.size() == converted_substituted_aig.size());
    check_simulated_eq(substituted_aig, converted_substituted_aig);
    check_simulated_eq(aig, converted_substituted_aig);

    const auto tec                       = blueprints::fanout_substitution_corner_case_network<technology_network>();
    const auto substituted_tec           = fanout_substitution<technology_network>(tec);
    const auto converted_substituted_tec = convert_network<technology_network>(substituted_tec);
    CHECK(substituted_tec.size() == converted_substituted_tec.size());
    check_simulated_eq(substituted_tec, converted_substituted_tec);
    check_simulated_eq(tec, converted_substituted_tec);
}

TEST_CASE("Conversion preserves size and logic after balancing", "[network-conversion]")
{
    const auto aig                    = blueprints::maj4_network<mockturtle::aig_network>();
    const auto balanced_aig           = network_balancing<technology_network>(aig);
    const auto converted_balanced_aig = convert_network<technology_network>(balanced_aig);
    CHECK(balanced_aig.size() == converted_balanced_aig.size());
    check_simulated_eq(balanced_aig, converted_balanced_aig);
    check_simulated_eq(aig, converted_balanced_aig);

    const auto tec                    = blueprints::fanout_substitution_corner_case_network<technology_network>();
    const auto balanced_tec           = network_balancing<technology_network>(tec);
    const auto converted_balanced_tec = convert_network<technology_network>(balanced_tec);
    CHECK(balanced_tec.size() == converted_balanced_tec.size());
    check_simulated_eq(balanced_tec, converted_balanced_tec);
    check_simulated_eq(tec, converted_balanced_tec);
}

TEST_CASE("Convert every two- and three-input function without losing the interface", "[network-conversion]")
{
    for (const auto variables : {2u, 3u})
    {
        for (auto function = 0u; function < (1u << (1u << variables)); ++function)
        {
            CAPTURE(variables, function);
            named_technology_network                source{};
            std::vector<technology_network::signal> inputs{};
            inputs.reserve(variables);
            for (auto index = 0u; index < variables; ++index)
            {
                inputs.push_back(source.create_pi(std::string(1, static_cast<char>('a' + index))));
            }
            kitty::dynamic_truth_table expected{variables};
            const std::array           words{function};
            kitty::create_from_words(expected, words.begin(), words.end());
            const auto output = source.create_node(inputs, expected);
            source.create_po(output, "f");
            source.create_po(source.create_not(output), "g");
            source.create_po(source.get_constant(true), "one");
            const auto verify = [&](const auto& converted, const char* destination)
            {
                CAPTURE(destination);
                CHECK(converted.num_pis() == variables);
                REQUIRE(converted.num_pos() == 3u);
                CHECK(converted.get_output_name(0) == "f");
                CHECK(converted.get_output_name(1) == "g");
                CHECK(converted.get_output_name(2) == "one");
                const auto actual = mockturtle::simulate<kitty::dynamic_truth_table>(
                    converted, mockturtle::default_simulator<kitty::dynamic_truth_table>{variables});
                REQUIRE(actual.size() == 3u);
                CHECK(actual[0] == expected);
                CHECK(actual[1] == ~expected);
                CHECK(kitty::is_const0(~actual[2]));
            };
            verify(convert_network<named_technology_network>(source), "TEC");
            verify(convert_network<named_aig_network>(source), "AIG");
            verify(convert_network<named_xag_network>(source), "XAG");
            verify(convert_network<named_mig_network>(source), "MIG");
        }
    }
}
