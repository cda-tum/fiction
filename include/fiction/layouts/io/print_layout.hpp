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
 * @brief Simplified 2D textual rendering of gate-, cell-, and SiDB-level layouts.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/traits.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include <array>
#include <cstddef>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace fiction::layouts::io
{

namespace detail
{

// Escape color sequence for input colors (green).
inline const auto INP_COLOR = fmt::fg(fmt::color::green);
// Escape color sequence for output colors (red).
inline const auto OUT_COLOR = fmt::fg(fmt::color::red);
// Escape color sequence for latch colors (yellow on black).
inline const auto SE_COLOR = fmt::fg(fmt::color::yellow) | fmt::bg(fmt::color::black);
// Escape color sequences for clock background colors (white to dark grey).
inline const std::array<fmt::text_style, 4> CLOCK_COLOR{{fmt::fg(fmt::color::black) | fmt::bg(fmt::color::white),
                                                         fmt::fg(fmt::color::black) | fmt::bg(fmt::color::light_gray),
                                                         fmt::fg(fmt::color::white) | fmt::bg(fmt::color::gray),
                                                         fmt::fg(fmt::color::white) | fmt::bg(fmt::color::dark_gray)}};
// Escape color sequence for negatively charged SiDB colors (cyan).
inline const auto SIDB_NEG_COLOR = fmt::fg(fmt::color::cyan);
// Escape color sequence for positively charged SiDB colors (red).
inline const auto SIDB_POS_COLOR = fmt::fg(fmt::color::red);
// Escape color sequence for charge-neutral SiDB colors (white).
inline const auto SIDB_NEUT_COLOR = fmt::fg(fmt::color::white);
// Escape color sequence for lattice background colors (grey).
inline const auto SIDB_LAT_COLOR = fmt::fg(fmt::color::gray);

// Escape color sequence for positively charged defect colors (red).
inline const auto SIDB_DEF_POS_COLOR = fmt::fg(fmt::color::red);
// Escape color sequence for negatively charged defect colors (blue).
inline const auto SIDB_DEF_NEG_COLOR = fmt::fg(fmt::color::blue);
// Escape color sequence for neutrally charged defect colors (yellow).
inline const auto SIDB_DEF_NEU_COLOR = fmt::fg(fmt::color::yellow);
// Empty escape color sequence
inline constexpr auto NO_COLOR = fmt::text_style{};

}  // namespace detail

/**
 * Writes a simplified 2D representation of a gate-level layout to an output stream.
 *
 * @tparam Lyt Gate-level layout type.
 * @param os Output stream to write into.
 * @param layout The gate-level layout to print.
 * @param io_color Flag to utilize color escapes for inputs and outputs.
 * @param clk_color Flag to utilize color escapes for clock zones.
 */
template <typename Lyt>
void print_gate_level_layout(std::ostream& os, const Lyt& layout, const bool io_color = true,
                             const bool clk_color = false)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    // empty layout
    if (layout.num_gates() == 0ul && layout.num_wires() == 0ul)
    {
        os << "[i] empty layout\n";
        return;
    }

    if constexpr (is_hexagonal_layout_v<Lyt>)
    {
        os << "[e] hexagonal layout printing is not supported\n";
        return;
    }
    else if constexpr (is_shifted_cartesian_layout_v<Lyt>)
    {
        os << "[e] shifted cartesian layout printing is not supported\n";
        return;
    }

    const auto num_cols = layout.x() + 1;
    const auto num_rows = layout.y() + 1;

    // cache operations and directions in a 2d-matrix-like object
    using s_matrix = std::vector<std::vector<std::string>>;
    s_matrix reprs(num_rows, std::vector<std::string>(num_cols));
    s_matrix x_dirs(num_rows, std::vector<std::string>(num_cols + 1u, " "));
    s_matrix y_dirs(num_rows + 1u, std::vector<std::string>(num_cols, " "));

    const auto gate_repr = [&layout](const auto& t)
    {
        if (layout.is_empty_tile(t))
        {
            return "▢";
        }

        // NOLINTBEGIN(*-else-after-return)

        if (const auto n = layout.get_node(t); layout.is_and(n))
        {
            return "&";
        }
        else if (layout.is_or(n))
        {
            return "|";
        }
        else if (layout.is_inv(n))
        {
            return "¬";
        }
        else if (layout.is_maj(n))
        {
            return "M";
        }
        else if (layout.is_xor(n))
        {
            return "X";
        }
        else if (layout.is_fanout(n))
        {
            return "F";
        }
        else if (layout.is_wire(n))
        {
            // second-layer wire indicates a crossing
            if (const auto at = layout.above(t); (at != t) && layout.is_wire_tile(at))
            {
                return "+";
            }
            if (layout.is_pi(n))
            {
                return "I";
            }
            if (layout.is_po(n))
            {
                return "O";
            }

            return "=";
        }

        // NOLINTEND(*-else-after-return)

        return "?";
    };

    for (auto i = 0ull; i < num_rows; ++i)
    {
        for (auto j = 0ull; j < num_cols; ++j)
        {
            auto t1     = tile<Lyt>{j, i};
            auto t2     = layout.above(t1);
            reprs[i][j] = gate_repr(t1);

            const auto east_west_connections = [&layout, &x_dirs, &t1, &t2, i, j](const auto n)
            {
                const auto ft = layout.get_tile(n);
                if (layout.is_east_of(t1, ft) || layout.is_east_of(t2, ft))
                {
                    x_dirs[i][j] = "→";
                }
                if (layout.is_west_of(t1, ft) || layout.is_west_of(t2, ft))
                {
                    x_dirs[i][j - 1] = "←";
                }
            };

            const auto north_south_connections = [&layout, &y_dirs, &t1, &t2, i, j](const auto n)
            {
                const auto ft = layout.get_tile(n);
                if (layout.is_north_of(t1, ft) || layout.is_north_of(t2, ft))
                {
                    y_dirs[i][j] = "↑";
                }
                if (layout.is_south_of(t1, ft) || layout.is_south_of(t2, ft))
                {
                    y_dirs[i + 1u][j] = "↓";
                }
            };

            layout.foreach_fanout(layout.get_node(t1), east_west_connections);
            layout.foreach_fanout(layout.get_node(t2), east_west_connections);
            layout.foreach_fanout(layout.get_node(t1), north_south_connections);
            layout.foreach_fanout(layout.get_node(t2), north_south_connections);
        }
    }

    // actual printing
    for (const auto r_ctr : std::views::iota(std::size_t{0}, reprs.size()))
    {
        const auto& row{reprs[r_ctr]};

        for (const auto& d : y_dirs[r_ctr])
        {
            os << d << " ";
        }
        os << '\n';

        for (const auto c_ctr : std::views::iota(std::size_t{0}, row.size()))
        {
            const auto& gate{row[c_ctr]};

            const auto t = tile<Lyt>{c_ctr, r_ctr};

            fmt::text_style color{};

            if (clk_color)
            {
                color = color | detail::CLOCK_COLOR[layout.get_clock_number(t)];
            }
            if constexpr (has_synchronization_elements_v<Lyt>)
            {
                if (io_color && layout.is_synchronization_element(t))
                {
                    color = color | detail::SE_COLOR;
                }
            }
            if (io_color && layout.is_pi_tile(t))
            {
                color = color | detail::INP_COLOR;
            }
            else if (io_color && layout.is_po_tile(t))
            {
                color = color | detail::OUT_COLOR;
            }

            os << fmt::format(color, fmt::runtime(gate));

            os << x_dirs[r_ctr][c_ctr];
        }
        os << '\n';
    }

    // terminate with a newline
    os << "\n";
}
/**
 * Writes a simplified 2D representation of a cell-level layout to an output stream.
 *
 * @tparam Lyt Cell-level layout type.
 * @param os Output stream to write into.
 * @param layout The cell-level layout to print.
 * @param io_color Flag to utilize color escapes for inputs and outputs.
 * @param clk_color Flag to utilize color escapes for clock zones.
 */
