#include "fiction/layouts/coordinates.hpp"
#include "fiction_experiments.hpp"

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/cost.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>       // pre-defined types suitable for the FCN domain
#include <fiction/algorithms/physical_design/orthogonal.hpp>  // scalable heuristic for physical design of FCN layouts
#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>  // critical path and throughput calculations
#include <fiction/algorithms/verification/equivalence_checking.hpp>               // SAT-based equivalence checking
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/traits.hpp>  // traits for type-checking
#include <fiction/types.hpp>

#include <fmt/format.h>                      // output formatting
#include <lorina/genlib.hpp>                 // Genlib file parsing
#include <lorina/lorina.hpp>                 // Verilog/BLIF/AIGER/... file parsing
#include <mockturtle/io/verilog_reader.hpp>  // call-backs to read Verilog files into networks

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

using coordinate = fiction::offset::ucoord_t;
using gate_lyt = fiction::gate_level_layout<
    fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<coordinate>>>>;
using obs_gate_lyt = fiction::obstruction_layout<gate_lyt>;
using coord_path   = fiction::layout_coordinate_path<obs_gate_lyt>;
using dist         = fiction::manhattan_distance_functor<obs_gate_lyt, uint64_t>;
using cost         = fiction::unit_cost_functor<obs_gate_lyt, uint8_t>;


// If wires cross over empty tiles, move them down one layer
coord_path check_wires(obs_gate_lyt lyt, std::vector<coordinate> tc)
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
            lyt.obstruct_coordinate({tile.x, tile.y, 0});
            lyt.clear_obstructed_coordinate({tile.x, tile.y, 1});
            moved_tiles.push_back(tile);
        }
    }
    return moved_tiles;
}

// Trace back wires of a gate
std::tuple<std::vector<coordinate>, std::vector<coordinate>,
           std::vector<coordinate>, coord_path, coord_path, coord_path, coord_path>
get_fanin_and_fanouts(obs_gate_lyt& lyt, coordinate op)
{
    auto              fanin1 = coordinate{}, fanin2= coordinate{}, fanout1= coordinate{}, fanout2 = coordinate{};
    std::vector<coordinate> fanins, fanouts, to_clear;
    coord_path                             route1, route2, route3, route4;
    std::set<coordinate>    fanins_set, fanouts_set;

    // trace back fanins
    for (auto fin : lyt.incoming_data_flow(op))
    {
        if (fanins_set.find(fin) == fanins_set.end())
        {
            coordinate fanin = fin;
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
                if (fanins_set.empty())
                {
                    route1.insert(route1.begin(), fanin);
                }
                else
                {
                    route2.insert(route2.begin(), fanin);
                }
            }
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
                if (fanouts_set.empty())
                {
                    route3.push_back(fanout);
                }
                else
                {
                    route4.push_back(fanout);
                }
            }
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

std::tuple<bool, coordinate> move_gate(coordinate old_pos, obs_gate_lyt lyt, int width,
                                                      int height)
{
    const fiction::a_star_params params{true};

    auto [fanins, fanouts, to_clear, r1, r2, r3, r4] = get_fanin_and_fanouts(lyt, old_pos);
    int min_x                                        = 0;
    int min_y                                        = 0;

    // determine minimum coordinates for new placements
    if (fanins.size() > 0)
    {
        min_x = max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.x < b.x; })->x;
        min_y = max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.y < b.y; })->y;
    }

    int max_x        = old_pos.x;
    int max_y        = old_pos.y;
    int max_diagonal = max_x + max_y;

    auto new_pos = coordinate {};

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
    check_wires(lyt, to_clear);

    bool                      success     = false;
    coordinate current_pos = old_pos;
    bool                      optimized   = false;

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
                        !(fanouts.size() > 0 and path3.empty()) && !(fanouts.size() == 2 and path4.empty()))
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
void delete_row(obs_gate_lyt& lyt, int row_idx, int width, int height)
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
                    coordinate             fanin_row      = lyt.incoming_data_flow({x, y, z})[0];
                    std::vector<coordinate> fanin_next_row = lyt.incoming_data_flow({x, y + 1, z});
                    fanins[y][x][z]                                       = {};
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
                        coordinate     new_pos = {x, y - 1, z};
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
void delete_wires(obs_gate_lyt& lyt, int width, int height)
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
void optimize_output(obs_gate_lyt& lyt)
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
        bool                      moved    = false;
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
            lyt.move_node(lyt.get_node(route.back()), route[1], {lyt.make_signal(lyt.get_node(route[0]))});
        }
    }
    for (std::tuple<coordinate, coordinate, coordinate>& update : updates)
    {
        coordinate tile;
        coordinate new_pos;
        coordinate dangling;
        std::tie(tile, new_pos, dangling) = update;
        lyt.move_node(lyt.get_node(tile), new_pos, {lyt.make_signal(lyt.get_node(tile))});
    }
}

