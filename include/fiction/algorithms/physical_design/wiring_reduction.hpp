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
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <ostream>
#include <utility>
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
     * Layout width before the wiring reduction process.
     */
    uint64_t x_size_before{0ull};
    /**
     * Layout height before the wiring reduction process.
     */
    uint64_t y_size_before{0ull};
    /**
     * Layout width after the wiring reduction process.
     */
    uint64_t x_size_after{0ull};
    /**
     * Layout height before the wiring reduction process.
     */
    uint64_t y_size_after{0ull};
    /**
     * Number of wire segments before the wiring reduction process.
     */
    uint64_t num_wires_before{0ull};
    /**
     * Number of wire segments after the wiring reduction process.
     */
    uint64_t num_wires_after{0ull};
    /**
     * Improvement in the number wire segments.
     */
    double wiring_improvement{0ull};
    /**
     * Improvement in layout area.
     */
    double area_improvement{0ull};

    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time                      = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size before optimization = {} × {}\n", x_size_before, y_size_before);
        out << fmt::format("[i] layout size after optimization  = {} × {}\n", x_size_after, y_size_after);
        out << fmt::format("[i] area reduction                  = {}%\n", area_improvement);
        out << fmt::format("[i] num. wires before optimization  = {}\n", num_wires_before);
        out << fmt::format("[i] num. wires after optimization   = {}\n", num_wires_after);
        out << fmt::format("[i] wiring reduction                = {}%\n", wiring_improvement);
    }
};

namespace detail
{

/**
 * The two search directions: horizontal (from left to right) and vertical (from top to bottom).
 */
enum class search_direction : uint8_t
{
    /**
     * Search from left to right.
     */
    HORIZONTAL,
    /**
     * Search from top to bottom.
     */
    VERTICAL
};
/**
 * Represents a layout used for wiring reduction derived from the `cartesian_layout` class.
 *
 * This class provides functionality for a wiring reduction layout based on a Cartesian coordinate system.
 * It inherits from the `cartesian_layout` class and extends it with specific behavior for finding excess wiring.
 *
 * @tparam OffsetCoordinateType The type of coordinates used in the layout. Defaults to `offset::ucoord_t` if not
 * explicitly provided.
 */
template <typename OffsetCoordinateType = offset::ucoord_t>
class wiring_reduction_layout : public cartesian_layout<OffsetCoordinateType>
{
  public:
    /**
     * This constructor initializes the `wiring_reduction_layout` with an optional aspect ratio.
     *
     * @param ar The aspect ratio for the layout. Defaults to an empty aspect ratio if not provided.
     * @param direction The search direction to be used. Defaults to HORIZONTAL if not provided.
     */
    explicit wiring_reduction_layout(const typename cartesian_layout<OffsetCoordinateType>::aspect_ratio& ar = {},
                                     search_direction direction = search_direction::HORIZONTAL) :
            cartesian_layout<OffsetCoordinateType>(ar),
            search_dir(direction)
    {}
    /**
     * Getter for the search direction.
     *
     * @return The current search direction.
     */
    [[nodiscard]] search_direction get_search_direction() const noexcept
    {
        return search_dir;
    }
    /**
     * Iterates over adjacent coordinates of a given coordinate and applies a given functor.
     *
     * This function iterates over adjacent coordinates of the given coordinate `c` and applies the provided
     * functor `fn` to each valid adjacent coordinate. The behavior depends on the position of `c` within the layout.
     *
     * @tparam Fn Type of the functor to apply to each adjacent coordinate.
     * @param c The reference coordinate for which adjacent coordinates are determined.
     * @param fn The functor to apply to each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
    {
        if (search_dir == search_direction::HORIZONTAL)
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the first column
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the middle columns
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the last column
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the first row
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the middle rows
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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
     * This function iterates over adjacent coordinates of the given coordinate `c` in the last row
     * and applies the provided functor `fn` to each valid adjacent coordinate.
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

  private:
    /**
     * The current search direction: horizontal (from left to right) and vertical (from top to bottom).
     */
    search_direction search_dir;
};

/**
 * Type alias for an obstruction layout specialized for finding excess wiring.
 */
template <typename OffsetCoordinateType>
using wiring_reduction_layout_type = obstruction_layout<wiring_reduction_layout<OffsetCoordinateType>>;

/**
 * Create a wiring_reduction_layout suitable for finding excess wiring based on a Cartesian layout.
 *
 * This function generates a new layout suitable for finding excess wiring by shifting the input layout based on
 * specified offsets. The generated layout is wrapped in an obstruction_layout. The shifted layout is constructed by
 * iterating through the input Cartesian layout diagonally and obstructing connections and coordinates accordingly.
 *
 * @tparam Lyt Type of the input Cartesian gate-level layout.
 * @param lyt The input Cartesian gate-level layout to be shifted.
 * @param x_offset The offset for shifting in the x-direction. Defaults to 0 if not specified.
 * @param y_offset The offset for shifting in the y-direction. Defaults to 0 if not specified.
 * @param search_direction If set to horizontally, paths are searched from left to right, otherwise from top to bottom.
 * @return wiring_reduction_layout suitable for finding excess wiring via A*.
 */
template <typename Lyt>
wiring_reduction_layout_type<coordinate<Lyt>>
create_wiring_reduction_layout(const Lyt& lyt, const uint64_t x_offset = 0, const uint64_t y_offset = 0,
                               search_direction direction = search_direction::HORIZONTAL) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // Create a wiring_reduction_layout with specified offsets
    wiring_reduction_layout<coordinate<Lyt>> obs_wiring_reduction_layout{
        {lyt.x() + x_offset + 1, lyt.y() + y_offset + 1, lyt.z()},
        direction};

