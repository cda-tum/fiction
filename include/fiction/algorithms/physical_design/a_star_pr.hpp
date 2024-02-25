//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_A_STAR_PR_HPP
#define FICTION_A_STAR_PR_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/algorithms/physical_design/apply_gate_library.hpp"
#include "fiction/algorithms/physical_design/color_routing.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/algorithms/physical_design/topo_view_ci_to_co.hpp"
#include "fiction/algorithms/physical_design/topo_view_co_to_ci.hpp"
#include "fiction/algorithms/verification/equivalence_checking.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/io/write_fgl_layout.hpp"
#include "fiction/io/write_svg_layout.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <mockturtle/networks/klut.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>
#include <random>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fiction
{

/**
 * This struct stores statistics about the wiring reduction process.
 */
struct a_star_pr_stats
{
    /**
     * Runtime of the wiring reduction process.
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

struct NestedVectorHash
{
    size_t operator()(const std::vector<std::vector<int>>& vec) const
    {
        size_t hash = 0;
        for (const auto& inner_vec : vec)
        {
            for (int i : inner_vec)
            {
                hash ^= std::hash<int>{}(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
        }
        return hash;
    }
};

template <typename coord_vector>
class PriorityQueue {
    std::priority_queue<std::pair<double, coord_vector>, std::vector<std::pair<double, coord_vector>>,
                        std::greater<std::pair<double, coord_vector>>>
        elements;

  public:
    bool empty() const {
        return elements.empty();
    }

    void put(const coord_vector& item, double priority)
    {
        elements.emplace(priority, item);
    }

    coord_vector get()
    {
        coord_vector item = elements.top().second;
        elements.pop();
        return item;
    }
};

template <typename Lyt, typename Ntk>
std::vector<std::vector<int>>
get_possible_actions(Lyt& layout, Ntk& network, std::vector<mockturtle::node<Ntk>> actions, int current_node,
                     std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict, bool pis_top,
                     bool pis_left, int expansions)
{
    std::vector<int> preceding_nodes{};
    network.foreach_fanin(actions[current_node], [&preceding_nodes](const auto& f) { preceding_nodes.push_back(f); });
    std::vector<std::vector<int>> possible_positions_nodes{};

    if (network.is_pi(actions[current_node]))
    {
        int count = 0;
        if (pis_top && pis_left)
        {
            for (int k = 0; k <= std::max(layout.x(), layout.y()); k++)
            {
                fiction::coordinate<Lyt> coord_top{k, 0, 0};
                std::vector<int>         tile_top{k, 0, 0};
                fiction::coordinate<Lyt> coord_left{0, k, 0};
                std::vector<int>         tile_left{0, k, 0};

                if (layout.is_empty_tile(coord_top))
                {
                    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                    using cost = unit_cost_functor<Lyt, uint8_t>;
                    static const a_star_params params{true};

                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                    auto                     path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                        layout, {coord_top, drain}, dist(), cost(), params);
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});

                    if (!path_to_drain.empty())
                    {
                        possible_positions_nodes.push_back(tile_top);
                        count++;
                    }
                }
                if (layout.is_empty_tile(coord_left))
                {
                    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                    using cost = unit_cost_functor<Lyt, uint8_t>;
                    static const a_star_params params{true};

                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                    auto                     path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                        layout, {coord_left, drain}, dist(), cost(), params);
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});

                    if (!path_to_drain.empty())
                    {
                        possible_positions_nodes.push_back(tile_left);
                        count++;
                    }
                }
                if (count >= 2 * expansions)
                {
                    return possible_positions_nodes;
                }
            }
        }
        else if (pis_top)
        {
            for (int k = 0; k <= layout.x(); k++)
            {
                fiction::coordinate<Lyt> coord{k, 0, 0};
                std::vector<int>         tile{k, 0, 0};

                if (layout.is_empty_tile(coord))
                {
                    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                    using cost = unit_cost_functor<Lyt, uint8_t>;
                    static const a_star_params params{true};

                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                    auto path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {coord, drain},
                                                                                               dist(), cost(), params);
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});

                    if (!path_to_drain.empty())
                    {
                        possible_positions_nodes.push_back(tile);
                        count++;
                    }
                }
                if (count >= expansions)
                {
                    return possible_positions_nodes;
                }
            }
        }
        else if (pis_left)
        {
            for (int k = 0; k <= layout.y(); k++)
            {
                fiction::coordinate<Lyt> coord{0, k, 0};
                std::vector<int>         tile{0, k, 0};

                if (layout.is_empty_tile(coord))
                {
                    using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                    using cost = unit_cost_functor<Lyt, uint8_t>;
                    static const a_star_params params{true};

                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                    auto path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {coord, drain},
                                                                                               dist(), cost(), params);
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});

                    if (!path_to_drain.empty())
                    {
                        possible_positions_nodes.push_back(tile);
                        count++;
                    }
                }
                if (count >= expansions)
                {
                    return possible_positions_nodes;
                }
            }
        }
        else
        {
            throw std::runtime_error("Allowed location for PIs has to be specified");
        }
    }
    else if (network.is_po(actions[current_node]))
    {
        int  count = 0;
        auto loc = layout.get_tile(node_dict[preceding_nodes[0]]);
        for (int k = 0; k <= std::max(layout.x() - loc.x, layout.y() - loc.y); ++k)
        {
            fiction::coordinate<Lyt> coord{loc.x + k, layout.y(), 0};
            std::vector<int>         tile{loc.x + k, static_cast<int>(layout.y()), 0};

            if (layout.is_empty_tile(coord))
            {
                using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                using cost = unit_cost_functor<Lyt, uint8_t>;
                static const a_star_params params{true};
                fiction::coordinate<Lyt>   previous{loc.x, loc.y, 0};
                auto path_to_new_pos = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {previous, coord},
                                                                                             dist(), cost(), params);
                if (!path_to_new_pos.empty())
                {
                    possible_positions_nodes.push_back(tile);
                }
            }

            if (count >= expansions)
            {
                return possible_positions_nodes;
            }

            fiction::coordinate<Lyt> coord2{layout.x(), loc.y + k, 0};
            std::vector<int>         tile2{static_cast<int>(layout.x()), loc.y + k, 0};

            if (layout.is_empty_tile(coord2))
            {
                using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                using cost = unit_cost_functor<Lyt, uint8_t>;
                static const a_star_params params{true};
                fiction::coordinate<Lyt>   previous{loc.x, loc.y, 0};
                auto path_to_new_pos = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {previous, coord2},
                                                                                             dist(), cost(), params);
                if (!path_to_new_pos.empty())
                {
                    possible_positions_nodes.push_back(tile2);
                }
            }
            if (count >= expansions)
            {
                return possible_positions_nodes;
            }
        }
    }
    else if (preceding_nodes.size() == 1)
    {
        int  count          = 0;
        int  node           = node_dict[preceding_nodes[0]];
        auto loc            = layout.get_tile(node);
        // iterate through cartesian layout diagonally
        for (int k = 0; k < layout.x() + layout.y() + 1; ++k)
        {
            for (int x = 0; x < k + 1; ++x)
            {
                const auto y = k - x;
                if ((loc.y + y) <= layout.y() && (loc.x + x) <= layout.x())
                {
                    std::vector<int> tile{loc.x + x, loc.y + y, 0};
                    if (layout.is_empty_tile({loc.x + x, loc.y + y, 0}))
                    {
                        using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                        using cost = unit_cost_functor<Lyt, uint8_t>;
                        static const a_star_params params{true};
                        fiction::coordinate<Lyt>   previous{loc.x, loc.y, 0};
                        fiction::coordinate<Lyt>   new_pos{loc.x + x, loc.y + y, 0};
                        auto path_to_new_pos = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                            layout, {previous, new_pos}, dist(), cost(), params);
                        if (!path_to_new_pos.empty())
                        {
                            // if (!network.is_fanout(actions[current_node]))
                            if (true)
                            {
                                layout.resize({layout.x() + 1, layout.y() + 1, 1});
                                fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                                auto path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                                    layout, {new_pos, drain}, dist(), cost(), params);
                                layout.resize({layout.x() - 1, layout.y() - 1, 1});

                                if (!path_to_drain.empty())
                                {
                                    possible_positions_nodes.push_back(tile);
                                    count++;
                                }
                            }
                            else
                            {
                                layout.resize({layout.x() + 1, layout.y() + 1, 1});
                                std::vector<fiction::routing_objective<Lyt>> objectives{};
                                fiction::coordinate<Lyt>                     drain{layout.x(), layout.y(), 0};
                                objectives.push_back({new_pos, drain});
                                fiction::coordinate<Lyt> drain2{layout.x() - 1, layout.y(), 0};
                                objectives.push_back({new_pos, drain2});
                                fiction::color_routing_params ps{};
                                ps.path_limit      = 1;
                                ps.crossings       = true;
                                ps.engine          = fiction::graph_coloring_engine::MCS;
                                const auto success = color_routing<Lyt>(layout, objectives, ps);
                                layout.resize({layout.x() - 1, layout.y() - 1, 1});
                                if (success)
                                {
                                    possible_positions_nodes.push_back(tile);
                                    count++;
                                }
                            }
                        }
                    }
                }
                if (count >= expansions)
                {
                    // for (auto t : possible_positions_nodes){for (auto tt : t){std::cout << tt;}std::cout << "\n";}
                    return possible_positions_nodes;
                }
            }
        }
        /*using dist = twoddwave_distance_functor<Lyt, uint64_t>;
        using cost = unit_cost_functor<Lyt, uint8_t>;
        static const a_star_params params{true};
        auto         start = loc;
        layout.resize({layout.x() + 1, layout.y() + 1, 1});
        fiction::tile<Lyt>         end{layout.x(), layout.y(), 0};
        auto                       path =
            fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {start, end}, dist(), cost(), params);
        if (path.empty())
        {
            layout.resize({layout.x() - 1, layout.y() - 1, 1});
            return possible_positions_nodes;
        }
        else
        {
            // Iterate through the array starting from the second element
            for (int i = 1; i < sizeof(path) / sizeof(path[0]); ++i) {
                if (path[i].z == 0){
                    std::vector<int>         tile{static_cast<int>(path[i].x), static_cast<int>(path[i].y), 0};
                    possible_positions_nodes.push_back(tile);
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});
                    return possible_positions_nodes;
                }
            }
        }*/
    }
    else if (preceding_nodes.size() == 2)
    {
        int  count  = 0;
        int  node_1 = node_dict[preceding_nodes[0]];
        auto loc_1  = layout.get_tile(node_1);
        int  node_2 = node_dict[preceding_nodes[1]];
        auto loc_2  = layout.get_tile(node_2);
        int  min_x  = std::max(loc_1.x, loc_2.x);
        int  min_y  = std::max(loc_1.y, loc_2.y);
        if (loc_1.x == loc_2.x)
        {
            min_x += 1;
        }
        if (loc_1.y == loc_2.y)
        {
            min_y += 1;
        }
        for (int k = 0; k < layout.x() + layout.y() + 1; ++k)
        {
            for (int x = 0; x < k + 1; ++x)
            {
                const auto y = k - x;
                if ((min_y + y) <= layout.y() && (min_x + x) <= layout.x())
                {
                    fiction::coordinate<Lyt> coord{min_x + x, min_y + y, 0};
                    std::vector<int>         tile{min_x + x, min_y + y, 0};

                    if (layout.is_empty_tile(coord))
                    {
                        using dist = twoddwave_distance_functor<Lyt, uint64_t>;
                        using cost = unit_cost_functor<Lyt, uint8_t>;
                        static const a_star_params params{true};
                        fiction::tile<Lyt>         start_1{loc_1.x, loc_1.y, loc_1.z};
                        fiction::tile<Lyt>         start_2{loc_2.x, loc_2.y, loc_2.z};
                        auto                       path_node_1 = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                            layout, {start_1, coord}, dist(), cost(), params);

                        if (!path_node_1.empty())
                        {
                            for (auto el : path_node_1)
                            {
                                layout.obstruct_coordinate(el);
                            }

                            auto path_node_2 = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                                layout, {start_2, coord}, dist(), cost(), params);

                            if (!path_node_2.empty())
                            {
                                layout.resize({layout.x() + 1, layout.y() + 1, 1});
                                fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};
                                auto path_to_drain = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(
                                    layout, {coord, drain}, dist(), cost(), params);
                                layout.resize({layout.x() - 1, layout.y() - 1, 1});

                                if (!path_to_drain.empty())
                                {
                                    possible_positions_nodes.push_back(tile);
                                    count++;
                                }
                            }
                            for (auto el : path_node_1)
                            {
                                layout.clear_obstructed_coordinate(el);
                            }
                        }
                    }
                }
                if (count >= expansions)
                {
                    return possible_positions_nodes;
                }
            }
        }
    }
    else
    {
        throw std::runtime_error("Invalid node type");
    }

    return possible_positions_nodes;
}

