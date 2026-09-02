//
// Created by Jan Drewniok on 08.01.25.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/qca/sim7_mol_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/technology/sidb/generators/design_gates.hpp>
#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/on_the_fly_gate_library.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/surfaces/defect_surface.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <string>
#include <vector>

using namespace fiction;

namespace
{
template <typename Lyt>
void check_equivalence(const Lyt& layout_designed, const std::string& path_layout_correct)
{
    const auto layout_correct = sidb::io::read_sqd_layout<Lyt>(path_layout_correct);

    REQUIRE(layout_designed.num_cells() == layout_correct.num_cells());

    REQUIRE(layout_designed.num_cells_of_given_type(technology<Lyt>::cell_type::LOGIC) ==
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
}  // namespace

using cell_lyt = sidb_100_cell_clk_lyt_cube;

TEST_CASE("Gate-level layout with AND gate", "[apply-gate-library]")
{
    hex_even_row_gate_clk_lyt layout{{2, 2}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

    layout.create_and(0, 1, {1, 2});

    SECTION("Apply static Bestagon gate library")
    {
        const auto bestagon_and = physical_design::apply_gate_library<sidb_100_cell_clk_lyt, sidb::bestagon_library,
                                                                      hex_even_row_gate_clk_lyt>(layout);

        CHECK(bestagon_and.num_cells() == 18);
    }
    SECTION("Design SiDB circuit on-the-fly")
    {
        sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

        sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
        design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
        design_gate_params.termination_cond =
            sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;
        design_gate_params.canvas = {{24, 17}, {34, 28}};

        SECTION("AND gate can be designed successfully")
        {
            design_gate_params.number_of_canvas_sidbs = 2;

            params.design_gate_params = design_gate_params;

            REQUIRE_NOTHROW(
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params));

            const auto bestagon_and =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                bestagon_and,
                fmt::format("{}/resources/on_the_fly_gate_library/single_tile_layout/AND_gate.sqd", TEST_PATH));

            CHECK(sidb::simulation::logic::is_operational(bestagon_and, std::vector<tt>{synthesis::create_and_tt()},
                                                          design_gate_params.operational_params)
                      .first == sidb::simulation::logic::operational_status::OPERATIONAL);

            CHECK(bestagon_and.num_cells() == 19);

            SECTION("with defects")
            {
                sidb::surfaces::defect_surface<cell_lyt> defect_surface{};

                defect_surface.assign_defect(
                    {30, 20, 0}, fiction::sidb::model::defect{fiction::sidb::model::defect_type::DB, -1, 4.1, 1.8});
                defect_surface.assign_defect(
                    {45, 55, 0}, fiction::sidb::model::defect{fiction::sidb::model::defect_type::DB, -1, 4.1, 1.8});

                const auto bestagon_and_with_defects =
                    physical_design::apply_parameterized_gate_library_to_defective_surface<
                        sidb::surfaces::defect_surface<cell_lyt>, sidb::on_the_fly_gate_library,
                        hex_even_row_gate_clk_lyt>(layout, params, defect_surface);

                CHECK(bestagon_and_with_defects.num_defects() == 2);
                CHECK(bestagon_and_with_defects.get_defect({30, 20, 0}).type == sidb::model::defect_type::DB);
                CHECK(bestagon_and_with_defects.get_defect({45, 55, 0}).type == sidb::model::defect_type::DB);

                CHECK(sidb::simulation::logic::is_operational(bestagon_and_with_defects,
                                                              std::vector<tt>{synthesis::create_and_tt()},
                                                              design_gate_params.operational_params)
                          .first == sidb::simulation::logic::operational_status::OPERATIONAL);

                CHECK(bestagon_and_with_defects.num_cells() == 19);
            }
        }
        SECTION("AND gate cannot be designed with one SiDB, exception handling on invalid parameters")
        {
            // Set up the parameters with an invalid configuration (1 SiDB for an AND gate)
            design_gate_params.number_of_canvas_sidbs = 1;
            params.design_gate_params                 = design_gate_params;

            CHECK_THROWS(physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                           hex_even_row_gate_clk_lyt>(layout, params));
        }
    }
}

TEST_CASE("Gate-level layout with two input wires, one double wire, and two output wires", "[apply-gate-library]")
{
    hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

    const auto x1   = layout.create_pi("x1", {0, 0});
    const auto buf1 = layout.create_buf(x1, {1, 1, 0});
    layout.create_po(buf1, "f1", {0, 2, 0});

    const auto x2   = layout.create_pi("x2", {1, 0});
    const auto buf2 = layout.create_buf(x2, {1, 1, 1});
    layout.create_po(buf2, "f2", {1, 2, 0});

    SECTION("Apply static Bestagon gate library")
    {
        const auto double_wire =
            physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(layout);

        check_equivalence(double_wire,
                          fmt::format("{}/resources/bestagon_library/multi_tile_layout/double_wire.sqd", TEST_PATH));
    }

    SECTION("Use parameterized gate library")
    {
        sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

        sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
        design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
        design_gate_params.canvas                        = {{24, 17}, {34, 28}};
        design_gate_params.termination_cond =
            sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

        SECTION("use predefined gate implementation for complex gates (double wire and crossing)")
        {
            design_gate_params.number_of_canvas_sidbs = 2;
            params.using_predefined_crossing_and_double_wire_if_possible =
                sidb::on_the_fly_gate_library_params<cell<cell_lyt>>::complex_gate_design_policy::USING_PREDEFINED;
            params.design_gate_params = design_gate_params;

            const auto bestagon_double_wire =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                bestagon_double_wire,
                fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/predefined_double_wire.sqd",
                            TEST_PATH));
        }
        SECTION("Design all gates of the layout on-the-fly")
        {
            design_gate_params.canvas                 = {{24, 17}, {29, 28}};
            design_gate_params.number_of_canvas_sidbs = 3;
            design_gate_params.design_mode =
                sidb::generators::design_gates_params<cell<cell_lyt>>::design_gates_mode::QUICKCELL;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;
            params.design_gate_params = design_gate_params;
            params.using_predefined_crossing_and_double_wire_if_possible =
                sidb::on_the_fly_gate_library_params<cell<cell_lyt>>::complex_gate_design_policy::DESIGN_ON_THE_FLY;

            const auto bestagon_double_wire =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                bestagon_double_wire,
                fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/double_wire.sqd", TEST_PATH));

            SECTION("with defects")
            {
                sidb::surfaces::defect_surface<cell_lyt> defect_surface{};

                defect_surface.assign_defect(
                    {30, 20, 0}, fiction::sidb::model::defect{fiction::sidb::model::defect_type::DB, -1, 4.1, 1.8});
                defect_surface.assign_defect(
                    {45, 55, 0}, fiction::sidb::model::defect{fiction::sidb::model::defect_type::DB, -1, 4.1, 1.8});

                const auto bestagon_double_wire_with_defects =
                    physical_design::apply_parameterized_gate_library_to_defective_surface<
                        sidb::surfaces::defect_surface<cell_lyt>, sidb::on_the_fly_gate_library,
                        hex_even_row_gate_clk_lyt>(layout, params, defect_surface);

                CHECK(bestagon_double_wire_with_defects.num_defects() == 2);
                CHECK(bestagon_double_wire_with_defects.get_defect({30, 20, 0}).type == sidb::model::defect_type::DB);
                CHECK(bestagon_double_wire_with_defects.get_defect({45, 55, 0}).type == sidb::model::defect_type::DB);

                check_equivalence(
                    bestagon_double_wire,
                    fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/double_wire_and_defects.sqd",
                                TEST_PATH));
            }
        }
    }
}