template <typename Lyt>
void print_cell_level_layout(std::ostream& os, const Lyt& layout, const bool io_color = true,
                             const bool clk_color = false)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    // empty layout
    if (layout.num_cells() == 0ul)
    {
        os << "[i] empty layout\n";
        return;
    }

    const auto has_cell_above = [&layout](const auto& c)
    {
        for (decltype(layout.z()) z = c.z + decltype(layout.z()){1}; z <= layout.z(); ++z)
        {
            if (!layout.is_empty_cell({c.x, c.y, z}))
            {
                return true;
            }
        }

        return false;
    };

    for (decltype(layout.y()) y_pos = 0; y_pos <= layout.y(); ++y_pos)
    {
        for (decltype(layout.x()) x_pos = 0; x_pos <= layout.x(); ++x_pos)
        {
            cell<Lyt> c{x_pos, y_pos};

            fmt::text_style color{};

            if (clk_color)
            {
                color = color | detail::CLOCK_COLOR[layout.get_clock_number(c)];
            }

            // crossing case
            if (has_cell_above(c))
            {
                os << fmt::format(color, "x");
            }
            else
            {
                const auto ct = layout.get_cell_type(c);

                if constexpr (has_synchronization_elements_v<Lyt>)
                {
                    if (io_color && layout.is_synchronization_element(c))
                    {
                        color = color | detail::SE_COLOR;
                    }
                }
                if (io_color && Lyt::technology::is_input_cell(ct))
                {
                    color = color | detail::INP_COLOR;
                }
                else if (io_color && Lyt::technology::is_output_cell(ct))
                {
                    color = color | detail::OUT_COLOR;
                }

                os << fmt::format(
                    color,
                    fmt::runtime(Lyt::technology::is_normal_cell(ct) ? "▢" : std::string(1u, static_cast<char>(ct))));
            }
        }
        os << '\n';
    }

    // terminate with a newline
    os << "\n";
}
/**
 * A unified printer of the versions above. Depending on the passed layout type, this function will automatically
 * select the appropriate printer to use. This simplifies printing by enabling the statement `print_layout(lyt)`.
 *
 * @note This function will use the respective function's default settings to print the layout.
 *
 * @tparam Lyt Any coordinate layout type.
 * @param lyt The coordinate layout.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void print_layout(const Lyt& lyt, std::ostream& os = std::cout)
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    if constexpr (is_gate_level_layout_v<Lyt>)
    {
        print_gate_level_layout(os, lyt);
    }
    else if constexpr (is_cell_level_layout_v<Lyt>)
    {
        print_cell_level_layout(os, lyt);
    }
    else
    {
        os << "[e] unknown layout type" << '\n';
    }
}

/**
 * Prints every node of a gate-level layout with the tile it occupies and the tiles of its fanins and
 * fanouts. This is a debugging aid; the output format is not stable.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to print.
 * @param os Output stream. Defaults to `std::cout`.
 */