template <typename Lyt, typename Ntk>
bool valid_layout(Lyt& layout, Ntk& network, std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict,
                  bool& placement_possible)
{
    /*int count = 0;

    for (auto& node : node_dict)
    {
        if ((!layout.is_po_tile(layout.get_tile(node.second)) && (layout.fanout_size(node.second) == 0)) ||
            ((layout.fanout_size(node.second) == 1) && network.is_fanout(node.first)))
        {
            count++;
        }
    }
    int offset = count;
    std::vector<fiction::routing_objective<Lyt>> objectives{};
    layout.resize({layout.x() + count, layout.y() + count, 1});*/
    for (auto& node : node_dict)
    {
        auto tile = layout.get_tile(node.second);
        if ((!layout.is_po_tile(layout.get_tile(node.second)) && (layout.fanout_size(node.second) == 0)) ||
            ((layout.fanout_size(node.second) == 1) && network.is_fanout(node.first)))
        {
            /*fiction::coordinate<Lyt> start{tile.x, tile.y, tile.z};
            fiction::coordinate<Lyt> end{layout.x(), layout.y() - count, 0};
            objectives.push_back({start, end});
            count--;*/
            using dist = twoddwave_distance_functor<Lyt, uint64_t>;
            using cost = unit_cost_functor<Lyt, uint8_t>;
            static const a_star_params params{true};

            layout.resize({layout.x() + 1, layout.y() + 1, 1});
            fiction::coordinate<Lyt> start{tile.x, tile.y, tile.z};
            fiction::coordinate<Lyt> end{layout.x(), layout.y(), 0};
            auto                     path =
                fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {start, end}, dist(), cost(), params);
            layout.resize({layout.x() - 1, layout.y() - 1, 1});

            if (path.empty())
            {
                placement_possible = false;
                return false;
            }
        }
        if ((layout.fanout_size(node.second) == 0) && network.is_fanout(node.first))
        {
            if (!((layout.is_empty_tile({tile.x + 1, tile.y, 0}) ||
                   (layout.is_empty_tile({tile.x + 1, tile.y, 1}) &&
                    !layout.is_obstructed_coordinate({tile.x + 1, tile.y, 1}))) &&
                  (layout.is_empty_tile({tile.x, tile.y + 1, 0}) ||
                   (layout.is_empty_tile({tile.x, tile.y + 1, 1}) &&
                    !layout.is_obstructed_coordinate({tile.x, tile.y + 1, 1})))))
            {
                placement_possible = false;
                return false;
            }
        }
        /*if ((layout.fanout_size(node.second) == 1) && network.is_fanout(node.first))
        {
            bool               below      = false;
            fiction::tile<Lyt> lower_tile = {tile.x, tile.y + 1, 0};
            layout.foreach_fanout(node.second,
                                  [&layout, &below, &lower_tile](const auto& fout)
                                  {
                                      fiction::tile<Lyt> fanout = layout.get_tile(fout);
                                      if ((fanout.x == lower_tile.x) && (fanout.y == lower_tile.y))
                                      {
                                          below = true;
                                      }
                                  });

            if (below)
            {

                if (layout.is_wire_tile(lower_tile) && !layout.is_wire_tile({lower_tile.x, lower_tile.y, 1}) &&
                    layout.is_obstructed_coordinate({lower_tile.x, lower_tile.y, 1}))
                {
                    if (!(layout.is_empty_tile({tile.x + 1, tile.y, 0}) && layout.is_empty_tile({lower_tile.x + 1,
        lower_tile.y, 0}) && layout.is_empty_tile({tile.x + 2, tile.y, 0}) && layout.is_empty_tile({lower_tile.x + 2,
        lower_tile.y, 0})))
                    {
                        placement_possible = false;
                        return false;
                    }
                }
            }
        }*/
    }
    /*fiction::color_routing_params ps{};
    ps.path_limit = 10;
    ps.crossings = true;
    ps.engine = fiction::graph_coloring_engine::MCS;
    const auto success = color_routing<Lyt>(layout, objectives, ps);
    layout.resize({layout.x() - offset, layout.y() - offset, 1});
    if (success){
        return true;
    }
    else{
        placement_possible = false;
        return false;
    }*/

    return true;
}

