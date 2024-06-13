//
// Created by Simon Hofmann on 30.01.24.
//
#ifndef FICTION_A_STAR_PR_HPP
#define FICTION_A_STAR_PR_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/path_finding/a_star.hpp"
#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/algorithms/verification/equivalence_checking.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/layouts/tile_based_layout.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/networks/views/topo_view_ci_to_co.hpp"
#include "fiction/networks/views/topo_view_co_to_ci.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <fmt/core.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Parameters for the A* P&R algorithm.
 */
struct a_star_pr_params
{
    /**
     * High effort mode.
     */
    bool high_effort = false;
    /**
     * Timeout limit.
     */
    uint64_t timeout;
    /**
     * Verbosity.
     */
    bool verbose = false;
};
/**
 * This struct stores statistics about the A* P&R process.
 */
struct a_star_pr_stats
{
    /**
     * Runtime of the A* P&R process.
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
 * Alias for a vector of tiles in a given layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using coord_vec_type = std::vector<fiction::tile<Lyt>>;
/**
 * This struct defines a hash function for a nested vector of layout tiles.
 * It calculates a combined hash value for a vector of tiles based on the coordinates of each tile.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
struct nested_vector_hash
{
    /**
     * Computes a hash value for a vector of `fiction::tile` objects.
     *
     * @param vec The vector of tiles to be hashed.
     * @return A combined hash value for the vector of tiles.
     */
    size_t operator()(const coord_vec_type<Lyt>& vec) const
    {
        size_t       hash  = 0;
        const size_t prime = 0x9e3779b9;
        for (const auto& tile : vec)
        {
            hash ^= std::hash<uint64_t>{}(tile.x) + prime + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint64_t>{}(tile.y) + prime + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint64_t>{}(tile.z) + prime + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};
/**
 * A priority queue class for managing elements with associated priorities.
 * The elements are stored in a priority queue, with the highest priority elements being
 * retrieved first.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
class priority_queue
{
    // Counter to keep track of insertion order
    size_t counter = 0;

    // Priority queue with tuples of (priority, insertion_order, element)
    using queue_element = std::tuple<double, size_t, coord_vec_type<Lyt>>;
    std::priority_queue<queue_element, std::vector<queue_element>, std::greater<queue_element>> elements;

  public:
    /**
     * Checks if the priority queue is empty.
     *
     * @return True if the priority queue is empty, false otherwise.
     */
    [[nodiscard]] bool empty() const
    {
        return elements.empty();
    }

    /**
     * Adds an element to the priority queue with a given priority.
     *
     * @param item The element to be added.
     * @param priority The priority of the element.
     */
    void put(const coord_vec_type<Lyt>& item, double priority)
    {
        elements.emplace(priority, counter++, item);
    }

    /**
     * Retrieves and removes the element with the highest priority from the queue.
     *
     * @return The element with the highest priority.
     */
    coord_vec_type<Lyt> get()
    {
        coord_vec_type<Lyt> item = std::get<2>(elements.top());
        elements.pop();
        return item;
    }
};
/**
 * Alias for a distance functor on 2DDWave-clocked layouts.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using dist = twoddwave_distance_functor<Lyt, uint64_t>;
/**
 * Alias for a cost functor with unit cost.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using cost = unit_cost_functor<Lyt, uint8_t>;
/**
 * Parameters for the A* algorithm with wire-crossings enabled.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
const a_star_params A_STAR_PARAMS{true};
/**
 * Alias for a dictionary that maps nodes from a mockturtle network to nodes in a layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Type of the mockturtle network.
 */
template <typename Lyt, typename Ntk>
using node_dict_type = std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>>;
/**
 * Alias for a priority queue containing vertices of the search space graph.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using frontiers_type = detail::priority_queue<Lyt>;
/**
 * Alias for an unordered map that maps a vertex in the search space graph to its accumulated cost value.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 */
template <typename Lyt>
using cost_so_far_type = std::unordered_map<coord_vec_type<Lyt>, double, detail::nested_vector_hash<Lyt>>;
/**
 * A structure representing a search space graph.
 *
 * This struct encapsulates all the necessary data for managing a search space graph
 * during the A*-based placement and routing algorithm. It holds the current vertex,
 * network, nodes to be placed, and other relevant information.
 *
 * @tparam Lyt The layout type.
 * @tparam Ntk The network type.
 */
template <typename Lyt>
struct SearchSpaceGraph
{
    /**
     * The current vertex in the search space graph.
     */
    coord_vec_type<Lyt> current_vertex;
    /**
     * The network associated with this search space graph.
     */
    fiction::tec_nt network;
    /**
     * Topological list of nodes to be placed in the layout.
     */
    std::vector<mockturtle::node<fiction::tec_nt>> nodes_to_place;
    /**
     * Primary output (PO) names.
     */
    std::vector<std::string> po_names;
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the top.
     */
    bool pis_top = false;
    /**
     * Flag indicating if primary inputs (PIs) can be placed at the left.
     */
    bool pis_left = false;
    /**
     * Flag indicating if this graph's frontier is active.
     */
    bool frontier_flag = false;
    /**
     * The cost so far for reaching each vertex in the layout.
     */
    cost_so_far_type<Lyt> cost_so_far;
    /**
     * The maximum number of positions to be considered for expansions.
     */
    uint64_t num_expansions;
    /**
     * Priority queue containing vertices of the search space graph.
     */
    frontiers_type<Lyt> frontier;
};
/**
 * Checks if there is a path between the source and destination tiles in the given layout.
 * Optionally returns the path if specified.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param layout The layout to be checked.
 * @param src The source tile.
 * @param dest The destination tile.
 * @param src_is_new_pos Flag indicating if the source is a new position and should be checked if empty. Defaults to
 * false.
 * @param dest_is_new_pos Flag indicating if the destination is a new position and should be checked if empty. Defaults
 * to false.
 * @param return_path Flag indicating if the path should be returned. Defaults to false.
 * @return A pair where the first element is a boolean indicating if a path exists,
 * and the second element is an optional path (returned if `return_path` is true).
 */
template <typename Lyt>
std::pair<bool, std::optional<fiction::layout_coordinate_path<Lyt>>>
check_path(Lyt& layout, fiction::tile<Lyt>& src, fiction::tile<Lyt>& dest, bool src_is_new_pos = false,
           bool dest_is_new_pos = false, bool return_path = false)
{
    if ((layout.is_empty_tile(src) & src_is_new_pos) || (layout.is_empty_tile(dest) & dest_is_new_pos) ||
        (!src_is_new_pos && !dest_is_new_pos))
    {
        auto path = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(layout, {src, dest}, dist<Lyt>(), cost<Lyt>(),
                                                                          A_STAR_PARAMS<Lyt>);

        if (!path.empty())
        {
            if (return_path)
            {
                return std::pair(true, path);
            }
            return std::pair(true, std::nullopt);
        }
    }
    return std::pair(false, std::nullopt);
}
/**
 * Retrieves the possible positions for Primary Inputs (PIs) in the given layout based on the specified
 * criteria of positioning at the top or left side, with a limit on the number of possible positions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param layout The layout in which to find the possible positions for PIs.
 * @param pis_top Flag indicating if PIs are allowed at the top side of the layout.
 * @param pis_left Flag indicating if PIs are allowed at the left side of the layout.
 * @param expansions The maximum number of positions to be returned (is doubled for PIs).
 * @return A vector of tiles representing the possible positions for PIs.
 * @throws std::runtime_error If neither top nor left positions are allowed for PIs.
 */
template <typename Lyt>
std::vector<fiction::tile<Lyt>> get_possible_positions_pis(Lyt& layout, bool pis_top, bool pis_left,
                                                           uint64_t expansions)
{
    uint64_t                        count = 0;
    std::vector<fiction::tile<Lyt>> possible_positions{};

    layout.resize({layout.x() + 1, layout.y() + 1, 1});
    fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};

