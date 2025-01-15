//
// Created by Jan Drewniok on 08.01.25.
//

#include <catch2/catch_assertion_result.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_on_the_fly_gate_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <string>
#include <vector>

using namespace fiction;

template <typename Lyt>
void check_equivalence(const Lyt& layout_designed, const std::string& path_layout_correct)
{
    const auto layout_correct = read_sqd_layout<Lyt>(path_layout_correct);

    REQUIRE(layout_designed.num_cells() == layout_correct.num_cells());

    CHECK(layout_designed.num_cells_of_given_type(technology<Lyt>::cell_type::LOGIC) ==
          layout_correct.num_cells_of_given_type(technology<Lyt>::cell_type::LOGIC));

    layout_designed.foreach_cell(
        [&layout_correct, &layout_designed](const auto& c)
        {
            // Gates designed on-the-fly are not necessarily identical each time.
            const auto cell_type = layout_designed.get_cell_type(c);
            if (cell_type != technology<Lyt>::cell_type::LOGIC)
            {
                CHECK(layout_designed.get_cell_type(c) == layout_correct.get_cell_type(c));
            }
        });
}

using cell_lyt = sidb_100_cell_clk_lyt_cube;

TEST_CASE("Gate-level layout with AND gate", "[apply-gate-library]")
{
    hex_even_row_gate_clk_lyt layout{{2, 2}, fiction::row_clocking<hex_even_row_gate_clk_lyt>()};

    layout.create_and(0, 1, {1, 2});

    SECTION("Apply static Bestagon gate library")
    {
        const auto bestagon_and =
            apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library, hex_even_row_gate_clk_lyt>(layout);

        CHECK(bestagon_and.num_cells() == 18);
    }
    SECTION("Design SiDB circuit on-the-fly")
    {
        sidb_on_the_fly_gate_library_params<cell_lyt> params{};

        design_sidb_gates_params<cell<cell_lyt>> design_gate_params{};
        design_gate_params.operational_params.simulation_parameters = fiction::sidb_simulation_parameters{2, -0.32};
        design_gate_params.canvas                                   = {{24, 17}, {34, 28}};

        SECTION("AND gate can be designed successfully")
        {
            design_gate_params.number_of_canvas_sidbs = 2;

            params.design_gate_params = design_gate_params;

            REQUIRE_NOTHROW(
                apply_parameterized_gate_library<cell_lyt, sidb_on_the_fly_gate_library, hex_even_row_gate_clk_lyt>(
                    layout, params));

            const auto bestagon_and =
                apply_parameterized_gate_library<cell_lyt, sidb_on_the_fly_gate_library, hex_even_row_gate_clk_lyt>(
                    layout, params);

            CHECK(is_operational(bestagon_and, std::vector<tt>{create_and_tt()}, design_gate_params.operational_params)
                      .first == operational_status::OPERATIONAL);

            CHECK(bestagon_and.num_cells() == 19);
        }
        SECTION("AND gate cannot be designed with one SiDB, exception handling on invalid parameters")
        {
            // Set up the parameters with an invalid configuration (1 SiDB for an AND gate)
            design_gate_params.number_of_canvas_sidbs = 1;
            params.design_gate_params                 = design_gate_params;

            CHECK_THROWS(
                apply_parameterized_gate_library<cell_lyt, sidb_on_the_fly_gate_library, hex_even_row_gate_clk_lyt>(
                    layout, params));
        }
    }
}

TEST_CASE("Gate-level layout with two input wires, one double wire, and two output wires", "[apply-gate-library]")
{
    hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::row_clocking<hex_even_row_gate_clk_lyt>()};

    const auto x1   = layout.create_pi("x1", {0, 0});
    const auto buf1 = layout.create_buf(x1, {1, 1, 0});
    layout.create_po(buf1, "f1", {0, 2, 0});

    const auto x2   = layout.create_pi("x2", {1, 0});
    const auto buf2 = layout.create_buf(x2, {1, 1, 1});
    layout.create_po(buf2, "f2", {1, 2, 0});

    SECTION("Apply static Bestagon gate library")
    {
        const auto double_wire = apply_gate_library<cell_lyt, sidb_bestagon_library, hex_even_row_gate_clk_lyt>(layout);

        check_equivalence(double_wire, fmt::format("{}/resources/double_wire_static.sqd", TEST_PATH));
    }

    SECTION("Use parameterized gate library")
    {
        sidb_on_the_fly_gate_library_params<cell_lyt> params{};

        design_sidb_gates_params<cell<cell_lyt>> design_gate_params{};
        design_gate_params.operational_params.simulation_parameters = fiction::sidb_simulation_parameters{2, -0.32};
        design_gate_params.canvas                                   = {{24, 17}, {34, 28}};

        SECTION("use predefined gate implementation for complex gates (double wire and crossing)")
        {
            design_gate_params.number_of_canvas_sidbs = 2;
            params.using_predefined_crossing_and_double_wire_if_possible =
                sidb_on_the_fly_gate_library_params<cell_lyt>::complex_gate_design_policy::USING_PREDEFINED;
            params.design_gate_params = design_gate_params;

            const auto bestagon_double_wire =
                apply_parameterized_gate_library<cell_lyt, sidb_on_the_fly_gate_library, hex_even_row_gate_clk_lyt>(
                    layout, params);

            check_equivalence(bestagon_double_wire,
                              fmt::format("{}/resources/on_the_fly_but_predefined_double_wire.sqd", TEST_PATH));
        }
        SECTION("Design all gates of the layout on-the-fly")
        {
            design_gate_params.canvas                 = {{24, 17}, {29, 28}};
            design_gate_params.number_of_canvas_sidbs = 3;
            design_gate_params.design_mode =
                design_sidb_gates_params<cell<cell_lyt>>::design_sidb_gates_mode::QUICKCELL;
            design_gate_params.termination_cond =
                design_sidb_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;
            params.design_gate_params = design_gate_params;
            params.using_predefined_crossing_and_double_wire_if_possible =
                sidb_on_the_fly_gate_library_params<cell_lyt>::complex_gate_design_policy::DESIGN_ON_THE_FLY;

            const auto bestagon_double_wire =
                apply_parameterized_gate_library<cell_lyt, sidb_on_the_fly_gate_library, hex_even_row_gate_clk_lyt>(
                    layout, params);

            check_equivalence(bestagon_double_wire, fmt::format("{}/resources/on_the_fly_double_wire.sqd", TEST_PATH));
        }
    }
}
