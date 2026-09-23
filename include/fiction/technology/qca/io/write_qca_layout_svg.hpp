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
 * @brief SVG writer for QCA layouts.
 * @author Sophia Kuhn
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/technology/fcn/io/cell_layout_svg.hpp"
#include "fiction/technology/qca/layout.hpp"
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/progress.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/format.h>

#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace fiction::qca::io
{

/**
 * Parameters for writing SVG QCA layouts.
 */
struct write_qca_layout_svg_params
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

// General layout
inline constexpr const double TILE_DISTANCE = 119;
inline constexpr const double CELL_DISTANCE = 23;

// Latch tiles
inline constexpr const double STARTING_OFFSET_LATCH_X = -483.5;
inline constexpr const int    STARTING_OFFSET_LATCH_Y = -57;

// Colors
inline constexpr const char* CLOCK_ZONE_1_CELL     = "86e291";
inline constexpr const char* CLOCK_ZONE_2_CELL     = "ffa5fa";
inline constexpr const char* CLOCK_ZONE_3_CELL     = "00c8bc";
inline constexpr const char* CLOCK_ZONE_4_CELL     = "ffffff";
inline constexpr const char* CLOCK_ZONE_LATCH_CELL = "ffe33a";
inline constexpr const char* CLOCK_ZONE_1_TILE     = "ffffff";
inline constexpr const char* CLOCK_ZONE_2_TILE     = "bfbfbf";
inline constexpr const char* CLOCK_ZONE_3_TILE     = "7f7f7f";
inline constexpr const char* CLOCK_ZONE_4_TILE     = "3f3f3f";
inline constexpr const char* CLOCK_ZONE_12_TEXT    = "000000";
inline constexpr const char* CLOCK_ZONE_34_TEXT    = "ffffff";
inline constexpr const char* PI_CELL               = "008dc8";
inline constexpr const char* PO_CELL               = "e28686";

// SVG parts with placeholders

inline constexpr const char* TILE =
    "<g\n"
    "transform=\"translate({0},{1})\">\n"
    "<g>\n"
    "<rect\n"
    "ry=\"1.4357216\"\n"
    "y=\"69.390663\"\n"
    "x=\"186.11467\"\n"
    "height=\"118.80357\"\n"
    "width=\"118.80357\"\n"
    "style=\"fill:#{2};stroke:#000000;\" />\n"
    "<text\n"
    "y=\"179.25412\"\n"
    "x=\"288.74234\"\n"
    "style=\"font-style:normal;font-weight:normal;font-size:14.5px;line-height:125%;font-"
    "family:'Noto Sans';fill:#{4};stroke:none;\"\n"
    "xml:space=\"preserve\"><tspan\n"
    "y=\"179.25412\"\n"
    "x=\"288.74234\">{5}</tspan></text>\n"
    "</g>\n"
    "<g>\n"
    "{3}\n"
    "</g>\n"
    "</g>";

inline constexpr const char* LATCH =
    "<g\n"
    "transform=\"translate({0},{1})\">\n"
    "<g>\n"
    "<g>\n"
    "<g>\n"
    "<path\n"
    "d=\"m 613.38686,68.413109 0,118.803571 -118.80358,0\"\n"
    "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:#{2};fill-opacity:1;fill-rule:nonzero;stroke:none;"
    "stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\" />\n"
    "<path\n"
    "d=\"m 494.58328,187.21668 0,-118.80357 118.80358,0\"\n"
    "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:#{3};fill-opacity:1;fill-rule:nonzero;stroke:none;"
    "stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\" />\n"
    "</g>\n"
    "<rect\n"
    "style=\"color:#000000;solid-color:#000000;solid-opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#"
    "000000;stroke-width:1.19643486;stroke-opacity:1;enable-background:accumulate\"\n"
    "width=\"118.80357\"\n"
    "height=\"118.80357\"\n"
    "x=\"494.58328\"\n"
    "y=\"68.413109\"\n"
    "ry=\"1.4357216\" />\n"
    "</g>\n"
    "<text\n"
    "y=\"86.340652\"\n"
    "x=\"504.43588\"\n"
    "style=\"font-style:normal;font-weight:normal;font-size:12.5px;line-height:125%;font-family:'Noto "
    "Sans';letter-spacing:0px;word-spacing:0px;fill:#{5};fill-opacity:1;\"\n"
    "xml:space=\"preserve\"><tspan\n"
    "y=\"86.340652\"\n"
    "x=\"504.43588\">{6}</tspan></text>\n"
    "<text\n"
    "y=\"178.27962\"\n"
    "x=\"596.48468\"\n"
    "style=\"font-style:normal;font-weight:normal;font-size:12.5px;line-height:125%;font-family:'Noto "
    "Sans';letter-spacing:0px;word-spacing:0px;fill:#{7};fill-opacity:1;\"\n"
    "xml:space=\"preserve\"><tspan\n"
    "y=\"178.27962\"\n"
    "x=\"596.48468\">{8}</tspan></text>\n"
    "</g>\n"
    "<g>\n"
    "{4}\n"
    "</g>\n"
    "</g>";

inline constexpr const char* CELL = "<g\n"
                                    "style=\"fill:#{0};\"\n"
                                    "transform=\"translate({1},{2})\">\n"
                                    "<rect\n"
                                    "style=\"fill:#{0};stroke:#000000;\"\n"
                                    "width=\"20\"\n"
                                    "height=\"20\"\n"
                                    "x=\"185.86807\"\n"
                                    "y=\"112.92032\"\n"
                                    "ry=\"1.5\" />\n"
                                    "<g\n"
                                    "style=\"fill:#{0};\"\n"
                                    "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                    "<circle\n"
                                    "r=\"2.7314816\"\n"
                                    "cx=\"191.0945\"\n"
                                    "cy=\"118.01437\"\n"
                                    "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                    "<circle\n"
                                    "r=\"2.7314816\"\n"
                                    "cx=\"191.0945\"\n"
                                    "cy=\"128.01437\"\n"
                                    "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                    "<circle\n"
                                    "r=\"2.7314816\"\n"
                                    "cx=\"201.0945\"\n"
                                    "cy=\"128.01437\"\n"
                                    "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                    "<circle\n"
                                    "r=\"2.7314816\"\n"
                                    "cx=\"201.0945\"\n"
                                    "cy=\"118.01437\"\n"
                                    "style=\"opacity:1;fill:#{0};stroke:#000000;\" />\n"
                                    "</g>\n"
                                    "</g>";

inline constexpr const char* CONST0 = "<g\n"
                                      "style=\"fill:#000000;fill-opacity:1\"\n"
                                      "transform=\"translate({1},{2})\">\n"
                                      "<rect\n"
                                      "style=\"fill:#000000;stroke:#000000;\"\n"
                                      "width=\"20\"\n"
                                      "height=\"20\"\n"
                                      "x=\"185.86807\"\n"
                                      "y=\"112.92032\"\n"
                                      "ry=\"1.5\" />\n"
                                      "<g\n"
                                      "style=\"fill:#000000;fill-opacity:1\"\n"
                                      "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                      "<circle\n"
                                      "r=\"2.7314816\"\n"
                                      "cx=\"191.0945\"\n"
                                      "cy=\"118.01437\"\n"
                                      "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                      "<circle\n"
                                      "r=\"2.7314816\"\n"
                                      "cx=\"201.0945\"\n"
                                      "cy=\"128.01437\"\n"
                                      "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                      "</g>\n"
                                      "</g>";

inline constexpr const char* CONST1 = "<g\n"
                                      "style=\"fill:#000000;\"\n"
                                      "transform=\"translate({1},{2})\">\n"
                                      "<rect\n"
                                      "style=\"fill:#000000;stroke:#000000;\"\n"
                                      "width=\"20\"\n"
                                      "height=\"20\"\n"
                                      "x=\"185.86807\"\n"
                                      "y=\"112.92032\"\n"
                                      "ry=\"1.5\" />\n"
                                      "<g\n"
                                      "style=\"fill:#000000;\"\n"
                                      "transform=\"translate(-0.22642517,-0.09405518)\">\n"
                                      "<circle\n"
                                      "r=\"2.7314816\"\n"
                                      "cx=\"191.0945\"\n"
                                      "cy=\"128.01437\"\n"
                                      "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                      "<circle\n"
                                      "r=\"2.7314816\"\n"
                                      "cx=\"201.0945\"\n"
                                      "cy=\"118.01437\"\n"
                                      "style=\"fill:#ffffff;stroke:#ffffff;\" />\n"
                                      "</g>\n"
                                      "</g>";

inline constexpr const char* VIA = "<g\n"
                                   "style=\"fill:#{0};\"\n"
                                   "transform=\"translate({1},{2})\">\n"
                                   "<g>\n"
                                   "<rect\n"
                                   "transform=\"rotate(90)\"\n"
                                   "ry=\"1.5\"\n"
                                   "y=\"-205.86807\"\n"
                                   "x=\"112.92032\"\n"
                                   "height=\"20\"\n"
                                   "width=\"20\"\n"
                                   "style=\"fill:#{0};stroke:#000000;\" />\n"
                                   "<circle\n"
                                   "r=\"7.446404\"\n"
                                   "transform=\"rotate(90)\"\n"
                                   "cx=\"122.92032\"\n"
                                   "cy=\"-195.86807\"\n"
                                   "style=\"fill:none;stroke:#000000;\" />\n"
                                   "</g>\n"
                                   "</g>";

inline constexpr const char* CROSS =
    "<g\n"
    "style=\"fill:#{0};fill-opacity:1\"\n"
    "transform=\"translate({1},{2})\">\n"
    "<g>\n"
    "<rect\n"
    "transform=\"rotate(90)\"\n"
    "ry=\"1.5\"\n"
    "y=\"-205.86807\"\n"
    "x=\"112.92032\"\n"
    "height=\"20\"\n"
    "width=\"20\"\n"
    "style=\"fill:#{0};stroke:#000000;\" />\n"
    "<g\n"
    "style=\"stroke-width:1.05832684;stroke-miterlimit:4;stroke-dasharray:none\"\n"
    "transform=\"matrix(0.94695856,0,0,0.94341063,31.332271,8.4394994)\">\n"
    "<path\n"
    "style=\"stroke:#000000;\"\n"
    "d=\"m 165.8261,113.42213 c 15.83125,15.8311 15.8515,15.85136 15.8515,15.85136\"/>\n"
    "<path\n"
    "style=\"stroke:#000000;\"\n"
    "d=\"m 181.6776,113.42213 c -15.83124,15.83111 -15.85149,15.85137 -15.85149,15.85137\"/>\n"
    "</g>\n"
    "</g>\n"
    "</g>";

inline constexpr const char* SIMPLE_CELL = "<g\n"
                                           "transform=\"translate({1},{2})\">\n"
                                           "<rect\n"
                                           "ry=\"1.5\"\n"
                                           "y=\"112.92032\"\n"
                                           "x=\"185.86807\"\n"
                                           "height=\"20\"\n"
                                           "width=\"20\"\n"
                                           "style=\"fill:#{0};stroke:#000000;\" />\n"
                                           "</g>";

}  // namespace svg

