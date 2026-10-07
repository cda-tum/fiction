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
 * @brief Shrinks a placed and routed gate-level layout by relocating its gates.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstructions.hpp"
#include "fiction/physical_design/path_finding/a_star.hpp"
#include "fiction/physical_design/path_finding/cost.hpp"
#include "fiction/physical_design/path_finding/distance.hpp"
#include "fiction/physical_design/routing_utils.hpp"
#include "fiction/physical_design/wiring_reduction.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/progress.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
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
 * Parameters for the post-layout optimization algorithm.
 */
struct post_layout_optimization_params
{
    /**
     * Maximum number of relocations to try for each gate. Defaults to the number of tiles in the given layout if not
     * specified.
     */
    std::optional<uint64_t> max_gate_relocations = std::nullopt;
    /**
     * Only optimize PO positions.
     */
    bool optimize_pos_only = false;
    /**
     * Disable the creation of crossings during optimization. If set to true, gates will only be relocated if a
     * crossing-free wiring is found. Defaults to false.
     */
    bool planar_optimization = false;
    /**
     * Timeout limit (in ms). Specifies the maximum allowed time in milliseconds for the optimization process. For large
     * layouts, the actual execution time may slightly exceed this limit because it's impractical to check the timeout
     * at every algorithm step and the functional correctness has to be ensured by completing essential algorithm steps.
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
    /**
     * Callback that receives the progress of the gate relocations and of the nested wiring reduction.
     */
    utils::progress_callback on_progress{};
};

/**
 * This struct stores statistics about the post-layout optimization process.
 */
struct post_layout_optimization_stats
{
    /**
     * Runtime of the post-layout optimization process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Layout width before the post-layout optimization process.
     */
    uint64_t x_size_before{0ull};
    /**
     * Layout height before the post-layout optimization process.
     */
    uint64_t y_size_before{0ull};
    /**
     * Layout width after the post-layout optimization process.
     */
    uint64_t x_size_after{0ull};
    /**
     * Layout height after the post-layout optimization process.
     */
    uint64_t y_size_after{0ull};
    /**
     * Area reduction (in %) after the post-layout optimization process.
     */
    double_t area_improvement{0ull};
    /**
     * Number of wire segments before the post-layout optimization process.
     */
    uint64_t num_wires_before{0ull};
    /**
     * Number of wire segments after the post-layout optimization process.
     */
    uint64_t num_wires_after{0ull};
    /**
     * Number of crossings before the post-layout optimization process.
     */
    uint64_t num_crossings_before{0ull};
    /**
     * Number of crossings after the post-layout optimization process.
     */
    uint64_t num_crossings_after{0ull};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time                          = {:.2f} secs\n",
                           mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size before optimization     = {} × {}\n", x_size_before, y_size_before);
        out << fmt::format("[i] layout size after optimization      = {} × {}\n", x_size_after, y_size_after);
        out << fmt::format("[i] area reduction                      = {}%\n", area_improvement);
        out << fmt::format("[i] num. wires before optimization      = {}\n", num_wires_before);
        out << fmt::format("[i] num. wires after optimization       = {}\n", num_wires_after);
        out << fmt::format("[i] num. crossings before optimization  = {}\n", num_crossings_before);
        out << fmt::format("[i] num. crossings after optimization   = {}\n", num_crossings_after);
    }
};

