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
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/name_utils.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/names_view.hpp>

#include <stdexcept>
#include <string_view>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;

TEST_CASE("Name transfer respects interface order and string-view bounds", "[name-utils]")
{
    mockturtle::names_view<technology_network> ntk{};
    const auto                                 input = ntk.create_pi("a");
    ntk.create_po(input, "f");
    set_name(ntk, std::string_view{"prefix suffix", 6});

    gate_level_layout<cartesian_layout> lyt{{2, 1}};
    const auto                          placed = lyt.create_pi("", {0, 0});
    lyt.create_po(placed, "", {1, 0});
    restore_names(ntk, lyt);
    CHECK(lyt.get_layout_name() == "prefix");
    CHECK(lyt.get_input_name(0) == "a");
    CHECK(lyt.get_output_name(0) == "f");

    mockturtle::names_view<technology_network> copied{};
    copied.create_po(copied.create_pi());
    restore_names(lyt, copied);
    CHECK(copied.get_network_name() == "prefix");
    CHECK(copied.get_name(copied.make_signal(copied.pi_at(0))) == "a");
    CHECK(copied.get_output_name(0) == "f");
}

TEST_CASE("Signal names transfer from a logic network to placed objects", "[name-utils]")
{
    /** Named logic source with an internal signal and a declared output. */
    mockturtle::names_view<technology_network> ntk{};
    /** Named primary input signal. */
    const auto input = ntk.create_pi("input");
    /** Named internal signal. */
    const auto internal = ntk.create_not(input);
    ntk.set_name(internal, "internal");
    /** Unnamed sink makes the internal signal a gate input. */
    const auto sink = ntk.create_buf(internal);
    ntk.create_po(sink, "output");
    /** Native target with separate placed terminals. */
    gate_level_layout<cartesian_layout> lyt{{4, 1}};
    /** Placed primary input. */
    const auto placed_input = lyt.create_pi("", {0, 0});
    /** Placed internal gate. */
    const auto placed_internal = lyt.create_not(placed_input, {1, 0});
    /** Separately named output terminal. */
    const auto placed_sink = lyt.create_buf(placed_internal, {2, 0});
    /** Named output terminal independent of the driver name. */
    const auto placed_output = lyt.create_po(placed_sink, "terminal", {3, 0});
    /** Source-node map with native output endpoints as its values. */
    mockturtle::node_map<decltype(lyt)::output_port, decltype(ntk)> mapping{ntk};
    mapping[ntk.get_node(input)]    = placed_input;
    mapping[ntk.get_node(internal)] = placed_internal;
    mapping[ntk.get_node(sink)]     = placed_sink;

    restore_signal_names(ntk, lyt, mapping);

    CHECK(lyt.get_name(placed_input) == "input");
    CHECK(lyt.get_name(placed_internal) == "internal");
    CHECK(lyt.get_name(placed_output) == "terminal");
}

TEST_CASE("Signal name restoration covers output drivers and mapped unused inputs", "[name-utils]")
{
    /** Logic source with a named output driver and unused inputs. */
    mockturtle::names_view<technology_network> ntk{};
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
    gate_level_layout<cartesian_layout> lyt{{4, 1}};
    /** Placed connected input. */
    const auto placed_input = lyt.create_pi("", {0, 0});
    /** Placed output driver. */
    const auto placed_driver = lyt.create_not(placed_input, {1, 0});
    /** Placed output terminal. */
    const auto placed_output = lyt.create_po(placed_driver, "terminal", {2, 0});
    /** Placed unused input. */
    const auto placed_unused = lyt.create_pi("", {3, 0});
    /** Mapped native endpoints; unmapped nodes retain an absent default endpoint. */
    mockturtle::node_map<decltype(lyt)::output_port, decltype(ntk)> mapping{ntk};
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
        lyt.remove(placed_unused.object);
        CHECK_THROWS_AS(restore_signal_names(ntk, lyt, mapping), std::invalid_argument);
    }
}

TEST_CASE("Signal name restoration retains used complemented names", "[name-utils]")
{
    /** Source with a separately named complemented signal. */
    mockturtle::names_view<mockturtle::aig_network> source{};
    /** Source input whose complement carries a name. */
    const auto input = source.create_pi();
    /** Named complemented source signal. */
    const auto inverted = !input;
    source.set_name(inverted, "inverted");
    SECTION("Gate input")
    {
        source.create_po(source.create_and(inverted, source.create_pi()));
    }
    SECTION("Primary output driver")
    {
        source.create_po(inverted);
    }
    /** Target network receiving the mapped signal name. */
    mockturtle::names_view<mockturtle::aig_network> target{};
    /** Mapped target signal. */
    const auto mapped = target.create_pi();
    /** Source-node mapping to target signals. */
    mockturtle::node_map<decltype(target)::signal, decltype(source)> mapping{source};
    mapping[source.get_node(input)] = mapped;

    restore_signal_names(source, target, mapping);

    REQUIRE(target.has_name(mapped));
    CHECK(target.get_name(mapped) == "inverted");
}
