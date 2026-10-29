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
 * @brief Tests for names transferred between networks and placed layouts.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/networks/name_utils.hpp>
#include <fiction/physical_design/placement_utils.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;

/** @brief Named network missing node traversal required for signal-name transfer. */
struct incomplete_named_network : named_aig_network
{
    /**
     * @brief Disables node traversal.
     * @tparam Fn Node callback type.
     */
    template <typename Fn>
    void foreach_node(Fn&&) const = delete;
};

/** @brief Named network without constant lookup. */
struct named_without_constant : named_aig_network
{
    /** @brief Creates an empty named network. */
    named_without_constant() = default;
    /** @brief Disables constant lookup. @param value Constant value. @return Constant signal. */
    [[nodiscard]] signal get_constant(bool value) const = delete;
};

/**
 * @brief Checks signal-name transfer and its forwarding wrapper.
 * @tparam Src Source network type.
 * @tparam Dest Destination network type.
 */
template <typename Src, typename Dest>
concept signal_names_transferable =
    requires(const Src& src, Dest& dest, mockturtle::node_map<mockturtle::aig_network::signal, Src>& map) {
        restore_signal_names(src, dest, map);
    };

/**
 * @brief Checks whether the full name-transfer wrapper accepts a source and destination.
 * @tparam Src Source network type.
 * @tparam Dest Destination network type.
 */
template <typename Src, typename Dest>
concept all_names_transferable =
    requires(const Src& src, Dest& dest, mockturtle::node_map<mockturtle::aig_network::signal, Src>& map) {
        restore_names(src, dest, map);
    };

/** @brief Native layout without an input-name query. */
struct layout_without_input_name_query : cartesian_gate_layout
{
    /** @brief Disables the input-name query. @param id Input object. */
    [[nodiscard]] bool has_name(object_id id) const = delete;
};

/** @brief Named input source without input traversal. */
struct input_source_without_traversal : named_aig_network
{
    /** @brief Disables input traversal. @tparam Fn Input callback type. */
    template <typename Fn>
    void foreach_pi(Fn&&) const = delete;
};

/**
 * @brief Checks source input-name restoration.
 * @tparam Src Source network or layout type.
 * @tparam Dest Destination network or layout type.
 */
template <typename Src, typename Dest = named_technology_network>
concept source_input_names_transferable = requires(const Src& src, Dest& dest) { restore_input_names(src, dest); };

static_assert(!source_input_names_transferable<layout_without_input_name_query>);
static_assert(!source_input_names_transferable<input_source_without_traversal>);
static_assert(source_input_names_transferable<input_source_without_traversal, mockturtle::aig_network>);

/** @brief Native layout without an input-name setter. */
struct layout_without_input_names : cartesian_gate_layout
{
    /** @brief Disables input-name assignment. @param id Input object. @param name Input name. */
    void set_name(object_id id, const std::string& name) = delete;
};

/** @brief Native layout with unsupported native naming and an unrelated network-style setter. */
struct dual_interface_without_input_names : layout_without_input_names
{
    /** @brief Network-style signal type. */
    using signal = mockturtle::aig_network::signal;
    /** @brief Keeps the native setter in overload lookup. */
    using layout_without_input_names::set_name;
    /** @brief Converts an input object for the unrelated network-style API. @param id Input object. @return Signal. */
    [[nodiscard]] signal make_signal(object_id id) const;
    /** @brief Accepts network-style names. @param input Input signal. @param name Input name. */
    void set_name(signal input, const std::string& name);
};

/** @brief Checks input-name restoration. @tparam Dest Destination network or layout type. */
template <typename Dest>
concept input_names_transferable =
    requires(const named_technology_network& src, Dest& dest) { restore_input_names(src, dest); };

static_assert(input_names_transferable<cartesian_gate_layout>);
static_assert(input_names_transferable<mockturtle::aig_network>);
static_assert(!input_names_transferable<layout_without_input_names>);
static_assert(!input_names_transferable<dual_interface_without_input_names>);

/** @brief Native layout without an output-name setter. */
struct layout_without_output_names : cartesian_gate_layout
{
    /** @brief Disables output-name assignment. @param index Output index. @param name Output name. */
    void set_output_name(uint32_t index, const std::string& name) = delete;
};

