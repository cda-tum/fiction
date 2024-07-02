//
// Created by marcel on 19.05.21.
//

#ifndef FICTION_PRINT_LAYOUT_HPP
#define FICTION_PRINT_LAYOUT_HPP

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_lattice.hpp"
#include "fiction/technology/sidb_lattice_orientations.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/layout_utils.hpp"

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
// Escape color sequence for latch colors (yellow on black).
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

// Escape color sequence for positively charged defect colors (red).
static const auto SIDB_DEF_POS_COLOR = fmt::fg(fmt::color::red);
// Escape color sequence for negatively charged defect colors (blue).
static const auto SIDB_DEF_NEG_COLOR = fmt::fg(fmt::color::blue);
// Escape color sequence for neutrally charged defect colors (yellow).
static const auto SIDB_DEF_NEU_COLOR = fmt::fg(fmt::color::yellow);
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
                             const bool clk_color = false)
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
 * Writes a simplified 2D representation of an SiDB layout (SiDB and defect charges are supported) to an output stream.
 *
 * @tparam Lyt SiDB cell-level layout with charge-information based on SiQAD coordinates or defect-information, e.g., a
 * `charge_distribution_surface` or `sidb_defect_surface`.
 * @param os Output stream to write into.
 * @param lyt The layout of which the information is to be printed.
 * @param lat_color Flag to utilize color escapes for the lattice, charge states, and atomic defects.
 * @param crop_layout Flag to print the 2D bounding box of the layout, while leaving a maximum padding of one dimer row
 * and two columns.
 * @param draw_lattice Flag to enable lattice background drawing.
 */
template <typename Lyt>
void print_sidb_layout(std::ostream& os, const Lyt& lyt, const bool lat_color = true, const bool crop_layout = false,
                       const bool draw_lattice = true)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if constexpr (!has_siqad_coord_v<Lyt>)
    {
        return print_sidb_layout(os, convert_to_siqad_coordinates(lyt), lat_color, crop_layout, draw_lattice);
    }

    if constexpr (!is_sidb_lattice_v<Lyt>)
    {
        return print_sidb_layout<sidb_lattice<sidb_100_lattice, Lyt>>(os, sidb_lattice<sidb_100_lattice, Lyt>{lyt},
                                                                      lat_color, crop_layout, draw_lattice);
    }

    if constexpr (has_siqad_coord_v<Lyt> && is_sidb_lattice_v<Lyt>)
    {
        // empty layout
        if (lyt.is_empty())
        {
            if constexpr (has_get_sidb_defect_v<Lyt>)
            {
                if (lyt.num_defects() == 0)
                {
                    os << "[i] empty layout" << '\n';
                    return;
                }
            }
            else
            {
                os << "[i] empty layout" << '\n';
                return;
            }
        }

        const bounding_box_2d bb{lyt};

        auto min_nw = bb.get_min();
        auto max_se = bb.get_max();

        if (crop_layout)
        {
            // apply padding of maximally one dimer row and two columns
            min_nw = min_nw - coordinate<Lyt>{2, 1};
            max_se = max_se + coordinate<Lyt>{2, 1};

            // ensure only full dimer rows are printed
            min_nw.z = 0;
            max_se.z = 1;
        }

        // loop coordinate is initialized with the north-west coordinate
        auto loop_coordinate = min_nw;

        while (loop_coordinate <= max_se)
        {
            // Is set to true if either charge or defect is printed at loop coordinate
            bool already_printed = false;

            // Check if layout is only a charge distribution surface
            if constexpr (has_get_charge_state_v<Lyt>)
            {
                switch (lyt.get_charge_state(
                    loop_coordinate))  // switch over the charge state of the SiDB at the current coordinate
                {
                    case sidb_charge_state::NEGATIVE:
                    {
                        os << fmt::format(lat_color ? detail::SIDB_NEG_COLOR : detail::NO_COLOR, " ● ");
                        already_printed = true;
                        break;
                    }
                    case sidb_charge_state::POSITIVE:
                    {
                        os << fmt::format(lat_color ? detail::SIDB_POS_COLOR : detail::NO_COLOR, " ⨁ ");
                        already_printed = true;
                        break;
                    }
                    case sidb_charge_state::NEUTRAL:
                    {
                        os << fmt::format(lat_color ? detail::SIDB_NEUT_COLOR : detail::NO_COLOR, " ◯ ");
                        already_printed = true;
                        break;
                    }
                    case sidb_charge_state::NONE:
                    {
                        break;
                    }
                }
            }

            if constexpr (has_get_sidb_defect_v<Lyt>)
            {
                if (lyt.get_sidb_defect(loop_coordinate) != sidb_defect{sidb_defect_type::NONE})
                {
                    if (is_negatively_charged_defect(lyt.get_sidb_defect(loop_coordinate)))
                    {
                        os << fmt::format(lat_color ? detail::SIDB_DEF_NEG_COLOR : detail::NO_COLOR, " ⊟ ");
                        already_printed = true;
                    }
                    else if (is_positively_charged_defect(lyt.get_sidb_defect(loop_coordinate)))
                    {
                        os << fmt::format(lat_color ? detail::SIDB_DEF_POS_COLOR : detail::NO_COLOR, " ⊞ ");
                        already_printed = true;
                    }
                    else if (is_neutrally_charged_defect(lyt.get_sidb_defect(loop_coordinate)))
                    {
                        os << fmt::format(lat_color ? detail::SIDB_DEF_NEU_COLOR : detail::NO_COLOR, " ⊡ ");
                        already_printed = true;
                    }
                }
            }

            if (!already_printed && lyt.get_cell_type(loop_coordinate) != sidb_technology::cell_type::EMPTY)
            {
                os << fmt::format(lat_color ? detail::SIDB_DEF_NEU_COLOR : detail::NO_COLOR, " ◯ ");
                already_printed = true;
            }

            if (!already_printed)
            {
                os << (draw_lattice ? fmt::format(lat_color ? detail::SIDB_LAT_COLOR : detail::NO_COLOR, " · ") : "  ");
            }

            // if the x-coordinate of loop_coordinate is still less than the x-coordinate of the south-west cell, the
            // x-coordinate is increased by 1
            if (loop_coordinate.x < max_se.x)
            {
                loop_coordinate.x += 1;
            }
            else if (loop_coordinate.x == max_se.x && loop_coordinate != max_se)
            {
                if (loop_coordinate.z == 1 && is_sidb_lattice_100_v<Lyt>)
                {
                    os << "\n\n";  // gap between two dimers
                }
                else
                {
                    os << "\n";
                }
                loop_coordinate.x = min_nw.x;
                loop_coordinate.y += (loop_coordinate.z == 1) ? 1 : 0;
                loop_coordinate.z = (loop_coordinate.z == 0) ? 1 : 0;
                if (is_sidb_lattice_111_v<Lyt> && loop_coordinate.z == 1)
                {
                    os << " ";
                }
            }
            else if (loop_coordinate == max_se)
            {
                if (is_sidb_lattice_111_v<Lyt>)
                {
                    os << "\n\n";  // add a gap between two dimers
                }
                break;
            }
        }
        // flush stream
        os << std::endl;
    }
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
        if constexpr (has_sidb_technology_v<Lyt>)
        {
            print_sidb_layout(os, lyt);
        }
        else
        {
            print_cell_level_layout(os, lyt);
        }
    }
    else
    {
        os << "[e] unknown layout type" << '\n';
    }
}

}  // namespace fiction

#endif  // FICTION_PRINT_LAYOUT_HPP