class write_qca_layout_svg_impl
{
  public:
    /**
     * @brief Stores the layout and drawing parameters.
     * @param layout Layout to draw.
     * @param stream Output stream.
     * @param p Drawing parameters.
     */
    write_qca_layout_svg_impl(const qca::layout& layout, std::ostream& stream, write_qca_layout_svg_params p = {}) :
            lyt{layout},
            os{stream},
            ps{std::move(p)}
    {}

    void run()
    {
        if (lyt.get_tile_size_x() == 1 && lyt.get_tile_size_y() == 1)
        {
            generate_cell_based_svg();
        }
        else
        {
            generate_tile_based_svg();
        }
    }

  private:
    const qca::layout& lyt;

    std::ostream& os;

    const write_qca_layout_svg_params ps;

    /**
     * Alias for an SVG description of a tile containing also its clock zone.
     */
    using tile_description = std::pair<std::string, qca::layout::clock_number_t>;
    /**
     * Maps coordinates of tiles to their string representation and their clock zone.
     */
    using coord_to_tile_mapping = std::unordered_map<qca::layout::cell, tile_description>;
    /**
     * Maps coordinates of tiles to a string representation of the cells contained within them.
     */
    using coord_to_cell_list_mapping = std::unordered_map<qca::layout::cell, std::string>;
    /**
     * Alias for an SVG description of a latch tile containing also its clock zone and its offset.
     */
    using svg_latch = std::tuple<std::string, qca::layout::clock_number_t, qca::layout::sync_elem_t>;
    /**
     * Maps coordinates of latch tiles to tuples containing their string representation, their clock zone,
     * and their latch offset.
     */
    using coord_to_latch_mapping = std::unordered_map<qca::layout::cell, svg_latch>;

