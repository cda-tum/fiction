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

using gate_lyt = fiction::gate_level_layout<
    fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
using obs_gate_lyt = fiction::obstruction_layout<gate_lyt>;
using coord_path   = fiction::layout_coordinate_path<obs_gate_lyt>;
using dist         = fiction::manhattan_distance_functor<obs_gate_lyt, uint64_t>;
using cost         = fiction::unit_cost_functor<obs_gate_lyt, uint8_t>;

coord_path check_wires(obs_gate_lyt lyt, std::vector<fiction::offset::ucoord_t> tc)
{
    coord_path moved_tiles;
    for (auto tile : tc)
    {
        if (lyt.is_empty_tile({tile.x, tile.y, 0}) && lyt.is_wire_tile({tile.x, tile.y, 1}))
        {
            auto fint  = lyt.incoming_data_flow({tile.x, tile.y, 1})[0];
            auto foutt = lyt.outgoing_data_flow({tile.x, tile.y, 1})[0];
            lyt.move_node(lyt.get_node({tile.x, tile.y, 1}), {tile.x, tile.y, 0},
                          {lyt.make_signal(lyt.get_node(fint))});
            if (lyt.incoming_data_flow(foutt).size() &&
                (find(tc.begin(), tc.end(), lyt.incoming_data_flow(foutt)[0]) == tc.end() ||
                 find(moved_tiles.begin(), moved_tiles.end(), lyt.incoming_data_flow(foutt)[0]) != moved_tiles.end()))
            {
                lyt.move_node(lyt.get_node(foutt), foutt,
                              {
                                  lyt.make_signal(lyt.get_node({tile.x, tile.y, 0})),
                                  lyt.make_signal(lyt.get_node(lyt.incoming_data_flow(foutt)[0])),
                              });
            }
            else
            {
                lyt.move_node(lyt.get_node(foutt), foutt, {lyt.make_signal(lyt.get_node({tile.x, tile.y, 0}))});
            }
            lyt.obstruct_coordinate({tile.x, tile.y, 0});
            lyt.clear_obstructed_coordinate({tile.x, tile.y, 1});
            moved_tiles.push_back(tile);
        }
    }
    return moved_tiles;
}

std::tuple<std::vector<fiction::offset::ucoord_t>, std::vector<fiction::offset::ucoord_t>,
           std::vector<fiction::offset::ucoord_t>, coord_path, coord_path, coord_path, coord_path>