    auto wiring_reduction_lyt = wiring_reduction_layout_type<coordinate<Lyt>>(obs_wiring_reduction_layout);

    // Iterate through nodes in the layout
    lyt.foreach_node(
        [&lyt, &wiring_reduction_lyt, &x_offset, &y_offset](const auto& node)
        {
            const tile<Lyt> old_coord = lyt.get_tile(node);
            const tile<Lyt> new_coord{old_coord.x + x_offset, old_coord.y + y_offset, old_coord.z};

            // Skip if the tile is empty
            if (lyt.is_empty_tile(old_coord))
            {
                return;
            }
            // Handle Primary Inputs (PI) and Primary Outputs (PO)
            if (lyt.is_pi(node) || lyt.is_po(node))
            {
                wiring_reduction_lyt.obstruct_coordinate(new_coord);
                wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y, 1});
            }

            // Handle single input gates and wires
            if (const auto signals = lyt.incoming_data_flow(old_coord); signals.size() == 1)
            {
                const auto      incoming_signal = signals[0];
                const tile<Lyt> shifted_tile{incoming_signal.x + x_offset, incoming_signal.y + y_offset,
                                             incoming_signal.z};

                // Obstruct the connection between the gate and its incoming signal
                wiring_reduction_lyt.obstruct_connection(shifted_tile, new_coord);

                // Obstruct horizontal/vertical wires, non-wire gates (inv) and fanouts
                if (!lyt.is_wire(node) || (lyt.fanout_size(node) != 1) || (old_coord.z != 0) ||
                    (lyt.has_western_incoming_signal({old_coord}) && lyt.has_eastern_outgoing_signal({old_coord}) &&
                     (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)) ||
                    (lyt.has_northern_incoming_signal({old_coord}) && lyt.has_southern_outgoing_signal({old_coord}) &&
                     (wiring_reduction_lyt.get_search_direction() == search_direction::VERTICAL)))
                {
                    wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y, 0});
                    wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y, 1});

                    if (old_coord.z == 0)
                    {
                        return;
                    }

                    // special case 1:
                    // +→=
                    // ↓ ↓
                    // =→+
                    //
                    // special case 2:
                    //   =
                    //   ↓
                    // = =
                    // ↓ ↓
                    // =→+
                    //
                    // special case 3:
                    //
                    //   =→=
                    //     ↓
                    // =→=→+
                    const auto special_case_1 = lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, 0}) &&
                                                lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, 0});
                    const auto special_case_2 = lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, 0}) &&
                                                lyt.has_northern_incoming_signal({old_coord.x, old_coord.y - 1, 0}) &&
                                                lyt.is_wire_tile({old_coord.x, old_coord.y - 2, 0}) &&
                                                lyt.is_wire_tile({old_coord.x - 1, old_coord.y - 1, 0});
                    const auto special_case_3 = lyt.has_western_incoming_signal({old_coord.x - 1, old_coord.y, 0}) &&
                                                lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, 0}) &&
                                                lyt.is_wire_tile({old_coord.x - 2, old_coord.y, 0}) &&
                                                lyt.is_wire_tile({old_coord.x - 1, old_coord.y - 1, 0});
                    if (!(special_case_1 || special_case_2 || special_case_3))
                    {
                        return;
                    }

                    // -> No crossing between coordinate to the left and coordinate above the gate
                    if ((wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL) &&
                        (special_case_1 || special_case_2))
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x - 1, new_coord.y, 0},
                                                                 {new_coord.x, new_coord.y - 1, 0});
                    }
                    else if ((wiring_reduction_lyt.get_search_direction() == search_direction::VERTICAL) &&
                             (special_case_1 || special_case_3))
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x, new_coord.y - 1, 0},
                                                                 {new_coord.x - 1, new_coord.y, 0});
                    }
                }

                // For bent wires from north to east, obstruct the connection between the wire and the
                // coordinate to the bottom right/ top left
                else if (lyt.has_northern_incoming_signal({old_coord}) && lyt.has_eastern_outgoing_signal({old_coord}))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        wiring_reduction_lyt.obstruct_connection(new_coord,
                                                                 {new_coord.x + 1, new_coord.y + 1, new_coord.z});
                        // special case:
                        // →=
                        //  ↓
                        //  =→
                        //
                        // -> Only one wire can be deleted
                        if (lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, old_coord.z}) &&
                            lyt.is_wire(lyt.get_node({old_coord.x, old_coord.y - 1, old_coord.z})))
                        {
                            wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y - 1, 0});
                            wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y - 1, 1});
                        }
                    }
                    else
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x - 1, new_coord.y - 1, new_coord.z},
                                                                 new_coord);
                    }
                }

                // For bent wires from west to south, obstruct the connection between the wire and the
                // coordinate to the top left/ bottom right
                else if (lyt.has_western_incoming_signal({old_coord}) && lyt.has_southern_outgoing_signal({old_coord}))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x - 1, new_coord.y - 1, new_coord.z},
                                                                 new_coord);
                    }
                    else
                    {
                        wiring_reduction_lyt.obstruct_connection(new_coord,
                                                                 {new_coord.x + 1, new_coord.y + 1, new_coord.z});
                        // special case:
                        // ↓
                        // =→=
                        //   ↓
                        //
                        // -> Only one wire can be deleted
                        if (lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                            lyt.is_wire(lyt.get_node({old_coord.x - 1, old_coord.y, old_coord.z})))
                        {
                            wiring_reduction_lyt.obstruct_coordinate({new_coord.x - 1, new_coord.y, 0});
                            wiring_reduction_lyt.obstruct_coordinate({new_coord.x - 1, new_coord.y, 1});
                        }
                    }
                }
            }

            // Handle double input gates (AND, OR, ...)
            else if (signals.size() == 2)
            {
                const auto signal_a = signals[0];
                const auto signal_b = signals[1];

                const auto shifted_tile_a = tile<Lyt>{signal_a.x + x_offset, signal_a.y + y_offset, signal_a.z};
                const auto shifted_tile_b = tile<Lyt>{signal_b.x + x_offset, signal_b.y + y_offset, signal_b.z};

                wiring_reduction_lyt.obstruct_connection(shifted_tile_a, new_coord);
                wiring_reduction_lyt.obstruct_connection(shifted_tile_b, new_coord);

                wiring_reduction_lyt.obstruct_coordinate(new_coord);
                wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y, 1});

                // special case:
                // +→=
                // ↓ ↓
                // =→&
                //
                // -> No crossing between coordinate to the left and coordinate above the gate
                if (lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                      lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, old_coord.z}))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x - 1, new_coord.y, new_coord.z},
                                                                 {new_coord.x, new_coord.y - 1, new_coord.z});
                    }
                    else
                    {
                        wiring_reduction_lyt.obstruct_connection({new_coord.x, new_coord.y - 1, new_coord.z},
                                                                 {new_coord.x - 1, new_coord.y, new_coord.z});
                    }
                }
                // special case:
                // +→=
                // ↓ ↓
                // = =
                // ↓ ↓
                // =→&
                //
                // -> No crossing between coordinate to the left and coordinate above the gate
                if (lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                    lyt.has_northern_incoming_signal({old_coord.x, old_coord.y - 1, old_coord.z}) &&
                    lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y - 1, old_coord.z}) &&
                    lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 2, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x, old_coord.y - 1, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x - 1, old_coord.y - 1, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x, old_coord.y - 2, old_coord.z}) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x - 1, old_coord.y, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x, old_coord.y - 1, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x - 1, old_coord.y - 1, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x, old_coord.y - 2, old_coord.z})))

                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        wiring_reduction_lyt.obstruct_coordinate({new_coord.x - 1, new_coord.y, 0});
                        wiring_reduction_lyt.obstruct_coordinate({new_coord.x - 1, new_coord.y, 1});
                    }
                }
                // special case:
                // +→=→=
                // ↓   ↓
                // =→=→&
                //
                // -> No crossing between coordinate to the left and coordinate above the gate
                if (lyt.has_western_incoming_signal({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                    lyt.has_western_incoming_signal({old_coord.x, old_coord.y - 1, old_coord.z}) &&
                    lyt.has_western_incoming_signal({old_coord.x - 1, old_coord.y - 1, old_coord.z}) &&
                    lyt.has_northern_incoming_signal({old_coord.x - 2, old_coord.y, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x - 1, old_coord.y, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x, old_coord.y - 1, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x - 1, old_coord.y - 1, old_coord.z}) &&
                    lyt.is_wire_tile({old_coord.x - 2, old_coord.y, old_coord.z}) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x - 1, old_coord.y, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x, old_coord.y - 1, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x - 1, old_coord.y - 1, old_coord.z})) &&
                    !lyt.is_fanout(lyt.get_node({old_coord.x - 2, old_coord.y, old_coord.z})))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::VERTICAL)
                    {
                        wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y - 1, 0});
                        wiring_reduction_lyt.obstruct_coordinate({new_coord.x, new_coord.y - 1, 1});
                    }
                }
            }
        });

    return wiring_reduction_lyt;
}
/**
 * Add obstructions to the layout.
 *
 * This function adds obstructions to the provided wiring_reduction_layout. It obstructs coordinates along the top and
 * bottom edges (for left to right) or along the left and right edges (for top to bottom) of the layout in both layers
 * (0 and 1).
 *
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt The wiring_reduction_layout to which obstructions will be added.
 */
