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
 * @brief Tests for `fiction/technology/sidb/io/read_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;

TEST_CASE("Read empty SQD layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\"/>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.get_lattice() == sidb::lattice::si_100_2x1());
    CHECK(lyt.is_empty());
    CHECK(lyt.num_cells() == 0);
    CHECK(lyt.num_defects() == 0);
    CHECK(lyt.is_empty_cell({0, 0, 0}));
}

TEST_CASE("Read single-dot SQD layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <name>Si(111) 1x1</name>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <physloc x=\"0\" y=\"0\"/>\n"
                                              "          <color>#ffc8c8c8</color>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.get_lattice().name == "Si(111) 1x1");
    CHECK(lyt.num_cells() == 1);
    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{0, 0, 0}});
}

TEST_CASE("Read multi-dot SQD layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.get_lattice() == sidb::lattice::si_100_2x1());
    CHECK(lyt.num_cells() == 4);
    CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{2, 2, 1}});

    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);
}

TEST_CASE("Read multi-dot SQD layout with cell type definitions", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <type>input</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"1\"/>\n"
                                              "          <type>output</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"0\"/>\n"
                                              "          <type>normal</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"1\" m=\"1\" l=\"1\"/>\n"
                                              "          <type>logic</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.num_cells() == 5);
    CHECK(lyt.num_pis() == 1);
    CHECK(lyt.num_pos() == 1);

    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::OUTPUT);
    CHECK(lyt.get_cell_type({2, 2, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({1, 1, 1}) == sidb_technology::cell_type::LOGIC);
}

TEST_CASE("Read multi-dot SQD layout with cell type definitions, Si-111", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <name>Si(111) 1x1</name>\n"
                                              "                <a1 x=\"6.65\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"3.84\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "              <b2 x=\"3.3255\" y=\"1.92\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <type>input</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"1\"/>\n"
                                              "          <type>output</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"0\"/>\n"
                                              "          <type>normal</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.get_lattice() == sidb::lattice::si_111_1x1());
    CHECK(lyt.num_cells() == 4);

    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::OUTPUT);
    CHECK(lyt.get_cell_type({2, 2, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);
}

TEST_CASE("Read single defect SQD layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "    </layer>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"2\" eps_r=\"1.2\" lambda_tf=\"3.4\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.is_empty());
    CHECK(lyt.num_defects() == 1);
    CHECK(lyt.bounding_box() == std::pair{lattice_site{5, 2, 0}, lattice_site{5, 2, 0}});

    const auto d = lyt.get_defect({5, 2, 0});
    CHECK(d.type == defect_type::UNKNOWN);
    CHECK(d.charge == 2);
    CHECK(d.epsilon_r == 1.2);
    CHECK(d.lambda_tf == 3.4);
}

TEST_CASE("Read multiple defects SQD layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "    </layer>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "          <property_map>\n"
                                              "              <type_label>\n"
                                              "                  <val>siloxane</val>\n"
                                              "              </type_label>\n"
                                              "          </property_map>\n"
                                              "      </defect>\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"3\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"3\" m=\"2\" l=\"1\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "          <property_map>\n"
                                              "              <type_label>\n"
                                              "                  <val>Missing_Dimer</val>\n"
                                              "              </type_label>\n"
                                              "          </property_map>\n"
                                              "      </defect>\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"0\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"0\" m=\"2\" l=\"1\" />\n"
                                              "              <latcoord n=\"1\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"1\" m=\"2\" l=\"1\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "          <property_map>\n"
                                              "              <type_label>\n"
                                              "                  <val>Etch_Pit</val>\n"
                                              "              </type_label>\n"
                                              "          </property_map>\n"
                                              "      </defect>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.is_empty());
    CHECK(lyt.num_defects() == 7);
    CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 2, 0}, lattice_site{5, 2, 1}});

    CHECK(lyt.get_defect({5, 2, 0}) == defect{defect_type::SILOXANE, -1, 5.6, 5.0});

    CHECK(lyt.get_defect({3, 2, 0}) == defect{defect_type::MISSING_DIMER, -1, 5.6, 5.0});
    CHECK(lyt.get_defect({3, 2, 1}) == defect{defect_type::MISSING_DIMER, -1, 5.6, 5.0});

    CHECK(lyt.get_defect({0, 2, 0}) == defect{defect_type::ETCH_PIT, -1, 5.6, 5.0});
    CHECK(lyt.get_defect({0, 2, 1}) == defect{defect_type::ETCH_PIT, -1, 5.6, 5.0});
    CHECK(lyt.get_defect({1, 2, 0}) == defect{defect_type::ETCH_PIT, -1, 5.6, 5.0});
    CHECK(lyt.get_defect({1, 2, 1}) == defect{defect_type::ETCH_PIT, -1, 5.6, 5.0});
}

TEST_CASE("Read multi-dot SQD layout with multi-cell defect", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"1\" />\n"
                                              "              <latcoord n=\"4\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"4\" m=\"2\" l=\"1\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "          <property_map>\n"
                                              "              <type_label>\n"
                                              "                  <val>DB</val>\n"
                                              "              </type_label>\n"
                                              "          </property_map>\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.num_cells() == 4);
    CHECK(lyt.bounding_box() == std::pair{lattice_site{0, 0, 0}, lattice_site{5, 2, 1}});

    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);

    const std::unordered_set<lattice_site> defect_positions{{5, 2, 0}, {5, 2, 1}, {4, 2, 0}, {4, 2, 1}};

    CHECK(lyt.num_defects() == defect_positions.size());

    lyt.foreach_defect(
        [&defect_positions](const auto& sd)
        {
            const auto& [s, d]{sd};

            CHECK(defect_positions.count(s) > 0);

            CHECK(d == defect{defect_type::DB, -1, 5.6, 5.0});
        });
}

TEST_CASE("Read SQD defect despite missing <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <name>Lattice</name>\n"
                                              "            <type>Lattice</type>\n"
                                              "            <role>Design</role>\n"
                                              "            <zoffset>0</zoffset>\n"
                                              "            <zheight>0</zheight>\n"
                                              "            <visible>1</visible>\n"
                                              "            <active>0</active>\n"
                                              "            <lat_vec>\n"
                                              "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"0\" m=\"0\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.num_defects() == 1);

    const auto d = lyt.get_defect({0, 0, 0});

    CHECK(d.type == defect_type::UNKNOWN);

    CHECK(d.charge == 0);
    CHECK(d.epsilon_r == 0.0);
    CHECK(d.lambda_tf == 0.0);
}

TEST_CASE("SQD parsing error: missing <siqad> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <design> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'type' attribute in <layer> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <latcoord> element in <dbdot> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'n' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord m=\"0\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'm' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'l' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: negative 'l' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"-15\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <latcoord> element in <incl_coords> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <type> element in <dbdot> element")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <type></type>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: invalid <type> element in <dbdot> element")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <type>invalid</type>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'charge' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"0\" m=\"0\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'eps_r' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"0\" m=\"0\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" lambda_tf=\"5\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'lambda_tf' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec/>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"0\" m=\"0\" l=\"0\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
}

TEST_CASE("Read SQD layout into an sidb::layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec>\n"
                                              "                <name>Si(111) 1x1</name>\n"
                                              "                <a1 x=\"6.65\" y=\"0\"/>\n"
                                              "                <a2 x=\"0\" y=\"3.84\"/>\n"
                                              "                <N>2</N>\n"
                                              "                <b1 x=\"0\" y=\"0\"/>\n"
                                              "                <b2 x=\"3.3255\" y=\"1.92\"/>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"0\"/>\n"
                                              "          <type>input</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"1\"/>\n"
                                              "          <type>output</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"-2\" m=\"2\" l=\"0\"/>\n"
                                              "          <type>logic</type>\n"
                                              "      </dbdot>\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"2\" m=\"2\" l=\"1\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "    <layer type=\"Defects\">\n"
                                              "      <defect>\n"
                                              "          <layer_id>5</layer_id>\n"
                                              "          <incl_coords>\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"0\" />\n"
                                              "              <latcoord n=\"5\" m=\"2\" l=\"1\" />\n"
                                              "          </incl_coords>\n"
                                              "          <coulomb charge=\"-1\" eps_r=\"5.6\" lambda_tf=\"5\" />\n"
                                              "          <property_map>\n"
                                              "              <type_label>\n"
                                              "                  <val>Vacancy</val>\n"
                                              "              </type_label>\n"
                                              "          </property_map>\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream, "gate");

    CHECK(lyt.get_layout_name() == "gate");
    CHECK(lyt.get_lattice() == sidb::lattice::si_111_1x1());
    CHECK(lyt.num_cells() == 4);
    CHECK(lyt.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(lyt.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::OUTPUT);
    CHECK(lyt.get_cell_type({-2, 2, 0}) == sidb_technology::cell_type::LOGIC);
    CHECK(lyt.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_defects() == 2);
    CHECK(lyt.get_defect({5, 2, 0}) == defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});
    CHECK(lyt.get_defect({5, 2, 1}) == defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});
}

TEST_CASE("Read SQD layout into an sidb::layout without a lattice name", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "    <layers>\n"
                                              "        <layer_prop>\n"
                                              "            <lat_vec>\n"
                                              "            </lat_vec>\n"
                                              "        </layer_prop>\n"
                                              "    </layers>\n"
                                              "  <design>\n"
                                              "    <layer type=\"DB\"/>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    const auto lyt = read_sqd_layout(layout_stream);

    CHECK(lyt.get_lattice() == sidb::lattice::si_100_2x1());
    CHECK(lyt.is_empty());
}

TEST_CASE("Malformed SQD defect values", "[sqd]")
{
    for (const auto* value :
         {"<property_map><type_label><val/></type_label></property_map>",
          R"(<coulomb charge="x" eps_r="5.6" lambda_tf="5"/>)",
          R"(<coulomb charge="999999999999999999999999" eps_r="5.6" lambda_tf="5"/>)",
          R"(<coulomb charge="-1" eps_r="x" lambda_tf="5"/>)", R"(<coulomb charge="-1" eps_r="1e9999" lambda_tf="5"/>)",
          R"(<coulomb charge="-1" eps_r="-1" lambda_tf="5"/>)", R"(<coulomb charge="-1" eps_r="nan" lambda_tf="5"/>)",
          R"(<coulomb charge="-1" eps_r="5.6" lambda_tf="x"/>)",
          R"(<coulomb charge="-1" eps_r="5.6" lambda_tf="1e9999"/>)",
          R"(<coulomb charge="-1" eps_r="5.6" lambda_tf="-1"/>)",
          R"(<coulomb charge="-1" eps_r="5.6" lambda_tf="inf"/>)"})
    {
        const std::string document =
            std::string{"<siqad><layers><layer_prop><lat_vec/></layer_prop></layers><design>"
                        R"(<layer type="Defects"><defect><incl_coords><latcoord n="0" m="0" l="0"/>)"
                        "</incl_coords>"} +
            value + "</defect></layer></design></siqad>";
        std::istringstream stream{document};
        CHECK_THROWS_AS(read_sqd_layout(stream), sqd_parsing_error);
    }
}

TEST_CASE("SQD parsing error into an sidb::layout", "[sqd]")
{
    SECTION("invalid numeric coordinates")
    {
        for (const auto* attributes : {R"(n="x" m="0" l="0")", R"(n="0" m="x" l="0")", R"(n="0" m="0" l="x")",
                                       R"(n="999999999999999999999999" m="0" l="0")", R"(n="2147483648" m="0" l="0")",
                                       R"(n="0" m="-2147483649" l="0")"})
        {
            std::istringstream stream{std::string{"<siqad><layers><layer_prop><lat_vec/></layer_prop></layers><design>"
                                                  R"(<layer type="DB"><dbdot><latcoord )"} +
                                      attributes + " /></dbdot></layer></design></siqad>"};
            CHECK_THROWS_AS(read_sqd_layout(stream), sqd_parsing_error);
        }
    }
    SECTION("malformed explicit lattice geometry")
    {
        const std::string geometry{R"(<name>custom</name><a1 x="4" y="1"/><a2 x="-2" y="8"/>)"
                                   R"(<N>2</N><b1 x="0" y="0"/><b2 x="1.5" y="2.5"/>)"};
        for (const auto& replacement :
             {std::pair{R"(<a1 x="4" y="1"/>)", ""}, std::pair{R"(<a2 x="-2" y="8"/>)", ""},
              std::pair{R"(<b1 x="0" y="0"/>)", ""}, std::pair{R"(<b2 x="1.5" y="2.5"/>)", ""},
              std::pair{"<N>2</N>", ""}, std::pair{"<N>2</N>", "<N/>"}, std::pair{"<N>2</N>", "<N>1</N>"},
              std::pair{"<N>2</N>", "<N>3</N>"}, std::pair{"<N>2</N>", "<N>2.5</N>"},
              std::pair{"<N>2</N>", "<N>2junk</N>"}, std::pair{R"(x="4")", ""}, std::pair{R"(y="1")", ""},
              std::pair{R"(x="4")", R"(x="")"}, std::pair{R"(x="4")", R"(x="nan")"},
              std::pair{R"(x="4")", R"(x="inf")"}, std::pair{R"(x="4")", R"(x="1e999")"},
              std::pair{R"(x="4")", R"(x="text")"}, std::pair{R"(x="4")", R"(x="4junk")"}})
        {
            CAPTURE(replacement.first, replacement.second);
            auto malformed = geometry;
            malformed.replace(malformed.find(replacement.first), std::string{replacement.first}.size(),
                              replacement.second);
            std::istringstream stream{"<siqad><layers><layer_prop><lat_vec>" + malformed +
                                      "</lat_vec></layer_prop></layers><design/></siqad>"};
            CHECK_THROWS_AS(read_sqd_layout(stream), sqd_parsing_error);
        }
    }
    SECTION("missing <layers> element")
    {
        static constexpr const char* sqd_layout = "<siqad>\n"
                                                  "  <design/>\n"
                                                  "</siqad>\n";

        std::istringstream layout_stream{sqd_layout};

        CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
    }
    SECTION("missing <lat_vec> element")
    {
        static constexpr const char* sqd_layout = "<siqad>\n"
                                                  "    <layers>\n"
                                                  "        <layer_prop/>\n"
                                                  "    </layers>\n"
                                                  "  <design/>\n"
                                                  "</siqad>\n";

        std::istringstream layout_stream{sqd_layout};

        CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
    }
    SECTION("unknown lattice")
    {
        static constexpr const char* sqd_layout = "<siqad>\n"
                                                  "    <layers>\n"
                                                  "        <layer_prop>\n"
                                                  "            <lat_vec>\n"
                                                  "                <name>Si(110) 1x1</name>\n"
                                                  "            </lat_vec>\n"
                                                  "        </layer_prop>\n"
                                                  "    </layers>\n"
                                                  "  <design/>\n"
                                                  "</siqad>\n";

        std::istringstream layout_stream{sqd_layout};

        CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
    }
    SECTION("out-of-bounds basis site")
    {
        static constexpr const char* sqd_layout = "<siqad>\n"
                                                  "    <layers>\n"
                                                  "        <layer_prop>\n"
                                                  "            <lat_vec/>\n"
                                                  "        </layer_prop>\n"
                                                  "    </layers>\n"
                                                  "  <design>\n"
                                                  "    <layer type=\"DB\">\n"
                                                  "      <dbdot>\n"
                                                  "          <latcoord n=\"0\" m=\"0\" l=\"2\"/>\n"
                                                  "      </dbdot>\n"
                                                  "    </layer>\n"
                                                  "  </design>\n"
                                                  "</siqad>\n";

        std::istringstream layout_stream{sqd_layout};

        CHECK_THROWS_AS(read_sqd_layout(layout_stream), sqd_parsing_error);
    }
    SECTION("missing file")
    {
        CHECK_THROWS_AS(read_sqd_layout("/this/file/does/not/exist.sqd"), std::ifstream::failure);
    }
}
