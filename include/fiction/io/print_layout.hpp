//
// Created by marcel on 19.05.21.
//

#ifndef FICTION_PRINT_LAYOUT_HPP
#define FICTION_PRINT_LAYOUT_HPP

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace fiction
{

namespace detail
{

// Escape color sequence for input colors (green).
static const auto INP_COLOR = fmt::fg(fmt::color::green);
// Escape color sequence for output colors (red).
static const auto OUT_COLOR = fmt::fg(fmt::color::red);
//// Escape color sequence for latch colors (yellow on black).
static const auto SE_COLOR = fmt::fg(fmt::color::yellow) | fmt::bg(fmt::color::black);
// Escape color sequences for clock background colors (white to dark grey).
static const std::array<fmt::text_style, 4> CLOCK_COLOR{{fmt::fg(fmt::color::black) | fmt::bg(fmt::color::white),
                                                         fmt::fg(fmt::color::black) | fmt::bg(fmt::color::light_gray),
                                                         fmt::fg(fmt::color::white) | fmt::bg(fmt::color::gray),
                                                         fmt::fg(fmt::color::white) | fmt::bg(fmt::color::dark_gray)}};
// Escape color sequence for negatively charged SiDB colors (cyan).
static const auto SIDB_NEG_COLOR = fmt::fg(fmt::color::cyan);
// Escape color sequence for positively charged SiDB colors (red).
static const auto SIDB_POS_COLOR = fmt::fg(fmt::color::red);
// Escape color sequence for charge-neutral SiDB colors (white).
static const auto SIDB_NEUT_COLOR = fmt::fg(fmt::color::white);
// Escape color sequence for lattice background colors (grey).
static const auto SIDB_LAT_COLOR = fmt::fg(fmt::color::gray);
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
                             const bool clk_color = false) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    // empty layout
    if (layout.num_gates() == 0ul && layout.num_wires() == 0ul)
    {
        os << "[i] empty layout" << std::endl;
        return;
    }

    if constexpr (is_hexagonal_layout_v<Lyt>)
    {
        os << "[e] hexagonal layout printing is not supported" << std::endl;
        return;
    }
    else if constexpr (is_shifted_cartesian_layout_v<Lyt>)
    {
        os << "[e] shifted cartesian layout printing is not supported" << std::endl;
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
    auto r_ctr = 0u;
    auto c_ctr = 0u;
    for (const auto& row : reprs)
    {
        for (const auto& d : y_dirs[r_ctr])
        {
            os << d << " ";
        }
        os << '\n';

        for (const auto& gate : row)
        {
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

            os << fmt::format(color, gate);

            os << x_dirs[r_ctr][c_ctr];
            ++c_ctr;
        }
        c_ctr = 0u;
        os << '\n';

        ++r_ctr;
    }

    // flush stream
    os << std::endl;
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
                             const bool clk_color = false) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    // empty layout
    if (layout.num_cells() == 0ul)
    {
        os << "[i] empty layout" << std::endl;
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

                os << fmt::format(color,
                                  (Lyt::technology::is_normal_cell(ct) ? "▢" : std::string(1u, static_cast<char>(ct))));
            }
        }
        os << '\n';
    }

    // flush stream
    os << std::endl;
}
/**
 * Writes a simplified 2D representation of an SiDB charge layout to an output stream.
 *
 * @tparam Lyt Cell-level layout based on SiQAD coordinates.
 * @param os Output stream to write into.
 * @param cds The charge distribution surface of which the charge layout is to be printed.
 * @param cs_color Flag to utilize color escapes for charge states.
 * @param crop_layout Flag to print the 2D bounding box of the layout, while leaving a maximum padding of one dimer row
 * and two columns.
 * @param draw_lattice Flag to enable lattice background drawing.
 */
template <typename Lyt>
void print_charge_layout(std::ostream& os, const charge_distribution_surface<Lyt>& cds, const bool cs_color = true,
                         const bool crop_layout = false, const bool draw_lattice = true) noexcept
{
    // empty layout
    if (cds.is_empty())
    {
        os << "[i] empty layout" << std::endl;
        return;
    }

    coordinate<Lyt> min{};
    coordinate<Lyt> max{cds.x(), cds.y(), 1};

    if (crop_layout)
    {
        const auto bb = bounding_box_2d{cds};

        // apply padding of maximally one dimer row and two columns
        min = bb.get_min() - coordinate<Lyt>{2, 1};
        max = bb.get_max() + coordinate<Lyt>{2, 1};

        // ensure only full dimer rows are printed
        min.z = 0;
        max.z = 1;
    }

    // iterate over all coordinates in the rows determined by the vertical crop
    cds.foreach_coordinate(
        [&](const coordinate<Lyt>& c)
        {
            if (crop_layout && (c.x < min.x || c.x > max.x))  // apply horizontal crop
            {
                return;
            }

            switch (cds.get_charge_state(c))  // switch over the charge state of the SiDB at the current coordinate
            {
                case sidb_charge_state::NEGATIVE:
                {
                    os << fmt::format(cs_color ? detail::SIDB_NEG_COLOR : detail::NO_COLOR, " ● ");
                    break;
                }
                case sidb_charge_state::POSITIVE:
                {
                    os << fmt::format(cs_color ? detail::SIDB_POS_COLOR : detail::NO_COLOR, " ⨁ ");
                    break;
                }
                case sidb_charge_state::NEUTRAL:
                {
                    os << fmt::format(cs_color ? detail::SIDB_NEUT_COLOR : detail::NO_COLOR, " ◯ ");
                    break;
                }
                default:  // NONE charge state case -> empty cell
                {
                    os << (draw_lattice || !cds.is_empty_cell(c) ?
                               fmt::format(cs_color ? detail::SIDB_LAT_COLOR : detail::NO_COLOR, " · ") :
                               "  ");
                }
            }

            if (c.x == max.x)
            {
                os << (c.z == 1 ? "\n\n" : "\n");
            }
        },
        min, max + coordinate<Lyt>{1, 0});

    // flush stream
    os << std::endl;
}

}  // namespace fiction

#endif  // FICTION_PRINT_LAYOUT_HPP