template <typename Lyt, typename Ntk>
void place_node_with_1_input(Lyt& layout, Ntk& network, int x, int y, int signal, int current_node,
                             std::vector<mockturtle::node<Ntk>> actions, int current_po,
                             std::vector<std::string> po_names)
{
    if (network.is_inv(actions[current_node]))
    {
        layout.create_not(signal, {x, y});
    }
    else if (network.is_po(actions[current_node]))
    {
        layout.create_po(signal, po_names[current_po], {x, y});
    }
    else if (network.is_fanout(actions[current_node]) || network.is_buf(actions[current_node]))
    {
        layout.create_buf(signal, {x, y});
    }
    else
    {
        throw std::runtime_error("Invalid node type");
    }
}

template <typename Lyt, typename Ntk>
void place_node_with_2_inputs(Lyt& layout, Ntk& network, int x, int y, int signal_1, int signal_2, int current_node,
                              std::vector<mockturtle::node<Ntk>> actions)
{
    if (network.is_and(actions[current_node]))
    {
        layout.create_and(signal_1, signal_2, {x, y});
    }
    else if (network.is_or(actions[current_node]))
    {
        layout.create_or(signal_1, signal_2, {x, y});
    }
    else if (network.is_xor(actions[current_node]))
    {
        layout.create_xor(signal_1, signal_2, {x, y});
    }
    else
    {
        throw std::runtime_error("Invalid node type");
    }
}

