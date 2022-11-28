//
// Created by marcel on 25.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("Simple network balancing", "[network-balancing]")
{
    const auto tec = blueprints::unbalanced_and_inv_network<technology_network>();

    const auto balanced = network_balancing<technology_network>(tec);

    CHECK(balanced.size() == tec.size() + 1);
    CHECK(is_balanced(balanced));

    check_eq(tec, balanced);
}

TEST_CASE("Complex network balancing", "[network-balancing]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    const auto mig = blueprints::maj4_network<mockturtle::mig_network>();
    const auto xag = blueprints::maj4_network<mockturtle::xag_network>();
    const auto tec = blueprints::maj4_network<fiction::technology_network>();

    SECTION("without primary output unification")
    {
        const auto balanced_aig = network_balancing<technology_network>(aig);
        const auto balanced_mig = network_balancing<technology_network>(mig);
        const auto balanced_xag = network_balancing<technology_network>(xag);
        const auto balanced_tec = network_balancing<technology_network>(tec);

        CHECK(is_balanced(balanced_aig));
        CHECK(is_balanced(balanced_mig));
        CHECK(is_balanced(balanced_xag));
        CHECK(is_balanced(balanced_tec));

        CHECK(!is_balanced(balanced_aig, {true}));
        CHECK(!is_balanced(balanced_mig, {true}));
        CHECK(!is_balanced(balanced_xag, {true}));
        CHECK(!is_balanced(balanced_tec, {true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
    SECTION("with primary output unification")
    {
        const auto balanced_aig = network_balancing<technology_network>(aig, {true});
        const auto balanced_mig = network_balancing<technology_network>(mig, {true});
        const auto balanced_xag = network_balancing<technology_network>(xag, {true});
        const auto balanced_tec = network_balancing<technology_network>(tec, {true});

        CHECK(is_balanced(balanced_aig, {true}));
        CHECK(is_balanced(balanced_mig, {true}));
        CHECK(is_balanced(balanced_xag, {true}));
        CHECK(is_balanced(balanced_tec, {true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
}
