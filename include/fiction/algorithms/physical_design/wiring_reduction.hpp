//
// Created by simon on 09.01.24.
//

#ifndef FICTION_WIRING_REDUCTION_HPP
#define FICTION_WIRING_REDUCTION_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fiction
{

/**
 * This struct stores statistics about the wiring reduction process.
 */
struct wiring_reduction_stats
{
    /**
     * Runtime of the wiring reduction process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

namespace
{
/**
 * Represents a layout used for wiring reduction derived from the cartesian_layout class.
 *
 * This class provides functionality for a wiring reduction layout based on a Cartesian coordinate system.
 * It inherits from the cartesian_layout class and extends it with specific behavior for finding excess wiring.
 *
 * @tparam OffsetCoordinateType The type of coordinates used in the layout.
 *                              Defaults to offset::ucoord_t if not explicitly provided.
 */
template <typename OffsetCoordinateType = offset::ucoord_t>
class wiring_reduction_layout : public cartesian_layout<OffsetCoordinateType>
{
  public:
    bool left_to_right;
    /**
     * This constructor initializes the WiringReductionLayout with an optional aspect ratio.
     *
     * @param ar The aspect ratio for the layout. Defaults to an empty aspect ratio if not provided.
     */
    explicit wiring_reduction_layout(const typename cartesian_layout<OffsetCoordinateType>::aspect_ratio& ar = {},
                                     bool search_left_to_right                                               = true) :
            cartesian_layout<OffsetCoordinateType>(ar),
            left_to_right(search_left_to_right)
    {}

    /**
     * Iterates over adjacent coordinates of a given coordinate.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' and applies the provided
     * functor 'fn' to each valid adjacent coordinate. The behavior depends on the position of 'c' within the layout.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
    {
        if (wiring_reduction_layout<OffsetCoordinateType>::left_to_right)
        {
            if (c.x == 0)
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_first_column(
                    c, std::forward<Fn>(fn));
            }
            else if (c.x == cartesian_layout<OffsetCoordinateType>::x())
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_last_column(
                    c, std::forward<Fn>(fn));
            }
            else
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_middle_columns(
                    c, std::forward<Fn>(fn));
            }
        }
        else
        {
            if (c.y == 0)
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_first_row(
                    c, std::forward<Fn>(fn));
            }
            else if (c.y == cartesian_layout<OffsetCoordinateType>::y())
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_last_row(
                    c, std::forward<Fn>(fn));
            }
            else
            {
                wiring_reduction_layout<OffsetCoordinateType>::foreach_adjacent_coordinate_middle_rows(
                    c, std::forward<Fn>(fn));
            }
        }
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the first column.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the first column
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_first_column(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(cartesian_layout<OffsetCoordinateType>::north(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(c));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(cartesian_layout<OffsetCoordinateType>::south(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(c));
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the middle columns.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the middle columns
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_middle_columns(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(cartesian_layout<OffsetCoordinateType>::north(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(c));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(cartesian_layout<OffsetCoordinateType>::south(c)));
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the last column.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the last column
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_last_column(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(c));
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the first row.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the first row
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_first_row(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(cartesian_layout<OffsetCoordinateType>::east(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(c));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(cartesian_layout<OffsetCoordinateType>::west(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(c));
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the middle rows.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the middle rows
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_middle_rows(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(cartesian_layout<OffsetCoordinateType>::east(c)));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(c));
        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::south(cartesian_layout<OffsetCoordinateType>::west(c)));
    }

    /**
     * Iterates over adjacent coordinates of a given coordinate in the last row.
     *
     * This function iterates over adjacent coordinates of the given coordinate 'c' in the last row
     * and applies the provided functor 'fn' to each valid adjacent coordinate.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each adjacent coordinate.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate_last_row(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(cartesian_layout<OffsetCoordinateType>::east(c));
    }
};

/**
 * Type alias for an obstruction layout specialized for finding excess wiring.
 *
 * The `wiring_reduction_layout` type is an alias for the specialization of the `obstruction_layout` template,
 * using the `WiringReductionLayout<>` class template as its template argument. The `WiringReductionLayout` class
 * provides functionality for a wiring reduction layout based on a Cartesian coordinate system.
 * This alias enhances code readability and indicates the association with wiring reduction features.
 */
using wiring_reduction_lyt = obstruction_layout<wiring_reduction_layout<>>;
}  // namespace

/**
 * Create a shifted layout suitable for finding excess wiring based on a Cartesian layout.
 *
 * This function generates a new layout suitable for finding excess wiring by shifting the input layout
 * based on specified offsets. The generated layout is wrapped in an obstruction_layout.
 * The shifted layout is constructed by iterating through the input Cartesian layout
 * diagonally and obstructing connections and coordinates accordingly.
 *
 * @tparam Lyt Type of the input Cartesian layout.
 * @param lyt The input Cartesian layout to be shifted.
 * @param x_offset The offset for shifting in the x-direction. Defaults to 0 if not specified.
 * @param y_offset The offset for shifting in the y-direction. Defaults to 0 if not specified.
 * @return A new layout with wiring reduction features.
 */
template <typename Lyt>
wiring_reduction_lyt create_shifted_layout(const Lyt& lyt, const uint64_t x_offset = 0, const uint64_t y_offset = 0,
                                           bool left_to_right = true) noexcept
{
    // Create WiringReductionLayout with specified offsets
    wiring_reduction_layout<> obs_shifted_layout{{lyt.x() + x_offset + 1, lyt.y() + y_offset + 1, lyt.z()},
                                                 left_to_right};
    auto                      shifted_layout = wiring_reduction_lyt(obs_shifted_layout);

    // Iterate through nodes in the layout
    lyt.foreach_node(
        [&lyt, &shifted_layout, &x_offset, &y_offset](const auto& node)
        {
            const tile<Lyt> old_coord = lyt.get_tile(node);
            const tile<Lyt> new_coord{old_coord.x + x_offset, old_coord.y + y_offset, old_coord.z};

            // Skip if the tile is empty
            if (!lyt.is_empty_tile(old_coord))
            {
                // Handle Primary Inputs (PI)
                if (lyt.is_pi(node))
                {
                    shifted_layout.obstruct_coordinate(new_coord);
                    shifted_layout.obstruct_coordinate({new_coord.x, new_coord.y, 1});
                }

                // Handle single input gates and wires
                if (const auto signals = lyt.incoming_data_flow(old_coord); signals.size() == 1)
                {
                    const auto      signal = signals[0];
                    const tile<Lyt> shifted_tile{signal.x + x_offset, signal.y + y_offset, signal.z};

                    // Obstruct the connection between the gate and its incoming signal
                    shifted_layout.obstruct_connection(shifted_tile, new_coord);

                    // Obstruct horizontal/vertical wires, non-wire gates (inv) and fanouts
                    if (!lyt.is_wire(node) || (lyt.fanout_size(node) != 1) || (old_coord.z != 0) ||
                        (lyt.has_western_incoming_signal({old_coord}) && lyt.has_eastern_outgoing_signal({old_coord}) &&
                         shifted_layout.left_to_right) ||
                        (lyt.has_northern_incoming_signal({old_coord}) &&
                         lyt.has_southern_outgoing_signal({old_coord}) && !shifted_layout.left_to_right))
                    {
                        shifted_layout.obstruct_coordinate({new_coord.x, new_coord.y, 0});
                        shifted_layout.obstruct_coordinate({new_coord.x, new_coord.y, 1});
                    }

                    // For bent wires from north to east, obstruct the connection between the wire and the
                    // coordinate to the bottom right/ top left
                    else if (lyt.has_northern_incoming_signal({old_coord}) &&
                             lyt.has_eastern_outgoing_signal({old_coord}))
                    {
                        if (shifted_layout.left_to_right)
                        {
                            shifted_layout.obstruct_connection(new_coord,
                                                               {new_coord.x + 1, new_coord.y + 1, new_coord.z});
                        }
                        else
                        {
                            shifted_layout.obstruct_connection({new_coord.x - 1, new_coord.y - 1, new_coord.z},
                                                               new_coord);
                        }
                    }

                    // For bent wires from west to south, obstruct the connection between the wire and the
                    // coordinate to the top left/ bottom right
                    else if (lyt.has_western_incoming_signal({old_coord}) &&
                             lyt.has_southern_outgoing_signal({old_coord}))
                    {
                        if (shifted_layout.left_to_right)
                        {
                            shifted_layout.obstruct_connection({new_coord.x - 1, new_coord.y - 1, new_coord.z},
                                                               new_coord);
                        }
                        else
                        {
                            shifted_layout.obstruct_connection(new_coord,
                                                               {new_coord.x + 1, new_coord.y + 1, new_coord.z});
                        }
                    }
                }

                // Handle double input gates (AND, OR, ...)
                else if (signals.size() == 2)
                {
                    const auto      signal_a = signals[0];
                    const auto      signal_b = signals[1];
                    const tile<Lyt> shifted_tile_a{signal_a.x + x_offset, signal_a.y + y_offset, signal_a.z};
                    const tile<Lyt> shifted_tile_b{signal_b.x + x_offset, signal_b.y + y_offset, signal_b.z};

                    shifted_layout.obstruct_connection(shifted_tile_a, new_coord);
                    shifted_layout.obstruct_connection(shifted_tile_b, new_coord);

                    shifted_layout.obstruct_coordinate(new_coord);
                    shifted_layout.obstruct_coordinate({new_coord.x, new_coord.y, 1});

                    // special case:
                    // +→=
                    // ↓ ↓
                    // =→&
                    //
                    // -> No crossing between coordinate to the left and coordinate above the gate
                    if (lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                        lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, old_coord.z}))
                    {
                        if (shifted_layout.left_to_right)
                        {
                            shifted_layout.obstruct_connection({new_coord.x - 1, new_coord.y, new_coord.z},
                                                               {new_coord.x, new_coord.y - 1, new_coord.z});
                        }
                        else
                        {
                            shifted_layout.obstruct_connection({new_coord.x, new_coord.y - 1, new_coord.z},
                                                               {new_coord.x - 1, new_coord.y, new_coord.z});
                        }
                    }
                }
            }
        });

    return shifted_layout;
}

