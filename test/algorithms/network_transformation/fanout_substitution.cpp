//
// Created by marcel on 25.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <type_traits>

using namespace fiction;

template <typename Ntk>
void substitute(const Ntk& ntk, const fanout_substitution_params ps, const uint32_t size)
{
    const auto substituted = fanout_substitution<technology_network>(ntk, ps);

    CHECK(substituted.size() == size);
    CHECK(is_fanout_substituted(substituted, ps));

    check_eq(ntk, substituted);
}

TEST_CASE("Name conservation after fanout substitution", "[fanout-substitution]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto substituted_maj = fanout_substitution<mockturtle::names_view<fiction::technology_network>>(maj);

    // network name
    CHECK(substituted_maj.get_network_name() == "maj");

    // PI names
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(2)) == "a");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(3)) == "b");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(4)) == "c");

    // PO names
    CHECK(substituted_maj.get_output_name(0) == "f");
}

TEST_CASE("Simple fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::multi_output_and_network<technology_network>();

    const fanout_substitution_params ps_depth{fanout_substitution_params::substitution_strategy::DEPTH};
    const fanout_substitution_params ps_breadth{fanout_substitution_params::substitution_strategy::BREADTH};
    const fanout_substitution_params ps_random{fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth, tec.size() + 3);
    substitute(tec, ps_breadth, tec.size() + 3);
    substitute(tec, ps_random, tec.size() + 3);
}

TEST_CASE("Complex fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::maj4_network<technology_network>();
    CHECK(!is_fanout_substituted(tec));

    const fanout_substitution_params ps_depth{fanout_substitution_params::substitution_strategy::DEPTH};
    const fanout_substitution_params ps_breadth{fanout_substitution_params::substitution_strategy::BREADTH};
    const fanout_substitution_params ps_random{fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth, tec.size() + 7);
    substitute(tec, ps_breadth, tec.size() + 7);
    substitute(tec, ps_random, tec.size() + 7);

    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    CHECK(!is_fanout_substituted(aig));
    substitute(aig, ps_depth, aig.size() + 41);
    substitute(aig, ps_breadth, aig.size() + 41);
    substitute(aig, ps_random, aig.size() + 41);
}

TEST_CASE("Degree and threshold in fanout substitution", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    const fanout_substitution_params ps_31{fanout_substitution_params::substitution_strategy::BREADTH, 3, 1};
    const fanout_substitution_params ps_22{fanout_substitution_params::substitution_strategy::DEPTH, 2, 2};
    const fanout_substitution_params ps_32{fanout_substitution_params::substitution_strategy::RANDOM, 3, 2};

    substitute(aig, ps_31, aig.size() + 35);
    substitute(aig, ps_22, aig.size() + 34);
    substitute(aig, ps_32, aig.size() + 32);
}

TEST_CASE("Random fanout substitution with fixed vs. varying seeds", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    SECTION("Fixed seed yields deterministic behavior")
    {
        fanout_substitution_params ps{fanout_substitution_params::substitution_strategy::RANDOM};
        ps.seed = 42;

        // expect no exceptions and consistent substitution
        substitute(aig, ps, aig.size() + 41);
    }

    SECTION("Different seeds produce different results")
    {
        fanout_substitution_params ps{fanout_substitution_params::substitution_strategy::RANDOM};

        // compute baseline depth using seed = 1
        ps.seed               = 1;
        const auto base_sub   = mockturtle::depth_view{fanout_substitution<technology_network>(aig, ps)};
        const auto base_depth = base_sub.depth();

        bool found_different = false;
        // try seeds 2 through 20; break as soon as we see a different depth
        for (int s = 2; s <= 20; ++s)
        {
            ps.seed        = s;
            const auto sub = mockturtle::depth_view{fanout_substitution<technology_network>(aig, ps)};
            if (sub.depth() != base_depth)
            {
                found_different = true;
                break;
            }
        }

        REQUIRE(found_different);
    }
}

TEST_CASE("Consistent network size after multiple fanout substitutions", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = fanout_substitution<technology_network>(aig);

    auto subsubsubsubstituted = fanout_substitution<technology_network>(
        fanout_substitution<technology_network>(fanout_substitution<technology_network>(substituted)));

    CHECK(substituted.size() == subsubsubsubstituted.size());
}

TEST_CASE("Consistent fanout substitution after balancing", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = fanout_substitution<technology_network>(aig);

    CHECK(is_fanout_substituted(substituted));
    auto balanced = network_balancing<technology_network>(substituted);
    CHECK(is_fanout_substituted(balanced));

    auto tec = blueprints::fanout_substitution_corner_case_network<technology_network>();

    auto substituted_tec = fanout_substitution<technology_network>(tec);
    CHECK(is_fanout_substituted(substituted_tec));
    auto balanced_tec = network_balancing<technology_network>(substituted_tec);
    CHECK(is_fanout_substituted(balanced_tec));
}