template <typename Ntk, typename Lyt>
std::tuple<double, bool> calculate_reward(Lyt& lyt, bool placed_node, int current_node,
                                          std::vector<mockturtle::node<Ntk>> actions, bool placement_possible,
                                          int& max_placed_nodes)
{
    double reward = (current_node == actions.size()) ? 10000.0 : (placed_node ? 1.0 : 0.0);
    bool   done   = (current_node == actions.size()) || !placement_possible;

    if (current_node > max_placed_nodes)
    {
        std::cout << "New best placement: " << current_node << "/" << actions.size() << std::endl;
        max_placed_nodes = current_node;
        /*std::stringstream print_stream{};
        print_gate_level_layout(print_stream, lyt, false, false);
        std::cout << print_stream.str();*/
    }

    return std::make_tuple(reward, done);
}

template <typename Lyt, typename Ntk>
std::pair<double, bool> step(std::vector<int> action, Lyt& layout, Ntk& network, bool& placement_possible,
                             int& current_node, std::vector<mockturtle::node<Ntk>> actions,
                             std::vector<std::string> po_names, int& current_pi, int& current_po,
                             std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>>& node_dict,
                             int& max_placed_nodes, mockturtle::node_map<mockturtle::node<Lyt>, Ntk>& pi2node)
{
    int x = action[0];
    int y = action[1];

    std::vector<int> preceding_nodes{};
    network.foreach_fanin(actions[current_node], [&preceding_nodes](const auto& f) { preceding_nodes.push_back(f); });

    bool   done   = false;
    double reward = 0;

    if (!placement_possible || !layout.is_empty_tile({x, y}))
    {
        done = true;
    }
    else
    {
        int placed_node = 0;
        int num_fanins  = preceding_nodes.size();

        if (num_fanins == 0)
        {
            layout.move_node(pi2node[actions[current_node]], {x, y});
            placed_node = 1;
            current_pi += 1;
        }
        else if (num_fanins == 1)
        {
            int  layout_node = node_dict[preceding_nodes[0]];
            auto layout_tile = layout.get_tile(layout_node);
            int  signal      = layout.make_signal(layout_node);

            place_node_with_1_input(layout, network, x, y, signal, current_node, actions, current_po, po_names);
            layout.move_node(layout.get_node({x, y}), {x, y}, {});

            using dist = twoddwave_distance_functor<Lyt, uint64_t>;
            using cost = unit_cost_functor<Lyt, uint8_t>;
            static const a_star_params params{true};
            fiction::tile<Lyt>         start{layout_tile.x, layout_tile.y, layout_tile.z};
            fiction::tile<Lyt>         end{x, y, 0};
            auto                       path =
                fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {start, end}, dist(), cost(), params);

            if (path.empty())
            {
                placement_possible = false;
                done               = true;
            }
            else
            {
                fiction::route_path(layout, path);
                placed_node = 1;

                if (network.is_po(actions[current_node]))
                {
                    current_po += 1;
                }

                for (auto el : path)
                {
                    layout.obstruct_coordinate(el);
                }
            }
        }
        else if (num_fanins == 2)
        {
            int  layout_node_1 = node_dict[preceding_nodes[0]];
            auto layout_tile_1 = layout.get_tile(layout_node_1);
            int  signal_1      = layout.make_signal(layout_node_1);

            int  layout_node_2 = node_dict[preceding_nodes[1]];
            auto layout_tile_2 = layout.get_tile(layout_node_2);
            int  signal_2      = layout.make_signal(layout_node_2);

            place_node_with_2_inputs(layout, network, x, y, signal_1, signal_2, current_node, actions);
            layout.move_node(layout.get_node({x, y}), {x, y}, {});

            using dist = twoddwave_distance_functor<Lyt, uint64_t>;
            using cost = unit_cost_functor<Lyt, uint8_t>;
            static const a_star_params params{true};
            fiction::tile<Lyt>         start_1{layout_tile_1.x, layout_tile_1.y, layout_tile_1.z};
            fiction::tile<Lyt>         start_2{layout_tile_2.x, layout_tile_2.y, layout_tile_2.z};
            fiction::coordinate<Lyt>   end{x, y, 0};
            auto                       path_node_1 =
                fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {start_1, end}, dist(), cost(), params);

            if (!path_node_1.empty())
            {
                for (auto el : path_node_1)
                {
                    layout.obstruct_coordinate(el);
                }

                auto path_node_2 = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {start_2, end}, dist(),
                                                                                         cost(), params);

                if (!path_node_2.empty())
                {
                    for (auto el : path_node_2)
                    {
                        layout.obstruct_coordinate(el);
                    }

                    placed_node = 1;
                    fiction::route_path(layout, path_node_1);
                    fiction::route_path(layout, path_node_2);
                }
                else
                {
                    placement_possible = false;
                    done               = true;

                    for (auto el : path_node_1)
                    {
                        layout.clear_obstructed_coordinate(el);
                    }
                }
            }
            else
            {
                placement_possible = false;
                done               = true;
            }
        }
        else
        {
            std::string error_message = "Not a valid node: " + std::to_string(actions[current_node]);
            throw std::runtime_error(error_message);
        }

        if (placed_node == 1)
        {
            node_dict[actions[current_node]] = layout.get_node({x, y});
            current_node += 1;
            layout.obstruct_coordinate({x, y, 0});
            layout.obstruct_coordinate({x, y, 1});
        }

        std::tie(reward, done) = calculate_reward<Ntk, Lyt>(layout, placed_node, current_node, actions,
                                                            placement_possible, max_placed_nodes);
    }

    return {reward, done};
}

