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
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/execution_timeout.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <array>
#include <chrono>
#include <optional>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::synthesis;

TEST_CASE("Parameterized gate library traits", "[parameterized-gate-library]")
{
    CHECK(std::is_same_v<on_the_fly_gate_library::layout, sidb::layout>);
}

TEST_CASE("Predefined SiDB gates on defective surfaces honor the per-gate timeout", "[parameterized-gate-library]")
{
    for (const bool crossing : std::array{false, true})
    {
        CAPTURE(crossing);
        hex_even_row_gate_clk_lyt gate_layout{{2, 2, 1}, clocking::row()};
        const auto                input1 = gate_layout.create_pi("input1", {0, 0});
        const auto                input2 = gate_layout.create_pi("input2", {1, 0});
        const auto                wire1  = gate_layout.create_buf(input1, {1, 1, 0});
        const auto                wire2  = gate_layout.create_buf(input2, {1, 1, 1});
        gate_layout.create_po(wire1, "output1", {crossing ? 1u : 0u, 2, 0});
        gate_layout.create_po(wire2, "output2", {crossing ? 0u : 1u, 2, 0});

        sidb::layout surface{};
        surface.assign_defect(site_at_row(0, 0), defect{defect_type::DB, -1, 2, 5});

        on_the_fly_gate_library_params params{};
        params.design_gate_params.operational_params.sim_params = simulation_parameters{2, -0.32};
        params.design_gate_params.operational_params.timeout    = 0;
        const auto enclosing_deadline = std::chrono::steady_clock::now() + std::chrono::seconds{2};
        params.design_gate_params.operational_params.deadline = enclosing_deadline;

        CHECK_THROWS_AS(on_the_fly_gate_library::set_up_gate(gate_layout, {1, 1, 0}, params, surface),
                        utils::timeout_error);
        // The zero gate budget must expire, not the enclosing deadline used to bound a regression.
        CHECK(std::chrono::steady_clock::now() < enclosing_deadline);
        CHECK(params.design_gate_params.operational_params.deadline == enclosing_deadline);
    }
}

TEST_CASE("Unsuccessful binary SiDB designs retain the tile, function, and ports", "[parameterized-gate-library]")
{
    kitty::dynamic_truth_table ge{2};
    kitty::dynamic_truth_table le{2};
    kitty::create_from_binary_string(ge, "1101");
    kitty::create_from_binary_string(le, "1011");
    const std::array functions{
        create_and_tt(), create_or_tt(), create_nand_tt(), create_nor_tt(), create_xor_tt(), create_xnor_tt(), ge, le,
        create_gt_tt(),  create_lt_tt()};

    for (const auto& function : functions)
    {
        for (const bool with_defects : std::array{false, true})
        {
            hex_even_row_gate_clk_lyt gate_layout{{2, 2}, clocking::row()};
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
                static_cast<void>(on_the_fly_gate_library::set_up_gate(gate_layout, {1, 1}, params, surface));
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
