//
// Created by simon on 14.06.23.
//

#ifndef FICTION_POST_LAYOUT_OPTIMIZATION_HPP
#define FICTION_POST_LAYOUT_OPTIMIZATION_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/algorithms/physical_design/wiring_reduction.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <ostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction
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
    }
};

namespace detail
{

/**
 * Utility function to move wires that cross over empty tiles down one layer. This can happen if the wiring of a gate is
 * deleted.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 * @param deleted_coords Tiles that got deleted.
 */
template <typename Lyt>
void fix_wires(Lyt& lyt, const std::vector<tile<Lyt>>& deleted_coords) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    std::unordered_set<tile<Lyt>> moved_tiles{};
    moved_tiles.reserve(deleted_coords.size());
    for (const auto& tile : deleted_coords)
    {
        const auto ground = lyt.below(tile);
        const auto above  = lyt.above(tile);

        if (lyt.is_empty_tile(ground) && lyt.is_wire_tile(above))
        {
            const auto incoming_tile = lyt.incoming_data_flow(above).front();
            const auto outgoing_tile = lyt.outgoing_data_flow(above).front();

            // move wire from z=1 to z=0
            lyt.move_node(lyt.get_node(above), ground, {lyt.make_signal(lyt.get_node(incoming_tile))});

            // if outgoing tile has other incoming signals (e.g. AND), update children
            if (const auto in_flow = lyt.incoming_data_flow(outgoing_tile); !in_flow.empty())
            {
                const auto front = in_flow.front();

                if (std::find(deleted_coords.cbegin(), deleted_coords.cend(), front) == deleted_coords.cend() ||
                    std::find(moved_tiles.cbegin(), moved_tiles.cend(), front) != moved_tiles.cend())
                {
                    lyt.move_node(lyt.get_node(outgoing_tile), outgoing_tile,
                                  {lyt.make_signal(lyt.get_node(ground)), lyt.make_signal(lyt.get_node(front))});
                }
            }
            // otherwise, the wire is the only incoming signal
            else
            {
                lyt.move_node(lyt.get_node(outgoing_tile), outgoing_tile, {lyt.make_signal(lyt.get_node(ground))});
            }

            if constexpr (has_is_obstructed_coordinate_v<Lyt>)
            {
                // update obstructions
                lyt.obstruct_coordinate(ground);
                lyt.clear_obstructed_coordinate(above);
            }

            moved_tiles.insert(tile);
        }
    }
}
/**
 * This struct stores information about the fan-in and fan-out connections of a gate in a layout.
 * These fan-in and fan-outs are the preceding and succeeding gates in the logic network.
 * It contains vectors for fan-ins, fan-outs, and temporary coordinates to clear before routing.
 * Additionally, it includes layout coordinate paths for routing signals between the gate and its fan-in/fan-out
 * connections.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
struct fanin_fanout_data
{
    /**
     * This vector holds the layout coordinates of all fan-in connections to the gate.
     */
    std::vector<tile<Lyt>> fanins;

    /**
     * This vector holds the layout coordinates of all fan-out connections from the gate.
     */
    std::vector<tile<Lyt>> fanouts;

    /**
     * During the gate relocation process, this vector holds temporary layout coordinates that need to be cleared or
     * reset.
     */
    std::vector<tile<Lyt>> to_clear;

    /**
     * This layout_coordinate_path object represents the path for routing signals from the first fan-in
     * to the gate within the layout.
     */
    layout_coordinate_path<Lyt> route_fanin_1_to_gate;

    /**
     * This layout_coordinate_path object represents the path for routing signals from the second fan-in
     * to the gate within the layout.
     */
    layout_coordinate_path<Lyt> route_fanin_2_to_gate;

    /**
     * This layout_coordinate_path object represents the path for routing signals from the gate to
     * the first fan-out within the layout.
     */
    layout_coordinate_path<Lyt> route_gate_to_fanout_1;

    /**
     * This layout_coordinate_path object represents the path for routing signals from the gate to
     * the second fan-out within the layout.
     */
    layout_coordinate_path<Lyt> route_gate_to_fanout_2;
};
/**
 * This helper function is used to add a fanin coordinate to the appropriate route
 * based on whether it belongs to the the route from the first or second fanin to the gate.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param fanin The fanin coordinate to be added to the route.
 * @param is_first_fanin A boolean indicating whether this is part of the route from the first fanin to the gate.
 * @param ffd Reference to the fanin_fanout_data structure containing the routes.
 */
