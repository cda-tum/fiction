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
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/network_balancing.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <cstdint>

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

TEMPLATE_TEST_CASE("Complex network balancing", "[network-balancing]", mockturtle::aig_network, mockturtle::mig_network,
                   mockturtle::xag_network, technology_network)
{
    const auto unify_outputs = GENERATE(false, true);
    CAPTURE(unify_outputs);
    const auto source   = blueprints::maj4_network<TestType>();
    const auto balanced = network_balancing<technology_network>(source, {.unify_outputs = unify_outputs});

    CHECK(is_balanced(balanced));
    CHECK(is_balanced(balanced, {.unify_outputs = true}) == unify_outputs);
    check_eq(source, balanced);
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

TEST_CASE("Constant primary outputs are not buffered when unifying outputs", "[network-balancing]")
{
    technology_network ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto a1 = ntk.create_and(x1, x2);
    const auto a2 = ntk.create_and(a1, x1);
    ntk.create_po(a2);
    ntk.create_po(ntk.get_constant(false));

    // by default, the constant output is buffered up to the output level like any other
    const auto buffered = network_balancing<technology_network>(ntk, {.unify_outputs = true});

    CHECK(is_balanced(buffered, {.unify_outputs = true}));
    CHECK(buffered.size() == ntk.size() + 1 + 2);

    const auto balanced =
        network_balancing<technology_network>(ntk, {.unify_outputs = true, .buffer_constant_outputs = false});

    CHECK(is_balanced(balanced, {.unify_outputs = true, .buffer_constant_outputs = false}));
    CHECK_FALSE(is_balanced(balanced, {.unify_outputs = true}));
    CHECK(balanced.num_pos() == 2);

    uint32_t constant_pos = 0;
    balanced.foreach_po(
        [&balanced, &constant_pos](const auto& po)
        {
            if (balanced.is_constant(balanced.get_node(po)))
            {
                ++constant_pos;
            }
        });
    CHECK(constant_pos == 1);

    // one buffer balances x1 into a2; the constant output receives none
    CHECK(balanced.size() == ntk.size() + 1);

    check_eq(ntk, balanced);
}
