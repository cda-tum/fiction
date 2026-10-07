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
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/read_fgl_layout.hpp>
#include <fiction/layouts/io/write_fgl_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

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
        CHECK(lyt.width() == 1);
        CHECK(lyt.height() == 1);
        CHECK(lyt.area() == 1);
        CHECK(lyt.get_layout_name() == "Test");
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    };

    using gate_layout = gate_level_layout<cartesian_layout>;
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
        CHECK(lyt.width() == 3);
        CHECK(lyt.height() == 2);
        CHECK(lyt.area() == 6);
        CHECK(lyt.get_layout_name() == "Test");
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(lyt.is_pi_tile({0, 1}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_input_0 = lyt.find_object({0, 1});
        CHECK((primary_input_0.has_value() && lyt.get_name(*primary_input_0) == "pi0"));
        CHECK(lyt.is_pi_tile({1, 0}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_input_1 = lyt.find_object({1, 0});
        CHECK((primary_input_1.has_value() && lyt.get_name(*primary_input_1) == "pi1"));
        /** @brief Gate restored at the expected position. */
        const auto gate = lyt.find_object({1, 1});
        CHECK((gate.has_value() && lyt.is_and(*gate)));
        CHECK(lyt.is_po_tile({2, 1}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_output = lyt.find_object({2, 1});
        CHECK((primary_output.has_value() && lyt.get_name(*primary_output) == "po0"));
    };

    using gate_layout = gate_level_layout<cartesian_layout>;
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
        CHECK(lyt.width() == 3);
        CHECK(lyt.height() == 2);
        CHECK(lyt.area() == 6);
        CHECK(lyt.get_layout_name() == "Test");
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(lyt.is_pi_tile({0, 1}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_input_0 = lyt.find_object({0, 1});
        CHECK((primary_input_0.has_value() && lyt.get_name(*primary_input_0) == "pi0"));
        CHECK(lyt.is_pi_tile({1, 0}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_input_1 = lyt.find_object({1, 0});
        CHECK((primary_input_1.has_value() && lyt.get_name(*primary_input_1) == "pi1"));
        /** @brief Gate restored at the expected position. */
        const auto gate = lyt.find_object({1, 1});
        CHECK((gate.has_value() && lyt.is_le(*gate)));
        CHECK(lyt.is_po_tile({2, 1}));
        /** @brief Object whose label is checked at the expected position. */
        const auto primary_output = lyt.find_object({2, 1});
        CHECK((primary_output.has_value() && lyt.get_name(*primary_output) == "po0"));
    };

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("Read FGL layout without topology", "[read-fgl-layout]")
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

    const auto* const topology = GENERATE("", "<topology/>");
    std::string       document{fgl_layout};
    document.insert(document.find("<size>"), topology);

    SECTION("Cartesian layouts default to Cartesian topology")
    {
        std::istringstream layout_stream{document};
        const auto         lyt = read_fgl_layout<cart_gate_clk_lyt>(layout_stream);
        CHECK(lyt.width() == 1);
        CHECK(lyt.height() == 1);
        CHECK(lyt.get_layout_name() == "Test");
        CHECK(lyt.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    }

    SECTION("Shifted Cartesian layouts require a topology")
    {
        std::istringstream layout_stream{document};
        CHECK_THROWS_AS(read_fgl_layout<shifted_cart_gate_clk_lyt>(layout_stream), fgl_parsing_error);
    }

    SECTION("Hexagonal layouts require a topology")
    {
        std::istringstream layout_stream{document};
        CHECK_THROWS_AS(read_fgl_layout<hex_gate_clk_lyt>(layout_stream), fgl_parsing_error);
    }
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<shifted_cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<shifted_cartesian_layout>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

namespace
{

/**
 * Returns an FGL document of an empty layout with the given topology.
 */
std::string fgl_with_topology(const std::string_view topology)
{
    return fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                       "<fgl>\n"
                       "  <layout>\n"
                       "    <name>Test</name>\n"
                       "    <topology>{}</topology>\n"
                       "    <size>\n"
                       "      <x>0</x>\n"
                       "      <y>0</y>\n"
                       "      <z>0</z>\n"
                       "    </size>\n"
                       "    <clocking>\n"
                       "      <name>2DDWave</name>\n"
                       "    </clocking>\n"
                       "  </layout>\n"
                       "</fgl>\n",
                       topology);
}

}  // namespace

TEST_CASE("Read FGL layout takes the arrangement from the file", "[read-fgl-layout]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    SECTION("shifted Cartesian")
    {
        std::istringstream layout_stream{fgl_with_topology(fmt::format("{}_cartesian", to_string(a)))};

        CHECK(read_fgl_layout<shifted_cart_gate_clk_lyt>(layout_stream).get_arrangement() == a);
    }
    SECTION("hexagonal")
    {
        std::istringstream layout_stream{fgl_with_topology(fmt::format("{}_hex", to_string(a)))};

        CHECK(read_fgl_layout<hex_gate_clk_lyt>(layout_stream).get_arrangement() == a);
    }
}

TEST_CASE("Parsing error: target layout has another arrangement than the file", "[read-fgl-layout]")
{
    const auto target_arrangement =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);
    const auto file_arrangement =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    std::istringstream layout_stream{fgl_with_topology(fmt::format("{}_hex", to_string(file_arrangement)))};

    hex_gate_clk_lyt target{target_arrangement};

    if (target_arrangement == file_arrangement)
    {
        CHECK_NOTHROW(read_fgl_layout(target, layout_stream));
    }
    else
    {
        CHECK_THROWS_AS(read_fgl_layout(target, layout_stream), fgl_parsing_error);
    }
}

TEST_CASE("Parsing error: size exceeds the coordinate domain", "[read-fgl-layout]")
{
    const auto [axis, value] =
        GENERATE(std::pair{"x", "2147483648"}, std::pair{"y", "2147483648"}, std::pair{"z", "2147483648"});

    const char* const x = std::string{axis} == "x" ? value : "2";
    const char* const y = std::string{axis} == "y" ? value : "1";
    const char* const z = std::string{axis} == "z" ? value : "0";

    std::istringstream layout_stream{fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                 "<fgl>\n"
                                                 "  <layout>\n"
                                                 "    <name>Test</name>\n"
                                                 "    <topology>cartesian</topology>\n"
                                                 "    <size>\n"
                                                 "      <x>{}</x>\n"
                                                 "      <y>{}</y>\n"
                                                 "      <z>{}</z>\n"
                                                 "    </size>\n"
                                                 "    <clocking>\n"
                                                 "      <name>2DDWave</name>\n"
                                                 "    </clocking>\n"
                                                 "  </layout>\n"
                                                 "</fgl>\n",
                                                 x, y, z)};

    CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(layout_stream), fgl_parsing_error);
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
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

    using gate_layout = gate_level_layout<cartesian_layout>;
    CHECK_THROWS_AS(read_fgl_layout<gate_layout>(layout_stream), fgl_parsing_error);
}

TEST_CASE("FGL preserves synchronization elements and labels", "[read-fgl-layout]")
{
    using sync_layout = gate_level_layout<cartesian_layout>;
    sync_layout original{{2, 1, 1}};
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
    /** @brief Object whose label is checked at the expected position. */
    const auto restored_input = restored.find_object({0, 0, 0});
    CHECK((restored_input.has_value() && restored.get_name(*restored_input) == "in<&>"));
    /** @brief Object whose label is checked at the expected position. */
    const auto restored_output = restored.find_object({1, 0, 0});
    CHECK((restored_output.has_value() && restored.get_name(*restored_output) == "out<&>"));
    CHECK(restored.num_se() == 2);
    CHECK(restored.get_synchronization_element({0, 0, 0}) == 2);
    CHECK(restored.get_synchronization_element({1, 0, 0}) == 255);
}

TEST_CASE("FGL rejects invalid numeric metadata", "[read-fgl-layout]")
{
    using sync_layout = gate_level_layout<cartesian_layout>;
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

TEST_CASE("FGL failed connection read leaves target unchanged", "[read-fgl-layout]")
{
    cart_gate_clk_lyt layout{{2, 1, 1}, clocking::twoddwave(), "original"};
    layout.create_pi("kept", {0, 0, 0});
    std::stringstream stream{R"(<fgl version="2"><layout><size><x>2</x><y>1</y><z>1</z></size>
      <clocking><name>2DDWave</name></clocking><inputs></inputs><outputs><id>1</id></outputs></layout>
      <gates><gate><id>1</id><type>PO</type><name>f</name><arity>1</arity>
      <loc><x>1</x><y>0</y><z>0</z></loc><incoming><signal><source>999</source><index>0</index>
      <input>0</input></signal></incoming></gate></gates></fgl>)"};
    CHECK_THROWS_AS(read_fgl_layout(layout, stream), fgl_parsing_error);
    CHECK(layout.get_layout_name() == "original");
    CHECK(layout.num_pis() == 1);
    CHECK(layout.get_input_name(0) == "kept");
}

TEST_CASE("Version-2 FGL validates explicit interfaces and ports", "[read-fgl-layout]")
{
    std::string xml{R"(<fgl version="2"><layout><size><x>2</x><y>1</y><z>1</z></size>
      <clocking><name>2DDWave</name></clocking><inputs><id>4294967295</id></inputs><outputs><id>2</id></outputs></layout>
      <gates><gate><id>2</id><type>PO</type><name></name><arity>1</arity>
      <loc><x>1</x><y>0</y><z>0</z></loc><incoming><signal><source>4294967295</source><index>0</index>
      <input>0</input></signal></incoming></gate><gate><id>4294967295</id><type>PI</type><name></name><arity>0</arity>
      <loc><x>0</x><y>0</y><z>0</z></loc></gate></gates></fgl>)"};
    SECTION("Sources can follow destinations and use the full ID range")
    {
        std::stringstream stream{xml};
        const auto        restored = read_fgl_layout<cart_gate_clk_lyt>(stream);
        CHECK(restored.get_input_name(0).empty());
        CHECK(restored.get_output_name(0).empty());
        /** @brief Output object restored from the destination-first XML. */
        const auto output = restored.find_object({1, 0, 0});
        REQUIRE(output.has_value());
        if (!output.has_value())
        {
            return;
        }
        /** @brief Source connected to the restored output. */
        const auto source = restored.source({*output, 0});
        CHECK((source.has_value() && restored.get_tile(source->object) == cart_gate_clk_lyt::tile{0, 0, 0}));
    }
    SECTION("Missing input")
    {
        xml.erase(xml.find("<signal>"), xml.find("</signal>") + 9 - xml.find("<signal>"));
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
    SECTION("Duplicate input connection")
    {
        const auto start     = xml.find("<signal>");
        const auto length    = xml.find("</signal>") + 9 - start;
        const auto duplicate = xml.substr(start, length);
        xml.insert(start, duplicate);
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
    SECTION("Input index exceeds arity")
    {
        xml.replace(xml.find("<input>0</input>"), 16, "<input>1</input>");
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
    SECTION("Invalid output index")
    {
        xml.replace(xml.find("<index>0</index>"), 16, "<index>1</index>");
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
    SECTION("Wrong interface role")
    {
        xml.replace(xml.find("<outputs><id>2</id>"), 19, "<outputs><id>4294967295</id>");
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
    SECTION("Unsupported version")
    {
        xml.replace(xml.find("version=\"2\""), 11, "version=\"3\"");
        std::stringstream stream{xml};
        CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(stream), fgl_parsing_error);
    }
}

TEST_CASE("Legacy FGL maximum layer indices become layer counts", "[read-fgl-layout]")
{
    std::stringstream stream{R"(<fgl><layout><size><x>0</x><y>0</y><z>1</z></size>
      <clocking><name>2DDWave</name></clocking></layout></fgl>)"};
    const auto        layout = read_fgl_layout<cart_gate_clk_lyt>(stream);
    CHECK(layout.width() == 1);
    CHECK(layout.height() == 1);
    CHECK(layout.layers() == 2);
}

TEST_CASE("FGL rejects more than two layers", "[read-fgl-layout]")
{
    /** Version and z value of an unsupported layer extent. */
    const auto xml = GENERATE(
        std::string{R"(<fgl><layout><size><x>0</x><y>0</y><z>2</z></size>
          <clocking><name>2DDWave</name></clocking></layout></fgl>)"},
        std::string{R"(<fgl version="2"><layout><size><x>1</x><y>1</y><z>3</z></size>
          <clocking><name>2DDWave</name></clocking></layout></fgl>)"});
    /** Target layout that a rejected read preserves. */
    cart_gate_clk_lyt target{{2, 3, 1}, clocking::twoddwave(), "kept"};
    /** Input with an unsupported layer extent. */
    std::stringstream stream{xml};
    CHECK_THROWS_AS(read_fgl_layout(target, stream), fgl_parsing_error);
    CHECK(target.dimensions() == layout_base::extent{2, 3, 1});
    CHECK(target.get_layout_name() == "kept");
}

TEST_CASE("Malformed manual FGL obstructions leave the target unchanged", "[read-fgl-layout]")
{
    const auto invalid = GENERATE(
        std::string{"<coordinates><coordinate><y>0</y><z>0</z></coordinate></coordinates>"},
        std::string{"<coordinates><coordinate><x>0</x><y>0</y></coordinate></coordinates>"},
        std::string{"<coordinates><coordinate><x>2147483648</x><y>0</y><z>0</z></coordinate></coordinates>"},
        std::string{"<coordinates><coordinate><x>-2147483649</x><y>0</y><z>0</z></coordinate></coordinates>"},
        std::string{"<coordinates><coordinate><x>-9223372036854775809</x><y>0</y><z>0</z></coordinate></coordinates>"},
        std::string{"<coordinates><coordinate><x>1junk</x><y>0</y><z>0</z></coordinate></coordinates>"},
        std::string{"<connections><connection><source><x>0</x><y>0</y><z>0</z></source></connection></connections>"});
    cart_gate_clk_lyt target{{1, 1, 1}, clocking::twoddwave(), "kept"};
    const auto        id = target.create_pi("input", {0, 0});
    target.obstruct_coordinate({-4, -5, -6});
    target.obstruct_connection({-4, -5, -6}, {7, 8, 9});
    std::string xml{R"(<fgl version="2"><layout><size><x>1</x><y>1</y><z>1</z></size>
<clocking><name>2DDWave</name></clocking><inputs/><outputs/></layout><gates/></fgl>)"};
    xml.insert(xml.find("</layout>"), "<obstructions>" + invalid + "</obstructions>");
    std::stringstream malformed{xml};
    CHECK_THROWS_AS(read_fgl_layout(target, malformed), fgl_parsing_error);
    CHECK(target.get_layout_name() == "kept");
    /** @brief Object retained after the failed read. */
    const auto retained = target.find_object({0, 0});
    CHECK((retained.has_value() && target.output(*retained) == id));
    CHECK(target.is_obstructed_coordinate({-4, -5, -6}));
    CHECK(target.is_obstructed_connection({-4, -5, -6}, {7, 8, 9}));
}

TEST_CASE("Version-2 FGL requires a finished layout before assigning the target", "[read-fgl-layout]")
{
    cart_gate_clk_lyt original{{3, 2, 1}, clocking::twoddwave()};
    const auto        input = original.create_pi("a", {0, 0});
    original.create_po(input, "f", {1, 0});
    const auto wire = original.create_buf(input, {0, 1});
    original.create_buf(wire, {1, 1});
    std::stringstream serialized{};
    write_fgl_layout(original, serialized);
    auto xml = serialized.str();
    SECTION("PO self-cycle")
    {
        xml.replace(xml.find("<source>0</source>"), 18, "<source>1</source>");
    }
    SECTION("Dangling cycle")
    {
        xml.replace(xml.rfind("<source>0</source>"), 18, "<source>3</source>");
    }
    SECTION("Outside extent")
    {
        const auto width = xml.find("<x>3</x>", xml.find("<size>"));
        xml.replace(width, 8, "<x>1</x>");
    }
    SECTION("Nonadjacent connection")
    {
        const auto po = xml.find("<type>PO</type>");
        const auto x  = xml.find("<x>1</x>", po);
        xml.replace(x, 8, "<x>2</x>");
    }
    SECTION("Wrong clock")
    {
        xml.insert(xml.find("</zones>"), "<zone><x>1</x><y>0</y><clock>0</clock></zone>");
    }
    cart_gate_clk_lyt target{{1, 1, 1}, clocking::twoddwave(), "kept"};
    const auto        kept = target.create_pi("input", {0, 0});
    target.obstruct_coordinate({-1, -2, -3});
    std::stringstream malformed{xml};
    CHECK_THROWS_AS(read_fgl_layout(target, malformed), fgl_parsing_error);
    CHECK(target.get_layout_name() == "kept");
    /** @brief Object retained after the failed read. */
    const auto retained = target.find_object({0, 0});
    CHECK((retained.has_value() && target.output(*retained) == kept));
    CHECK(target.is_obstructed_coordinate({-1, -2, -3}));
    std::stringstream rejected{xml};
    CHECK_THROWS_AS(read_fgl_layout<cart_gate_clk_lyt>(rejected), fgl_parsing_error);
}

TEST_CASE("Legacy FGL keeps editable physical placement", "[read-fgl-layout]")
{
    std::stringstream stream{R"(<fgl><layout><size><x>0</x><y>0</y><z>0</z></size>
<clocking><name>2DDWave</name></clocking></layout><gates>
<gate><id>0</id><type>PI</type><name>a</name><loc><x>1</x><y>0</y><z>0</z></loc></gate>
</gates></fgl>)"};
    const auto        layout = read_fgl_layout<cart_gate_clk_lyt>(stream);
    CHECK(layout.width() == 1);
    CHECK(layout.find_object({1, 0}).has_value());
}
