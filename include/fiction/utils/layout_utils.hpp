//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_lattice.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Returns the number of adjacent coordinates of a given one. This is not a constant value because `c` could be located
 * at a layout border.
 *
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param c Coordinate whose number of adjacencies are required.
 * @return Number of `c`'s adjacent coordinates.
 */
template <typename Lyt>
[[nodiscard]] uint8_t num_adjacent_coordinates(const Lyt& lyt, const coordinate<Lyt>& c) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return static_cast<uint8_t>(lyt.adjacent_coordinates(c).size());
}

/**
 * Converts a relative cell position within a tile to an absolute cell position within a layout. To compute the absolute
 * position, the layout topology is taken into account.
 *
 * @tparam GateSizeX Horizontal tile size.
 * @tparam GateSizeY Vertical tile size.
 * @tparam GateLyt Gate-level layout type.
 * @tparam CellLyt Cell-level layout type.
 * @param gate_lyt The gate-level layout whose tiles are to be considered.
 * @param t Tile within gate_lyt.
 * @param relative_c Relative cell position within t.
 * @return Absolute cell position in a layout.
 */
template <uint16_t GateSizeX, uint16_t GateSizeY, typename GateLyt, typename CellLyt>
[[nodiscard]] cell<CellLyt> relative_to_absolute_cell_position(const GateLyt& gate_lyt, const tile<GateLyt>& t,
                                                               const cell<CellLyt>& relative_c) noexcept
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(!has_siqad_coord_v<CellLyt>, "CellLyt cannot have SiQAD coordinates");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");

    assert(relative_c.x < GateSizeX && relative_c.y < GateSizeY &&
           "relative_c must be within the bounds of a single tile");

    cell<CellLyt> absolute_c{};

    // Cartesian layouts
    if constexpr (is_cartesian_layout_v<GateLyt>)
    {
        absolute_c = {t.x * GateSizeX, t.y * GateSizeY, t.z};
    }
    // shifted Cartesian layouts
    else if constexpr (is_shifted_cartesian_layout_v<GateLyt>)
    {
        if constexpr (has_horizontally_shifted_cartesian_orientation_v<GateLyt>)
        {
            absolute_c = {t.x * GateSizeX, static_cast<decltype(absolute_c.y)>(t.y * (GateSizeY)), t.z};
        }
        else if constexpr (has_vertically_shifted_cartesian_orientation_v<GateLyt>)
        {
            absolute_c = {static_cast<decltype(absolute_c.x)>(t.x * (GateSizeX)), t.y * (GateSizeY), t.z};
        }

        if constexpr (has_odd_row_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_row(t))
            {
                // odd rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_even_row_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_row(t))
            {
                // even rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_odd_column_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_column(t))
            {
                // odd columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
        else if constexpr (has_even_column_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_column(t))
            {
                // even columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
    }
    // hexagonal layouts
    else if constexpr (is_hexagonal_layout_v<GateLyt>)
    {
        if constexpr (has_pointy_top_hex_orientation_v<GateLyt>)
        {
            // vertical distance between pointy top hexagons is height * 3/4
            absolute_c = {t.x * GateSizeX, static_cast<decltype(absolute_c.y)>(t.y * (GateSizeY * 3 / 4)), t.z};
        }
        else if constexpr (has_flat_top_hex_orientation_v<GateLyt>)
        {
            // horizontal distance between flat top hexagons is width * 3/4
            absolute_c = {static_cast<decltype(absolute_c.x)>(t.x * (GateSizeX * 3 / 4)), t.y * (GateSizeY), t.z};
        }

        if constexpr (has_odd_row_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_row(t))
            {
                // odd rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_even_row_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_row(t))
            {
                // even rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_odd_column_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_column(t))
            {
                // odd columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
        else if constexpr (has_even_column_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_column(t))
            {
                // even columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
    }
    // more gate-level layout types go here
    else
    {
        assert(false && "unknown gate-level layout type");
    }

    absolute_c.x += relative_c.x;
    absolute_c.y += relative_c.y;

    return absolute_c;
}

/**
 * Port directions address coordinates relative to each other by specifying cardinal directions. This function converts
 * such a relative direction to an absolute coordinate when given a layout and a coordinate therein to consider. That
 * is, when presented with, e.g., a `NORTH_EAST` direction, it will return the coordinate that is to the `NORTH_EAST` of
 * the given coordinate `c` in the layout `lyt`.
 *
 * @tparam Lyt Coordinate layout type.
 * @param lyt Coordinate layout.
 * @param c Coordinate to consider.
 * @param port Port direction.
 * @return Absolute coordinate specified by a coordinate `c` in layout `lyt` and a port direction.
 */
template <typename Lyt>
[[nodiscard]] coordinate<Lyt> port_direction_to_coordinate(const Lyt& lyt, const coordinate<Lyt>& c,
                                                           const port_direction& port) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    switch (port.dir)
    {
        case port_direction::cardinal::NORTH:
        {
            return lyt.north(c);
        }
        case port_direction::cardinal::NORTH_EAST:
        {
            return lyt.north_east(c);
        }
        case port_direction::cardinal::EAST:
        {
            return lyt.east(c);
        }
        case port_direction::cardinal::SOUTH_EAST:
        {
            return lyt.south_east(c);
        }
        case port_direction::cardinal::SOUTH:
        {
            return lyt.south(c);
        }
        case port_direction::cardinal::SOUTH_WEST:
        {
            return lyt.south_west(c);
        }
        case port_direction::cardinal::WEST:
        {
            return lyt.west(c);
        }
        case port_direction::cardinal::NORTH_WEST:
        {
            return lyt.north_west(c);
        }
        default:
        {
            assert(false && "Given port does not specify a cardinal direction");
        }
    }

    return {};
}

/**
 * A new layout is constructed and returned that is equivalent to the given cell-level layout. However, its coordinates
 * are normalized, i.e., start at `(0, 0)` and are all positive. To this end, all existing coordinates are shifted by an
 * x and y offset.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout which is to be normalized.
 * @return New normalized equivalent layout.
 */
template <typename Lyt>
Lyt normalize_layout_coordinates(const Lyt& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    auto x_offset = std::numeric_limits<decltype(lyt.x())>::max();
    auto y_offset = std::numeric_limits<decltype(lyt.y())>::max();

    lyt.foreach_cell(
        [&x_offset, &y_offset](const auto& c)
        {
            if (c.y < y_offset)
            {
                y_offset = c.y;
            }
            if (c.x < x_offset)
            {
                x_offset = c.x;
            }
        });

    Lyt lyt_new{};

    assert(lyt.x() - x_offset >= 0 && "x_offset is too large");
    assert(lyt.y() - y_offset >= 0 && "y_offset is too large");

    lyt_new.resize(
        {static_cast<std::size_t>(lyt.x() - x_offset), static_cast<std::size_t>(lyt.y() - y_offset), lyt.z()});

    lyt_new.set_layout_name(lyt.get_layout_name());
    lyt_new.set_tile_size_x(lyt.get_tile_size_x());
    lyt_new.set_tile_size_y(lyt.get_tile_size_y());

    lyt.foreach_cell(
        [&lyt_new, &lyt, &x_offset, &y_offset](const auto& c)
        {
            lyt_new.assign_cell_type({c.x - x_offset, c.y - y_offset}, lyt.get_cell_type(c));
            lyt_new.assign_cell_mode({c.x - x_offset, c.y - y_offset}, lyt.get_cell_mode(c));
            lyt_new.assign_cell_name({c.x - x_offset, c.y - y_offset}, lyt.get_cell_name(c));
        });

    return lyt_new;
}
/**
 * Converts the coordinates of a given cell-level layout (cds and defect surface can be layered on top) to SiQAD
 * coordinates. A new equivalent layout based on SiQAD coordinates is returned.
 *
 * @tparam Lyt SiDB cell-level layout type based on fiction coordinates, e.g., `offset::ucoord_t` or `cube::coord_t`.
 * @param lyt The layout that is to be converted to a new layout based on SiQAD coordinates.
 * @return A new equivalent layout based on SiQAD coordinates.
 */
template <typename Lyt>
auto convert_layout_to_siqad_coordinates(const Lyt& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    auto process_layout = [](auto& lyt_orig, auto lyt_new)
    {
        lyt_new.resize({lyt_orig.x(), (lyt_orig.y() - lyt_orig.y() % 2) / 2});
        lyt_new.set_layout_name(lyt_orig.get_layout_name());
        lyt_new.set_tile_size_x(lyt_orig.get_tile_size_x());
        lyt_new.set_tile_size_y(lyt_orig.get_tile_size_y());

        lyt_orig.foreach_cell(
            [&lyt_new, &lyt_orig](const auto& c)
            {
                lyt_new.assign_cell_type(siqad::to_siqad_coord(c), lyt_orig.get_cell_type(c));
                lyt_new.assign_cell_mode(siqad::to_siqad_coord(c), lyt_orig.get_cell_mode(c));
                lyt_new.assign_cell_name(siqad::to_siqad_coord(c), lyt_orig.get_cell_name(c));
            });

        if constexpr (is_charge_distribution_surface_v<Lyt> && is_sidb_defect_surface_v<Lyt>)
        {
            auto lyt_defect = sidb_defect_surface{lyt_new};

            lyt_orig.foreach_sidb_defect(
                [&lyt_defect](const auto& cd)
                { lyt_defect.assign_sidb_defect(siqad::to_siqad_coord(cd.first), cd.second); });

            auto lyt_cds_defect = charge_distribution_surface{lyt_defect};

            lyt_orig.foreach_cell(
                [&lyt_cds_defect, &lyt_orig](const auto& c)
                {
                    lyt_cds_defect.assign_charge_state(siqad::to_siqad_coord(c), lyt_orig.get_charge_state(c),
                                                       charge_index_mode::KEEP_CHARGE_INDEX);
                });

            lyt_cds_defect.assign_physical_parameters(lyt_orig.get_simulation_params());

            return lyt_cds_defect;
        }
        else if constexpr (is_sidb_defect_surface_v<Lyt> && !is_charge_distribution_surface_v<Lyt>)
        {
            sidb_defect_surface<decltype(lyt_new)> lyt_surface{lyt_new};
            lyt_orig.foreach_sidb_defect(
                [&lyt_surface, &lyt_orig](const auto& cd)
                {
                    lyt_surface.assign_sidb_defect(siqad::to_siqad_coord(cd.first), lyt_orig.get_sidb_defect(cd.first));
                });
            return lyt_surface;
        }
        else if constexpr (is_charge_distribution_surface_v<Lyt> && !is_sidb_defect_surface_v<Lyt>)
        {
            charge_distribution_surface<decltype(lyt_new)> lyt_new_cds{lyt_new};

            lyt_orig.foreach_cell(
                [&lyt_new_cds, &lyt_orig](const auto& c)
                {
                    lyt_new_cds.assign_charge_state(siqad::to_siqad_coord(c), lyt_orig.get_charge_state(c),
                                                    charge_index_mode::KEEP_CHARGE_INDEX);
                });

            lyt_new_cds.assign_physical_parameters(lyt_orig.get_simulation_params());

            return lyt_new_cds;
        }
        else
        {
            return lyt_new;
        }
    };

    if constexpr (!is_sidb_lattice_v<Lyt>)
    {
        return process_layout(lyt, sidb_cell_clk_lyt_siqad{});
    }
    else
    {
        return process_layout(lyt, sidb_lattice<lattice_orientation<Lyt>, sidb_cell_clk_lyt_siqad>{});
    }
}
/**
 * Converts the coordinates of a given SiDB cell-level layout (cds and defect surface can be layered on top) to
 * alternative coordinates, such as `offset::ucoord_t` or `cube::coord_t`. Returns a new layout equivalent to the
 * original layout but based on the specified coordinate system.
 *
 * @tparam LytDest Source SiDB cell-level layout type.
 * @tparam LytSrc Target SiDB cell-level layout type.
 * @param lyt The layout that is to be converted to a new layout based on fiction coordinates.
 * @return A new equivalent layout based on fiction coordinates.
 */
template <typename LytDest, typename LytSrc>
[[nodiscard]] LytDest convert_layout_to_fiction_coordinates(const LytSrc& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<LytSrc>, "LytSrc is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<LytSrc>, "LytSrc is not a cell-level layout");
    static_assert(has_sidb_technology_v<LytSrc>, "LytSrc is not an SiDB layout");
    static_assert(has_siqad_coord_v<LytSrc>, "LytSrc is not based on SiQAD coordinates");
    static_assert(is_cartesian_layout_v<LytDest>, "LytDest is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<LytDest>, "LytDest is not a cell-level layout");
    static_assert(has_sidb_technology_v<LytDest>, "LytDest is not an SiDB layout");

    static_assert(is_sidb_defect_surface_v<LytDest> == is_sidb_defect_surface_v<LytSrc>,
                  "Only one layout type supports defects");
    static_assert(is_charge_distribution_surface_v<LytDest> == is_charge_distribution_surface_v<LytSrc>,
                  "Only one layout type supports charge distributions");

    static_assert((is_sidb_lattice_111_v<LytDest> == is_sidb_lattice_111_v<LytSrc>) ||
                      (is_sidb_lattice_100_v<LytDest> == is_sidb_lattice_100_v<LytSrc>),
                  "Different lattice orientations");

    bool are_cells_assigned_to_negative_coordinates = false;

    // determine if cells are assigned to negative coordinates. If true, the layout must be normalized first when
    // converting to offset coordinates.
    lyt.foreach_cell(
        [&are_cells_assigned_to_negative_coordinates](const auto& c)
        {
            if (c.x < 0 || c.y < 0)
            {
                are_cells_assigned_to_negative_coordinates = true;
                return false;  // abort
            }

            return true;  // keep looping
        });

    if (are_cells_assigned_to_negative_coordinates && has_offset_ucoord_v<LytDest>)
    {
        return convert_layout_to_fiction_coordinates<LytDest>(normalize_layout_coordinates(lyt));
    }

    auto process_layout = [&lyt](auto lyt_new)
    {
        if constexpr (is_sidb_lattice_v<LytSrc>)
        {
            lyt_new.resize({lyt.x(), lyt.y() * 2 + 1});
            lyt_new.set_layout_name(lyt.get_layout_name());
            lyt_new.set_tile_size_x(lyt.get_tile_size_x());
            lyt_new.set_tile_size_y(lyt.get_tile_size_y());

            lyt.foreach_cell(
                [&lyt_new, &lyt](const auto& c)
                {
                    lyt_new.assign_cell_type(siqad::to_fiction_coord<coordinate<LytDest>>(c), lyt.get_cell_type(c));
                    lyt_new.assign_cell_mode(siqad::to_fiction_coord<coordinate<LytDest>>(c), lyt.get_cell_mode(c));
                    lyt_new.assign_cell_name(siqad::to_fiction_coord<coordinate<LytDest>>(c), lyt.get_cell_name(c));
                });

            if constexpr (is_charge_distribution_surface_v<LytSrc> && is_sidb_defect_surface_v<LytSrc>)
            {
                auto lyt_defect = sidb_defect_surface{lyt_new};

                lyt.foreach_sidb_defect(
                    [&lyt_defect](const auto& cd)
                    {
                        lyt_defect.assign_sidb_defect(siqad::to_fiction_coord<coordinate<LytDest>>(cd.first),
                                                      cd.second);
                    });

                auto lyt_cds_defect = charge_distribution_surface{lyt_defect};

                lyt.foreach_cell(
                    [&lyt_cds_defect, &lyt](const auto& c)
                    {
                        lyt_cds_defect.assign_charge_state(siqad::to_fiction_coord<coordinate<LytDest>>(c),
                                                           lyt.get_charge_state(c),
                                                           charge_index_mode::KEEP_CHARGE_INDEX);
                    });

                lyt_cds_defect.assign_physical_parameters(lyt.get_simulation_params());

                return lyt_cds_defect;
            }
            else if constexpr (is_sidb_defect_surface_v<LytSrc> && !is_charge_distribution_surface_v<LytSrc>)
            {
                LytDest lyt_surface{lyt_new};
                lyt.foreach_sidb_defect(
                    [&lyt_surface, &lyt](const auto& cd)
                    {
                        lyt_surface.assign_sidb_defect(siqad::to_fiction_coord<coordinate<LytDest>>(cd.first),
                                                       lyt.get_sidb_defect(cd.first));
                    });

                return lyt_surface;
            }
            else if constexpr (is_charge_distribution_surface_v<LytSrc> && !is_sidb_defect_surface_v<LytSrc>)
            {
                LytDest lyt_new_cds{sidb_defect_surface{lyt_new}};

                lyt.foreach_cell(
                    [&lyt_new_cds, &lyt](const auto& c)
                    {
                        lyt_new_cds.assign_charge_state(siqad::to_fiction_coord<coordinate<LytDest>>(c),
                                                        lyt.get_charge_state(c), charge_index_mode::KEEP_CHARGE_INDEX);
                    });

                lyt_new_cds.assign_physical_parameters(lyt.get_simulation_params());

                return lyt_new_cds;
            }
            else
            {
                return lyt_new;
            }
        }
    };

    if constexpr (!is_sidb_lattice_v<LytSrc>)
    {
        if constexpr (is_charge_distribution_surface_v<LytSrc> && is_sidb_defect_surface_v<LytSrc>)
        {
            cds_sidb_defect_100_cell_clk_lyt_siqad lyt_100{lyt};

            lyt.foreach_cell([&lyt_100, &lyt](const auto& c)
                             { lyt_100.assign_charge_state(c, lyt.get_charge_state(c), false); });

            lyt_100.assign_physical_parameters(lyt.get_simulation_params());

            lyt.foreach_sidb_defect([&lyt_100](const auto& cd) { lyt_100.assign_sidb_defect(cd.first, cd.second); });
            return convert_layout_to_fiction_coordinates<LytDest, cds_sidb_defect_100_cell_clk_lyt_siqad>(lyt_100);
        }
        else if constexpr (is_charge_distribution_surface_v<LytSrc> && !is_sidb_defect_surface_v<LytSrc>)
        {
            const sidb_100_cell_clk_lyt_siqad lyt_100{lyt};
            cds_sidb_100_cell_clk_lyt_siqad   cds_lyt_100{lyt_100};
            lyt.foreach_cell(
                [&cds_lyt_100, &lyt](const auto& c)
                { cds_lyt_100.assign_charge_state(c, lyt.get_charge_state(c), charge_index_mode::KEEP_CHARGE_INDEX); });

            cds_lyt_100.assign_physical_parameters(lyt.get_simulation_params());

            return convert_layout_to_fiction_coordinates<LytDest, cds_sidb_100_cell_clk_lyt_siqad>(cds_lyt_100);
        }
        else if constexpr (is_sidb_defect_surface_v<LytSrc> && !is_charge_distribution_surface_v<LytSrc>)
        {
            const sidb_100_cell_clk_lyt_siqad                lyt_100{lyt};
            sidb_defect_surface<sidb_100_cell_clk_lyt_siqad> lyt_100_defect{lyt_100};
            lyt.foreach_sidb_defect([&lyt_100_defect, &lyt](const auto& cd)
                                    { lyt_100_defect.assign_sidb_defect(cd.first, lyt.get_sidb_defect(cd.first)); });
            return convert_layout_to_fiction_coordinates<LytDest>(lyt_100_defect);
        }
        else
        {
            return convert_layout_to_fiction_coordinates<LytDest, sidb_100_cell_clk_lyt_siqad>(
                sidb_100_cell_clk_lyt_siqad{lyt});
        }
    }

    else
    {
        if constexpr (has_offset_ucoord_v<LytDest>)
        {
            return process_layout(sidb_lattice<lattice_orientation<LytSrc>, sidb_cell_clk_lyt>{});
        }
        else if constexpr (has_cube_coord_v<LytDest>)
        {
            return process_layout(sidb_lattice<lattice_orientation<LytSrc>, sidb_cell_clk_lyt_cube>{});
        }
        else
        {
            return process_layout(sidb_lattice<lattice_orientation<LytSrc>, sidb_cell_clk_lyt_siqad>{});
        }
    }
}
/**
 * Generates a random coordinate within the region spanned by two given coordinates. The two given coordinates form the
 * top left corner and the bottom right corner of the spanned region.
 *
 * @tparam CoordinateType The coordinate implementation to be used.
 * @param coordinate1 Top left Coordinate.
 * @param coordinate2 Bottom right Coordinate (coordinate order is not important, automatically swapped if
 * necessary).
 * @return Randomly generated coordinate.
 */
template <typename CoordinateType>
CoordinateType random_coordinate(CoordinateType coordinate1, CoordinateType coordinate2) noexcept
{
    static std::mt19937_64 generator(std::random_device{}());

    if (coordinate1 > coordinate2)
    {
        std::swap(coordinate1, coordinate2);
    }

    if constexpr (is_siqad_coord_v<CoordinateType>)
    {
        std::uniform_int_distribution<> dist_x(coordinate1.x, coordinate2.x);
        std::uniform_int_distribution<> dist_y(coordinate1.y, coordinate2.y);
        std::uniform_int_distribution<> dist_z(0, 1);

        return std::clamp(siqad::coord_t{dist_x(generator), dist_y(generator), dist_z(generator)}, coordinate1,
                          coordinate2);
    }
    else
    {
        std::uniform_int_distribution<> dist_x(coordinate1.x, coordinate2.x);
        std::uniform_int_distribution<> dist_y(coordinate1.y, coordinate2.y);
        std::uniform_int_distribution<> dist_z(coordinate1.z, coordinate2.z);

        return {dist_x(generator), dist_y(generator), dist_z(generator)};
    }
}
// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
/**
 * Generates a vector of all coordinates within an area spanned by two coordinates.
 *
 * This function calculates and returns a list of all coordinates within a rectangular area defined by two corner
 * coordinates, inclusive of the boundaries. The coordinates are generated in a top-to-bottom, left-to-right order,
 * covering the entire area between the two specified corners.
 *
 * @tparam CoordinateType Coordinate Type.
 * @param cell_first_corner The cell defining the first corner of the area.
 * @param cell_second_corner The cell defining the second corner of the area.
 * @return A vector containing all cells within the specified area.
 */
template <typename CoordinateType>
[[nodiscard]] inline std::vector<CoordinateType>
all_coordinates_in_spanned_area(const CoordinateType& cell_first_corner,
                                const CoordinateType& cell_second_corner) noexcept
{
    // for SiQAD coordinates
    if constexpr (std::is_same_v<CoordinateType, siqad::coord_t>)
    {
        auto cell_first_corner_cube  = siqad::to_fiction_coord<cube::coord_t>(cell_first_corner);
        auto cell_second_corner_cube = siqad::to_fiction_coord<cube::coord_t>(cell_second_corner);

        cube::coord_t nw_cell{std::min(cell_first_corner_cube.x, cell_second_corner_cube.x),
                              std::min(cell_first_corner_cube.y, cell_second_corner_cube.y)};
        cube::coord_t se_cell{std::max(cell_first_corner_cube.x, cell_second_corner_cube.x),
                              std::max(cell_first_corner_cube.y, cell_second_corner_cube.y)};

        const auto total_cell_count = static_cast<uint64_t>(std::abs(nw_cell.x - se_cell.x) + 1) *
                                      static_cast<uint64_t>(std::abs(nw_cell.y - se_cell.y) + 1);

        std::vector<CoordinateType> all_cells{};
        all_cells.reserve(total_cell_count);

        auto current_cell = nw_cell;

        // collect all cells in the area (spanned by the nw `north-west` and se `south-east` cell) going from top to
        // down from left to right.
        while (current_cell <= se_cell)
        {
            all_cells.push_back(siqad::to_siqad_coord(current_cell));
            if (current_cell.x < se_cell.x)
            {
                current_cell.x += 1;
            }
            else
            {
                current_cell.x = nw_cell.x;
                current_cell.y += 1;
            }
        }

        return all_cells;
    }
    // for cube and offset coordinates
    else
    {
        CoordinateType nw_cell{std::min(cell_first_corner.x, cell_second_corner.x),
                               std::min(cell_first_corner.y, cell_second_corner.y)};
        CoordinateType se_cell{std::max(cell_first_corner.x, cell_second_corner.x),
                               std::max(cell_first_corner.y, cell_second_corner.y)};

        const auto total_cell_count =
            static_cast<uint64_t>(std::abs(static_cast<int64_t>(nw_cell.x) - static_cast<int64_t>(se_cell.x)) + 1) *
            static_cast<uint64_t>(std::abs(static_cast<int64_t>(nw_cell.y) - static_cast<int64_t>(se_cell.y)) + 1);
        std::vector<CoordinateType> all_cells{};
        all_cells.reserve(total_cell_count);

        auto current_cell = nw_cell;

        // collect all cells in the area (spanned by the nw `north-west` and se `south-east` cell) going from top to
        // down from left to right.
        while (current_cell <= se_cell)
        {
            all_cells.push_back(current_cell);

            if (current_cell.x < se_cell.x)
            {
                current_cell.x += 1;
            }
            else
            {
                current_cell.x = nw_cell.x;
                current_cell.y += 1;
            }
        }

        return all_cells;
    }
}
#pragma GCC diagnostic pop
/**
 * This function checks whether the given layouts `first_lyt` and `second_lyt` are identical by comparing various
 * properties such as the number of cells, the types of cells, defects (if applicable), and charge states (if
 * applicable). The comparison is done in a detailed manner depending on the specific layout type.
 *
 * @Note The aspect ratios of the cell-level layouts are not compared.
 *
 * @tparam Lyt The layout type. Must be a cell-level layout.
 * @param first_lyt The first layout to compare.
 * @param second_lyt The second layout to compare.
 * @return `true` if the layouts are identical, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] inline bool are_cell_layouts_identical(const Lyt& first_lyt, const Lyt& second_lyt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    if (first_lyt.num_cells() != second_lyt.num_cells())
    {
        return false;
    }

    bool different_cells = false;

    first_lyt.foreach_cell(
        [&first_lyt, &second_lyt, &different_cells](const auto& c)
        {
            if (first_lyt.get_cell_type(c) != second_lyt.get_cell_type(c))
            {
                different_cells = true;
                return false;  // abort
            }
            return true;  // keep looping
        });

    if (different_cells)
    {
        return false;
    }

    if constexpr (is_sidb_defect_surface_v<Lyt>)
    {
        if (second_lyt.num_defects() != first_lyt.num_defects())
        {
            return false;
        }

        bool different_defects = false;

        first_lyt.foreach_sidb_defect(
            [&first_lyt, &second_lyt, &different_defects](const auto& defect_old)
            {
                if (first_lyt.get_sidb_defect(defect_old.first) != second_lyt.get_sidb_defect(defect_old.first))
                {
                    different_defects = true;
                    return false;  // abort
                }
                return true;  // keep looping
            });

        if (different_defects)
        {
            return false;
        }
    }

    if constexpr (is_charge_distribution_surface_v<Lyt>)
    {
        if (second_lyt.num_neutral_sidbs() != first_lyt.num_neutral_sidbs())
        {
            return false;
        }

        if (second_lyt.num_negative_sidbs() != first_lyt.num_negative_sidbs())
        {
            return false;
        }

        if (second_lyt.num_positive_sidbs() != first_lyt.num_positive_sidbs())
        {
            return false;
        }

        bool different_charge_state = false;
        first_lyt.foreach_cell(
            [&different_charge_state, &first_lyt, &second_lyt](const auto& c)
            {
                if (first_lyt.get_charge_state(c) != second_lyt.get_charge_state(c))
                {
                    different_charge_state = true;
                    return false;  // abort
                }
                return true;  // keep looping
            });

        if (different_charge_state)
        {
            return false;
        }
    }

    return true;
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