    if (pis_top && pis_left)
    {
        for (uint64_t k = 0; k < std::max(layout.x(), layout.y()); k++)
        {
            fiction::tile<Lyt> tile_top{k, 0, 0};

            if (detail::check_path(layout, tile_top, drain, true, false, false).first)
            {
                count++;
                possible_positions.push_back(tile_top);
            }

            fiction::tile<Lyt> tile_left{0, k, 0};

            if (detail::check_path(layout, tile_left, drain, true, false, false).first)
            {
                count++;
                possible_positions.push_back(tile_left);
            }

            if (count >= 2 * expansions)
            {
                layout.resize({layout.x() - 1, layout.y() - 1, 1});
                return possible_positions;
            }
        }
    }
    else if (pis_top)
    {
        for (uint64_t k = 0; k < layout.x(); k++)
        {
            fiction::tile<Lyt> tile_top{k, 0, 0};

            if (detail::check_path(layout, tile_top, drain, true, false, false).first)
            {
                count++;
                possible_positions.push_back(tile_top);
            }

            if (count >= expansions)
            {
                layout.resize({layout.x() - 1, layout.y() - 1, 1});
                return possible_positions;
            }
        }
    }
    else if (pis_left)
    {
        for (uint64_t k = 0; k < layout.y(); k++)
        {
            fiction::tile<Lyt> tile_left{0, k, 0};

            if (detail::check_path(layout, tile_left, drain, true, false, false).first)
            {
                count++;
                possible_positions.push_back(tile_left);
            }

            if (count >= expansions)
            {
                layout.resize({layout.x() - 1, layout.y() - 1, 1});
                return possible_positions;
            }
        }
    }
    else
    {
        throw std::runtime_error("Allowed positions for PIs have to be specified");
    }
    layout.resize({layout.x() - 1, layout.y() - 1, 1});
    return possible_positions;
}
/**
 * Retrieves the possible positions for Primary Outputs (POs) in the given layout based on the positions
 * of the preceding nodes.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to find the possible positions for POs.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @param preceding_nodes A vector of nodes that precede the PO nodes.
 * @return A vector of tiles representing the possible positions for POs.
 */
template <typename Lyt, typename Ntk>
std::vector<fiction::tile<Lyt>> get_possible_positions_pos(Lyt& layout, node_dict_type<Lyt, Ntk>& node_dict,
                                                           std::vector<mockturtle::node<Ntk>>& preceding_nodes)
{
    std::vector<fiction::tile<Lyt>> possible_positions{};
    auto                            loc = layout.get_tile(node_dict[preceding_nodes[0]]);
    for (uint64_t k = 0; k <= std::max(layout.x() - loc.x, layout.y() - loc.y); ++k)
    {
        fiction::tile<Lyt> tile_bottom_border{loc.x + k, static_cast<int>(layout.y()), 0};
        fiction::tile<Lyt> src{loc.x, loc.y, 0};

        if (detail::check_path(layout, src, tile_bottom_border, false, true, false).first)
        {
            possible_positions.push_back(tile_bottom_border);
        }

        fiction::tile<Lyt> tile_right_border{static_cast<int>(layout.x()), loc.y + k, 0};

        if (detail::check_path(layout, src, tile_right_border, false, true, false).first)
        {
            possible_positions.push_back(tile_right_border);
        }
    }
    return possible_positions;
}
/**
 * Retrieves the possible positions for a single fan-in node in the given layout, based on the positions
 * of preceding nodes and a specified number of expansions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to find the possible positions for a single fan-in node.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @param expansions The maximum number of positions to be returned.
 * @param preceding_nodes A vector of nodes that precede the single fanin node.
 * @return A vector of tiles representing the possible positions for a single fan-in node.
 */
