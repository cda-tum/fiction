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
 * @brief Tests for `fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::sidb;
using namespace fiction::sidb::simulation::logic;

TEST_CASE("Three separate BDL pairs on the same y-coord (three wires)", "[detect-bdl-wires]")
{
    layout lyt{};

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
    layout lyt{};

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
        CHECK(all_bdl_wires[0].port.dir == port_direction::cardinal::NONE);
    }

    SECTION("Determine output wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_bdl_wires.size() == 1);
        CHECK(all_bdl_wires[0].port.dir == port_direction::cardinal::NONE);
    }
}

TEST_CASE("BDL wire without I/O BDL pairs", "[detect-bdl-wires]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({9, 0, 0}, sidb_technology::cell_type::NORMAL);

    const auto all_bdl_wires = detect_bdl_wires(lyt);
    REQUIRE(all_bdl_wires.size() == 1);
    CHECK(all_bdl_wires[0].port.dir == port_direction::cardinal::NONE);
    CHECK(!all_bdl_wires.front().first_bdl_pair.has_value());
    CHECK(!all_bdl_wires.front().last_bdl_pair.has_value());
}

TEST_CASE("Output BDL wire from west to east", "[detect-bdl-wires]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::ALL);
    REQUIRE(all_bdl_wires.size() == 1);

    const auto output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
    REQUIRE(output_bdl_wires.size() == 1);
    const auto& output_bdl_wire = output_bdl_wires.front();
    CHECK(output_bdl_wire.port.dir == port_direction::cardinal::EAST);
    REQUIRE(output_bdl_wire.first_bdl_pair.has_value());
    REQUIRE(output_bdl_wire.last_bdl_pair.has_value());
    CHECK(output_bdl_wire.first_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
    CHECK(output_bdl_wire.last_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{18, 0, 0}, lattice_site{20, 0, 0}});
}

TEST_CASE("BDL wire from west to east", "[detect-bdl-wires]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    const auto all_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::ALL);
    REQUIRE(all_bdl_wires.size() == 1);

    const auto input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    REQUIRE(input_bdl_wires.size() == 1);
    const auto& input_bdl_wire = input_bdl_wires.front();
    CHECK(input_bdl_wire.port.dir == port_direction::cardinal::EAST);
    REQUIRE(input_bdl_wire.first_bdl_pair.has_value());
    REQUIRE(input_bdl_wire.last_bdl_pair.has_value());
    CHECK(input_bdl_wire.first_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
    CHECK(input_bdl_wire.last_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{18, 0, 0}, lattice_site{20, 0, 0}});

    const auto output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
    REQUIRE(output_bdl_wires.size() == 1);
    const auto& output_bdl_wire = output_bdl_wires.front();
    CHECK(output_bdl_wire.port.dir == port_direction::cardinal::EAST);
    REQUIRE(output_bdl_wire.first_bdl_pair.has_value());
    REQUIRE(output_bdl_wire.last_bdl_pair.has_value());
    CHECK(output_bdl_wire.first_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
    CHECK(output_bdl_wire.last_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{18, 0, 0}, lattice_site{20, 0, 0}});
}

TEST_CASE("BDL wire bottom to up", "[detect-bdl-wires]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({0, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 7, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 13, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 18, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 19, 0}, sidb_technology::cell_type::INPUT);

    const detect_bdl_wires_params params{.threshold_bdl_interdistance = 5.0};

    const auto all_bdl_wires = detect_bdl_wires(lyt, params);
    REQUIRE(all_bdl_wires.size() == 1);
    CHECK(all_bdl_wires[0].port.dir == port_direction::cardinal::NORTH);

    REQUIRE(all_bdl_wires.front().first_bdl_pair.has_value());
    REQUIRE(all_bdl_wires.front().last_bdl_pair.has_value());
    CHECK(all_bdl_wires.front().first_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 18, 0}, lattice_site{0, 19, 0}});
    CHECK(all_bdl_wires.front().last_bdl_pair ==
          bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{0, 0, 0}, lattice_site{0, 1, 0}});
}

TEST_CASE("BDL wire from east to west", "[detect-bdl-wires]")
{
    layout lyt{};

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
        CHECK(all_bdl_wires[0].port.dir == port_direction::cardinal::WEST);
    }
}

TEST_CASE("Three separate BDL pairs (three wires)", "[detect-bdl-wires]")
{
    layout lyt{};

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
    layout lyt{};

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
        CHECK(all_input_bdl_wires[0].port.dir == port_direction::cardinal::SOUTH);
        CHECK(all_input_bdl_wires[1].port.dir == port_direction::cardinal::SOUTH);
        const auto& input_wire = all_input_bdl_wires.front();
        CHECK(input_wire.pairs.size() == 3);
        CHECK(input_wire.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 1, 0}});
        CHECK(input_wire.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 2, 0}, lattice_site{8, 3, 0}});
        CHECK(input_wire.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 4, 0}, lattice_site{14, 5, 0}});
    }

    SECTION("Determine output wires and port")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 1);
        const auto& output_wire = all_output_bdl_wires.front();
        CHECK(output_wire.pairs.size() == 2);
        CHECK(output_wire.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{24, 15, 0}, lattice_site{26, 16, 0}});
        CHECK(output_wire.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{30, 17, 0}, lattice_site{32, 18, 0}});
        CHECK(output_wire.port.dir == port_direction::cardinal::SOUTH);
    }
}

TEST_CASE("Output cells at the top and input at the bottom", "[detect-bdl-wires]")
{
    layout lyt{};

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
        CHECK(all_output_bdl_wires[0].port.dir == port_direction::cardinal::NORTH);
        CHECK(all_output_bdl_wires[1].port.dir == port_direction::cardinal::NORTH);
        const auto& output_wire = all_output_bdl_wires.front();
        CHECK(output_wire.pairs.size() == 3);
        CHECK(output_wire.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{0, 0, 0}, lattice_site{2, 1, 0}});
        CHECK(output_wire.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 2, 0}, lattice_site{8, 3, 0}});
        CHECK(output_wire.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 4, 0}, lattice_site{14, 5, 0}});
    }

    SECTION("Determine input wires and port")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 1);
        const auto& input_wire = all_input_bdl_wires.front();
        CHECK(input_wire.pairs.size() == 2);
        CHECK(input_wire.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{24, 15, 0}, lattice_site{26, 16, 0}});
        CHECK(input_wire.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{30, 17, 0}, lattice_site{32, 18, 0}});
        CHECK(input_wire.port.dir == port_direction::cardinal::NORTH);
    }
}

TEST_CASE("Determine I/O wires of Bestagon CROSSING gate", "[detect-bdl-wires]")
{
    const auto lyt = to_sidb_layout(blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>());

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt);
        REQUIRE(all_bdl_wires.size() == 4);
    }

    SECTION("Determine output wires")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 2);
        CHECK(all_output_bdl_wires[0].port.dir == port_direction::cardinal::SOUTH);
        CHECK(all_output_bdl_wires[1].port.dir == port_direction::cardinal::SOUTH);
        const auto& output_wire_first = all_output_bdl_wires[0];
        CHECK(output_wire_first.pairs.size() == 2);
        CHECK(output_wire_first.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{14, 15, 0}, lattice_site{12, 16, 0}});
        CHECK(output_wire_first.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{8, 17, 0}, lattice_site{6, 18, 0}});

        const auto& output_wire_second = all_output_bdl_wires[1];
        CHECK(output_wire_second.pairs.size() == 2);
        CHECK(output_wire_second.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{24, 15, 0}, lattice_site{26, 16, 0}});
        CHECK(output_wire_second.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{30, 17, 0}, lattice_site{32, 18, 0}});
    }

    SECTION("Determine input wires and port")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 2);
        const auto& input_wire_first = all_input_bdl_wires[0];
        REQUIRE(input_wire_first.pairs.size() == 3);
        CHECK(input_wire_first.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 1, 0}});
        CHECK(input_wire_first.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 2, 0}, lattice_site{8, 3, 0}});
        CHECK(input_wire_first.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 4, 0}, lattice_site{14, 5, 0}});
        CHECK(input_wire_first.port.dir == port_direction::cardinal::SOUTH);

        const auto& input_wire_second = all_input_bdl_wires[1];
        CHECK(input_wire_second.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{38, 0, 0}, lattice_site{36, 1, 0}});
        CHECK(input_wire_second.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{32, 2, 0}, lattice_site{30, 3, 0}});
        CHECK(input_wire_second.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{26, 4, 0}, lattice_site{24, 5, 0}});

        CHECK(input_wire_first.port.dir == port_direction::cardinal::SOUTH);
    }
}

TEST_CASE("Determine I/O wires of 111-AND gate", "[detect-bdl-wires]")
{
    const auto lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>(), lattice::si_111_1x1());

    const detect_bdl_wires_params params{.threshold_bdl_interdistance = 2.0};

    SECTION("Determine all wires")
    {
        const auto all_bdl_wires = detect_bdl_wires(lyt, params);
        REQUIRE(all_bdl_wires.size() == 3);
    }

    SECTION("Determine output wires")
    {
        const auto all_output_bdl_wires = detect_bdl_wires(lyt, params, bdl_wire_selection::OUTPUT);
        REQUIRE(all_output_bdl_wires.size() == 1);
        CHECK(all_output_bdl_wires[0].port.dir == port_direction::cardinal::SOUTH);
        const auto& output_wire_first = all_output_bdl_wires[0];
        CHECK(output_wire_first.pairs.size() == 2);
        CHECK(output_wire_first.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{15, 21, 1}, lattice_site{17, 23, 0}});
        CHECK(output_wire_first.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::OUTPUT, lattice_site{19, 25, 1}, lattice_site{21, 27, 0}});
    }

    SECTION("Determine input wires and port")
    {
        const auto all_input_bdl_wires = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);
        REQUIRE(all_input_bdl_wires.size() == 2);
        const auto& input_wire_first = all_input_bdl_wires[0];
        REQUIRE(input_wire_first.pairs.size() == 3);
        CHECK(input_wire_first.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{1, 1, 1}});
        CHECK(input_wire_first.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{4, 4, 0}, lattice_site{5, 5, 1}});
        CHECK(input_wire_first.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{8, 8, 0}, lattice_site{9, 9, 1}});
        CHECK(input_wire_first.port.dir == port_direction::cardinal::SOUTH);

        const auto& input_wire_second = all_input_bdl_wires[1];
        CHECK(input_wire_second.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{25, 0, 0}, lattice_site{23, 1, 1}});
        CHECK(input_wire_second.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{21, 4, 0}, lattice_site{19, 5, 1}});
        CHECK(input_wire_second.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{17, 8, 0}, lattice_site{15, 9, 1}});

        CHECK(input_wire_first.port.dir == port_direction::cardinal::SOUTH);
    }
}

TEST_CASE("special cases", "[detect-bdl-wires]")
{
    SECTION("Wire with input BDL pair from west to east, but moving up and down, first and last BDL pair have the same "
            "y-coordinate")
    {
        layout lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 0, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);

        const detect_bdl_wires_params params{.threshold_bdl_interdistance = 2.5,
                                             .bdl_pairs_params = detect_bdl_pairs_params{.minimum_distance = 0.5}};

        const auto input_wire = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);

        REQUIRE(input_wire.size() == 1);
        CHECK(input_wire[0].port.dir == port_direction::cardinal::EAST);
        CHECK(input_wire[0].pairs.size() == 4);
        CHECK(input_wire[0].pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
        CHECK(input_wire[0].pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{18, 0, 0}, lattice_site{20, 0, 0}});
        CHECK(input_wire[0].pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 0, 1}, lattice_site{14, 0, 1}});
        CHECK(input_wire[0].pairs[3] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 1, 0}, lattice_site{8, 1, 0}});
    }

    SECTION("Wire with input BDL pair from east to west, but moving up and down, first and last BDL pair have the same "
            "y-coordinate")
    {
        layout lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({6, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 0, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::INPUT);

        const detect_bdl_wires_params params{.threshold_bdl_interdistance = 2.5,
                                             .bdl_pairs_params = detect_bdl_pairs_params{.minimum_distance = 0.5}};

        const auto input_wire = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);

        REQUIRE(input_wire.size() == 1);
        CHECK(input_wire[0].port.dir == port_direction::cardinal::WEST);
        CHECK(input_wire[0].pairs.size() == 4);
        CHECK(input_wire[0].pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
        CHECK(input_wire[0].pairs[1] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{18, 0, 0}, lattice_site{20, 0, 0}});
        CHECK(input_wire[0].pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 0, 1}, lattice_site{14, 0, 1}});
        CHECK(input_wire[0].pairs[3] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 1, 0}, lattice_site{8, 1, 0}});
    }

    SECTION("Wire with input BDL pair from west to east, but moving up and down, last BDL pair is below the input BDL "
            "pair --> moving south")
    {
        layout lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 0, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, 1, 0}, sidb_technology::cell_type::NORMAL);

        const detect_bdl_wires_params params{.threshold_bdl_interdistance = 2.5,
                                             .bdl_pairs_params = detect_bdl_pairs_params{.minimum_distance = 0.5}};

        const auto input_wire = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);

        REQUIRE(input_wire.size() == 1);
        CHECK(input_wire[0].port.dir == port_direction::cardinal::SOUTH);
        CHECK(input_wire[0].pairs.size() == 4);
        CHECK(input_wire[0].pairs[0] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
        CHECK(input_wire[0].pairs[1] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 0, 1}, lattice_site{14, 0, 1}});
        CHECK(input_wire[0].pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 1, 0}, lattice_site{8, 1, 0}});
        CHECK(input_wire[0].pairs[3] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{18, 1, 0}, lattice_site{20, 1, 0}});
    }

    SECTION("Wire with input BDL pair from west to east, but moving up and down, last BDL pair is above the input BDL "
            "pair --> moving north")
    {
        layout lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 0, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, -1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, -1, 1}, sidb_technology::cell_type::NORMAL);

        const detect_bdl_wires_params params{.threshold_bdl_interdistance = 3.0,
                                             .bdl_pairs_params = detect_bdl_pairs_params{.minimum_distance = 0.5}};

        const auto input_wire = detect_bdl_wires(lyt, params, bdl_wire_selection::INPUT);

        REQUIRE(input_wire.size() == 1);

        const auto& wire = input_wire.front();

        CHECK(wire.port.dir == port_direction::cardinal::NORTH);
        CHECK(wire.pairs.size() == 4);
        CHECK(wire.pairs[0] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{18, -1, 1}, lattice_site{20, -1, 1}});
        CHECK(wire.pairs[1] ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
        CHECK(wire.pairs[2] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{12, 0, 1}, lattice_site{14, 0, 1}});
        CHECK(wire.pairs[3] ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{6, 1, 0}, lattice_site{8, 1, 0}});
        REQUIRE(wire.first_bdl_pair.has_value());
        REQUIRE(wire.last_bdl_pair.has_value());
        CHECK(wire.first_bdl_pair ==
              bdl_pair{sidb_technology::cell_type::INPUT, lattice_site{0, 0, 0}, lattice_site{2, 0, 0}});
        CHECK(wire.last_bdl_pair ==
              bdl_pair{sidb_technology::cell_type::NORMAL, lattice_site{18, -1, 1}, lattice_site{20, -1, 1}});
    }
}
