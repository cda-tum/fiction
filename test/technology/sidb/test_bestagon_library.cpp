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
 * @brief Tests for `fiction/technology/sidb/bestagon_library.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("Bestagon traits", "[sidb-bestagon-gate-library]")
{
    CHECK(!has_post_layout_optimization_v<bestagon_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<bestagon_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<bestagon_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<bestagon_library, cart_gate_clk_lyt>);
    CHECK(has_get_functional_implementations_v<bestagon_library>);
    CHECK(has_get_gate_ports_v<bestagon_library>);
}
