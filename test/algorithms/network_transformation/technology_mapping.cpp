//
// Created by marcel on 06.09.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>
#include <fiction/algorithms/properties/count_gate_types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>

using namespace fiction;

namespace
{

template <typename Ntk>
void map_and_check_aoi(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, and_or_not(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);

    CHECK(gt_stats.num_and3 == 0);
    CHECK(gt_stats.num_xor_and == 0);
    CHECK(gt_stats.num_or_and == 0);
    CHECK(gt_stats.num_onehot == 0);
    CHECK(gt_stats.num_maj3 == 0);
    CHECK(gt_stats.num_gamble == 0);
    CHECK(gt_stats.num_dot == 0);
    CHECK(gt_stats.num_mux == 0);
    CHECK(gt_stats.num_and_xor == 0);
}

template <typename Ntk>
void map_and_check_aoim(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, and_or_not_maj(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);

    CHECK(gt_stats.num_and3 == 0);
    CHECK(gt_stats.num_xor_and == 0);
    CHECK(gt_stats.num_or_and == 0);
    CHECK(gt_stats.num_onehot == 0);
    CHECK(gt_stats.num_gamble == 0);
    CHECK(gt_stats.num_dot == 0);
    CHECK(gt_stats.num_mux == 0);
    CHECK(gt_stats.num_and_xor == 0);
}

template <typename Ntk>
void map_and_check_all_2_inp(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_2_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_and3 == 0);
    CHECK(gt_stats.num_xor_and == 0);
    CHECK(gt_stats.num_or_and == 0);
    CHECK(gt_stats.num_onehot == 0);
    CHECK(gt_stats.num_maj3 == 0);
    CHECK(gt_stats.num_gamble == 0);
    CHECK(gt_stats.num_dot == 0);
    CHECK(gt_stats.num_mux == 0);
    CHECK(gt_stats.num_and_xor == 0);
}

template <typename Ntk>
void map_and_check_all_standard_2_inp(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_2_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_lt2 == 0);
    CHECK(gt_stats.num_gt2 == 0);
    CHECK(gt_stats.num_le2 == 0);
    CHECK(gt_stats.num_ge2 == 0);

    CHECK(gt_stats.num_and3 == 0);
    CHECK(gt_stats.num_xor_and == 0);
    CHECK(gt_stats.num_or_and == 0);
    CHECK(gt_stats.num_onehot == 0);
    CHECK(gt_stats.num_maj3 == 0);
    CHECK(gt_stats.num_gamble == 0);
    CHECK(gt_stats.num_dot == 0);
    CHECK(gt_stats.num_mux == 0);
    CHECK(gt_stats.num_and_xor == 0);
}

template <typename Ntk>
void map_and_check_all_standard_3_inp(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_3_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_and2 == 0);
    CHECK(gt_stats.num_or2 == 0);
    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);
}

template <typename Ntk>
void map_and_check_all_3_inp(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_3_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_and2 == 0);
    CHECK(gt_stats.num_or2 == 0);
    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);
}

template <typename Ntk>
void map_and_check_all_func(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_3_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_and2 == 0);
    CHECK(gt_stats.num_or2 == 0);
    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);
}

template <typename Ntk>
void map_and_check_all_standard_func(const Ntk& ntk)
{
    technology_mapping_stats stats{};

    const auto mapped_ntk = technology_mapping(ntk, all_standard_3_input_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);

    count_gate_types_stats gt_stats{};
    count_gate_types(mapped_ntk, &gt_stats);

    CHECK(gt_stats.num_and2 == 0);
    CHECK(gt_stats.num_or2 == 0);
    CHECK(gt_stats.num_nand2 == 0);
    CHECK(gt_stats.num_nor2 == 0);
    CHECK(gt_stats.num_xor2 == 0);
    CHECK(gt_stats.num_xnor2 == 0);
}

}  // namespace

