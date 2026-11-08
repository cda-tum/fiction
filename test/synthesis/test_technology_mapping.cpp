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
 * @brief Tests for `fiction/synthesis/technology_mapping.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/technology_mapping.hpp>
#include <fiction/verification/count_gate_types.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/sequential.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/cut_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <array>
#include <cstdint>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;
using namespace fiction::verification;

namespace
{

/** @brief Sequential network whose optional combinational query is available only on lvalues. */
class register_count_network : public mockturtle::sequential<technology_network>
{
  public:
    /** @brief Reports whether this lvalue has no registers. */
    [[nodiscard]] bool is_combinational() const&
    {
        return mockturtle::sequential<technology_network>::is_combinational();
    }
    /** @brief Omits the rvalue query inspected by the optional-interface trait. */
    bool is_combinational() && = delete;
};

static_assert(!mockturtle::has_is_combinational_v<register_count_network>);
static_assert(mockturtle::has_num_registers_v<register_count_network>);

/**
 * @brief Checks mapping success, Boolean behavior, and the selected gate library.
 * @tparam Ntk Source network type.
 * @param ntk Source network.
 * @param params Selected gate library.
 * @return Counts of mapped gate types.
 */
template <typename Ntk>
count_gate_types_stats map_and_check(const Ntk& ntk, const technology_mapping_params& params)
{
    technology_mapping_stats stats{};
    const auto               mapped = technology_mapping(ntk, params, &stats);
    REQUIRE_FALSE(stats.mapper_stats.mapping_error);
    check_eq(ntk, mapped);

    count_gate_types_stats gates{};
    count_gate_types(mapped, &gates);
    CHECK((params.inv || gates.num_inv == 0));
    CHECK((params.and2 || gates.num_and2 == 0));
    CHECK((params.or2 || gates.num_or2 == 0));
    CHECK((params.nand2 || gates.num_nand2 == 0));
    CHECK((params.nor2 || gates.num_nor2 == 0));
    CHECK((params.xor2 || gates.num_xor2 == 0));
    CHECK((params.xnor2 || gates.num_xnor2 == 0));
    CHECK((params.lt2 || gates.num_lt2 == 0));
    CHECK((params.gt2 || gates.num_gt2 == 0));
    CHECK((params.le2 || gates.num_le2 == 0));
    CHECK((params.ge2 || gates.num_ge2 == 0));
    CHECK((params.and3 || gates.num_and3 == 0));
    CHECK((params.xor_and || gates.num_xor_and == 0));
    CHECK((params.or_and || gates.num_or_and == 0));
    CHECK((params.onehot || gates.num_onehot == 0));
    CHECK((params.maj3 || gates.num_maj3 == 0));
    CHECK((params.gamble || gates.num_gamble == 0));
    CHECK((params.dot || gates.num_dot == 0));
    CHECK((params.mux || gates.num_mux == 0));
    CHECK((params.and_xor || gates.num_and_xor == 0));
    CHECK(gates.num_other == 0);
    return gates;
}

}  // namespace

TEST_CASE("Asymmetric gate selection", "[technology-mapping]")
{
    technology_network        ntk{};
    const auto                a = ntk.create_pi();
    const auto                b = ntk.create_pi();
    technology_mapping_params params{};
    params.inv               = true;
    const auto check_mapping = [&]
    {
        const auto gates = map_and_check(ntk, params);
        CHECK(gates.num_lt2 == static_cast<uint32_t>(params.lt2));
        CHECK(gates.num_gt2 == static_cast<uint32_t>(params.gt2));
        CHECK(gates.num_le2 == static_cast<uint32_t>(params.le2));
        CHECK(gates.num_ge2 == static_cast<uint32_t>(params.ge2));
    };

    SECTION("LT")
    {
        ntk.create_po(ntk.create_lt(a, b));
        params.lt2 = true;
        check_mapping();
    }
    SECTION("GT")
    {
        ntk.create_po(ntk.create_gt(a, b));
        params.gt2 = true;
        check_mapping();
    }
    SECTION("LE")
    {
        ntk.create_po(ntk.create_le(a, b));
        params.le2 = true;
        check_mapping();
    }
    SECTION("GE")
    {
        ntk.create_po(ntk.create_ge(a, b));
        params.ge2 = true;
        check_mapping();
    }
}

TEST_CASE("Single comparison gate without an inverter", "[technology-mapping]")
{
    technology_network network{};
    const auto         a = network.create_pi();
    const auto         b = network.create_pi();
    network.create_po(network.create_lt(a, b));

    SECTION("The matching gate maps successfully")
    {
        const auto gates = map_and_check(network, technology_mapping_params{.lt2 = true});
        CHECK(gates.num_lt2 == 1);
    }
    SECTION("A gate from the complementary class reports failure")
    {
        technology_mapping_stats stats{};
        const auto               mapped = technology_mapping(network, technology_mapping_params{.le2 = true}, &stats);
        CHECK(stats.mapper_stats.mapping_error);
        CHECK(mapped.num_pos() == 0);
    }
}

TEST_CASE("Simple AOI network mapping", "[technology-mapping]")
{
    map_and_check(blueprints::maj1_network<mockturtle::aig_network>(), and_or_not());
    map_and_check(blueprints::and_or_network<mockturtle::aig_network>(), and_or_not());
    map_and_check(blueprints::inverter_network<mockturtle::aig_network>(), and_or_not());
}

// XAG is intentionally excluded: `and_or_not_maj()` does not enable XOR, which XAG networks require, so mapping them
// would (correctly) throw `missing_required_gates_exception`.
TEMPLATE_TEST_CASE("Simple AOIM network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xmg_network)
{
    map_and_check(blueprints::maj1_network<TestType>(), and_or_not_maj());
    map_and_check(blueprints::and_or_network<TestType>(), and_or_not_maj());
    map_and_check(blueprints::inverter_network<TestType>(), and_or_not_maj());
}

TEMPLATE_TEST_CASE("Simple 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check(blueprints::maj1_network<TestType>(), all_standard_2_input_functions());
    map_and_check(blueprints::and_or_network<TestType>(), all_standard_2_input_functions());
    map_and_check(blueprints::inverter_network<TestType>(), all_standard_2_input_functions());

    map_and_check(blueprints::maj1_network<TestType>(), all_2_input_functions());
    map_and_check(blueprints::and_or_network<TestType>(), all_2_input_functions());
    map_and_check(blueprints::inverter_network<TestType>(), all_2_input_functions());
}

TEMPLATE_TEST_CASE("Complex 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check(blueprints::maj4_network<TestType>(), all_standard_2_input_functions());

    map_and_check(blueprints::maj4_network<TestType>(), all_2_input_functions());
}

// AIG and XAG are intentionally excluded: `all_standard_3_input_functions()` enables neither AND nor XOR, which AIG
// (AND) and XAG (AND, XOR) networks require, so mapping them would (correctly) throw
// `missing_required_gates_exception`.
TEMPLATE_TEST_CASE("Complex 3-input network mapping", "[technology-mapping]", mockturtle::mig_network,
                   mockturtle::xmg_network)
{
    map_and_check(blueprints::maj4_network<TestType>(), all_standard_3_input_functions());
}

TEMPLATE_TEST_CASE("Complex all function network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    // Use all_supported_standard_functions which includes both 2-input and 3-input gates
    technology_mapping_stats stats{};
    const auto               mapped_ntk =
        technology_mapping(blueprints::maj4_network<TestType>(), all_supported_standard_functions(), &stats);
    REQUIRE_FALSE(stats.mapper_stats.mapping_error);
    check_eq(blueprints::maj4_network<TestType>(), mapped_ntk);
}

// XAG is intentionally excluded: `and_or_not_maj()` does not enable XOR, which XAG networks require, so mapping them
// would (correctly) throw `missing_required_gates_exception`.
TEMPLATE_TEST_CASE("Name conservation after technology mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xmg_network)
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<TestType>>();
    maj.set_network_name("maj");

    technology_mapping_stats stats{};

    const auto mapped_maj = technology_mapping(maj, and_or_not_maj(), &stats);

    REQUIRE_FALSE(stats.mapper_stats.mapping_error);

    REQUIRE(mapped_maj.num_pis() == 3);
    REQUIRE(mapped_maj.num_pos() == 1);
    CHECK_THAT(mapped_maj.get_network_name(), Catch::Matchers::Equals("maj"));
    CHECK_THAT(mapped_maj.get_name(mapped_maj.make_signal(mapped_maj.pi_at(0))), Catch::Matchers::Equals("a"));
    CHECK_THAT(mapped_maj.get_name(mapped_maj.make_signal(mapped_maj.pi_at(1))), Catch::Matchers::Equals("b"));
    CHECK_THAT(mapped_maj.get_name(mapped_maj.make_signal(mapped_maj.pi_at(2))), Catch::Matchers::Equals("c"));
    CHECK_THAT(mapped_maj.get_output_name(0), Catch::Matchers::Equals("f"));
}

TEST_CASE("Exception for missing required gates in AIG", "[technology-mapping]")
{
    const auto aig = blueprints::maj1_network<mockturtle::aig_network>();

    SECTION("Missing INV")
    {
        const technology_mapping_params params{.and2 = true};
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }

    SECTION("Missing AND")
    {
        const technology_mapping_params params{.inv = true};
        // params.and2 is false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }

    SECTION("Missing both INV and AND")
    {
        const technology_mapping_params params{};
        // Both params.inv and params.and2 are false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }
}

TEST_CASE("Exception for missing required gates in XAG", "[technology-mapping]")
{
    const auto xag = blueprints::maj1_network<mockturtle::xag_network>();

    SECTION("Missing INV")
    {
        const technology_mapping_params params{.and2 = true, .xor2 = true};
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing AND")
    {
        const technology_mapping_params params{.inv = true, .xor2 = true};
        // params.and2 is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing XOR")
    {
        const technology_mapping_params params{.inv = true, .and2 = true};
        // params.xor2 is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing all required gates")
    {
        const technology_mapping_params params{};
        // All required params are false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }
}

TEST_CASE("Exception for missing required gates in MIG", "[technology-mapping]")
{
    const auto mig = blueprints::maj1_network<mockturtle::mig_network>();

    SECTION("Missing INV")
    {
        const technology_mapping_params params{.maj3 = true};
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }

    SECTION("Missing MAJ")
    {
        const technology_mapping_params params{.inv = true};
        // params.maj3 is false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }

    SECTION("Missing both INV and MAJ")
    {
        const technology_mapping_params params{};
        // Both params.inv and params.maj3 are false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }
}

TEST_CASE("No exception when all required gates are present", "[technology-mapping]")
{
    SECTION("AIG with AND and INV")
    {
        const auto                      aig = blueprints::maj1_network<mockturtle::aig_network>();
        const technology_mapping_params params{.inv = true, .and2 = true};

        CHECK_NOTHROW(technology_mapping(aig, params));
    }

    SECTION("XAG with AND, XOR, and INV")
    {
        const auto                      xag = blueprints::maj1_network<mockturtle::xag_network>();
        const technology_mapping_params params{.inv = true, .and2 = true, .xor2 = true};

        CHECK_NOTHROW(technology_mapping(xag, params));
    }

    SECTION("MIG with MAJ and INV")
    {
        const auto                      mig = blueprints::maj1_network<mockturtle::mig_network>();
        const technology_mapping_params params{.inv = true, .maj3 = true};

        CHECK_NOTHROW(technology_mapping(mig, params));
    }
}

TEST_CASE("Incomplete technology libraries report mapping failure", "[technology-mapping]")
{
    technology_network network{};
    const auto         a             = network.create_pi();
    const auto         b             = network.create_pi();
    const auto         c             = network.create_pi();
    const auto         exceeds_arity = GENERATE(false, true);
    CAPTURE(exceeds_arity);
    if (exceeds_arity)
    {
        network.create_po(network.create_maj(a, b, c));
    }
    else
    {
        network.create_po(network.create_xor(a, b));
    }
    const technology_mapping_params params{.inv = true, .and2 = true};
    technology_mapping_stats        stats{};
    const auto                      mapped = technology_mapping(network, params, &stats);
    CHECK(stats.mapper_stats.mapping_error);
    CHECK(mapped.num_pos() == 0);
    CHECK(network.num_pis() == 3);
    CHECK(network.num_pos() == 1);
}

TEST_CASE("Mapping preserves separate constant values in outputs and fanins", "[technology-mapping]")
{
    technology_network network{};
    const auto         a    = network.create_pi();
    const auto         b    = network.create_pi();
    const auto         zero = network.get_constant(false);
    const auto         one  = network.get_constant(true);
    network.create_po(zero);
    network.create_po(one);
    network.create_po(network.create_and(a, one));
    network.create_po(network.create_and(a, zero));
    network.create_po(network.create_or(b, one));
    network.create_po(network.create_or(b, zero));

    const auto                      inv = GENERATE(false, true);
    const technology_mapping_params params{.inv = inv, .and2 = true, .or2 = true};
    technology_mapping_stats        stats{};
    const auto                      mapped = technology_mapping(network, params, &stats);
    REQUIRE_FALSE(stats.mapper_stats.mapping_error);
    const auto       simulator = mockturtle::default_simulator<kitty::dynamic_truth_table>{2};
    const auto       actual    = mockturtle::simulate<kitty::dynamic_truth_table>(mapped, simulator);
    const auto       original  = mockturtle::simulate<kitty::dynamic_truth_table>(network, simulator);
    const std::array expected{"0000", "1111", "1010", "0000", "1111", "1100"};
    REQUIRE(actual.size() == expected.size());
    REQUIRE(original.size() == expected.size());
    for (auto output = 0u; output < expected.size(); ++output)
    {
        CAPTURE(inv, output);
        CHECK(kitty::to_binary(actual[output]) == expected.at(output));
        CHECK(kitty::to_binary(original[output]) == expected.at(output));
    }
}

TEMPLATE_TEST_CASE("Sequential inputs report mapping failure", "[technology-mapping]",
                   mockturtle::sequential<technology_network>, register_count_network)
{
    /** @brief Sequential source with a used, distinct constant-one node. */
    TestType network{};
    /** @brief Primary input driving the register input. */
    const auto input = network.create_pi();
    network.create_ro();
    network.create_po(network.get_constant(true));
    network.create_ri(input);
    REQUIRE_FALSE(network.is_combinational());
    REQUIRE(network.num_registers() == 1);
    REQUIRE(network.get_node(network.get_constant(false)) != network.get_node(network.get_constant(true)));
    REQUIRE(network.po_at(0) == network.get_constant(true));
    /** @brief Failure status of the unsupported source. */
    technology_mapping_stats stats{};
    /** @brief The unsupported source produces no mapped interface. */
    const auto mapped = technology_mapping(network, and_or_not(), &stats);
    CHECK(stats.mapper_stats.mapping_error);
    CHECK(mapped.num_pis() == 0);
    CHECK(mapped.num_pos() == 0);
    CHECK(mapped.num_gates() == 0);
    CHECK(network.num_pis() == 1);
    CHECK(network.num_pos() == 1);
    CHECK(network.num_registers() == 1);
    CHECK(network.po_at(0) == network.get_constant(true));
}

TEST_CASE("Mapping preserves a constant-one cut-view output", "[technology-mapping]")
{
    /** @brief Underlying network with an otherwise unused true constant. */
    const technology_network network{};
    /** @brief Distinct constant-one signal exposed only by the cut. */
    const auto one = network.get_constant(true);
    /** @brief Constant-only cut whose output does not increase the inherited fanout count. */
    const mockturtle::cut_view view{network, std::vector<technology_network::node>{}, one};
    REQUIRE(view.get_node(one) != view.get_node(view.get_constant(false)));
    REQUIRE(view.fanout_size(view.get_node(one)) == 0);
    /** @brief Outputs exposed by the cut rather than the underlying network. */
    std::vector<technology_network::signal> outputs{};
    view.foreach_po([&](const auto& output) { outputs.push_back(output); });
    REQUIRE(outputs == std::vector{one});
    /** @brief Mapping status for the constant-only cut. */
    technology_mapping_stats stats{};
    /** @brief Mapped function of the cut. */
    const auto mapped = technology_mapping(view, and_or_not(), &stats);
    REQUIRE_FALSE(stats.mapper_stats.mapping_error);
    /** @brief Exhaustive simulation of the zero-input output. */
    const auto actual = mockturtle::simulate<kitty::dynamic_truth_table>(
        mapped, mockturtle::default_simulator<kitty::dynamic_truth_table>{0});
    REQUIRE(actual.size() == 1);
    CHECK(kitty::to_binary(actual.front()) == "1");
    CHECK(network.num_pos() == 0);
    CHECK(network.fanout_size(network.get_node(one)) == 0);
}

TEST_CASE("Register-count fallback accepts combinational input", "[technology-mapping]")
{
    /** @brief A register-capable interface with no sequential state. */
    register_count_network network{};
    /** @brief Input forwarded to the primary output. */
    const auto input = network.create_pi();
    network.create_po(input);
    REQUIRE(network.num_registers() == 0);
    /** @brief Status for a supported combinational source. */
    technology_mapping_stats stats{};
    /** @brief Mapped identity function. */
    const auto mapped = technology_mapping(network, and_or_not(), &stats);
    REQUIRE_FALSE(stats.mapper_stats.mapping_error);
    /** @brief Complete one-input simulation. */
    const auto actual = mockturtle::simulate<kitty::dynamic_truth_table>(
        mapped, mockturtle::default_simulator<kitty::dynamic_truth_table>{1});
    REQUIRE(actual.size() == 1);
    CHECK(kitty::to_binary(actual.front()) == "10");
}
