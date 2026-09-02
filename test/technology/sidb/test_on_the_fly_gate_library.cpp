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
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Parameterized gate library traits", "[parameterized-gate-library]")
{
    CHECK(!has_post_layout_optimization_v<sidb::on_the_fly_gate_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::on_the_fly_gate_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::on_the_fly_gate_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sidb::on_the_fly_gate_library, cart_gate_clk_lyt>);
}
