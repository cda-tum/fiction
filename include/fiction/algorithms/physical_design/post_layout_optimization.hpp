//
// Created by simon on 14.06.23.
//

#ifndef FICTION_POST_LAYOUT_OPTIMIZATION_HPP
#define FICTION_POST_LAYOUT_OPTIMIZATION_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

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
 * This struct stores statistics about the post-layout optimization process.
 */
struct post_layout_optimization_stats
{
    /**
     * Runtime of the post-layout optimization process.
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
 *
 * @return The computed path as a sequence of coordinates in the layout.
 */
template <typename Lyt>
layout_coordinate_path<Lyt> get_path_and_obstruct(Lyt& lyt, const tile<Lyt>& start, const tile<Lyt>& end)
{
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
 * @return Flag that indicates if gate was moved successfully and the new coordinate of the moved gate.
 */
template <typename Lyt>
[[nodiscard]] std::pair<bool, tile<Lyt>> improve_gate_location(Lyt& lyt, const tile<Lyt>& old_pos) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(has_is_obstructed_coordinate_v<Lyt>, "Lyt is not an obstruction layout");

    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "Clocking scheme is not 2DDWave" << std::endl;
        std::make_pair(false, old_pos);
    }

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
                return std::make_pair(false, new_pos);
            }
        }
    }

    // remove wiring
    for (const auto& tile : to_clear)
    {
        lyt.clear_tile(tile);
        lyt.clear_obstructed_coordinate(tile);
    }

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
    lyt.move_node(lyt.get_node(old_pos), old_pos, {});

    // fix wires that cross over empty tiles
    fix_wires(lyt, to_clear);

    bool moved_gate             = false;
    auto current_pos            = old_pos;
    bool improved_gate_location = false;
    // iterate over layout diagonally
    for (uint64_t k = 0; k < lyt.x() + lyt.y() + 1; ++k)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const uint64_t y = k - x;

            if (moved_gate)
            {
                break;
            }

            // only check better positions
            if (lyt.y() >= y && y >= min_y && lyt.x() >= x && x >= min_x && ((x + y) <= max_diagonal) &&
                ((!lyt.is_pi_tile(current_pos)) || (lyt.is_pi_tile(current_pos) && (x == 0 || y == 0))))
            {
                new_pos = {x, y};
                if (lyt.is_empty_tile(new_pos) && lyt.is_empty_tile({new_pos.x, new_pos.y, 1}))
                {
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

                        if (new_pos != old_pos)
                        {
                            improved_gate_location = true;
                        }

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

        if (moved_gate)
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

        // update children oóf fanouts
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
    }

    return std::make_pair(improved_gate_location, new_pos);
}

/**
 * Utility function that deletes all specified rows and columns.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 * @param rows_to_delete Rows to be deleted.
 * @param columns_to_delete Columns to be deleted.
 *
 * @note This function requires the gate-level layout to be 2DDWave-clocked and Cartesian.
 */
template <typename Lyt>
void delete_rows_and_columns(Lyt& lyt, std::vector<uint64_t>& rows_to_delete,
                             std::vector<uint64_t>& columns_to_delete) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "Clocking scheme is not 2DDWave" << std::endl;
        return;
    }

    auto layout_copy = lyt.clone();

    // delete all excess wiring in rows with only vertical wires
    for (uint64_t row : rows_to_delete)
    {
        for (uint64_t x = 0; x <= lyt.x(); ++x)
        {
            if (const tile<Lyt> old_pos = {x, row, 0}; !lyt.is_empty_tile(old_pos))
            {
                lyt.clear_tile(old_pos);
            }
        }
    }

    // delete all excess wiring in columns with only horizontal wires
    for (uint64_t column : columns_to_delete)
    {
        for (uint64_t y = 0; y <= lyt.y(); ++y)
        {
            if (const tile<Lyt> old_pos = {column, y, 0}; !lyt.is_empty_tile(old_pos))
            {
                lyt.clear_tile(old_pos);
            }
        }
    }

    for (uint64_t x = 0; x <= lyt.x(); ++x)
    {
        // calculate column offset based on number of columns deleted to the left of current column
        uint64_t column_offset = 0;
        if (!columns_to_delete.empty())
        {
            column_offset = static_cast<uint64_t>(
                std::upper_bound(columns_to_delete.cbegin(), columns_to_delete.cend(), x) - columns_to_delete.cbegin());
        }
        for (uint64_t y = 0; y <= lyt.y(); ++y)
        {
            // calculate row offset based on number of rows deleted above the current row
            uint64_t row_offset = 0;
            if (!rows_to_delete.empty())
            {
                row_offset = static_cast<uint64_t>(std::upper_bound(rows_to_delete.cbegin(), rows_to_delete.cend(), y) -
                                                   rows_to_delete.cbegin());
            }

            for (uint64_t z = 0; z <= lyt.z(); ++z)
            {

                if (const tile<Lyt> old_pos = {x, y, z}; !lyt.is_empty_tile(old_pos))
                {
                    const tile<Lyt> new_pos = {x - column_offset, y - row_offset, z};

                    std::vector<mockturtle::signal<Lyt>> fins{};

                    for (auto& fanin : layout_copy.incoming_data_flow(old_pos))
                    {
                        // skip removed columns and decrease column offset
                        uint64_t excess_column_offset = 0;
                        if (!columns_to_delete.empty())
                        {
                            while (std::find(std::cbegin(columns_to_delete), std::cend(columns_to_delete), fanin.x) !=
                                   std::cend(columns_to_delete))
                            {
                                fanin = layout_copy.incoming_data_flow(fanin)[0];
                                excess_column_offset++;
                            }
                        }

                        // skip removed rows and decrease row offset
                        uint64_t excess_row_offset = 0;
                        if (!rows_to_delete.empty())
                        {
                            while (std::find(std::begin(rows_to_delete), std::end(rows_to_delete), fanin.y) !=
                                   std::end(rows_to_delete))
                            {
                                fanin = layout_copy.incoming_data_flow(fanin)[0];
                                excess_row_offset++;
                            }
                        }

                        fins.push_back(
                            lyt.make_signal(lyt.get_node({fanin.x - column_offset + excess_column_offset,
                                                          fanin.y - row_offset + excess_row_offset, fanin.z})));
                    }
                    lyt.move_node(lyt.get_node(old_pos), new_pos, fins);
                }
            }
        }
    }
}

