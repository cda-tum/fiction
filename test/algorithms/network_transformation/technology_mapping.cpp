//
// Created by marcel on 06.09.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>
#include <fiction/algorithms/properties/count_gate_types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>

using namespace fiction;

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

    const auto mapped_ntk = technology_mapping(ntk, all_supported_standard_functions(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    check_eq(ntk, mapped_ntk);
}

TEMPLATE_TEST_CASE("Simple AOI network mapping", "[technology-mapping]", mockturtle::aig_network)
{
    map_and_check_aoi(blueprints::maj1_network<TestType>());
    map_and_check_aoi(blueprints::and_or_network<TestType>());
    map_and_check_aoi(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Simple AOIM network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    map_and_check_aoim(blueprints::maj1_network<TestType>());
    map_and_check_aoim(blueprints::and_or_network<TestType>());
    map_and_check_aoim(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Simple 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check_all_2_inp(blueprints::maj1_network<TestType>());
    map_and_check_all_2_inp(blueprints::and_or_network<TestType>());
    map_and_check_all_2_inp(blueprints::inverter_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex 2-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    map_and_check_all_2_inp(blueprints::maj4_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex 3-input network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    map_and_check_all_3_inp(blueprints::maj4_network<TestType>());
}

TEMPLATE_TEST_CASE("Complex all function network mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    map_and_check_all_func(blueprints::maj4_network<TestType>());
}

TEMPLATE_TEST_CASE("Name conservation after technology mapping", "[technology-mapping]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<TestType>>();
    maj.set_network_name("maj");

    technology_mapping_stats stats{};

    const auto mapped_maj = technology_mapping(maj, and_or_not_maj(), &stats);

    REQUIRE(!stats.mapper_stats.mapping_error);

    // network name
    CHECK(mapped_maj.get_network_name() == "maj");
}
