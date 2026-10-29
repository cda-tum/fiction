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
 * @brief Tests for `fiction/networks/views/static_depth_view.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/static_depth_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/synthesis/delete_virtual_pis.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/sequential.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/cost_functions.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;

/**
 * @brief Checks whether a network can form a depth view.
 * @tparam Ntk Network type to test.
 */
template <typename Ntk>
concept depth_viewable = requires { typename static_depth_view<Ntk>; };

/** @brief Network without indexed node storage access. */
struct depth_without_node_index : mockturtle::aig_network
{
    /** @brief Disables node indexing. @param n Node to inspect. */
    [[nodiscard]] uint32_t node_to_index(const node& n) const = delete;
};

/** @brief Network without combinational-input traversal. */
struct depth_without_ci_traversal : mockturtle::aig_network
{
    /** @brief Disables input traversal. @tparam Fn Callback type. */
    template <typename Fn>
    void foreach_ci(Fn&&) const = delete;
};

static_assert(!depth_viewable<depth_without_node_index>);
static_assert(!depth_viewable<depth_without_ci_traversal>);
static_assert(depth_viewable<mockturtle::aig_network>);
static_assert(!depth_viewable<int>);
static_assert(requires(const static_depth_view<mockturtle::aig_network>& ntk) { static_depth_view{ntk}; });

TEST_CASE("Depth view assignment preserves network names", "[static-depth-view]")
{
    static_depth_view<named_technology_network> source{};
    const auto                                  input = source.create_pi();
    source.create_po(input);
    source.set_network_name("source");
    source.set_name(input, "input");
    source.set_output_name(0, "output");
    source.update_levels();

    decltype(source) dest{};
    SECTION("Copy assignment")
    {
        dest = source;
        CHECK(dest.num_pis() == 1);
    }
    SECTION("Move assignment")
    {
        dest = std::move(source);
        CHECK(dest.num_pis() == 1);
    }

    REQUIRE(dest.num_pis() == 1);
    REQUIRE(dest.num_pos() == 1);
    CHECK(dest.get_network_name() == "source");
    CHECK(dest.get_name(dest.pi_at(0)) == "input");
    CHECK(dest.get_output_name(0) == "output");
    CHECK(dest.depth() == 0);
}

TEMPLATE_TEST_CASE("Depth views preserve network traits", "[static-depth-view]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network, mockturtle::klut_network,
                   technology_network)
{
    STATIC_REQUIRE(mockturtle::is_network_type_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_depth_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_level_v<TestType>);

    using depth_ntk = static_depth_view<TestType>;

    STATIC_REQUIRE(mockturtle::is_network_type_v<depth_ntk>);
    STATIC_REQUIRE(mockturtle::has_depth_v<depth_ntk>);
    STATIC_REQUIRE(mockturtle::has_level_v<depth_ntk>);

    using depth_depth_ntk = static_depth_view<depth_ntk>;

    STATIC_REQUIRE(mockturtle::is_network_type_v<depth_depth_ntk>);
    STATIC_REQUIRE(mockturtle::has_depth_v<depth_depth_ntk>);
    STATIC_REQUIRE(mockturtle::has_level_v<depth_depth_ntk>);
}

TEST_CASE("Update Levels Technology Network", "[static-depth-view]")
{
    static_depth_view<technology_network> tec_d;

    const auto x1_r = tec_d.create_pi();
    const auto x2_r = tec_d.create_pi();
    const auto a1_r = tec_d.create_and(x1_r, x2_r);
    tec_d.create_po(a1_r);
    tec_d.update_levels();

    CHECK(tec_d.level(2) == 0u);
    CHECK(tec_d.level(3) == 0u);
    CHECK(tec_d.level(4) == 1u);
    CHECK(tec_d.depth() == 1u);
}

TEST_CASE("Compute depth and levels for AIG", "[static-depth-view]")
{
    mockturtle::aig_network aig;
    const auto              a  = aig.create_pi();
    const auto              b  = aig.create_pi();
    const auto              f1 = aig.create_nand(a, b);
    const auto              f2 = aig.create_nand(a, f1);
    const auto              f3 = aig.create_nand(b, f1);
    const auto              f4 = aig.create_nand(f2, f3);
    aig.create_po(f4);

    SECTION("Unit gate costs")
    {
        const static_depth_view depth_aig{aig};
        CHECK(depth_aig.depth() == 3);
        CHECK(depth_aig.level(aig.get_node(a)) == 0);
        CHECK(depth_aig.level(aig.get_node(b)) == 0);
        CHECK(depth_aig.level(aig.get_node(f1)) == 1);
        CHECK(depth_aig.level(aig.get_node(f2)) == 2);
        CHECK(depth_aig.level(aig.get_node(f3)) == 2);
        CHECK(depth_aig.level(aig.get_node(f4)) == 3);
    }
    SECTION("Complemented edges add cost")
    {
        const static_depth_view depth_aig{aig, {}, depth_view_params{.count_complements = true}};
        CHECK(depth_aig.depth() == 6);
        CHECK(depth_aig.level(aig.get_node(a)) == 0);
        CHECK(depth_aig.level(aig.get_node(b)) == 0);
        CHECK(depth_aig.level(aig.get_node(f1)) == 1);
        CHECK(depth_aig.level(aig.get_node(f2)) == 3);
        CHECK(depth_aig.level(aig.get_node(f3)) == 3);
        CHECK(depth_aig.level(aig.get_node(f4)) == 5);
    }
}

TEST_CASE("Compute critical path information", "[static-depth-view]")
{
    mockturtle::aig_network aig;
    const auto              a = aig.create_pi();
    const auto              b = aig.create_pi();
    const auto              c = aig.create_pi();
    const auto              d = aig.create_pi();
    const auto              e = aig.create_pi();

    const auto f1 = aig.create_and(a, b);
    const auto f2 = aig.create_and(c, f1);
    const auto f3 = aig.create_and(d, e);
    const auto f  = aig.create_and(f2, f3);
    aig.create_po(f);

    const static_depth_view depth_aig{aig};
    STATIC_REQUIRE_FALSE(mockturtle::has_is_on_critical_path_v<decltype(aig)>);
    STATIC_REQUIRE(mockturtle::has_is_on_critical_path_v<decltype(depth_aig)>);
    CHECK(depth_aig.is_on_critical_path(aig.get_node(a)));
    CHECK(depth_aig.is_on_critical_path(aig.get_node(b)));
    CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(c)));
    CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(d)));
    CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(e)));
    CHECK(depth_aig.is_on_critical_path(aig.get_node(f1)));
    CHECK(depth_aig.is_on_critical_path(aig.get_node(f2)));
    CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(f3)));
    CHECK(depth_aig.is_on_critical_path(aig.get_node(f)));
}

TEST_CASE("Critical paths include complemented output edges", "[static-depth-view]")
{
    mockturtle::aig_network aig{};
    const auto              a          = aig.create_pi();
    const auto              b          = aig.create_pi();
    const auto              c          = aig.create_pi();
    const auto              short_gate = aig.create_and(a, b);
    const auto              middle     = aig.create_and(b, c);
    const auto              long_gate  = aig.create_and(middle, a);
    aig.create_po(!short_gate);
    aig.create_po(long_gate);

    const static_depth_view depth_aig{aig, {}, depth_view_params{.count_complements = true}};
    CHECK(depth_aig.depth() == 2);
    CHECK(depth_aig.is_on_critical_path(aig.get_node(short_gate)));
    CHECK(depth_aig.is_on_critical_path(aig.get_node(long_gate)));

    mockturtle::aig_network complemented{};
    const auto              input = complemented.create_pi();
    complemented.create_po(!input);
    const static_depth_view depth_complemented{complemented, {}, depth_view_params{.count_complements = true}};
    CHECK(depth_complemented.depth() == 1);
    CHECK(depth_complemented.is_on_critical_path(complemented.get_node(input)));
}

TEST_CASE("Critical paths follow explicit level and depth updates", "[static-depth-view]")
{
    mockturtle::aig_network aig{};
    const auto              a    = aig.create_pi();
    const auto              b    = aig.create_pi();
    const auto              gate = aig.create_and(a, b);
    aig.create_po(gate);
    static_depth_view depth_aig{aig};
    REQUIRE(depth_aig.is_on_critical_path(aig.get_node(a)));

    SECTION("Level update")
    {
        depth_aig.set_level(aig.get_node(gate), 2);
        CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(gate)));
    }
    SECTION("Depth update")
    {
        depth_aig.set_depth(2);
        CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(gate)));
    }
    SECTION("Added node")
    {
        const auto next = depth_aig.create_and(gate, a);
        depth_aig.create_po(next);
        depth_aig.on_add(depth_aig.get_node(next));
        CHECK(depth_aig.depth() == 2);
        CHECK(depth_aig.is_on_critical_path(depth_aig.get_node(next)));
    }
}

TEST_CASE("Critical paths include complemented register-input edges", "[static-depth-view]")
{
    mockturtle::sequential<mockturtle::aig_network> ntk{};
    const auto                                      input = ntk.create_ro();
    ntk.create_ri(!input);
    const static_depth_view depth_ntk{ntk, {}, depth_view_params{.count_complements = true}};
    CHECK(depth_ntk.depth() == 1);
    CHECK(depth_ntk.is_on_critical_path(ntk.get_node(input)));
}

TEST_CASE("Added nodes include existing complemented output edges in depth", "[static-depth-view]")
{
    mockturtle::sequential<mockturtle::aig_network> ntk{};
    const auto                                      a = ntk.create_pi();
    const auto                                      b = ntk.create_pi();
    ntk.create_po(a);
    static_depth_view depth_ntk{ntk, {}, depth_view_params{.count_complements = true}};
    REQUIRE(depth_ntk.is_on_critical_path(depth_ntk.get_node(a)));
    const auto gate = depth_ntk.create_and(a, b);
    SECTION("Primary output")
    {
        depth_ntk.create_po(!gate);
        CHECK(depth_ntk.num_pos() == 2);
    }
    SECTION("Register input")
    {
        depth_ntk.create_ro();
        depth_ntk.create_ri(!gate);
        CHECK(depth_ntk.num_registers() == 1);
    }
    depth_ntk.on_add(depth_ntk.get_node(gate));
    CHECK(depth_ntk.depth() == 2);
    CHECK(depth_ntk.is_on_critical_path(depth_ntk.get_node(gate)));
}

TEST_CASE("The critical path follows the levels after an update and survives a copy", "[static-depth-view]")
{
    static_depth_view<technology_network> tec_d;

    const auto x1 = tec_d.create_pi();
    const auto x2 = tec_d.create_pi();
    const auto x3 = tec_d.create_pi();
    const auto g1 = tec_d.create_and(x1, x2);
    tec_d.create_po(g1);
    tec_d.update_levels();

    CHECK(tec_d.is_on_critical_path(tec_d.get_node(g1)));
    CHECK(tec_d.is_on_critical_path(tec_d.get_node(x1)));
    CHECK_FALSE(tec_d.is_on_critical_path(tec_d.get_node(x3)));

    // a deeper output extends the critical path; the input that joins late stays off it
    const auto g2 = tec_d.create_not(tec_d.create_and(g1, x3));
    tec_d.create_po(g2);
    tec_d.update_levels();

    CHECK(tec_d.is_on_critical_path(tec_d.get_node(g2)));
    CHECK(tec_d.is_on_critical_path(tec_d.get_node(g1)));
    CHECK(tec_d.is_on_critical_path(tec_d.get_node(x1)));
    CHECK_FALSE(tec_d.is_on_critical_path(tec_d.get_node(x3)));

    // a copy keeps the answers, whether the path was computed before the copy or not
    const auto computed_copy = tec_d;

    tec_d.update_levels();

    const auto pending_copy = tec_d;

    for (const auto& copy : {computed_copy, pending_copy})
    {
        CHECK(copy.is_on_critical_path(copy.get_node(g2)));
        CHECK(copy.is_on_critical_path(copy.get_node(x1)));
        CHECK_FALSE(copy.is_on_critical_path(copy.get_node(x3)));
    }
}

TEST_CASE("Compute levels during node construction", "[static-depth-view]")
{
    static_depth_view<mockturtle::xag_network> dxag;

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();

    dxag.create_po(dxag.create_xor(b, dxag.create_and(dxag.create_xor(a, b), dxag.create_xor(b, c))));

    dxag.update_levels();

    CHECK(dxag.depth() == 3u);
}

TEST_CASE("Compute levels during node construction with cost function", "[static-depth-view]")
{
    const mockturtle::xag_network                                                            xag;
    static_depth_view<mockturtle::xag_network, mockturtle::mc_cost<mockturtle::xag_network>> dxag{xag};

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();

    dxag.create_po(dxag.create_xor(b, dxag.create_and(dxag.create_xor(a, b), dxag.create_xor(b, c))));

    dxag.update_levels();

    CHECK(dxag.depth() == 1u);
}

TEST_CASE("Assigned depth views support construction after the source dies", "[static-depth-view]")
{
    static_depth_view<mockturtle::xag_network> dxag{};
    {
        static_depth_view<mockturtle::xag_network> source{mockturtle::xag_network{}};
        const auto                                 input = source.create_pi();
        source.create_po(input);
        SECTION("Copy assignment")
        {
            dxag = source;
            CHECK(dxag.num_pis() == 1);
            CHECK(dxag.num_pos() == 1);
        }
        SECTION("Move assignment")
        {
            dxag = std::move(source);
            CHECK(dxag.num_pis() == 1);
            CHECK(dxag.num_pos() == 1);
        }
    }

    const auto a = dxag.create_pi();
    const auto b = dxag.create_pi();
    const auto c = dxag.create_pi();
    dxag.create_po(dxag.create_xor(b, dxag.create_and(dxag.create_xor(a, b), dxag.create_xor(b, c))));
    dxag.update_levels();
    CHECK(dxag.depth() == 3u);
}

TEST_CASE("Dangling nodes have no level", "[static-depth-view]")
{
    mockturtle::aig_network aig;
    const auto              a  = aig.create_pi();
    const auto              b  = aig.create_pi();
    const auto              f1 = aig.create_nand(a, b);
    const auto              f2 = aig.create_nand(a, f1);
    aig.create_po(f2);

    const static_depth_view depth_aig{aig};

    CHECK(depth_aig.has_level(aig.get_node(a)));
    CHECK(depth_aig.has_level(aig.get_node(f1)));
    CHECK(depth_aig.has_level(aig.get_node(f2)));

    // networks share their storage on copy; clone to keep `aig` untouched
    auto       dangling_aig = aig.clone();
    const auto f3           = dangling_aig.create_nand(b, f1);

    const static_depth_view depth_dangling{dangling_aig};

    CHECK(depth_dangling.has_level(dangling_aig.get_node(f2)));
    CHECK_FALSE(depth_dangling.has_level(dangling_aig.get_node(f3)));
    CHECK(depth_dangling.depth() == 2);

    // a depth view over a depth view passes the question through instead of claiming a level for every node
    // (the template argument is spelled out, since class template argument deduction would pick the copy)
    const static_depth_view<static_depth_view<mockturtle::aig_network>> nested{depth_dangling};

    CHECK(nested.has_level(dangling_aig.get_node(f2)));
    CHECK_FALSE(nested.has_level(dangling_aig.get_node(f3)));

    // a depth view over mockturtle's depth view answers true, since that view keeps levels for every node
    const static_depth_view over_mockturtle{mockturtle::depth_view{aig}};

    CHECK(over_mockturtle.has_level(aig.get_node(f2)));
}

TEST_CASE("Primary inputs without fanout have level 0", "[static-depth-view]")
{
    mockturtle::aig_network aig;
    const auto              a      = aig.create_pi();
    const auto              unused = aig.create_pi();
    const auto              b      = aig.create_pi();
    const auto              f      = aig.create_and(a, b);
    aig.create_po(f);

    const static_depth_view depth_aig{aig};

    // an input that drives nothing is not reached from any output but belongs to the interface; it sits on level 0
    CHECK(depth_aig.has_level(aig.get_node(unused)));
    CHECK(depth_aig.level(aig.get_node(unused)) == 0);
    CHECK(depth_aig.level(aig.get_node(f)) == 1);
    CHECK(depth_aig.depth() == 1);
    CHECK_FALSE(depth_aig.is_on_critical_path(aig.get_node(unused)));

    // the same holds after recomputing the levels
    static_depth_view updated{aig};
    updated.update_levels();

    CHECK(updated.has_level(aig.get_node(unused)));
    CHECK(updated.level(aig.get_node(unused)) == 0);
}

/** @brief Unit node costs that support copying and forbid moving. */
struct copy_only_depth_cost : mockturtle::unit_cost<technology_network>
{
    /** @brief Constructs unit costs. */
    copy_only_depth_cost() = default;
    /** @brief Copies unit costs. */
    copy_only_depth_cost(const copy_only_depth_cost&) = default;
    /** @brief Forbids move construction. */
    copy_only_depth_cost(copy_only_depth_cost&&) = delete;
    /** @brief Copies unit costs. */
    copy_only_depth_cost& operator=(const copy_only_depth_cost&) = default;
    /** @brief Forbids move assignment. */
    copy_only_depth_cost& operator=(copy_only_depth_cost&&) = delete;
    /** @brief Destroys unit costs. */
    ~copy_only_depth_cost() = default;
};

/** @brief Unit node costs whose move construction throws. */
struct throwing_depth_cost : mockturtle::unit_cost<technology_network>
{
    /** @brief Constructs unit costs. */
    throwing_depth_cost() = default;
    /** @brief Copies unit costs. */
    throwing_depth_cost(const throwing_depth_cost&) = default;
    /** @brief Reports move failure. @throws std::runtime_error Always. */
    throwing_depth_cost(throwing_depth_cost&&)
    {
        throw std::runtime_error{"cost move"};
    }
    /** @brief Copies unit costs. */
    throwing_depth_cost& operator=(const throwing_depth_cost&) = default;
    /** @brief Assigns unit costs from an rvalue. */
    throwing_depth_cost& operator=(throwing_depth_cost&&) = default;
    /** @brief Destroys unit costs. */
    ~throwing_depth_cost() = default;
};

TEMPLATE_TEST_CASE("Depth views support copy-only and throwing cost construction", "[static-depth-view]",
                   copy_only_depth_cost, throwing_depth_cost)
{
    /** @brief Depth view with the cost policy under test. */
    using depth_ntk         = static_depth_view<virtual_pi_network<technology_network>, TestType>;
    const auto delete_alias = GENERATE(false, true);
    CAPTURE(delete_alias);

    depth_ntk  source{};
    const auto a     = source.create_pi();
    const auto b     = source.create_pi();
    const auto alias = source.create_virtual_pi(a);
    source.create_po(source.create_and(alias, b));
    source.update_levels();

    try
    {
        /** @brief Constructs a view from an rvalue or removes virtual inputs. */
        const auto construct_result = [&]
        {
            if (delete_alias)
            {
                return synthesis::delete_virtual_pis(source);
            }
            auto copy = source;
            return depth_ntk{std::move(copy)};
        };
        auto result = construct_result();
        result.update_levels();
        const auto inputs = delete_alias ? 2u : 3u;
        REQUIRE(result.num_pis() == inputs);
        CHECK(result.num_virtual_pis() == (delete_alias ? 0u : 1u));
        REQUIRE(result.num_pos() == 1);
        CHECK(result.depth() == 1);
        kitty::dynamic_truth_table expected{inputs};
        kitty::create_from_hex_string(expected, delete_alias ? "8" : "c0");
        CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(
                  result, mockturtle::default_simulator<kitty::dynamic_truth_table>{inputs})
                  .at(0) == expected);
    }
    catch (const std::runtime_error&)
    {
        CHECK(std::is_same_v<TestType, throwing_depth_cost>);
    }

    REQUIRE(source.num_pis() == 3);
    CHECK(source.num_virtual_pis() == 1);
    REQUIRE(source.num_pos() == 1);
    CHECK(source.depth() == 1);
    kitty::dynamic_truth_table source_function{3};
    kitty::create_from_hex_string(source_function, "c0");
    CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(source,
                                                           mockturtle::default_simulator<kitty::dynamic_truth_table>{3})
              .at(0) == source_function);
}
