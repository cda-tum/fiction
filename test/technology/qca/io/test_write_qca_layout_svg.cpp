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
 * @brief Tests for `fiction/technology/qca/io/write_qca_layout_svg.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <fiction/technology/fcn/io/cell_layout_svg.hpp>
#include <fiction/technology/qca/io/write_qca_layout_svg.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <fmt/format.h>
#include <tinyxml2.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::qca;
using namespace fiction::qca::io;

/**
 * Counts SVG elements independently of their grouping and attribute formatting.
 *
 * @param parent Document or element whose descendants to inspect.
 * @param name Element name to count.
 * @return Number of matching descendant elements.
 */
[[nodiscard]] static std::size_t count_svg_elements(const tinyxml2::XMLNode& parent, const char* name)
{
    std::size_t count{};
    for (const auto* child = parent.FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
    {
        count += std::strcmp(child->Name(), name) == 0 ? 1u : 0u;
        count += count_svg_elements(*child, name);
    }
    return count;
}

TEST_CASE("Generate QCA layout in simple SVG mode with constant cells", "[write-qca-layout-svg]")
{
    qca::layout layout{{2, 1}, "QCA constant cells"};
    layout.assign_cell_type({0, 0}, qca::cell_type::CONST_0);
    layout.assign_cell_type({1, 0}, qca::cell_type::CONST_1);

    std::ostringstream                layout_stream{};
    const write_qca_layout_svg_params params{.simple = true};

    write_qca_layout_svg(layout, layout_stream, params);

    const auto svg = layout_stream.str();

    CHECK_THAT(svg, Catch::Matchers::ContainsSubstring("fill:#000000"));
    CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring("fill:##"));
    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    CHECK(count_svg_elements(document, "rect") == 2);
    CHECK(count_svg_elements(document, "circle") == 0);
}

TEST_CASE("Render QCA cell and clock colors in SVG", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    const auto [cell_type, clock, color, dots] =
        GENERATE(Catch::Generators::table<qca::cell_type, uint8_t, const char*, std::size_t>(
            {{qca::cell_type::NORMAL, uint8_t{0}, "86e291", 4},
             {qca::cell_type::NORMAL, uint8_t{1}, "ffa5fa", 4},
             {qca::cell_type::NORMAL, uint8_t{2}, "00c8bc", 4},
             {qca::cell_type::NORMAL, uint8_t{3}, "ffffff", 4},
             {qca::cell_type::INPUT, uint8_t{0}, "008dc8", 4},
             {qca::cell_type::OUTPUT, uint8_t{0}, "e28686", 4},
             {qca::cell_type::CONST_0, uint8_t{0}, "000000", 2},
             {qca::cell_type::CONST_1, uint8_t{0}, "000000", 2}}));
    CAPTURE(simple, cell_type, clock);

    qca::layout layout{{1, 1}, "QCA cell colors"};
    layout.assign_cell_type({0, 0}, cell_type);
    layout.assign_clock_number({0, 0}, clock);

    std::ostringstream layout_stream{};
    write_qca_layout_svg(layout, layout_stream, {.simple = simple});
    const auto svg = layout_stream.str();

    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(document.FirstChildElement("svg") != nullptr);
    CHECK_THAT(svg, Catch::Matchers::ContainsSubstring(fmt::format("fill:#{};", color)));
    CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring("fill:##"));
    CHECK(count_svg_elements(document, "rect") == 1);
    CHECK(count_svg_elements(document, "circle") == (simple ? 0 : dots));
}

TEST_CASE("Render QCA tile clock labels in detailed SVG mode", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    const auto clock  = GENERATE(uint8_t{0}, uint8_t{1}, uint8_t{2}, uint8_t{3});
    CAPTURE(simple, clock);

    qca::layout layout{{5, 5}, "QCA tile", 5, 5};
    layout.assign_cell_type({2, 2}, qca::cell_type::NORMAL);
    layout.assign_clock_number({0, 0}, clock);

    std::ostringstream layout_stream{};
    write_qca_layout_svg(layout, layout_stream, {.simple = simple});
    const auto svg = layout_stream.str();

    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    if (simple)
    {
        CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", clock + 1)));
    }
    else
    {
        CHECK_THAT(svg, Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", clock + 1)));
    }
    CHECK(count_svg_elements(document, "circle") == (simple ? 0 : 4));
    CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring("fill:#;"));
}

TEST_CASE("Write QCA SVG files with the selected detail level", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    CAPTURE(simple);
    qca::layout layout{{1, 1}, "QCA SVG file"};
    layout.assign_cell_type({0, 0}, qca::cell_type::INPUT);

    const write_qca_layout_svg_params params{.simple = simple};
    std::ostringstream                expected{};
    write_qca_layout_svg(layout, expected, params);

    const auto filename =
        std::filesystem::temp_directory_path() / ("fiction-qca-svg-" + std::to_string(std::random_device{}()) + ".svg");
    write_qca_layout_svg(layout, filename.string(), params);
    std::ifstream file{filename};
    REQUIRE(file.is_open());
    const std::string svg{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
    CHECK(svg == expected.str());
    file.close();
    std::filesystem::remove(filename);

    CHECK_THROWS_AS(write_qca_layout_svg(layout, std::filesystem::temp_directory_path().string(), params),
                    std::ofstream::failure);
}

TEST_CASE("Render synchronized QCA cells in tiled SVG", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    const auto [clock, lower_clock] =
        GENERATE(Catch::Generators::table<uint8_t, uint8_t>({{uint8_t{0}, uint8_t{1}}, {uint8_t{3}, uint8_t{0}}}));
    CAPTURE(simple, clock);
    qca::layout layout{{5, 5}, "Synchronized QCA tile", 5, 5};
    layout.assign_cell_type({2, 2}, qca::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, qca::cell_type::INPUT);
    layout.assign_clock_number({0, 0}, clock);
    layout.assign_synchronization_element({0, 0}, 1);

    std::ostringstream stream{};
    write_qca_layout_svg(layout, stream, {.simple = simple});
    const auto            svg = stream.str();
    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    CHECK_THAT(svg, Catch::Matchers::ContainsSubstring("fill:#ffe33a;"));
    CHECK_THAT(svg, Catch::Matchers::ContainsSubstring("fill:#008dc8;"));
    CHECK(count_svg_elements(document, "rect") == 3);
    CHECK(count_svg_elements(document, "circle") == (simple ? 0 : 8));
    if (simple)
    {
        CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", clock + 1)));
        CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", lower_clock + 1)));
    }
    else
    {
        CHECK_THAT(svg, Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", clock + 1)));
        CHECK_THAT(svg, Catch::Matchers::ContainsSubstring(fmt::format(">{}</tspan>", lower_clock + 1)));
    }
    CHECK_THAT(svg, !Catch::Matchers::ContainsSubstring(">5</tspan>"));
}

TEST_CASE("QCA SVG synchronization preserves cell positions", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    CAPTURE(simple);
    qca::layout layout{{1, 1}, "Synchronized cell"};
    layout.assign_cell_type({0, 0}, qca::cell_type::NORMAL);
    std::ostringstream before{};
    write_qca_layout_svg(layout, before, {.simple = simple});
    layout.assign_synchronization_element({0, 0}, 1);
    std::ostringstream after{};
    write_qca_layout_svg(layout, after, {.simple = simple});

    tinyxml2::XMLDocument original{}, synchronized{};
    REQUIRE(original.Parse(before.str().c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(synchronized.Parse(after.str().c_str()) == tinyxml2::XML_SUCCESS);
    const auto* original_cells     = original.FirstChildElement("svg");
    const auto* synchronized_cells = synchronized.FirstChildElement("svg");
    REQUIRE(original_cells != nullptr);
    REQUIRE(synchronized_cells != nullptr);
    original_cells     = original_cells->FirstChildElement("g");
    synchronized_cells = synchronized_cells->FirstChildElement("g");
    REQUIRE(original_cells != nullptr);
    REQUIRE(synchronized_cells != nullptr);
    const auto* original_cell     = original_cells->FirstChildElement("g");
    const auto* synchronized_cell = synchronized_cells->FirstChildElement("g");
    REQUIRE(original_cell != nullptr);
    REQUIRE(synchronized_cell != nullptr);
    REQUIRE(original_cell->Attribute("transform") != nullptr);
    REQUIRE(synchronized_cell->Attribute("transform") != nullptr);
    CHECK(std::string{original_cell->Attribute("transform")} == synchronized_cell->Attribute("transform"));
}

TEST_CASE("QCA SVG includes partial boundary tiles", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    const auto [width, full_width, rectangles, dots] =
        GENERATE(Catch::Generators::table<uint64_t, uint64_t, std::size_t, std::size_t>({{1, 5, 2, 4}, {6, 10, 4, 8}}));
    CAPTURE(simple, width);
    qca::layout layout{{width, 1}, "Partial tile", 5, 5};
    layout.assign_cell_type({0, 0}, qca::cell_type::NORMAL);
    if (width == 6)
    {
        layout.assign_cell_type({5, 0}, qca::cell_type::NORMAL);
    }
    std::ostringstream partial{};
    write_qca_layout_svg(layout, partial, {.simple = simple});
    layout.resize({full_width, 5});
    std::ostringstream full{};
    write_qca_layout_svg(layout, full, {.simple = simple});

    tinyxml2::XMLDocument partial_document{}, full_document{};
    REQUIRE(partial_document.Parse(partial.str().c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(full_document.Parse(full.str().c_str()) == tinyxml2::XML_SUCCESS);
    const auto* partial_svg = partial_document.FirstChildElement("svg");
    const auto* full_svg    = full_document.FirstChildElement("svg");
    REQUIRE(partial_svg != nullptr);
    REQUIRE(full_svg != nullptr);
    REQUIRE(partial_svg->Attribute("viewBox") != nullptr);
    REQUIRE(full_svg->Attribute("viewBox") != nullptr);
    CHECK(std::string{partial_svg->Attribute("viewBox")} == full_svg->Attribute("viewBox"));
    CHECK(count_svg_elements(partial_document, "rect") == rectangles);
    CHECK(count_svg_elements(full_document, "rect") == rectangles);
    CHECK(count_svg_elements(partial_document, "circle") == (simple ? 0 : dots));
    CHECK(count_svg_elements(full_document, "circle") == (simple ? 0 : dots));
}

TEST_CASE("QCA SVG rejects unsupported cell types without changing output", "[write-qca-layout-svg]")
{
    const auto tile_size = GENERATE(uint16_t{1}, uint16_t{5});
    CAPTURE(tile_size);
    qca::layout layout{{1, 1}, "Unsupported cell", tile_size, tile_size};
    layout.assign_cell_type({0, 0}, static_cast<qca::cell_type>('z'));

    SECTION("stream")
    {
        std::ostringstream stream{};
        stream << "sentinel";
        CHECK_THROWS_AS(write_qca_layout_svg(layout, stream),
                        fcn::io::unsupported_cell_type_exception<qca::layout::cell>);
        CHECK(stream.str() == "sentinel");
    }
    SECTION("file")
    {
        const auto filename = std::filesystem::temp_directory_path() /
                              ("fiction-qca-svg-error-" + std::to_string(std::random_device{}()) + ".svg");
        {
            std::ofstream original{filename};
            REQUIRE(original.is_open());
            original << "sentinel";
        }
        CHECK_THROWS_AS(write_qca_layout_svg(layout, filename.string()),
                        fcn::io::unsupported_cell_type_exception<qca::layout::cell>);
        std::ifstream original{filename};
        REQUIRE(original.is_open());
        const std::string contents{std::istreambuf_iterator<char>{original}, std::istreambuf_iterator<char>{}};
        CHECK(contents == "sentinel");
        original.close();
        std::filesystem::remove(filename);
    }
}

TEST_CASE("QCA SVG clips cells to the frame in both render modes", "[write-qca-layout-svg]")
{
    const auto simple    = GENERATE(false, true);
    const auto tile_size = GENERATE(uint16_t{1}, uint16_t{5});
    CAPTURE(simple, tile_size);
    qca::layout layout{{3, 3, 2}, "Crossing", tile_size, tile_size};
    layout.assign_cell_type({1, 1}, qca::cell_type::NORMAL);
    layout.assign_cell_type({1, 1, 1}, qca::cell_type::NORMAL);
    layout.assign_cell_mode({1, 1, 1}, qca::cell_mode::VERTICAL);
    std::ostringstream expected{};
    write_qca_layout_svg(layout, expected, {.simple = simple});

    layout.assign_cell_type({-1, 1}, qca::cell_type::INPUT);
    layout.assign_cell_type({3, 1}, qca::cell_type::OUTPUT);
    layout.assign_cell_type({1, 1, 2}, qca::cell_type::NORMAL);
    std::ostringstream actual{};
    write_qca_layout_svg(layout, actual, {.simple = simple});
    CHECK(actual.str() == expected.str());
}

TEST_CASE("QCA SVG paints lower layers before upper layers", "[write-qca-layout-svg]")
{
    const auto simple    = GENERATE(false, true);
    const auto tile_size = GENERATE(uint16_t{1}, uint16_t{5});
    CAPTURE(simple, tile_size);
    qca::layout layout{{10, 5, 2}, "Layers", tile_size, tile_size};
    layout.assign_cell_type({1, 1, 1}, qca::cell_type::OUTPUT);
    layout.assign_cell_type({1, 1, 0}, qca::cell_type::INPUT);
    std::ostringstream stream{};
    write_qca_layout_svg(layout, stream, {.simple = simple});
    const auto svg = stream.str();
    REQUIRE(svg.find("fill:#008dc8") != std::string::npos);
    REQUIRE(svg.find("fill:#e28686") != std::string::npos);
    CHECK(svg.find("fill:#008dc8") < svg.find("fill:#e28686"));
}
