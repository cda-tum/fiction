//
// Created by Jan Drewniok on 12.09.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Use SiQAD XNOR skeleton and generate SiQAD XNOR gate, exhaustive", "[design-sidb-gates]")
{
    using offset_layout = sidb_100_cell_clk_lyt;
    using siqad_layout  = sidb_100_cell_clk_lyt_siqad;
    using cube_layout   = sidb_100_cell_clk_lyt_cube;

    siqad_layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 9, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 10, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 12, 1}, sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_cells() == 13);

    const design_sidb_gates_params<siqad_layout> params{
        sidb_simulation_parameters{2, -0.32},
        design_sidb_gates_params<siqad_layout>::design_sidb_gates_mode::EXHAUSTIVE,
        {{10, 4, 0}, {10, 4, 0}},
        1,
        sidb_simulation_engine::QUICKEXACT};

    const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_xnor_tt()}, params);

    REQUIRE(found_gate_layouts.size() == 1);
    CHECK(found_gate_layouts[0].num_cells() == 14);
    CHECK(found_gate_layouts[0].get_cell_type({10, 4, 0}) == siqad_layout::technology::NORMAL);

    // using cube coordinates
    const auto                                  lyt_in_cube_coord = convert_to_fiction_coordinates<cube_layout>(lyt);
    const design_sidb_gates_params<cube_layout> params_cube{
        sidb_simulation_parameters{2, -0.32},
        design_sidb_gates_params<cube_layout>::design_sidb_gates_mode::EXHAUSTIVE,
        {siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 4, 0}),
         siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 4, 0})},
        1,
        sidb_simulation_engine::QUICKEXACT};

    const auto found_gate_layouts_cube =
        design_sidb_gates(lyt_in_cube_coord, std::vector<tt>{create_xnor_tt()}, params_cube);

    REQUIRE(found_gate_layouts_cube.size() == 1);
    CHECK(found_gate_layouts_cube[0].num_cells() == 14);
    CHECK(found_gate_layouts_cube[0].get_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 4, 0})) ==
          siqad_layout::technology::NORMAL);

    // using offset coordinates
    const auto lyt_in_offset_coord = convert_to_fiction_coordinates<offset_layout>(lyt);
    const design_sidb_gates_params<offset_layout> params_offset{
        sidb_simulation_parameters{2, -0.32},
        design_sidb_gates_params<offset_layout>::design_sidb_gates_mode::EXHAUSTIVE,
        {siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 4, 0}),
         siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 4, 0})},
        1,
        sidb_simulation_engine::QUICKEXACT};

    const auto found_gate_layouts_offset =
        design_sidb_gates(lyt_in_offset_coord, std::vector<tt>{create_xnor_tt()}, params_offset);

    REQUIRE(found_gate_layouts_offset.size() == 1);
    CHECK(found_gate_layouts_offset[0].num_cells() == 14);
    CHECK(found_gate_layouts_offset[0].get_cell_type(
              siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 4, 0})) == siqad_layout::technology::NORMAL);
}

TEST_CASE("Use SiQAD's AND gate skeleton to generate all possible AND gates", "[design-sidb-gates]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad> params{
        sidb_simulation_parameters{2, -0.28},
        design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad>::design_sidb_gates_mode::EXHAUSTIVE,
        {{4, 4, 0}, {14, 5, 1}},
        1,
        sidb_simulation_engine::EXGS};

    SECTION("Exhaustive Generation")
    {
        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts.empty());
    }

    SECTION("Random Generation")
    {
        params.design_mode = design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad>::design_sidb_gates_mode::RANDOM;
        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        CHECK(!found_gate_layouts.empty());
    }
}