/** @brief Checks output-name restoration. @tparam Dest Destination network or layout type. */
template <typename Dest>
concept output_names_transferable =
    requires(const named_technology_network& src, Dest& dest) { restore_output_names(src, dest); };

static_assert(output_names_transferable<cartesian_gate_layout>);
static_assert(output_names_transferable<mockturtle::aig_network>);
static_assert(!output_names_transferable<layout_without_output_names>);

/**
 * @brief Checks branching name transfers with native layout endpoints.
 * @tparam Dest Destination network or layout type.
 */
template <typename Dest>
concept branching_names_transferable =
    requires(const named_technology_network& src, Dest& dest,
             const mockturtle::node_map<physical_design::branching_signal_container<>, named_technology_network>& map) {
        restore_signal_names(src, dest, map);
    };

static_assert(branching_names_transferable<cartesian_gate_layout>);
static_assert(branching_names_transferable<mockturtle::aig_network>);
static_assert(!branching_names_transferable<named_aig_network>);

static_assert(!all_names_transferable<incomplete_named_network, named_aig_network>);
static_assert(signal_names_transferable<mockturtle::aig_network, mockturtle::aig_network>);
static_assert(signal_names_transferable<incomplete_named_network, mockturtle::aig_network>);
static_assert(!signal_names_transferable<incomplete_named_network, named_aig_network>);

static_assert(signal_names_transferable<named_without_constant, named_aig_network>);

TEST_CASE("Signal names transfer without constant lookup", "[name-utils]")
{
    named_without_constant source{};
    const auto             input = source.create_pi("input");
    source.create_po(input);
    named_aig_network                                                             destination{};
    const auto                                                                    target = destination.create_pi();
    mockturtle::node_map<mockturtle::aig_network::signal, named_without_constant> mapping{source};
    mapping[source.get_node(input)] = target;
    restore_signal_names(source, destination, mapping);
    CHECK(destination.get_name(target) == "input");
}

TEST_CASE("Unnamed network name transfers clear names and remain optional", "[name-utils]")
{
    mockturtle::aig_network unnamed{};
    named_aig_network       named{};
    named.set_network_name("old");
    restore_network_name(unnamed, named);
    CHECK(named.get_network_name().empty());
    named.set_network_name("source");
    restore_names(named, unnamed);
    CHECK(get_name(unnamed).empty());
    restore_names(unnamed, named);
    CHECK(named.get_network_name().empty());
}

TEMPLATE_TEST_CASE("Name transfer respects interface order and string-view bounds", "[name-utils]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    named_technology_network ntk{};
    const auto               input = ntk.create_pi("a");
    ntk.create_po(input, "f");
    set_name(ntk, std::string_view{"prefix suffix", 6});

    auto       lyt    = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {2, 1});
    const auto placed = lyt.create_pi("", {0, 0});
    lyt.create_po(placed, "", {1, 0});
    restore_names(ntk, lyt);
    CHECK(lyt.get_layout_name() == "prefix");
    CHECK(lyt.get_input_name(0) == "a");
    CHECK(lyt.get_output_name(0) == "f");

    named_technology_network copied{};
    copied.create_po(copied.create_pi());
    restore_names(lyt, copied);
    CHECK(copied.get_network_name() == "prefix");
    CHECK(copied.get_name(copied.make_signal(copied.pi_at(0))) == "a");
    CHECK(copied.get_output_name(0) == "f");
}

