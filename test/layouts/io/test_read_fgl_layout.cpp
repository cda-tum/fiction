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
 * @brief Tests for `fiction/layouts/io/read_fgl_layout.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/read_fgl_layout.hpp>
#include <fiction/layouts/io/write_fgl_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/name_utils.hpp>

#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::layouts::io;
using namespace fiction::networks;

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
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(lyt.is_pi_tile({0, 1}));
        CHECK(lyt.get_name(lyt.get_node({0, 1})) == "pi0");
        CHECK(lyt.is_pi_tile({1, 0}));
        CHECK(lyt.get_name(lyt.get_node({1, 0})) == "pi1");
        CHECK(lyt.is_and(lyt.get_node({1, 1})));
        CHECK(lyt.is_po_tile({2, 1}));
        CHECK(lyt.get_name(lyt.get_node({2, 1})) == "po0");
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
    check(read_fgl_layout<gate_layout>(layout_stream));
}

TEST_CASE("Read FGL layout with hexadecimal gate type", "[read-fgl-layout]")
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
                                              "      <type>B</type>\n"
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
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(lyt.is_pi_tile({0, 1}));
        CHECK(lyt.get_name(lyt.get_node({0, 1})) == "pi0");
        CHECK(lyt.is_pi_tile({1, 0}));
        CHECK(lyt.get_name(lyt.get_node({1, 0})) == "pi1");
        CHECK(lyt.is_le(lyt.get_node({1, 1})));
        CHECK(lyt.is_po_tile({2, 1}));
        CHECK(lyt.get_name(lyt.get_node({2, 1})) == "po0");
    };

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, odd_row_cartesian>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, even_row_cartesian>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, odd_row_cartesian>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, odd_row_cartesian>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, odd_row_cartesian>>>>;
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
        clocked_layout<tile_based_layout<shifted_cartesian_layout<coords::offset, odd_row_cartesian>>>>;
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
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<coords::offset, even_row_hex>>>>;
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
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<coords::offset, odd_row_hex>>>>;
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
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<coords::offset, odd_row_hex>>>>;
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
        gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<coords::offset, odd_row_hex>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
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

    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("FGL preserves synchronization elements and labels", "[read-fgl-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
    using sync_layout = gate_level_layout<
        synchronization_element_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>>;
    sync_layout original{{2, 0, 0}};
    original.set_layout_name("A & B < C");
    const auto input = original.create_pi("in<&>", {0, 0, 0});
    original.create_po(input, "out<&>", {1, 0, 0});
    original.assign_synchronization_element({0, 0, 0}, 2);
    original.assign_synchronization_element({1, 0, 0}, 255);
    std::stringstream stream{};
    write_fgl_layout(original, stream);
    const auto restored = read_fgl_layout<sync_layout>(stream);
    CHECK(restored.get_layout_name() == original.get_layout_name());
    CHECK(restored.num_pis() == 1);
    CHECK(restored.num_pos() == 1);
    CHECK(restored.get_name(restored.get_node({0, 0, 0})) == "in<&>");
    CHECK(restored.get_name(restored.get_node({1, 0, 0})) == "out<&>");
    CHECK(restored.num_se() == 2);
    CHECK(restored.get_synchronization_element({0, 0, 0}) == 2);
    CHECK(restored.get_synchronization_element({1, 0, 0}) == 255);
    stream.clear();
    stream.seekg(0);
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(stream), fgl_parsing_error);
}

TEST_CASE("FGL rejects invalid numeric metadata", "[read-fgl-layout]")
{
    using sync_layout = gate_level_layout<
        synchronization_element_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>>;
    std::string x{"0"};
    std::string clock{"0"};
    std::string delay{"0"};
    std::string id{"0"};
    SECTION("Negative gate ID")
    {
        id = "-1";
    }
    SECTION("Trailing gate ID data")
    {
        id = "1oops";
    }
    SECTION("Non-numeric gate ID")
    {
        id = "gate";
    }
    SECTION("Empty gate ID")
    {
        id = " ";
    }
    SECTION("Gate ID storage overflow")
    {
        id = "2147483648";
    }
    SECTION("Gate ID integer overflow")
    {
        id = "18446744073709551616";
    }
    SECTION("Negative coordinate")
    {
        x = "-1";
    }
    SECTION("Trailing coordinate data")
    {
        x = "1oops";
    }
    SECTION("Empty coordinate")
    {
        x = "   ";
    }
    SECTION("Coordinate overflow")
    {
        x = "4294967296";
    }
    SECTION("Integer overflow")
    {
        x = "18446744073709551616";
    }
    SECTION("Clock exceeds phase count")
    {
        clock = "4";
    }
    SECTION("Synchronization delay overflow")
    {
        delay = "256";
    }
    std::istringstream stream{
        "<fgl><layout><name>invalid</name><topology>cartesian</topology><size><x>" + x +
        "</x><y>0</y><z>0</z></size><clocking><name>OPEN4</name><zones><zone><x>0</x><y>0</y><clock>" + clock +
        "</clock></zone></zones><synchronization_elements><element><x>0</x><y>0</y><z>0</z><delay>" + delay +
        "</delay></element></synchronization_elements></clocking></layout><gates><gate><id>" + id +
        "</id><type>PI</type><name>input</name><loc><x>0</x><y>0</y><z>0</z></loc></gate></gates></fgl>"};
    CHECK_THROWS_AS(read_fgl_layout<sync_layout>(stream), fgl_parsing_error);
}
