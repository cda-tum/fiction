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
 * @brief Tests for `fiction/technology/sidb/io/write_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <tinyxml2.h>

#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::sidb;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;

TEST_CASE("Write empty SQD layout", "[sqd]")
{
    const sidb::layout lyt{sidb::lattice::si_100_2x1(), "empty"};

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream, "empty");

    CHECK(read_layout == lyt);
    CHECK(read_layout.is_empty());
    CHECK(read_layout.num_defects() == 0);
}

TEST_CASE("Write single-dot SQD layout", "[sqd]")
{
    sidb::layout lyt{};
    lyt.assign_cell_type(site_at_row(1, 2), sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream);

    CHECK(read_layout == lyt);
    CHECK(read_layout.num_cells() == 1);
    CHECK(read_layout.get_cell_type({1, 1, 0}) == sidb_technology::cell_type::NORMAL);
}

TEST_CASE("Write multi-dot SQD layout", "[sqd]")
{
    sidb::layout lyt{};
    lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(site_at_row(1, 1), sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(site_at_row(0, 2), sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(site_at_row(0, 3), sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(site_at_row(4, 4), sidb_technology::cell_type::NORMAL);

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream);

    CHECK(read_layout == lyt);
    CHECK(read_layout.num_cells() == 5);
    CHECK(read_layout.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{4, 2, 0}});
}

TEST_CASE("Write multi-dot SQD layout with differing dot types", "[sqd]")
{
    sidb::layout lyt{};
    lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(site_at_row(1, 1), sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(site_at_row(0, 2), sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(site_at_row(0, 3), sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(site_at_row(4, 4), sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(site_at_row(4, 6), sidb_technology::cell_type::LOGIC);

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream);

    CHECK(read_layout == lyt);
    CHECK(read_layout.num_pis() == 2);
    CHECK(read_layout.num_pos() == 2);
    CHECK(read_layout.get_cell_type({4, 3, 0}) == sidb_technology::cell_type::LOGIC);
}

TEST_CASE("Write Bestagon SQD layout", "[sqd]")
{
    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<coords::offset, even_row_hex>>>>;

    auto g_layout = blueprints::row_clocked_and_xor_gate_layout<gate_layout>();
    g_layout.set_layout_name("Bestagon");

    const auto c_layout = apply_gate_library<sidb_cell_clk_lyt, bestagon_library>(g_layout);
    const auto lyt      = to_sidb_layout(c_layout);

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream, "Bestagon");

    CHECK(read_layout == lyt);
    CHECK(read_layout.num_cells() == c_layout.num_cells());
    CHECK(read_layout.num_pis() == c_layout.num_pis());
    CHECK(read_layout.num_pos() == c_layout.num_pos());
}

TEST_CASE("Write defective surface SQD layout", "[sqd]")
{
    static const std::vector<std::pair<lattice_site, defect>> defects{
        {site_at_row(0, 0), defect{defect_type::NONE}},
        {site_at_row(0, 1), defect{defect_type::DB}},
        {site_at_row(0, 2), defect{defect_type::SI_VACANCY}},
        {site_at_row(0, 3), defect{defect_type::SINGLE_DIHYDRIDE}},
        {site_at_row(0, 4), defect{defect_type::DIHYDRIDE_PAIR}},
        {site_at_row(0, 5), defect{defect_type::ONE_BY_ONE}},
        {site_at_row(0, 6), defect{defect_type::THREE_BY_ONE}},
        {site_at_row(0, 7), defect{defect_type::SILOXANE}},
        {site_at_row(0, 8), defect{defect_type::RAISED_SI}},
        {site_at_row(0, 9), defect{defect_type::MISSING_DIMER}},
        {site_at_row(0, 10), defect{defect_type::ETCH_PIT}},
        {site_at_row(0, 11), defect{defect_type::STEP_EDGE}},
        {site_at_row(0, 12), defect{defect_type::GUNK}},
        {site_at_row(0, 13), defect{defect_type::UNKNOWN}}};

    sidb::layout lyt{};

    for (const auto& [s, d] : defects)
    {
        lyt.assign_defect(s, d);
    }

    // a NONE defect is no defect
    CHECK(lyt.num_defects() == defects.size() - 1);

    std::stringstream layout_stream{};

    write_sqd_layout(lyt, layout_stream);

    const auto read_layout = read_sqd_layout(layout_stream);

    CHECK(read_layout == lyt);
    CHECK(read_layout.is_empty());
    CHECK(read_layout.num_defects() == lyt.num_defects());

    for (const auto& [s, d] : defects)
    {
        CHECK(read_layout.get_defect(s) == d);
    }
}

TEST_CASE("Write and read back an sidb::layout", "[sqd]")
{
    SECTION("lattice names are XML text")
    {
        auto lat = sidb::lattice::si_100_2x1();
        lat.name = "custom & <surface>";
        const sidb::layout lyt{lat};
        std::stringstream  stream{};
        write_sqd_layout(lyt, stream);
        tinyxml2::XMLDocument document{};
        REQUIRE(document.Parse(stream.str().c_str()) == tinyxml2::XML_SUCCESS);
        const auto* name = document.FirstChildElement("siqad")
                               ->FirstChildElement("layers")
                               ->FirstChildElement("layer_prop")
                               ->FirstChildElement("lat_vec")
                               ->FirstChildElement("name");
        REQUIRE(name != nullptr);
        CHECK(std::string{name->GetText()} == lat.name);
        CHECK(read_sqd_layout(stream) == lyt);
    }
    SECTION("serialized geometry takes precedence over the lattice name")
    {
        for (const auto* name : {"custom", "Si(100) 2x1", "Si(111) 1x1", ""})
        {
            const sidb::lattice lat{.name  = name,
                                    .a1    = {4.0, 1.0},
                                    .a2    = {-2.0, 8.0},
                                    .basis = {{{0.0, 0.0}, {1.5, 2.5}}}};
            sidb::layout        lyt{lat};
            lyt.assign_cell_type({-3, 2, 1}, sidb_technology::cell_type::INPUT);
            lyt.assign_defect({1, -2, 0}, defect{defect_type::DB});
            std::stringstream stream{};
            write_sqd_layout(lyt, stream);
            CHECK(read_sqd_layout(stream) == lyt);
        }
    }
    SECTION("H-Si(100)-2x1 with defects")
    {
        sidb::layout lyt{sidb::lattice::si_100_2x1(), "Bestagon"};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({3, -1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({5, 2, 0}, sidb_technology::cell_type::LOGIC);
        lyt.assign_cell_type({8, 2, 1}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_defect({2, 1, 0}, defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});
        lyt.assign_defect({4, 1, 1}, defect{defect_type::DB});

        std::stringstream layout_stream{};

        write_sqd_layout(lyt, layout_stream);

        const auto read_layout = read_sqd_layout(layout_stream, "Bestagon");

        CHECK(read_layout == lyt);
    }
    SECTION("H-Si(111)-1x1")
    {
        sidb::layout lyt{sidb::lattice::si_111_1x1()};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::NORMAL);

        std::stringstream layout_stream{};

        write_sqd_layout(lyt, layout_stream);

        const auto read_layout = read_sqd_layout(layout_stream);

        CHECK(read_layout.get_lattice() == sidb::lattice::si_111_1x1());
        CHECK(read_layout == lyt);
        CHECK(layout_stream.str().find("Si(111) 1x1") != std::string::npos);
        CHECK(layout_stream.str().find("Defects") == std::string::npos);
    }
}