template <typename Lyt>
void add_fanin_to_route(const tile<Lyt>& fanin, bool is_first_fanin, fanin_fanout_data<Lyt>& ffd) noexcept
{
    if (is_first_fanin)
    {
        ffd.route_fanin_1_to_gate.insert(ffd.route_fanin_1_to_gate.cbegin(), fanin);
    }
    else
    {
        ffd.route_fanin_2_to_gate.insert(ffd.route_fanin_2_to_gate.cbegin(), fanin);
    }
}
/**
 * This helper function is used to add a fanout coordinate to the appropriate route
 * based on whether it belongs to the the route from the gate to the first or second fanout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param fanout The fanout coordinate to be added to the route.
 * @param is_first_fanout A boolean indicating whether it belongs to the route from the gate to the first fanout.
 * @param ffd Reference to the fanin_fanout_data structure containing the routes.
 */
template <typename Lyt>
void add_fanout_to_route(const tile<Lyt>& fanout, bool is_first_fanout, fanin_fanout_data<Lyt>& ffd) noexcept
{
    if (is_first_fanout)
    {
        ffd.route_gate_to_fanout_1.push_back(fanout);
    }
    else
    {
        ffd.route_gate_to_fanout_2.push_back(fanout);
    }
}
/**
 * Utility function to trace back fanins and fanouts of a gate. Based on the gate to be moved, this function returns the
 * location of the fanins and fanouts, as well as the wiring in between them. Additionally, all wire tiles between
 * fanins and the gate, as well as between the gate and fanouts are collected for deletion.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Cartesian Gate-level layout.
 * @param op coordinate of the gate to be moved.
 * @return fanin and fanout gates, wires to be deleted and old routing paths.
 */
template <typename Lyt>
[[nodiscard]] fanin_fanout_data<Lyt> get_fanin_and_fanouts(const Lyt& lyt, const tile<Lyt>& op) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    fanin_fanout_data<Lyt> ffd{};

    auto fanin1  = tile<Lyt>{};
    auto fanin2  = tile<Lyt>{};
    auto fanout1 = tile<Lyt>{};
    auto fanout2 = tile<Lyt>{};

    std::unordered_set<tile<Lyt>> fanins_set{};
    fanins_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);
    std::unordered_set<tile<Lyt>> fanouts_set{};
    fanouts_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);

    lyt.foreach_fanin(lyt.get_node(op),
                      [&lyt, &fanins_set, &op, &fanin1, &fanin2, &ffd](const auto& fin)
                      {
                          auto fanin = static_cast<tile<Lyt>>(fin);
                          if (fanins_set.find(fanin) == fanins_set.cend())
                          {

                              // Add fanin to the respective route
                              add_fanin_to_route(op, fanins_set.empty(), ffd);
                              add_fanin_to_route(fanin, fanins_set.empty(), ffd);

                              // Continue until gate or primary input (PI) is found
                              while (lyt.is_wire_tile(fanin) && lyt.fanout_size(lyt.get_node(fanin)) == 1 &&
                                     !lyt.is_pi_tile(fanin))
                              {
                                  ffd.to_clear.push_back(fanin);
                                  fanin = lyt.incoming_data_flow(fanin).front();

                                  // Add fanin to the respective route
                                  add_fanin_to_route(fanin, fanins_set.empty(), ffd);
                              }

                              // Set the respective fanin based on the route
                              if (fanins_set.empty())
                              {
                                  fanin1 = fanin;
                              }
                              else
                              {
                                  fanin2 = fanin;
                              }

                              fanins_set.insert(fanin);
                          }
                      });
    // same for fanouts
    lyt.foreach_fanout(lyt.get_node(op),
                       [&lyt, &fanouts_set, &op, &fanout1, &fanout2, &ffd](const auto& fout)
                       {
                           tile<Lyt> fanout = lyt.get_tile(fout);

                           if (fanouts_set.find(fanout) == fanouts_set.cend())
                           {

                               // Add fanout to the respective route
                               add_fanout_to_route(op, fanouts_set.empty(), ffd);
                               add_fanout_to_route(fanout, fanouts_set.empty(), ffd);

                               // Continue until gate or primary output (PO) is found
                               while (lyt.is_wire_tile(fanout) && lyt.fanout_size(lyt.get_node(fanout)) != 0 &&
                                      lyt.fanout_size(lyt.get_node(fanout)) != 2)
                               {
                                   ffd.to_clear.push_back(fanout);
                                   fanout = lyt.outgoing_data_flow(fanout).front();

                                   // Add fanout to the respective route
                                   add_fanout_to_route(fanout, fanouts_set.empty(), ffd);
                               }

                               // Set the respective fanout based on the route
                               if (fanouts_set.empty())
                               {
                                   fanout1 = fanout;
                               }
                               else
                               {
                                   fanout2 = fanout;
                               }

                               fanouts_set.insert(fanout);
                           }
                       });

    // add fanins and fanouts if existing
    if (!fanin1.is_dead())
    {
        ffd.fanins.push_back(fanin1);
    }
    if (!fanin2.is_dead())
    {
        ffd.fanins.push_back(fanin2);
    }
    if (!fanout1.is_dead())
    {
        ffd.fanouts.push_back(fanout1);
    }
    if (!fanout2.is_dead())
    {
        ffd.fanouts.push_back(fanout2);
    }

    return ffd;
}
/**
 * This helper function computes a path between two coordinates using the A* algorithm.
 * It then obstructs the tiles along the path in the given layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Reference to the layout.
 * @param start The starting coordinate of the path.
 * @param end The ending coordinate of the path.
 * @return The computed path as a sequence of coordinates in the layout.
 */
