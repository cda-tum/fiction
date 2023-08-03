//
// Created by simon on 14.06.23.
//

#ifndef FICTION_OPTIMIZATION_HPP
#define FICTION_OPTIMIZATION_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fiction
{

namespace detail
{

/**
 * Utility function to move wires that cross over empty tiles down one layer. This can happen if the wiring of a gate is
 * deleted.
 *
 * @tparam Lyt Gate-level layout type.
 *
 * @param lyt Gate-level layout.
 * @param deleted_coords Tiles that got deleted.
 *
 * @note This function requires the layout to be a gate-level layout and a Cartesian layout.
 */
template <typename Lyt>
void fix_wires(Lyt& lyt, const std::vector<coordinate<Lyt>>& deleted_coords) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    std::unordered_set<coordinate<Lyt>> moved_tiles{};
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
 * @brief A data structure representing the fan-in and fan-out connections of a gate in a layout.
 *
 * This struct stores information about the fan-in and fan-out connections of a gate in a layout.
 * These fan-in and fan-outs are the preceding and succeeding gates in the logic network.
 * It contains vectors for fan-ins, fan-outs, and temporary coordinates to clear before routing.
 * Additionally, it includes layout coordinate paths for routing signals between the gate and its fan-in/fan-out
 * connections.
 *
 * @tparam Lyt Gate-level layout type.
 */
template <typename Lyt>
struct fanin_fanout_data
{
    /**
     * @brief Vector storing the coordinates of fan-in connections to the gate.
     *
     * This vector holds the layout coordinates of all fan-in connections to the gate.
     */
    std::vector<coordinate<Lyt>> fanins;

    /**
     * @brief Vector storing the coordinates of fan-out connections from the gate.
     *
     * This vector holds the layout coordinates of all fan-out connections from the gate.
     */
    std::vector<coordinate<Lyt>> fanouts;

    /**
     * @brief Vector storing temporary coordinates to clear before moving the gate.
     *
     * During the gate relocation process, this vector holds temporary layout coordinates that need to be cleared or
     * reset.
     */
    std::vector<coordinate<Lyt>> to_clear;

    /**
     * @brief Layout coordinate path for routing signals from the first fan-in to the gate.
     *
     * This layout_coordinate_path object represents the path for routing signals from the first fan-in
     * to the gate within the layout.
     */
    layout_coordinate_path<Lyt> route_fanin_1_to_gate;

    /**
     * @brief Layout coordinate path for routing signals from the second fan-in to the gate.
     *
     * This layout_coordinate_path object represents the path for routing signals from the second fan-in
     * to the gate within the layout.
     */
    layout_coordinate_path<Lyt> route_fanin_2_to_gate;

    /**
     * @brief Layout coordinate path for routing signals from the gate to the first fan-out.
     *
     * This layout_coordinate_path object represents the path for routing signals from the gate to
     * the first fan-out within the layout.
     */
    layout_coordinate_path<Lyt> route_gate_to_fanout_1;

    /**
     * @brief Layout coordinate path for routing signals from the gate to the second fan-out.
     *
     * This layout_coordinate_path object represents the path for routing signals from the gate to
     * the second fan-out within the layout.
     */
    layout_coordinate_path<Lyt> route_gate_to_fanout_2;
};

/**
 * @brief Adds a fanin coordinate to the respective route.
 *
 * This helper function is used to add a fanin coordinate to the appropriate route
 * based on whether it belongs to the the route from the first or second fanin to the gate.
 *
 * @tparam Lyt Gate-level layout type.
 *
 * @param fanin The fanin coordinate to be added to the route.
 * @param isFirstFanin A boolean indicating whether this is part of the route from the first fanin to the gate.
 * @param ffd Reference to the fanin_fanout_data structure containing the routes.
 */
template <typename Lyt>
void addFaninToRoute(const coordinate<Lyt>& fanin, bool isFirstFanin, fanin_fanout_data<Lyt>& ffd)
{
    if (isFirstFanin)
    {
        ffd.route_fanin_1_to_gate.insert(ffd.route_fanin_1_to_gate.cbegin(), fanin);
    }
    else
    {
        ffd.route_fanin_2_to_gate.insert(ffd.route_fanin_2_to_gate.cbegin(), fanin);
    }
}

/**
 * @brief Adds a fanout coordinate to the respective route.
 *
 * This helper function is used to add a fanout coordinate to the appropriate route
 * based on whether it belongs to the the route from the gate to the first or second fanout.
 *
 * @tparam Lyt Gate-level layout type.
 *
 * @param fanout The fanout coordinate to be added to the route.
 * @param isFirstFanout A boolean indicating whether it belongs to the route from the gate to the first fanout.
 * @param ffd Reference to the fanin_fanout_data structure containing the routes.
 */
template <typename Lyt>
void addFanoutToRoute(const coordinate<Lyt>& fanout, bool isFirstFanout, fanin_fanout_data<Lyt>& ffd)
{
    if (isFirstFanout)
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
 * @param lyt Gate-level layout.
 * @param op coordinate of the gate to be moved.
 * @return fanin and fanout gates, wires to be deleted and old routing paths.
 *
 * @note This function requires the layout to be a gate-level layout and a Cartesian layout.
 */
template <typename Lyt>
[[nodiscard]] fanin_fanout_data<Lyt> get_fanin_and_fanouts(const Lyt& lyt, const coordinate<Lyt>& op) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    fanin_fanout_data<Lyt> ffd{};

    auto fanin1  = coordinate<Lyt>{};
    auto fanin2  = coordinate<Lyt>{};
    auto fanout1 = coordinate<Lyt>{};
    auto fanout2 = coordinate<Lyt>{};

    std::unordered_set<coordinate<Lyt>> fanins_set{};
    fanins_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);
    std::unordered_set<coordinate<Lyt>> fanouts_set{};
    fanouts_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);

    // TODO whenever you have a for loop like this, it would be better to refactor it
    // TODO using lyt.foreach_fanin(op) for performance reasons. Same for fanouts.
    lyt.foreach_fanin(lyt.get_node(op),
                      [&](const auto& fin)
                      {
                          coordinate<Lyt> fanin = static_cast<tile<Lyt>>(fin);
                          if (fanins_set.find(fanin) == fanins_set.end())
                          {

                              // Add fanin to the respective route
                              addFaninToRoute(op, fanins_set.empty(), ffd);
                              addFaninToRoute(fanin, fanins_set.empty(), ffd);

                              // Continue until gate or primary input (PI) is found
                              while (lyt.is_wire_tile(fanin) && lyt.fanout_size(lyt.get_node(fanin)) == 1 &&
                                     !lyt.is_pi_tile(fanin))
                              {
                                  ffd.to_clear.push_back(fanin);
                                  fanin = lyt.incoming_data_flow(fanin).front();

                                  // Add fanin to the respective route
                                  addFaninToRoute(fanin, fanins_set.empty(), ffd);
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
                       [&](const auto& fout)
                       {
                           coordinate<Lyt> fanout = lyt.get_tile(fout);

                           if (fanouts_set.find(fanout) == fanouts_set.end())
                           {

                               // Add fanout to the respective route
                               addFanoutToRoute(op, fanouts_set.empty(), ffd);
                               addFanoutToRoute(fanout, fanouts_set.empty(), ffd);

                               // Continue until gate or primary output (PO) is found
                               while (lyt.is_wire_tile(fanout) && lyt.fanout_size(lyt.get_node(fanout)) != 0 &&
                                      lyt.fanout_size(lyt.get_node(fanout)) != 2)
                               {
                                   ffd.to_clear.push_back(fanout);
                                   fanout = lyt.outgoing_data_flow(fanout).front();

                                   // Add fanout to the respective route
                                   addFanoutToRoute(fanout, fanouts_set.empty(), ffd);
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
 * @brief Gets a path between two coordinates and obstructs the tiles along the path.
 *
 * This helper function computes a path between two coordinates using the A* algorithm.
 * It then obstructs the tiles along the path in the given layout.
 *
 * @tparam Lyt Gate-level layout.
 *
 * @param lyt Reference to the layout.
 * @param start The starting coordinate of the path.
 * @param end The ending coordinate of the path.
 * @param distance The distance functor used by the A* algorithm.
 * @param cost_func The cost functor used by the A* algorithm.
 * @param params The A* search parameters.
 *
 * @return The computed path as a sequence of coordinates in the layout.
 */
template <typename Lyt>
layout_coordinate_path<Lyt> getPathAndObstruct(Lyt& lyt, const coordinate<Lyt>& start, const coordinate<Lyt>& end,
                                               const twoddwave_distance_functor<Lyt, uint64_t>& distance,
                                               const unit_cost_functor<Lyt, uint8_t>&           cost_func,
                                               const a_star_params&                             params)
{
    layout_coordinate_path<Lyt> path =
        a_star<layout_coordinate_path<Lyt>>(lyt, {start, end}, distance, cost_func, params);

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
 * @param old_pos Old position of the gate to be moved.
 * @param width Width of the gate-level layout.
 * @param height Height of the gate-level layout.
 * @return Flag that indicates if gate was moved successfully and the new coordinate of the moved gate.
 *
 * @note This function requires the layout to be a gate-level layout, a Cartesian layout, 2DDWave-clocked and implement
 * the obstruction interface.
 */
template <typename Lyt>
[[nodiscard]] std::tuple<bool, coordinate<Lyt>>
improve_gate_location(const coordinate<Lyt>& old_pos, Lyt& lyt, const uint64_t width, const uint64_t height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");
    static_assert(has_is_obstructed_coordinate_v<Lyt>, "Lyt is not an obstruction layout");

    const a_star_params params{true};

    const auto& [fanins, fanouts, to_clear, r1, r2, r3, r4] = get_fanin_and_fanouts(lyt, old_pos);

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

    auto new_pos = coordinate<Lyt>{};

    // if gate is directly connected to one of its fanins, no improvement is possible
    for (const auto& fanin : fanins)
    {
        for (const auto& i : lyt.incoming_data_flow(old_pos))
        {
            if (i == fanin)
            {
                return std::make_tuple(false, new_pos);
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
                          [&](const auto& i)
                          {
                              coordinate<Lyt> fout = static_cast<tile<Lyt>>(i);
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

    bool            moved_gate             = false;
    coordinate<Lyt> current_pos            = old_pos;
    bool            improved_gate_location = false;
    // iterate over layout diagonally
    for (uint64_t k = 0; k < width + height + 1; k++)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const uint64_t y = k - x;

            if (moved_gate)
            {
                break;
            }

            // only check better positions
            if (height >= y && y >= min_y && width >= x && x >= min_x && ((x + y) <= max_diagonal) &&
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

                    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                    using cost = unit_cost_functor<Lyt, uint8_t>;

                    // get paths for fanins and fanouts
                    layout_coordinate_path<Lyt> path1, path2, path3, path4;
                    // Get paths for fanins and fanouts
                    if (!fanins.empty())
                    {
                        path1 = getPathAndObstruct<Lyt>(lyt, fanins[0], new_pos, dist(), cost(), params);
                    }

                    if (fanins.size() == 2)
                    {
                        path2 = getPathAndObstruct(lyt, fanins[1], new_pos, dist(), cost(), params);
                    }

                    if (!fanouts.empty())
                    {
                        path3 = getPathAndObstruct(lyt, new_pos, fanouts[0], dist(), cost(), params);
                    }

                    if (fanouts.size() == 2)
                    {
                        path4 = getPathAndObstruct(lyt, new_pos, fanouts[1], dist(), cost(), params);
                    }

                    // if possible routing was found, it will be applied
                    if (!(fanins.size() > 0 && path1.empty()) && !(fanins.size() == 2 && path2.empty()) &&
                        !(fanouts.size() > 0 && path3.empty()) && !(fanouts.size() == 2 && path4.empty()))
                    {
                        for (const auto& path : {path1, path2, path3, path4})
                        {
                            if (!path.empty())
                            {
                                route_path(lyt, path);
                                for (auto tile : path)
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
                                              lyt.make_signal(lyt.get_node(path1[path1.size() - 2])),
                                              lyt.make_signal(lyt.get_node(path2[path2.size() - 2])),
                                          });
                        }
                        else if (fanins.size() == 1)
                        {
                            lyt.move_node(lyt.get_node(new_pos), new_pos,
                                          {lyt.make_signal(lyt.get_node(path1[path1.size() - 2]))});
                        }

                        // update children of fanouts
                        for (const auto& fanout : fanouts)
                        {
                            std::vector<mockturtle::signal<Lyt>> signals{};
                            signals.reserve(lyt.fanin_size(lyt.get_node(fanout)));

                            lyt.foreach_fanin(lyt.get_node(fanout),
                                              [&](const auto& i)
                                              {
                                                  coordinate<Lyt> fout = static_cast<tile<Lyt>>(i);
                                                  signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                                              });

                            lyt.move_node(lyt.get_node(fanout), fanout, signals);
                        }
                    }
                    // if no routing was found, remove added obstructions
                    else
                    {
                        for (const auto& path : {path1, path2, path3, path4})
                        {
                            if (!path.empty())
                            {
                                for (const auto& tile : path)
                                {
                                    lyt.clear_obstructed_coordinate(tile);
                                }
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
    // if no better coordinate<Lyt>was found, restore old wiring
    if (!moved_gate)
    {
        lyt.move_node(lyt.get_node(current_pos), old_pos, {});

        for (const auto& r : {r1, r2, r3, r4})
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
                          [&](const auto& i)
                          {
                              coordinate<Lyt> fanin = static_cast<tile<Lyt>>(i);
                              signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
                          });

        lyt.move_node(lyt.get_node(old_pos), old_pos, signals);

        // update children oóf fanouts
        for (const auto& fanout : fanouts)
        {
            std::vector<mockturtle::signal<Lyt>> fout_signals{};
            fout_signals.reserve(lyt.fanin_size(lyt.get_node(fanout)));

            lyt.foreach_fanin(lyt.get_node(fanout),
                              [&](const auto& i)
                              {
                                  coordinate<Lyt> fout = static_cast<tile<Lyt>>(i);
                                  fout_signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                              });

            lyt.move_node(lyt.get_node(fanout), fanout, fout_signals);
        }
    }

    return std::make_tuple(improved_gate_location, new_pos);
}
/**
 * @brief A data structure representing incoming signals' coordinates in a 3-dimensional layout.
 *
 * This struct stores the coordinates of incoming signals in a 3-dimensional layout, where each signal is associated
 * with a specific depth level, row index, and column index.
 *
 * @tparam Lyt Gate-level layout.
 */
template <typename Lyt>
struct IncomingSignalsMap
{
  private:
    using coord_vec_t = std::vector<coordinate<Lyt>>;

  public:
    /**
     * @brief An unordered map associating depth levels with vectors of layout coordinates for incoming signals.
     *
     * The 'depth_map' unordered map holds depth levels as keys (integer values) and maps them to vectors of layout
     * coordinates. Each depth level represents a specific level in the layout, and the associated vector contains the
     * coordinates of incoming signals located at that depth level.
     */
    std::unordered_map<uint64_t, coord_vec_t> depth{};
};

/**
 * @brief A data structure representing incoming signals' coordinates in a row of a 3-dimensional layout.
 *
 * This struct stores the coordinates of incoming signals in a row of a 3-dimensional layout, where each signal is
 * associated with a specific depth level and column index.
 *
 * @tparam Lyt Gate-level layout.
 */
template <typename Lyt>
struct RowIncomingSignalsMap
{
  private:
    using DepthMapType = IncomingSignalsMap<Lyt>;
    using InnerMapType = std::unordered_map<uint64_t, DepthMapType>;

  public:
    /**
     * @brief An unordered map associating row indices with DepthMapType data structures.
     *
     * The 'row_map' unordered map holds row indices as keys (integer values) and maps them to DepthMapType data
     * structures. Each row index represents a specific row in the layout, and the associated DepthMapType stores the
     * coordinates of incoming signals at different depth levels within that row.
     */
    InnerMapType row{};
};

/**
 * @brief A data structure representing incoming signals' coordinates in a column of a 3-dimensional layout.
 *
 * This struct stores the coordinates of incoming signals in a column of a 3-dimensional layout, where each signal is
 * associated with a specific row index and depth level.
 *
 * @tparam Lyt Gate-level layout.
 */
template <typename Lyt>
struct ColumnIncomingSignalsMap
{
  private:
    using RowIncomingSignalsMapType = RowIncomingSignalsMap<Lyt>;

  public:
    /**
     * @brief An unordered map associating column indices with RowIncomingSignalsMapType data structures.
     *
     * The 'column_map' unordered map holds column indices as keys (integer values) and maps them to
     * RowIncomingSignalsMapType data structures. Each column index represents a specific column in the layout, and the
     * associated RowIncomingSignalsMapType stores the coordinates of incoming signals at different row and depth levels
     * within that column.
     */
    std::unordered_map<uint64_t, RowIncomingSignalsMapType> column{};
};

/**
 * Utility function that deletes a row in the layout by moving all southern gates up one positions.
 *
 * @param lyt Gate-level layout.
 * @param row_idx Row to be deleted.
 * @param width Width of the gate-level layout.
 * @param height Height of the gate-level layout.
 *
 * @note This function requires the layout to be a gate-level layout, a Cartesian layout and 2DDWave-clocked.
 */
template <typename Lyt>
void delete_row(Lyt& lyt, const uint64_t row_idx, const uint64_t width, const uint64_t height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    // create a map that stores fanins of each coordinate
    ColumnIncomingSignalsMap<Lyt> fanins{};

    for (uint64_t y = row_idx; y <= height; ++y)
    {
        for (uint64_t x = 0; x <= width; ++x)
        {
            for (uint64_t z = 0; z <= lyt.z(); ++z)
            {
                if (y == row_idx && lyt.incoming_data_flow({x, y, z}).size() > 0)
                {
                    const auto fanin_row      = lyt.incoming_data_flow({x, y, z}).front();
                    const auto fanin_next_row = lyt.incoming_data_flow({x, y + 1, z});

                    for (const auto& fanin : fanin_next_row)
                    {
                        if (fanin.y == y)
                        {
                            fanins.column[x].row[y].depth[z].push_back({fanin_row.x, fanin_row.y + 1, fanin_row.z});
                        }
                        else
                        {
                            fanins.column[x].row[y].depth[z].push_back(fanin);
                        }
                    }
                }
                else
                {
                    fanins.column[x].row[y].depth[z] = lyt.incoming_data_flow({x, y + 1, z});
                }
            }
        }

        // iterate through row and move gates
        for (uint64_t x = 0; x <= width; ++x)
        {
            for (uint64_t z = 0; z <= lyt.z(); ++z)
            {
                if (const coordinate<Lyt> old_pos{x, y, z}; !lyt.is_empty_tile(old_pos))
                {
                    // delete row
                    if (y == row_idx)
                    {
                        lyt.clear_tile(old_pos);
                    }

                    // move up one position
                    else
                    {
                        const coordinate<Lyt> new_pos{x, y - 1, z};

                        std::vector<mockturtle::signal<Lyt>> fins{};
                        fins.reserve(fanins.column[x].row[y - 1].depth[z].size());

                        for (const auto& fanin : fanins.column[x].row[y - 1].depth[z])
                        {
                            fins.push_back(lyt.make_signal(lyt.get_node({fanin.x, fanin.y - 1, fanin.z})));
                        }

                        lyt.move_node(lyt.get_node(old_pos), new_pos, fins);
                    }
                }
            }
        }
    }
}

/**
 * Utility function that deletes a column in the layout by moving all eastern gates to the left by one positions.
 *
 * @param lyt Gate-level layout.
 * @param column_idx Column to be deleted.
 * @param width Width of the gate-level layout.
 * @param height Height of the gate-level layout.
 *
 * @note This function requires the layout to be a gate-level layout, a Cartesian layout and 2DDWave-clocked.
 */
template <typename Lyt>
void delete_column(Lyt& lyt, const uint64_t column_idx, const uint64_t width, const uint64_t height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    // create a map that stores fanins of each coordinate
    ColumnIncomingSignalsMap<Lyt> fanins{};

    for (uint64_t x = column_idx; x <= width; ++x)
    {
        for (uint64_t y = 0; y <= height; ++y)
        {
            for (uint64_t z = 0; z <= lyt.z(); ++z)
            {
                if (x == column_idx && lyt.fanin_size(lyt.get_node({x, y, z})) > 0)
                {
                    const auto fanin_column      = lyt.incoming_data_flow({x, y, z}).front();
                    const auto fanin_next_column = lyt.incoming_data_flow({x + 1, y, z});

                    for (const auto& fanin : fanin_next_column)
                    {
                        if (fanin.x == x)
                        {
                            fanins.column[x].row[y].depth[z].push_back(
                                {fanin_column.x + 1, fanin_column.y, fanin_column.z});
                        }
                        else
                        {
                            fanins.column[x].row[y].depth[z].push_back(fanin);
                        }
                    }
                }
                else
                {
                    fanins.column[x].row[y].depth[z] = lyt.incoming_data_flow({x + 1, y, z});
                }
            }
        }

        // iterate through column and move gates
        for (uint64_t y = 0; y <= height; ++y)
        {
            for (uint64_t z = 0; z <= lyt.z(); ++z)
            {
                if (const coordinate<Lyt> old_pos = {x, y, z}; !lyt.is_empty_tile(old_pos))
                {
                    // delete row
                    if (x == column_idx)
                    {
                        lyt.clear_tile(old_pos);
                    }

                    // move left one position
                    else
                    {
                        const coordinate<Lyt> new_pos = {x - 1, y, z};

                        std::vector<mockturtle::signal<Lyt>> fins{};
                        fins.reserve(fanins.column[x - 1].row[y].depth[z].size());

                        for (const auto& fanin : fanins.column[x - 1].row[y].depth[z])
                        {
                            fins.push_back(lyt.make_signal(lyt.get_node({fanin.x - 1, fanin.y, fanin.z})));
                        }

                        lyt.move_node(lyt.get_node(old_pos), new_pos, fins);
                    }
                }
            }
        }
    }
}

/**
 * Utility function that deletes rows that only contain vertically connected wires.
 *
 * @param lyt Gate-level layout.
 * @param width Width of the gate-level layout.
 * @param height Height of the gate-level layout.
 *
 * @note This function requires the layout to be a gate-level layout, a Cartesian layout and 2DDWave-clocked.
 */
template <typename Lyt>
void delete_wires(Lyt& lyt, const uint64_t width, const uint64_t height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    for (int64_t y = static_cast<int64_t>(height); y >= 0; --y)
    {
        bool found_row = true;
        for (int64_t x = static_cast<int64_t>(width); x >= 0; --x)
        {
            // Check if the row can be deleted based on certain conditions
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
            delete_row(lyt, static_cast<uint64_t>(y), width, height);
        }
    }

    for (int64_t x = static_cast<int64_t>(width); x >= 0; --x)
    {
        bool found_column = true;
        for (int64_t y = static_cast<int64_t>(height); y >= 0; --y)
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
            delete_column(lyt, static_cast<uint64_t>(x), width, height);
        }
    }
}

/**
 * @brief A struct representing an output update to the layout.
 *
 * This struct is used to hold information about an update to the layout, where a PO tile
 * is moved from its old coordinate to a new coordinate and connected to its new child node.
 *
 * @tparam Lyt Gate-level layout.
 */
template <typename Lyt>
struct Update
{
    /**
     * @brief The old coordinate of the PO.
     */
    coordinate<Lyt> oldCoordinate;

    /**
     * @brief The new coordinate of the PO.
     */
    coordinate<Lyt> newCoordinate;

    /**
     * @brief The coordinate of the child node (after the update).
     *
     * This is the coordinate of the new child (i.e. incoming signal) after the update.
     */
    coordinate<Lyt> childCoordinate;

    /**
     * @brief Construct a new Update object.
     *
     * @param oldCoord The old coordinate of the PO.
     * @param newCoord The new coordinate of the PO.
     * @param childCoord The coordinate of the child node (after the update).
     */
    Update(const coordinate<Lyt>& oldCoord, const coordinate<Lyt>& newCoord, const coordinate<Lyt>& childCoord) :
            oldCoordinate(oldCoord),
            newCoordinate(newCoord),
            childCoordinate(childCoord)
    {}
};

/**
 * Utility function that traces back all output nodes and calculate optimal positions.
 *
 * @param lyt Gate-level layout.
 *
 * @note This function requires the layout to be a gate-level layout, a Cartesian layout and 2DDWave-clocked.
 */
template <typename Lyt>
void optimize_output_positions(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    // get all outputs
    std::vector<coordinate<Lyt>> pos{};
    pos.reserve(lyt.num_pos());
    lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });

    // get path from output to preceding gate
    std::vector<layout_coordinate_path<Lyt>> paths{};
    paths.reserve(pos.size());

    for (const auto& po : pos)
    {
        layout_coordinate_path<Lyt> route{};

        // trace back outputs
        lyt.foreach_fanin(lyt.get_node(po),
                          [&](const auto& fin)
                          {
                              coordinate<Lyt> fanin = static_cast<tile<Lyt>>(fin);

                              route.insert(route.begin(), po);
                              route.insert(route.begin(), fanin);

                              while (lyt.is_wire_tile(fanin) &&
                                     lyt.fanout_size(lyt.get_node(fanin)) == 1  // -> Wire with single fanout
                                     && !lyt.is_pi_tile(fanin))                 // Can't go further back than until a PI
                              {
                                  fanin = lyt.incoming_data_flow(fanin).front();
                                  route.insert(route.begin(), fanin);
                              }
                          });

        paths.push_back(route);

        if (route.size() != 2)  // Output is directly connected to a gate already
        {
            lyt.move_node(lyt.get_node(po), po, {});
        }
    }

    // calculate bounding box around gates without outputs
    const auto min_x =
        std::max_element(paths.begin(), paths.end(), [](const auto& a, const auto& b) { return a[1].x < b[1].x; })
            ->at(1)
            .x;
    const auto min_y =
        std::max_element(paths.begin(), paths.end(), [](const auto& a, const auto& b) { return a[1].y < b[1].y; })
            ->at(1)
            .y;

    std::vector<Update<Lyt>>     updates;
    std::vector<coordinate<Lyt>> cleared{};

    // move output along its wiring until it lies on the bounding box
    for (const auto& route : paths)
    {
        auto dangling = coordinate<Lyt>{};
        auto new_pos  = coordinate<Lyt>{};
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
                    Update<Lyt> update(tile, new_pos, dangling);
                    updates.emplace_back(update);
                }
                moved = true;
            }
        }
        if (!moved)
        {
            new_pos = {route[1].x, route[1].y, 0};
            Update<Lyt> update(route.back(), new_pos, route.front());
            updates.emplace_back(update);
        }
    }

    fix_wires(lyt, cleared);
    for (const auto& [tile, new_pos, dangling] : updates)
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
 * @brief Fixes dead nodes in the layout by moving them to empty coordinates and back.
 *
 * This function fixes dead nodes in the layout by moving them to available empty coordinates and back.
 * A dead node is a node in the layout that is placed, but not alive, which can happen during the optimization process.
 * The function finds an empty coordinate in the layout and moves the dead gates to that coordinate, before moving it
 * back to its old location and connecting any fanin signals again.
 *
 * @param lyt Gate-level layout.
 * @param gt Vector containing the coordinates of all gates in the layout.
 *
 * @note This function requires the layout to be a gate-level layout and a Cartesian layout.
 */
template <typename Lyt>
void fix_dead_nodes(Lyt& lyt, std::vector<coordinate<Lyt>>& gt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    // Find an empty coordinate to move the gate to.
    auto empty_pos = coordinate<Lyt>{};

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

}  // namespace detail

/**
 * Optimization algorithm that can be used to reduce the layout area of sub-optimal physical design created by
 * heuristics or machine learning. This optimization utilizes the distinct characteristics of the 2DDWave clocking
 * scheme, which only allows information flow from top to bottom and left to right, therefore only aforementioned
 * clocking scheme is supported.
 *
 * To reduce the layout area, first, gates are moved up and to left as far as possible, including rerouting. This
 * creates more compact layouts by freeing up space to the right and bottom, as all gates were moved to the top left
 * corner.
 *
 * After moving all gates, this algorithm also checks if excess wiring exists on the layout, i.e., rows that only
 * contain vertical wires or columns that only contain horizontal wires and removes them.
 *
 * As outputs have to lay on the border of a layout for better accessibility, they are also moved to new borders
 * determined based on the location of all other gates.
 *
 * @param lyt Gate-level layout.
 *
 * @note This function requires the layout to be a gate-level layout and a Cartesian layout.
 */
template <typename Lyt>
void post_layout_optimization(const Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    std::cout << "here";
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    // calculate bounding box
    const auto bounding_box_before = bounding_box_2d(lyt);
    const auto width               = bounding_box_before.get_x_size();
    const auto height              = bounding_box_before.get_y_size();

    // Optimization
    auto layout = obstruction_layout<Lyt>(lyt);

    std::vector<coordinate<Lyt>> gates{};
    gates.reserve(layout.num_wires() + layout.num_gates());

    for (auto x = 0; x <= static_cast<int>(width); ++x)
    {
        for (auto y = 0; y <= static_cast<int>(height); ++y)
        {
            if (layout.is_inv(layout.get_node({x, y})) || layout.is_and(layout.get_node({x, y})) ||
                layout.is_xor(layout.get_node({x, y})) || layout.is_fanout(layout.get_node({x, y})) ||
                layout.is_or(layout.get_node({x, y})) || layout.is_pi_tile({x, y}))
            {
                layout.obstruct_coordinate({x, y, 1});
                gates.emplace_back(x, y);
            }
        }
    }

    // sort gates based on diagonal line
    std::sort(gates.begin(), gates.end(), [](const auto& a, const auto& b) { return a.x + a.y < b.x + b.y; });

    bool moved_gates = true;
    int  moved;

    while (moved_gates)
    {
        moved = 0;

        for (auto& gate : gates)
        {
            const auto moved_gate = detail::improve_gate_location(gate, layout, width, height);

            if (std::get<0>(moved_gate))
            {
                moved += 1;
                gate = std::get<1>(moved_gate);  // update gate location
            }
        }

        std::sort(gates.begin(), gates.end(), [](const auto& a, const auto& b) { return a.x + a.y < b.x + b.y; });
        if (moved == 0)
        {
            moved_gates = false;
        }
    }

    detail::delete_wires(layout, width, height);
    detail::optimize_output_positions(layout);
    detail::fix_dead_nodes(layout, gates);

    // calculate bounding box
    const auto bounding_box            = bounding_box_2d(layout);
    const auto optimized_layout_width  = bounding_box.get_x_size();
    const auto optimized_layout_height = bounding_box.get_y_size();

    layout.resize({optimized_layout_width, optimized_layout_height, layout.z()});
}

}  // namespace fiction

#endif  // FICTION_OPTIMIZATION_HPP
