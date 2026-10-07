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

#include <fiction/technology/qca/io/write_qca_layout_svg.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <fmt/format.h>
#include <tinyxml2.h>

#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::qca;
using namespace fiction::qca::io;

/**
 * Removes whitespace from SVG output for snapshot comparisons.
 *
 * @param svg The input SVG string to be normalized.
 * @return A string with all whitespace characters removed.
 */
[[nodiscard]] static std::string normalize_svg(const std::string& svg)
{
    std::string result = svg;
    std::erase_if(result, [](const unsigned char c) { return std::isspace(c) != 0; });

    return result;
}

/**
 * Checks whether `text` contains `substring`.
 *
 * @param text Text to search.
 * @param substring Substring to find.
 * @return `true` iff `substring` appears in `text`.
 */
[[nodiscard]] static bool contains(const std::string& text, const std::string& substring) noexcept
{
    return text.find(substring) != std::string::npos;
}

TEST_CASE("Normalize SVG whitespace without changing UTF-8 text", "[write-qca-layout-svg]")
{
    CHECK(normalize_svg(" \t<svg>\r\n\xc2\xb5</svg> ") == "<svg>\xc2\xb5</svg>");
}

TEST_CASE("Generate QCA layout in simple SVG mode with constant cells", "[write-qca-layout-svg]")
{
    qca::layout layout{{1, 0}, "QCA constant cells"};
    layout.assign_cell_type({0, 0}, qca::cell_type::CONST_0);
    layout.assign_cell_type({1, 0}, qca::cell_type::CONST_1);

    std::ostringstream                layout_stream{};
    const write_qca_layout_svg_params params{.simple = true};

    write_qca_layout_svg(layout, layout_stream, params);

    const auto svg = layout_stream.str();

    CHECK(contains(svg, "fill:#000000"));
    CHECK_FALSE(contains(svg, "fill:##"));
}

TEST_CASE("Render QCA cell and clock colors in SVG", "[write-qca-layout-svg]")
{
    const auto simple                    = GENERATE(false, true);
    const auto [cell_type, clock, color] = GENERATE(Catch::Generators::table<qca::cell_type, uint8_t, const char*>(
        {{qca::cell_type::NORMAL, uint8_t{0}, "86e291"},
         {qca::cell_type::NORMAL, uint8_t{1}, "ffa5fa"},
         {qca::cell_type::NORMAL, uint8_t{2}, "00c8bc"},
         {qca::cell_type::NORMAL, uint8_t{3}, "ffffff"},
         {qca::cell_type::INPUT, uint8_t{0}, "008dc8"},
         {qca::cell_type::OUTPUT, uint8_t{0}, "e28686"},
         {qca::cell_type::CONST_0, uint8_t{0}, "000000"},
         {qca::cell_type::CONST_1, uint8_t{0}, "000000"}}));
    CAPTURE(simple, cell_type, clock);

    qca::layout layout{{0, 0}, "QCA cell colors"};
    layout.assign_cell_type({0, 0}, cell_type);
    layout.assign_clock_number({0, 0}, clock);

    std::ostringstream layout_stream{};
    write_qca_layout_svg(layout, layout_stream, {.simple = simple});
    const auto svg = layout_stream.str();

    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(document.FirstChildElement("svg") != nullptr);
    CHECK(contains(svg, fmt::format("fill:#{};", color)));
    CHECK_FALSE(contains(svg, "fill:##"));
    CHECK(contains(svg, "<circle") == !simple);
}

TEST_CASE("Render QCA tile clock labels in detailed SVG mode", "[write-qca-layout-svg]")
{
    const auto simple = GENERATE(false, true);
    const auto clock  = GENERATE(uint8_t{0}, uint8_t{1}, uint8_t{2}, uint8_t{3});
    CAPTURE(simple, clock);

    qca::layout layout{{4, 4}, "QCA tile", 5, 5};
    layout.assign_cell_type({2, 2}, qca::cell_type::NORMAL);
    layout.assign_clock_number({0, 0}, clock);

    std::ostringstream layout_stream{};
    write_qca_layout_svg(layout, layout_stream, {.simple = simple});
    const auto svg = layout_stream.str();

    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    CHECK(contains(svg, fmt::format(">{}</tspan>", clock + 1)) == !simple);
    CHECK(contains(svg, "<circle") == !simple);
    CHECK_FALSE(contains(svg, "fill:#;"));
}

