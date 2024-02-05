//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_A_STAR_PR_HPP
#define FICTION_A_STAR_PR_HPP

#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/algorithms/verification/equivalence_checking.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <fiction/utils/debug/network_writer.hpp>  // DOT writer for logic networks and layouts

#include <mockturtle/networks/klut.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>
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

template <typename Lyt, typename Ntk, typename coord_vector>
std::vector<std::vector<int>>
get_possible_actions(Lyt& layout, Ntk& network, std::vector<mockturtle::node<Ntk>> actions, int current_node,
                     std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict)
{
    std::vector<int> preceding_nodes{};
    network.foreach_fanin(actions[current_node], [&preceding_nodes](const auto& f) { preceding_nodes.push_back(f); });
    std::vector<std::vector<int>> possible_positions_nodes{};

    if (network.is_pi(actions[current_node]))
    {
        for (int x = 0; x <= layout.x(); ++x)
        {
            fiction::coordinate<Lyt> coord{x, layout.y(), 0};
            std::vector<int>         tile{x, static_cast<int>(layout.y()), 0};

            if (layout.is_empty_tile(coord))
            {
                possible_positions_nodes.push_back(tile);
            }
        }
        for (int y = 0; y < layout.y(); ++y)
        {
            fiction::coordinate<Lyt> coord{layout.x(), y, 0};
            std::vector<int>         tile{static_cast<int>(layout.x()), y, 0};

            if (layout.is_empty_tile(coord))
            {
                possible_positions_nodes.push_back(tile);
            }
        }
    }
    else if (network.is_po(actions[current_node]))
    {
        auto loc = layout.get_tile(node_dict[preceding_nodes[0]]);
        for (int x = loc.x; x <= layout.x(); ++x)
        {
            fiction::coordinate<Lyt> coord{x, layout.y(), 0};
            std::vector<int>         tile{x, static_cast<int>(layout.y()), 0};

            if (layout.is_empty_tile(coord))
            {
                possible_positions_nodes.push_back(tile);
            }
        }

        for (int y = loc.y; y <= layout.y(); ++y)
        {
            fiction::coordinate<Lyt> coord{layout.x(), y, 0};
            std::vector<int>         tile{static_cast<int>(layout.x()), y, 0};

            if (layout.is_empty_tile(coord))
            {
                possible_positions_nodes.push_back(tile);
            }
        }
    }
    else if (preceding_nodes.size() == 1)
    {
        int  node           = node_dict[preceding_nodes[0]];
        auto loc            = layout.get_tile(node);
        auto outgoing_zones = layout.outgoing_clocked_zones(loc);
        for (const auto& zone : outgoing_zones)
        {
            std::vector<int> tile{zone.x, zone.y, 0};
            if ((zone.x <= layout.x()) && (zone.y <= layout.y()) && layout.is_empty_tile({zone.x, zone.y, 0}))
            {
                possible_positions_nodes.push_back(tile);
            }
        }
    }
    else if (preceding_nodes.size() == 2)
    {
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
        for (int x = min_x; x <= layout.x(); ++x)
        {
            for (int y = min_y; y <= layout.y(); ++y)
            {
                fiction::coordinate<Lyt> coord{x, y, 0};
                std::vector<int>         tile{x, y, 0};

                if (layout.is_empty_tile(coord))
                {
                    possible_positions_nodes.push_back(tile);
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
    for (auto& node : node_dict)
    {
        if ((!layout.is_po_tile(layout.get_tile(node.second)) && (layout.fanout_size(node.second) == 0)) ||
            ((layout.fanout_size(node.second) == 1) && network.is_fanout(node.first)))
        {
            auto tile  = layout.get_tile(node.second);
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
    }
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

template <typename Ntk>
std::tuple<double, bool> calculate_reward(bool placed_node, int current_node,
                                          std::vector<mockturtle::node<Ntk>> actions, bool placement_possible,
                                          int& max_placed_nodes)
{
    double reward = (current_node == actions.size()) ? 10000.0 : (placed_node ? 1.0 : 0.0);
    bool   done   = (current_node == actions.size()) || !placement_possible;

    if (current_node > max_placed_nodes)
    {
        std::cout << "New best placement: " << current_node << "/" << actions.size() << std::endl;
        max_placed_nodes = current_node;
    }

    return std::make_tuple(reward, done);
}

template <typename Lyt, typename Ntk, typename coord_vector>
std::pair<double, bool>
step(std::vector<int> action, Lyt& layout, Ntk& network, bool& placement_possible, int& current_node,
     std::vector<mockturtle::node<Ntk>> actions, std::vector<std::string> pi_names, std::vector<std::string> po_names,
     int& current_pi, int& current_po, std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict,
     int& max_placed_nodes)
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
            layout.create_pi(pi_names[current_pi], {x, y});
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

                    for (auto el : path_node_1)
                    {
                        layout.clear_obstructed_coordinate(el);
                    }
                }
            }
            else
            {
                placement_possible = false;
            }
        }
        else
        {
            std::string error_message = "Not a valid node: " + std::to_string(actions[current_node]);
            throw std::runtime_error(error_message);
        }

        node_dict[actions[current_node]] = layout.get_node({x, y});

        if (placed_node == 1)
        {
            current_node += 1;
            layout.obstruct_coordinate({x, y, 0});
            layout.obstruct_coordinate({x, y, 1});
        }

        std::tie(reward, done) =
            calculate_reward<Ntk>(placed_node, current_node, actions, placement_possible, max_placed_nodes);
    }

    return {reward, done};
}

template <typename Lyt, typename Ntk, typename coord_vector>
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
          int max_layout_width, int max_layout_height, bool& improv_mode, int min_layout_width, int min_layout_height,
          int& current_node, int& current_pi, int& current_po, bool& placement_possible,
          std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict, int& best_solution, Ntk& network,
          std::vector<mockturtle::node<Ntk>> actions, std::vector<std::string> pi_names,
          std::vector<std::string> po_names, int& max_placed_nodes)
{
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
    Lyt  layout = detail::reset<Lyt, Ntk, coord_vector>(min_layout_width, min_layout_height, current_node, current_pi,
                                                        current_po, placement_possible, node_dict);
    bool done   = false;
    std::vector<std::vector<int>> possible_actions{};
    if (sequence.empty())
    {
        possible_actions =
            detail::get_possible_actions<Lyt, Ntk, coord_vector>(layout, network, actions, current_node, node_dict);
    }

    for (int idx = 0; idx < sequence.size(); ++idx)
    {
        auto action = sequence[idx];

        if (!done)
        {
            double reward;
            std::tie(reward, done) =
                step<Lyt, Ntk, coord_vector>(action, layout, network, placement_possible, current_node, actions,
                                             pi_names, po_names, current_pi, current_po, node_dict, max_placed_nodes);
            int area = 0;
            /*if (improv_mode)
            {
                auto bb = fiction::bounding_box_2d(layout);
                area    = (bb.get_x_size() + 1) * (bb.get_y_size() + 1);
            }*/
            if (reward > 1000)  // && (!improv_mode || (improv_mode && (area < best_solution))))
            {
                auto bb = fiction::bounding_box_2d(layout);
                layout.resize({bb.get_x_size(), bb.get_y_size(), layout.z()});
                area = (layout.x() + 1) * (layout.y() + 1);
                std::cout << "Found improved solution:" << std::endl;
                // std::cout << "Total Time: " << time() - start << std::endl;
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

                return {};
            }
            /*if (improv_mode)
            {
                if (area >= best_solution)
                {
                    return {};
                }
            }*/
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
            possible_actions =
                detail::get_possible_actions<Lyt, Ntk, coord_vector>(layout, network, actions, current_node, node_dict);
        }
    }
    for (auto action : possible_actions)
    {
        auto new_sequence = sequence;
        new_sequence.push_back(action);
        sequence_to_coord[new_sequence] = action;
        auto bb                         = fiction::bounding_box_2d(layout);
        auto max_el                     = bb.get_max();
        // double size                     = ((max_el.x + 1) * (max_el.y + 1)) / (max_layout_width * max_layout_height);
        // double size2                    = ((action[0] + 1) * (action[1] + 1)) / (max_layout_width *
        // max_layout_height);
        next_actions.push_back({new_sequence, ((actions.size()) - (sequence.size() + 1))});  // + size + size2});
    }

    return next_actions;
}

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt a_star_pr(const Ntk& ntk, a_star_pr_stats* pst = nullptr) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // Initialize stats for runtime measurement
    a_star_pr_stats stats{};

    // Measure runtime
    {
        const mockturtle::stopwatch stop{stats.time_total};
        using coord_vector = std::vector<std::vector<int>>;
        fiction::debug::write_dot_network(ntk);
        mockturtle::fanout_view network_substituted{
            fanout_substitution<mockturtle::names_view<technology_network>>(ntk)};
        mockturtle::topo_view network{network_substituted};
        network.substitute_po_signals();
        int                                              min_layout_width  = 1;
        int                                              min_layout_height = 1;
        int                                              max_layout_width  = 100;
        int                                              max_layout_height = 100;
        std::vector<mockturtle::node<decltype(network)>> actions{};
        fiction::debug::write_dot_network(network);
        network.foreach_node(
            [&actions, &network](const auto& n, [[maybe_unused]] const auto i)
            {
                if (!network.is_constant(n))
                {
                    actions.push_back(n);
                }
            });

        std::vector<std::string> pi_names{};
        ntk.foreach_pi([&pi_names, &ntk](const auto& pi) { pi_names.push_back(ntk.get_name(pi)); });

        std::vector<std::string> po_names{};
        ntk.foreach_po([&po_names, &ntk](const auto& po)
                       { po_names.push_back(ntk.get_output_name(ntk.po_index(po))); });

        int                                                              current_node       = 0;
        int                                                              current_pi         = 0;
        int                                                              current_po         = 0;
        bool                                                             placement_possible = true;
        std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict{};
        int                                                              max_placed_nodes = 0;
        int                                                              best_solution    = 100000;
        bool                                                             improv_mode      = false;

        std::unordered_map<coord_vector, std::vector<int>, detail::NestedVectorHash> sequence_to_coord;
        detail::PriorityQueue<coord_vector>                                          frontier;
        std::unordered_map<coord_vector, coord_vector, detail::NestedVectorHash>     came_from;
        coord_vector                                                                 current = {{1000, 1000}};
        std::unordered_map<coord_vector, bool, detail::NestedVectorHash>             cost_so_far;
        cost_so_far[current] = 0;
        Lyt  lyt{{min_layout_width - 1, min_layout_height - 1, 1}, twoddwave_clocking<Lyt>()};
        auto layout = fiction::obstruction_layout<Lyt>(lyt);
        int  count  = 0;

        while (true)
        {
            count++;
            auto neighbors = detail::neighbors<decltype(layout), Ntk, coord_vector>(
                current, sequence_to_coord, came_from, count, max_layout_width, max_layout_height, improv_mode,
                min_layout_width, min_layout_height, current_node, current_pi, current_po, placement_possible,
                node_dict, best_solution, network, actions, pi_names, po_names, max_placed_nodes);
            for (const auto& [next, cost] : neighbors)
            {
                if (cost_so_far.find(next) == cost_so_far.end() || cost < cost_so_far[next])
                {
                    cost_so_far[next] = cost;
                    int priority      = cost;
                    frontier.put(next, priority);
                    came_from[next] = current;
                }
            }
            if (frontier.empty())
            {
                break;
            }
            else
            {
                current = frontier.get();
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