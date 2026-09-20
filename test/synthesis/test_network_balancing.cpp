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
 * @brief Tests for `fiction/synthesis/network_balancing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/network_balancing.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;

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
    const auto tec = blueprints::maj4_network<technology_network>();

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

        CHECK(!is_balanced(balanced_aig, {.unify_outputs = true}));
        CHECK(!is_balanced(balanced_mig, {.unify_outputs = true}));
        CHECK(!is_balanced(balanced_xag, {.unify_outputs = true}));
        CHECK(!is_balanced(balanced_tec, {.unify_outputs = true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
    SECTION("with primary output unification")
    {
        const auto balanced_aig = network_balancing<technology_network>(aig, {.unify_outputs = true});
        const auto balanced_mig = network_balancing<technology_network>(mig, {.unify_outputs = true});
        const auto balanced_xag = network_balancing<technology_network>(xag, {.unify_outputs = true});
        const auto balanced_tec = network_balancing<technology_network>(tec, {.unify_outputs = true});

        CHECK(is_balanced(balanced_aig, {.unify_outputs = true}));
        CHECK(is_balanced(balanced_mig, {.unify_outputs = true}));
        CHECK(is_balanced(balanced_xag, {.unify_outputs = true}));
        CHECK(is_balanced(balanced_tec, {.unify_outputs = true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
}

TEST_CASE("Balance a network without primary outputs", "[network-balancing]")
{
    technology_network ntk{};
    for (const bool unify_outputs : {false, true})
    {
        const auto empty = network_balancing<technology_network>(ntk, {.unify_outputs = unify_outputs});
        CHECK(empty.num_gates() == 0);
        CHECK(empty.num_pos() == 0);
    }
    ntk.create_pi();
    const auto balanced = network_balancing<technology_network>(ntk, {.unify_outputs = true});
    CHECK(balanced.num_pis() == 1);
    CHECK(balanced.num_pos() == 0);
}