template <typename Lyt, typename Ntk>
Lyt reset(int min_layout_width, int min_layout_height, int& current_node, int& current_pi, int& current_po,
          bool& placement_possible, std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>>& node_dict)
{
    fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>> lyt{
        {min_layout_width - 1, min_layout_height - 1, 1},
        twoddwave_clocking<Lyt>()};
    auto layout = fiction::obstruction_layout<
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>>(
        lyt);
    current_node       = 0;
    current_pi         = 0;
    current_po         = 0;
    placement_possible = true;
    node_dict          = {};

    return layout;
}

template <typename Lyt, typename Ntk, typename coord_vector>
std::vector<std::pair<coord_vector, double>>
neighbors(coord_vector                                                                  cur,
          std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash>& sequence_to_coord,
          std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>& came_from, int count,
          bool& improv_mode, int& current_node, int& best_solution, Ntk& network,
          std::vector<mockturtle::node<Ntk>>& actions, std::vector<std::string>& po_names, int& max_placed_nodes,
          std::chrono::time_point<std::chrono::high_resolution_clock> start_time, bool pis_top, bool pis_left, int expansions)
{
    bool      placement_possible = true;
    int       current_pi         = 0;
    int       current_po         = 0;
    const int min_layout_width   = static_cast<int>(network.num_pis());
    const int min_layout_height  = 1;
    const int max_layout_width   = static_cast<int>(actions.size());
    const int max_layout_height  = static_cast<int>(actions.size());
    std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict{};
    std::vector<std::pair<coord_vector, double>> next_actions;
    coord_vector                                 sequence;
    coord_vector                                 start = {{1000, 1000}};
    if (cur != start)
    {
        sequence.push_back(sequence_to_coord[cur]);
        while (came_from[cur] != start)
        {
            cur = came_from[cur];
            sequence.push_back(sequence_to_coord[cur]);
        }
    }
    std::reverse(sequence.begin(), sequence.end());
    Lyt  layout  = detail::reset<Lyt, Ntk>(min_layout_width, min_layout_height, current_node, current_pi, current_po,
                                           placement_possible, node_dict);
    auto pi2node = reserve_input_nodes(layout, network);

    bool done   = false;
    std::vector<std::vector<int>> possible_actions{};
    if (sequence.empty())
    {
        possible_actions = detail::get_possible_actions<Lyt, Ntk>(layout, network, actions, current_node, node_dict,
                                                                  pis_top, pis_left, expansions);
    }

    for (int idx = 0; idx < sequence.size(); ++idx)
    {
        auto action = sequence[idx];

        if (!done)
        {
            double reward = 0;
            std::tie(reward, done) =
                step<Lyt, Ntk>(action, layout, network, placement_possible, current_node, actions, po_names, current_pi,
                               current_po, node_dict, max_placed_nodes, pi2node);
            int area = 0;
            if (improv_mode)
            {
                auto bb = fiction::bounding_box_2d(layout);
                area    = (bb.get_x_size() + 1) * (bb.get_y_size() + 1);
            }
            if (reward > 1000 && (!improv_mode || (area < best_solution)))
            {
                auto bb = fiction::bounding_box_2d(layout);
                layout.resize({bb.get_x_size(), bb.get_y_size(), layout.z()});
                area = (layout.x() + 1) * (layout.y() + 1);
                std::cout << "Found improved solution:" << std::endl;
                // Get the current time point after the operation
                auto end = std::chrono::high_resolution_clock::now();
                // Calculate the duration between start and end
                auto duration_us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time);
                auto duration_ms  = std::chrono::duration_cast<std::chrono::milliseconds>(duration_us);
                auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(duration_us);

                // Extract microseconds, milliseconds, and seconds from the durations
                auto us  = duration_us.count() % 1000;
                auto ms  = duration_ms.count() % 1000;
                auto sec = duration_sec.count();

                // Output the elapsed time
                std::cout << "Time taken: " << sec << " seconds, " << ms << " milliseconds, and " << us
                          << " microseconds" << std::endl;
                std::cout << "Evaluated Paths: " << count << std::endl;
                std::cout << "Layout Dimension: " << layout.x() + 1 << " x " << layout.y() + 1 << " = " << area
                          << std::endl;
                // check equivalence
                fiction::equivalence_checking_stats eq_stats{};
                fiction::equivalence_checking<Ntk, Lyt>(network, layout, &eq_stats);

                const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                              eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                        "NO";
                std::cout << "Equivalent: " << eq_result << std::endl;
                std::cout << "" << std::endl;
                best_solution = area;
                improv_mode   = true;
                using cell_lyt =
                    fiction::cell_level_layout<fiction::qca_technology,
                                               fiction::clocked_layout<fiction::cartesian_layout<offset::ucoord_t>>>;
                const auto cell_level_layout = fiction::apply_gate_library<cell_lyt, fiction::qca_one_library>(layout);
                fiction::write_qca_layout_svg(cell_level_layout, network.get_network_name() + ".svg");
                fiction::write_fgl_layout(layout, network.get_network_name() + ".fgl");
                return {};
            }
            if (improv_mode)
            {
                if (area >= best_solution)
                {
                    return {};
                }
            }
        }
        else
        {
            return {};
        }
        if (action[0] == layout.x() && layout.x() < (max_layout_width - 1))
        {
            layout.resize({layout.x() + 1, layout.y(), layout.z()});
        }
        if (action[1] == layout.y() && layout.y() < (max_layout_height - 1))
        {
            layout.resize({layout.x(), layout.y() + 1, layout.z()});
        }
        if (idx == (sequence.size() - 1))
        {
            if (!valid_layout<Lyt, Ntk>(layout, network, node_dict, placement_possible))
            {
                return {};
            }
            possible_actions = detail::get_possible_actions<Lyt, Ntk>(layout, network, actions, current_node, node_dict,
                                                                      pis_top, pis_left, expansions);
        }
    }
    for (auto action : possible_actions)
    {
        auto new_sequence = sequence;
        new_sequence.push_back(action);
        sequence_to_coord[new_sequence] = action;
        const double rest_actions       = actions.size() - (sequence.size() + 1);
        const double size1 = static_cast<double>(((std::max(static_cast<int>(layout.x() - 1), action[0]) + 1) *
                                                  (std::max(static_cast<int>(layout.y() - 1), action[1]) + 1))) /
                             static_cast<double>((max_layout_width * max_layout_height));
        const double size2 = static_cast<double>(((action[0] + 1) * (action[1] + 1))) /
                             static_cast<double>((max_layout_width * max_layout_height));

        double priority = rest_actions + size1 + size2;
        next_actions.push_back({new_sequence, priority});
    }

    return next_actions;
}

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt a_star_pr(Ntk& ntk, a_star_pr_stats* pst = nullptr, bool high_effort = false) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // Initialize stats for runtime measurement
    a_star_pr_stats stats{};

    // Measure runtime
    {
        using coord_vector = std::vector<std::vector<int>>;
        ntk.substitute_po_signals();
        fiction::fanout_substitution_params params{};
        params.strategy = fiction::fanout_substitution_params::substitution_strategy::BREADTH;
        mockturtle::fanout_view network_substituted_breadth{
            fanout_substitution<mockturtle::names_view<fiction::technology_network>>(ntk, params)};
        params.strategy = fiction::fanout_substitution_params::substitution_strategy::DEPTH;
        mockturtle::fanout_view network_substituted_depth{
            fanout_substitution<mockturtle::names_view<fiction::technology_network>>(ntk, params)};

        fiction::topo_view_co_to_ci network_breadth_co_to_ci{network_substituted_breadth};
        fiction::topo_view_ci_to_co network_breadth_ci_to_co{network_substituted_breadth};
        fiction::topo_view_co_to_ci network_depth_co_to_ci{network_substituted_depth};
        fiction::topo_view_ci_to_co network_depth_ci_to_co{network_substituted_depth};

        std::vector<mockturtle::node<decltype(network_breadth_co_to_ci)>> actions_breadth_co_to_ci{};
        std::vector<mockturtle::node<decltype(network_breadth_ci_to_co)>> actions_breadth_ci_to_co{};
        std::vector<mockturtle::node<decltype(network_depth_co_to_ci)>>   actions_depth_co_to_ci{};
        std::vector<mockturtle::node<decltype(network_depth_co_to_ci)>>   actions_depth_ci_to_co{};

        std::vector<std::string>                         po_names{};

        network_breadth_co_to_ci.foreach_node(
            [&actions_breadth_co_to_ci, &network_breadth_co_to_ci](const auto& n)
            {
                if (!network_breadth_co_to_ci.is_constant(n) & !network_breadth_co_to_ci.is_po(n))
                {
                    actions_breadth_co_to_ci.push_back(n);
                }
            });
        network_breadth_ci_to_co.foreach_node(
            [&actions_breadth_ci_to_co, &network_breadth_ci_to_co](const auto& n)
            {
                if (!network_breadth_ci_to_co.is_constant(n) & !network_breadth_ci_to_co.is_po(n))
                {
                    actions_breadth_ci_to_co.push_back(n);
                }
            });
        network_depth_co_to_ci.foreach_node(
            [&actions_depth_co_to_ci, &network_depth_co_to_ci](const auto& n)
            {
                if (!network_depth_co_to_ci.is_constant(n) & !network_depth_co_to_ci.is_po(n))
                {
                    actions_depth_co_to_ci.push_back(n);
                }
            });
        network_depth_ci_to_co.foreach_node(
            [&actions_depth_ci_to_co, &network_depth_ci_to_co](const auto& n)
            {
                if (!network_depth_ci_to_co.is_constant(n) & !network_depth_ci_to_co.is_po(n))
                {
                    actions_depth_ci_to_co.push_back(n);
                }
            });
        network_substituted_breadth.foreach_co(
            [&actions_breadth_co_to_ci, &actions_breadth_ci_to_co, &actions_depth_co_to_ci, &actions_depth_ci_to_co,
             &network_substituted_breadth, &po_names](const auto& f)
            {
                auto n = network_substituted_breadth.get_node(f);
                if (network_substituted_breadth.is_po(n))
                {
                    actions_breadth_co_to_ci.push_back(n);
                    actions_breadth_ci_to_co.push_back(n);
                    actions_depth_co_to_ci.push_back(n);
                    actions_depth_ci_to_co.push_back(n);
                    po_names.push_back(
                        network_substituted_breadth.get_output_name(network_substituted_breadth.po_index(n)));
                }
            });

        int                                                              current_node       = 0;
        int                                                              max_placed_nodes = 0;
        int                                                              best_solution    = 100000;
        bool                                                             improv_mode      = false;

        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord1;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord2;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord3;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord4;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord5;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord6;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord7;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord8;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord9;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord10;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord11;
        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord12;

        detail::PriorityQueue<coord_vector>                                          frontier1;
        detail::PriorityQueue<coord_vector>                                          frontier2;
        detail::PriorityQueue<coord_vector>                                          frontier3;
        detail::PriorityQueue<coord_vector>                                          frontier4;
        detail::PriorityQueue<coord_vector>                                          frontier5;
        detail::PriorityQueue<coord_vector>                                          frontier6;
        detail::PriorityQueue<coord_vector>                                          frontier7;
        detail::PriorityQueue<coord_vector>                                          frontier8;
        detail::PriorityQueue<coord_vector>                                          frontier9;
        detail::PriorityQueue<coord_vector>                                          frontier10;
        detail::PriorityQueue<coord_vector>                                          frontier11;
        detail::PriorityQueue<coord_vector>                                          frontier12;

        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from1;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from2;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from3;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from4;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from5;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from6;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from7;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from8;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from9;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from10;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from11;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from12;

        coord_vector                                                                 current1  = {{1000, 1000}};
        coord_vector                                                                 current2 = {{1000, 1000}};
        coord_vector                                                                 current3  = {{1000, 1000}};
        coord_vector                                                                 current4  = {{1000, 1000}};
        coord_vector                                                                 current5  = {{1000, 1000}};
        coord_vector                                                                 current6  = {{1000, 1000}};
        coord_vector                                                                 current7  = {{1000, 1000}};
        coord_vector                                                                 current8  = {{1000, 1000}};
        coord_vector                                                                 current9  = {{1000, 1000}};
        coord_vector                                                                 current10 = {{1000, 1000}};
        coord_vector                                                                 current11 = {{1000, 1000}};
        coord_vector                                                                 current12 = {{1000, 1000}};

        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far1;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far2;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far3;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far4;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far5;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far6;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far7;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far8;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far9;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far10;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far11;
        std::unordered_map<coord_vector, double, detail::NestedVectorHash>           cost_so_far12;

        cost_so_far1[current1]   = 0;
        cost_so_far2[current2] = 0;
        cost_so_far3[current3]   = 0;
        cost_so_far4[current4]   = 0;
        cost_so_far5[current5]   = 0;
        cost_so_far6[current6]   = 0;
        cost_so_far7[current7]   = 0;
        cost_so_far8[current8]   = 0;
        cost_so_far9[current9]   = 0;
        cost_so_far10[current10] = 0;
        cost_so_far11[current11] = 0;
        cost_so_far12[current12] = 0;

        Lyt  lyt{{}, twoddwave_clocking<Lyt>()};
        auto layout = fiction::obstruction_layout<Lyt>(lyt);
        int  count  = 0;

        auto start          = std::chrono::high_resolution_clock::now();
        bool first_frontier = true;
        bool second_frontier   = false;
        bool third_frontier    = false;
        bool fourth_frontier   = false;
        bool fifth_frontier    = true;
        bool sixth_frontier    = false;
        bool seventh_frontier  = false;
        bool eighth_frontier   = false;
        bool ninth_frontier    = false;
        bool tenth_frontier    = false;
        bool eleventh_frontier = false;
        bool twelfth_frontier  = false;

        if (high_effort)
        {
            second_frontier   = true;
            third_frontier    = true;
            fourth_frontier   = true;
            sixth_frontier    = true;
            seventh_frontier  = true;
            eighth_frontier   = true;
            ninth_frontier    = true;
            tenth_frontier    = true;
            eleventh_frontier = true;
            twelfth_frontier  = true;
        }

        const mockturtle::stopwatch stop{stats.time_total};
        while (true)
        {
            if (improv_mode && false)
            {
                const auto ortho_layout = fiction::orthogonal<Lyt, Ntk>(ntk);
                /*using cell_lyt = fiction::cell_level_layout<fiction::qca_technology,
                fiction::clocked_layout<fiction::cartesian_layout<offset::ucoord_t>>>; const auto cell_level_layout =
                    fiction::apply_gate_library<cell_lyt, fiction::qca_one_library>(ortho_layout);
                fiction::write_qca_layout_svg(cell_level_layout, ntk.get_network_name() + "_ortho.svg");
                fiction::write_fgl_layout(ortho_layout, ntk.get_network_name() + "_ortho.fgl");*/
                return ortho_layout;
            }
            if (first_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current1, sequence_to_coord1, came_from1, count, improv_mode, current_node, best_solution,
                    network_breadth_co_to_ci, actions_breadth_co_to_ci, po_names, max_placed_nodes, start, true, false, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far1.find(next) == cost_so_far1.end() || cost < cost_so_far1[next])
                    {
                        cost_so_far1[next] = cost;
                        double priority   = cost;
                        frontier1.put(next, priority);
                        came_from1[next] = current1;
                    }
                }
            }
            if (second_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current2, sequence_to_coord2, came_from2, count, improv_mode, current_node, best_solution,
                    network_breadth_ci_to_co, actions_breadth_ci_to_co, po_names, max_placed_nodes, start, true, false, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far2.find(next) == cost_so_far2.end() || cost < cost_so_far2[next])
                    {
                        cost_so_far2[next] = cost;
                        double priority    = cost;
                        frontier2.put(next, priority);
                        came_from2[next] = current2;
                    }
                }
            }
            if (third_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current3, sequence_to_coord3, came_from3, count, improv_mode, current_node, best_solution,
                    network_depth_co_to_ci, actions_depth_co_to_ci, po_names, max_placed_nodes, start, true, false, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far3.find(next) == cost_so_far3.end() || cost < cost_so_far3[next])
                    {
                        cost_so_far3[next] = cost;
                        double priority    = cost;
                        frontier3.put(next, priority);
                        came_from3[next] = current3;
                    }
                }
            }
            if (fourth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current4, sequence_to_coord4, came_from4, count, improv_mode, current_node, best_solution,
                    network_depth_ci_to_co, actions_depth_ci_to_co, po_names, max_placed_nodes, start, true, false, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far4.find(next) == cost_so_far4.end() || cost < cost_so_far4[next])
                    {
                        cost_so_far4[next] = cost;
                        double priority    = cost;
                        frontier4.put(next, priority);
                        came_from4[next] = current4;
                    }
                }
            }
            if (fifth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current5, sequence_to_coord5, came_from5, count, improv_mode, current_node, best_solution,
                    network_breadth_co_to_ci, actions_breadth_co_to_ci, po_names, max_placed_nodes, start, false, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far5.find(next) == cost_so_far5.end() || cost < cost_so_far5[next])
                    {
                        cost_so_far5[next] = cost;
                        double priority    = cost;
                        frontier5.put(next, priority);
                        came_from5[next] = current5;
                    }
                }
            }
            if (sixth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current6, sequence_to_coord6, came_from6, count, improv_mode, current_node, best_solution,
                    network_breadth_ci_to_co, actions_breadth_ci_to_co, po_names, max_placed_nodes, start, false, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far6.find(next) == cost_so_far6.end() || cost < cost_so_far6[next])
                    {
                        cost_so_far6[next] = cost;
                        double priority    = cost;
                        frontier6.put(next, priority);
                        came_from6[next] = current6;
                    }
                }
            }
            if (seventh_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current7, sequence_to_coord7, came_from7, count, improv_mode, current_node, best_solution,
                    network_depth_co_to_ci, actions_depth_co_to_ci, po_names, max_placed_nodes, start, false, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far7.find(next) == cost_so_far7.end() || cost < cost_so_far7[next])
                    {
                        cost_so_far7[next] = cost;
                        double priority    = cost;
                        frontier7.put(next, priority);
                        came_from7[next] = current7;
                    }
                }
            }
            if (eighth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current8, sequence_to_coord8, came_from8, count, improv_mode, current_node, best_solution,
                    network_depth_ci_to_co, actions_depth_ci_to_co, po_names, max_placed_nodes, start, false, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far8.find(next) == cost_so_far8.end() || cost < cost_so_far8[next])
                    {
                        cost_so_far8[next] = cost;
                        double priority    = cost;
                        frontier8.put(next, priority);
                        came_from8[next] = current8;
                    }
                }
            }
            if (ninth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current9, sequence_to_coord9, came_from9, count, improv_mode, current_node, best_solution,
                    network_breadth_co_to_ci, actions_breadth_co_to_ci, po_names, max_placed_nodes, start, true, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far9.find(next) == cost_so_far9.end() || cost < cost_so_far9[next])
                    {
                        cost_so_far9[next] = cost;
                        double priority    = cost;
                        frontier9.put(next, priority);
                        came_from9[next] = current9;
                    }
                }
            }
            if (tenth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current10, sequence_to_coord10, came_from10, count, improv_mode, current_node, best_solution,
                    network_breadth_ci_to_co, actions_breadth_ci_to_co, po_names, max_placed_nodes, start, true, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far10.find(next) == cost_so_far10.end() || cost < cost_so_far10[next])
                    {
                        cost_so_far10[next] = cost;
                        double priority     = cost;
                        frontier10.put(next, priority);
                        came_from10[next] = current10;
                    }
                }
            }
            if (eleventh_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current11, sequence_to_coord11, came_from11, count, improv_mode, current_node, best_solution,
                    network_depth_co_to_ci, actions_depth_co_to_ci, po_names, max_placed_nodes, start, true, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far11.find(next) == cost_so_far11.end() || cost < cost_so_far11[next])
                    {
                        cost_so_far11[next] = cost;
                        double priority     = cost;
                        frontier11.put(next, priority);
                        came_from11[next] = current11;
                    }
                }
            }
            if (twelfth_frontier)
            {
                count++;
                auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                    current12, sequence_to_coord12, came_from12, count, improv_mode, current_node, best_solution,
                    network_depth_ci_to_co, actions_depth_ci_to_co, po_names, max_placed_nodes, start, true, true, 4);
                for (const auto& [next, cost] : neighbors)
                {
                    if (cost_so_far12.find(next) == cost_so_far12.end() || cost < cost_so_far12[next])
                    {
                        cost_so_far12[next] = cost;
                        double priority     = cost;
                        frontier12.put(next, priority);
                        came_from12[next] = current12;
                    }
                }
            }
            if (first_frontier)
            {
                if (!frontier1.empty())
                {
                    current1 = frontier1.get();
                }
                else
                {
                    first_frontier = false;
                }
            }
            if (second_frontier)
            {
                if (!frontier2.empty())
                {
                    current2       = frontier2.get();
                }
                else
                {
                    second_frontier = false;
                }
            }
            if (third_frontier)
            {
                if (!frontier3.empty())
                {
                    current3 = frontier3.get();
                }
                else
                {
                    third_frontier = false;
                }
            }
            if (fourth_frontier)
            {
                if (!frontier4.empty())
                {
                    current4 = frontier4.get();
                }
                else
                {
                    fourth_frontier = false;
                }
            }
            if (fifth_frontier)
            {
                if (!frontier5.empty())
                {
                    current5 = frontier5.get();
                }
                else
                {
                    fifth_frontier = false;
                }
            }
            if (sixth_frontier)
            {
                if (!frontier6.empty())
                {
                    current6 = frontier6.get();
                }
                else
                {
                    sixth_frontier = false;
                }
            }
            if (seventh_frontier)
            {
                if (!frontier7.empty())
                {
                    current7 = frontier7.get();
                }
                else
                {
                    seventh_frontier = false;
                }
            }
            if (eighth_frontier)
            {
                if (!frontier8.empty())
                {
                    current8 = frontier8.get();
                }
                else
                {
                    eighth_frontier = false;
                }
            }
            if (ninth_frontier)
            {
                if (!frontier9.empty())
                {
                    current9 = frontier9.get();
                }
                else
                {
                    ninth_frontier = false;
                }
            }
            if (tenth_frontier)
            {
                if (!frontier10.empty())
                {
                    current10 = frontier10.get();
                }
                else
                {
                    tenth_frontier = false;
                }
            }
            if (eleventh_frontier)
            {
                if (!frontier11.empty())
                {
                    current11 = frontier11.get();
                }
                else
                {
                    eleventh_frontier = false;
                }
            }
            if (twelfth_frontier)
            {
                if (!frontier12.empty())
                {
                    current12 = frontier12.get();
                }
                else
                {
                    twelfth_frontier = false;
                }
            }

            if (!(first_frontier || second_frontier || third_frontier || fourth_frontier || fifth_frontier ||
                  sixth_frontier || seventh_frontier || eighth_frontier || ninth_frontier || tenth_frontier ||
                  eleventh_frontier || twelfth_frontier))
            {
                break;
            }
        }

        std::cout << "Total paths: " << count << std::endl;
    }

    // Record runtime statistics if a valid pointer is provided
    if (pst != nullptr)
    {
        *pst = stats;
    }

    return fiction::orthogonal<Lyt, Ntk>(ntk);
}

}  // namespace fiction

#endif  // FICTION_A_STAR_PR_HPP