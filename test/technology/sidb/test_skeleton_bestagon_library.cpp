/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by Jan Drewniok on 24.10.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/skeleton_bestagon_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Bestagon traits", "[sidb-dynamic-gate-library]")
{
    CHECK(!has_post_layout_optimization_v<sidb::skeleton_bestagon_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::skeleton_bestagon_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::skeleton_bestagon_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::skeleton_bestagon_library, cart_gate_clk_lyt>);
    CHECK(has_get_functional_implementations_v<sidb::skeleton_bestagon_library>);
    CHECK(has_get_gate_ports_v<sidb::skeleton_bestagon_library>);
}
