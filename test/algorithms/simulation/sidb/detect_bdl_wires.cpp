//
// Created by Jan Drewniok on 15.07.24.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Three separate BDL pairs on the same y-coord (three wires)", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({36, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({9, 0, 0}, sidb_technology::cell_type::OUTPUT);

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 2);
    }

    SECTION("Determine input wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_bdl_wires.size() == 1);
    }

    SECTION("Determine output wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_bdl_wires.size() == 1);
    }
}

TEST_CASE("Three separate BDL pairs on the same x-coord (three wires)", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 36, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 37, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 8, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 9, 0}, sidb_technology::cell_type::OUTPUT);

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 3);
    }

    SECTION("Determine input wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::NO_DIRECTION);
    }

    SECTION("Determine output wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::NO_DIRECTION);
    }
}

TEST_CASE("BDL wire without I/O BDL pairs", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({9, 0, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::NO_DIRECTION);
    }
}

TEST_CASE("BDL wire left to right", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    SECTION("Determine wire")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::NORTH_SOUTH);
    }
}

TEST_CASE("BDL wire bottom to up", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({0, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 13, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 18, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 19, 0}, sidb_technology::cell_type::INPUT);

    SECTION("Determine wire")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::SOUTH_NORTH);
    }
}

TEST_CASE("BDL wire right to left", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::INPUT);

    SECTION("Determine wire")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(determine_wire_direction<decltype(lyt)>(all_bdl_wires[0]) == bdl_wire_direction::SOUTH_NORTH);
    }
}

TEST_CASE("Three separate BDL pairs (three wires)", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({36, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({30, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 3);
    }

    SECTION("Determine input wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_bdl_wires.size() == 1);
    }

    SECTION("Determine output wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_bdl_wires.size() == 1);
    }
}

TEST_CASE("Detect BDL wires", "[detect-bdl-wires]")
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

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 3);
    }

    SECTION("Determine input wires")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 2);
        CHECK(determine_wire_direction<decltype(lyt)>(all_input_bdl_wires[0]) == bdl_wire_direction::NORTH_SOUTH);
        CHECK(determine_wire_direction<decltype(lyt)>(all_input_bdl_wires[1]) == bdl_wire_direction::NORTH_SOUTH);
        const auto input_wire = all_input_bdl_wires.front();
        CHECK(input_wire.size() == 3);
        CHECK(input_wire[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, cell<decltype(lyt)>{0, 0, 0}, cell<decltype(lyt)>{2, 1, 0}});
        CHECK(input_wire[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{6, 2, 0}, cell<decltype(lyt)>{8, 3, 0}});
        CHECK(input_wire[2] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{12, 4, 0},
                                        cell<decltype(lyt)>{14, 5, 0}});
    }

    SECTION("Determine output wires and direction")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 1);
        const auto output_wire = all_output_bdl_wires.front();
        CHECK(output_wire.size() == 2);
        CHECK(output_wire[0] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{24, 15, 0},
                                         cell<decltype(lyt)>{26, 16, 0}});
        CHECK(output_wire[1] == bdl_pair{sidb_technology::cell_type::OUTPUT, cell<decltype(lyt)>{30, 17, 0},
                                         cell<decltype(lyt)>{32, 18, 0}});
        CHECK(determine_wire_direction<decltype(lyt)>(output_wire) == bdl_wire_direction::NORTH_SOUTH);
    }
}