template <typename WiringReductionLyt>
void add_obstructions(WiringReductionLyt& lyt) noexcept
{
    if (lyt.get_search_direction() == search_direction::HORIZONTAL)
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
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt Reference to the layout.
 * @param start The starting coordinate of the path.
 * @param end The ending coordinate of the path.
 * @return The computed path as a sequence of coordinates in the layout.
 */
template <typename WiringReductionLyt>
[[nodiscard]] layout_coordinate_path<WiringReductionLyt> get_path(WiringReductionLyt&                   lyt,
                                                                  const coordinate<WiringReductionLyt>& start,
                                                                  const coordinate<WiringReductionLyt>& end) noexcept
{
    using dist = manhattan_distance_functor<WiringReductionLyt, uint64_t>;
    using cost = unit_cost_functor<WiringReductionLyt, uint8_t>;

    static const a_star_params params{false};

    return a_star<layout_coordinate_path<WiringReductionLyt>>(lyt, {start, end}, dist(), cost(), params);
}
/**
 * Update the to-delete list based on a possible path in a wiring_reduction_layout.
 *
 * This function updates the to-delete list by appending coordinates from the given possible path
 * in a wiring_reduction_layout. It considers coordinates that are not at the leftmost (`x == 0`) or rightmost (`x ==
 * lyt.x()`) positions for left to right, or at the top (`y == 0`) or bottom (`y == lyt.y()`) positions for top to
 * bottom and shifts them to get the corresponding coordinates on the original layout. The coordinates are then
 * obstructed in both layers (0 and 1).
 *
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt The `wiring_reduction_layout` to be updated.
 * @param possible_path The path of coordinates to be considered for updating the to-delete list.
 * @param to_delete Reference to the to-delete list to be updated with new coordinates.
 */
template <typename Lyt, typename WiringReductionLyt>
void update_to_delete_list(WiringReductionLyt& lyt, const layout_coordinate_path<WiringReductionLyt>& possible_path,
                           layout_coordinate_path<WiringReductionLyt>& to_delete) noexcept
{
    for (const auto& coord : possible_path)
    {
        // Check if the coordinate is not at the leftmost or rightmost position
        if (((lyt.get_search_direction() == search_direction::HORIZONTAL) && coord.x != 0 && coord.x != lyt.x()) ||
            ((lyt.get_search_direction() == search_direction::VERTICAL) && coord.y != 0 && coord.y != lyt.y()))
        {
            // Create the corresponding coordinate on the original layout
            const fiction::coordinate<WiringReductionLyt> shifted_coord{coord.x - 1, coord.y - 1, 0};

            // Append the corresponding coordinate to the to-delete list
            to_delete.append(shifted_coord);

            // Obstruct the coordinate in both layers
            lyt.obstruct_coordinate({coord.x, coord.y, 0});
            lyt.obstruct_coordinate({coord.x, coord.y, 1});
        }
    }
}
/**
 * Offset matrix type alias.
 */
using offset_matrix = std::vector<std::vector<uint64_t>>;
/**
 * Calculate an offset matrix based on a to-delete list in a `wiring_reduction_layout`.
 *
 * The offset matrix represents the number of deletable coordinates in the same column but above of each specific
 * coordinate when searching from left to right.
 * When searching from top to bottom, the offset matrix represents the number of deletable coordinates in the same row
 * but to the left of each specific coordinate.
 * The matrix is initialized with zeros and updated by incrementing the values for each deletable coordinate.
 *
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt The `wiring_reduction_layout` for which the offset matrix is calculated.
 * @param to_delete The to-delete list representing coordinates to be considered for the offset matrix.
 * @return A 2D vector representing the calculated offset matrix.
 */
template <typename Lyt, typename WiringReductionLyt>
[[nodiscard]] offset_matrix
calculate_offset_matrix(const WiringReductionLyt&                         lyt,
                        const layout_coordinate_path<WiringReductionLyt>& to_delete) noexcept
{
    // Initialize matrix with zeros
    offset_matrix matrix(lyt.y() + 1, std::vector<uint64_t>(lyt.x() + 1, 0));

    // Update matrix based on coordinates
    for (const auto& coord : to_delete)
    {
        const auto x = coord.x;
        const auto y = coord.y;

        if (lyt.get_search_direction() == search_direction::HORIZONTAL)
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
 * This function calculates the new coordinates of a tile after adjusting for wire deletion based on the
 * specified offset and search direction.
 *
 * @tparam Lyt Type of the Cartesian gate-level layout.
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param wiring_reduction_lyt The `wiring_reduction_layout` used to determine the search direction.
 * @param x X-coordinate of the tile.
 * @param y Y-coordinate of the tile.
 * @param z Z-coordinate of the tile.
 * @param offset The offset value used for adjusting the layout.
 * @return The new coordinates of the tile after adjustment.
 */
template <typename Lyt, typename WiringReductionLyt>
[[nodiscard]] tile<Lyt> determine_new_coord(const WiringReductionLyt& wiring_reduction_lyt, uint64_t& x,
                                            const uint64_t& y, uint64_t& z, const uint64_t& offset) noexcept
{
    tile<Lyt> new_coord{};
    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
    {
        new_coord = {x, y - offset, z};
    }
    else
    {
        new_coord = {x - offset, y, z};
    }
    return new_coord;
}
/**
 * This function adjusts the tile and gates in the layout after deleting wires, specifically when traversing
 * in the horizontal search direction. It updates the signals and coordinates accordingly based on the offset matrix.
 *
 * @tparam Lyt Type of the Cartesian gate-level layout.
 * @tparam LytCpy Type of the copy of the original layout for reference.
 * @param lyt The 2DDWave-clocked layout whose wiring is to be reduced.
 * @param layout_copy Copy of the original layout for reference.
 * @param fanin Reference to the tile representing the fanin signal.
 * @param offset_mtrx The offset matrix used for adjusting the layout.
 * @param old_coord The old coordinates before adjustment.
 * @param offset The offset value used for adjusting the layout.
 * @param signals Vector to store signals for the adjusted coordinates.
 */
template <typename Lyt, typename LytCpy>
void adjust_tile_horizontal_search_dir(Lyt& lyt, const LytCpy& layout_copy, tile<Lyt>& fanin,
                                       const offset_matrix& offset_mtrx, const tile<Lyt>& old_coord,
                                       const uint64_t& offset, std::vector<mockturtle::signal<Lyt>>& signals) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (fanin.y == old_coord.y)
    {
        signals.push_back(lyt.make_signal(lyt.get_node({fanin.x, fanin.y - offset, fanin.z})));
    }
    else
    {
        bool traversing_deleted_wires = false;

        // Check if traversing through deleted wires
        if (offset_mtrx[fanin.y + 1][fanin.x] != offset_mtrx[fanin.y][fanin.x])
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
                if ((fanin.y > 0) && (offset_mtrx[fanin.y][fanin.x] != offset_mtrx[fanin.y - 1][fanin.x]) &&
                    (layout_copy.incoming_data_flow(fanin)[0].y != fanin.y))
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
        signals.push_back(lyt.make_signal(lyt.get_node({fanin.x, fanin.y - offset + offset_offset, fanin.z})));
    }
}
/**
 * This function adjusts the tile and gates in the layout after deleting wires, specifically when traversing
 * in the vertical search direction. It updates the signals and coordinates accordingly based on the offset matrix.
 *
 * @tparam Lyt Type of the Cartesian gate-level layout.
 * @tparam LytCpy Type of the copy of the original layout for reference.
 * @param lyt The 2DDWave-clocked layout whose wiring is to be reduced.
 * @param layout_copy Copy of the original layout for reference.
 * @param fanin Reference to the tile representing the fanin signal.
 * @param offset_mtrx The offset matrix used for adjusting the layout.
 * @param old_coord The old coordinates before adjustment.
 * @param offset The offset value used for adjusting the layout.
 * @param signals Vector to store signals for the adjusted coordinates.
 */
template <typename Lyt, typename LytCpy>
void adjust_tile_vertical_search_dir(Lyt& lyt, const LytCpy& layout_copy, tile<Lyt>& fanin,
                                     const offset_matrix& offset_mtrx, const tile<Lyt>& old_coord,
                                     const uint64_t offset, std::vector<mockturtle::signal<Lyt>>& signals) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (fanin.x == old_coord.x)
    {
        signals.push_back(lyt.make_signal(lyt.get_node({fanin.x - offset, fanin.y, fanin.z})));
    }
    else
    {
        bool traversing_deleted_wires = false;

        // Check if traversing through deleted wires
        if (offset_mtrx[fanin.y][fanin.x + 1] != offset_mtrx[fanin.y][fanin.x])
        {
            fanin                    = {fanin.x, fanin.y, 0};
            traversing_deleted_wires = true;
        }

        uint64_t excess_offset = 0;

        // If traversing through deleted wires, update the offset
        if (traversing_deleted_wires)
        {
            for (uint64_t o = 0; o < offset; ++o)
            {
                excess_offset++;
                if ((fanin.x > 0) && (offset_mtrx[fanin.y][fanin.x] != offset_mtrx[fanin.y][fanin.x - 1]) &&
                    (layout_copy.incoming_data_flow(fanin)[0].x != fanin.x))
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
        signals.push_back(lyt.make_signal(lyt.get_node({fanin.x - offset + excess_offset, fanin.y, fanin.z})));
    }
}
/**
 * This function adjusts the tile and gates in the layout after deleting wires. It shifts gates
 * to fill the empty coordinates and adjusts the layout according to the provided offset matrix.
 *
 * @tparam Lyt Type of the Cartesian gate-level layout.
 * @tparam LytCpy Type of the copy of the original layout for reference.
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt The 2DDWave-clocked layout whose wiring is to be reduced.
 * @param layout_copy Copy of the original layout for reference.
 * @param wiring_reduction_lyt The `wiring_reduction_layout`.
 * @param x X-index of the tile to adjust.
 * @param y Y-index of the tile to adjust.
 * @param z Z-index of the tile to adjust.
 * @param offset_mtrx The offset matrix used for adjusting the layout.
 */
template <typename Lyt, typename LytCpy, typename WiringReductionLyt>
void adjust_tile(Lyt& lyt, const LytCpy& layout_copy, const WiringReductionLyt& wiring_reduction_lyt, uint64_t x,
                 const uint64_t y, uint64_t z, const offset_matrix& offset_mtrx) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    const auto      offset    = offset_mtrx[y][x];
    const tile<Lyt> old_coord = {x, y, z};

    // Check if the tile is not empty and has an offset
    if (!(lyt.is_empty_tile(old_coord)) && (offset != 0))
    {
        const auto new_coord = determine_new_coord<Lyt, WiringReductionLyt>(wiring_reduction_lyt, x, y, z, offset);
        std::vector<mockturtle::signal<Lyt>> signals{};
        signals.reserve(layout_copy.fanin_size(layout_copy.get_node(old_coord)));

        // Iterate through fanin and create signals for the new coordinates
        layout_copy.foreach_fanin(
            layout_copy.get_node(old_coord),
            [&lyt, &signals, &offset, &old_coord, &layout_copy, &offset_mtrx, &wiring_reduction_lyt](const auto& fi)
            {
                auto fanin = static_cast<tile<Lyt>>(fi);

                if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                {
                    adjust_tile_horizontal_search_dir(lyt, layout_copy, fanin, offset_mtrx, old_coord, offset, signals);
                }
                else
                {
                    adjust_tile_vertical_search_dir(lyt, layout_copy, fanin, offset_mtrx, old_coord, offset, signals);
                }
            });

        // make sure PO does not die when moving it
        if (lyt.is_po(lyt.get_node(old_coord)))
        {
            if ((wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL) &&
                !lyt.is_empty_tile({new_coord.x + 1, new_coord.y, new_coord.z}))
            {
                lyt.move_node(lyt.get_node({new_coord.x + 1, new_coord.y, new_coord.z}),
                              {new_coord.x + 1, new_coord.y, new_coord.z}, {});
            }
            if ((wiring_reduction_lyt.get_search_direction() == search_direction::VERTICAL) &&
                !lyt.is_empty_tile({new_coord.x, new_coord.y + 1, new_coord.z}))
            {
                lyt.move_node(lyt.get_node({new_coord.x, new_coord.y + 1, new_coord.z}),
                              {new_coord.x, new_coord.y + 1, new_coord.z}, {});
            }
        }
        // Move the node to the new coordinates
        lyt.move_node(lyt.get_node(old_coord), new_coord, signals);
    }
}
/**
 * This function deletes wires from the provided `wiring_reduction_layout` based on the specified coordinates
 * and offset matrix. It clears the tiles in the to-delete list, shifts all gates to fill the empty coordinates, and
 * resizes the layout to an optimized size by calculating the bounding box.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt The 2DDWave-clocked layout whose wiring is to be reduced.
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param wiring_reduction_layout The `wiring_reduction_layout`.
 * @param to_delete The to-delete list representing coordinates of wires to be deleted.
 * each specific coordinate.
 */
template <typename Lyt, typename WiringReductionLyt>
void delete_wires(Lyt& lyt, WiringReductionLyt& wiring_reduction_layout,
                  const layout_coordinate_path<WiringReductionLyt>& to_delete) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    const auto off_mat = calculate_offset_matrix<WiringReductionLyt>(wiring_reduction_layout, to_delete);

    // Create a copy of the original layout for reference
    const auto layout_copy = lyt.clone();

    // Clear tiles based on the to-delete list
    for (const auto& tile_to_delete : to_delete)
    {
        lyt.clear_tile(tile_to_delete);
    }

    // Iterate over the layout to delete wires and adjust the layout
    for (uint64_t k = 0; k < lyt.x() + lyt.y() + 1; ++k)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const uint64_t y = k - x;
            if (x <= lyt.x() && y <= lyt.y())
            {
                for (uint64_t z = 0; z <= lyt.z(); ++z)
                {
                    adjust_tile(lyt, layout_copy, wiring_reduction_layout, x, y, z, off_mat);
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

template <typename Lyt>
class wiring_reduction_impl
{
  public:
    wiring_reduction_impl(const Lyt& lyt, wiring_reduction_stats& st) : plyt{lyt}, pst{st} {}

    void run()
    {
        static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
        static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        pst.num_wires_before = plyt.num_wires() - plyt.num_pis() - plyt.num_pos();
        pst.x_size_before    = plyt.x() + 1;
        pst.y_size_before    = plyt.y() + 1;

        auto layout = obstruction_layout<Lyt>(plyt);

        layout_coordinate_path<wiring_reduction_layout_type<coordinate<Lyt>>> to_delete = {};

        bool found_wires = true;

        // Perform wiring reduction iteratively until no further wires can be deleted
        while (found_wires)
        {
            // Continue until no further wires can be deleted
            found_wires = false;

            for (const auto direction : {search_direction::HORIZONTAL, search_direction::VERTICAL})
            {
                auto wiring_reduction_lyt = create_wiring_reduction_layout<Lyt>(layout, 1, 1, direction);
                add_obstructions(wiring_reduction_lyt);
                to_delete = {};

                // Get the initial possible path
                auto possible_path =
                    get_path(wiring_reduction_lyt, {0, 0}, {wiring_reduction_lyt.x(), wiring_reduction_lyt.y()});

                // Iterate while there is a possible path
                while (!possible_path.empty())
                {
                    update_to_delete_list<Lyt, wiring_reduction_layout_type<coordinate<Lyt>>>(wiring_reduction_lyt,
                                                                                              possible_path, to_delete);

                    // Update possible_path for the next iteration
                    possible_path =
                        get_path(wiring_reduction_lyt, {0, 0}, {wiring_reduction_lyt.x(), wiring_reduction_lyt.y()});
                }

                if (!to_delete.empty())
                {
                    // Calculate offset matrix and delete wires based on to-delete list
                    delete_wires(layout, wiring_reduction_lyt, to_delete);
                    found_wires = true;
                }
            }
        }

        pst.x_size_after = plyt.x() + 1;
        pst.y_size_after = plyt.y() + 1;

        const uint64_t area_before = pst.x_size_before * pst.y_size_before;
        const uint64_t area_after  = pst.x_size_after * pst.y_size_after;

        double area_percentage_difference =
            static_cast<double>(area_before - area_after) / static_cast<double>(area_before) * 100.0;

        area_percentage_difference = std::round(area_percentage_difference * 100) / 100;

        pst.area_improvement = area_percentage_difference;
        pst.num_wires_after  = plyt.num_wires() - plyt.num_pis() - plyt.num_pos();

        double wiring_percentage_difference = static_cast<double>(pst.num_wires_before - pst.num_wires_after) /
                                              static_cast<double>(pst.num_wires_before) * 100.0;

        wiring_percentage_difference = std::round(wiring_percentage_difference * 100) / 100;
        pst.wiring_improvement       = wiring_percentage_difference;
    }

  private:
    /**
     * The 2DDWave-clocked layout whose wiring is to be reduced.
     */
    const Lyt& plyt;
    /**
     * Statistics about the wiring_reduction process.
     */
    wiring_reduction_stats& pst;
};
}  // namespace detail

/**
 * A scalable wiring reduction algorithm for 2DDWave-clocked layouts based on A* path finding.
 *
 * The core concept revolves around the selective removal of excess wiring by cutting them from a layout, contingent
 * upon the ability to restore functional correctness by realigning the remaining layout fragments. Given the complexity
 * of identifying these cuts, obstructions are strategically inserted into the layout to safeguard against the
 * inadvertent deletion of standard gates or wire segments essential for the layout's integrity. Leveraging the
 * obstructed layout as a basis, A* Search is employed to systematically identify feasible cuts either from left to
 * right or top to bottom. Subsequently, these identified cuts are removed from the layout to minimize not only the
 * number of wire segments, but also the area and critical path length.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt The 2DDWave-clocked layout whose wiring is to be reduced.
 * @param pst Pointer to a `wiring_reduction_stats` object to record runtime statistics.
 */
template <typename Lyt>
void wiring_reduction(const Lyt& lyt, wiring_reduction_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // Check if the clocking scheme is 2DDWave
    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "[e] the given layout has to be 2DDWave-clocked\n";
        return;
    }

    // Initialize stats for runtime measurement
    wiring_reduction_stats             st{};
    detail::wiring_reduction_impl<Lyt> p{lyt, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_WIRING_REDUCTION_HPP