TEST_CASE("Use FO2 Bestagon gate without SiDB at {17, 11, 0} and generate original one", "[design-sidb-gates]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    // SiDB, originally part of the Bestagon fo2 gate, is excluded.
    // lyt.assign_cell_type({17, 11, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({21, 11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 7, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("generate original FO2")
    {
        const design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad>::design_sidb_gates_mode::EXHAUSTIVE,
            {{17, 11, 0}, {17, 11, 0}},
            1,
            sidb_simulation_engine::QUICKEXACT};

        CHECK(lyt.get_cell_type({17, 11, 0}) == sidb_100_cell_clk_lyt_siqad::technology::EMPTY);

        // generate gate by placing one SiDB

        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_cells() == 21);
        CHECK(found_gate_layouts[0].get_cell_type({17, 11, 0}) == sidb_100_cell_clk_lyt_siqad::technology::NORMAL);
    }

    SECTION("replace the output perturbers by equivalent negatively charged defects")
    {
        const design_sidb_gates_params<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<
                sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::EXHAUSTIVE,
            {{17, 11, 0}, {17, 11, 0}},
            1,
            sidb_simulation_engine::QUICKEXACT};

        sidb_defect_surface defect_layout{lyt};
        defect_layout.assign_cell_type({36, 19, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        defect_layout.assign_cell_type({2, 19, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({36, 19, 0}) == sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({2, 19, 0}) == sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);

        defect_layout.assign_sidb_defect(
            {36, 19, 0},
            sidb_defect{sidb_defect_type::DB, -1, params.phys_params.epsilon_r, params.phys_params.lambda_tf});
        defect_layout.assign_sidb_defect({2, 19, 0}, sidb_defect{sidb_defect_type::DB, -1, params.phys_params.epsilon_r,
                                                                 params.phys_params.lambda_tf});

        const auto found_gate_layouts = design_sidb_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts.size() == 1);
        CHECK(found_gate_layouts[0].num_cells() == 19);
        CHECK(found_gate_layouts[0].get_cell_type({17, 11, 0}) == sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
    }
}

TEST_CASE("Design AND Bestagon shaped gate", "[design-sidb-gates]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("Random Generation")
    {
        const design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<sidb_100_cell_clk_lyt_siqad>::design_sidb_gates_mode::RANDOM,
            {{14, 6, 0}, {24, 12, 0}},
            3,
            sidb_simulation_engine::QUICKEXACT};

        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }

    SECTION("Random Generation with defects")
    {
        sidb_defect_surface defect_layout{lyt};

        const design_sidb_gates_params<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::RANDOM,
            {{14, 6, 0}, {24, 12, 0}},
            3,
            sidb_simulation_engine::QUICKEXACT};

        defect_layout.assign_sidb_defect(
            {15, 10, 0},
            sidb_defect{sidb_defect_type::DB, -1, params.phys_params.epsilon_r, params.phys_params.lambda_tf});
        defect_layout.assign_sidb_defect(
            {20, 12, 0},
            sidb_defect{sidb_defect_type::DB, -1, params.phys_params.epsilon_r, params.phys_params.lambda_tf});

        const auto found_gate_layouts = design_sidb_gates(defect_layout, std::vector<tt>{create_and_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_defects() == 2);
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);

        found_gate_layouts.front().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell != siqad::coord_t{15, 10, 0});
                CHECK(cell != siqad::coord_t{20, 12, 0});
            });
    }
}

TEST_CASE("Design AND Bestagon shaped gate on H-Si 111", "[design-sidb-gates]")
{
    sidb_111_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({25, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({23, 1, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({5, 5, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 5, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 8, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({9, 9, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({15, 9, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({15, 21, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, 23, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({19, 25, 1}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({21, 27, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({23, 29, 1}, sidb_technology::cell_type::NORMAL);

    SECTION("Random Generation")
    {
        const design_sidb_gates_params<sidb_111_cell_clk_lyt_siqad> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<sidb_111_cell_clk_lyt_siqad>::design_sidb_gates_mode::RANDOM,
            {{10, 11, 0}, {14, 17, 0}},
            3,
            sidb_simulation_engine::QUICKEXACT};

        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(!found_gate_layouts.empty());
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }

    SECTION("Random Generation")
    {
        const design_sidb_gates_params<sidb_111_cell_clk_lyt_siqad> params{
            sidb_simulation_parameters{2, -0.32},
            design_sidb_gates_params<sidb_111_cell_clk_lyt_siqad>::design_sidb_gates_mode::EXHAUSTIVE,
            {{10, 11, 0}, {14, 17, 0}},
            3,
            sidb_simulation_engine::QUICKEXACT};

        const auto found_gate_layouts = design_sidb_gates(lyt, std::vector<tt>{create_nor_tt()}, params);
        REQUIRE(found_gate_layouts.size() == 206);
        CHECK(found_gate_layouts.front().num_cells() == lyt.num_cells() + 3);
    }
}
