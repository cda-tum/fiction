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

#include <mockturtle/views/names_view.hpp>

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