namespace detail
{

/** @brief Routes adjacent to a gate, with the logical destination of each route.
 * @tparam Lyt Gate-level layout type.
 */
template <typename Lyt>
struct fanin_fanout_data
{
    /** @brief Retained source coordinates in logical input order. */
    std::vector<tile<Lyt>> fanins{};
    /** @brief Retained destination coordinates. */
    std::vector<tile<Lyt>> fanouts{};
    /** @brief Intermediate wire coordinates to remove. */
    std::vector<tile<Lyt>> to_clear{};
    /** @brief Original routes, inputs first and then outputs. */
    std::vector<layout_coordinate_path<Lyt>> routes{};
    /** @brief Explicit input endpoints corresponding to routes. */
    std::vector<typename Lyt::input_port> destinations{};
};
/** @brief Fits zero-origin geometry to occupied objects. Empty layouts receive empty geometry.
 * @tparam Lyt Layout type. @param lyt Layout to resize.
 */
template <typename Lyt>
void fit_occupied_geometry(Lyt& lyt)
{
    const auto maximum = layouts::bounding_box_2d{lyt}.get_max();
    lyt.resize(maximum ? typename Lyt::extent{static_cast<int64_t>(maximum->x) + 1,
                                              static_cast<int64_t>(maximum->y) + 1, lyt.layers()} :
                         typename Lyt::extent{});
}
/** @brief Moves an output and inserts a wire at its former coordinate, preserving its identity.
 * @tparam Lyt Layout type. @param lyt Layout. @param id Output identity. @param target New coordinate.
 */
template <typename Lyt>
void extend_output_position(Lyt& lyt, const typename Lyt::object_id id, const tile<Lyt>& target)
{
    const auto old    = lyt.get_tile(id);
    const auto source = lyt.source({id, 0});
    lyt.move_node(id, target);
    const auto wire = source ? lyt.create_buf(*source, old) : lyt.create_buf(old);
    lyt.connect(wire, {id, 0});
}
/** @brief Shrinks empty final rows and columns while keeping outputs accessible at the border.
 * @tparam Lyt Cartesian layout type. @param lyt Layout to optimize.
 */
template <typename Lyt>
void optimize_output_positions(Lyt& lyt)
{
    if (lyt.is_empty())
    {
        fit_occupied_geometry(lyt);
        return;
    }
    auto x_max       = static_cast<int32_t>(lyt.width()) - 1;
    auto y_max       = static_cast<int32_t>(lyt.height()) - 1;
    bool optimizable = y_max > 0;
    for (int32_t x = 0; x <= x_max; ++x)
    {
        if (!(lyt.is_empty_tile({x, y_max}) ||
              (lyt.is_po_tile({x, y_max}) && x < x_max && lyt.is_empty_tile({x + 1, y_max - 1}))))
        {
            optimizable = false;
            break;
        }
    }
    if (optimizable)
    {
        for (int32_t x = 0; x < x_max; ++x)
        {
            if (const auto id = lyt.find_object({x, y_max}); id && lyt.is_po(*id))
            {
                lyt.move_node(*id, {x + 1, y_max - 1});
            }
        }
    }
    optimizable = x_max > 0;
    for (int32_t y = 0; y <= y_max; ++y)
    {
        if (!(lyt.is_empty_tile({x_max, y}) ||
              (lyt.is_po_tile({x_max, y}) && y < y_max && lyt.is_empty_tile({x_max - 1, y + 1}))))
        {
            optimizable = false;
            break;
        }
    }
    if (optimizable)
    {
        for (int32_t y = 0; y < y_max; ++y)
        {
            if (const auto id = lyt.find_object({x_max, y}); id && lyt.is_po(*id))
            {
                lyt.move_node(*id, {x_max - 1, y + 1});
            }
        }
    }
    fit_occupied_geometry(lyt);
    x_max = static_cast<int32_t>(lyt.width()) - 1;
    y_max = static_cast<int32_t>(lyt.height()) - 1;
    for (int32_t x = 0; x < x_max && y_max > 0; ++x)
    {
        if (const auto id = lyt.find_object({x, y_max - 1}); id && lyt.is_po(*id) && lyt.is_empty_tile({x, y_max}))
        {
            extend_output_position(lyt, *id, {x, y_max});
        }
    }
    for (int32_t y = 0; y < y_max && x_max > 0; ++y)
    {
        if (const auto id = lyt.find_object({x_max - 1, y}); id && lyt.is_po(*id) && lyt.is_empty_tile({x_max, y}))
        {
            extend_output_position(lyt, *id, {x_max, y});
        }
    }
    fit_occupied_geometry(lyt);
    x_max = static_cast<int32_t>(lyt.width()) - 1;
    y_max = static_cast<int32_t>(lyt.height()) - 1;
    if (lyt.num_pos() == 1 && x_max > 0 && y_max > 0)
    {
        const auto id = lyt.find_object({x_max, y_max});
        if (id && lyt.is_po(*id))
        {
            if (lyt.has_western_incoming_signal({x_max, y_max}) && x_max <= y_max &&
                lyt.is_empty_tile({x_max - 1, y_max + 1}))
            {
                lyt.move_node(*id, {x_max - 1, y_max + 1});
                fit_occupied_geometry(lyt);
            }
            else if (lyt.has_northern_incoming_signal({x_max, y_max}) && y_max <= x_max &&
                     lyt.is_empty_tile({x_max + 1, y_max - 1}))
            {
                lyt.move_node(*id, {x_max + 1, y_max - 1});
                fit_occupied_geometry(lyt);
            }
        }
    }
}
/** @brief Extends outputs one tile to the nearest border while preserving their logical inputs.
 * @tparam Lyt Layout type. @param lyt Layout. @param moved_gates Relocation count to adjust.
 */
template <typename Lyt>
void check_and_optimize_po_positions(Lyt& lyt, uint64_t& moved_gates)
{
    const auto x_max = static_cast<int32_t>(lyt.width()) - 1;
    const auto y_max = static_cast<int32_t>(lyt.height()) - 1;
    lyt.foreach_po(
        [&](const auto id)
        {
            const auto t = lyt.get_tile(id);
            if (lyt.is_at_eastern_border(t) || lyt.is_at_southern_border(t))
            {
                return;
            }
            if (t.x == x_max - 1 && lyt.is_empty_tile({x_max, t.y}))
            {
                extend_output_position(lyt, id, {x_max, t.y});
                if (moved_gates)
                {
                    --moved_gates;
                }
            }
            else if (t.y == y_max - 1 && lyt.is_empty_tile({t.x, y_max}))
            {
                extend_output_position(lyt, id, {t.x, y_max});
                if (moved_gates)
                {
                    --moved_gates;
                }
            }
        });
    fit_occupied_geometry(lyt);
}
/**
 * Custom comparison function for sorting tiles based on the sum of their coordinates that breaks ties based on the
 * x-coordinate.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param a First tile to compare.
 * @param b Second tile to compare.
 * @return `true` iff `a < b` based on the aforementioned rule.
 */
template <typename Lyt>
bool compare_gate_tiles(const tile<Lyt>& a, const tile<Lyt>& b)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    return static_cast<bool>(std::make_pair(static_cast<int64_t>(a.x) + a.y, a.x) <
                             std::make_pair(static_cast<int64_t>(b.x) + b.y, b.x));
}