template <typename Lyt, typename Ntk>
std::vector<fiction::tile<Lyt>> get_possible_positions_single_fanin(Lyt& layout, node_dict_type<Lyt, Ntk>& node_dict,
                                                                    uint64_t                            expansions,
                                                                    std::vector<mockturtle::node<Ntk>>& preceding_nodes)
{
    std::vector<fiction::tile<Lyt>> possible_positions{};
    uint64_t                        count = 0;
    auto                            node  = node_dict[preceding_nodes[0]];
    auto                            loc   = layout.get_tile(node);
    // iterate through cartesian layout diagonally
    for (uint64_t k = 0; k < layout.x() + layout.y() + 1; ++k)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const auto y = k - x;
            if ((loc.y + y) <= layout.y() && (loc.x + x) <= layout.x())
            {
                fiction::tile<Lyt> new_pos{loc.x + x, loc.y + y, 0};
                fiction::tile<Lyt> src{loc.x, loc.y, 0};

                if (detail::check_path(layout, src, new_pos, false, true, false).first)
                {
                    layout.resize({layout.x() + 1, layout.y() + 1, 1});
                    fiction::tile<Lyt> drain{layout.x(), layout.y(), 0};

                    if (detail::check_path(layout, new_pos, drain, true, false, false).first)
                    {
                        possible_positions.push_back(new_pos);
                        count++;
                    }
                    layout.resize({layout.x() - 1, layout.y() - 1, 1});
                }
            }
            if (count >= expansions)
            {
                return possible_positions;
            }
        }
    }
    return possible_positions;
}
/**
 * Retrieves the possible positions for a double fan-in node in the given layout, based on the positions
 * of preceding nodes and a specified number of expansions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to find the possible positions for a double fan-in node.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @param expansions The maximum number of positions to be returned.
 * @param preceding_nodes A vector of nodes that precede the double fanin node.
 * @return A vector of tiles representing the possible positions for a double fan-in node.
 */
template <typename Lyt, typename Ntk>
std::vector<fiction::tile<Lyt>> get_possible_positions_double_fanin(Lyt& layout, node_dict_type<Lyt, Ntk>& node_dict,
                                                                    uint64_t                            expansions,
                                                                    std::vector<mockturtle::node<Ntk>>& preceding_nodes)
{
    std::vector<fiction::tile<Lyt>> possible_positions{};
    uint64_t                        count  = 0;
    auto                            node_1 = node_dict[preceding_nodes[0]];
    auto                            loc_1  = layout.get_tile(node_1);
    auto                            node_2 = node_dict[preceding_nodes[1]];
    auto                            loc_2  = layout.get_tile(node_2);
    auto                            min_x  = std::max(loc_1.x, loc_2.x);
    auto                            min_y  = std::max(loc_1.y, loc_2.y);
    if (loc_1.x == loc_2.x)
    {
        min_x += 1;
    }
    if (loc_1.y == loc_2.y)
    {
        min_y += 1;
    }
    for (uint64_t k = 0; k < layout.x() + layout.y() + 1; ++k)
    {
        for (uint64_t x = 0; x < k + 1; ++x)
        {
            const auto y = k - x;
            if ((min_y + y) <= layout.y() && (min_x + x) <= layout.x())
            {
                fiction::tile<Lyt> tile{min_x + x, min_y + y, 0};
                fiction::tile<Lyt> start_1{loc_1.x, loc_1.y, loc_1.z};
                fiction::tile<Lyt> start_2{loc_2.x, loc_2.y, loc_2.z};

                auto path_1 = detail::check_path(layout, start_1, tile, false, true, true);
                if (path_1.first)
                {
                    for (auto el : *path_1.second)
                    {
                        layout.obstruct_coordinate(el);
                    }

                    if (detail::check_path(layout, start_2, tile, false, true, false).first)
                    {
                        layout.resize({layout.x() + 1, layout.y() + 1, 1});
                        fiction::coordinate<Lyt> drain{layout.x(), layout.y(), 0};

                        if (detail::check_path(layout, tile, drain, true, false, false).first)
                        {
                            possible_positions.push_back(tile);
                            count++;
                        }

                        layout.resize({layout.x() - 1, layout.y() - 1, 1});
                    }
                    for (auto el : *path_1.second)
                    {
                        layout.clear_obstructed_coordinate(el);
                    }
                }
            }
            if (count >= expansions)
            {
                return possible_positions;
            }
        }
    }
    return possible_positions;
}
/**
 * Retrieves the possible positions for a given node in the layout based on its type and preceding nodes.
 * It determines the type of the node (PI, PO, single fan-in, double fan-in) and returns the corresponding
 * possible positions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to find the possible positions.
 * @param search_space_graph The search space graph.
 * @param current_node The current node index in the nodes_to_place vector.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @return A vector of tiles representing the possible positions for the current node.
 * @throws std::runtime_error If the node type is invalid.
 */
