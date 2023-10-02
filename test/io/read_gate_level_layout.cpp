//
// Created by simon on 25.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/read_gate_level_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/utils/name_utils.hpp>

#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

using namespace fiction;

TEST_CASE("Read empty FCN layout", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(get_name(lyt) == "Test");
        CHECK(lyt.is_clocking_scheme(clock_name::TWODDWAVE));
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    check(read_gate_level_layout<gate_layout>(layout_stream));
}

TEST_CASE("Read simple FCN layout", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

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
    check(read_gate_level_layout<gate_layout>(layout_stream));
}

TEST_CASE("Parsing error: malformed xml", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no root element 'fcn'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'layout' in 'fcn'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'clocking' in 'layout'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
                                              "  <layout>\n"
                                              "    <name>Test</name>\n"
                                              "    <topology>cartesian</topology>\n"
                                              "    <size>\n"
                                              "      <x>0</x>\n"
                                              "      <y>0</y>\n"
                                              "      <z>0</z>\n"
                                              "    </size>\n"
                                              "  </layout>\n"
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'name' in 'clocking'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown clocking scheme", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'zones' in 'clocking'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'zone'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'zone'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'clock' in 'zone'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'size' in 'layout'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'size'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'size'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'size'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'id' in 'gate", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'type' in 'gate", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'name' in 'gate", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'loc' in 'gate'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'loc'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'loc'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'loc'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 0 incoming signals", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 1 incoming signal", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 2 incoming signals", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with 3 incoming signals", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: unknown gate type with more than 3 incoming signals", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'x' in 'signal'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'y' in 'signal'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}

TEST_CASE("Parsing error: no element 'z' in 'signal'", "[fcn]")
{
    static constexpr const char* fcn_layout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                              "<fcn>\n"
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
                                              "</fcn>\n";

    std::istringstream layout_stream{fcn_layout};

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    CHECK_THROWS_AS(read_gate_level_layout<gate_layout>(layout_stream), gate_level_parsing_error);
}