/**
 * Add obstructions to the layout.
 *
 * This function adds obstructions to the provided WiringReductionLayout. It obstructs coordinates
 * along the top and bottom edges (for left to right) or along the left and right edges (for top to bottom) of the
 * layout in both layers (0 and 1).
 *
 * @tparam Lyt Type of the Cartesian layout.
 * @param lyt The Cartesian layout to which obstructions will be added.
 */
template <typename Lyt>
void add_obstructions(Lyt& lyt) noexcept
{
    if (lyt.left_to_right)
    {
        // Add obstructions to the top edge of the layout
        for (uint64_t x = 1; x <= lyt.x(); x++)
        {
            lyt.obstruct_coordinate({x, 0, 0});
            lyt.obstruct_coordinate({x, 0, 1});
        }

        // Add obstructions to the bottom edge of the layout
        for (uint64_t x = 0; x < lyt.x(); x++)
        {
            lyt.obstruct_coordinate({x, lyt.y(), 0});
            lyt.obstruct_coordinate({x, lyt.y(), 1});
        }
    }
    else
    {
        // Add obstructions to the left edge of the layout
        for (uint64_t y = 1; y <= lyt.y(); y++)
        {
            lyt.obstruct_coordinate({0, y, 0});
            lyt.obstruct_coordinate({0, y, 1});
        }

        // Add obstructions to the right edge of the layout
        for (uint64_t y = 0; y < lyt.y(); y++)
        {
            lyt.obstruct_coordinate({lyt.x(), y, 0});
            lyt.obstruct_coordinate({lyt.y(), y, 1});
        }
    }
}
/**
 * This helper function computes a path between two coordinates using the A* algorithm.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Reference to the layout.
 * @param start The starting coordinate of the path.
 * @param end The ending coordinate of the path.
 *
 * @return The computed path as a sequence of coordinates in the layout.
 */
