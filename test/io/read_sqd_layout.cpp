//
// Created by marcel on 23.06.22.
//

#include "catch.hpp"

#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_surface.hpp>

#include <set>
#include <sstream>
#include <string>
#include <type_traits>

using namespace fiction;

TEST_CASE("Read empty layout", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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
    check(read_sqd_layout<sidb_layout>(layout_stream));
}

TEST_CASE("Read single-dot layout", "[sqd]")
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
    check(read_sqd_layout<sidb_layout>(layout_stream));
}

TEST_CASE("Read multi-dot layout", "[sqd]")
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

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    const auto layout = read_sqd_layout<sidb_layout>(layout_stream);

    CHECK(layout.x() == 2);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);
}

TEST_CASE("Read single defect", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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
                                              "          <coulomb charge=\"2\" eps_r=\"-1.2\" lambda_tf=\"3.4\" />\n"
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_layout>(layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 4);
    CHECK(layout.is_empty());

    const auto defect = layout.get_sidb_defect({5, 4});
    CHECK(defect.type == sidb_defect_type::UNKNOWN);
    CHECK(defect.charge == 2);
    CHECK(defect.epsilon_r == -1.2);
    CHECK(defect.lambda_tf == 3.4);
}

TEST_CASE("Read multiple defects", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
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
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_layout>(layout_stream);

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

TEST_CASE("Read multi-dot layout with multi-cell defect", "[sqd]")
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
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto layout = read_sqd_layout<sidb_layout>(layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);

    const std::set<cell<sidb_layout>> defect_positions{{5, 4}, {5, 5}, {4, 4}, {4, 5}};

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

TEST_CASE("In-place reader with ignored defects", "[sqd]")
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
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    sidb_surface_params params{std::set<sidb_defect_type>{sidb_defect_type::DB}};
    sidb_layout         layout{params};

    read_sqd_layout(layout, layout_stream);

    CHECK(layout.x() == 5);
    CHECK(layout.y() == 5);

    CHECK(layout.get_cell_type({0, 0}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({0, 1}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 4}) == sidb_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 5}) == sidb_technology::cell_type::NORMAL);

    // should have ignored the DB defects
    CHECK(layout.num_defects() == 0);
}

TEST_CASE("Read defect despite missing <coulomb> element", "[sqd]")
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
                                              "      </defect>"
                                              "    </layer>\n"
                                              "  </design>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    const auto layout = read_sqd_layout<sidb_layout>(layout_stream);

    const auto defect = layout.get_sidb_defect({0, 0});

    CHECK(defect.type == sidb_defect_type::UNKNOWN);

    CHECK(defect.charge == 0.0);
    CHECK(defect.epsilon_r == 0.0);
    CHECK(defect.lambda_tf == 0.0);
}

TEST_CASE("Parsing error: missing <siqad> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing <design> element", "[sqd]")
{
    static constexpr const char* sqd_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<siqad>\n"
                                              "</siqad>\n";

    std::istringstream layout_stream{sqd_layout};

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'type' attribute in <layer> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing <latcoord> element in <dbdot> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'n' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'm' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'l' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: negative 'n' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: negative 'm' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: negative 'l' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: out-of-bounds 'l' attribute in <latcoord> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing <latcoord> element in <incl_coords> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'charge' attribute in <coulomb> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'eps_r' attribute in <coulomb> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}

TEST_CASE("Parsing error: missing 'lambda_tf' attribute in <coulomb> element", "[sqd]")
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

    using sidb_layout =
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>;

    CHECK_THROWS_AS(read_sqd_layout<sidb_layout>(layout_stream), sqd_parsing_error);
}