/**
 * Utility function that deletes rows that only contain vertically connected wires.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 */
template <typename Lyt>
void delete_wires(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "Clocking scheme is not 2DDWave" << std::endl;
        return;
    }

    std::vector<uint64_t> rows_to_delete{};
    rows_to_delete.reserve(lyt.y());
    for (uint64_t y = 0; y <= lyt.y(); ++y)
    {
        bool found_row = true;
        for (uint64_t x = 0; x <= lyt.x(); ++x)
        {
            const bool is_vertical_wire =
                (lyt.is_wire_tile({x, y}) && lyt.fanin_size(lyt.get_node({x, y})) == 1 &&
                 lyt.fanout_size(lyt.get_node({x, y})) == 1 && lyt.has_northern_incoming_signal({x, y}) &&
                 lyt.has_southern_outgoing_signal({x, y}));

            if (!(is_vertical_wire || lyt.is_empty_tile({x, y})))
            {
                found_row = false;
            }
        }
        if (found_row)
        {
            rows_to_delete.push_back(y);
        }
    }

    std::vector<uint64_t> columns_to_delete{};
    columns_to_delete.reserve(lyt.x());
    for (uint64_t x = 0; x <= lyt.x(); ++x)
    {
        bool found_column = true;
        for (uint64_t y = 0; y <= lyt.y(); ++y)
        {
            // Check if the column can be deleted based on certain conditions
            const bool is_horizontal_wire =
                (lyt.is_wire_tile({x, y}) && lyt.fanin_size(lyt.get_node({x, y})) == 1 &&
                 lyt.fanout_size(lyt.get_node({x, y})) == 1 && lyt.has_western_incoming_signal({x, y}) &&
                 lyt.has_eastern_outgoing_signal({x, y}));

            if (!(is_horizontal_wire || lyt.is_empty_tile({x, y})))
            {
                found_column = false;
            }
        }
        if (found_column)
        {
            columns_to_delete.push_back(x);
        }
    }

    delete_rows_and_columns(lyt, rows_to_delete, columns_to_delete);
}

/**
 * This struct is used to hold information about an update to the layout, where a PO tile
 * is moved from its old coordinate to a new coordinate and connected to its new child node.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
struct output_update
{
    /**
     * The old coordinate of the PO.
     */
    tile<Lyt> old_coordinate;

    /**
     * The new coordinate of the PO.
     */
    tile<Lyt> new_coordinate;

    /**
     * The coordinate of the child node (after the update).
     *
     * This is the coordinate of the new child (i.e. incoming signal) after the update.
     */
    tile<Lyt> child_coordinate;

    /**
     * Construct a new Update object.
     *
     * @param old_coord The old coordinate of the PO.
     * @param new_coord The new coordinate of the PO.
     * @param child_coord The coordinate of the child node (after the update).
     */
    output_update(const tile<Lyt>& old_coord, const tile<Lyt>& new_coord, const tile<Lyt>& child_coord) :
            old_coordinate(old_coord),
            new_coordinate(new_coord),
            child_coordinate(child_coord)
    {}
};

