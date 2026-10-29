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

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/technology_mapping.hpp>
#include <fiction/verification/count_gate_types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/views/names_view.hpp>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;
using namespace fiction::verification;

namespace
{

/**
 * @brief Checks mapping success, Boolean behavior, and the selected gate library.
 * @tparam Ntk Source network type.
 * @param ntk Source network.
 * @param params Selected gate library.
 */
template <typename Ntk>
void map_and_check(const Ntk& ntk, const technology_mapping_params& params)
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
}

}  // namespace

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

    // network name
    CHECK(mapped_maj.get_network_name() == "maj");
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