TEST_CASE("Write QCA SVG files with the selected detail level", "[write-qca-layout-svg]")
{
    const auto  simple = GENERATE(false, true);
    qca::layout layout{{0, 0}, "QCA SVG file"};
    layout.assign_cell_type({0, 0}, qca::cell_type::INPUT);

    const write_qca_layout_svg_params params{.simple = simple};
    std::ostringstream                expected{};
    write_qca_layout_svg(layout, expected, params);

    const auto filename = std::filesystem::temp_directory_path() / "fiction_qca_layout.svg";
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
    const auto  simple = GENERATE(false, true);
    const auto  clock  = GENERATE(uint8_t{0}, uint8_t{3});
    qca::layout layout{{4, 4}, "Synchronized QCA tile", 5, 5};
    layout.assign_cell_type({2, 2}, qca::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, qca::cell_type::INPUT);
    layout.assign_clock_number({0, 0}, clock);
    layout.assign_synchronization_element({0, 0}, 1);

    std::ostringstream stream{};
    write_qca_layout_svg(layout, stream, {.simple = simple});
    const auto            svg = stream.str();
    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(svg.c_str()) == tinyxml2::XML_SUCCESS);
    CHECK(contains(svg, "fill:#ffe33a;"));
    CHECK(contains(svg, "fill:#008dc8;"));
    CHECK(contains(svg, "<circle") == !simple);
    CHECK(contains(svg, fmt::format(">{}</tspan>", clock + 1)) == !simple);
    CHECK_FALSE(contains(svg, ">5</tspan>"));
}

TEST_CASE("QCA SVG synchronization preserves cell positions", "[write-qca-layout-svg]")
{
    const auto  simple = GENERATE(false, true);
    qca::layout layout{{0, 0}, "Synchronized cell"};
    layout.assign_cell_type({0, 0}, qca::cell_type::NORMAL);
    std::ostringstream before{};
    write_qca_layout_svg(layout, before, {.simple = simple});
    layout.assign_synchronization_element({0, 0}, 1);
    std::ostringstream after{};
    write_qca_layout_svg(layout, after, {.simple = simple});

    tinyxml2::XMLDocument original{}, synchronized{};
    REQUIRE(original.Parse(before.str().c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(synchronized.Parse(after.str().c_str()) == tinyxml2::XML_SUCCESS);
    const auto* original_cell = original.FirstChildElement("svg")->FirstChildElement("g")->FirstChildElement("g");
    const auto* synchronized_cell =
        synchronized.FirstChildElement("svg")->FirstChildElement("g")->FirstChildElement("g");
    REQUIRE(original_cell != nullptr);
    REQUIRE(synchronized_cell != nullptr);
    REQUIRE(original_cell->Attribute("transform") != nullptr);
    REQUIRE(synchronized_cell->Attribute("transform") != nullptr);
    CHECK(std::string{original_cell->Attribute("transform")} == synchronized_cell->Attribute("transform"));
}

TEST_CASE("QCA SVG includes partial boundary tiles", "[write-qca-layout-svg]")
{
    const auto  simple = GENERATE(false, true);
    qca::layout layout{{0, 0}, "Partial tile", 5, 5};
    layout.assign_cell_type({0, 0}, qca::cell_type::NORMAL);
    std::ostringstream partial{};
    write_qca_layout_svg(layout, partial, {.simple = simple});
    layout.resize({4, 4});
    std::ostringstream full{};
    write_qca_layout_svg(layout, full, {.simple = simple});

    tinyxml2::XMLDocument partial_document{}, full_document{};
    REQUIRE(partial_document.Parse(partial.str().c_str()) == tinyxml2::XML_SUCCESS);
    REQUIRE(full_document.Parse(full.str().c_str()) == tinyxml2::XML_SUCCESS);
    CHECK(std::string{partial_document.FirstChildElement("svg")->Attribute("viewBox")} ==
          full_document.FirstChildElement("svg")->Attribute("viewBox"));
}