template <typename Lyt, typename Ntk>
std::vector<fiction::tile<Lyt>> get_possible_positions(Lyt& layout, SearchSpaceGraph<Lyt>& search_space_graph,
                                                       uint64_t& current_node, node_dict_type<Lyt, Ntk>& node_dict)
{
    std::vector<mockturtle::node<Ntk>> preceding_nodes{};
    search_space_graph.network.foreach_fanin(search_space_graph.nodes_to_place[current_node],
                                             [&preceding_nodes](const auto& f) { preceding_nodes.push_back(f); });

    if (search_space_graph.network.is_pi(search_space_graph.nodes_to_place[current_node]))
    {
        return detail::get_possible_positions_pis(layout, search_space_graph.pis_top, search_space_graph.pis_left,
                                                  search_space_graph.network.num_pis());
    }
    if (search_space_graph.network.is_po(search_space_graph.nodes_to_place[current_node]))
    {
        return detail::get_possible_positions_pos<Lyt, Ntk>(layout, node_dict, preceding_nodes);
    }
    if (preceding_nodes.size() == 1)
    {
        return detail::get_possible_positions_single_fanin<Lyt, Ntk>(
            layout, node_dict, search_space_graph.num_expansions, preceding_nodes);
    }
    if (preceding_nodes.size() == 2)
    {
        return detail::get_possible_positions_double_fanin<Lyt, Ntk>(
            layout, node_dict, search_space_graph.num_expansions, preceding_nodes);
    }
    throw std::runtime_error("Invalid node type");
}
/**
 * Validates the given layout based on the nodes in the network and their mappings in the node dictionary.
 * It checks if the placement of nodes in the layout is possible and ensures there are valid paths from each tile to the
 * drain.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout to be validated.
 * @param network The network containing the nodes.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @param placement_possible A boolean flag that will be set to false if placement is not possible.
 * @return True if the layout is valid, false otherwise.
 */