template <typename Lyt>
[[nodiscard]] layout_coordinate_path<Lyt> get_path(Lyt& lyt, const coordinate<Lyt>& start,
                                                   const coordinate<Lyt>& end) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(has_is_obstructed_coordinate_v<Lyt>, "Lyt is not an obstruction layout");

    using dist = manhattan_distance_functor<Lyt, uint64_t>;
    using cost = unit_cost_functor<Lyt, uint8_t>;
    static const a_star_params params{false};

    auto path = a_star<layout_coordinate_path<Lyt>>(lyt, {start, end}, dist(), cost(), params);

    return path;
}

/**
 * Update the to-delete list based on a possible path in a Wiring Reduction layout.
 *
 * This function updates the to-delete list by appending coordinates from the given possible path
 * in a Wiring Reduction layout. It considers coordinates that are not at the leftmost (x = 0) or rightmost (x =
 * lyt.x()) positions for left to right or at the top (y = 0) or bottom (y =
 * lyt.y()) positions for top to bottom and shifts them to get the corresponding coordinates on the original layout. The
 * coordinates are then obstructed in both layers (0 and 1).
 *
 * @tparam Lyt Type of the Wiring Reduction layout.
 * @param lyt The Wiring Reduction layout to be updated.
 * @param possible_path The path of coordinates to be considered for updating the to-delete list.
 * @param to_delete Reference to the to-delete list to be updated with new coordinates.
 */