template <typename Lyt>
layout_coordinate_path<Lyt> get_path_and_obstruct(Lyt& lyt, const tile<Lyt>& start, const tile<Lyt>& end)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
    using cost = unit_cost_functor<Lyt, uint8_t>;
    static const a_star_params params{true};

    layout_coordinate_path<Lyt> path = a_star<layout_coordinate_path<Lyt>>(lyt, {start, end}, dist(), cost(), params);

    // Obstruct the tiles along the computed path.
    for (const auto& tile : path)
    {
        lyt.obstruct_coordinate(tile);
    }

    return path;
}
/**
 * Utility function that moves gates to new coordinates and checks if routing is possible.
 * This includes:
 *
 * - removing the old wiring between fanins, the gate and fanouts
 * - updating the incoming signals
 * - determining coordinates that would improve the layout
 * - testing all those coordinates by moving the gate to each one and checking if a new wiring can be found
 * - if a new coordinate is found and wiring is possible, it is applied and incoming signals are updated
 * - if no better coordinate is found, the old wiring is restored
 *
 * @tparam Lyt Cartesian obstruction gate-level layout type.
 * @param lyt 2DDWave-clocked cartesian obstruction gate-level layout.
 * @param old_pos Old position of the gate to be moved.
 * @return `true` if the gate was moved successfully, `false` otherwise.
 */
