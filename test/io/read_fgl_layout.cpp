//
// Created by simon on 25.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/read_fgl_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/utils/name_utils.hpp>

#include <sstream>

using namespace fiction;

TEST_CASE("Read empty FGL layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(get_name(lyt) == "Test");
        CHECK(lyt.is_clocking_scheme(clock_name::TWODDWAVE));
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    check(read_fgl_layout<gate_layout>(layout_stream));
}

TEST_CASE("Read simple FGL layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 2);
        CHECK(lyt.y() == 1);
        CHECK(lyt.area() == 6);
        CHECK(get_name(lyt) == "Test");
        CHECK(lyt.is_clocking_scheme(clock_name::TWODDWAVE));
        CHECK(lyt.is_pi_tile({0, 1}));
        CHECK(lyt.get_name(lyt.get_node({0, 1})) == "pi0");
        CHECK(lyt.is_pi_tile({1, 0}));
        CHECK(lyt.get_name(lyt.get_node({1, 0})) == "pi1");
        CHECK(lyt.is_and(lyt.get_node({1, 1})));
        CHECK(lyt.is_po_tile({2, 1}));
        CHECK(lyt.get_name(lyt.get_node({2, 1})) == "po0");
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    check(read_fgl_layout<gate_layout>(layout_stream));
}

TEST_CASE("Parsing error: malformed xml", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no root element 'fgl'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'layout' in 'fgl'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'clocking' in 'layout'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'name' in 'clocking'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown clocking scheme", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>CoolClocking</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'zones' in 'clocking'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>OPEN</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'zone'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>OPEN</name>\n"
                                              "      <zones>\n"
                                              "        <zone>\n"
                                              "          <y>0</y>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "      </zones>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'zone'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>OPEN</name>\n"
                                              "      <zones>\n"
                                              "        <zone>\n"
                                              "          <x>0</x>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "      </zones>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'clock' in 'zone'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>OPEN</name>\n"
                                              "      <zones>\n"
                                              "        <zone>\n"
                                              "          <x>0</x>\n"
                                              "          <y>0</y>\n"
                                              "        </zone>\n"
                                              "      </zones>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'topology' in 'layout'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown topology", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>unknown</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not a cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not a shifted_cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_row_cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an odd_row_cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_row_cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, even_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an even_row_cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>even_row_cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an odd_column_cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_column_cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an even_column_cartesian layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>even_column_cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not a hexagonal layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_row_hex</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<
        clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an odd_row_hex layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_row_hex</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an even_row_hex layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>even_row_hex</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an odd_column_hex layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>odd_column_hex</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: Lyt is not an even_column_hex layout", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>even_column_hex</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout =
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'size' in 'layout'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'size'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'size'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'size'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'id' in 'gate", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'type' in 'gate", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'name' in 'gate", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'loc' in 'gate'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'loc'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'loc'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'loc'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 0 incoming signals", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>unknown</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 1 incoming signal", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>unknown</type>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 2 incoming signals", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>unknown</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 3 incoming signals", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>2</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>RES</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi2</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>unknown</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>2</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>4</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>2</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with more than 3 incoming signals", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>2</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>OPEN</name>\n"
                                              "      <zones>\n"
                                              "        <zone>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "        <zone>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "        <zone>\n"
                                              "          <x>2</x>\n"
                                              "          <y>1</y>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "        <zone>\n"
                                              "          <x>1</x>\n"
                                              "          <y>2</y>\n"
                                              "          <clock>0</clock>\n"
                                              "        </zone>\n"
                                              "        <zone>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <clock>1</clock>\n"
                                              "        </zone>\n"
                                              "      </zones>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi2</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi3</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>2</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>4</id>\n"
                                              "      <type>unknown</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>2</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>2</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'signal'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'signal'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'signal'", "[read-fgl-layout]")
{
    static constexpr const char* fgl_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fgl>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>2</x>\n"
                                              "      <y>1</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "    <clocking>\n"
                                              "      <name>2DDWave</name>\n"
                                              "    </clocking>\n"
                                              "  </layout>\n"
                                              "  <gates>\n"
                                              "    <gate>\n"
                                              "      <id>0</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi0</name>\n"
                                              "      <loc>\n"
                                              "        <x>0</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>1</id>\n"
                                              "      <type>PI</type>\n"
                                              "      <name>pi1</name>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>0</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>2</id>\n"
                                              "      <type>AND</type>\n"
                                              "      <loc>\n"
                                              "        <x>1</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>0</x>\n"
                                              "          <y>1</y>\n"
                                              "        </signal>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>0</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "    <gate>\n"
                                              "      <id>3</id>\n"
                                              "      <type>PO</type>\n"
                                              "      <name>po0</name>\n"
                                              "      <loc>\n"
                                              "        <x>2</x>\n"
                                              "        <y>1</y>\n"
                                              "        <z>0</z>\n"
                                              "      </loc>\n"
                                              "      <incoming>\n"
                                              "        <signal>\n"
                                              "          <x>1</x>\n"
                                              "          <y>1</y>\n"
                                              "          <z>0</z>\n"
                                              "        </signal>\n"
                                              "      </incoming>\n"
                                              "    </gate>\n"
                                              "  </gates>\n"
                                              "</fgl>\n";

    std::istringstream layout_stream{fgl_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}