template <typename Lyt>
void update_to_delete_list(Lyt& lyt, layout_coordinate_path<wiring_reduction_lyt>& possible_path,
                           layout_coordinate_path<wiring_reduction_lyt>& to_delete)
{
    for (const auto coord : possible_path)
    {
        // Check if the coordinate is not at the leftmost or rightmost position
        if ((lyt.left_to_right && coord.x != 0 && coord.x != lyt.x()) ||
            (!lyt.left_to_right && coord.y != 0 && coord.y != lyt.y()))
        {
            // Create the corresponding coordinate on the original layout
            const fiction::coordinate<Lyt> shifted_coord{coord.x - 1, coord.y - 1, 0};

            // Append the corresponding coordinate to the to-delete list
            to_delete.append(shifted_coord);

            // Obstruct the coordinate in both layers
            lyt.obstruct_coordinate({coord.x, coord.y, 0});
            lyt.obstruct_coordinate({coord.x, coord.y, 1});
        }
    }
}

/**
 * Calculate an offset matrix based on a to-delete list in a Wiring Reduction layout.
 *
 * This function calculates an offset matrix based on the provided to-delete list in a Wiring Reduction layout.
 * The offset matrix represents the number of deletable coordinates in the same column for left to right and same row
 * for top to bottom but above/ to the right of each specific coordinate. The matrix is initialized with zeros and
 * updated by incrementing the values for each deletable coordinate.
 *
 * @tparam Lyt Type of the Wiring Reduction layout.
 * @param lyt The Wiring Reduction layout for which the offset matrix is calculated.
 * @param to_delete The to-delete list representing coordinates to be considered for the offset matrix.
 * @return A 2D vector representing the calculated offset matrix.
 */
template <typename Lyt>
[[nodiscard]] std::vector<std::vector<uint64_t>>
calculate_offset_matrix(Lyt& lyt, layout_coordinate_path<wiring_reduction_lyt>& to_delete, bool left_to_right) noexcept
{
    // Initialize matrix with zeros
    std::vector<std::vector<uint64_t>> matrix(lyt.y() + 1, std::vector<uint64_t>(lyt.x() + 1, 0));

    // Update matrix based on coordinates
    for (const auto& coord : to_delete)
    {
        const uint64_t x = coord.x;
        const uint64_t y = coord.y;
        if (left_to_right)
        {
            for (uint64_t i = lyt.y(); i > y; --i)
            {
                matrix[i][x] += 1;
            }
        }
        else
        {
            for (uint64_t i = lyt.x(); i > x; --i)
            {
                matrix[y][i] += 1;
            }
        }
    }

    return matrix;
}

/**
 * Delete wires from a Wiring Reduction layout based on specified coordinates and offset matrix.
 *
 * This function deletes wires from the provided Wiring Reduction layout based on the specified coordinates
 * and offset matrix. It clears the tiles in the to-delete list, shifts all gates to fill the empty coordinates, and
 * resizes the layout to an optimized size by calculating the bounding box.
 *
 * @tparam Lyt Type of the Wiring Reduction layout.
 * @param lyt The Wiring Reduction layout to be modified.
 * @param to_delete The to-delete list representing coordinates of wires to be deleted.
 * @param offset_matrix The offset matrix representing the number of obstructed coordinates in the same column
 *                     but above each specific coordinate.
 */