TEST_CASE("Gate-level layout with with different gates", "[apply-gate-library]")
{
    SECTION("INV with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1   = layout.create_pi("x1", {0, 0});
        const auto buf1 = layout.create_not(x1, {1, 1, 0});
        layout.create_po(buf1, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto inverter =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(inverter,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/INV.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{25, 19}, {32, 25}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto inverter =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(inverter,
                              fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/INV.sqd", TEST_PATH));

            SECTION("with defects")
            {
                sidb::surfaces::defect_surface<cell_lyt> defect_layout{};
                defect_layout.assign_defect(
                    {25, 39, 0}, sidb::model::defect{sidb::model::defect_type::DB, -1,
                                                     design_gate_params.operational_params.sim_params.epsilon_r,
                                                     design_gate_params.operational_params.sim_params.lambda_tf});

                const auto inverter_with_defects =
                    physical_design::apply_parameterized_gate_library_to_defective_surface<
                        sidb::surfaces::defect_surface<cell_lyt>, sidb::on_the_fly_gate_library,
                        hex_even_row_gate_clk_lyt>(layout, params, defect_layout);

                CHECK(inverter_with_defects.num_defects() == 1);

                check_equivalence(
                    inverter_with_defects,
                    fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/INV_and_defects.sqd",
                                TEST_PATH));
            }
        }
    }

    SECTION("OR with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1      = layout.create_pi("x1", {0, 0});
        const auto x2      = layout.create_pi("x2", {1, 0});
        const auto or_gate = layout.create_or(x1, x2, {1, 1, 0});
        layout.create_po(or_gate, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto or_layout =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(or_layout,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/OR.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto or_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(or_layout,
                              fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/OR.sqd", TEST_PATH));

            SECTION("with defects")
            {
                sidb::surfaces::defect_surface<cell_lyt> defect_layout{};
                defect_layout.assign_defect(
                    {50, 19, 0}, sidb::model::defect{sidb::model::defect_type::SI_VACANCY, -1,
                                                     design_gate_params.operational_params.sim_params.epsilon_r,
                                                     design_gate_params.operational_params.sim_params.lambda_tf});

                const auto or_layout_with_defects =
                    physical_design::apply_parameterized_gate_library_to_defective_surface<
                        sidb::surfaces::defect_surface<cell_lyt>, sidb::on_the_fly_gate_library,
                        hex_even_row_gate_clk_lyt>(layout, params, defect_layout);

                CHECK(or_layout_with_defects.num_defects() == 1);

                check_equivalence(
                    or_layout_with_defects,
                    fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/OR_and_defects.sqd",
                                TEST_PATH));
            }
        }
    }

    SECTION("NAND with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1        = layout.create_pi("x1", {0, 0});
        const auto x2        = layout.create_pi("x2", {1, 0});
        const auto nand_gate = layout.create_nand(x1, x2, {1, 1, 0});
        layout.create_po(nand_gate, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto nand_layout =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(nand_layout,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/NAND.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto nand_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                nand_layout, fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/NAND.sqd", TEST_PATH));

            SECTION("with defects")
            {
                sidb::surfaces::defect_surface<cell_lyt> defect_layout{};
                defect_layout.assign_defect(
                    {50, 19, 0}, sidb::model::defect{sidb::model::defect_type::SI_VACANCY, -1,
                                                     design_gate_params.operational_params.sim_params.epsilon_r,
                                                     design_gate_params.operational_params.sim_params.lambda_tf});

                defect_layout.assign_defect(
                    {50, 70, 0}, sidb::model::defect{sidb::model::defect_type::SI_VACANCY, -1,
                                                     design_gate_params.operational_params.sim_params.epsilon_r,
                                                     design_gate_params.operational_params.sim_params.lambda_tf});

                const auto nand_layout_with_defects =
                    physical_design::apply_parameterized_gate_library_to_defective_surface<
                        sidb::surfaces::defect_surface<cell_lyt>, sidb::on_the_fly_gate_library,
                        hex_even_row_gate_clk_lyt>(layout, params, defect_layout);

                CHECK(nand_layout_with_defects.num_defects() == 2);

                check_equivalence(
                    nand_layout_with_defects,
                    fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/NAND_and_defects.sqd",
                                TEST_PATH));
            }
        }
    }

    SECTION("NOR with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1       = layout.create_pi("x1", {0, 0});
        const auto x2       = layout.create_pi("x2", {1, 0});
        const auto nor_gate = layout.create_nor(x1, x2, {1, 1, 0});
        layout.create_po(nor_gate, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto nor_layout =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(nor_layout,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/NOR.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto nor_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(nor_layout,
                              fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/NOR.sqd", TEST_PATH));
        }
    }

    SECTION("XOR with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1       = layout.create_pi("x1", {0, 0});
        const auto x2       = layout.create_pi("x2", {1, 0});
        const auto xor_gate = layout.create_xor(x1, x2, {1, 1, 0});
        layout.create_po(xor_gate, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto xor_layout =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(xor_layout,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/XOR.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto xor_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(xor_layout,
                              fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/XOR.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library, reject kinks")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;
            design_gate_params.operational_params.op_condition =
                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;

            params.design_gate_params = design_gate_params;

            const auto xor_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                xor_layout,
                fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/XOR_rejecting_kinks.sqd",
                            TEST_PATH));
        }
    }

    SECTION("XNOR with input and output wire tile")
    {
        hex_even_row_gate_clk_lyt layout{{2, 2, 1}, fiction::layouts::clocking::row<hex_even_row_gate_clk_lyt>()};

        const auto x1        = layout.create_pi("x1", {0, 0});
        const auto x2        = layout.create_pi("x2", {1, 0});
        const auto xnor_gate = layout.create_xnor(x1, x2, {1, 1, 0});
        layout.create_po(xnor_gate, "f1", {0, 2, 0});

        SECTION("Apply static Bestagon gate library")
        {
            const auto xnor_layout =
                physical_design::apply_gate_library<cell_lyt, sidb::bestagon_library, hex_even_row_gate_clk_lyt>(
                    layout);

            check_equivalence(xnor_layout,
                              fmt::format("{}/resources/bestagon_library/multi_tile_layout/XNOR.sqd", TEST_PATH));
        }

        SECTION("Use parameterized gate library")
        {
            sidb::on_the_fly_gate_library_params<cell<cell_lyt>> params{};

            sidb::generators::design_gates_params<cell<cell_lyt>> design_gate_params{};
            design_gate_params.operational_params.sim_params = fiction::sidb::model::simulation_parameters{2, -0.32};
            design_gate_params.canvas                        = {{24, 17}, {34, 28}};
            design_gate_params.number_of_canvas_sidbs        = 3;
            design_gate_params.termination_cond =
                sidb::generators::design_gates_params<cell<cell_lyt>>::termination_condition::AFTER_FIRST_SOLUTION;

            params.design_gate_params = design_gate_params;

            const auto xnor_layout =
                physical_design::apply_parameterized_gate_library<cell_lyt, sidb::on_the_fly_gate_library,
                                                                  hex_even_row_gate_clk_lyt>(layout, params);

            check_equivalence(
                xnor_layout, fmt::format("{}/resources/on_the_fly_gate_library/multi_tile_layout/XNOR.sqd", TEST_PATH));
        }
    }
}

TEST_CASE("Applying the QCA ONE gate library", "[apply-gate-library]")
{
    using GateLyt = cart_even_row_gate_clk_lyt;

    SECTION("Without crossings")
    {
        const auto gate_lyt = blueprints::straight_wire_gate_layout<GateLyt>();

        const auto layout =
            physical_design::apply_gate_library<qca_cell_clk_lyt, qca::qca_one_library, GateLyt>(gate_lyt);

        CHECK(layout.x() == 16);
        CHECK(layout.y() == 14);
        CHECK(layout.z() == 0);
    }

    SECTION("With crossings")
    {
        const auto gate_lyt = blueprints::optimization_layout_corner_case_outputs_2<GateLyt>();

        const auto layout =
            physical_design::apply_gate_library<stacked_qca_cell_clk_lyt, qca::qca_one_library, GateLyt>(gate_lyt);

        CHECK(layout.x() == 21);
        CHECK(layout.y() == 14);
        CHECK(layout.z() == 1);
    }
}

TEST_CASE("Apply molecular QCA gate library end-to-end", "[apply-gate-library]")
{
    using gate_layout = layouts::gate_level_layout<
        layouts::clocked_layout<layouts::tile_based_layout<layouts::cartesian_layout<layouts::coords::offset>>>>;

    const auto layout = blueprints::and_or_inv_gate_layout<gate_layout>();

    const auto cell_layout = physical_design::apply_gate_library<mol_qca_cell_clk_lyt, qca::sim7_mol_library>(layout);

    CHECK(cell_layout.num_cells() > 0u);
}
