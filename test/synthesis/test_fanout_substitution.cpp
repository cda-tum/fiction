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
 * @brief Tests for `fiction/synthesis/fanout_substitution.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;

/**
 * @brief Checks the interface, fanout degrees, and Boolean behavior of a substituted network.
 * @tparam Ntk Source logic network type.
 * @param ntk Source logic network.
 * @param ps Fanout substitution parameters.
 * @return Substituted network.
 */
template <typename Ntk>
technology_network substitute(const Ntk& ntk, const fanout_substitution_params& ps)
{
    const auto substituted = fanout_substitution<technology_network>(ntk, ps);

    CHECK(substituted.num_pis() == ntk.num_pis());
    CHECK(substituted.num_pos() == ntk.num_pos());

    mockturtle::node_map<uint32_t, technology_network> fanouts{substituted, 0u};
    substituted.foreach_gate(
        [&](const auto node)
        { substituted.foreach_fanin(node, [&](const auto signal) { ++fanouts[substituted.get_node(signal)]; }); });
    substituted.foreach_po([&](const auto signal) { ++fanouts[substituted.get_node(signal)]; });
    substituted.foreach_node(
        [&](const auto node)
        {
            if (substituted.is_constant(node))
            {
                return;
            }
            CAPTURE(node, ps.degree, ps.threshold);
            CHECK(substituted.fanout_size(node) == fanouts[node]);
            CHECK(fanouts[node] <= (substituted.is_buf(node) ? ps.degree : ps.threshold));
        });
    CHECK(is_fanout_substituted(substituted, ps));

    check_eq(ntk, substituted);
    return substituted;
}

TEST_CASE("Simple fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::multi_output_and_network<technology_network>();

    const fanout_substitution_params ps_depth{.strategy = fanout_substitution_params::substitution_strategy::DEPTH};
    const fanout_substitution_params ps_breadth{.strategy = fanout_substitution_params::substitution_strategy::BREADTH};
    const fanout_substitution_params ps_random{.strategy = fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth);
    substitute(tec, ps_breadth);
    substitute(tec, ps_random);
}

TEST_CASE("Complex fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::maj4_network<technology_network>();
    CHECK_FALSE(is_fanout_substituted(tec));

    const fanout_substitution_params ps_depth{.strategy = fanout_substitution_params::substitution_strategy::DEPTH};
    const fanout_substitution_params ps_breadth{.strategy = fanout_substitution_params::substitution_strategy::BREADTH};
    const fanout_substitution_params ps_random{.strategy = fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth);
    substitute(tec, ps_breadth);
    substitute(tec, ps_random);

    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    CHECK_FALSE(is_fanout_substituted(aig));
    substitute(aig, ps_depth);
    substitute(aig, ps_breadth);
    substitute(aig, ps_random);
}

TEST_CASE("Name conservation after fanout substitution", "[fanout-substitution]")
{
    auto maj = blueprints::maj1_network<named_mig_network>();
    maj.set_network_name("maj");

    const auto substituted_maj = fanout_substitution<named_technology_network>(maj);

    // network name
    CHECK(substituted_maj.get_network_name() == "maj");

    // PI names
    REQUIRE(substituted_maj.num_pis() == 3u);
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(substituted_maj.pi_at(0))) == "a");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(substituted_maj.pi_at(1))) == "b");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(substituted_maj.pi_at(2))) == "c");

    // PO names
    CHECK(substituted_maj.get_output_name(0) == "f");
}

TEST_CASE("Degree and threshold in fanout substitution", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    const fanout_substitution_params ps_31{.strategy  = fanout_substitution_params::substitution_strategy::BREADTH,
                                           .degree    = 3,
                                           .threshold = 1};
    const fanout_substitution_params ps_22{.strategy  = fanout_substitution_params::substitution_strategy::DEPTH,
                                           .degree    = 2,
                                           .threshold = 2};
    const fanout_substitution_params ps_32{.strategy  = fanout_substitution_params::substitution_strategy::RANDOM,
                                           .degree    = 3,
                                           .threshold = 2};

    substitute(aig, ps_31);
    substitute(aig, ps_22);
    substitute(aig, ps_32);
}

TEST_CASE("Consistent fanout substitution after balancing", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = fanout_substitution<technology_network>(aig);

    CHECK(is_fanout_substituted(substituted));
    auto balanced = network_balancing<technology_network>(substituted);
    CHECK(is_fanout_substituted(balanced));
    check_eq(aig, balanced);

    auto tec = blueprints::fanout_substitution_corner_case_network<technology_network>();

    auto substituted_tec = fanout_substitution<technology_network>(tec);
    CHECK(is_fanout_substituted(substituted_tec));
    auto balanced_tec = network_balancing<technology_network>(substituted_tec);
    CHECK(is_fanout_substituted(balanced_tec));
    check_eq(tec, balanced_tec);
}

TEST_CASE("Repeated inputs respect fanout degrees below the source threshold", "[fanout-substitution]")
{
    using strategy                    = fanout_substitution_params::substitution_strategy;
    const auto                 choice = GENERATE(strategy::DEPTH, strategy::BREADTH, strategy::RANDOM);
    technology_network         original{};
    const auto                 a = original.create_pi();
    kitty::dynamic_truth_table parity{5};
    kitty::create_from_hex_string(parity, "96696996");
    original.create_po(original.create_node({a, a, a, a, a}, parity));
    const fanout_substitution_params ps{.strategy = choice, .degree = 2, .threshold = 3, .seed = 42};
    substitute(original, ps);
}

TEST_CASE("Fanout substitution rejects degrees and thresholds that cannot branch", "[fanout-substitution]")
{
    const auto params = GENERATE(fanout_substitution_params{.degree = 0}, fanout_substitution_params{.degree = 1},
                                 fanout_substitution_params{.threshold = 0});
    CHECK_THROWS_AS(fanout_substitution<technology_network>(technology_network{}, params), std::invalid_argument);
}

TEST_CASE("Fanout substitution preserves uint32 parameter bounds", "[fanout-substitution]")
{
    using strategy            = fanout_substitution_params::substitution_strategy;
    const auto         choice = GENERATE(strategy::DEPTH, strategy::BREADTH, strategy::RANDOM);
    technology_network original{};
    const auto         a = original.create_pi();
    const auto         b = original.create_pi();
    original.create_po(original.create_and(a, b));
    original.create_po(original.create_or(a, b));
    original.create_po(original.create_xor(a, b));
    fanout_substitution_params params{.strategy = choice, .seed = 42};
    if (GENERATE(false, true))
    {
        params.threshold = std::numeric_limits<uint32_t>::max();
    }
    else
    {
        params.degree = std::numeric_limits<uint32_t>::max();
    }
    substitute(original, params);
}

TEST_CASE("Fanout substitution preserves logic when the destination elides buffers", "[fanout-substitution]")
{
    using strategy                            = fanout_substitution_params::substitution_strategy;
    const auto                       choice   = GENERATE(strategy::DEPTH, strategy::BREADTH, strategy::RANDOM);
    const auto                       original = blueprints::multi_output_and_network<mockturtle::aig_network>();
    const fanout_substitution_params params{.strategy = choice, .seed = 42};
    const auto                       substituted = fanout_substitution<mockturtle::aig_network>(original, params);
    check_eq(original, substituted);
}

TEST_CASE("Random fanout substitution with fixed vs. varying seeds", "[fanout-substitution]")
{
    const auto aig         = blueprints::maj4_network<mockturtle::aig_network>();
    const auto connections = [](const technology_network& ntk)
    {
        std::vector<std::vector<technology_network::signal>> result{};
        ntk.foreach_node(
            [&](const auto node)
            {
                result.emplace_back();
                ntk.foreach_fanin(node, [&](const auto signal) { result.back().push_back(signal); });
            });
        result.emplace_back();
        ntk.foreach_po([&](const auto signal) { result.back().push_back(signal); });
        return result;
    };
    SECTION("Fixed seeds repeat the routing")
    {
        const auto seed = GENERATE(1u, 2u, 20u, 42u);
        CAPTURE(seed);
        const fanout_substitution_params params{.strategy = fanout_substitution_params::substitution_strategy::RANDOM,
                                                .seed     = seed};
        const auto                       substituted = substitute(aig, params);
        const auto                       repeated    = substitute(aig, params);
        CHECK(connections(substituted) == connections(repeated));
    }
    SECTION("Varying seeds preserve the network contract")
    {
        const auto seed = GENERATE(Catch::Generators::range(1u, 21u));
        CAPTURE(seed);
        const fanout_substitution_params params{.strategy = fanout_substitution_params::substitution_strategy::RANDOM,
                                                .seed     = seed};
        substitute(aig, params);
    }
}

TEST_CASE("Repeated fanout substitution preserves interface and logic", "[fanout-substitution]")
{
    const auto aig         = blueprints::maj4_network<mockturtle::aig_network>();
    auto       substituted = substitute(aig, {});
    for (auto pass = 0u; pass < 3u; ++pass)
    {
        CAPTURE(pass);
        substituted = substitute(substituted, {});
    }
}

TEST_CASE("Fanout checks accept progress callbacks whose copies throw", "[fanout-substitution]")
{
    /** @brief Injects a copy failure without throwing from progress reports. */
    struct throwing_copy_progress
    {
        /** @brief Controls whether copying the callback fails. */
        const bool& fail_copy;
        /** @brief Stores the copy-failure flag. */
        explicit throwing_copy_progress(const bool& fail) noexcept : fail_copy{fail} {}
        /** @brief Throws after copy-failure injection is armed. */
        throwing_copy_progress(const throwing_copy_progress& other) : fail_copy{other.fail_copy}
        {
            if (fail_copy)
            {
                throw std::bad_alloc{};
            }
        }
        /** @brief Moves the callback without throwing. */
        throwing_copy_progress(throwing_copy_progress&&) noexcept = default;
        /** @brief Destroys the callback. */
        ~throwing_copy_progress() = default;
        /** @brief The copy-failure flag cannot be rebound. */
        throwing_copy_progress& operator=(const throwing_copy_progress&) = delete;
        /** @brief The copy-failure flag cannot be rebound. */
        throwing_copy_progress& operator=(throwing_copy_progress&&) = delete;
        /** @brief Accepts progress reports without throwing. */
        void operator()(std::string_view, std::size_t, std::size_t) const noexcept {}
    };

    const auto [degree, threshold, expected] =
        GENERATE(std::tuple{3u, 2u, true}, std::tuple{2u, 2u, false}, std::tuple{3u, 1u, false});
    CAPTURE(degree, threshold);
    technology_network network{};
    const auto         a      = network.create_pi();
    const auto         buffer = network.create_buf(a);
    network.create_po(a);
    network.create_po(buffer);
    network.create_po(buffer);
    network.create_po(buffer);
    bool                       fail_copy{};
    fanout_substitution_params params{.strategy    = fanout_substitution_params::substitution_strategy::RANDOM,
                                      .degree      = degree,
                                      .threshold   = threshold,
                                      .seed        = 42,
                                      .on_progress = throwing_copy_progress{fail_copy}};
    fail_copy = true;
    CHECK(is_fanout_substituted(network, std::move(params)) == expected);
}