/** @brief Relocates retained objects and reroutes their explicit logical input endpoints.
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
class post_layout_optimization_impl
{
  public:
    /** @brief Initializes relocation search. @param lyt Layout. @param p Parameters. @param st Statistics. */
    post_layout_optimization_impl(Lyt& lyt, post_layout_optimization_params p, post_layout_optimization_stats& st) :
            plyt{lyt},
            ps{std::move(p)},
            pst{st},
            start{std::chrono::high_resolution_clock::now()}
    {
        wiring_reduction_params.on_progress = ps.on_progress;
    }

    /** @brief Optimizes placement and wiring until convergence or timeout. */
    void run()
    {
        static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
        static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

        // start the stopwatch to measure total optimization time
        const mockturtle::stopwatch stop{pst.time_total};

        // record initial layout statistics
        pst.x_size_before        = plyt.width();
        pst.y_size_before        = plyt.height();
        pst.num_wires_before     = plyt.num_wires() - plyt.num_pis() - plyt.num_pos();
        pst.num_crossings_before = plyt.num_crossings();

        // determine the maximum number of gate relocations
        max_gate_relocations = ps.max_gate_relocations.value_or(plyt.area());

        // edit the caller-owned layout
        auto& layout = plyt;

        // the number of gate tiles is only known per pass; the reporter is reset for each of them
        utils::progress_reporter progress{ps.on_progress, "gate relocations"};

        // initialize flags to control the optimization loop
        bool moved_at_least_one_gate = true;
        bool reduced_wiring          = true;
        timeout_limit_reached        = false;

        if (max_gate_relocations == 0)
        {
            // update the remaining timeout
            wiring_reduction_params.timeout = update_timeout();

            if (!timeout_limit_reached)
            {
                fiction::physical_design::wiring_reduction(layout, wiring_reduction_params, &wiring_reduction_stats);
            }
        }
        else
        {
            // iteratively optimize the layout until no more improvements can be made or timeout is reached
            while ((moved_at_least_one_gate || reduced_wiring) && !timeout_limit_reached)
            {
                reduced_wiring = false;

                // update the remaining timeout
                wiring_reduction_params.timeout = update_timeout();

                if (moved_at_least_one_gate && !ps.optimize_pos_only && !timeout_limit_reached)
                {
                    fiction::physical_design::wiring_reduction(layout, wiring_reduction_params,
                                                               &wiring_reduction_stats);

                    // check if wiring reduction made any improvements
                    if (wiring_reduction_stats.area_improvement != 0ull ||
                        wiring_reduction_stats.wiring_improvement != 0ull)
                    {
                        reduced_wiring = true;
                    }
                }

                // gather all relevant gate tiles for relocation
                std::vector<tile<Lyt>> gate_tiles{};
                gate_tiles.reserve(layout.num_gates() + layout.num_pis() + layout.num_pos());
                layout.foreach_node(
                    [this, &layout, &gate_tiles](const auto& node)
                    {
                        if (const tile<Lyt> gate_tile = layout.get_tile(node);
                            layout.is_gate(node) || layout.is_fanout(node) || layout.is_pi_tile(gate_tile) ||
                            layout.is_po_tile(gate_tile))
                        {
                            search_obstructions.obstruct_coordinate({gate_tile.x, gate_tile.y, 1});
                            gate_tiles.emplace_back(gate_tile);
                        }
                    });

                // sort the gate tiles using the custom comparator
                std::ranges::sort(gate_tiles, compare_gate_tiles<Lyt>);

                progress.reset(gate_tiles.size());

                max_non_po = tile<Lyt>{0, 0};
                // determine minimal border for POs
                for (const auto& gate_tile : gate_tiles)
                {
                    if (!layout.is_po_tile(gate_tile))
                    {
                        max_non_po.x = std::max(max_non_po.x, gate_tile.x);
                        max_non_po.y = std::max(max_non_po.y, gate_tile.y);
                    }
                }

                // reset the gate movement flag
                moved_at_least_one_gate = false;
                uint64_t moved_gates    = 0;

                // attempt to relocate each gate tile
                for (const auto& gate_tile : gate_tiles)
                {
                    if (!timeout_limit_reached)
                    {
                        if (!ps.optimize_pos_only || (ps.optimize_pos_only && layout.is_po_tile(gate_tile)))
                        {
                            if (improve_gate_location(layout, gate_tile))
                            {
                                ++moved_gates;
                            }
                        }

                        // update the remaining timeout after each relocation attempt
                        update_timeout();
                    }

                    progress.advance();
                }

                // resize the layout to fit within the new bounding box after relocations
                fit_occupied_geometry(layout);

                // check and optimize PO positions after each full gate relocation iteration
                check_and_optimize_po_positions(layout, moved_gates);

                if (moved_gates > 0)
                {
                    moved_at_least_one_gate = true;
                }
            }
        }

        // if the optimization did not time out, optimize the output positions
        if (!timeout_limit_reached)
        {
            optimize_output_positions(layout);
        }

        // final bounding box calculation and layout resizing
        fit_occupied_geometry(layout);

        // update final layout statistics
        pst.x_size_after = layout.width();
        pst.y_size_after = layout.height();

        const uint64_t area_before = pst.x_size_before * pst.y_size_before;
        const uint64_t area_after  = pst.x_size_after * pst.y_size_after;

        double area_percentage_difference = area_before == 0 ?
                                                0.0 :
                                                (static_cast<double>(area_before) - static_cast<double>(area_after)) /
                                                    static_cast<double>(area_before) * 100.0;
        pst.area_improvement              = std::round(area_percentage_difference * 100) / 100.0;

        pst.num_wires_after     = plyt.num_wires() - plyt.num_pis() - plyt.num_pos();
        pst.num_crossings_after = plyt.num_crossings();
    }

  private:
    /** @brief Temporary constraints used while moving gates and routing wires. */
    layouts::obstructions search_obstructions{};

    /**
     * 2DDWave-clocked Cartesian gate-level layout to optimize.
     */
    Lyt& plyt;
    /**
     * Post-layout optimization parameters.
     */
    post_layout_optimization_params ps;
    /**
     * Statistics about the post-layout optimization process.
     */
    post_layout_optimization_stats& pst;
    /**
     * Start time.
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    /**
     * Maximum number of relocations to try for each gate.
     */
    uint64_t max_gate_relocations{};
    /**
     * Maximum coordinate of all gates that are not POs.
     */
    tile<Lyt> max_non_po{0, 0};
    /**
     * Timeout limit reached.
     */
    bool timeout_limit_reached = false;
    /**
     * Wiring reduction parameters.
     */
    fiction::physical_design::wiring_reduction_params wiring_reduction_params{};
    /**
     * Wiring reduction stats.
     */
    fiction::physical_design::wiring_reduction_stats wiring_reduction_stats{};
    /** @brief Moves crossing wires onto empty ground positions; topology follows their identities.
     * @param lyt Layout. @param deleted_coords Positions cleared by rerouting.
     */
    void fix_wires(Lyt& lyt, const std::vector<tile<Lyt>>& deleted_coords)
    {
        for (const auto& t : deleted_coords)
        {
            const tile<Lyt> ground{t.x, t.y, 0};
            const auto      above = lyt.above(ground);
            if (above && lyt.is_empty_tile(ground))
            {
                if (const auto id = lyt.find_object(*above); id && lyt.is_wire(*id))
                {
                    lyt.move_node(*id, ground);
                    search_obstructions.clear_obstructed_coordinate(*above);
                }
            }
        }
    }
    /** @brief Collects retained endpoints and wire routes without compacting logical input indices.
     * @param lyt Layout. @param position Gate coordinate. @return Routes adjacent to the gate.
     * @throws std::invalid_argument If an intermediate wire is disconnected or cyclic.
     */
    [[nodiscard]] fanin_fanout_data<Lyt> get_fanin_and_fanouts(const Lyt& lyt, const tile<Lyt>& position)
    {
        fanin_fanout_data<Lyt> data{};
        const auto             gate = *lyt.find_object(position);
        const auto             wire = [&](const auto id)
        { return lyt.is_wire(id) && !lyt.is_gate(id) && !lyt.is_pi(id) && !lyt.is_po(id) && lyt.fanout_size(id) == 1; };
        lyt.foreach_fanin(gate,
                          [&](const auto source, const auto index)
                          {
                              layout_coordinate_path<Lyt>                 path{position};
                              auto                                        id = source;
                              std::unordered_set<typename Lyt::object_id> visited{};
                              while (true)
                              {
                                  path.push_back(lyt.get_tile(id));
                                  if (!wire(id))
                                  {
                                      break;
                                  }
                                  if (!visited.insert(id).second)
                                  {
                                      throw std::invalid_argument("A routing wire chain contains a cycle");
                                  }
                                  data.to_clear.push_back(lyt.get_tile(id));
                                  const auto previous = lyt.source({id, 0});
                                  if (!previous)
                                  {
                                      throw std::invalid_argument("A routing wire has no input connection");
                                  }
                                  id = *previous;
                              }
                              std::ranges::reverse(path);
                              data.fanins.push_back(path.source());
                              data.routes.push_back(std::move(path));
                              data.destinations.push_back({gate, index});
                          });
        lyt.foreach_sink(gate,
                         [&](auto destination)
                         {
                             layout_coordinate_path<Lyt>                 path{position};
                             std::unordered_set<typename Lyt::object_id> visited{};
                             while (true)
                             {
                                 const auto id = destination.object;
                                 path.push_back(lyt.get_tile(id));
                                 if (!wire(id))
                                 {
                                     break;
                                 }
                                 if (!visited.insert(id).second)
                                 {
                                     throw std::invalid_argument("A routing wire chain contains a cycle");
                                 }
                                 data.to_clear.push_back(lyt.get_tile(id));
                                 lyt.foreach_sink(id, [&](const auto next) { destination = next; });
                             }
                             data.fanouts.push_back(path.target());
                             data.routes.push_back(std::move(path));
                             data.destinations.push_back(destination);
                         });
        return data;
    }
    /**
     * This helper function computes a path between two coordinates using the A* algorithm.
     * It then marks the tiles along the path in the search obstructions.
     *
     * @param lyt Gate-level layout.
     * @param start_tile The starting coordinate of the path.
     * @param end_tile The ending coordinate of the path.
     * @return The computed path as a sequence of coordinates in the layout.
     */
    layout_coordinate_path<Lyt> get_path_and_obstruct(Lyt& lyt, const tile<Lyt>& start_tile, const tile<Lyt>& end_tile)
    {
        using dist = physical_design::path_finding::twoddwave_distance_functor<Lyt, uint64_t>;
        using cost = physical_design::path_finding::unit_cost_functor<Lyt, uint8_t>;
        physical_design::path_finding::a_star_params astar_params{};
        astar_params.crossings = !ps.planar_optimization;

        const auto path = physical_design::path_finding::a_star<layout_coordinate_path<Lyt>>(
            lyt, {start_tile, end_tile}, dist(), cost(), astar_params, search_obstructions);

        // obstruct the tiles along the computed path.
        for (const auto& tile : path)
        {
            search_obstructions.obstruct_coordinate(tile);
        }

        return path;
    }
    /**
     * Calculates the elapsed milliseconds since the `start` time, sets the `timeout_limit_reached` flag
     * if the timeout is exceeded, and returns the remaining time.
     *
     * @return Remaining time in milliseconds before timeout, or `0` if timeout has been reached.
     */
    uint64_t update_timeout() noexcept
    {
        const auto current_time = std::chrono::high_resolution_clock::now();
        const auto elapsed_ms =
            static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start).count());
        timeout_limit_reached = (elapsed_ms >= ps.timeout);
        return timeout_limit_reached ? 0 : ps.timeout - elapsed_ms;
    }
    /** @brief Attempts a placement and restores every route to its explicit destination input.
     * @param lyt Layout. @param candidate Placement to try. @param attempts Attempt count.
     * @param current Current gate coordinate. @param data Adjacent routes. @param moved Successful movement flag.
     * @param original Original coordinate. @return Whether another candidate may be tried.
     */
    bool check_new_position(Lyt& lyt, const tile<Lyt>& candidate, uint64_t& attempts, tile<Lyt>& current,
                            const fanin_fanout_data<Lyt>& data, bool& moved, const tile<Lyt>& original)
    {
        if ((candidate != current && !lyt.is_empty_tile(candidate)) ||
            !lyt.is_empty_tile({candidate.x, candidate.y, 1}))
        {
            return true;
        }
        ++attempts;
        const auto id = *lyt.find_object(current);
        lyt.move_node(id, candidate);
        search_obstructions.clear_obstructed_coordinate(current);
        search_obstructions.clear_obstructed_coordinate({current.x, current.y, 1});
        search_obstructions.obstruct_coordinate(candidate);
        search_obstructions.obstruct_coordinate({candidate.x, candidate.y, 1});
        current = candidate;
        std::vector<layout_coordinate_path<Lyt>> paths{};
        bool                                     complete = true;
        for (std::size_t index{}; index < data.routes.size(); ++index)
        {
            const bool incoming = index < data.fanins.size();
            auto       path     = get_path_and_obstruct(lyt, incoming ? data.routes[index].source() : candidate,
                                                        incoming ? candidate : data.routes[index].target());
            complete &= !path.empty();
            paths.push_back(std::move(path));
        }
        if (!complete)
        {
            for (const auto& path : paths)
            {
                for (const auto& t : path)
                {
                    search_obstructions.clear_obstructed_coordinate(t);
                }
            }
            return true;
        }
        for (std::size_t index{}; index < paths.size(); ++index)
        {
            route_path(lyt, paths[index], data.destinations[index]);
        }
        moved = true;
        return candidate != original;
    }
    /** @brief Moves the gate back and recreates its original routing with ordered input endpoints.
     * @param lyt Layout. @param current Current coordinate. @param original Original coordinate. @param data Routes.
     */
    void restore_original_wiring(Lyt& lyt, const tile<Lyt>& current, const tile<Lyt>& original,
                                 const fanin_fanout_data<Lyt>& data)
    {
        lyt.move_node(*lyt.find_object(current), original);
        for (std::size_t index{}; index < data.routes.size(); ++index)
        {
            route_path(lyt, data.routes[index], data.destinations[index]);
            for (const auto& t : data.routes[index])
            {
                search_obstructions.obstruct_coordinate(t);
            }
        }
        search_obstructions.clear_obstructed_coordinate(current);
        search_obstructions.clear_obstructed_coordinate({current.x, current.y, 1});
        search_obstructions.obstruct_coordinate(original);
        search_obstructions.obstruct_coordinate({original.x, original.y, 1});
    }
    /** @brief Relocates a gate toward the origin and reroutes without changing identities or input numbering.
     * @param lyt Layout. @param original Original coordinate. @return Whether the gate moved.
     */
    bool improve_gate_location(Lyt& lyt, const tile<Lyt>& original)
    {
        const auto data = get_fanin_and_fanouts(lyt, original);
        int32_t    min_x{}, min_y{};
        const auto direct_inputs = lyt.incoming_data_flow(original);
        for (const auto& source : data.fanins)
        {
            min_x = std::max(min_x, source.x);
            min_y = std::max(min_y, source.y);
            if (std::ranges::find(direct_inputs, source) != direct_inputs.end())
            {
                return false;
            }
        }
        const auto gate = *lyt.find_object(original);
        for (const auto& t : data.to_clear)
        {
            lyt.clear_tile(t);
            search_obstructions.clear_obstructed_coordinate(t);
        }
        for (const auto destination : data.destinations)
        {
            lyt.disconnect(destination);
        }
        fix_wires(lyt, data.to_clear);
        bool       moved{};
        auto       current = original;
        uint64_t   attempts{};
        const auto diagonal_limit = static_cast<int64_t>(original.x) + original.y;
        for (int64_t diagonal{}; diagonal < static_cast<int64_t>(lyt.width()) + lyt.height() - 1; ++diagonal)
        {
            for (int64_t x{}; x <= diagonal; ++x)
            {
                const auto y = diagonal - x;
                if (x >= lyt.width() || y >= lyt.height())
                {
                    continue;
                }
                if (x < min_x || y < min_y)
                {
                    continue;
                }
                if (diagonal > diagonal_limit || (diagonal == diagonal_limit && y > original.y))
                {
                    continue;
                }
                if (lyt.is_pi(gate) && x != 0 && y != 0)
                {
                    continue;
                }
                if (lyt.is_po(gate))
                {
                    if ((x < max_non_po.x && y < max_non_po.y) || diagonal == diagonal_limit)
                    {
                        continue;
                    }
                }
                if (!check_new_position(lyt, {x, y}, attempts, current, data, moved, original))
                {
                    return false;
                }
                if (moved)
                {
                    break;
                }
            }
            if (moved || (attempts >= max_gate_relocations && !lyt.is_po(gate)))
            {
                break;
            }
        }
        if (!moved)
        {
            restore_original_wiring(lyt, current, original, data);
        }
        return moved;
    }
};

}  // namespace detail