TEMPLATE_TEST_CASE("Signal names transfer from a logic network to placed objects", "[name-utils]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    /** Named logic source with an internal signal and a declared output. */
    named_technology_network ntk{};
    /** Named primary input signal. */
    const auto input = ntk.create_pi("input");
    /** Named internal signal. */
    const auto internal = ntk.create_not(input);
    ntk.set_name(internal, "internal");
    /** Unnamed sink makes the internal signal a gate input. */
    const auto sink = ntk.create_buf(internal);
    ntk.create_po(sink, "output");
    /** Native target with separate placed terminals. */
    auto lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 1});
    /** Placed primary input. */
    const auto placed_input = lyt.create_pi("", {0, 0});
    /** Placed internal gate. */
    const auto placed_internal = lyt.create_not(placed_input, {1, 0});
    /** Separately named output terminal. */
    const auto placed_sink = lyt.create_buf(placed_internal, {2, 0});
    /** Named output terminal independent of the driver name. */
    const auto placed_output = lyt.create_po(placed_sink, "terminal", {3, 0});
    /** Source-node map with native output endpoints as its values. */
    mockturtle::node_map<typename decltype(lyt)::object_id, decltype(ntk)> mapping{ntk};
    mapping[ntk.get_node(input)]    = placed_input;
    mapping[ntk.get_node(internal)] = placed_internal;
    mapping[ntk.get_node(sink)]     = placed_sink;

    restore_signal_names(ntk, lyt, mapping);

    CHECK(lyt.get_name(placed_input) == "input");
    CHECK(lyt.get_name(placed_internal) == "internal");
    CHECK(lyt.get_name(placed_output) == "terminal");
}

TEMPLATE_TEST_CASE("Signal name restoration covers output drivers and mapped unused inputs", "[name-utils]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    /** Logic source with a named output driver and unused inputs. */
    named_technology_network ntk{};
    /** Connected primary input. */
    const auto input = ntk.create_pi("input");
    /** Unused input with a target mapping. */
    const auto unused = ntk.create_pi("unused");
    ntk.create_pi("unmapped");
    /** Named gate driving only a primary output. */
    const auto driver = ntk.create_not(input);
    ntk.set_name(driver, "driver");
    ntk.create_po(driver, "output");
    /** Placed target with an independent output terminal name. */
    auto lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 1});
    /** Placed connected input. */
    const auto placed_input = lyt.create_pi("", {0, 0});
    /** Placed output driver. */
    const auto placed_driver = lyt.create_not(placed_input, {1, 0});
    /** Placed output terminal. */
    const auto placed_output = lyt.create_po(placed_driver, "terminal", {2, 0});
    /** Placed unused input. */
    const auto placed_unused = lyt.create_pi("", {3, 0});
    /** Mapped native endpoints; unmapped nodes retain an absent default endpoint. */
    mockturtle::node_map<typename decltype(lyt)::object_id, decltype(ntk)> mapping{ntk};
    mapping[ntk.get_node(input)]  = placed_input;
    mapping[ntk.get_node(driver)] = placed_driver;
    mapping[ntk.get_node(unused)] = placed_unused;

    SECTION("Live and absent native mappings")
    {
        CHECK_NOTHROW(restore_signal_names(ntk, lyt, mapping));
        CHECK(lyt.get_name(placed_driver) == "driver");
        CHECK(lyt.get_name(placed_unused) == "unused");
        CHECK(lyt.get_name(placed_output) == "terminal");
    }
    SECTION("Stale native mappings reject")
    {
        lyt.remove(placed_unused);
        CHECK_THROWS_AS(restore_signal_names(ntk, lyt, mapping), std::invalid_argument);
    }
}

TEST_CASE("Signal name restoration retains used complemented names", "[name-utils]")
{
    /** Source with a separately named complemented signal. */
    named_aig_network source{};
    /** Source input whose complement carries a name. */
    const auto input = source.create_pi();
    /** Named complemented source signal. */
    const auto inverted = !input;
    source.set_name(inverted, "inverted");
    /** @brief Whether the named complement feeds a gate or a primary output. */
    const auto uses_gate_input = GENERATE(false, true);
    CAPTURE(uses_gate_input);
    if (uses_gate_input)
    {
        source.create_po(source.create_and(inverted, source.create_pi()));
    }
    else
    {
        source.create_po(inverted);
    }
    /** Target network receiving the mapped signal name. */
    named_aig_network target{};
    /** Mapped target signal. */
    const auto mapped = target.create_pi();
    /** Source-node mapping to target signals. */
    mockturtle::node_map<decltype(target)::signal, decltype(source)> mapping{source};
    mapping[source.get_node(input)] = mapped;

    restore_signal_names(source, target, mapping);

    REQUIRE(target.has_name(mapped));
    CHECK(target.get_name(mapped) == "inverted");
}
