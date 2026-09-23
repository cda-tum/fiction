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
 * @brief SVG writer for molQCA layouts.
 * @author Sophia Kuhn
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/technology/fcn/io/cell_layout_svg.hpp"
#include "fiction/technology/mol_qca/layout.hpp"
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/progress.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/format.h>

#include <array>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace fiction::mol_qca::io
{

/**
 * Parameters for writing SVG molQCA layouts.
 */
struct write_mol_qca_layout_svg_params
{
    /**
     * Limit details to create smaller file sizes.
     */
    bool simple = false;
    /** @brief Receives completed serialization work and the phase total. */
    utils::progress_callback on_progress{};
};

namespace detail
{

namespace svg
{

/**
 * @brief molQCA SVG color codes, geometry constants, and cell templates.
 *
 * This group defines the clock-zone colors, intra-cell spacing, offsets, and SVG glyphs used by the molQCA layout
 * writer.
 */
inline constexpr const char* CLOCK_ZONE_1_CELL = "ffe700";
inline constexpr const char* CLOCK_ZONE_2_CELL = "65ff00";
inline constexpr const char* CLOCK_ZONE_3_CELL = "0071ff";
inline constexpr const char* CLOCK_ZONE_4_CELL = "f800ff";
inline constexpr const char* PI_CELL           = "cccccc";
inline constexpr const char* PO_CELL           = "4a4a4a";

inline constexpr const double CELL_DISTANCE = 11.5;

/**
 * MolQCA glyph templates draw the visible 10x10 cell body at these SVG-local coordinates. The layout translation
 * offsets below are derived from those anchors to keep their origin visible instead of using unexplained magic numbers.
 */
inline constexpr const double GLYPH_ANCHOR_X         = 92.93404;
inline constexpr const double GLYPH_ANCHOR_Y         = 56.46016;
inline constexpr const double STARTING_OFFSET_CELL_X = fcn::io::detail::svg::STARTING_OFFSET_CELL_X + GLYPH_ANCHOR_X;
inline constexpr const double STARTING_OFFSET_CELL_Y = fcn::io::detail::svg::STARTING_OFFSET_CELL_Y + GLYPH_ANCHOR_Y;

inline constexpr const char* CONST0 = "<g\n"
                                      "    transform=\"translate({1},{2})\">\n"
                                      "\n"
                                      "  <rect\n"
                                      "    style=\"fill:#000000;stroke:#000000;\"\n"
                                      "    width=\"10\"\n"
                                      "    height=\"10\"\n"
                                      "    x=\"92.93404\"\n"
                                      "    y=\"56.46016\" />\n"
                                      "\n"
                                      "  <text\n"
                                      "    x=\"97.93404\"\n"
                                      "    y=\"63.46016\"\n"
                                      "    style=\"font-family:Arial, sans-serif;font-size:6px;fill:#ffffff;\"\n"
                                      "    text-anchor=\"middle\"\n"
                                      "    alignment-baseline=\"central\">\n"
                                      "    C0\n"
                                      "  </text>\n"
                                      "</g>";

inline constexpr const char* CONST1 = "<g\n"
                                      "    transform=\"translate({1},{2})\">\n"
                                      "\n"
                                      "  <rect\n"
                                      "    style=\"fill:#000000;stroke:#000000;\"\n"
                                      "    width=\"10\"\n"
                                      "    height=\"10\"\n"
                                      "    x=\"92.93404\"\n"
                                      "    y=\"56.46016\" />\n"
                                      "\n"
                                      "  <text\n"
                                      "    x=\"97.93404\"\n"
                                      "    y=\"63.46016\"\n"
                                      "    style=\"font-family:Arial, sans-serif;font-size:6px;fill:#ffffff;\"\n"
                                      "    text-anchor=\"middle\"\n"
                                      "    alignment-baseline=\"central\">\n"
                                      "    C1\n"
                                      "  </text>\n"
                                      "</g>";

inline constexpr const char* CELL = "<g\n"
                                    "    style=\"fill:#{0};\"\n"
                                    "    transform=\"translate({1},{2})\">\n"
                                    "\n"
                                    "  <rect\n"
                                    "    style=\"fill:#{0};stroke:#{0};\"\n"
                                    "    width=\"10\"\n"
                                    "    height=\"10\"\n"
                                    "    x=\"92.93404\"\n"
                                    "    y=\"56.46016\" />\n"
                                    "\n"
                                    "  <g style=\"stroke:#000000;stroke-width:0.5;fill:#{0};\">\n"
                                    "    <!-- left molecule -->\n"
                                    "    <line x1=\"95.43404\" y1=\"58.96016\" x2=\"95.43404\" y2=\"63.96016\" />\n"
                                    "    <circle r=\"0.97553\" cx=\"95.43404\" cy=\"58.96016\" />\n"
                                    "    <circle r=\"0.50000\" cx=\"95.43404\" cy=\"61.46016\" />\n"
                                    "    <circle r=\"0.97553\" cx=\"95.43404\" cy=\"63.96016\" />\n"
                                    "\n"
                                    "    <!-- right molecule -->\n"
                                    "    <line x1=\"100.43404\" y1=\"58.96016\" x2=\"100.43404\" y2=\"63.96016\" />\n"
                                    "    <circle r=\"0.97553\" cx=\"100.43404\" cy=\"58.96016\" />\n"
                                    "    <circle r=\"0.50000\" cx=\"100.43404\" cy=\"61.46016\" />\n"
                                    "    <circle r=\"0.97553\" cx=\"100.43404\" cy=\"63.96016\" />\n"
                                    "  </g>\n"
                                    "</g>";

inline constexpr const char* SIMPLE_CELL = "<g\n"
                                           "transform=\"translate({1},{2})\">\n"
                                           "<rect\n"
                                           "ry=\"0.75\"\n"
                                           "y=\"56.46016\"\n"
                                           "x=\"92.93404\"\n"
                                           "height=\"10\"\n"
                                           "width=\"10\"\n"
                                           "style=\"fill:#{0};stroke:#000000;\" />\n"
                                           "</g>";

}  // namespace svg

class write_mol_qca_layout_svg_impl
{
  public:
    /**
     * @brief Stores the layout and drawing parameters.
     * @param layout Layout to draw.
     * @param stream Output stream.
     * @param p Drawing parameters.
     */
    write_mol_qca_layout_svg_impl(const mol_qca::layout& layout, std::ostream& stream,
                                  write_mol_qca_layout_svg_params p = {}) :
            lyt{layout},
            os{stream},
            ps{std::move(p)}
    {}

    void run()
    {
        generate_cell_based_svg();
    }

  private:
    const mol_qca::layout& lyt;

    std::ostream& os;

    write_mol_qca_layout_svg_params ps;

    /**
     * Generates and returns a pair of strings representing the description and color of the given cell.
     *
     * @param c The cell for which to generate the description and color.
     * @return A pair of strings representing the description and color of the given cell `c`.
     */
    std::pair<std::string, std::string> generate_description_color(const mol_qca::layout::cell& c)
    {
        std::string cell_description{};
        std::string cell_color{};

        static constexpr const std::array<const char*, 4> cell_colors{
            {svg::CLOCK_ZONE_1_CELL, svg::CLOCK_ZONE_2_CELL, svg::CLOCK_ZONE_3_CELL, svg::CLOCK_ZONE_4_CELL}};

        const auto ct = lyt.get_cell_type(c);

        if (mol_qca::is_normal(ct))
        {
            cell_color = cell_colors.at(mol_qca::clock_number(ct));

            if (ps.simple)
            {
                cell_description = svg::SIMPLE_CELL;
            }
            else
            {
                cell_description = svg::CELL;
            }
        }
        else if (ct == mol_qca::cell_type::INPUT)
        {
            cell_color       = svg::PI_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CELL;
        }
        else if (ct == mol_qca::cell_type::OUTPUT)
        {
            cell_color       = svg::PO_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CELL;
        }
        else if (ct == mol_qca::cell_type::CONST_0)
        {
            cell_color       = fcn::io::detail::svg::CONST_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CONST0;
        }
        else if (ct == mol_qca::cell_type::CONST_1)
        {
            cell_color       = fcn::io::detail::svg::CONST_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CONST1;
        }
        else
        {
            throw fcn::io::unsupported_cell_type_exception(c);
        }

        return std::make_pair(cell_description, cell_color);
    }

    /**
     * Generates an SVG string representing the layout and appends it to the output stream.
     */
    void generate_cell_based_svg()
    {
        std::stringstream cell_descriptions{};

        utils::progress_reporter progress{ps.on_progress, "rendering cell positions", (lyt.x() + 1) * (lyt.y() + 1)};
        lyt.foreach_cell_position(
            [this, &cell_descriptions, &progress](const auto& c)
            {
                if (!lyt.is_empty_cell(c))
                {
                    // Determines cell type and color
                    const auto desc_col = generate_description_color(c);

                    // Current cell-description can now be appended to the description of all cells
                    cell_descriptions << fmt::format(fmt::runtime(desc_col.first), desc_col.second,
                                                     fcn::io::detail::svg::STARTING_OFFSET_TILE_X +
                                                         svg::STARTING_OFFSET_CELL_X + (c.x * svg::CELL_DISTANCE),
                                                     fcn::io::detail::svg::STARTING_OFFSET_TILE_Y +
                                                         svg::STARTING_OFFSET_CELL_Y + (c.y * svg::CELL_DISTANCE));
                }
                progress.advance();
            });

        const double viewbox_x =
            (2 * fcn::io::detail::svg::VIEWBOX_DISTANCE) + (static_cast<double>(lyt.x() + 1) * svg::CELL_DISTANCE);
        const double viewbox_y =
            (2 * fcn::io::detail::svg::VIEWBOX_DISTANCE) + (static_cast<double>(lyt.y() + 1) * svg::CELL_DISTANCE);

        os << fmt::format(fcn::io::detail::svg::HEADER, FICTION_VERSION, FICTION_REPO, viewbox_x, viewbox_y,
                          cell_descriptions.str());
    }
};

}  // namespace detail

/**
 * Writes an SVG representation of a molQCA layout to an output stream. Each regular cell is colored by its clock
 * phase.
 *
 * May throw an `unsupported_cell_type_exception` if it encounters unsupported cell types in the layout.
 *
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 * @param ps Parameters.
 */
inline void write_mol_qca_layout_svg(const mol_qca::layout& lyt, std::ostream& os,
                                     const write_mol_qca_layout_svg_params& ps = {})
{

    detail::write_mol_qca_layout_svg_impl p{lyt, os, ps};

    p.run();
}

/**
 * Writes an SVG representation of a molQCA layout to a file. Each regular cell is colored by its clock phase.
 *
 * May throw an `unsupported_cell_type_exception` if it encounters unsupported cell types in the layout. May throw an
 * `std::ofstream::failure` if it cannot open the file.
 *
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the `.svg` extension.
 * @param ps Parameters.
 */
inline void write_mol_qca_layout_svg(const mol_qca::layout& lyt, const std::string_view& filename,
                                     const write_mol_qca_layout_svg_params& ps = {})
{
    fiction::detail::atomic_write(filename, [&](std::ostream& os) { write_mol_qca_layout_svg(lyt, os, ps); });
}

}  // namespace fiction::mol_qca::io
