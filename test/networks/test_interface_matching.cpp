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
 * @brief Tests for deterministic interface matching.
 * @author Marcel Walter (marcelwa)
 */
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/interface_matching.hpp>

#include <mockturtle/networks/klut.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;

TEST_CASE("Unnamed interfaces match in declared order and unequal sizes fail", "[interface-matching]")
{
    mockturtle::klut_network left{}, right{};
    CHECK(match_interfaces(left, right).inputs.empty());
    CHECK(match_interfaces(left, right).outputs.empty());
    const auto a = left.create_pi();
    const auto b = right.create_pi();
    left.create_po(a);
    right.create_po(b);
    CHECK(match_interfaces(left, right).inputs == std::vector<uint32_t>{0});
    CHECK(match_interfaces(left, right).outputs == std::vector<uint32_t>{0});
    right.create_po(b);
    CHECK_THROWS_AS(match_interfaces(left, right), std::invalid_argument);
    left.create_po(a);
    right.create_pi();
    CHECK_THROWS_AS(match_interfaces(left, right), std::invalid_argument);
}

TEST_CASE("A name duplicated on either side falls back to position", "[interface-matching]")
{
    /** @brief Network with unique terminal names. */
    mockturtle::names_view<mockturtle::klut_network> unique_names{};
    /** @brief Network whose repeated terminal name falls back to position. */
    mockturtle::names_view<mockturtle::klut_network> duplicate_names{};
    unique_names.create_pi("a");
    unique_names.create_pi("b");
    duplicate_names.create_pi("b");
    duplicate_names.create_pi("b");
    CHECK(match_interfaces(unique_names, duplicate_names).inputs == std::vector<uint32_t>{0, 1});
    CHECK(match_interfaces(duplicate_names, unique_names).inputs == std::vector<uint32_t>{0, 1});
}

TEST_CASE("Interface matching uses unique names before positional remainders", "[interface-matching]")
{
    gate_layout<cartesian_layout>                    lyt{};
    mockturtle::names_view<mockturtle::klut_network> ntk{};
    const std::vector<std::string>                   left{"unique", "dup", "dup", "", "left", "other"};
    const std::vector<std::string>                   right{"dup", "other", "right", "unique", "dup", ""};
    for (uint32_t i{}; i < left.size(); ++i)
    {
        const auto pi = lyt.create_pi(left[i], {i, 0});
        lyt.create_po(pi, left[i], {i, 1});
        const auto signal = ntk.create_pi(right[i]);
        ntk.create_po(signal, right[i]);
    }
    const auto match = match_interfaces(lyt, ntk);
    CHECK(match.inputs == std::vector<uint32_t>{3, 0, 2, 4, 5, 1});
    CHECK(match.outputs == std::vector<uint32_t>{3, 0, 2, 4, 5, 1});
}

TEST_CASE("Interface matching compares complete names and keeps input and output orders separate",
          "[interface-matching]")
{
    const auto                                       prefix = std::string(128, 'x') + '\0';
    const auto                                       first  = prefix + "first";
    const auto                                       second = prefix + "second";
    mockturtle::names_view<mockturtle::klut_network> left{}, right{};
    const auto                                       a = left.create_pi(first);
    const auto                                       b = left.create_pi(second);
    left.create_po(a, first);
    left.create_po(b, second);
    const auto rb = right.create_pi(second);
    const auto ra = right.create_pi(first);
    right.create_po(ra, first);
    right.create_po(rb, second);
    const auto match = match_interfaces(left, right);
    CHECK(match.inputs == std::vector<uint32_t>{1, 0});
    CHECK(match.outputs == std::vector<uint32_t>{0, 1});
}
