//
// Created by marcel on 23.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/read_sqd_layout.hpp>
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

#include <sstream>
#include <unordered_set>

using namespace fiction;

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

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(lyt.is_empty_cell({0, 0}));
    };

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    check(read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream));
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

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(lyt.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    };

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    check(read_sqd_layout<sidb_lattice<sidb_111_lattice, sidb_layout>>(layout_stream));
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

    SECTION("Fiction coordinates")
    {
        using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
        const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

        CHECK(layout.x() == 2);
        CHECK(layout.y() == 5);

        CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);
    }

    SECTION("SiQAD coordinates")
    {
        using siqad_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;
        const auto layout  = read_sqd_layout<sidb_lattice<sidb_100_lattice, siqad_layout>>(layout_stream);

        CHECK(layout.x() == 2);
        CHECK(layout.y() == 2);
        CHECK(layout.z() == 1);

        CHECK(layout.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({0, 0, 1}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({2, 2, 0}) == sidb_technology::cell_type::NORMAL);
        CHECK(layout.get_cell_type({2, 2, 1}) == sidb_technology::cell_type::NORMAL);
    }
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

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    CHECK(layout.x() == 2);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::OUTPUT);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({1, 3}) == sidb_technology::cell_type::LOGIC);
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

    const auto layout = read_sqd_layout<sidb_111_cell_clk_lyt>(layout_stream);

    CHECK(layout.x() == 2);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::OUTPUT);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);
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

    using sidb_layout =
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 4);
    CHECK(layout.is_empty());

    const auto defect = layout.get_sidb_defect({5, 4});
    CHECK(defect.type == sidb_defect_type::UNKNOWN);
    CHECK(defect.charge == 2);
    CHECK(defect.epsilon_r == 1.2);
    CHECK(defect.lambda_tf == 3.4);
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

    using sidb_layout =
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 5);
    CHECK(layout.is_empty());
    {
        const auto defect = layout.get_sidb_defect({5, 4});
        CHECK(defect.type == sidb_defect_type::SILOXANE);
        CHECK(defect.charge == -1);
        CHECK(defect.epsilon_r == 5.6);
        CHECK(defect.lambda_tf == 5.0);
    }
    {
        const auto defect1 = layout.get_sidb_defect({3, 4});
        const auto defect2 = layout.get_sidb_defect({3, 5});
        CHECK(defect1.type == sidb_defect_type::MISSING_DIMER);
        CHECK(defect1.charge == -1);
        CHECK(defect1.epsilon_r == 5.6);
        CHECK(defect1.lambda_tf == 5.0);
        CHECK(defect2.type == sidb_defect_type::MISSING_DIMER);
        CHECK(defect2.charge == -1);
        CHECK(defect2.epsilon_r == 5.6);
        CHECK(defect2.lambda_tf == 5.0);
    }
    {
        const auto defect1 = layout.get_sidb_defect({0, 4});
        const auto defect2 = layout.get_sidb_defect({0, 5});
        const auto defect3 = layout.get_sidb_defect({1, 4});
        const auto defect4 = layout.get_sidb_defect({1, 5});
        CHECK(defect1.type == sidb_defect_type::ETCH_PIT);
        CHECK(defect1.charge == -1);
        CHECK(defect1.epsilon_r == 5.6);
        CHECK(defect1.lambda_tf == 5.0);
        CHECK(defect2.type == sidb_defect_type::ETCH_PIT);
        CHECK(defect2.charge == -1);
        CHECK(defect2.epsilon_r == 5.6);
        CHECK(defect2.lambda_tf == 5.0);
        CHECK(defect3.type == sidb_defect_type::ETCH_PIT);
        CHECK(defect3.charge == -1);
        CHECK(defect3.epsilon_r == 5.6);
        CHECK(defect3.lambda_tf == 5.0);
        CHECK(defect4.type == sidb_defect_type::ETCH_PIT);
        CHECK(defect4.charge == -1);
        CHECK(defect4.epsilon_r == 5.6);
        CHECK(defect4.lambda_tf == 5.0);
    }
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

    using sidb_layout =
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);

    const std::unordered_set<cell<sidb_layout>> defect_positions{{5, 4}, {5, 5}, {4, 4}, {4, 5}};

    layout.foreach_sidb_defect(
        [&defect_positions](const auto& cd)
        {
            const auto& cell   = cd.first;
            const auto& defect = cd.second;

            CHECK(defect_positions.count(cell) > 0);

            CHECK(defect.type == sidb_defect_type::DB);

            CHECK(defect.charge == -1);
            CHECK(defect.epsilon_r == 5.6);
            CHECK(defect.lambda_tf == 5.0);
        });
}

TEST_CASE("In-place SQD reader with ignored defects", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "  <layers>\n"
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

    using sidb_layout =
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    const sidb_defect_surface_params            params{std::unordered_set<sidb_defect_type>{sidb_defect_type::DB}};
    const sidb_layout                           layout{params};
    sidb_lattice<sidb_111_lattice, sidb_layout> lattice_layout{layout};

    read_sqd_layout(lattice_layout, layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);

    // should have ignored the DB defects
    CHECK(layout.num_defects() == 0);
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

    using sidb_layout =
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    const auto layout = read_sqd_layout<sidb_lattice<sidb_100_lattice, sidb_layout>>(layout_stream);

    const auto defect = layout.get_sidb_defect({0, 0});

    CHECK(defect.type == sidb_defect_type::UNKNOWN);

    CHECK(defect.charge == 0);
    CHECK(defect.epsilon_r == 0.0);
    CHECK(defect.lambda_tf == 0.0);
}

TEST_CASE("SQD parsing error: missing <siqad> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <design> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'type' attribute in <layer> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "  <design>\n"
                                              "    <layer/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <latcoord> element in <dbdot> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'n' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'm' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'l' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
    ;
}

TEST_CASE("SQD parsing error: negative 'n' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"-1\" m=\"0\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: negative 'm' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"-3\" l=\"0\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: negative 'l' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: out-of-bounds 'l' attribute in <latcoord> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "  <design>\n"
                                              "    <layer type=\"Lattice\"/>\n"
                                              "    <layer type=\"Misc\"/>\n"
                                              "    <layer type=\"Electrode\"/>\n"
                                              "    <layer type=\"DB\">\n"
                                              "      <dbdot>\n"
                                              "          <layer_id>2</layer_id>\n"
                                              "          <latcoord n=\"0\" m=\"0\" l=\"2\"/>\n"
                                              "      </dbdot>\n"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <latcoord> element in <incl_coords> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing <type> element in <dbdot> element")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: invalid <type> element in <dbdot> element")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'charge' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'eps_r' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("SQD parsing error: missing 'lambda_tf' attribute in <coulomb> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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

    using sidb_layout = sidb_lattice<
        sidb_100_lattice,
        sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>>;
    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}
