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

    mockturtle::equivalence_checking_stats st{};

    return mockturtle::equivalence_checking(*miter, {}, &st);
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

    auto miter_network = virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2);
    CHECK(!miter_network.has_value());
}

TEST_CASE("Virtual miter with technology networks", "[virtual-miter]")
{
    technology_network tec{};
    const auto         x1 = tec.create_pi();
    const auto         x2 = tec.create_pi();
    const auto         a1 = tec.create_and(x1, x2);
    const auto         o1 = tec.create_or(x1, x2);
    tec.create_po(a1);
    tec.create_po(o1);

    mockturtle::dont_care_view<technology_network, false, true> const tec_dc(tec);

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

    // check for the exodc path 1
    auto maybe_cec_m = virtual_miter_equivalent(tec, tec_dc);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the exodc path 2
    maybe_cec_m = virtual_miter_equivalent(tec_dc, tec);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 1
    maybe_cec_m = virtual_miter_equivalent(vpi_ntk_1, tec);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 2
    maybe_cec_m = virtual_miter_equivalent(tec, vpi_ntk_2);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 3
    maybe_cec_m = virtual_miter_equivalent(vpi_ntk_1, vpi_ntk_2);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
}

TEMPLATE_TEST_CASE("Virtual miter with mockturtle networks", "[virtual-miter]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
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

    // check for the exodc path 1
    auto maybe_cec_m = virtual_miter_equivalent(test_ntk, test_dc);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the exodc path 2
    maybe_cec_m = virtual_miter_equivalent(test_dc, test_ntk);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 1
    maybe_cec_m = virtual_miter_equivalent(vpi_ntk_1, test_ntk);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 2
    maybe_cec_m = virtual_miter_equivalent(test_ntk, vpi_ntk_2);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
    }
    // check for the handle virtual pi path 3
    maybe_cec_m = virtual_miter_equivalent(vpi_ntk_1, vpi_ntk_2);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(maybe_cec_m.value_or(false));
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

    const auto miter = virtual_miter<technology_network>(tec, ranked);
    REQUIRE(miter.has_value());

    mockturtle::equivalence_checking_stats st{};
    const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);
    REQUIRE(cec.has_value());
    CHECK(*cec);
}
