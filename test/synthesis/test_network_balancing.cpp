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

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("Simple network balancing", "[network-balancing]")
{
    const auto tec = blueprints::unbalanced_and_inv_network<networks::technology_network>();

    const auto balanced = synthesis::network_balancing<networks::technology_network>(tec);

    CHECK(balanced.size() == tec.size() + 1);
    CHECK(synthesis::is_balanced(balanced));

    check_eq(tec, balanced);
}

TEST_CASE("Complex network balancing", "[network-balancing]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    const auto mig = blueprints::maj4_network<mockturtle::mig_network>();
    const auto xag = blueprints::maj4_network<mockturtle::xag_network>();
    const auto tec = blueprints::maj4_network<fiction::networks::technology_network>();

    SECTION("without primary output unification")
    {
        const auto balanced_aig = synthesis::network_balancing<networks::technology_network>(aig);
        const auto balanced_mig = synthesis::network_balancing<networks::technology_network>(mig);
        const auto balanced_xag = synthesis::network_balancing<networks::technology_network>(xag);
        const auto balanced_tec = synthesis::network_balancing<networks::technology_network>(tec);

        CHECK(synthesis::is_balanced(balanced_aig));
        CHECK(synthesis::is_balanced(balanced_mig));
        CHECK(synthesis::is_balanced(balanced_xag));
        CHECK(synthesis::is_balanced(balanced_tec));

        CHECK(!synthesis::is_balanced(balanced_aig, {true}));
        CHECK(!synthesis::is_balanced(balanced_mig, {true}));
        CHECK(!synthesis::is_balanced(balanced_xag, {true}));
        CHECK(!synthesis::is_balanced(balanced_tec, {true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
    SECTION("with primary output unification")
    {
        const auto balanced_aig = synthesis::network_balancing<networks::technology_network>(aig, {true});
        const auto balanced_mig = synthesis::network_balancing<networks::technology_network>(mig, {true});
        const auto balanced_xag = synthesis::network_balancing<networks::technology_network>(xag, {true});
        const auto balanced_tec = synthesis::network_balancing<networks::technology_network>(tec, {true});

        CHECK(synthesis::is_balanced(balanced_aig, {true}));
        CHECK(synthesis::is_balanced(balanced_mig, {true}));
        CHECK(synthesis::is_balanced(balanced_xag, {true}));
        CHECK(synthesis::is_balanced(balanced_tec, {true}));

        check_eq(aig, balanced_aig);
        check_eq(mig, balanced_mig);
        check_eq(xag, balanced_xag);
        check_eq(tec, balanced_tec);
    }
}