/**
 * A post-layout optimization algorithm as originally proposed in \"Post-Layout Optimization for Field-coupled
 * Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in NANOARCH 2023
 * (https://dl.acm.org/doi/10.1145/3611315.3633247) and extended in \"Efficient and Scalable Post-Layout Optimization
 * for Field-coupled Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
 * (https://ieeexplore.ieee.org/document/10916761). It can be used to reduce the area of a given sub-optimal Cartesian
 * gate-level layout created by heuristics or machine learning. This optimization utilizes the distinct characteristics
 * of the 2DDWave clocking scheme, which only allows information flow from top to bottom and left to right, therefore
 * only aforementioned clocking scheme is supported.
 *
 * To reduce the layout area, first, gates are moved up and to the left as far as possible, including rerouting. This
 * creates more compact layouts by freeing up space to the right and bottom, as all gates were moved to the top left
 * corner.
 *
 * After moving all gates, this algorithm also checks if excess wiring exists on the layout using the `wiring_reduction`
 * algorithm (cf. `wiring_reduction.hpp`)
 *
 * As outputs have to lay on the border of a layout for better accessibility, they are also moved to new borders
 * determined based on the location of all other gates.
 *
 * @note This function requires the gate-level layout to be 2DDWave-clocked!
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt 2DDWave-clocked Cartesian gate-level layout to optimize.
 * @param ps Parameters.
 * @param pst Statistics.
 * @throws std::invalid_argument If clocking or occupied geometry is invalid.
 * @throws std::overflow_error If the extent leaves no room for signed routing coordinates.
 */
template <typename Lyt>
void post_layout_optimization(Lyt& lyt, post_layout_optimization_params ps = {},
                              post_layout_optimization_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (!lyt.is_clocking_scheme(layouts::clocking::TWODDWAVE_NAME))
    {
        throw std::invalid_argument("Post-layout optimization requires 2DDWave clocking");
    }
    if (lyt.width() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max() - 1) ||
        lyt.height() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max() - 1) ||
        lyt.layers() > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    {
        throw std::overflow_error("Layout extent leaves no room for signed routing coordinates");
    }
    lyt.foreach_node(
        [&](const auto id)
        {
            if (!lyt.is_within_bounds(lyt.get_tile(id)))
            {
                throw std::invalid_argument("Post-layout optimization requires objects inside the geometry");
            }
        });
    // initialize stats for runtime measurement
    post_layout_optimization_stats             st{};
    detail::post_layout_optimization_impl<Lyt> p{lyt, ps, st};

    p.run();

    if (pst != nullptr)
    {
        *pst = st;
    }
}

}  // namespace fiction::physical_design
#pragma GCC diagnostic pop