template <typename Lyt>
void delete_wires(Lyt& lyt, layout_coordinate_path<wiring_reduction_lyt>& to_delete,
                  std::vector<std::vector<uint64_t>> offset_matrix, bool left_to_right)
{
    // Create a copy of the original layout for reference
    auto layout_copy = lyt.clone();

    // Clear tiles based on the to-delete list
    for (const auto& tile : to_delete)
    {
        lyt.clear_tile(tile);
    }

    if (left_to_right)
    {
        // Iterate through the layout to delete wires and adjust the layout
        for (uint64_t x = 0; x <= lyt.x(); ++x)
        {
            for (uint64_t y = 0; y <= lyt.y(); ++y)
            {
                uint64_t offset = offset_matrix[y][x];

                for (uint64_t z = 0; z <= lyt.z(); ++z)
                {
                    const tile<Lyt> old_coord = {x, y, z};

                    // Check if the tile is not empty and has an offset
                    if (!(lyt.is_empty_tile(old_coord) && (offset != 0)))
                    {
                        const tile<Lyt> new_coord = {x, y - offset, z};

                        std::vector<mockturtle::signal<Lyt>> signals{};
                        signals.reserve(layout_copy.fanin_size(layout_copy.get_node(old_coord)));

                        // Iterate through fanin and create signals for the new coordinates
                        layout_copy.foreach_fanin(
                            layout_copy.get_node(old_coord),
                            [&lyt, &signals, &offset, &old_coord, &layout_copy, &offset_matrix](const auto& i)
                            {
                                auto fanin = static_cast<tile<Lyt>>(i);

                                if (fanin.y == old_coord.y)
                                {
                                    signals.push_back(
                                        lyt.make_signal(lyt.get_node({fanin.x, fanin.y - offset, fanin.z})));
                                }
                                else
                                {
                                    bool traversing_deleted_wires = false;

                                    // Check if traversing through deleted wires
                                    if (offset_matrix[fanin.y + 1][fanin.x] != offset_matrix[fanin.y][fanin.x])
                                    {
                                        fanin                    = {fanin.x, fanin.y, 0};
                                        traversing_deleted_wires = true;
                                    }

                                    uint64_t offset_offset = 0;

                                    // If traversing through deleted wires, update the offset
                                    if (traversing_deleted_wires)
                                    {
                                        for (uint64_t o = 0; o < offset; ++o)
                                        {
                                            offset_offset++;
                                            if (offset_matrix[fanin.y][fanin.x] != offset_matrix[fanin.y - 1][fanin.x])
                                            {
                                                fanin = {fanin.x, fanin.y - 1, fanin.z};
                                            }
                                            else
                                            {
                                                fanin = layout_copy.incoming_data_flow(fanin)[0];
                                                break;
                                            }
                                        }
                                    }

                                    // Create signals for the new coordinates
                                    signals.push_back(lyt.make_signal(
                                        lyt.get_node({fanin.x, fanin.y - offset + offset_offset, fanin.z})));
                                }
                            });

                        // Move the node to the new coordinates
                        lyt.move_node(lyt.get_node(old_coord), new_coord, signals);
                    }
                }
            }
        }
    }
    else
    {
        // Iterate through the layout to delete wires and adjust the layout
        for (uint64_t y = 0; y <= lyt.y(); ++y)
        {
            for (uint64_t x = 0; x <= lyt.x(); ++x)
            {
                uint64_t offset = offset_matrix[y][x];

                for (uint64_t z = 0; z <= lyt.z(); ++z)
                {
                    const tile<Lyt> old_coord = {x, y, z};

                    // Check if the tile is not empty and has an offset
                    if (!(lyt.is_empty_tile(old_coord) && (offset != 0)))
                    {
                        const tile<Lyt> new_coord = {x - offset, y, z};

                        std::vector<mockturtle::signal<Lyt>> signals{};
                        signals.reserve(layout_copy.fanin_size(layout_copy.get_node(old_coord)));

                        // Iterate through fanin and create signals for the new coordinates
                        layout_copy.foreach_fanin(
                            layout_copy.get_node(old_coord),
                            [&lyt, &signals, &offset, &old_coord, &layout_copy, &offset_matrix](const auto& i)
                            {
                                auto fanin = static_cast<tile<Lyt>>(i);

                                if (fanin.x == old_coord.x)
                                {
                                    signals.push_back(
                                        lyt.make_signal(lyt.get_node({fanin.x - offset, fanin.y, fanin.z})));
                                }
                                else
                                {
                                    bool traversing_deleted_wires = false;

                                    // Check if traversing through deleted wires
                                    if (offset_matrix[fanin.y][fanin.x + 1] != offset_matrix[fanin.y][fanin.x])
                                    {
                                        fanin                    = {fanin.x, fanin.y, 0};
                                        traversing_deleted_wires = true;
                                    }

                                    uint64_t offset_offset = 0;

                                    // If traversing through deleted wires, update the offset
                                    if (traversing_deleted_wires)
                                    {
                                        for (uint64_t o = 0; o < offset; ++o)
                                        {
                                            offset_offset++;
                                            if (offset_matrix[fanin.y][fanin.x] != offset_matrix[fanin.y][fanin.x - 1])
                                            {
                                                fanin = {fanin.x - 1, fanin.y, fanin.z};
                                            }
                                            else
                                            {
                                                fanin = layout_copy.incoming_data_flow(fanin)[0];
                                                break;
                                            }
                                        }
                                    }

                                    // Create signals for the new coordinates
                                    signals.push_back(lyt.make_signal(
                                        lyt.get_node({fanin.x - offset + offset_offset, fanin.y, fanin.z})));
                                }
                            });

                        // Move the node to the new coordinates
                        lyt.move_node(lyt.get_node(old_coord), new_coord, signals);
                    }
                }
            }
        }
    }

    // Calculate bounding box for optimized layout size
    const auto bounding_box            = bounding_box_2d(lyt);
    const auto optimized_layout_width  = bounding_box.get_x_size();
    const auto optimized_layout_height = bounding_box.get_y_size();

    // Resize the layout to the optimized size
    lyt.resize({optimized_layout_width, optimized_layout_height, lyt.z()});
}

}  // namespace detail