get_fanin_and_fanouts(obs_gate_lyt& lyt, fiction::offset::ucoord_t op)
{
    fiction::offset::ucoord_t              fanin1 = {1000000, 1000000};
    fiction::offset::ucoord_t              fanin2 = {1000000, 1000000};
    std::vector<fiction::offset::ucoord_t> fanins;
    fiction::offset::ucoord_t              fanout1 = {1000000, 1000000};
    fiction::offset::ucoord_t              fanout2 = {1000000, 1000000};
    std::vector<fiction::offset::ucoord_t> fanouts;
    std::vector<fiction::offset::ucoord_t> to_clear;
    coord_path                             route1;
    coord_path                             route2;
    coord_path                             route3;
    coord_path                             route4;
    std::set<fiction::offset::ucoord_t>    fanins_set;
    std::set<fiction::offset::ucoord_t>    fanouts_set;
    for (auto fin : lyt.incoming_data_flow(op))
    {
        if (fanins_set.find(fin) == fanins_set.end())
        {
            fiction::offset::ucoord_t fanin = fin;
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
    for (auto fout : lyt.outgoing_data_flow(op))
    {
        if (fanouts_set.find(fout) == fanouts_set.end())
        {
            fiction::offset::ucoord_t fanout = fout;
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
    if (fanin1.x != 1000000)
    {
        fanins.push_back(fanin1);
    }
    if (fanin2.x != 1000000)
    {
        fanins.push_back(fanin2);
    }
    if (fanout1.x != 1000000)
    {
        fanouts.push_back(fanout1);
    }
    if (fanout2.x != 1000000)
    {
        fanouts.push_back(fanout2);
    }
    return std::make_tuple(fanins, fanouts, to_clear, route1, route2, route3, route4);
}

bool move_gate(fiction::offset::ucoord_t old_pos, obs_gate_lyt& lyt, int width, int height)
{
    const fiction::a_star_params           params{true};
    std::vector<fiction::offset::ucoord_t> fanins;
    std::vector<fiction::offset::ucoord_t> fanouts;
    std::vector<fiction::offset::ucoord_t> to_clear;
    coord_path                             r1;
    coord_path                             r2;
    coord_path                             r3;
    coord_path                             r4;
    auto                                   ff = get_fanin_and_fanouts(lyt, old_pos);
    fanins                                    = std::get<0>(ff);
    fanouts                                   = std::get<1>(ff);
    to_clear                                  = std::get<2>(ff);
    r1                                        = std::get<3>(ff);
    r2                                        = std::get<4>(ff);
    r3                                        = std::get<5>(ff);
    r4                                        = std::get<6>(ff);
    int min_x = max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.x < b.x; })->x;
    int min_y = max_element(fanins.begin(), fanins.end(), [](auto a, auto b) { return a.y < b.y; })->y;
    int max_x = old_pos.x;
    int max_y = old_pos.y;
    for (auto fanin : fanins)
    {
        for (fiction::offset::ucoord_t i : lyt.incoming_data_flow(old_pos))
            if (i == fanin)
            {
                return false;
            }
    }
    for (auto tile : to_clear)
    {
        lyt.clear_tile(tile);
        lyt.clear_obstructed_coordinate(tile);
    }
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
    lyt.move_node(lyt.get_node(old_pos), old_pos, {});
    check_wires(lyt, to_clear);
    bool                      success     = false;
    fiction::offset::ucoord_t current_pos = old_pos;
    bool                      optimized   = false;
    for (int k = 0; k < width + height + 1; k++)
    {
        for (int x = 0; x < k + 1; x++)
        {
            int y = k - x;
            if (success)
            {
                break;
            }
            else if (std::min(max_y, height) >= y && y >= min_y && std::min(width, max_x) >= x && x >= min_x)
            {
                fiction::offset::ucoord_t new_pos = {x, y};
                if (lyt.is_empty_tile(new_pos) && lyt.is_empty_tile({new_pos.x, new_pos.y, 1}))
                {
                    lyt.move_node(lyt.get_node(current_pos), new_pos, {});
                    lyt.obstruct_coordinate(new_pos);
                    lyt.obstruct_coordinate({new_pos.x, new_pos.y, 1});
                    lyt.clear_obstructed_coordinate(current_pos);
                    lyt.clear_obstructed_coordinate({current_pos.x, current_pos.y, 1});
                    coord_path path1 = fiction::a_star<coord_path>(lyt, {fanins[0], new_pos}, dist(), cost(), params);
                    for (auto tile : path1)
                    {
                        lyt.obstruct_coordinate(tile);
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
                    if (!path1.empty() && !path3.empty() && !(fanins.size() == 2 && path2.empty()) &&
                        !(fanouts.size() == 2 and path4.empty()))
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
                        if (fanins.size() == 2)
                        {
                            lyt.move_node(lyt.get_node(new_pos), new_pos,
                                          {
                                              lyt.make_signal(lyt.get_node(path1[path1.size() - 2])),
                                              lyt.make_signal(lyt.get_node(path2[path2.size() - 2])),
                                          });
                        }
                        else
                        {
                            lyt.move_node(lyt.get_node(new_pos), new_pos,
                                          {lyt.make_signal(lyt.get_node(path1[path1.size() - 2]))});
                        }
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
        lyt.clear_obstructed_coordinate(current_pos);
        lyt.clear_obstructed_coordinate({current_pos.x, current_pos.y, 1});
        lyt.obstruct_coordinate(old_pos);
        lyt.obstruct_coordinate({old_pos.x, old_pos.y, 1});
        std::vector<gate_lyt::signal> signals;
        for (auto fanin : lyt.incoming_data_flow(old_pos))
        {
            signals.push_back(lyt.make_signal(lyt.get_node(fanin)));
        }
        lyt.move_node(lyt.get_node(old_pos), old_pos, signals);
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
    return optimized;
}

void delete_row(obs_gate_lyt& lyt, int row_idx, int width, int height)
{
    std::map<int, std::map<int, std::map<int, std::vector<fiction::offset::ucoord_t>>>> fanins;
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
                    fiction::offset::ucoord_t              fanin_row      = lyt.incoming_data_flow({x, y, z})[0];
                    std::vector<fiction::offset::ucoord_t> fanin_next_row = lyt.incoming_data_flow({x, y + 1, z});
                    fanins[y][x][z]                                       = {};
                    for (fiction::offset::ucoord_t fanin : fanin_next_row)
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
        for (int x = 0; x <= width; x++)
        {
            for (int z = 0; z < 2; z++)
            {
                fiction::offset::ucoord_t old_pos = {x, y, z};
                if (!lyt.is_empty_tile(old_pos))
                {
                    if (y == row_idx)
                    {
                        lyt.clear_tile(old_pos);
                    }
                    else
                    {
                        fiction::offset::ucoord_t     new_pos = {x, y - 1, z};
                        std::vector<gate_lyt::signal> fins;
                        for (fiction::offset::ucoord_t fanin : fanins[y - 1][x][z])
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

void delete_wires(obs_gate_lyt& lyt, int width, int height)
{
    for (int y = height; y >= 0; y--)
    {
        bool found_row = true;
        for (int x = width; x >= 0; x--)
        {
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

void optimize_output(obs_gate_lyt& lyt)
{
    std::vector<fiction::offset::ucoord_t> pos{};
    lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });
    std::vector<coord_path> paths;
    for (auto po : pos)
    {
        coord_path route;
        for (auto fin : lyt.incoming_data_flow(po))
        {
            fiction::offset::ucoord_t fanin = fin;
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
    auto min_x =
        std::max_element(paths.begin(), paths.end(), [](auto& a, auto& b) { return a[1].x < b[1].x; })->at(1).x;
    auto min_y =
        std::max_element(paths.begin(), paths.end(), [](auto& a, auto& b) { return a[1].y < b[1].y; })->at(1).y;
    for (auto& route : paths)
    {
        fiction::offset::ucoord_t dangling = {100000, 100000};
        fiction::offset::ucoord_t new_pos  = {100000, 100000};
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
                if (new_pos.x == 100000)
                {
                    new_pos = tile;
                }
            }
            else if (dangling.x != 100000)
            {
                if (new_pos.x != 100000)
                {
                    lyt.move_node(lyt.get_node(tile), new_pos, {lyt.make_signal(lyt.get_node(dangling))});
                }
                moved = true;
            }
        }
        if (!moved)
        {
            lyt.move_node(lyt.get_node(route.back()), route[1], {lyt.make_signal(lyt.get_node(route[0]))});
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
        fiction_experiments::clpl;  // & fiction_experiments::fontes18; // & ~fiction_experiments::log2 &
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
        // std::stringstream print_stream1{};

        // print_gate_level_layout(print_stream1, gate_level_layout, true, false);
        // std::cout << print_stream1.str();
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
        for (int x = 0; x <= width; x++)
        {
            for (int y = 0; y <= height; y++)
            {
                if (layout.is_inv(layout.get_node({x, y})) || layout.is_and(layout.get_node({x, y})) ||
                    layout.is_xor(layout.get_node({x, y})) || layout.is_fanout(layout.get_node({x, y})) ||
                    layout.is_or(layout.get_node({x, y})))
                {
                    layout.obstruct_coordinate({x, y, 1});
                }
            }
        }

        int moved = 1;
        for (int h = 0; h < 20; h++)
        {
            if (moved != 0)
            {
                moved = 0;
                for (int k = 0; k < width + height + 1; k++)
                {
                    for (int x = 0; x < k + 1; x++)
                    {
                        int y = k - x;
                        if (y <= height && x <= width)
                        {
                            if (layout.is_inv(layout.get_node({x, y})) || layout.is_and(layout.get_node({x, y})) ||
                                layout.is_xor(layout.get_node({x, y})) || layout.is_fanout(layout.get_node({x, y})) ||
                                layout.is_or(layout.get_node({x, y})))
                            {
                                if (move_gate({x, y}, layout, width, height))
                                {
                                    moved += 1;
                                }
                            }
                        }
                    }
                }
                std::cout << moved << std::endl;
            }
        }
        delete_wires(layout, width, height);
        optimize_output(layout);
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
