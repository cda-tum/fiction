//
// Created by benjamin on 13.04.23.
//
#include "fiction/algorithms/network_transformation/inverter_balancing.hpp"

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/views/input_ordering_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

using namespace fiction;

TEST_CASE("Double Balancing", "[inverter_balancing]")
{
    auto test_nw = blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>();

    auto aig_ntk{input_ordering_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)}}};

    std::cout<<"Before"<<std::endl;
    /*aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(aig_ntk.is_constant(n))
            {
                std::cout<<n<<std::endl;
            }
        });*/
    std::cout<<"Pi"<<std::endl;
    aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(aig_ntk.is_pi(n))
            {
                std::cout<<n<<std::endl;
            }
        });
    std::cout<<"Fo"<<std::endl;
    aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(aig_ntk.is_fanout(n))
            {
                std::cout<<n<<std::endl;
            }
        });
    std::cout<<"Inv"<<std::endl;
    aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(aig_ntk.is_inv(n))
            {
                std::cout<<n<<std::endl;
            }
        });

    auto balanced_aig_ntk{input_ordering_view{mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(test_nw))}}};

    std::cout<<"After"<<std::endl;
    /*balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(balanced_aig_ntk.is_constant(n))
            {
                std::cout<<n<<std::endl;
            }
        });*/
    std::cout<<"Pi"<<std::endl;
    balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(balanced_aig_ntk.is_pi(n))
            {
                std::cout<<n<<std::endl;
            }
        });
    std::cout<<"Fo"<<std::endl;
    balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(balanced_aig_ntk.is_fanout(n))
            {
                std::cout<<n<<std::endl;
            }
        });
    std::cout<<"Buf"<<std::endl;
    balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(balanced_aig_ntk.is_buf(n))
            {
                std::cout<<n<<std::endl;
            }
        });
    std::cout<<"Inv"<<std::endl;
    balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            if(balanced_aig_ntk.is_inv(n))
            {
                std::cout<<n<<std::endl;
            }
        });

    std::cout<<"All"<<std::endl;
    balanced_aig_ntk.foreach_node(
        [&](const auto& n)
        {
            std::cout<<n<<std::endl;
        });
}