/**
 * Utility function that traces back all output nodes and calculate optimal positions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 */
template <typename Lyt>
void optimize_output_positions(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "Clocking scheme is not 2DDWave" << std::endl;
        return;
    }

    // get path from output to preceding gate
    std::vector<layout_coordinate_path<Lyt>> paths{};
    paths.reserve(lyt.num_pos());

    lyt.foreach_po(
        [&lyt, &paths](const auto& po)
        {
            tile<Lyt>                   po_tile = lyt.get_tile(lyt.get_node(po));
            layout_coordinate_path<Lyt> route{};

            // trace back outputs
            lyt.foreach_fanin(lyt.get_node(po_tile),
                              [&lyt, &route, &po_tile](const auto& fin)
                              {
                                  auto fanin = static_cast<tile<Lyt>>(fin);

                                  route.insert(route.begin(), po_tile);
                                  route.insert(route.begin(), fanin);

                                  while (lyt.is_wire_tile(fanin) &&
                                         lyt.fanout_size(lyt.get_node(fanin)) == 1  // -> Wire with single fanout
                                         && !lyt.is_pi_tile(fanin))  // Can't go further back than until a PI
                                  {
                                      fanin = lyt.incoming_data_flow(fanin).front();
                                      route.insert(route.begin(), fanin);
                                  }
                              });

            paths.push_back(route);

            if (route.size() != 2)  // Output is directly connected to a gate already
            {
                lyt.move_node(lyt.get_node(po), po_tile, {});
            }
        });

    // calculate bounding box around gates without outputs
    const auto min_x =
        std::max_element(paths.begin(), paths.end(), [](const auto& a, const auto& b) { return a[1].x < b[1].x; })
            ->at(1)
            .x;
    const auto min_y =
        std::max_element(paths.begin(), paths.end(), [](const auto& a, const auto& b) { return a[1].y < b[1].y; })
            ->at(1)
            .y;

    std::vector<output_update<Lyt>> output_updates{};
    std::vector<tile<Lyt>>          cleared{};

    // move output along its wiring until it lies on the bounding box
    for (const auto& route : paths)
    {
        auto dangling = tile<Lyt>{};
        auto new_pos  = tile<Lyt>{};
        bool moved    = false;

        for (const auto& tile : route)
        {
            if ((tile.x < min_x && tile.y < min_y))
            {
                dangling = tile;
            }
            else if (!lyt.is_po_tile(tile) && lyt.is_wire_tile(tile) && lyt.fanout_size(lyt.get_node(tile)) != 2)
            {
                lyt.clear_tile(tile);
                cleared.emplace_back(tile);
                if (new_pos.is_dead())
                {
                    new_pos = tile;
                }
            }
            else if (!dangling.is_dead())
            {
                if (!new_pos.is_dead())
                {
                    new_pos = {new_pos.x, new_pos.y, 0};
                    output_update<Lyt> new_update(tile, new_pos, dangling);
                    output_updates.emplace_back(new_update);
                }
                moved = true;
            }
        }
        if (!moved)
        {
            new_pos = {route[1].x, route[1].y, 0};
            output_update<Lyt> new_update(route.back(), new_pos, route.front());
            output_updates.emplace_back(new_update);
        }
    }

    fix_wires(lyt, cleared);
    for (const auto& [tile, new_pos, dangling] : output_updates)
    {
        if (lyt.is_empty_tile(new_pos) || (tile == new_pos))
        {
            lyt.move_node(lyt.get_node(tile), new_pos, {lyt.make_signal(lyt.get_node(dangling))});
        }
        else
        {
            lyt.create_buf(lyt.make_signal(lyt.get_node(dangling)), {new_pos.x, new_pos.y, 1});
            if (new_pos.x == min_x)
            {
                lyt.move_node(lyt.get_node(tile), {new_pos.x + 1, new_pos.y, 0},
                              {lyt.make_signal(lyt.get_node({new_pos.x, new_pos.y, 1}))});
            }
            else
            {
                lyt.move_node(lyt.get_node(tile), {new_pos.x, new_pos.y + 1, 0},
                              {lyt.make_signal(lyt.get_node({new_pos.x, new_pos.y, 1}))});
            }
        }
    }
}

/**
 * This function fixes dead nodes in the layout by moving them to available empty coordinates and back.
 * A dead node is a node in the layout that is placed, but not alive, which can happen during the optimization process.
 * The function finds an empty coordinate in the layout and moves the dead gates to that coordinate, before moving it
 * back to its old location and connecting any fanin signals again.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt Gate-level layout.
 * @param gt Vector containing the coordinates of all gates in the layout.
 */