// fixes dead node warning by moving gates back and forth
void fix_dead_nodes(obs_gate_lyt& lyt, std::vector<coordinate>& gt)
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

int main()  // NOLINT
{
    experiments::experiment<std::string, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t,
                            uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, double, double, double, std::string>
        optimization_exp{"optimization",
                         "benchmark",
                         "inputs",
                         "outputs",
                         "initial nodes",
                         "ortho layout width (in tiles)",
                         "ortho layout height (in tiles)",
                         "ortho layout area (in tiles)",
                         "optimized layout width (in tiles)",
                         "optimized layout height (in tiles)",
                         "optimized layout area (in tiles)",
                         "gates",
                         "wires",
                         "critical path",
                         "throughput",
                         "runtime ortho (in sec)",
                         "runtime optimization (in sec)",
                         "improvement (%)",
                         "equivalent"};

    // stats for SMT-based physical design
    fiction::orthogonal_physical_design_stats orthogonal_stats{};

    static constexpr const uint64_t bench_select =
        fiction_experiments::parity;  // & fiction_experiments::fontes18;  // & fiction_experiments::fontes18; // &
                                      // ~fiction_experiments::log2 &
                                      //~fiction_experiments::sqrt & ~fiction_experiments::multiplier;

    for (const auto& benchmark : fiction_experiments::all_benchmarks(bench_select))
    {
        fmt::print("[i] processing {}\n", benchmark);
        // mockturtle::xag_network xag{};
        fiction::technology_network network;
        const auto                  read_verilog_result =
            lorina::read_verilog(fiction_experiments::benchmark_path(benchmark), mockturtle::verilog_reader(network));
        assert(read_verilog_result == lorina::return_code::success);

        // perform layout generation with an SMT-based exact algorithm
        const auto gate_level_layout = fiction::orthogonal<gate_lyt>(network, {}, &orthogonal_stats);

        //  compute critical path and throughput
        fiction::critical_path_length_and_throughput_stats cp_tp_stats{};
        fiction::critical_path_length_and_throughput(gate_level_layout, &cp_tp_stats);

        // calculate bounding box
        const auto bounding_box_before = fiction::bounding_box_2d(gate_level_layout);
        const auto width               = bounding_box_before.get_x_size();
        const auto height              = bounding_box_before.get_y_size();

        const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        // Optimization
        obs_gate_lyt layout = fiction::obstruction_layout<gate_lyt>(gate_level_layout);

        std::vector<coordinate> gates;
        for (int x = 0; x <= width; x++)
        {
            for (int y = 0; y <= height; y++)
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
                  [](coordinate a, coordinate b) { return a.x + a.y < b.x + b.y; });

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

            for (int gate_id = 0; gate_id < gates.size(); gate_id++)
            {
                std::pair<bool, coordinate> moved_gate =
                    move_gate(gates[gate_id], layout, width, height);

                if (moved_gate.first)
                {
                    moved += 1;
                    gates[gate_id] = moved_gate.second;  // update gate location
                }
            }
            std::sort(gates.begin(), gates.end(),
                      [](coordinate a, coordinate b) { return a.x + a.y < b.x + b.y; });
        }

        delete_wires(layout, width, height);
        optimize_output(layout);
        fix_dead_nodes(layout, gates);

        const auto end = std::chrono::steady_clock::now();
        // check equivalence
        std::stringstream print_stream1{};

        print_gate_level_layout(print_stream1, layout, true, false);
        std::cout << print_stream1.str();
        fiction::equivalence_checking_stats eq_stats{};
        // fiction::equivalence_checking<fiction::technology_network, obs_gate_lyt>(network, layout, &eq_stats);

        // const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
        //                               eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
        //                                                                         "NO";

        // calculate bounding box
        const auto bounding_box            = fiction::bounding_box_2d(layout);
        const auto optimized_layout_width  = bounding_box.get_x_size();
        const auto optimized_layout_height = bounding_box.get_y_size();
        layout.resize({optimized_layout_width, optimized_layout_height, 1});
        const double improv =
            100 * (((width + 1) * (height + 1)) - ((optimized_layout_width + 1) * (optimized_layout_height + 1))) /
            ((width + 1) * (height + 1));
        // log results
        optimization_exp(benchmark, network.num_pis(), network.num_pos(), network.num_gates(), width + 1, height + 1,
                         (width + 1) * (height + 1), optimized_layout_width + 1, optimized_layout_height + 1,
                         (optimized_layout_width + 1) * (optimized_layout_height + 1), layout.num_gates(),
                         layout.num_wires(), cp_tp_stats.critical_path_length, cp_tp_stats.throughput,
                         mockturtle::to_seconds(orthogonal_stats.time_total),
                         static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()),
                         improv, "??");

        optimization_exp.save();
        optimization_exp.table();
    }

    return EXIT_SUCCESS;
}