TEMPLATE_TEST_CASE("Simple AOI network mapping", "[technology-mapping]", mockturtle::aig_network)
{
    map_and_check_aoi(blueprints::maj1_network<TestType>());
    map_and_check_aoi(blueprints::and_or_network<TestType>());
    map_and_check_aoi(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Simple AOIM network mapping", "[technology-mapping]", mockturtle::mig_network)
{
    map_and_check_aoim(blueprints::maj1_network<TestType>());
    map_and_check_aoim(blueprints::and_or_network<TestType>());
    map_and_check_aoim(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Simple 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check_all_standard_2_inp(blueprints::maj1_network<TestType>());
    map_and_check_all_standard_2_inp(blueprints::and_or_network<TestType>());
    map_and_check_all_standard_2_inp(blueprints::inverter_network<TestType>());

    map_and_check_all_2_inp(blueprints::maj1_network<TestType>());
    map_and_check_all_2_inp(blueprints::and_or_network<TestType>());
    map_and_check_all_2_inp(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check_all_standard_2_inp(blueprints::maj4_network<TestType>());

    map_and_check_all_2_inp(blueprints::maj4_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex 3-input network mapping", "[technology-mapping]", mockturtle::mig_network)
{
    map_and_check_all_3_inp(blueprints::maj4_network<TestType>());
    map_and_check_all_standard_3_inp(blueprints::maj4_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex all function network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    // Use all_supported_standard_functions which includes both 2-input and 3-input gates
    technology_mapping_stats stats{};
    const auto               mapped_ntk =
        technology_mapping(blueprints::maj4_network<TestType>(), all_supported_standard_functions(), &stats);
    REQUIRE(!stats.mapper_stats.mapping_error);
    check_eq(blueprints::maj4_network<TestType>(), mapped_ntk);
}

TEMPLATE_TEST_CASE("Name conservation after technology mapping", "[technology-mapping]", mockturtle::mig_network)
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<TestType>>();
    maj.set_network_name("maj");

    technology_mapping_stats stats{};

    const auto mapped_maj = technology_mapping(maj, and_or_not_maj(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    // network name
    CHECK(mapped_maj.get_network_name() == "maj");
}

TEST_CASE("Exception for missing required gates in AIG", "[technology-mapping]")
{
    const auto aig = blueprints::maj1_network<mockturtle::aig_network>();

    technology_mapping_params params{};

    SECTION("Missing INV")
    {
        params.and2 = true;
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }

    SECTION("Missing AND")
    {
        params.inv = true;
        // params.and2 is false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }

    SECTION("Missing both INV and AND")
    {
        // Both params.inv and params.and2 are false
        CHECK_THROWS_AS(technology_mapping(aig, params), missing_required_gates_exception);
    }
}

TEST_CASE("Exception for missing required gates in XAG", "[technology-mapping]")
{
    const auto xag = blueprints::maj1_network<mockturtle::xag_network>();

    technology_mapping_params params{};

    SECTION("Missing INV")
    {
        params.and2 = true;
        params.xor2 = true;
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing AND")
    {
        params.inv  = true;
        params.xor2 = true;
        // params.and2 is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing XOR")
    {
        params.inv  = true;
        params.and2 = true;
        // params.xor2 is false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }

    SECTION("Missing all required gates")
    {
        // All required params are false
        CHECK_THROWS_AS(technology_mapping(xag, params), missing_required_gates_exception);
    }
}

TEST_CASE("Exception for missing required gates in MIG", "[technology-mapping]")
{
    const auto mig = blueprints::maj1_network<mockturtle::mig_network>();

    technology_mapping_params params{};

    SECTION("Missing INV")
    {
        params.maj3 = true;
        // params.inv is false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }

    SECTION("Missing MAJ")
    {
        params.inv = true;
        // params.maj3 is false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }

    SECTION("Missing both INV and MAJ")
    {
        // Both params.inv and params.maj3 are false
        CHECK_THROWS_AS(technology_mapping(mig, params), missing_required_gates_exception);
    }
}

TEST_CASE("No exception when all required gates are present", "[technology-mapping]")
{
    SECTION("AIG with AND and INV")
    {
        const auto                aig = blueprints::maj1_network<mockturtle::aig_network>();
        technology_mapping_params params{};
        params.and2 = true;
        params.inv  = true;

        CHECK_NOTHROW(technology_mapping(aig, params));
    }

    SECTION("XAG with AND, XOR, and INV")
    {
        const auto                xag = blueprints::maj1_network<mockturtle::xag_network>();
        technology_mapping_params params{};
        params.and2 = true;
        params.xor2 = true;
        params.inv  = true;

        CHECK_NOTHROW(technology_mapping(xag, params));
    }

    SECTION("MIG with MAJ and INV")
    {
        const auto                mig = blueprints::maj1_network<mockturtle::mig_network>();
        technology_mapping_params params{};
        params.maj3 = true;
        params.inv  = true;

        CHECK_NOTHROW(technology_mapping(mig, params));
    }
}