template <typename Lyt>
bool improve_gate_location(Lyt& lyt, const tile<Lyt>& old_pos, const tile<Lyt>& max_non_po,
                           const uint64_t max_gate_relocations) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    const auto& [fanins, fanouts, to_clear, old_path_from_fanin_1_to_gate, old_path_from_fanin_2_to_gate,
                 old_path_from_gate_to_fanout_1, old_path_from_gate_to_fanout_2] = get_fanin_and_fanouts(lyt, old_pos);

    uint64_t min_x = 0;
    uint64_t min_y = 0;

    // determine minimum coordinates for new placements
    if (!fanins.empty())
    {
        min_x =
            std::max_element(fanins.cbegin(), fanins.cend(), [](const auto& a, const auto& b) { return a.x < b.x; })->x;
        min_y =
            std::max_element(fanins.cbegin(), fanins.cend(), [](const auto& a, const auto& b) { return a.y < b.y; })->y;
    }

    const auto max_x        = old_pos.x;
    const auto max_y        = old_pos.y;
    const auto max_diagonal = max_x + max_y;

    auto new_pos = tile<Lyt>{};

    // if gate is directly connected to one of its fanins, no improvement is possible
    for (const auto& fanin : fanins)
    {
        for (const auto& i : lyt.incoming_data_flow(old_pos))
        {
            if (i == fanin)
            {
                return false;
            }
        }
    }

    // if gate is a PI and directly connected to its fanout, no improvement is possible
    if (lyt.is_pi_tile(old_pos))
    {
        for (const auto& fanout : fanouts)
        {
            for (const auto& outgoing_tile : lyt.outgoing_data_flow(old_pos))
            {
                if (outgoing_tile == fanout)
                {
                    return false;
                }
            }
        }
    }

    // remove wiring
    for (const auto& tile : to_clear)
    {
        lyt.clear_tile(tile);
        lyt.clear_obstructed_coordinate(tile);
    }

    // remove children of gate to be moved
    lyt.resize({lyt.x() + 2, lyt.y(), lyt.z()});
    lyt.move_node(lyt.get_node(old_pos), {lyt.x(), 0}, {});

    // update children of fanouts
    for (const auto& fanout : fanouts)
    {
        std::vector<mockturtle::signal<Lyt>> fins{};
        fins.reserve(2);
        lyt.foreach_fanin(lyt.get_node(fanout),
                          [&lyt, &fins, &old_pos](const auto& i)
                          {
                              auto fout = static_cast<tile<Lyt>>(i);
                              if (fout != old_pos)
                              {
                                  fins.push_back(lyt.make_signal(lyt.get_node(fout)));
                              }
                          });

        lyt.move_node(lyt.get_node(fanout), fanout, fins);
    }

    // remove children of gate to be moved
    lyt.move_node(lyt.get_node({lyt.x(), 0}), old_pos, {});
    lyt.resize({lyt.x() - 2, lyt.y(), lyt.z()});

    // fix wires that cross over empty tiles
    fix_wires(lyt, to_clear);

    auto moved_gate  = false;
    auto current_pos = old_pos;

    uint64_t num_gate_relocations = 0;

    // iterate over layout diagonally
    for (uint64_t k = 0; k < lyt.x() + lyt.y() + 1; ++k)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const uint64_t y = k - x;

            if (moved_gate || ((num_gate_relocations >= max_gate_relocations) && !lyt.is_po_tile(current_pos)))
            {
                break;
            }

            // only check better positions
            if (lyt.y() >= y && y >= min_y && lyt.x() >= x && x >= min_x && ((x + y) <= max_diagonal) &&
                (((x + y) < max_diagonal) || (y <= max_y)) &&
                ((!lyt.is_pi_tile(current_pos)) || (lyt.is_pi_tile(current_pos) && (x == 0 || y == 0))) &&
                !(lyt.is_po_tile(current_pos) && (((x < max_non_po.x) && (y < max_non_po.y)) ||
                                                  ((x + y) == static_cast<uint64_t>(old_pos.x + old_pos.y)))))
            {
                new_pos = tile<Lyt>{x, y};
                if (lyt.is_empty_tile(new_pos) && lyt.is_empty_tile({new_pos.x, new_pos.y, 1}))
                {
                    num_gate_relocations++;
                    // move gate to new positions and update obstructions
                    lyt.move_node(lyt.get_node(current_pos), new_pos, {});
                    lyt.obstruct_coordinate(new_pos);
                    lyt.obstruct_coordinate({new_pos.x, new_pos.y, 1});
                    lyt.clear_obstructed_coordinate(current_pos);
                    lyt.clear_obstructed_coordinate({current_pos.x, current_pos.y, 1});

                    // get paths for fanins and fanouts
                    layout_coordinate_path<Lyt> new_path_from_fanin_1_to_gate, new_path_from_fanin_2_to_gate,
                        new_path_from_gate_to_fanout_1, new_path_from_gate_to_fanout_2;
                    // Get paths for fanins and fanouts
                    if (!fanins.empty())
                    {
                        new_path_from_fanin_1_to_gate = get_path_and_obstruct(lyt, fanins[0], new_pos);
                    }

                    if (fanins.size() == 2)
                    {
                        new_path_from_fanin_2_to_gate = get_path_and_obstruct(lyt, fanins[1], new_pos);
                    }

                    if (!fanouts.empty())
                    {
                        new_path_from_gate_to_fanout_1 = get_path_and_obstruct(lyt, new_pos, fanouts[0]);
                    }

                    if (fanouts.size() == 2)
                    {
                        new_path_from_gate_to_fanout_2 = get_path_and_obstruct(lyt, new_pos, fanouts[1]);
                    }

                    // if possible routing was found, it will be applied
                    if (!(fanins.size() > 0 && new_path_from_fanin_1_to_gate.empty()) &&
                        !(fanins.size() == 2 && new_path_from_fanin_2_to_gate.empty()) &&
                        !(fanouts.size() > 0 && new_path_from_gate_to_fanout_1.empty()) &&
                        !(fanouts.size() == 2 && new_path_from_gate_to_fanout_2.empty()))
                    {
                        for (const auto& path : {new_path_from_fanin_1_to_gate, new_path_from_fanin_2_to_gate,
                                                 new_path_from_gate_to_fanout_1, new_path_from_gate_to_fanout_2})
                        {
                            if (!path.empty())
                            {
                                route_path(lyt, path);
                                for (const auto& tile : path)
                                {
                                    lyt.obstruct_coordinate(tile);
                                }
                            }
                        }

                        moved_gate = true;

                        // update children based on number of fanins
                        if (fanins.size() == 2)
                        {
                            lyt.move_node(lyt.get_node(new_pos), new_pos,
                                          {
                                              lyt.make_signal(lyt.get_node(new_path_from_fanin_1_to_gate.end()[-2])),
                                              lyt.make_signal(lyt.get_node(new_path_from_fanin_2_to_gate.end()[-2])),
                                          });
                        }
                        else if (fanins.size() == 1)
                        {
                            lyt.move_node(lyt.get_node(new_pos), new_pos,
                                          {lyt.make_signal(lyt.get_node(new_path_from_fanin_1_to_gate.end()[-2]))});
                        }

                        // update children of fanouts
                        for (const auto& fanout : fanouts)
                        {
                            std::vector<mockturtle::signal<Lyt>> signals{};
                            signals.reserve(lyt.fanin_size(lyt.get_node(fanout)));

                            lyt.foreach_fanin(lyt.get_node(fanout),
                                              [&lyt, &signals](const auto& i)
                                              {
                                                  auto fout = static_cast<tile<Lyt>>(i);
                                                  signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                                              });

                            lyt.move_node(lyt.get_node(fanout), fanout, signals);
                        }

                        if (new_pos == old_pos)
                        {
                            return false;
                        }
                    }
                    // if no routing was found, remove added obstructions
                    else
                    {
                        for (const auto& path : {new_path_from_fanin_1_to_gate, new_path_from_fanin_2_to_gate,
                                                 new_path_from_gate_to_fanout_1, new_path_from_gate_to_fanout_2})
                        {
                            for (const auto& tile : path)
                            {
                                lyt.clear_obstructed_coordinate(tile);
                            }
                        }
                    }

                    current_pos = new_pos;
                }
            }
        }

        if (moved_gate || ((num_gate_relocations >= max_gate_relocations) && !lyt.is_po_tile(current_pos)))
        {
            break;
        }
    }

    // if no better coordinate was found, restore old wiring
    if (!moved_gate)
    {
        lyt.move_node(lyt.get_node(current_pos), old_pos, {});

        for (const auto& r : {old_path_from_fanin_1_to_gate, old_path_from_fanin_2_to_gate,
                              old_path_from_gate_to_fanout_1, old_path_from_gate_to_fanout_2})
        {
            if (!r.empty())
            {
                route_path<Lyt, layout_coordinate_path<Lyt>>(lyt, r);
            }
            for (const auto& t : r)
            {
                lyt.obstruct_coordinate(t);
            }
        }

        // update obstructions
        lyt.clear_obstructed_coordinate(current_pos);
        lyt.clear_obstructed_coordinate({current_pos.x, current_pos.y, 1});
        lyt.obstruct_coordinate(old_pos);
        lyt.obstruct_coordinate({old_pos.x, old_pos.y, 1});

        // update children on old position
        std::vector<mockturtle::signal<Lyt>> signals{};
        signals.reserve(lyt.fanin_size(lyt.get_node(old_pos)));

        lyt.foreach_fanin(lyt.get_node(old_pos),
                          [&lyt, &signals](const auto& i)
                          {
                              auto fanin = static_cast<tile<Lyt>>(i);
                              signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
                          });

        lyt.move_node(lyt.get_node(old_pos), old_pos, signals);

        // update children of fanouts
        for (const auto& fanout : fanouts)
        {
            std::vector<mockturtle::signal<Lyt>> fout_signals{};
            fout_signals.reserve(lyt.fanin_size(lyt.get_node(fanout)));

            lyt.foreach_fanin(lyt.get_node(fanout),
                              [&lyt, &fout_signals](const auto& i)
                              {
                                  auto fout = static_cast<tile<Lyt>>(i);
                                  fout_signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                              });

            lyt.move_node(lyt.get_node(fanout), fanout, fout_signals);
        }
        return false;
    }

    return true;
}
/**
 * Utility function that moves outputs from the last row to the previous row, and from the last column to the previous
 * column, if possible.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 */