    /**
     * Generates and returns a pair of strings representing the description and color of the given cell.
     *
     * @param c The cell for which to generate the description and color.
     * @return A pair of strings representing the description and color of the given cell `c`.
     */
    std::pair<std::string, std::string> generate_description_color(const qca::layout::cell& c)
    {
        std::string cell_description{};
        std::string cell_color{};

        static constexpr const std::array<const char*, 4> cell_colors{
            {svg::CLOCK_ZONE_1_CELL, svg::CLOCK_ZONE_2_CELL, svg::CLOCK_ZONE_3_CELL, svg::CLOCK_ZONE_4_CELL}};

        if (lyt.is_empty_cell(c))
        {
            // skip empty cells
        }
        else if (const auto ct = lyt.get_cell_type(c); ct == qca::cell_type::NORMAL)
        {
            cell_color = lyt.is_synchronization_element(c) ? svg::CLOCK_ZONE_LATCH_CELL :
                                                             cell_colors.at(lyt.get_clock_number(c));

            if (ps.simple)
            {
                cell_description = svg::SIMPLE_CELL;
            }
            else if (c.z != 0 && (lyt.get_cell_mode(c) == qca::cell_mode::VERTICAL))
            {
                cell_description = svg::VIA;
            }
            // a ground cell below a crossing-layer cell draws the crossing; a crossing-layer cell draws it only where
            // the ground below is empty, since nothing else would show it
            else if (const auto ac = lyt.above(c), bc = lyt.below(c);
                     ((c != ac) && !lyt.is_empty_cell(ac)) || ((c != bc) && lyt.is_empty_cell(bc)))
            {
                cell_description = svg::CROSS;
            }
            else if (c.z != 0)
            {
                // skip non-ground layer cells
            }
            else
            {
                cell_description = svg::CELL;
            }
        }
        else if (ct == qca::cell_type::INPUT)
        {
            cell_color       = svg::PI_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CELL;
        }
        else if (ct == qca::cell_type::OUTPUT)
        {
            cell_color       = svg::PO_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CELL;
        }
        else if (ct == qca::cell_type::CONST_0)
        {
            cell_color       = fcn::io::detail::svg::CONST_CELL;
            cell_description = ps.simple ? svg::SIMPLE_CELL : svg::CONST0;
        }
        else if (ct == qca::cell_type::CONST_1)
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
     * Generates an SVG string representing the cell-based clocked cell layout and appends it to the output stream.
     */
    void generate_cell_based_svg()
    {
        std::stringstream cell_descriptions{};

        utils::progress_reporter progress{ps.on_progress, "rendering cell positions",
                                          (lyt.x() + 1) * (lyt.y() + 1) * (lyt.z() + 1)};
        lyt.foreach_cell_position(
            [this, &cell_descriptions, &progress](const auto& c)
            {
                if (!lyt.is_empty_cell(c))
                {
                    // Determines cell type and color
                    const auto desc_col = generate_description_color(c);

                    cell_descriptions << fmt::format(
                        fmt::runtime(desc_col.first), desc_col.second,
                        fcn::io::detail::svg::STARTING_OFFSET_TILE_X + fcn::io::detail::svg::STARTING_OFFSET_CELL_X +
                            (c.x * svg::CELL_DISTANCE),
                        fcn::io::detail::svg::STARTING_OFFSET_TILE_Y + fcn::io::detail::svg::STARTING_OFFSET_CELL_Y +
                            (c.y * svg::CELL_DISTANCE));
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

    /**
     * Generates an SVG string representing the tile-based clocked cell layout and appends it to the output stream.
     */
    void generate_tile_based_svg()
    {
        // Collects ALL tile-descriptions
        std::stringstream tile_descriptions{};

        // Used for generating tile-descriptions with information about the tile's coordinates and clock zone
        // It is needed because cells may not be in "tile-order" when read from a cell layout
        coord_to_tile_mapping      coord_to_tile{};
        coord_to_cell_list_mapping coord_to_cells{};
        coord_to_latch_mapping     coord_to_latch_tile{};

        // Used to determine the color of cells, tiles and text based on its clock zone
        static constexpr const std::array<const char*, 4> tile_colors{
            {svg::CLOCK_ZONE_1_TILE, svg::CLOCK_ZONE_2_TILE, svg::CLOCK_ZONE_3_TILE, svg::CLOCK_ZONE_4_TILE}};
        static constexpr const std::array<const char*, 4> text_colors{
            {svg::CLOCK_ZONE_12_TEXT, svg::CLOCK_ZONE_12_TEXT, svg::CLOCK_ZONE_34_TEXT, svg::CLOCK_ZONE_34_TEXT}};

        utils::progress_reporter progress{ps.on_progress, "rendering cell positions",
                                          (lyt.x() + 1) * (lyt.y() + 1) * (lyt.z() + 1)};
        lyt.foreach_cell_position(
            [this, &coord_to_tile, &coord_to_cells, &coord_to_latch_tile, &progress](const auto& c)
            {
                const auto tile_coords  = lyt.get_clock_zone(c);
                const auto clock_number = lyt.get_clock_number(c);
                coord_to_tile.try_emplace(tile_coords, svg::TILE, clock_number);

                if (!lyt.is_empty_cell(c))
                {
                    if (const auto delay = lyt.get_synchronization_element(c); delay > 0)
                    {
                        coord_to_latch_tile.try_emplace(tile_coords, svg::LATCH, clock_number, delay);
                    }

                    const qca::layout::cell in_tile{c.x % lyt.get_tile_size_x(), c.y % lyt.get_tile_size_y()};
                    const auto              desc_col = generate_description_color(c);
                    coord_to_cells[tile_coords].append(
                        fmt::format(fmt::runtime(desc_col.first), desc_col.second,
                                    fcn::io::detail::svg::STARTING_OFFSET_CELL_X + (in_tile.x * svg::CELL_DISTANCE),
                                    fcn::io::detail::svg::STARTING_OFFSET_CELL_Y + (in_tile.y * svg::CELL_DISTANCE)));
                }
                progress.advance();
            });

        // All cell-descriptions are done and tiles have been created

        // Delete empty tiles in simple designs
        if (ps.simple)
        {
            // Delete tiles with missing cell-descriptions for their coordinates
            std::erase_if(coord_to_tile,
                          [&coord_to_cells](const auto& item) { return coord_to_cells.count(item.first) == 0; });
        }

        // Associate tiles with cell-descriptions now; coordinates of tiles are used for tile- and cell-descriptions
        utils::progress_reporter assembly{ps.on_progress, "assembling tile", coord_to_tile.size()};
        for (const auto& [coord, tdscr] : coord_to_tile)
        {
            if (coord_to_latch_tile.contains(coord))
            {
                assembly.advance();
                continue;
            }
            const auto [descr, czone] = tdscr;

            const auto cell_descriptions = coord_to_cells[coord];

            const double x_pos = fcn::io::detail::svg::STARTING_OFFSET_TILE_X + (coord.x * svg::TILE_DISTANCE);
            const double y_pos = fcn::io::detail::svg::STARTING_OFFSET_TILE_Y + (coord.y * svg::TILE_DISTANCE);

            const auto c_descr =
                fmt::format(fmt::runtime(descr), x_pos, y_pos, tile_colors.at(czone), cell_descriptions,
                            text_colors.at(czone), ps.simple ? "" : std::to_string(czone + 1));

            tile_descriptions << c_descr;
            assembly.advance();
        }

        utils::progress_reporter latch_assembly{ps.on_progress, "assembling latch_tile", coord_to_latch_tile.size()};
        for (const auto& [coord, ldscr] : coord_to_latch_tile)
        {
            const auto [descr, czone_up, latch_delay] = ldscr;
            const auto czone_lo = static_cast<std::size_t>((czone_up + latch_delay) % lyt.num_clocks());

            // Tile templates have different origins; all cells use the ordinary tile's coordinate system.
            const auto cell_descriptions = fmt::format(
                "<g transform=\"translate({},{})\">{}</g>",
                fcn::io::detail::svg::STARTING_OFFSET_TILE_X - svg::STARTING_OFFSET_LATCH_X,
                fcn::io::detail::svg::STARTING_OFFSET_TILE_Y - svg::STARTING_OFFSET_LATCH_Y, coord_to_cells[coord]);

            const double x_pos = svg::STARTING_OFFSET_LATCH_X + (coord.x * svg::TILE_DISTANCE);
            const double y_pos = svg::STARTING_OFFSET_LATCH_Y + (coord.y * svg::TILE_DISTANCE);

            const auto t_descr =
                fmt::format(fmt::runtime(descr), x_pos, y_pos, tile_colors.at(czone_lo), tile_colors.at(czone_up),
                            cell_descriptions, text_colors.at(czone_up), ps.simple ? "" : std::to_string(czone_up + 1),
                            text_colors.at(czone_lo), ps.simple ? "" : std::to_string(czone_lo + 1));

            tile_descriptions << t_descr;
            latch_assembly.advance();
        }

        const qca::layout::cell length = {(lyt.x() / lyt.get_tile_size_x()) + 1, (lyt.y() / lyt.get_tile_size_y()) + 1};

        const double viewbox_x = (2 * fcn::io::detail::svg::VIEWBOX_DISTANCE) + (length.x * svg::TILE_DISTANCE);
        const double viewbox_y = (2 * fcn::io::detail::svg::VIEWBOX_DISTANCE) + (length.y * svg::TILE_DISTANCE);

        os << fmt::format(fcn::io::detail::svg::HEADER, FICTION_VERSION, FICTION_REPO, viewbox_x, viewbox_y,
                          tile_descriptions.str());
    }
};

}  // namespace detail

/**
 * Writes an SVG representation of a QCA layout into an output stream. Both tile- and cell-based layouts are
 * supported. For tile-based layouts, QCA layouts of tile size \f$5 \times 5\f$ are supported exclusively so far.
 *
 * The utilized color scheme is based on the standard scheme used in QCADesigner
 * (https://waluslab.ece.ubc.ca/qcadesigner/).
 *
 * May throw an `unsupported_cell_type_exception` if it encounters unsupported cell types in the layout.
 *
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 * @param ps Parameters.
 */
inline void write_qca_layout_svg(const qca::layout& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})
{
    detail::write_qca_layout_svg_impl p{lyt, os, ps};

    p.run();
}
/**
 * Writes an SVG representation of a QCA layout into a file. Both tile- and cell-based layouts are supported.
 * For tile-based layouts, QCA layouts of tile size \f$5 \times 5\f$ are supported exclusively so far.
 *
 * The utilized color scheme is based on the standard scheme used in QCADesigner
 * (https://waluslab.ece.ubc.ca/qcadesigner/).
 *
 * May throw an `unsupported_cell_type_exception` if it encounters unsupported cell types in the layout. May throw an
 * `std::ofstream::failure` if it cannot open the file.
 *
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the `.svg` extension.
 * @param ps Parameters.
 */
inline void write_qca_layout_svg(const qca::layout& lyt, const std::string_view& filename,
                                 const write_qca_layout_svg_params& ps = {})
{
    fiction::detail::atomic_write(filename, [&](std::ostream& os) { write_qca_layout_svg(lyt, os, ps); });
}

}  // namespace fiction::qca::io
