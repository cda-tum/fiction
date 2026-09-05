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
 * @brief Prints an `sidb::layout` to a stream as a colored lattice picture.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/technology.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include <ostream>

namespace fiction::sidb::io
{

namespace detail
{

/**
 * Color of input SiDBs.
 */
inline const auto INPUT_COLOR = fmt::fg(fmt::color::green);
/**
 * Color of output SiDBs.
 */
inline const auto OUTPUT_COLOR = fmt::fg(fmt::color::red);
/**
 * Color of normal SiDBs and of neutral defects.
 */
inline const auto NORMAL_COLOR = fmt::fg(fmt::color::yellow);
/**
 * Color of empty lattice sites.
 */
inline const auto LATTICE_COLOR = fmt::fg(fmt::color::gray);
/**
 * Color of positively charged defects.
 */
inline const auto POSITIVE_DEFECT_COLOR = fmt::fg(fmt::color::red);
/**
 * Color of negatively charged defects.
 */
inline const auto NEGATIVE_DEFECT_COLOR = fmt::fg(fmt::color::blue);
/**
 * Color of negatively charged SiDBs.
 */
inline const auto NEGATIVE_COLOR = fmt::fg(fmt::color::cyan);
/**
 * Color of positively charged SiDBs.
 */
inline const auto POSITIVE_COLOR = fmt::fg(fmt::color::red);
/**
 * Color of neutrally charged SiDBs.
 */
inline const auto NEUTRAL_COLOR = fmt::fg(fmt::color::white);
/**
 * No color.
 */
inline constexpr auto NO_COLOR = fmt::text_style{};

}  // namespace detail

/**
 * Prints an `sidb::layout` as a lattice picture: one symbol per lattice site of the bounding box, row by row, with a
 * blank line between the dimer rows of an H-Si(100)-2x1 surface and an indented second row per unit cell of an
 * H-Si(111)-1x1 surface. Inputs, outputs, normal SiDBs, defects, and empty sites differ by color and symbol. With a
 * charge distribution, a charged SiDB is a filled circle colored by its sign and a neutral one a hollow circle.
 *
 * @param os Output stream to print into.
 * @param lyt Layout to print.
 * @param cd Charge distribution over the layout's SiDBs, or `nullptr` to print cell types only.
 * @param lat_color Whether to color the picture.
 * @param crop_layout Whether to pad the bounding box by two columns and one unit cell instead of printing it tight.
 * @param draw_lattice Whether to print empty lattice sites as dots.
 * @throws std::out_of_range if padding exceeds the lattice-site coordinate range.
 */
inline void print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution* cd, const bool lat_color,
                              const bool crop_layout, const bool draw_lattice)
{
    if (lyt.is_empty() && lyt.num_defects() == 0)
    {
        os << "[i] empty layout" << '\n';
        return;
    }

    const auto& lat = lyt.get_lattice();

    const bool is_100 = lat == lattice::si_100_2x1();
    const bool is_111 = lat == lattice::si_111_1x1();

    auto [min_nw, max_se] = lyt.bounding_box();

    if (crop_layout)
    {
        // apply padding of maximally one dimer row and two columns
        min_nw = min_nw - lattice_site{2, 1};
        max_se = max_se + lattice_site{2, 1};

        // ensure only full dimer rows are printed
        min_nw.z = 0;
        max_se.z = 1;
    }

    const auto paint = [&](const fmt::text_style& style, const char* symbol)
    { os << fmt::format(lat_color ? style : detail::NO_COLOR, "{}", symbol); };

    auto s = min_nw;

    while (s <= max_se)
    {
        bool printed = false;

        if (cd != nullptr)
        {
            switch (cd->get_charge_state(s))
            {
                case model::charge_state::NEGATIVE:
                {
                    paint(detail::NEGATIVE_COLOR, " ● ");
                    printed = true;
                    break;
                }
                case model::charge_state::POSITIVE:
                {
                    paint(detail::POSITIVE_COLOR, " ● ");
                    printed = true;
                    break;
                }
                case model::charge_state::NEUTRAL:
                {
                    paint(detail::NEUTRAL_COLOR, " ◯ ");
                    printed = true;
                    break;
                }
                case model::charge_state::NONE:
                {
                    break;
                }
            }
        }

        if (const auto d = lyt.get_defect(s); d.type != model::defect_type::NONE && !printed)
        {
            if (model::is_negatively_charged_defect(d))
            {
                paint(detail::NEGATIVE_DEFECT_COLOR, " ⊟ ");
            }
            else if (model::is_positively_charged_defect(d))
            {
                paint(detail::POSITIVE_DEFECT_COLOR, " ⊞ ");
            }
            else
            {
                paint(detail::NORMAL_COLOR, " ⊡ ");
            }

            printed = true;
        }

        if (const auto ct = lyt.get_cell_type(s); ct != sidb_technology::cell_type::EMPTY && !printed)
        {
            if (ct == sidb_technology::cell_type::INPUT)
            {
                paint(detail::INPUT_COLOR, " ◯ ");
            }
            else if (ct == sidb_technology::cell_type::OUTPUT)
            {
                paint(detail::OUTPUT_COLOR, " ◯ ");
            }
            else
            {
                paint(detail::NORMAL_COLOR, " ◯ ");
            }

            printed = true;
        }

        if (!printed)
        {
            if (draw_lattice)
            {
                paint(detail::LATTICE_COLOR, " · ");
            }
            else
            {
                os << "  ";
            }
        }

        if (s.x < max_se.x)
        {
            s.x += 1;
        }
        else if (s == max_se)
        {
            if (is_111)
            {
                os << "\n\n";  // gap between two unit cells
            }
            break;
        }
        else
        {
            // gap between two dimer rows
            os << ((s.z == 1 && is_100) ? "\n\n" : "\n");

            s.x = min_nw.x;
            s.y += (s.z == 1) ? 1 : 0;
            s.z = (s.z == 0) ? 1 : 0;

            if (is_111 && s.z == 1)
            {
                os << " ";
            }
        }
    }

    os << "\n";
}

/**
 * Prints an `sidb::layout` as a lattice picture of its cell types. See the overload with a charge distribution
 * pointer for the picture's layout.
 *
 * @param os Output stream to print into.
 * @param lyt Layout to print.
 * @param lat_color Whether to color the picture.
 * @param crop_layout Whether to pad the bounding box by two columns and one unit cell instead of printing it tight.
 * @param draw_lattice Whether to print empty lattice sites as dots.
 */
inline void print_sidb_layout(std::ostream& os, const layout& lyt, const bool lat_color = true,
                              const bool crop_layout = false, const bool draw_lattice = true)
{
    print_sidb_layout(os, lyt, nullptr, lat_color, crop_layout, draw_lattice);
}
/**
 * Prints an `sidb::layout` with a charge distribution as a lattice picture. See the overload with a charge
 * distribution pointer for the picture's layout.
 *
 * @param os Output stream to print into.
 * @param lyt Layout to print.
 * @param cd Charge distribution over the layout's SiDBs.
 * @param lat_color Whether to color the picture.
 * @param crop_layout Whether to pad the bounding box by two columns and one unit cell instead of printing it tight.
 * @param draw_lattice Whether to print empty lattice sites as dots.
 */
inline void print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution& cd,
                              const bool lat_color = true, const bool crop_layout = false,
                              const bool draw_lattice = true)
{
    print_sidb_layout(os, lyt, &cd, lat_color, crop_layout, draw_lattice);
}

}  // namespace fiction::sidb::io