/**
 * Perform wiring reduction on a 2DDWave clocking scheme layout.
 *
 * This function performs wiring reduction on a 2DDWave clocking scheme layout by iteratively creating
 * shifted layouts, adding obstructions, finding paths, updating the to-delete list, and deleting wires.
 * The process continues until no further wires can be deleted. Optionally, it measures and records
 * runtime statistics.
 *
 * @tparam Lyt Type of the layout.
 * @param lyt The 2DDWave clocking scheme layout to be reduced.
 * @param pst Pointer to a wiring_reduction_stats object to record runtime statistics (optional).
 */
template <typename Lyt>
void wiring_reduction(const Lyt& lyt, wiring_reduction_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // Check if the clocking scheme is 2DDWave
    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "Clocking scheme is not 2DDWave!\n";
        return;
    }

    // Initialize stats for runtime measurement
    wiring_reduction_stats stats{};

    // Measure runtime
    {
        const mockturtle::stopwatch stop{stats.time_total};

        auto                                                 layout    = obstruction_layout<Lyt>(lyt);
        layout_coordinate_path<detail::wiring_reduction_lyt> to_delete = {};

        bool found_wires = true;

        // Perform wiring reduction iteratively until no further wires can be deleted
        while (found_wires)
        {  // Continue until no further wires can be deleted
            found_wires = false;

            for (bool left_to_right : {true, false})
            {
                auto shifted_layout = detail::create_shifted_layout<Lyt>(layout, 1, 1, left_to_right);
                detail::add_obstructions(shifted_layout);
                to_delete = {};

                // Get the initial possible path
                auto possible_path = detail::get_path(shifted_layout, {0, 0}, {shifted_layout.x(), shifted_layout.y()});

                // Iterate while there is a possible path
                while (!possible_path.empty())
                {
                    detail::update_to_delete_list(shifted_layout, possible_path, to_delete);

                    // Update possible_path for the next iteration
                    possible_path = detail::get_path(shifted_layout, {0, 0}, {shifted_layout.x(), shifted_layout.y()});
                }

                // Calculate offset matrix and delete wires based on to-delete list
                const auto offset_matrix = detail::calculate_offset_matrix<Lyt>(layout, to_delete, left_to_right);
                detail::delete_wires(layout, to_delete, offset_matrix, left_to_right);

                if (!to_delete.empty())
                {
                    found_wires = true;
                }
            }
        }
    }

    // Record runtime statistics if a valid pointer is provided
    if (pst != nullptr)
    {
        *pst = stats;
    }
}

}  // namespace fiction

#endif  // FICTION_WIRING_REDUCTION_HPP
