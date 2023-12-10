//
// Created by benjamin on 13.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/inverter_substitution.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <algorithm>
#include <cstdlib>
#include <vector>

using namespace fiction;

TEST_CASE("Check correct substitution at one FO node", "[inverter-substitution]")
{
    // checks the right node assignment after a single substitution
    const auto check_single_substitution = [](const auto& ntk)
    {
        std::vector<mockturtle::node<mockturtle::names_view<technology_network>>> fo_to_inv;
        std::vector<mockturtle::node<mockturtle::names_view<technology_network>>> inv_to_fo;
        const auto fo_ntk = mockturtle::fanout_view<mockturtle::names_view<technology_network>>(ntk);

        // Gather the affected nodes
        fo_ntk.foreach_gate(
            [&](const auto& g)
            {
                if (fo_ntk.is_fanout(g))
                {
                    const auto fanout_inv = fanouts(fo_ntk, g);
                    const auto do_substitute =
                        std::all_of(fanout_inv.cbegin(), fanout_inv.cend(),
                                    [&fo_ntk](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });

                    if (do_substitute && fanout_inv.size() > 1)
                    {
                        fo_to_inv.emplace_back(g);
                        inv_to_fo.emplace_back(fanout_inv[0]);
                    }
                }
            });

        assert(fo_to_inv.size() == inv_to_fo.size());

        auto balanced_ntk = inverter_substitution(ntk);
        // check for equivalence
        check_eq(ntk, balanced_ntk);
        CHECK(fo_ntk.num_gates() == balanced_ntk.num_gates() + 1);
        // check if the affected nodes have been assigned correctly
        int correct_node_assignment = 1;
        for (std::size_t i = 0; i < fo_to_inv.size(); ++i)
        {
            if (!balanced_ntk.is_inv(fo_to_inv[i]) || !balanced_ntk.is_fanout(inv_to_fo[i]))
            {
                correct_node_assignment = 0;
            }
        }
        return correct_node_assignment;
    };

    CHECK(check_single_substitution(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_a<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_single_substitution(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_b<mockturtle::names_view<mockturtle::aig_network>>())));
}

TEST_CASE("Check if output network is substituted", "[inverter-substitution]")
{
    // returns true, when no further inverters can be substituted in the output network
    const auto check_substituted = [](const auto& ntk)
    {
        auto balanced_ntk =
            mockturtle::fanout_view<mockturtle::names_view<technology_network>>(inverter_substitution(ntk));
        // check for equivalence
        check_eq(ntk, balanced_ntk);
        CHECK(ntk.num_gates() >= balanced_ntk.num_gates());
        int properly_substituted = 1;

        // Gather the affected nodes
        balanced_ntk.foreach_gate(
            [&](const auto& g)
            {
                if (balanced_ntk.is_fanout(g))
                {
                    const auto fanout_inv = fanouts(balanced_ntk, g);
                    const auto do_substitute =
                        std::all_of(fanout_inv.cbegin(), fanout_inv.cend(),
                                    [&balanced_ntk](const auto& fo_node) { return balanced_ntk.is_inv(fo_node); });

                    if (do_substitute && fanout_inv.size() > 1)
                    {
                        properly_substituted = 0;
                    }  // check for equivalence
                }
            });
        return properly_substituted;
    };

    // no substitution
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_inv_flag_a<mockturtle::names_view<mockturtle::aig_network>>())));
    // one substitution
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_a<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_b<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::par_check<mockturtle::names_view<mockturtle::aig_network>>())));
    // two concatenated substitutions
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::two_inverter_substitutions_a<mockturtle::names_view<mockturtle::aig_network>>())));
    // three concatenated substitutions
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::three_inverter_substitutions_a<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::three_inverter_substitutions_b<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::three_inverter_substitutions_c<mockturtle::names_view<mockturtle::aig_network>>())));
    // four concatenated substitutions
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::four_inverter_substitutions_a<mockturtle::names_view<mockturtle::aig_network>>())));
    CHECK(check_substituted(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::four_inverter_substitutions_b<mockturtle::names_view<mockturtle::aig_network>>())));
}

TEST_CASE("Check output preservation", "[inverter-substitution]")
{
    // checks if the PO of the deleted inverter is preserved and assigned to the right node (for a single substitution)
    const auto check_output_preservation_for_single_balancing = [](const auto& ntk)
    {
        std::vector<mockturtle::node<mockturtle::names_view<technology_network>>> inv_to_fo;
        const auto fo_ntk = mockturtle::fanout_view<mockturtle::names_view<technology_network>>(ntk);
        auto       balanced_ntk =
            mockturtle::fanout_view<mockturtle::names_view<technology_network>>(inverter_substitution(ntk));
        // check for equivalence
        check_eq(ntk, balanced_ntk);

        // Gather the affected nodes
        fo_ntk.foreach_gate(
            [&](const auto& g)
            {
                if (fo_ntk.is_fanout(g))
                {
                    const auto fanout_inv = fanouts(fo_ntk, g);
                    const auto do_substitute =
                        std::all_of(fanout_inv.cbegin(), fanout_inv.cend(),
                                    [&fo_ntk](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });

                    if (do_substitute && fanout_inv.size() > 1)
                    {
                        inv_to_fo.emplace_back(fanout_inv[0]);
                        if (fo_ntk.is_po(fanout_inv[0]))
                        {
                            CHECK(balanced_ntk.is_fanout(fanout_inv[0]));
                            const auto fanout_pos = fanouts(fo_ntk, g);
                            CHECK(balanced_ntk.is_po(fanout_pos[0]));
                            CHECK(balanced_ntk.is_po(fanout_pos[1]));
                        }
                    }
                }
            });
    };

    // checks if the number of POs stays the same after inverter substitution
    const auto check_output_preservation_num_pos = [](const auto& ntk)
    {
        const auto fo_ntk = mockturtle::fanout_view<mockturtle::names_view<technology_network>>(ntk);
        auto       balanced_ntk =
            mockturtle::fanout_view<mockturtle::names_view<technology_network>>(inverter_substitution(ntk));
        CHECK(balanced_ntk.num_pos() == fo_ntk.num_pos());
    };

    check_output_preservation_for_single_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_a<mockturtle::names_view<mockturtle::aig_network>>()));
    check_output_preservation_for_single_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_b<mockturtle::names_view<mockturtle::aig_network>>()));

    check_output_preservation_num_pos(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_inv_flag_a<mockturtle::names_view<mockturtle::aig_network>>()));
    check_output_preservation_num_pos(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_a<mockturtle::names_view<mockturtle::aig_network>>()));
    check_output_preservation_num_pos(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::test_single_inverter_substitution_b<mockturtle::names_view<mockturtle::aig_network>>()));
    check_output_preservation_num_pos(fanout_substitution<mockturtle::names_view<technology_network>>(
        blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>()));
}