template <typename Lyt>
void fix_dead_nodes(Lyt& lyt, std::vector<tile<Lyt>>& gt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // Find an empty coordinate to move the gate to.
    auto empty_pos = tile<Lyt>{};

    lyt.foreach_coordinate(
        [&lyt, &empty_pos](const auto& coord)
        {
            if (lyt.is_empty_tile(coord))
            {
                empty_pos = coord;
                return false;
            }
            return true;
        });

    // If an empty coordinate is found, proceed with moving the dead gates.
    if (!empty_pos.is_dead())
    {
        for (const auto& gate : gt)
        {
            if (lyt.is_dead(lyt.get_node(gate)))
            {
                // Collect fanin signals of the dead gate.
                std::vector<mockturtle::signal<Lyt>> signals{};
                signals.reserve(lyt.fanin_size(lyt.get_node(gate)));
                lyt.foreach_fanin(lyt.get_node(gate), [&signals](const auto& fanin) { signals.push_back(fanin); });

                // Move the dead gate to the empty coordinate.
                lyt.move_node(lyt.get_node(gate), empty_pos);
                lyt.clear_tile(gate);

                // Move the fanin signals to the newly moved gate.
                lyt.move_node(lyt.get_node(empty_pos), gate, signals);
            }
        }
    }
}

/**
 * Custom comparison function that sorts gates based on the sum of its coordinates and breaks ties based on the
 * x-coordinate.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param a first gate
 * @param b second gate
 * @return result of the comparison
 */
template <typename Lyt>
bool compare_gates(const tile<Lyt>& a, const tile<Lyt>& b)
{
    return static_cast<bool>(std::make_pair(a.x + a.y, a.x) < std::make_pair(b.x + b.y, b.x));
}

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
 * After moving all gates, this algorithm also checks if excess wiring exists on the layout, i.e., rows that only
 * contain vertical wires or columns that only contain horizontal wires and removes them.
 *
 * As outputs have to lay on the border of a layout for better accessibility, they are also moved to new borders
 * determined based on the location of all other gates.
 *
 * @note This function requires the gate-level layout to be 2DDWave-clocked!
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param lyt 2DDWave-clocked Cartesian gate-level layout to optimize.
 */
template <typename Lyt>
void post_layout_optimization(const Lyt& lyt, post_layout_optimization_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (!lyt.is_clocking_scheme(clock_name::TWODDWAVE))
    {
        std::cout << "[e] the given layout has to be 2DDWave-clocked" << std::endl;
        return;
    }

    post_layout_optimization_stats stats{};

    // measure run time
    {
        const mockturtle::stopwatch stop{stats.time_total};

        // Optimization
        auto layout = obstruction_layout<Lyt>(lyt);

        std::vector<tile<Lyt>> gate_tiles{};
        gate_tiles.reserve(layout.num_gates() + layout.num_pis() - layout.num_pos());

        layout.foreach_node(
            [&layout, &gate_tiles](const auto& node)
            {
                if (const tile<Lyt> tile = layout.get_tile(node); layout.is_inv(node) || layout.is_and(node) ||
                                                                  layout.is_xor(node) || layout.is_fanout(node) ||
                                                                  layout.is_or(node) || layout.is_pi_tile(tile))
                {
                    layout.obstruct_coordinate({tile.x, tile.y, 1});
                    gate_tiles.emplace_back(tile);
                }
            });

        std::sort(gate_tiles.begin(), gate_tiles.end(), detail::compare_gates<Lyt>);

        // sort gates based on diagonal line
        std::sort(gate_tiles.begin(), gate_tiles.end(),
                  [](const auto& a, const auto& b) { return a.x + a.y < b.x + b.y; });

        bool moved_at_least_one_gate = false;

        do {
            moved_at_least_one_gate = false;
            for (auto& gate_tile : gate_tiles)
            {
                const auto try_gate_relocation = detail::improve_gate_location(layout, gate_tile);

                if (std::get<0>(try_gate_relocation))
                {
                    moved_at_least_one_gate = true;
                    gate_tile               = std::get<1>(try_gate_relocation);  // update gate location
                }
            }

            std::sort(gate_tiles.begin(), gate_tiles.end(), detail::compare_gates<Lyt>);
        } while (moved_at_least_one_gate);

        detail::delete_wires(layout);
        detail::optimize_output_positions(layout);
        detail::fix_dead_nodes(layout, gate_tiles);

        // calculate bounding box
        const auto bounding_box            = bounding_box_2d(layout);
        const auto optimized_layout_width  = bounding_box.get_x_size();
        const auto optimized_layout_height = bounding_box.get_y_size();

        layout.resize({optimized_layout_width, optimized_layout_height, layout.z()});
    }

    if (pst != nullptr)
    {
        *pst = stats;
    }
}

}  // namespace fiction

#endif  // FICTION_POST_LAYOUT_OPTIMIZATION_HPP
