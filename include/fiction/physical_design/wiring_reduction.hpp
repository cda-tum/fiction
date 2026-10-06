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
 * @brief Removes excess wiring from a gate-level layout to shrink its area.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/obstructions.hpp"
#include "fiction/physical_design/path_finding/a_star.hpp"
#include "fiction/physical_design/path_finding/cost.hpp"
#include "fiction/physical_design/path_finding/distance.hpp"
#include "fiction/physical_design/routing_utils.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/progress.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::physical_design
{

/**
 * Parameters for the wiring reduction algorithm.
 */
struct wiring_reduction_params
{
    /**
     * Timeout limit (in ms). Specifies the maximum allowed time in milliseconds for the optimization process. For large
     * layouts, the actual execution time may slightly exceed this limit because it's impractical to check the timeout
     * at every algorithm step and the functional correctness has to be ensured by completing essential algorithm steps.
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
    /**
     * Callback that receives the number of wire paths processed so far.
     */
    utils::progress_callback on_progress{};
};

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
 */
class wiring_reduction_layout : public layouts::cartesian_layout
{
  public:
    /**
     * Constructs a search grid with zero-origin, half-open dimensions.
     *
     * @param ar Search-grid extent. Defaults to an empty extent.
     * @param direction The search direction to be used. Defaults to HORIZONTAL if not provided.
     */
    explicit wiring_reduction_layout(const layouts::cartesian_layout::extent& ar = {},
                                     search_direction direction                  = search_direction::HORIZONTAL) :
            layouts::cartesian_layout(ar),
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
    void foreach_adjacent_coordinate(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        if (search_dir == search_direction::HORIZONTAL)
        {
            if (c.x == 0)
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_first_column(c, std::forward<Fn>(fn));
            }
            else if (c.x == (layouts::cartesian_layout::width() - 1))
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_last_column(c, std::forward<Fn>(fn));
            }
            else
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_middle_columns(c, std::forward<Fn>(fn));
            }
        }
        else
        {
            if (c.y == 0)
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_first_row(c, std::forward<Fn>(fn));
            }
            else if (c.y == (layouts::cartesian_layout::height() - 1))
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_last_row(c, std::forward<Fn>(fn));
            }
            else
            {
                wiring_reduction_layout::foreach_adjacent_coordinate_middle_rows(c, std::forward<Fn>(fn));
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
    void foreach_adjacent_coordinate_first_column(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::north(c) ?
                           layouts::cartesian_layout::east(*layouts::cartesian_layout::north(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::east(c));
        apply_if_not_c(layouts::cartesian_layout::south(c) ?
                           layouts::cartesian_layout::east(*layouts::cartesian_layout::south(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::south(c));
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
    void foreach_adjacent_coordinate_middle_columns(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::north(c) ?
                           layouts::cartesian_layout::east(*layouts::cartesian_layout::north(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::east(c));
        apply_if_not_c(layouts::cartesian_layout::south(c) ?
                           layouts::cartesian_layout::east(*layouts::cartesian_layout::south(c)) :
                           std::nullopt);
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
    void foreach_adjacent_coordinate_last_column(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::south(c));
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
    void foreach_adjacent_coordinate_first_row(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::east(c) ?
                           layouts::cartesian_layout::south(*layouts::cartesian_layout::east(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::south(c));
        apply_if_not_c(layouts::cartesian_layout::west(c) ?
                           layouts::cartesian_layout::south(*layouts::cartesian_layout::west(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::east(c));
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
    void foreach_adjacent_coordinate_middle_rows(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::east(c) ?
                           layouts::cartesian_layout::south(*layouts::cartesian_layout::east(c)) :
                           std::nullopt);
        apply_if_not_c(layouts::cartesian_layout::south(c));
        apply_if_not_c(layouts::cartesian_layout::west(c) ?
                           layouts::cartesian_layout::south(*layouts::cartesian_layout::west(c)) :
                           std::nullopt);
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
    void foreach_adjacent_coordinate_last_row(const layouts::layout_base::coordinate& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal)
        {
            if (cardinal && *cardinal != c)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_not_c(layouts::cartesian_layout::east(c));
    }

    /** @brief Constraints of this wiring-cut search, passed to path searches on this layout. */
    layouts::obstructions search_obstructions{};

  private:
    /**
     * The current search direction: horizontal (from left to right) and vertical (from top to bottom).
     */
    search_direction search_dir;
};

/**
 * Create a wiring_reduction_layout suitable for finding excess wiring based on a Cartesian layout.
 *
 * This function generates a new layout suitable for finding excess wiring by shifting the input layout based on
 * specified offsets. The generated search layout owns its obstruction data. The shifted layout is constructed by
 * iterating through the input Cartesian layout diagonally and obstructing connections and coordinates accordingly.
 *
 * @tparam Lyt Type of the input Cartesian gate-level layout.
 * @param lyt The input Cartesian gate-level layout to be shifted.
 * @param x_offset The offset for shifting in the x-direction. Defaults to 0 if not specified.
 * @param y_offset The offset for shifting in the y-direction. Defaults to 0 if not specified.
 * @param direction If set to horizontally, paths are searched from left to right, otherwise from top to bottom.
 * @return wiring_reduction_layout suitable for finding excess wiring via A*.
 */
template <typename Lyt>
wiring_reduction_layout create_wiring_reduction_layout(const Lyt& lyt, const int32_t x_offset = 0,
                                                       const int32_t    y_offset  = 0,
                                                       search_direction direction = search_direction::HORIZONTAL)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // create a wiring_reduction_layout with specified offsets
    wiring_reduction_layout wiring_reduction_lyt{{static_cast<int64_t>(lyt.width()) + x_offset + 1,
                                                  static_cast<int64_t>(lyt.height()) + y_offset + 1, lyt.layers()},
                                                 direction};

    // iterate through nodes in the layout
    lyt.foreach_node(
        [&lyt, &wiring_reduction_lyt, &x_offset, &y_offset](const auto& node)
        {
            const tile<Lyt> old_coord = lyt.get_tile(node);
            const tile<Lyt> new_coord{old_coord.x + x_offset, old_coord.y + y_offset, old_coord.z};

            // skip if the tile is empty
            if (lyt.is_empty_tile(old_coord))
            {
                return;
            }
            // handle Primary Inputs (PI) and Primary Outputs (PO)
            if (lyt.is_pi(node) || lyt.is_po(node))
            {
                wiring_reduction_lyt.search_obstructions.obstruct_coordinate(new_coord);
                wiring_reduction_lyt.search_obstructions.obstruct_coordinate({new_coord.x, new_coord.y, 1});
            }

            // utility function to check if a tile hosts a single wire only, which is not a fanout or hosts a
            // crossing:
            //
            // =
            auto is_single_wire = [&lyt, &old_coord](const int32_t add_x_offset, const int32_t add_y_offset)
            {
                const auto id = lyt.find_object({old_coord.x - add_x_offset, old_coord.y - add_y_offset, 0});
                return id && lyt.is_wire(*id) && !lyt.is_gate(*id) && !lyt.is_pi(*id) && !lyt.is_po(*id) &&
                       !lyt.is_fanout(*id) &&
                       lyt.is_empty_tile({old_coord.x - add_x_offset, old_coord.y - add_y_offset, 1});
            };

            // utility function to check for crossings with outgoing wires to the bottom layer:
            //
            // +→=
            // ↓
            // =
            auto is_crossing = [&lyt, &old_coord](const int32_t add_x_offset, const int32_t add_y_offset)
            {
                return lyt.has_northern_incoming_signal(
                           {old_coord.x - add_x_offset, old_coord.y - add_y_offset + 1, 0}) &&
                       lyt.has_western_incoming_signal({old_coord.x - add_x_offset + 1, old_coord.y - add_y_offset, 0});
            };

            // utility function to fully obstruct a coordinate
            auto obstruct_coordinate =
                [&wiring_reduction_lyt, &new_coord](const int32_t add_x_offset, const int32_t add_y_offset)
            {
                wiring_reduction_lyt.search_obstructions.obstruct_coordinate(
                    {new_coord.x - add_x_offset, new_coord.y - add_y_offset, 0});
                wiring_reduction_lyt.search_obstructions.obstruct_coordinate(
                    {new_coord.x - add_x_offset, new_coord.y - add_y_offset, 1});
            };

            if (lyt.is_gate(node) || !lyt.is_wire(node) || lyt.fanout_size(node) != 1 || old_coord.z != 0)
            {
                obstruct_coordinate(0, 0);
            }

            // handle single input gates and wires
            if (const auto signals = lyt.incoming_data_flow(old_coord); signals.size() == 1)
            {
                const auto      incoming_signal = signals[0];
                const tile<Lyt> shifted_tile{incoming_signal.x + x_offset, incoming_signal.y + y_offset,
                                             incoming_signal.z};

                // obstruct the connection between the gate and its incoming signal
                wiring_reduction_lyt.search_obstructions.obstruct_connection(shifted_tile, new_coord);

                // obstruct horizontal/vertical wires, non-wire gates (inv) and fanouts
                if (!lyt.is_wire(node) || lyt.is_gate(node) || (lyt.fanout_size(node) != 1) || (old_coord.z != 0) ||
                    (lyt.has_western_incoming_signal({old_coord}) && lyt.has_eastern_outgoing_signal({old_coord}) &&
                     (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)) ||
                    (lyt.has_northern_incoming_signal({old_coord}) && lyt.has_southern_outgoing_signal({old_coord}) &&
                     (wiring_reduction_lyt.get_search_direction() == search_direction::VERTICAL)))
                {
                    obstruct_coordinate(0, 0);
                }

                // for bent wires from north to east, obstruct the connection between the wire and the
                // coordinate to the bottom right/ top left
                else if (lyt.has_northern_incoming_signal({old_coord}) && lyt.has_eastern_outgoing_signal({old_coord}))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        {
                            wiring_reduction_lyt.search_obstructions.obstruct_connection(
                                new_coord, {new_coord.x + 1, new_coord.y + 1, new_coord.z});

                            // special cases:
                            // →=
                            //  ↓
                            // ...
                            //  ↓
                            //  =→
                            for (int32_t i = 1; is_single_wire(0, i); ++i)
                            {
                                if (lyt.has_western_incoming_signal({old_coord.x, old_coord.y - i, old_coord.z}))
                                {
                                    obstruct_coordinate(0, i);
                                    break;
                                }
                            }
                        }
                    }

                    else
                    {
                        wiring_reduction_lyt.search_obstructions.obstruct_connection(
                            {new_coord.x - 1, new_coord.y - 1, new_coord.z}, new_coord);
                    }
                }

                // for bent wires from west to south, obstruct the connection between the wire and the
                // coordinate to the top left/ bottom right
                else if (lyt.has_western_incoming_signal({old_coord}) && lyt.has_southern_outgoing_signal({old_coord}))
                {
                    if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                    {
                        wiring_reduction_lyt.search_obstructions.obstruct_connection(
                            {new_coord.x - 1, new_coord.y - 1, new_coord.z}, new_coord);
                    }
                    else
                    {
                        wiring_reduction_lyt.search_obstructions.obstruct_connection(
                            new_coord, {new_coord.x + 1, new_coord.y + 1, new_coord.z});

                        // special cases:
                        // ↓
                        // =→...→=
                        //       ↓
                        for (int32_t i = 1; is_single_wire(i, 0); ++i)
                        {
                            if (lyt.has_northern_incoming_signal({old_coord.x - i, old_coord.y, old_coord.z}))
                            {
                                obstruct_coordinate(i, 0);
                                break;
                            }
                        }
                    }
                }
            }

            // handle double input gates (AND, OR, ...)
            else if (signals.size() == 2)
            {
                const auto signal_a = signals[0];
                const auto signal_b = signals[1];

                const auto shifted_tile_a = tile<Lyt>{signal_a.x + x_offset, signal_a.y + y_offset, signal_a.z};
                const auto shifted_tile_b = tile<Lyt>{signal_b.x + x_offset, signal_b.y + y_offset, signal_b.z};

                wiring_reduction_lyt.search_obstructions.obstruct_connection(shifted_tile_a, new_coord);
                wiring_reduction_lyt.search_obstructions.obstruct_connection(shifted_tile_b, new_coord);

                obstruct_coordinate(0, 0);
            }

            if (const auto signals = lyt.incoming_data_flow(old_coord); (old_coord.z == 1) || (signals.size() == 2))
            {
                // special cases (where the crossing can also be placed further to the left or top):
                // +→=
                // ↓ ↓
                // =→&
                //
                // or:
                //
                // +→=
                // ↓ ↓
                // =→+
                if (is_single_wire(1, 0) && is_single_wire(0, 1))
                {
                    bool obstruct = false;

                    for (int32_t i = 1; true; ++i)
                    {
                        if (is_crossing(1, 1))
                        {
                            obstruct = true;
                            break;
                        }
                        if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                        {
                            if (!is_single_wire(1, i) || !is_single_wire(0, i + 1) ||
                                !lyt.has_northern_incoming_signal({old_coord.x - 1, old_coord.y - i + 1, 0}) ||
                                !lyt.has_northern_incoming_signal({old_coord.x, old_coord.y - i, 0}))
                            {
                                break;
                            }
                            if (is_crossing(1, i + 1))
                            {
                                obstruct = true;
                                break;
                            }
                        }
                        else
                        {
                            if (!is_single_wire(i, 1) || !is_single_wire(i + 1, 0) ||
                                !lyt.has_western_incoming_signal({old_coord.x - i, old_coord.y, 0}) ||
                                !lyt.has_western_incoming_signal({old_coord.x - i + 1, old_coord.y - i, 0}))
                            {
                                break;
                            }
                            if (is_crossing(i + 1, 1))
                            {
                                obstruct = true;
                                break;
                            }
                        }
                    }

                    if (obstruct)
                    {
                        if (wiring_reduction_lyt.get_search_direction() == search_direction::HORIZONTAL)
                        {
                            obstruct_coordinate(1, 0);
                        }
                        else
                        {
                            obstruct_coordinate(0, 1);
                        }
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
void add_obstructions(WiringReductionLyt& lyt)
{
    if (lyt.get_search_direction() == search_direction::HORIZONTAL)
    {
        // add obstructions to the top edge of the layout
        for (int32_t x = 1; x <= (static_cast<int32_t>(lyt.width()) - 1); x++)
        {
            lyt.search_obstructions.obstruct_coordinate({x, 0, 0});
            lyt.search_obstructions.obstruct_coordinate({x, 0, 1});
        }

        // add obstructions to the bottom edge of the layout
        for (int32_t x = 0; x < (static_cast<int32_t>(lyt.width()) - 1); x++)
        {
            lyt.search_obstructions.obstruct_coordinate({x, (static_cast<int32_t>(lyt.height()) - 1), 0});
            lyt.search_obstructions.obstruct_coordinate({x, (static_cast<int32_t>(lyt.height()) - 1), 1});
        }
    }
    else
    {
        // add obstructions to the left edge of the layout
        for (int32_t y = 1; y <= (static_cast<int32_t>(lyt.height()) - 1); y++)
        {
            lyt.search_obstructions.obstruct_coordinate({0, y, 0});
            lyt.search_obstructions.obstruct_coordinate({0, y, 1});
        }

        // add obstructions to the right edge of the layout
        for (int32_t y = 0; y < (static_cast<int32_t>(lyt.height()) - 1); y++)
        {
            lyt.search_obstructions.obstruct_coordinate({(static_cast<int32_t>(lyt.width()) - 1), y, 0});
            lyt.search_obstructions.obstruct_coordinate({(static_cast<int32_t>(lyt.width()) - 1), y, 1});
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
                                                                  const coordinate<WiringReductionLyt>& end)
{
    using dist = physical_design::path_finding::manhattan_distance_functor<WiringReductionLyt, uint64_t>;
    using cost = physical_design::path_finding::unit_cost_functor<WiringReductionLyt, uint8_t>;

    static const physical_design::path_finding::a_star_params params{false};

    return physical_design::path_finding::a_star<layout_coordinate_path<WiringReductionLyt>>(
        lyt, {start, end}, dist(), cost(), params, lyt.search_obstructions);
}
/**
 * Update the to-delete list based on a possible path in a wiring_reduction_layout.
 *
 * This function updates the to-delete list by appending coordinates from the given possible path
 * in a wiring_reduction_layout. It considers coordinates that are not at the leftmost (`x == 0`) or rightmost (`x ==
 * (static_cast<int32_t>(lyt.width()) - 1)`) positions for left to right, or at the top (`y == 0`) or bottom (`y ==
 * (static_cast<int32_t>(lyt.height()) - 1)`) positions for top to bottom and shifts them to get the corresponding
 * coordinates on the original layout. The coordinates are then obstructed in both layers (0 and 1).
 *
 * @tparam WiringReductionLyt Type of the `wiring_reduction_layout`.
 * @param lyt The `wiring_reduction_layout` to be updated.
 * @param possible_path The path of coordinates to be considered for updating the to-delete list.
 * @param to_delete Reference to the to-delete list to be updated with new coordinates.
 */
template <typename WiringReductionLyt>
void update_to_delete_list(WiringReductionLyt& lyt, const layout_coordinate_path<WiringReductionLyt>& possible_path,
                           layout_coordinate_path<WiringReductionLyt>& to_delete)
{
    for (const auto& coord : possible_path)
    {
        // check if the coordinate is not at the leftmost or rightmost position
        if (((lyt.get_search_direction() == search_direction::HORIZONTAL) && coord.x != 0 &&
             coord.x != (static_cast<int32_t>(lyt.width()) - 1)) ||
            ((lyt.get_search_direction() == search_direction::VERTICAL) && coord.y != 0 &&
             coord.y != (static_cast<int32_t>(lyt.height()) - 1)))
        {
            // create the corresponding coordinate on the original layout
            const fiction::coordinate<WiringReductionLyt> shifted_coord{coord.x - 1, coord.y - 1, 0};

            // append the corresponding coordinate to the to-delete list
            to_delete.append(shifted_coord);

            // obstruct the coordinate in both layers
            lyt.search_obstructions.obstruct_coordinate({coord.x, coord.y, 0});
            lyt.search_obstructions.obstruct_coordinate({coord.x, coord.y, 1});
        }
    }
}
/**
 * Offset matrix type alias.
 */
using offset_matrix = std::vector<std::vector<int32_t>>;
/**
 * Accesses the entry of an offset matrix at a tile position.
 *
 * @tparam Matrix Offset matrix type, possibly `const`.
 * @param matrix Offset matrix.
 * @param y Row index, i.e., the y-coordinate.
 * @param x Column index, i.e., the x-coordinate.
 * @return The entry at row `y` and column `x`.
 */
template <typename Matrix>
[[nodiscard]] auto& offset_at(Matrix& matrix, const int32_t y, const int32_t x) noexcept
{
    return matrix[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}
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
template <typename WiringReductionLyt>
[[nodiscard]] offset_matrix calculate_offset_matrix(const WiringReductionLyt&                         lyt,
                                                    const layout_coordinate_path<WiringReductionLyt>& to_delete)
{
    // initialize matrix with zeros
    offset_matrix matrix(
        static_cast<std::size_t>((static_cast<int32_t>(lyt.height()) - 1)) + 1,
        std::vector<int32_t>(static_cast<std::size_t>((static_cast<int32_t>(lyt.width()) - 1)) + 1, 0));

    // update matrix based on coordinates
    for (const auto& coord : to_delete)
    {
        const auto x = coord.x;
        const auto y = coord.y;

        if (lyt.get_search_direction() == search_direction::HORIZONTAL)
        {
            for (int32_t i = (static_cast<int32_t>(lyt.height()) - 1); i > y; --i)
            {
                offset_at(matrix, i, x) += 1;
            }
        }
        else
        {
            for (int32_t i = (static_cast<int32_t>(lyt.width()) - 1); i > x; --i)
            {
                offset_at(matrix, y, i) += 1;
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
[[nodiscard]] tile<Lyt> determine_new_coord(const WiringReductionLyt& wiring_reduction_lyt, const int32_t x,
                                            const int32_t y, const int32_t z, const int32_t offset)
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
 * @brief Removes selected wires, bypasses their declared inputs, and shifts surviving object identities.
 *
 * The copy is committed after all reconnections and moves succeed. Logical input indices and disconnected slots
 * remain unchanged. Temporary negative coordinates prevent occupied-target conflicts during bulk movement.
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam WiringReductionLyt Wiring-reduction search layout type.
 * @param lyt Layout to edit.
 * @param wiring_reduction_layout Search layout containing the cut direction.
 * @param to_delete Coordinates selected by the cut search.
 * @throws std::invalid_argument If a cut selects a retained object or a cyclic wire chain.
 */
template <typename Lyt, typename WiringReductionLyt>
void delete_wires(Lyt& lyt, const WiringReductionLyt& wiring_reduction_layout,
                  const layout_coordinate_path<WiringReductionLyt>& to_delete)
{
    const auto                                  offsets = calculate_offset_matrix(wiring_reduction_layout, to_delete);
    std::unordered_set<typename Lyt::object_id> removed{};
    for (const auto& t : to_delete)
    {
        if (const auto id = lyt.find_object(t))
        {
            if (!lyt.is_wire(*id) || lyt.is_gate(*id) || lyt.is_pi(*id) || lyt.is_po(*id) || lyt.is_fanout(*id))
            {
                throw std::invalid_argument("A wiring cut selects a retained object");
            }
            removed.insert(*id);
        }
    }
    auto reduced = lyt;
    lyt.foreach_node(
        [&](const auto id)
        {
            if (removed.contains(id))
            {
                return;
            }
            lyt.foreach_fanin(id,
                              [&](const auto source, const auto input)
                              {
                                  auto upstream = std::optional<typename Lyt::output_port>{source};
                                  std::unordered_set<typename Lyt::object_id> visited{};
                                  while (upstream && removed.contains(upstream->object))
                                  {
                                      if (!visited.insert(upstream->object).second)
                                      {
                                          throw std::invalid_argument("A wiring cut contains a cyclic wire chain");
                                      }
                                      upstream = lyt.source({upstream->object, 0});
                                  }
                                  if (upstream)
                                  {
                                      reduced.connect(*upstream, {id, input});
                                  }
                                  else
                                  {
                                      reduced.disconnect({id, input});
                                  }
                              });
        });
    for (const auto id : removed)
    {
        reduced.remove(id);
    }
    std::vector<std::pair<typename Lyt::object_id, tile<Lyt>>> placements{};
    placements.reserve(reduced.size());
    lyt.foreach_node(
        [&](const auto id)
        {
            if (!removed.contains(id))
            {
                const auto old    = lyt.get_tile(id);
                const auto offset = offset_at(offsets, old.y, old.x);
                placements.emplace_back(id,
                                        determine_new_coord<Lyt>(wiring_reduction_layout, old.x, old.y, old.z, offset));
            }
        });
    for (std::size_t i{}; i < placements.size(); ++i)
    {
        reduced.move_node(placements[i].first, {-1, static_cast<int64_t>(i), 0});
    }
    for (const auto& [id, position] : placements)
    {
        reduced.move_node(id, position);
    }
    const auto maximum = layouts::bounding_box_2d{reduced}.get_max();
    reduced.resize(maximum ? typename Lyt::extent{static_cast<int64_t>(maximum->x) + 1,
                                                  static_cast<int64_t>(maximum->y) + 1, reduced.layers()} :
                             typename Lyt::extent{});
    lyt = std::move(reduced);
}
/** @brief Searches horizontal and vertical wire cuts and edits the caller's layout.
 * @tparam Lyt Cartesian gate-level layout type.
 */

template <typename Lyt>
class wiring_reduction_impl
{
  public:
    /** @brief Initializes a wiring-cut search. @param lyt Layout. @param p Parameters. @param st Statistics. */
    wiring_reduction_impl(Lyt& lyt, wiring_reduction_params p, wiring_reduction_stats& st) :
            plyt{lyt},
            ps{std::move(p)},
            pst{st},
            start{std::chrono::high_resolution_clock::now()}
    {}

    /** @brief Runs wire-cut searches until convergence or timeout. */
    void run()
    {
        static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
        static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

        // measure run time
        mockturtle::stopwatch stop{pst.time_total};

        // record initial layout statistics
        pst.num_wires_before = plyt.num_wires() - plyt.num_pis() - plyt.num_pos();
        pst.x_size_before    = plyt.width();
        pst.y_size_before    = plyt.height();

        // edit the caller-owned layout
        auto& layout = plyt;

        // initialize the list of wires to delete
        layout_coordinate_path<wiring_reduction_layout> to_delete = {};

        bool found_wires = !layout.is_empty();

        // lambda to update the timeout status and calculate remaining time
        const auto update_timeout = [start_time = this->start, &params = this->ps,
                                     &timeout_limit_is_reached = this->timeout_limit_reached]() noexcept -> void
        {
            const auto current_time = std::chrono::high_resolution_clock::now();
            const auto elapsed_ms   = static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count());
            timeout_limit_is_reached = (elapsed_ms >= params.timeout);
        };

        // the number of paths to process is not known in advance, so the total stays unknown
        utils::progress_reporter progress{ps.on_progress, "wire paths"};

        // perform wiring reduction iteratively until no further wires can be deleted
        while (found_wires && !timeout_limit_reached)
        {
            found_wires = false;

            for (const auto direction : {search_direction::HORIZONTAL, search_direction::VERTICAL})
            {
                // update the remaining timeout
                update_timeout();

                if (timeout_limit_reached)
                {
                    break;
                }

                // create wiring reduction layout for the current direction
                auto wiring_reduction_lyt = create_wiring_reduction_layout<Lyt>(layout, 1, 1, direction);
                add_obstructions(wiring_reduction_lyt);

                // update the remaining timeout
                update_timeout();

                if (timeout_limit_reached)
                {
                    break;
                }

                // reset the list of wires to delete
                to_delete.clear();

                // get the initial possible path for wire deletion
                auto possible_path = get_path(wiring_reduction_lyt, {0, 0},
                                              {(static_cast<int32_t>(wiring_reduction_lyt.width()) - 1),
                                               (static_cast<int32_t>(wiring_reduction_lyt.height()) - 1)});

                // iterate while there is a possible path and timeout not reached
                while (!possible_path.empty() && !timeout_limit_reached)
                {
                    // update the list of wires to delete based on the current path
                    update_to_delete_list(wiring_reduction_lyt, possible_path, to_delete);

                    progress.advance();

                    // update the remaining timeout after processing the path
                    update_timeout();

                    if (!timeout_limit_reached)
                    {
                        // get the next possible path for wire deletion
                        possible_path = get_path(wiring_reduction_lyt, {0, 0},
                                                 {(static_cast<int32_t>(wiring_reduction_lyt.width()) - 1),
                                                  (static_cast<int32_t>(wiring_reduction_lyt.height()) - 1)});
                    }
                }

                if (!to_delete.empty())
                {
                    // delete the identified wires from the layout
                    delete_wires(layout, wiring_reduction_lyt, to_delete);
                    found_wires = true;
                }
            }
        }

        // calculate the final bounding box and resize the layout accordingly
        const auto bounding_box = layouts::bounding_box_2d(layout);
        const auto maximum      = bounding_box.get_max();
        layout.resize(maximum ? typename Lyt::extent{static_cast<int64_t>(maximum->x) + 1,
                                                     static_cast<int64_t>(maximum->y) + 1, layout.layers()} :
                                typename Lyt::extent{});

        // update final layout statistics
        pst.x_size_after = layout.width();
        pst.y_size_after = layout.height();

        const uint64_t area_before = pst.x_size_before * pst.y_size_before;
        const uint64_t area_after  = pst.x_size_after * pst.y_size_after;

        double area_percentage_difference = area_before == 0 ?
                                                0.0 :
                                                (static_cast<double>(area_before) - static_cast<double>(area_after)) /
                                                    static_cast<double>(area_before) * 100.0;

        // round the area improvement to two decimal places
        area_percentage_difference = std::round(area_percentage_difference * 100.0) / 100.0;

        pst.area_improvement = area_percentage_difference;
        pst.num_wires_after  = layout.num_wires() - layout.num_pis() - layout.num_pos();

        double wiring_percentage_difference =
            pst.num_wires_before == 0 ?
                0.0 :
                (static_cast<double>(pst.num_wires_before) - static_cast<double>(pst.num_wires_after)) /
                    static_cast<double>(pst.num_wires_before) * 100.0;

        // round the wiring improvement to two decimal places
        wiring_percentage_difference = std::round(wiring_percentage_difference * 100.0) / 100.0;
        pst.wiring_improvement       = wiring_percentage_difference;
    }

  private:
    /**
     * The 2DDWave-clocked layout whose wiring is to be reduced.
     */
    Lyt& plyt;
    /**
     * Wiring reduction parameters.
     */
    wiring_reduction_params ps;
    /**
     * Statistics about the wiring_reduction process.
     */
    wiring_reduction_stats& pst;
    /**
     * Timeout limit reached.
     */
    bool timeout_limit_reached = false;
    /**
     * Start time.
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
}  // namespace detail

/**
 * A scalable wiring reduction algorithm for 2DDWave-clocked layouts based on A* path finding as originally proposed in
 * \"Late Breaking Results: Wiring Reduction for Field-coupled Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille
 * in DAC 2024 (https://dl.acm.org/doi/10.1145/3649329.3663491) and extended in \"Efficient and Scalable Post-Layout
 * Optimization for Field-coupled Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
 * (https://ieeexplore.ieee.org/document/10916761).
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
 * @param ps Parameters.
 * @param pst Statistics.
 * @throws std::invalid_argument If clocking, occupied geometry, or interface placement is invalid.
 * @throws std::overflow_error If dimensions leave no room for signed routing coordinates.
 */
template <typename Lyt>
void wiring_reduction(Lyt& lyt, wiring_reduction_params ps = {}, wiring_reduction_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (!lyt.is_clocking_scheme(layouts::clocking::TWODDWAVE_NAME))
    {
        throw std::invalid_argument("Wiring reduction requires 2DDWave clocking");
    }
    if (lyt.width() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max() - 1) ||
        lyt.height() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max() - 1) ||
        lyt.layers() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    {
        throw std::overflow_error("Layout dimensions leave no room for signed routing coordinates");
    }
    lyt.foreach_node(
        [&](const auto id)
        {
            if (!lyt.is_within_bounds(lyt.get_tile(id)))
            {
                throw std::invalid_argument("Wiring reduction requires objects inside the geometry");
            }
        });
    lyt.foreach_pi(
        [&](const auto id)
        {
            const auto t = lyt.get_tile(id);
            if (!lyt.is_at_northern_border(t) && !lyt.is_at_western_border(t))
            {
                throw std::invalid_argument("Primary inputs must lie on the northern or western border");
            }
        });
    lyt.foreach_po(
        [&](const auto id)
        {
            const auto t = lyt.get_tile(id);
            if (!lyt.is_at_eastern_border(t) && !lyt.is_at_southern_border(t))
            {
                throw std::invalid_argument("Primary outputs must lie on the eastern or southern border");
            }
        });

    // initialize stats for runtime measurement
    wiring_reduction_stats             st{};
    detail::wiring_reduction_impl<Lyt> p{lyt, ps, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction::physical_design