template <typename Lyt, typename Ntk>
bool valid_layout(Lyt& layout, Ntk& network, node_dict_type<Lyt, Ntk>& node_dict, bool& placement_possible)
{
    for (auto& node : node_dict)
    {
        auto tile = layout.get_tile(node.second);
        if ((!layout.is_po_tile(layout.get_tile(node.second)) && (layout.fanout_size(node.second) == 0)) ||
            ((layout.fanout_size(node.second) == 1) && network.is_fanout(node.first)))
        {
            layout.resize({layout.x() + 1, layout.y() + 1, 1});
            fiction::tile<Lyt> dest{layout.x(), layout.y(), 0};

            if (!detail::check_path(layout, tile, dest, false, true, false).first)
            {
                placement_possible = false;
                return false;
            }

            layout.resize({layout.x() - 1, layout.y() - 1, 1});
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
    }
    return true;
}
/**
 * Places a node with one input in the given layout based on the specified position and signal.
 * It handles different types of nodes, such as inverters, primary outputs, and buffers.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to place the node.
 * @param network The network containing the nodes.
 * @param position The tile representing the position for placement.
 * @param signal The signal associated with the preceding node.
 * @param current_node The current node index in the nodes_to_place vector.
 * @param nodes_to_place A vector representing the nodes to be placed
 * @param current_po The current primary output index.
 * @param po_names A vector of primary output names.
 * @throws std::runtime_error If the node type is invalid.
 */
template <typename Lyt, typename Ntk>
void place_node_with_1_input(Lyt& layout, Ntk& network, fiction::tile<Lyt> position, mockturtle::signal<Lyt> signal,
                             uint64_t current_node, std::vector<mockturtle::node<Ntk>>& nodes_to_place,
                             uint64_t current_po, std::vector<std::string>& po_names)
{
    if (network.is_inv(nodes_to_place[current_node]))
    {
        layout.create_not(signal, position);
    }
    else if (network.is_po(nodes_to_place[current_node]))
    {
        layout.create_po(signal, po_names[current_po], position);
    }
    else if (network.is_fanout(nodes_to_place[current_node]) || network.is_buf(nodes_to_place[current_node]))
    {
        layout.create_buf(signal, position);
    }
    else
    {
        throw std::runtime_error("Invalid node type");
    }
}
/**
 * Places a node with two inputs in the given layout based on the specified position and signals.
 * It handles different types of nodes, such as AND, NAND, OR, NOR, XOR, and XNOR gates.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param layout The layout in which to place the node.
 * @param network The network containing the nodes.
 * @param position The tile representing the position for placement.
 * @param signal_1 The signal associated with the first preceding node.
 * @param signal_2 The signal associated with the second preceding node.
 * @param current_node The current node index in the nodes_to_place vector.
 * @param nodes_to_place A vector representing the nodes to be placed.
 * @throws std::runtime_error If the node type is invalid.
 */
template <typename Lyt, typename Ntk>
void place_node_with_2_inputs(Lyt& layout, Ntk& network, fiction::tile<Lyt> position, mockturtle::signal<Lyt> signal_1,
                              mockturtle::signal<Lyt> signal_2, uint64_t current_node,
                              std::vector<mockturtle::node<Ntk>>& nodes_to_place)
{
    if (network.is_and(nodes_to_place[current_node]))
    {
        layout.create_and(signal_1, signal_2, position);
    }
    else if (network.is_nand(nodes_to_place[current_node]))
    {
        layout.create_nand(signal_1, signal_2, position);
    }
    else if (network.is_or(nodes_to_place[current_node]))
    {
        layout.create_or(signal_1, signal_2, position);
    }
    else if (network.is_nor(nodes_to_place[current_node]))
    {
        layout.create_nor(signal_1, signal_2, position);
    }
    else if (network.is_xor(nodes_to_place[current_node]))
    {
        layout.create_xor(signal_1, signal_2, position);
    }
    else if (network.is_xnor(nodes_to_place[current_node]))
    {
        layout.create_xnor(signal_1, signal_2, position);
    }
    else
    {
        throw std::runtime_error("Invalid node type");
    }
}
/**
 * Calculates the reward for placing a node in the network layout and determines if the placement process is done.
 * The reward system assigns a high reward if all nodes are placed, a small reward if a node is placed, and no reward
 * otherwise.
 *
 * @tparam Ntk Network type.
 * @param placed_node Boolean flag indicating if the current node was successfully placed.
 * @param current_node The current node index in the nodes_to_placed vector.
 * @param nodes_to_placed A vector representing the nodes to be placed.
 * @param placement_possible Boolean flag indicating if the placement is possible.
 * @param max_placed_nodes Reference to the maximum number of nodes placed so far.
 * @return A tuple containing the reward and a boolean indicating if the placement process is done.
 */
template <typename Ntk>
std::tuple<double, bool> calculate_reward(bool placed_node, uint64_t& current_node,
                                          std::vector<mockturtle::node<Ntk>>& nodes_to_placed, bool placement_possible,
                                          uint64_t& max_placed_nodes)
{
    double reward = 0.0;
    if (current_node == nodes_to_placed.size())
    {
        reward = 10000.0;
    }
    else if (placed_node)
    {
        reward = 1.0;
    }
    bool done = (current_node == nodes_to_placed.size()) || !placement_possible;

    max_placed_nodes = std::max(current_node, max_placed_nodes);

    return std::make_tuple(reward, done);
}
/**
 * Executes a single placement step in the layout for the given network node. It determines the type of the node,
 * places it accordingly, and calculates the reward for the placement.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param position The tile representing the position for placement.
 * @param layout The layout in which to place the node.
 * @param network The network containing the nodes.
 * @param placement_possible Boolean flag indicating if the placement is possible.
 * @param current_node The current node index in the nodes_to_place vector.
 * @param nodes_to_placed A vector representing the nodes to be placed.
 * @param po_names A vector of primary output names.
 * @param current_pi The current primary input index.
 * @param current_po The current primary output index.
 * @param node_dict A dictionary mapping nodes from the network to tiles in the layout.
 * @param max_placed_nodes Reference to the maximum number of nodes placed so far.
 * @param pi2node A mapping of primary input nodes to layout nodes.
 * @return A pair containing the reward and a boolean indicating if the placement process is done.
 * @throws std::runtime_error If the node type is invalid or not recognized.
 */
template <typename Lyt, typename Ntk>
std::pair<double, bool> place(fiction::tile<Lyt> position, Lyt& layout, Ntk& network, bool& placement_possible,
                              uint64_t& current_node, std::vector<mockturtle::node<Ntk>>& nodes_to_place,
                              std::vector<std::string>& po_names, uint64_t& current_pi, uint64_t& current_po,
                              node_dict_type<Lyt, Ntk>& node_dict, uint64_t& max_placed_nodes,
                              mockturtle::node_map<mockturtle::node<Lyt>, Ntk>& pi2node)
{
    std::vector<mockturtle::signal<Lyt>> preceding_nodes{};
    network.foreach_fanin(nodes_to_place[current_node],
                          [&preceding_nodes](const auto& f) { preceding_nodes.push_back(f); });

    bool   done   = false;
    double reward = 0;

    if (!placement_possible || !layout.is_empty_tile(position))
    {
        done = true;
    }
    else
    {
        uint64_t placed_node = 0;
        auto     num_fanins  = preceding_nodes.size();

        if (num_fanins == 0)
        {
            layout.move_node(pi2node[nodes_to_place[current_node]], position);
            placed_node = 1;
            current_pi += 1;
        }
        else if (num_fanins == 1)
        {
            auto                    layout_node = node_dict[preceding_nodes[0]];
            auto                    src         = layout.get_tile(layout_node);
            mockturtle::signal<Lyt> signal      = layout.make_signal(layout_node);

            place_node_with_1_input(layout, network, position, signal, current_node, nodes_to_place, current_po,
                                    po_names);
            layout.move_node(layout.get_node(position), position, {});

            auto path = detail::check_path(layout, src, position, false, false, true);
            if (!path.first)
            {
                placement_possible = false;
                done               = true;
            }
            else
            {
                fiction::route_path(layout, *path.second);
                placed_node = 1;

                if (network.is_po(nodes_to_place[current_node]))
                {
                    current_po += 1;
                }

                for (auto el : *path.second)
                {
                    layout.obstruct_coordinate(el);
                }
            }
        }
        else if (num_fanins == 2)
        {
            mockturtle::node<Lyt>   layout_node_1 = node_dict[preceding_nodes[0]];
            auto                    src_1         = layout.get_tile(layout_node_1);
            mockturtle::signal<Lyt> signal_1      = layout.make_signal(layout_node_1);

            mockturtle::node<Lyt>   layout_node_2 = node_dict[preceding_nodes[1]];
            auto                    src_2         = layout.get_tile(layout_node_2);
            mockturtle::signal<Lyt> signal_2      = layout.make_signal(layout_node_2);

            place_node_with_2_inputs(layout, network, position, signal_1, signal_2, current_node, nodes_to_place);
            layout.move_node(layout.get_node(position), position, {});

            auto path_1 = detail::check_path(layout, src_1, position, false, false, true);

            if (path_1.first)
            {
                for (auto el : *path_1.second)
                {
                    layout.obstruct_coordinate(el);
                }

                auto path_2 = detail::check_path(layout, src_2, position, false, false, true);

                if (path_2.first)
                {
                    for (auto el : *path_2.second)
                    {
                        layout.obstruct_coordinate(el);
                    }

                    placed_node = 1;
                    fiction::route_path(layout, *path_1.second);
                    fiction::route_path(layout, *path_2.second);
                }
                else
                {
                    placement_possible = false;
                    done               = true;

                    for (auto el : *path_1.second)
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
            std::string error_message = "Not a valid node: " + std::to_string(nodes_to_place[current_node]);
            throw std::runtime_error(error_message);
        }

        if (placed_node == 1)
        {
            node_dict[nodes_to_place[current_node]] = layout.get_node(position);
            current_node += 1;
            layout.obstruct_coordinate({position.x, position.y, 0});
            layout.obstruct_coordinate({position.x, position.y, 1});
        }

        std::tie(reward, done) =
            calculate_reward<Ntk>(placed_node, current_node, nodes_to_place, placement_possible, max_placed_nodes);
    }

    return {reward, done};
}
/**
 * Resets and initializes a new layout with the specified minimum width and height.
 * The new layout is set up with 2DDWave clocking and can be obstructed.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param min_layout_width The minimum width of the layout.
 * @param min_layout_height The minimum height of the layout.
 * @return A new layout initialized with the specified dimensions and clocking.
 */
template <typename Lyt, typename Ntk>
Lyt reset(uint64_t min_layout_width, uint64_t min_layout_height)
{
    fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>> lyt{
        {min_layout_width - 1, min_layout_height - 1, 1},
        twoddwave_clocking<Lyt>()};
    auto layout = fiction::obstruction_layout<
        fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<>>>>>(
        lyt);
    return layout;
}
/**
 * Computes possible expansions and their priorities for the current vertex in the search space graph.
 * It handles placement of nodes, checks for valid paths, and finds potential next positions based on priorities.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param search_space_graph The search space graph.
 * @param count The number of evaluated paths.
 * @param improv_mode Boolean flag indicating if the improvement mode is active.
 * @param best_solution The best solution found so far in terms of layout area.
 * @param max_placed_nodes Reference to the maximum number of nodes placed so far.
 * @param start_time The start time point for performance measurement.
 * @param verbose Flag indicating if verbose output is enabled.
 * @return A pair containing a vector of next positions with their priorities and an optional layout.
 * If an improved solution is found, the layout is returned.
 * If the layout is invalid or no improvement is possible, std::nullopt is returned.
 */
template <typename Lyt, typename Ntk>
std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>
neighbors(SearchSpaceGraph<Lyt>& search_space_graph, uint64_t count, bool& improv_mode, uint64_t& best_solution,
          uint64_t& max_placed_nodes, std::chrono::time_point<std::chrono::high_resolution_clock> start_time,
          bool verbose)
{
    bool     placement_possible = true;
    uint64_t current_pi         = 0;
    uint64_t current_po         = 0;
    uint64_t current_node       = 0;

    const uint64_t min_layout_width  = search_space_graph.network.num_pis();
    const uint64_t min_layout_height = 1;
    const uint64_t max_layout_width  = search_space_graph.nodes_to_place.size();
    const uint64_t max_layout_height = search_space_graph.nodes_to_place.size();

    std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Lyt>> node_dict{};
    std::vector<std::pair<coord_vec_type<Lyt>, double>>              next_positions;

    Lyt  layout  = detail::reset<Lyt, Ntk>(min_layout_width, min_layout_height);
    auto pi2node = reserve_input_nodes(layout, search_space_graph.network);

    bool                   done = false;
    std::vector<tile<Lyt>> possible_positions{};
    if (search_space_graph.current_vertex.empty())
    {
        possible_positions =
            detail::get_possible_positions<Lyt, Ntk>(layout, search_space_graph, current_node, node_dict);
    }

    for (uint64_t idx = 0; idx < search_space_graph.current_vertex.size(); ++idx)
    {
        auto position = search_space_graph.current_vertex[idx];

        if (!done)
        {
            double reward = 0;
            std::tie(reward, done) =
                place<Lyt, Ntk>(position, layout, search_space_graph.network, placement_possible, current_node,
                                search_space_graph.nodes_to_place, search_space_graph.po_names, current_pi, current_po,
                                node_dict, max_placed_nodes, pi2node);
            uint64_t area = 0;
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
                if (verbose)
                {
                    std::cout << "Found improved solution:\n";
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
                              << " microseconds\n";
                    std::cout << "Evaluated Paths: " << count << "\n";
                    std::cout << "Layout Dimension: " << layout.x() + 1 << " x " << layout.y() + 1 << " = " << area
                              << std::endl;
                    // check equivalence
                    fiction::equivalence_checking_stats eq_stats{};
                    fiction::equivalence_checking<Ntk, Lyt>(search_space_graph.network, layout, &eq_stats);

                    const std::string eq_result = eq_stats.eq == fiction::eq_type::STRONG ? "STRONG" :
                                                  eq_stats.eq == fiction::eq_type::WEAK   ? "WEAK" :
                                                                                            "NO";
                    std::cout << "Equivalent: " << eq_result << "\n";
                    std::cout << "\n";
                }
                best_solution = area;
                improv_mode   = true;
                std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>> return_val({},
                                                                                                              layout);
                return return_val;
            }
            if (improv_mode)
            {
                if (area >= best_solution)
                {
                    return std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>(
                        {}, std::nullopt);
                }
            }
        }
        else
        {
            return std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>({}, std::nullopt);
        }
        if (position.x == layout.x() && layout.x() < (max_layout_width - 1))
        {
            layout.resize({layout.x() + 1, layout.y(), layout.z()});
        }
        if (position.y == layout.y() && layout.y() < (max_layout_height - 1))
        {
            layout.resize({layout.x(), layout.y() + 1, layout.z()});
        }
        if (idx == (search_space_graph.current_vertex.size() - 1))
        {
            if (!valid_layout<Lyt, Ntk>(layout, search_space_graph.network, node_dict, placement_possible))
            {
                return std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>({},
                                                                                                          std::nullopt);
            }
            possible_positions =
                detail::get_possible_positions<Lyt, Ntk>(layout, search_space_graph, current_node, node_dict);
        }
    }
    for (auto position : possible_positions)
    {
        auto new_sequence = search_space_graph.current_vertex;
        new_sequence.push_back(position);
        const double rest_nodes_to_place =
            search_space_graph.nodes_to_place.size() - (search_space_graph.current_vertex.size() + 1);
        const double size1 = static_cast<double>(((std::max(layout.x() - 1, position.x) + 1) *
                                                  (std::max(layout.y() - 1, position.y) + 1))) /
                             static_cast<double>((max_layout_width * max_layout_height));
        const double size2 = static_cast<double>(((position.x + 1) * (position.y + 1))) /
                             static_cast<double>((max_layout_width * max_layout_height));

        double priority = rest_nodes_to_place + size1 + size2;
        next_positions.push_back({new_sequence, priority});
    }
    return std::pair<std::vector<std::pair<coord_vec_type<Lyt>, double>>, std::optional<Lyt>>(next_positions,
                                                                                              std::nullopt);
}
/**
 * Initializes the allowed positions for primary inputs (PIs), the cost for each search space graph and the maximum
 * number of expansions.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param search_space_graphs The vector of search space graphs to be initialized.
 */
template <typename Lyt>
void initialize_pis_cost_and_num_expansions(std::vector<SearchSpaceGraph<Lyt>>& search_space_graphs)
{
    std::vector<bool> pis_top  = {true, true, true, true, false, false, false, false, true, true, true, true};
    std::vector<bool> pis_left = {false, false, false, false, true, true, true, true, true, true, true, true};

    for (uint64_t i = 0; i < search_space_graphs.size(); ++i)
    {
        search_space_graphs[i].pis_top                                            = pis_top[i];
        search_space_graphs[i].pis_left                                           = pis_left[i];
        search_space_graphs[i].cost_so_far[search_space_graphs[i].current_vertex] = 0;
        search_space_graphs[i].num_expansions                                     = 4;
    }
}
/**
 * Initializes the networks and nodes to place.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param ntk The network to be placed and routed.
 * @param search_space_graphs The vector of search space graphs to be initialized.
 */
template <typename Lyt, typename Ntk>
void initialize_networks_and_nodes_to_place(Ntk& ntk, std::vector<SearchSpaceGraph<Lyt>>& search_space_graphs)
{
    // Convert network and apply fanout substitution
    auto tec_ntk = fiction::convert_network<fiction::tec_nt, Ntk>(ntk);
    tec_ntk.substitute_po_signals();
    fiction::fanout_substitution_params params{};
    params.strategy = fiction::fanout_substitution_params::substitution_strategy::BREADTH;
    mockturtle::fanout_view network_substituted_breadth{
        fanout_substitution<mockturtle::names_view<fiction::technology_network>>(tec_ntk, params)};
    params.strategy = fiction::fanout_substitution_params::substitution_strategy::DEPTH;
    mockturtle::fanout_view network_substituted_depth{
        fanout_substitution<mockturtle::names_view<fiction::technology_network>>(tec_ntk, params)};

    fiction::topo_view_co_to_ci network_breadth_co_to_ci{network_substituted_breadth};
    fiction::topo_view_ci_to_co network_breadth_ci_to_co{network_substituted_breadth};
    fiction::topo_view_co_to_ci network_depth_co_to_ci{network_substituted_depth};
    fiction::topo_view_ci_to_co network_depth_ci_to_co{network_substituted_depth};

    // Initialize search space graphs with networks
    for (uint64_t i = 0; i < 3; ++i)
    {
        search_space_graphs[i * 4 + 0].network = network_breadth_co_to_ci;
        search_space_graphs[i * 4 + 1].network = network_breadth_ci_to_co;
        search_space_graphs[i * 4 + 2].network = network_depth_co_to_ci;
        search_space_graphs[i * 4 + 3].network = network_depth_ci_to_co;
    }

    // Prepare nodes to place for each topology view
    auto prepare_nodes_to_place = [](auto& network, auto& nodes_to_place)
    {
        network.foreach_node(
            [&nodes_to_place, &network](const auto& n)
            {
                if (!network.is_constant(n) && !network.is_po(n))
                {
                    nodes_to_place.push_back(n);
                }
            });
    };

    std::vector<mockturtle::node<decltype(network_breadth_co_to_ci)>> nodes_to_place_breadth_co_to_ci{};
    std::vector<mockturtle::node<decltype(network_breadth_ci_to_co)>> nodes_to_place_breadth_ci_to_co{};
    std::vector<mockturtle::node<decltype(network_depth_co_to_ci)>>   nodes_to_place_depth_co_to_ci{};
    std::vector<mockturtle::node<decltype(network_depth_ci_to_co)>>   nodes_to_place_depth_ci_to_co{};
    prepare_nodes_to_place(network_breadth_co_to_ci, nodes_to_place_breadth_co_to_ci);
    prepare_nodes_to_place(network_breadth_ci_to_co, nodes_to_place_breadth_ci_to_co);
    prepare_nodes_to_place(network_depth_co_to_ci, nodes_to_place_depth_co_to_ci);
    prepare_nodes_to_place(network_depth_ci_to_co, nodes_to_place_depth_ci_to_co);

    // Process POs and update search space graphs
    network_substituted_breadth.foreach_co(
        [&](const auto& f)
        {
            auto n = network_substituted_breadth.get_node(f);
            if (network_substituted_breadth.is_po(n))
            {
                nodes_to_place_breadth_co_to_ci.push_back(n);
                nodes_to_place_breadth_ci_to_co.push_back(n);
                nodes_to_place_depth_co_to_ci.push_back(n);
                nodes_to_place_depth_ci_to_co.push_back(n);
                for (auto& graph : search_space_graphs)
                {
                    graph.po_names.push_back(
                        network_substituted_breadth.get_output_name(network_substituted_breadth.po_index(n)));
                }
            }
        });

    // Assign nodes to place to each search space graphs
    for (uint64_t i = 0; i < 3; ++i)
    {
        search_space_graphs[i * 4 + 0].nodes_to_place = nodes_to_place_breadth_co_to_ci;
        search_space_graphs[i * 4 + 1].nodes_to_place = nodes_to_place_breadth_ci_to_co;
        search_space_graphs[i * 4 + 2].nodes_to_place = nodes_to_place_depth_co_to_ci;
        search_space_graphs[i * 4 + 3].nodes_to_place = nodes_to_place_depth_ci_to_co;
    }
}
/**
 * Initializes the enabled search space graphs.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @param search_space_graphs The vector of search space graphs to be initialized.
 * @param high_effort Flag indicating if high effort mode is enabled
 */
template <typename Lyt>
void initialize_frontiers_flags(std::vector<SearchSpaceGraph<Lyt>>& search_space_graphs, bool high_effort)
{
    search_space_graphs[0].frontier_flag = true;
    search_space_graphs[4].frontier_flag = true;

    if (high_effort)
    {
        for (uint64_t i = 0; i < 12; ++i)
        {
            search_space_graphs[i].frontier_flag = true;
        }
    }
}
/**
 * Implementation of the A* P&R algorithm.
 * This class handles the initialization and execution of the algorithm.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 */
template <typename Lyt, typename Ntk>
class a_star_pr_impl
{
  public:
    /**
     * Constructor for the A* P&R algorithm.
     *
     * @param src The source network to be placed.
     * @param p The parameters for the A* priority routing algorithm.
     * @param st The statistics object to record execution details.
     */
    a_star_pr_impl(const Ntk& src, const a_star_pr_params& p, a_star_pr_stats& st) : ntk{src}, ps{p}, pst{st} {}
    /**
     * Executes the A* P&R algorithm and returns the best found layout.
     *
     * @return The best layout found by the algorithm.
     */
    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        auto                  start = std::chrono::high_resolution_clock::now();

        // Set timeout based on parameters
        uint64_t timeout = ps.timeout ? ps.timeout : (ps.high_effort ? 1000 : 10);
        bool     verbose = ps.verbose;

        // Initialize empty layout
        Lyt  lyt{{}, twoddwave_clocking<Lyt>()};
        auto layout    = fiction::obstruction_layout<Lyt>(lyt);
        using ObstrLyt = decltype(layout);

        // Initialize layout to keep track of current best solution
        Lyt best_lyt{{}, twoddwave_clocking<Lyt>()};

        // Initialize search space graphs
        std::vector<SearchSpaceGraph<ObstrLyt>> search_space_graphs(12);

        // Count evaluated paths in the search space graphs
        uint64_t count = 0;

        // Keep track of the maximum number of placed nodes
        uint64_t max_placed_nodes = 0;

        // Current best solution w.r.t. area
        uint64_t best_solution = 100000;

        // Flag indicating if initial solution was already found and other search space graphs should be pruned
        bool improv_mode = false;

        // Initial setup for networks and nodes to place
        initialize_networks_and_nodes_to_place<ObstrLyt, Ntk>(ntk, search_space_graphs);

        // Initial setup for PIs and cost
        initialize_pis_cost_and_num_expansions(search_space_graphs);

        // Initial setup for frontiers flags
        initialize_frontiers_flags(search_space_graphs, ps.high_effort);

        bool timeout_limit_reached = false;
        // Main A* loop
        while (!timeout_limit_reached)
        {
            for (auto& search_space_graph : search_space_graphs)
            {
                if (search_space_graph.frontier_flag)
                {
                    count++;
                    auto neighbors = detail::neighbors<decltype(layout), fiction::tec_nt>(
                        search_space_graph, count, improv_mode, best_solution, max_placed_nodes, start, verbose);
                    if (neighbors.second)
                    {
                        best_lyt = *neighbors.second;
                    }
                    for (const auto& [next, cost] : neighbors.first)
                    {
                        if (search_space_graph.cost_so_far.find(next) == search_space_graph.cost_so_far.end() ||
                            cost < search_space_graph.cost_so_far[next])
                        {
                            search_space_graph.cost_so_far[next] = cost;
                            double priority                      = cost;
                            search_space_graph.frontier.put(next, priority);
                        }
                    }
                }
            }
            for (auto& graph : search_space_graphs)
            {
                if (graph.frontier_flag)
                {
                    if (!graph.frontier.empty())
                    {
                        graph.current_vertex = graph.frontier.get();
                    }
                    else
                    {
                        graph.frontier_flag = false;
                    }
                }
            }
            timeout_limit_reached = !std::any_of(search_space_graphs.begin(), search_space_graphs.end(),
                                                 [](const SearchSpaceGraph<decltype(layout)>& search_space_graph)
                                                 { return search_space_graph.frontier_flag; });

            auto end          = std::chrono::high_resolution_clock::now();
            auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(end - start);
            if (duration_sec.count() >= timeout)
            {
                timeout_limit_reached = true;
            }
        }

        best_lyt.set_layout_name(ntk.get_network_name());
        return best_lyt;
    }

  private:
    Ntk              ntk;
    a_star_pr_params ps;
    a_star_pr_stats& pst;
};

}  // namespace detail
/**
 * Executes the A* P&R algorithm for the given network and returns the resulting layout.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam Ntk Network type.
 * @param ntk The network to be placed and routed.
 * @param ps The parameters for the A* priority routing algorithm. Defaults to an empty parameter set.
 * @param pst A pointer to a statistics object to record execution details. Defaults to nullptr.
 * @return The resulting layout after applying the A* priority routing algorithm.
 * @throws high_degree_fanin_exception If the network has nodes with a fanin degree higher than 2.
 */
template <typename Lyt, typename Ntk>
Lyt a_star_pr(Ntk& ntk, a_star_pr_params ps = {}, a_star_pr_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, 2))
    {
        throw high_degree_fanin_exception();
    }

    a_star_pr_stats                  st{};
    detail::a_star_pr_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_A_STAR_PR_HPP