template <typename Lyt>
void print_node_to_tile_assignments(const Lyt& lyt, std::ostream& os = std::cout)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    os << "\n---- NODE TO TILE ----\n";

    lyt.foreach_node(
        [&lyt, &os](const auto& n)
        {
            os << fmt::format("node {} @ {}\n", n, lyt.get_tile(n));
            os << "\t with fanins:\n";
            lyt.foreach_fanin(n,
                              [&lyt, &os](const auto& f)
                              {
                                  const auto fn = lyt.get_node(f);
                                  os << fmt::format("\t   {} @ {}\n", fn, lyt.get_tile(fn));
                              });
            os << "\n\t with fanouts:\n";
            lyt.foreach_fanout(n, [&lyt, &os](const auto& fn)
                               { os << fmt::format("\t   {} @ {}\n", fn, lyt.get_tile(fn)); });
        });

    os << "\n----------------------\n";
}
/**
 * Prints every tile of a gate-level layout with the node it holds, if any, and the tiles of that node's
 * fanins and fanouts. This is a debugging aid; the output format is not stable.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to print.
 * @param os Output stream. Defaults to `std::cout`.
 */
template <typename Lyt>
void print_tile_to_node_assignments(const Lyt& lyt, std::ostream& os = std::cout)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    os << "\n---- TILE TO NODE ----\n";

    lyt.foreach_tile(
        [&lyt, &os](const auto& t)
        {
            os << fmt::format("tile {}\n", t);

            if (const auto n = lyt.get_node(t); n != 0)
            {
                os << fmt::format("node {} @ {}\n", n, lyt.get_tile(n));
                os << "\t with fanins:\n";
                lyt.foreach_fanin(n,
                                  [&lyt, &os](const auto& f)
                                  {
                                      const auto fn = lyt.get_node(f);
                                      os << fmt::format("\t   {} @ {}\n", fn, lyt.get_tile(fn));
                                  });
                os << "\n\t with fanouts:\n";
                lyt.foreach_fanout(n, [&lyt, &os](const auto& fn)
                                   { os << fmt::format("\t   {} @ {}\n", fn, lyt.get_tile(fn)); });
                os << '\n';
            }
        });

    os << "\n----------------------\n";
}

}  // namespace fiction::layouts::io