TEST_CASE("Output cells at the top and input at the bottom", "[detect-bdl-wires]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::OUTPUT);

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

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 3);
    }

    SECTION("Determine output wires")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 2);
        CHECK(determine_wire_direction<decltype(lyt)>(all_output_bdl_wires[0]) == bdl_wire_direction::SOUTH_NORTH);
        CHECK(determine_wire_direction<decltype(lyt)>(all_output_bdl_wires[1]) == bdl_wire_direction::SOUTH_NORTH);
        const auto output_wire = all_output_bdl_wires.front();
        CHECK(output_wire.size() == 3);
        CHECK(output_wire[0] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, cell<decltype(lyt)>{0, 0, 0}, cell<decltype(lyt)>{2, 1, 0}});
        CHECK(output_wire[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{6, 2, 0}, cell<decltype(lyt)>{8, 3, 0}});
        CHECK(output_wire[2] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{12, 4, 0},
                                         cell<decltype(lyt)>{14, 5, 0}});
    }

    SECTION("Determine input wires and direction")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 1);
        const auto input_wire = all_input_bdl_wires.front();
        CHECK(input_wire.size() == 2);
        CHECK(input_wire[0] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{24, 15, 0},
                                        cell<decltype(lyt)>{26, 16, 0}});
        CHECK(input_wire[1] == bdl_pair{sidb_technology::cell_type::INPUT, cell<decltype(lyt)>{30, 17, 0},
                                        cell<decltype(lyt)>{32, 18, 0}});
        CHECK(determine_wire_direction<decltype(lyt)>(input_wire) == bdl_wire_direction::SOUTH_NORTH);
    }
}

TEST_CASE("Determine I/O wires of Bestagon AND gate", "[detect-bdl-wires]")
{
    const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

    detect_bdl_wires_params params{2.0};

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, params);
        REQUIRE(all_bdl_wires.size() == 4);
    }

    SECTION("Determine output wires")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, params, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 2);
        CHECK(determine_wire_direction<decltype(lyt)>(all_output_bdl_wires[0]) == bdl_wire_direction::NORTH_SOUTH);
        CHECK(determine_wire_direction<decltype(lyt)>(all_output_bdl_wires[1]) == bdl_wire_direction::NORTH_SOUTH);
        const auto output_wire_first = all_output_bdl_wires[0];
        CHECK(output_wire_first.size() == 2);
        CHECK(output_wire_first[0] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{14, 15, 0},
                                               cell<decltype(lyt)>{12, 16, 0}});
        CHECK(output_wire_first[1] == bdl_pair{sidb_technology::cell_type::OUTPUT, cell<decltype(lyt)>{8, 17, 0},
                                               cell<decltype(lyt)>{6, 18, 0}});

        const auto output_wire_second = all_output_bdl_wires[1];
        CHECK(output_wire_second.size() == 2);
        CHECK(output_wire_second[0] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{24, 15, 0},
                                                cell<decltype(lyt)>{26, 16, 0}});
        CHECK(output_wire_second[1] == bdl_pair{sidb_technology::cell_type::OUTPUT, cell<decltype(lyt)>{30, 17, 0},
                                                cell<decltype(lyt)>{32, 18, 0}});
    }

    SECTION("Determine input wires and direction")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 2);
        const auto input_wire_first = all_input_bdl_wires[0];
        REQUIRE(input_wire_first.size() == 3);
        CHECK(input_wire_first[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, cell<decltype(lyt)>{0, 0, 0}, cell<decltype(lyt)>{2, 1, 0}});
        CHECK(input_wire_first[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{6, 2, 0}, cell<decltype(lyt)>{8, 3, 0}});
        CHECK(input_wire_first[2] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{12, 4, 0},
                                              cell<decltype(lyt)>{14, 5, 0}});
        CHECK(determine_wire_direction<decltype(lyt)>(input_wire_first) == bdl_wire_direction::NORTH_SOUTH);

        const auto input_wire_second = all_input_bdl_wires[1];
        CHECK(input_wire_second[0] == bdl_pair{sidb_technology::cell_type::INPUT, cell<decltype(lyt)>{38, 0, 0},
                                               cell<decltype(lyt)>{36, 1, 0}});
        CHECK(input_wire_second[1] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{32, 2, 0},
                                               cell<decltype(lyt)>{30, 3, 0}});
        CHECK(input_wire_second[2] == bdl_pair{sidb_technology::cell_type::NORMAL, cell<decltype(lyt)>{26, 4, 0},
                                               cell<decltype(lyt)>{24, 5, 0}});
        CHECK(determine_wire_direction<decltype(lyt)>(input_wire_first) == bdl_wire_direction::NORTH_SOUTH);
    }
}
