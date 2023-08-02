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
 * @param lyt Gate level layout.
 * @param deleted_coords Tiles that got deleted.
 */
template <typename Lyt>
void check_wires(Lyt& lyt, const std::vector<coordinate<Lyt>>& deleted_coords) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    std::unordered_set<coordinate<Lyt>> moved_tiles{};
    moved_tiles.reserve(deleted_coords.size());
    for (const auto& tile : deleted_coords)
    {
        const auto ground = lyt.below(tile);
        const auto above  = lyt.above(tile);

        if (lyt.is_empty_tile(ground) && lyt.is_wire_tile(above))
        {
            const auto incoming_tile = lyt.incoming_data_flow(above)[0];
            const auto outgoing_tile = lyt.outgoing_data_flow(above)[0];

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

template <typename Lyt>
struct FaninFanoutData
{
    std::vector<coordinate<Lyt>> fanins;
    std::vector<coordinate<Lyt>> fanouts;
    std::vector<coordinate<Lyt>> to_clear;
    layout_coordinate_path<Lyt>  route_fanin_1_to_gate;
    layout_coordinate_path<Lyt>  route_fanin_2_to_gate;
    layout_coordinate_path<Lyt>  route_gate_to_fanout_1;
    layout_coordinate_path<Lyt>  route_gate_to_fanout_2;
};

/**
 * Utility function to trace back fanins and fanouts of a gate. Based on the gate to be moved, this function returns the
 * location of the fanins and fanouts, as well as the wiring in between them. Additionally, all wire tiles between
 * fanins and the gate, as well as between the gate and fanouts are collected for deletion.
 *
 * @param lyt Gate level layout.
 * @param op coordinate<Lyt>of the gate to be moved.
 *
 * @return fanin and fanout gates, wires to be deleted and old routing paths.
 */
template <typename Lyt>
[[nodiscard]] FaninFanoutData<Lyt> get_fanin_and_fanouts(const Lyt& lyt, const coordinate<Lyt>& op) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    FaninFanoutData<Lyt> faninFanoutData;

    auto fanin1  = coordinate<Lyt>{};
    auto fanin2  = coordinate<Lyt>{};
    auto fanout1 = coordinate<Lyt>{};
    auto fanout2 = coordinate<Lyt>{};

    std::unordered_set<coordinate<Lyt>> fanins_set{};
    fanins_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);
    std::unordered_set<coordinate<Lyt>> fanouts_set{};
    fanouts_set.reserve(lyt.num_wires() + lyt.num_gates() - 2);

    // trace back fanins
    for (const auto& fin : lyt.incoming_data_flow(op))
    {
        if (fanins_set.find(fin) == fanins_set.end())
        {
            coordinate<Lyt> fanin = fin;

            // switch between first and second fanin
            if (fanins_set.empty())
            {
                faninFanoutData.route_fanin_1_to_gate.insert(faninFanoutData.route_fanin_1_to_gate.cbegin(), op);
                faninFanoutData.route_fanin_1_to_gate.insert(faninFanoutData.route_fanin_1_to_gate.cbegin(), fanin);
            }
            else
            {
                faninFanoutData.route_fanin_2_to_gate.insert(faninFanoutData.route_fanin_2_to_gate.cbegin(), op);
                faninFanoutData.route_fanin_2_to_gate.insert(faninFanoutData.route_fanin_2_to_gate.cbegin(), fanin);
            }

            // go back till gate or pi is found
            while (lyt.is_wire_tile(fanin) && lyt.fanout_size(lyt.get_node(fanin)) != 2 &&
                   lyt.fanin_size(lyt.get_node(fanin)) != 0)
            {
                faninFanoutData.to_clear.push_back(fanin);
                fanin = lyt.incoming_data_flow(fanin)[0];

                // switch between first and second fanin
                if (fanins_set.empty())
                {
                    faninFanoutData.route_fanin_1_to_gate.insert(faninFanoutData.route_fanin_1_to_gate.begin(), fanin);
                }
                else
                {
                    faninFanoutData.route_fanin_2_to_gate.insert(faninFanoutData.route_fanin_2_to_gate.begin(), fanin);
                }
            }

            // switch between first and second fanin
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
    }
    // same for fanouts
    for (const auto& fout : lyt.outgoing_data_flow(op))
    {
        if (fanouts_set.find(fout) == fanouts_set.end())
        {
            coordinate<Lyt> fanout = fout;

            // switch between first and second fanout
            if (fanouts_set.empty())
            {
                faninFanoutData.route_gate_to_fanout_1.push_back(op);
                faninFanoutData.route_gate_to_fanout_1.push_back(fanout);
            }
            else
            {
                faninFanoutData.route_gate_to_fanout_2.push_back(op);
                faninFanoutData.route_gate_to_fanout_2.push_back(fanout);
            }

            // continue until gate or po is found
            while (lyt.is_wire_tile(fanout) && lyt.fanout_size(lyt.get_node(fanout)) != 0 &&
                   lyt.fanout_size(lyt.get_node(fanout)) != 2)
            {
                faninFanoutData.to_clear.push_back(fanout);
                fanout = lyt.outgoing_data_flow(fanout)[0];

                // switch between first and second fanout
                if (fanouts_set.empty())
                {
                    faninFanoutData.route_gate_to_fanout_1.push_back(fanout);
                }
                else
                {
                    faninFanoutData.route_gate_to_fanout_2.push_back(fanout);
                }
            }

            // switch between first and second fanout
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
    }

    // add fanins and fanouts if existing
    if (!fanin1.is_dead())
    {
        faninFanoutData.fanins.push_back(fanin1);
    }
    if (!fanin2.is_dead())
    {
        faninFanoutData.fanins.push_back(fanin2);
    }
    if (!fanout1.is_dead())
    {
        faninFanoutData.fanouts.push_back(fanout1);
    }
    if (!fanout2.is_dead())
    {
        faninFanoutData.fanouts.push_back(fanout2);
    }

    return faninFanoutData;
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
 * @param width Width of the gate level layout.
 * @param height Height of the gate level layout.
 *
 * @return flag that indicates if gate was moved successfully and the new coordinate of the moved gate.
 */
template <typename Lyt>
[[nodiscard]] std::tuple<bool, coordinate<Lyt>> move_gate(const coordinate<Lyt>& old_pos, Lyt& lyt, const int width,
                                                          const int height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    const a_star_params params{true};

    const auto& [fanins, fanouts, to_clear, r1, r2, r3, r4] = get_fanin_and_fanouts(lyt, old_pos);
    int min_x                                               = 0;
    int min_y                                               = 0;

    // determine minimum coordinates for new placements
    if (!fanins.empty())
    {
        min_x = static_cast<int>(
            std::max_element(fanins.cbegin(), fanins.cend(), [](auto a, auto b) { return a.x < b.x; })->x);
        min_y = static_cast<int>(
            std::max_element(fanins.cbegin(), fanins.cend(), [](auto a, auto b) { return a.y < b.y; })->y);
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
        for (const auto& fout : lyt.incoming_data_flow(fanout))
        {
            if (fout != old_pos)
            {
                fins.push_back(lyt.make_signal(lyt.get_node(fout)));
            }
        }

        lyt.move_node(lyt.get_node(fanout), fanout, fins);
    }

    // remove children of gate to be moved
    lyt.move_node(lyt.get_node(old_pos), old_pos, {});

    // fix wires that cross over empty tiles
    check_wires(lyt, to_clear);

    bool            success     = false;
    coordinate<Lyt> current_pos = old_pos;
    bool            optimized   = false;

    // iterate over layout diagonally
    for (auto k = 0; k < width + height + 1; k++)
    {
        for (auto x = 0; x < k + 1; x++)
        {
            const auto y = k - x;

            if (success)
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
                    layout_coordinate_path<Lyt> path1;
                    if (!fanins.empty())
                    {
                        path1 = a_star<layout_coordinate_path<Lyt>>(lyt, {fanins[0], new_pos}, dist(), cost(), params);
                        for (const auto& tile : path1)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    layout_coordinate_path<Lyt> path2;
                    if (fanins.size() == 2)
                    {
                        path2 = a_star<layout_coordinate_path<Lyt>>(lyt, {fanins[1], new_pos}, dist(), cost(), params);
                        for (const auto& tile : path2)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    layout_coordinate_path<Lyt> path3;
                    if (!fanouts.empty())
                    {
                        path3 = a_star<layout_coordinate_path<Lyt>>(lyt, {new_pos, fanouts[0]}, dist(), cost(), params);
                        for (const auto& tile : path3)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    layout_coordinate_path<Lyt> path4;
                    if (fanouts.size() == 2)
                    {
                        path4 = a_star<layout_coordinate_path<Lyt>>(lyt, {new_pos, fanouts[1]}, dist(), cost(), params);
                        for (const auto& tile : path4)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
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
                        success = true;
                        if (new_pos != old_pos)
                        {
                            optimized = true;
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
                            for (const auto& fout : lyt.incoming_data_flow(fanout))
                            {
                                signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                            }

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
        if (success)
        {
            break;
        }
    }
    // if no better coordinate<Lyt>was found, restore old wiring
    if (!success)
    {
        lyt.move_node(lyt.get_node(current_pos), old_pos, {});

        for (const auto& r : {r1, r2, r3, r4})
        {
            if (!r.empty())
            {
                route_path<Lyt, layout_coordinate_path<Lyt>>(lyt, r);
            }
            for (auto tile : r)
            {
                lyt.obstruct_coordinate(tile);
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
        for (const auto& fanin : lyt.incoming_data_flow(old_pos))
        {
            signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
        }

        lyt.move_node(lyt.get_node(old_pos), old_pos, signals);

        // update children oóf fanouts
        for (const auto& fanout : fanouts)
        {
            std::vector<mockturtle::signal<Lyt>> fout_signals{};
            fout_signals.reserve(lyt.fanin_size(lyt.get_node(fanout)));
            for (const auto& fout : lyt.incoming_data_flow(fanout))
            {
                fout_signals.push_back(lyt.make_signal(lyt.get_node(fout)));
            }

            lyt.move_node(lyt.get_node(fanout), fanout, fout_signals);
        }
    }

    return std::make_tuple(optimized, new_pos);
}

template <typename Lyt>
struct DepthMap
{
    using CoordinateVector = std::vector<coordinate<Lyt>>;
    std::unordered_map<int, CoordinateVector> depthMap;
};

template <typename Lyt>
struct RowMap
{
    using InnerMap = std::unordered_map<int, DepthMap<Lyt>>;
    InnerMap rowMap;
};

template <typename Lyt>
struct ColumnMap
{
    using RowMapType = RowMap<Lyt>;
    std::unordered_map<int, RowMapType> columnMap;
};

/**
 * Utility function that deletes a row in the layout by moving all southern gates up one positions.
 *
 * @param lyt Gate level layout.
 * @param row_idx Row to be deleted.
 * @param width Width of the gate level layout.
 * @param height Height of the gate level layout.
 */
template <typename Lyt>
void delete_row(Lyt& lyt, const int row_idx, const int width, const int height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    // create a map that stores fanins of each coordinate
    ColumnMap<Lyt> fanins;

    for (auto y = row_idx; y <= height; y++)
    {
        for (auto x = 0; x <= width; x++)
        {
            for (auto z = 0; z <= lyt.z(); z++)
            {
                if (y == row_idx && lyt.incoming_data_flow({x, y, z}).size() > 0)
                {
                    const coordinate<Lyt>              fanin_row      = lyt.incoming_data_flow({x, y, z})[0];
                    const std::vector<coordinate<Lyt>> fanin_next_row = lyt.incoming_data_flow({x, y + 1, z});

                    for (const auto& fanin : fanin_next_row)
                    {
                        if (fanin.y == y)
                        {
                            fanins.columnMap[x].rowMap[y].depthMap[z].push_back(
                                coordinate<Lyt>{fanin_row.x, fanin_row.y + 1, fanin_row.z});
                        }
                        else
                        {
                            fanins.columnMap[x].rowMap[y].depthMap[z].push_back(fanin);
                        }
                    }
                }
                else
                {
                    fanins.columnMap[x].rowMap[y].depthMap[z] = lyt.incoming_data_flow({x, y + 1, z});
                }
            }
        }

        // iterate through row and move gates
        for (auto x = 0; x <= width; x++)
        {
            for (auto z = 0; z <= lyt.z(); z++)
            {
                const coordinate<Lyt> old_pos = {x, y, z};
                if (!lyt.is_empty_tile(old_pos))
                {
                    // delete row
                    if (y == row_idx)
                    {
                        lyt.clear_tile(old_pos);
                    }

                    // move up one position
                    else
                    {
                        const coordinate<Lyt> new_pos = {x, y - 1, z};

                        std::vector<mockturtle::signal<Lyt>> fins{};
                        fins.reserve(fanins.columnMap[x].rowMap[y - 1].depthMap[z].size());
                        for (const auto& fanin : fanins.columnMap[x].rowMap[y - 1].depthMap[z])
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
 * @param lyt Gate level layout.
 * @param column_idx Column to be deleted.
 * @param width Width of the gate level layout.
 * @param height Height of the gate level layout.
 */
template <typename Lyt>
void delete_column(Lyt& lyt, const int column_idx, const int width, const int height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    // create a map that stores fanins of each coordinate
    ColumnMap<Lyt> fanins;

    for (auto x = column_idx; x <= width; x++)
    {
        for (auto y = 0; y <= height; y++)
        {
            for (auto z = 0; z <= lyt.z(); z++)
            {
                if (x == column_idx && lyt.incoming_data_flow({x, y, z}).size() > 0)
                {
                    const auto fanin_column      = lyt.incoming_data_flow({x, y, z})[0];
                    const auto fanin_next_column = lyt.incoming_data_flow({x + 1, y, z});

                    for (const auto& fanin : fanin_next_column)
                    {
                        if (fanin.x == x)
                        {
                            fanins.columnMap[x].rowMap[y].depthMap[z].push_back(
                                coordinate<Lyt>{fanin_column.x + 1, fanin_column.y, fanin_column.z});
                        }
                        else
                        {
                            fanins.columnMap[x].rowMap[y].depthMap[z].push_back(fanin);
                        }
                    }
                }
                else
                {
                    fanins.columnMap[x].rowMap[y].depthMap[z] = lyt.incoming_data_flow({x + 1, y, z});
                }
            }
        }

        // iterate through column and move gates
        for (auto y = 0; y <= height; y++)
        {
            for (auto z = 0; z <= lyt.z(); z++)
            {
                const coordinate<Lyt> old_pos = {x, y, z};
                if (!lyt.is_empty_tile(old_pos))
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
                        fins.reserve(fanins.columnMap[x - 1].rowMap[y].depthMap[z].size());
                        for (const auto& fanin : fanins.columnMap[x - 1].rowMap[y].depthMap[z])
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
 * @param lyt Gate level layout.
 * @param width Width of the gate level layout.
 * @param height Height of the gate level layout.
 */
template <typename Lyt>
void delete_wires(Lyt& lyt, const int width, const int height) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    for (auto y = height; y >= 0; y--)
    {
        bool found_row = true;
        for (auto x = width; x >= 0; x--)
        {
            // if row has gates or bent/ horizontal wires, row cannot be deleted
            if (!((lyt.is_wire_tile({x, y}) && lyt.fanin_size(lyt.get_node({x, y})) == 1 &&
                   lyt.fanout_size(lyt.get_node({x, y})) == 1 && lyt.has_northern_incoming_signal({x, y}) &&
                   lyt.has_southern_outgoing_signal({x, y})) ||
                  lyt.is_empty_tile({x, y})))
            {
                found_row = false;
            }
        }
        if (found_row)
        {
            std::cout << "Row " << y << " can be deleted" << '\n';
            delete_row(lyt, y, width, height);
        }
    }

    for (auto x = width; x >= 0; x--)
    {
        bool found_column = true;
        for (auto y = height; y >= 0; y--)
        {
            // if column has gates or bent/ horizontal wires, column cannot be deleted
            if (!((lyt.is_wire_tile({x, y}) && lyt.fanin_size(lyt.get_node({x, y})) == 1 &&
                   lyt.fanout_size(lyt.get_node({x, y})) == 1 && lyt.has_western_incoming_signal({x, y}) &&
                   lyt.has_eastern_outgoing_signal({x, y})) ||
                  lyt.is_empty_tile({x, y})))
            {
                found_column = false;
            }
        }
        if (found_column)
        {
            std::cout << "Column " << x << " can be deleted" << '\n';
            delete_column(lyt, x, width, height);
        }
    }
}

/**
 * Utility function that traces back all output nodes and calculate optimal positions.
 *
 * @param lyt Gate level layout.
 */
template <typename Lyt>
void optimize_output(Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

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
        for (const auto& fin : lyt.incoming_data_flow(po))
        {
            coordinate<Lyt> fanin = fin;

            route.insert(route.begin(), po);
            route.insert(route.begin(), fanin);

            while (lyt.is_wire_tile(fanin) && lyt.fanout_size(lyt.get_node(fanin)) != 2 &&
                   lyt.fanin_size(lyt.get_node(fanin)) != 0)
            {
                fanin = lyt.incoming_data_flow(fanin)[0];
                route.insert(route.begin(), fanin);
            }
        }

        paths.push_back(route);
        if (route.size() != 2)
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

    std::vector<std::tuple<coordinate<Lyt>, coordinate<Lyt>, coordinate<Lyt>>> updates{};
    std::vector<coordinate<Lyt>>                                               cleared{};

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
                    updates.emplace_back(tile, new_pos, dangling);
                }
                moved = true;
            }
        }
        if (!moved)
        {
            new_pos = {route[1].x, route[1].y, 0};
            updates.emplace_back(route.back(), new_pos, route[0]);
        }
    }
    detail::check_wires(lyt, cleared);
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
 * Utility function that fixes dead node warning by moving gates back and forth.
 *
 * @param lyt Gate level layout.
 * @param gt Gate locations.
 */
template <typename Lyt>
void fix_dead_nodes(Lyt& lyt, std::vector<coordinate<Lyt>>& gt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a cartesian layout");

    // find an empty coordinate to move gate to
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

    if (!empty_pos.is_dead())
    {
        for (const auto& gate : gt)
        {
            if (lyt.is_dead(lyt.get_node(gate)))
            {
                std::vector<mockturtle::signal<Lyt>> signals{};
                signals.reserve(lyt.fanin_size(lyt.get_node(gate)));

                lyt.foreach_fanin(lyt.get_node(gate), [&signals](const auto& fanin) { signals.push_back(fanin); });

                lyt.move_node(lyt.get_node(gate), empty_pos);
                lyt.clear_tile(gate);

                lyt.move_node(lyt.get_node(empty_pos), gate, signals);
            }
        }
    }
}

}  // namespace detail

/**
 * Optimization algorithm that can be used to reduce the layout area of sub-optimal physical design created by
 * heuristics or machine learning. This optimization utilizes the distinct characterisics of the 2DDWave clocking scheme,
 * which only allow information flow from top to bottom and left to right, therefore only aforementioned clocking scheme
 * if supported.
 *
 * To reduce the layout are, first, gates are moved up and to left as far as possible, include rerouting. This creates
 * more compact layouts by freeing up space on the right and bottom, as all gates were moved to the top left corner.
 *
 * After moving all gates, this algorithm also checks if excess wiring exists on the layout, i.e., rows that only
 * contain vertical wires or columns that only contain horizontal wires and removes them.
 *
 * As outputs have to lay on the border of a layout for better accessibility, they are also moved to new borders
 * determined on the location of all other gates.
 *
 * @param lyt Gate level layout.
 */
template <typename Lyt>
void post_layout_optimization(const Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE) && "Clocking scheme is not 2DDWave");

    // calculate bounding box
    const auto bounding_box_before = bounding_box_2d(lyt);
    const auto width               = bounding_box_before.get_x_size();
    const auto height              = bounding_box_before.get_y_size();

    // Optimization
    auto layout = obstruction_layout<Lyt>(lyt);

    std::vector<coordinate<Lyt>> gates{};
    gates.reserve(layout.num_wires() + layout.num_gates());

    for (auto x = 0; x <= static_cast<int>(width); x++)
    {
        for (auto y = 0; y <= static_cast<int>(height); y++)
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

    auto moved = 1;

    while (moved != 0)
    {
        moved = 0;

        for (auto& gate : gates)
        {
            std::tuple<bool, coordinate<Lyt>> moved_gate =
                detail::move_gate(gate, layout, static_cast<int>(width), static_cast<int>(height));

            if (std::get<0>(moved_gate))
            {
                moved += 1;
                gate = std::get<1>(moved_gate);  // update gate location
            }
        }

        std::sort(gates.begin(), gates.end(), [](const auto& a, const auto& b) { return a.x + a.y < b.x + b.y; });
    }

    detail::delete_wires(layout, static_cast<int>(width), static_cast<int>(height));
    detail::optimize_output(layout);
    detail::fix_dead_nodes(layout, gates);

    // calculate bounding box
    const auto bounding_box            = bounding_box_2d(layout);
    const auto optimized_layout_width  = bounding_box.get_x_size();
    const auto optimized_layout_height = bounding_box.get_y_size();

    layout.resize({optimized_layout_width, optimized_layout_height, layout.z()});

    // return layout;
}

}  // namespace fiction

#endif  // FICTION_OPTIMIZATION_HPP