template <typename Lyt>
void optimize_output_positions(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    bool optimizable = true;

    for (uint64_t x = 0; x <= lyt.x(); ++x)
    {
        if (!(lyt.is_empty_tile({x, lyt.y()}) ||
              (lyt.is_po_tile({x, lyt.y(), 0}) && lyt.is_empty_tile({x + 1, lyt.y() - 1, 0}) && (x < lyt.x()))))
        {
            optimizable = false;
        }
    }

    if (optimizable)
    {
        for (uint64_t x = 0; x < lyt.x(); ++x)
        {
            if (lyt.is_po_tile({x, lyt.y(), 0}))
            {
                std::vector<mockturtle::signal<Lyt>> signals{};
                signals.reserve(lyt.fanin_size(lyt.get_node({x, lyt.y()})));
                lyt.foreach_fanin(lyt.get_node({x, lyt.y()}),
                                  [&signals](const auto& fanin) { signals.push_back(fanin); });
                lyt.move_node(lyt.get_node({x, lyt.y()}), {x + 1, lyt.y() - 1, 0}, signals);
            }
        }
    }

    optimizable = true;

    for (uint64_t y = 0; y <= lyt.y(); ++y)
    {
        if (!(lyt.is_empty_tile({lyt.x(), y}) ||
              (lyt.is_po_tile({lyt.x(), y, 0}) && lyt.is_empty_tile({lyt.x() - 1, y + 1, 0}) && (y < lyt.y()))))
        {
            optimizable = false;
        }
    }

    if (optimizable)
    {
        for (uint64_t y = 0; y < lyt.y(); ++y)
        {
            if (lyt.is_po_tile({lyt.x(), y, 0}))
            {
                std::vector<mockturtle::signal<Lyt>> signals{};
                signals.reserve(lyt.fanin_size(lyt.get_node({lyt.x(), y})));
                lyt.foreach_fanin(lyt.get_node({lyt.x(), y}),
                                  [&signals](const auto& fanin) { signals.push_back(fanin); });
                lyt.move_node(lyt.get_node({lyt.x(), y}), {lyt.x() - 1, y + 1, 0}, signals);
            }
        }
    }
    // calculate bounding box
    auto bounding_box = bounding_box_2d(lyt);
    lyt.resize({bounding_box.get_max().x, bounding_box.get_max().y, lyt.z()});

    // Check for misplaced POs in second last row and move them one row down
    for (uint64_t x = 0; x < lyt.x(); ++x)
    {
        if (lyt.is_po_tile({x, lyt.y() - 1, 0}))
        {
            // Get fanin signal of the PO
            std::vector<mockturtle::signal<Lyt>> signals{};
            signals.reserve(lyt.fanin_size(lyt.get_node({x, lyt.y()})));
            lyt.foreach_fanin(lyt.get_node({x, lyt.y() - 1}),
                              [&signals](const auto& fanin) { signals.push_back(fanin); });

            // Move PO one row down
            lyt.move_node(lyt.get_node({x, lyt.y() - 1}), {x, lyt.y(), 0}, {});

            // Create a wire segment at the previous location of the PO and connect it with its fanin
            lyt.create_buf(signals[0], {x, lyt.y() - 1});

            // Connect the PO with the new wire segment
            lyt.move_node(lyt.get_node({x, lyt.y()}), {x, lyt.y(), 0},
                          {lyt.make_signal(lyt.get_node({x, lyt.y() - 1}))});
        }
    }

    // Check for misplaced POs in second last column and move them one column to the right
    for (uint64_t y = 0; y < lyt.y(); ++y)
    {
        if (lyt.is_po_tile({lyt.x() - 1, y, 0}))
        {
            // Get fanin signal of the PO
            std::vector<mockturtle::signal<Lyt>> signals{};
            signals.reserve(lyt.fanin_size(lyt.get_node({lyt.x(), y})));
            lyt.foreach_fanin(lyt.get_node({lyt.x() - 1, y}),
                              [&signals](const auto& fanin) { signals.push_back(fanin); });

            // Move PO one column to the right
            lyt.move_node(lyt.get_node({lyt.x() - 1, y}), {lyt.x(), y, 0}, {});

            // Create a wire segment at the previous location of the PO and connect it with its fanin
            lyt.create_buf(signals[0], {lyt.x() - 1, y});

            // Connect the PO with the new wire segment
            lyt.move_node(lyt.get_node({lyt.x(), y}), {lyt.x(), y, 0},
                          {lyt.make_signal(lyt.get_node({lyt.x() - 1, y}))});
        }
    }

    // update bounding box
    bounding_box.update_bounding_box();
    lyt.resize({bounding_box.get_max().x, bounding_box.get_max().y, lyt.z()});

    // Check if PO is located in bottom right corner and relocation would save more tiles (only possible for layouts
    // with a single PO)
    if (lyt.is_po_tile({lyt.x(), lyt.y(), 0}) && (lyt.num_pos() == 1))
    {
        // Check if relocation would save tiles
        if (lyt.has_western_incoming_signal({lyt.x(), lyt.y(), 0}) &&
            ((lyt.x() * (lyt.y() + 2)) < ((lyt.x() + 1) * (lyt.y() + 1))))
        {
            // Get fanin signal of the PO
            std::vector<mockturtle::signal<Lyt>> signals{};
            signals.reserve(lyt.fanin_size(lyt.get_node({lyt.x(), lyt.y()})));
            lyt.foreach_fanin(lyt.get_node({lyt.x(), lyt.y()}),
                              [&signals](const auto& fanin) { signals.push_back(fanin); });

            // Resize layout
            lyt.resize({lyt.x(), lyt.y() + 1, lyt.z()});

            // Move PO one tile down and to the left
            lyt.move_node(lyt.get_node({lyt.x(), lyt.y() - 1}), {lyt.x() - 1, lyt.y(), 0}, signals);
        }
        // Check if relocation would save tiles
        else if (lyt.has_northern_incoming_signal({lyt.x(), lyt.y(), 0}) &&
                 (((lyt.x() + 2) * lyt.y()) < ((lyt.x() + 1) * (lyt.y() + 1))))
        {
            // Get fanin signal of the PO
            std::vector<mockturtle::signal<Lyt>> signals{};
            signals.reserve(lyt.fanin_size(lyt.get_node({lyt.x(), lyt.y()})));
            lyt.foreach_fanin(lyt.get_node({lyt.x(), lyt.y()}),
                              [&signals](const auto& fanin) { signals.push_back(fanin); });

            // Resize layout
            lyt.resize({lyt.x() + 1, lyt.y(), lyt.z()});

            // Move PO one tile up and to the right
            lyt.move_node(lyt.get_node({lyt.x() - 1, lyt.y()}), {lyt.x(), lyt.y() - 1, 0}, signals);
        }
    }
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

    return static_cast<bool>(std::make_pair(a.x + a.y, a.x) < std::make_pair(b.x + b.y, b.x));
}

