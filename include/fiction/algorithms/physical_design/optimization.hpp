//
// Created by simon on 14.06.23.
//

#ifndef FICTION_OPTIMIZATION_HPP
#define FICTION_OPTIMIZATION_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/cost.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>  // pre-defined types suitable for the FCN domain
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/obstruction_layout.hpp>

#include <mockturtle/traits.hpp>

#include <cmath>
#include <cstdint>

namespace fiction
{

namespace detail
{

using coordinate   = offset::ucoord_t;
using gate_lyt     = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coordinate>>>>;
using obs_gate_lyt = obstruction_layout<gate_lyt>;
using coord_path   = layout_coordinate_path<obs_gate_lyt>;
using dist         = twoddwave_distance_functor<obs_gate_lyt, uint64_t>;
using cost         = unit_cost_functor<obs_gate_lyt, uint8_t>;

template <typename Obs_Lyt, typename Coord_Vec>
void check_wires(Obs_Lyt lyt, Coord_Vec tc) noexcept
{
    coord_path moved_tiles;
    for (auto tile : tc)
    {
        if (lyt.is_empty_tile({tile.x, tile.y, 0}) && lyt.is_wire_tile({tile.x, tile.y, 1}))
        {
            auto incoming_tile = lyt.incoming_data_flow({tile.x, tile.y, 1})[0];
            auto outgoing_tile = lyt.outgoing_data_flow({tile.x, tile.y, 1})[0];

            // move wire from z=1 to z=0
            lyt.move_node(lyt.get_node({tile.x, tile.y, 1}), {tile.x, tile.y, 0},
                          {lyt.make_signal(lyt.get_node(incoming_tile))});

            // if outgoing tile has other incoming signals (e.g. AND), update children
            if (lyt.incoming_data_flow(outgoing_tile).size() &&
                (find(tc.begin(), tc.end(), lyt.incoming_data_flow(outgoing_tile)[0]) == tc.end() ||
                 find(moved_tiles.begin(), moved_tiles.end(), lyt.incoming_data_flow(outgoing_tile)[0]) !=
                     moved_tiles.end()))
            {
                lyt.move_node(lyt.get_node(outgoing_tile), outgoing_tile,
                              {
                                  lyt.make_signal(lyt.get_node({tile.x, tile.y, 0})),
                                  lyt.make_signal(lyt.get_node(lyt.incoming_data_flow(outgoing_tile)[0])),
                              });
            }

            // otherwise, wire is only incoming signal
            else
            {
                lyt.move_node(lyt.get_node(outgoing_tile), outgoing_tile,
                              {lyt.make_signal(lyt.get_node({tile.x, tile.y, 0}))});
            }

            // update obstructions
            lyt.obstruct_coordinate({tile.x, tile.y, 0});
            lyt.clear_obstructed_coordinate({tile.x, tile.y, 1});
            moved_tiles.push_back(tile);
        }
    }
}

// Trace back wires of a gate
[[nodiscard]] std::tuple<std::vector<coordinate>, std::vector<coordinate>, std::vector<coordinate>, coord_path,
                         coord_path, coord_path, coord_path>
get_fanin_and_fanouts(obs_gate_lyt& lyt, coordinate op) noexcept
{
    auto fanin1 = coordinate{}, fanin2 = coordinate{}, fanout1 = coordinate{}, fanout2 = coordinate{};
    std::vector<coordinate> fanins, fanouts, to_clear;
    coord_path              route1, route2, route3, route4;
    std::set<coordinate>    fanins_set, fanouts_set;

    // trace back fanins
    for (auto fin : lyt.incoming_data_flow(op))
    {
        if (fanins_set.find(fin) == fanins_set.end())
        {
            coordinate fanin = fin;

            // switch between first and second fanin
            if (fanins_set.empty())
            {
                route1.insert(route1.begin(), op);
                route1.insert(route1.begin(), fanin);
            }
            else
            {
                route2.insert(route2.begin(), op);
                route2.insert(route2.begin(), fanin);
            }

            // go back till gate or pi is found
            while (lyt.is_wire_tile(fanin) && lyt.fanout_size(lyt.get_node(fanin)) != 2 &&
                   lyt.fanin_size(lyt.get_node(fanin)) != 0)
            {
                to_clear.push_back(fanin);
                fanin = lyt.incoming_data_flow(fanin)[0];

                // switch between first and second fanin
                if (fanins_set.empty())
                {
                    route1.insert(route1.begin(), fanin);
                }
                else
                {
                    route2.insert(route2.begin(), fanin);
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
    for (auto fout : lyt.outgoing_data_flow(op))
    {
        if (fanouts_set.find(fout) == fanouts_set.end())
        {
            coordinate fanout = fout;

            // switch between first and second fanout
            if (fanouts_set.empty())
            {
                route3.push_back(op);
                route3.push_back(fanout);
            }
            else
            {
                route4.push_back(op);
                route4.push_back(fanout);
            }

            // continue until gate or po is found
            while (lyt.is_wire_tile(fanout) && lyt.fanout_size(lyt.get_node(fanout)) != 0 &&
                   lyt.fanout_size(lyt.get_node(fanout)) != 2)
            {
                to_clear.push_back(fanout);
                fanout = lyt.outgoing_data_flow(fanout)[0];

                // switch between first and second fanout
                if (fanouts_set.empty())
                {
                    route3.push_back(fanout);
                }
                else
                {
                    route4.push_back(fanout);
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
        fanins.push_back(fanin1);
    }
    if (!fanin2.is_dead())
    {
        fanins.push_back(fanin2);
    }
    if (!fanout1.is_dead())
    {
        fanouts.push_back(fanout1);
    }
    if (!fanout2.is_dead())
    {
        fanouts.push_back(fanout2);
    }
    return std::make_tuple(fanins, fanouts, to_clear, route1, route2, route3, route4);
}

[[nodiscard]] std::tuple<bool, coordinate> move_gate(coordinate old_pos, obs_gate_lyt lyt, int width,
                                                     int height) noexcept
{
    const fiction::a_star_params params{true};

    auto [fanins, fanouts, to_clear, r1, r2, r3, r4] = get_fanin_and_fanouts(lyt, old_pos);
    int min_x                                        = 0;
    int min_y                                        = 0;

    // determine minimum coordinates for new placements
    if (fanins.size() > 0)
    {
        min_x = static_cast<int>(max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.x < b.x; })->x);
        min_y = static_cast<int>(max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.y < b.y; })->y);
    }

    int max_x        = old_pos.x;
    int max_y        = old_pos.y;
    int max_diagonal = max_x + max_y;

    auto new_pos = coordinate{};

    // if gate is directly connected to one of its fanins, no improvement is possible
    for (auto fanin : fanins)
    {
        for (coordinate i : lyt.incoming_data_flow(old_pos))
            if (i == fanin)
            {
                return std::make_tuple(false, new_pos);
            }
    }

    // remove wiring
    for (auto tile : to_clear)
    {
        lyt.clear_tile(tile);
        lyt.clear_obstructed_coordinate(tile);
    }

    // update children of fanouts
    for (auto fanout : fanouts)
    {
        std::vector<gate_lyt::signal> fins;
        for (auto fout : lyt.incoming_data_flow(fanout))
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
    check_wires<obs_gate_lyt, std::vector<coordinate>>(lyt, to_clear);

    bool       success     = false;
    coordinate current_pos = old_pos;
    bool       optimized   = false;

    // iterate over layout diagonally
    for (int k = 0; k < width + height + 1; k++)
    {
        for (int x = 0; x < k + 1; x++)
        {
            int y = k - x;
            if (success)
            {
                break;
            }
            // only check better positions
            else if (height >= y && y >= min_y && width >= x && x >= min_x && ((x + y) <= max_diagonal) &&
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
                    coord_path path1;
                    if (fanins.size() > 0)
                    {
                        path1 = fiction::a_star<coord_path>(lyt, {fanins[0], new_pos}, dist(), cost(), params);
                        for (auto tile : path1)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    coord_path path2;
                    if (fanins.size() == 2)
                    {
                        path2 = fiction::a_star<coord_path>(lyt, {fanins[1], new_pos}, dist(), cost(), params);
                        for (auto tile : path2)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    coord_path path3;
                    if (!fanouts.empty())
                    {
                        path3 = fiction::a_star<coord_path>(lyt, {new_pos, fanouts[0]}, dist(), cost(), params);
                        for (auto tile : path3)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    coord_path path4;
                    if (fanouts.size() == 2)
                    {
                        path4 = fiction::a_star<coord_path>(lyt, {new_pos, fanouts[1]}, dist(), cost(), params);
                        for (auto tile : path4)
                        {
                            lyt.obstruct_coordinate(tile);
                        }
                    }

                    // if possible routing was found, it will be applied
                    if (!(fanins.size() > 0 && path1.empty()) && !(fanins.size() == 2 && path2.empty()) &&
                        !(fanouts.size() > 0 && path3.empty()) && !(fanouts.size() == 2 && path4.empty()))
                    {
                        for (auto path : {path1, path2, path3, path4})
                        {
                            if (!path.empty())
                            {
                                fiction::route_path(lyt, path);
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
                        for (auto fanout : fanouts)
                        {
                            std::vector<gate_lyt::signal> signals;
                            for (auto fout : lyt.incoming_data_flow(fanout))
                            {
                                signals.push_back(lyt.make_signal(lyt.get_node(fout)));
                            }
                            lyt.move_node(lyt.get_node(fanout), fanout, signals);
                        }
                    }
                    // if no routing was found, remove added obstructions
                    else
                    {
                        for (auto path : {path1, path2, path3, path4})
                        {
                            if (!path.empty())
                            {
                                for (auto tile : path)
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
    // if no better coordinate was found, restore old wiring
    if (!success)
    {
        lyt.move_node(lyt.get_node(current_pos), old_pos, {});
        for (auto r : {r1, r2, r3, r4})
        {
            if (!r.empty())
            {
                fiction::route_path<obs_gate_lyt, coord_path>(lyt, r);
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
        std::vector<gate_lyt::signal> signals;
        for (auto fanin : lyt.incoming_data_flow(old_pos))
        {
            signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
        }
        lyt.move_node(lyt.get_node(old_pos), old_pos, signals);

        // update children oóf fanouts
        for (auto fanout : fanouts)
        {
            std::vector<gate_lyt::signal> fout_signals;
            for (auto fout : lyt.incoming_data_flow(fanout))
            {
                fout_signals.push_back(lyt.make_signal(lyt.get_node(fout)));
            }
            lyt.move_node(lyt.get_node(fanout), fanout, fout_signals);
        }
    }
    return std::make_tuple(optimized, new_pos);
}

// deletes a row in the layout by moving all southern gates up on positions
void delete_row(obs_gate_lyt& lyt, int row_idx, int width, int height) noexcept
{
    // create a map that store fanins of each coordinate
    std::map<int, std::map<int, std::map<int, std::vector<coordinate>>>> fanins;
    for (int y = row_idx; y <= height; y++)
    {
        fanins[y] = {};
        for (int x = 0; x <= width; x++)
        {
            fanins[y][x] = {};
            for (int z = 0; z < 2; z++)
            {
                if (y == row_idx && lyt.incoming_data_flow({x, y, z}).size() > 0)
                {
                    coordinate              fanin_row      = lyt.incoming_data_flow({x, y, z})[0];
                    std::vector<coordinate> fanin_next_row = lyt.incoming_data_flow({x, y + 1, z});
                    fanins[y][x][z]                        = {};
                    for (coordinate fanin : fanin_next_row)
                    {
                        if (fanin.y == y)
                        {
                            fanins[y][x][z].push_back(
                                fiction::coordinate<obs_gate_lyt>{fanin_row.x, fanin_row.y + 1, fanin_row.z});
                        }
                        else
                        {
                            fanins[y][x][z].push_back(fanin);
                        }
                    }
                }
                else
                {
                    fanins[y][x][z] = lyt.incoming_data_flow({x, y + 1, z});
                }
            }
        }

        // iterate through row and move gates
        for (int x = 0; x <= width; x++)
        {
            for (int z = 0; z < 2; z++)
            {
                coordinate old_pos = {x, y, z};
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
                        coordinate                    new_pos = {x, y - 1, z};
                        std::vector<gate_lyt::signal> fins;
                        for (coordinate fanin : fanins[y - 1][x][z])
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

// deletes rows that only contain vertically connected wires
void delete_wires(obs_gate_lyt& lyt, int width, int height) noexcept
{
    for (int y = height; y >= 0; y--)
    {
        bool found_row = true;
        for (int x = width; x >= 0; x--)
        {
            // check if tile has vertically connected wire
            if (lyt.is_wire_tile({x, y}) && lyt.fanin_size(lyt.get_node({x, y})) == 1 &&
                lyt.fanout_size(lyt.get_node({x, y})) == 1 && lyt.has_northern_incoming_signal({x, y}) &&
                lyt.has_southern_outgoing_signal({x, y}))
            {
                // do nothing
            }
            else if (lyt.is_empty_tile({x, y}))
            {
                // do nothing
            }
            // if row has gates or bent/ horizontal wires, row cannot be deleted
            else
            {
                found_row = false;
            }
        }
        if (found_row)
        {
            std::cout << "Row " << y << " can be deleted" << std::endl;
            delete_row(lyt, y, width, height);
        }
    }
}

// trace back all output nodes and calculate optimal positions
void optimize_output(obs_gate_lyt& lyt) noexcept
{
    // get all outputs
    std::vector<coordinate> pos{};
    lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });

    // get path from output to preceding gate
    std::vector<coord_path> paths;
    for (auto po : pos)
    {
        coord_path route;
        // trace back outputs
        for (auto fin : lyt.incoming_data_flow(po))
        {
            coordinate fanin = fin;
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
    }

    // calculate bounding box around gates without outputs
    auto min_x =
        std::max_element(paths.begin(), paths.end(), [](auto& a, auto& b) { return a[1].x < b[1].x; })->at(1).x;
    auto min_y =
        std::max_element(paths.begin(), paths.end(), [](auto& a, auto& b) { return a[1].y < b[1].y; })->at(1).y;
    std::vector<std::tuple<coordinate, coordinate, coordinate>> updates;

    // move output along its wiring until it lies on the bounding box
    for (auto& route : paths)
    {
        auto dangling = coordinate{};
        auto new_pos  = coordinate{};
        bool moved    = false;
        for (auto& tile : route)
        {
            if ((tile.x < min_x && tile.y < min_y))
            {
                dangling = tile;
            }
            else if (!lyt.is_po_tile(tile) && lyt.is_wire_tile(tile) && lyt.fanout_size(lyt.get_node(tile)) != 2)
            {
                lyt.clear_tile(tile);
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
                    updates.push_back(std::make_tuple(tile, new_pos, dangling));
                }
                moved = true;
            }
        }
        if (!moved)
        {
            new_pos = {route[1].x, route[1].y, 0};
            updates.push_back(std::make_tuple(route.back(), new_pos, route[0]));
        }
    }
    for (std::tuple<coordinate, coordinate, coordinate>& update : updates)
    {
        coordinate tile;
        coordinate new_pos;
        coordinate dangling;
        std::tie(tile, new_pos, dangling) = update;

        if (lyt.is_empty_tile(new_pos))
        {
            lyt.move_node(lyt.get_node(tile), new_pos, {lyt.make_signal(lyt.get_node(dangling))});
        }
        else if (new_pos.x == min_x)
        {
            lyt.create_buf(lyt.get_node(dangling), {new_pos.x, new_pos.y, 1});
            if (lyt.is_empty_tile({new_pos.x, new_pos.y + 1, 0}))
            {
                lyt.move_node(lyt.get_node(tile), {new_pos.x, new_pos.y + 1, 0},
                              {lyt.make_signal(lyt.get_node({new_pos.x, new_pos.y, 1}))});
            }
            else
            {
                lyt.move_node(lyt.get_node(tile), {new_pos.x + 1, new_pos.y, 0},
                              {lyt.make_signal(lyt.get_node({new_pos.x, new_pos.y, 1}))});
            }
        }
        else
        {
            lyt.create_buf(lyt.make_signal(lyt.get_node(dangling)), {new_pos.x, new_pos.y, 1});
            if (lyt.is_empty_tile({new_pos.x + 1, new_pos.y, 0}))
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

// fixes dead node warning by moving gates back and forth
void fix_dead_nodes(obs_gate_lyt& lyt, std::vector<coordinate>& gt) noexcept
{
    // find an empty coordinate to move gate to
    auto empty_pos = coordinate{};
    for (auto coord : lyt.coordinates())
    {
        if (lyt.is_empty_tile(coord))
        {
            empty_pos = coord;
            break;
        }
    }
    if (!empty_pos.is_dead())
    {
        for (auto gate : gt)
        {
            if (lyt.is_dead(lyt.get_node(gate)))
            {
                std::vector<coordinate> ins = lyt.incoming_data_flow(gate);
                lyt.move_node(lyt.get_node(gate), empty_pos);
                lyt.clear_tile(gate);
                std::vector<gate_lyt::signal> signals;
                for (auto fanin : ins)
                {
                    signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
                }
                lyt.move_node(lyt.get_node(empty_pos), gate, signals);
            }
        }
    }
}
}  // namespace detail

template <typename Lyt>
[[nodiscard]] detail::obs_gate_lyt optimize(const Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate level layout");
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    assert(lyt.is_clocking_scheme(clock_name::TWODDWAVE));

    // calculate bounding box
    const auto bounding_box_before = bounding_box_2d(lyt);
    const auto width               = bounding_box_before.get_x_size();
    const auto height              = bounding_box_before.get_y_size();

    // Optimization
    detail::obs_gate_lyt layout = fiction::obstruction_layout<Lyt>(lyt);

    std::vector<detail::coordinate> gates;
    for (int x = 0; x <= static_cast<int>(width); x++)
    {
        for (int y = 0; y <= static_cast<int>(height); y++)
        {
            if (layout.is_inv(layout.get_node({x, y})) || layout.is_and(layout.get_node({x, y})) ||
                layout.is_xor(layout.get_node({x, y})) || layout.is_fanout(layout.get_node({x, y})) ||
                layout.is_or(layout.get_node({x, y})) || layout.is_pi_tile({x, y}))
            {
                layout.obstruct_coordinate({x, y, 1});
                gates.push_back({x, y});
            }
        }
    }

    // sort gates based on diagonal line
    std::sort(gates.begin(), gates.end(),
              [](detail::coordinate a, detail::coordinate b) { return a.x + a.y < b.x + b.y; });

    int moved = 1;

    for (int h = 0; h < 20; h++)
    {
        if (moved != 0)
        {
            moved = 0;
        }
        else  // if no gates were moved, stop optimization
        {
            break;
        }

        for (size_t gate_id = 0; gate_id < gates.size(); gate_id++)
        {
            std::tuple<bool, detail::coordinate> moved_gate = detail::move_gate(gates[gate_id], layout, static_cast<int>(width), static_cast<int>(height));

            if (std::get<0>(moved_gate))
            {
                moved += 1;
                gates[gate_id] = std::get<1>(moved_gate);  // update gate location
            }
        }
        std::sort(gates.begin(), gates.end(),
                  [](detail::coordinate a, detail::coordinate b) { return a.x + a.y < b.x + b.y; });
    }

    detail::delete_wires(layout, static_cast<int>(width), static_cast<int>(height));
    detail::optimize_output(layout);
    detail::fix_dead_nodes(layout, gates);

    // calculate bounding box
    const auto bounding_box            = bounding_box_2d(layout);
    const auto optimized_layout_width  = bounding_box.get_x_size();
    const auto optimized_layout_height = bounding_box.get_y_size();
    layout.resize({optimized_layout_width, optimized_layout_height, 1});
    return layout;
}

}  // namespace fiction

#endif  // FICTION_OPTIMIZATION_HPP
