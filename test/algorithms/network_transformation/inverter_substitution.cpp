//
// Created by benjamin on 13.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/inverter_substitution.hpp>

using namespace fiction;

TEST_CASE("One Balancing", "[inverter-substitution]")
{
    auto test_nw = blueprints::test_inverter_substitution<mockturtle::names_view<mockturtle::aig_network>>();
    auto aig_ntk{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)};

    CHECK(aig_ntk.num_gates() == 9);

    auto balanced_aig_ntk{inverter_substitution(aig_ntk)};

    CHECK(balanced_aig_ntk.num_gates() == 8);
}

TEST_CASE("Double Balancing", "[inverter-substitution]")
{
    auto test_nw =
        blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>();

    auto aig_ntk{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)};

    CHECK(aig_ntk.num_gates() == 7);

    auto balanced_aig_ntk{inverter_substitution(aig_ntk)};

    CHECK(balanced_aig_ntk.num_gates() == 4);

    test_nw.foreach_po([&](const auto& po){
                           auto poo = test_nw.get_node(po);
                           std::cout<<"PO: "<<poo<<std::endl;
    });
}

TEST_CASE("Test no impact", "[inverter-substitution]")
{
    auto test_nw =
        blueprints::test_inv_flag_a<mockturtle::names_view<mockturtle::aig_network>>();

    auto aig_ntk{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)};

    auto balanced_aig_ntk{inverter_substitution(aig_ntk)};

    CHECK(aig_ntk.num_gates() == balanced_aig_ntk.num_gates());
}