template <typename Lyt>
class post_layout_optimization_impl
{
  public:
    post_layout_optimization_impl(const Lyt& lyt, const post_layout_optimization_params& p,
                                  post_layout_optimization_stats& st) :
            plyt{lyt},
            ps{p},
            pst{st}
    {}

    void run()
    {
        static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
        static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

        const mockturtle::stopwatch stop{pst.time_total};
        pst.x_size_before = plyt.x() + 1;
        pst.y_size_before = plyt.y() + 1;

        uint64_t max_gate_relocations = ps.max_gate_relocations.value_or((plyt.x() + 1) * (plyt.y() + 1));

        // Optimization
        auto layout                  = obstruction_layout<Lyt>(plyt);
        bool moved_at_least_one_gate = true;
        bool reduced_wiring          = true;

        if (max_gate_relocations == 0)
        {
            fiction::wiring_reduction_stats wiring_reduction_stats{};
            fiction::wiring_reduction(layout, &wiring_reduction_stats);
        }
        else
        {
            while (moved_at_least_one_gate || reduced_wiring)
            {
                reduced_wiring = false;
                fiction::wiring_reduction_stats wiring_reduction_stats{};
                if (moved_at_least_one_gate && !ps.optimize_pos_only)
                {
                    fiction::wiring_reduction(layout, &wiring_reduction_stats);
                    if ((wiring_reduction_stats.area_improvement != 0ull) ||
                        (wiring_reduction_stats.wiring_improvement != 0ull))
                    {
                        reduced_wiring = true;
                    }
                }

                std::vector<tile<Lyt>> gate_tiles{};
                gate_tiles.reserve(layout.num_gates() + layout.num_pis() - layout.num_pos());
                layout.foreach_node(
                    [&layout, &gate_tiles](const auto& node)
                    {
                        if (const tile<Lyt> tile = layout.get_tile(node);
                            (layout.is_gate(node) && !layout.is_wire(node)) || layout.is_fanout(node) ||
                            layout.is_pi_tile(tile) || layout.is_po_tile(tile))
                        {
                            layout.obstruct_coordinate({tile.x, tile.y, 1});
                            gate_tiles.emplace_back(tile);
                        }
                    });

                std::sort(gate_tiles.begin(), gate_tiles.end(), detail::compare_gate_tiles<Lyt>);

                tile<Lyt> max_non_po{0, 0};
                // Determine minimal border for POs
                for (const auto& gate_tile : gate_tiles)
                {
                    if (!layout.is_po_tile(gate_tile))
                    {
                        max_non_po.x = std::max(max_non_po.x, gate_tile.x);
                        max_non_po.y = std::max(max_non_po.y, gate_tile.y);
                    }
                }
                moved_at_least_one_gate = false;
                for (auto& gate_tile : gate_tiles)
                {
                    if (!ps.optimize_pos_only || (ps.optimize_pos_only && layout.is_po_tile(gate_tile)))
                    {
                        if (detail::improve_gate_location(layout, gate_tile, max_non_po, max_gate_relocations))
                        {
                            moved_at_least_one_gate = true;
                        }
                    }
                }
                // calculate bounding box
                const auto bounding_box = bounding_box_2d(layout);
                layout.resize({bounding_box.get_max().x, bounding_box.get_max().y, layout.z()});
            }
        }
        detail::optimize_output_positions(layout);

        // calculate bounding box
        const auto bounding_box = bounding_box_2d(layout);
        layout.resize({bounding_box.get_max().x, bounding_box.get_max().y, layout.z()});

        pst.x_size_after           = layout.x() + 1;
        pst.y_size_after           = layout.y() + 1;
        const uint64_t area_before = pst.x_size_before * pst.y_size_before;
        const uint64_t area_after  = pst.x_size_after * pst.y_size_after;
        double_t       area_percentage_difference =
            static_cast<double_t>(area_before - area_after) / static_cast<double_t>(area_before) * 100.0;
        area_percentage_difference = std::round(area_percentage_difference * 100) / 100;
        pst.area_improvement       = area_percentage_difference;
    }

  private:
    /**
     * 2DDWave-clocked Cartesian gate-level layout to optimize.
     */
    const Lyt& plyt;
    /**
     * Post-layout optimization parameters.
     */
    post_layout_optimization_params ps;
    /**
     * Statistics about the post-layout optimization process.
     */
    post_layout_optimization_stats& pst;
};
}  // namespace detail

/**
 * A post-layout optimization algorithm as originally proposed in \"Post-Layout Optimization for Field-coupled
 * Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in NANOARCH 2023. It can be used to reduce the area of a
 * given sub-optimal Cartesian gate-level layout created by heuristics or machine learning. This optimization utilizes
 * the distinct characteristics of the 2DDWave clocking scheme, which only allows information flow from top to bottom
 * and left to right, therefore only aforementioned clocking scheme is supported.
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
 */
template <typename Lyt>
void post_layout_optimization(const Lyt& lyt, post_layout_optimization_params ps = {},
                              post_layout_optimization_stats* pst = nullptr) noexcept
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
    post_layout_optimization_stats             st{};
    detail::post_layout_optimization_impl<Lyt> p{lyt, ps, st};

    p.run();

    if (pst != nullptr)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_POST_LAYOUT_OPTIMIZATION_HPP
