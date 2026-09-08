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
 * @brief Tests for `fiction/technology/sidb/on_the_fly_gate_library.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <array>
#include <optional>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::synthesis;

TEST_CASE("Parameterized gate library traits", "[parameterized-gate-library]")
{
    CHECK(!has_post_layout_optimization_v<on_the_fly_gate_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<on_the_fly_gate_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<on_the_fly_gate_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<on_the_fly_gate_library, cart_gate_clk_lyt>);
}

TEST_CASE("Unsuccessful binary SiDB designs retain the tile, function, and ports", "[parameterized-gate-library]")
{
    const std::array functions{create_and_tt(),  create_or_tt(), create_nand_tt(), create_nor_tt(), create_xor_tt(),
                               create_xnor_tt(), create_ge_tt(), create_le_tt(),   create_gt_tt(),  create_lt_tt()};

    for (const auto& function : functions)
    {
        for (const bool with_defects : std::array{false, true})
        {
            hex_even_row_gate_clk_lyt gate_layout{{2, 2}, clocking::row<hex_even_row_gate_clk_lyt>()};
            const auto                input1 = gate_layout.create_pi("input1", {0, 0});
            const auto                input2 = gate_layout.create_pi("input2", {1, 0});
            const auto                gate   = gate_layout.create_node({input1, input2}, function, {1, 1});
            gate_layout.create_po(gate, "output", {0, 2});

            on_the_fly_gate_library_params params{};
            params.design_gate_params.canvas                 = {site_at_row(24, 17), site_at_row(24, 17)};
            params.design_gate_params.number_of_canvas_sidbs = 2;

            std::optional<sidb::layout> surface{};
            if (with_defects)
            {
                surface.emplace();
                surface->assign_defect(site_at_row(0, 0), defect{defect_type::DB, -1, 2, 5});
            }

            try
            {
                static_cast<void>(on_the_fly_gate_library::set_up_gate<hex_even_row_gate_clk_lyt, sidb_100_cell_clk_lyt_cube>(
                    gate_layout, {1, 1}, params, surface));
                FAIL("Two canvas SiDBs cannot fit on one available site");
            }
            catch (const gate_design_exception<hex_even_row_gate_clk_lyt>& error)
            {
                CHECK(error.which_tile() == tile<hex_even_row_gate_clk_lyt>{1, 1});
                CHECK(error.which_truth_table() == function);
                CHECK(error.which_port_list().inp.size() == 2);
                CHECK(error.which_port_list().out.size() == 1);
            }
        }
    }
}
