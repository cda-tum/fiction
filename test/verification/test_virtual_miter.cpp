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
 * @brief Tests for `fiction/verification/virtual_miter.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/views/dont_care_view.hpp>

#include <optional>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::verification;

namespace
{

/**
 * Builds the virtual miter of two networks and checks it with SAT.
 *
 * @return The SAT result, or `std::nullopt` if the miter could not be built or SAT gave up.
 */
template <typename Spec, typename Impl>
std::optional<bool> virtual_miter_equivalent(const Spec& spec, const Impl& impl)
{
    const auto miter = virtual_miter<technology_network>(spec, impl);

    if (!miter.has_value())
    {
        return std::nullopt;
    }

    return mockturtle::equivalence_checking(*miter);
}

}  // namespace

TEST_CASE("Virtual miter different num_pis", "[virtual-miter]")
{
    virtual_pi_network<technology_network> vpi_ntk_1{};
    const auto                             x1_v = vpi_ntk_1.create_pi();
    const auto                             x2_v = vpi_ntk_1.create_pi();
    const auto                             v1   = vpi_ntk_1.create_pi();
    const auto                             a1_v = vpi_ntk_1.create_and(x1_v, x2_v);
    const auto                             o1_v = vpi_ntk_1.create_or(v1, x2_v);
    vpi_ntk_1.create_po(a1_v);
    vpi_ntk_1.create_po(o1_v);

    virtual_pi_network<technology_network> vpi_ntk_2{};
    const auto                             x1_v2 = vpi_ntk_2.create_pi();
    const auto                             x2_v2 = vpi_ntk_2.create_pi();
    const auto                             v1_2  = vpi_ntk_2.create_virtual_pi(x1_v2);
    const auto                             a1_v2 = vpi_ntk_2.create_and(v1_2, x2_v2);
    const auto                             o1_v2 = vpi_ntk_2.create_or(x1_v2, x2_v2);
    vpi_ntk_2.create_po(a1_v2);
    vpi_ntk_2.create_po(o1_v2);

    CHECK_FALSE(virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2).has_value());
}

TEMPLATE_TEST_CASE("Virtual miter with supported networks", "[virtual-miter]", technology_network,
                   mockturtle::aig_network, mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    TestType   test_ntk{};
    const auto x1 = test_ntk.create_pi();
    const auto x2 = test_ntk.create_pi();
    const auto a1 = test_ntk.create_and(x1, x2);
    const auto o1 = test_ntk.create_or(x1, x2);
    test_ntk.create_po(a1);
    test_ntk.create_po(o1);

    mockturtle::dont_care_view<TestType, false, true> const test_dc(test_ntk);

    virtual_pi_network<technology_network> vpi_ntk_1{};
    const auto                             x1_v = vpi_ntk_1.create_pi();
    const auto                             x2_v = vpi_ntk_1.create_pi();
    const auto                             v1   = vpi_ntk_1.create_virtual_pi(x1_v);
    const auto                             a1_v = vpi_ntk_1.create_and(x1_v, x2_v);
    const auto                             o1_v = vpi_ntk_1.create_or(v1, x2_v);
    vpi_ntk_1.create_po(a1_v);
    vpi_ntk_1.create_po(o1_v);

    virtual_pi_network<technology_network> vpi_ntk_2{};
    const auto                             x1_v2 = vpi_ntk_2.create_pi();
    const auto                             x2_v2 = vpi_ntk_2.create_pi();
    const auto                             v1_2  = vpi_ntk_2.create_virtual_pi(x1_v2);
    const auto                             a1_v2 = vpi_ntk_2.create_and(v1_2, x2_v2);
    const auto                             o1_v2 = vpi_ntk_2.create_or(x1_v2, x2_v2);
    vpi_ntk_2.create_po(a1_v2);
    vpi_ntk_2.create_po(o1_v2);

    SECTION("EXODC in the specification")
    {
        const auto equivalent = virtual_miter_equivalent(test_ntk, test_dc);
        REQUIRE(equivalent.has_value());
        CHECK(*equivalent);
    }
    SECTION("EXODC in the implementation")
    {
        const auto equivalent = virtual_miter_equivalent(test_dc, test_ntk);
        REQUIRE(equivalent.has_value());
        CHECK(*equivalent);
    }
    SECTION("Virtual PIs in the specification")
    {
        const auto equivalent = virtual_miter_equivalent(vpi_ntk_1, test_ntk);
        REQUIRE(equivalent.has_value());
        CHECK(*equivalent);
    }
    SECTION("Virtual PIs in the implementation")
    {
        const auto equivalent = virtual_miter_equivalent(test_ntk, vpi_ntk_2);
        REQUIRE(equivalent.has_value());
        CHECK(*equivalent);
    }
    SECTION("Virtual PIs in both networks")
    {
        const auto equivalent = virtual_miter_equivalent(vpi_ntk_1, vpi_ntk_2);
        REQUIRE(equivalent.has_value());
        CHECK(*equivalent);
    }
}

TEST_CASE("Virtual miter pairs the inputs of a rank view by creation order", "[virtual-miter]")
{
    // the second input drives nothing, so a rank view lists it after the third one
    technology_network tec{};

    const auto x1 = tec.create_pi();
    tec.create_pi();
    const auto x3 = tec.create_pi();
    tec.create_po(tec.create_and(x1, x3));
    tec.create_po(x3);

    virtual_pi_network<technology_network> vpi{};

    const auto y1 = vpi.create_pi();
    vpi.create_pi();
    const auto y3 = vpi.create_pi();
    const auto v1 = vpi.create_virtual_pi(y1);
    vpi.create_po(vpi.create_and(v1, y3));
    vpi.create_po(y3);

    const mutable_rank_view ranked{vpi};

    const auto cec = virtual_miter_equivalent(tec, ranked);
    REQUIRE(cec.has_value());
    CHECK(*cec);
}

TEST_CASE("Virtual miter rejects different output counts", "[virtual-miter]")
{
    technology_network left{}, right{};
    left.create_po(left.create_pi());
    right.create_pi();
    CHECK_FALSE(virtual_miter<technology_network>(left, right).has_value());
}

TEST_CASE("Virtual miter detects different output functions", "[virtual-miter]")
{
    technology_network left{}, right{};
    const auto         a = left.create_pi();
    const auto         b = left.create_pi();
    left.create_po(left.create_and(a, b));
    const auto x = right.create_pi();
    const auto y = right.create_pi();
    right.create_po(right.create_or(x, y));
    const auto equivalent = virtual_miter_equivalent(left, right);
    REQUIRE(equivalent.has_value());
    CHECK_FALSE(*equivalent);
}
