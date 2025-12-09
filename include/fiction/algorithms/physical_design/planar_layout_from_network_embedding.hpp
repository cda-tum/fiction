//
// Created by benjamin on 21.01.25.
//

#ifndef FICTION_PLANAR_LAYOUT_FROM_NETWORK_EMBEDDING_HPP
#define FICTION_PLANAR_LAYOUT_FROM_NETWORK_EMBEDDING_HPP

#include "fiction/algorithms/graph/mincross.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/network_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

/**
 * Parameters for the planar layout from network embedding algorithm.
 */
struct planar_layout_from_network_embedding_params
{
    /**
     * Number of clock phases to use. 3 and 4 are supported.
     */
    num_clks number_of_clock_phases = num_clks::FOUR;
    /**
     * Verbosity.
     */
    bool verbose = false;
};
/**
 * This struct stores statistics about the planar layout design process.
 */
struct planar_layout_from_network_embedding_stats
{
    /**
     * Runtime of the planar layout design process.
     */
    mockturtle::stopwatch<>::duration time_total{};
    /**
     * Layout width.
     */
    uint64_t x_size{0ull};
    /**
     * Layout height.
     */
    uint64_t y_size{0ull};
    /**
     * Number of gates.
     */
    uint64_t num_gates{0ull};
    /**
     * Number of wires.
     */
    uint64_t num_wires{0ull};
    /**
     * Number of crossings.
     */
    uint64_t num_crossings{0ull};
    /**
     * Reports the statistics to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time      = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size     = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates      = {}\n", num_gates);
        out << fmt::format("[i] num. wires      = {}\n", num_wires);
        out << fmt::format("[i] num. crossings  = {}\n", num_crossings);
    }
};

namespace detail
{

/**
 * Defines a 3D lookup table using `std::array` and encapsulates it within a function. This table encodes all possible
 * combinations of the previous level, connection type, orientations, and surrounding spacing (gaps). Based on these
 * inputs, it returns the corresponding orientation and spacing configuration for the current buffer.
 */
inline std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 2>& get_buffer_lookup()
{
    static std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 2> array = {
        {  // Array
         {{// Unconnected
           {{
               // East
               {{{0, 0}, {1, 0}, {1, 1}, {0, 1}}},  // gap 0
               {{{0, 0}, {0, 0}, {1, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2 and higher
           }},
           {{
               // South
               {{{3, 0}, {2, 0}, {0, 0}, {0, 0}}},  // gap 0; only first two entries used
               {{{3, 0}, {2, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{3, 0}, {2, 0}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // Free
               {{{0, 0}, {1, 0}, {2, 0}, {3, 0}}},  // gap 0
               {{{0, 0}, {0, 0}, {1, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2 and higher
           }}}},
         {{// Connected
           {{
               // East
               {{{0, 0}, {0, 0}, {0, 1}, {0, 1}}},  // gap 0
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2 and higher
           }},
           {{
               // South
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}},  // gap 0
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}}   // gap 2 and higher
           }},
           {{
               // Free
               {{{0, 0}, {0, 0}, {3, 0}, {3, 0}}},  // gap 0
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2
           }}}}}};
    return array;
}

/**
 * Defines a 3D lookup table using `std::array` and encapsulates it within a function. This table encodes all possible
 * combinations of the previous level, connection type, orientations, and surrounding spacing (gaps). Based on these
 * inputs, it returns the corresponding orientation and spacing configuration for the current fanout.
 */
inline std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 4>& get_fanout_lookup()
{
    static std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 4> array = {
        {  // Array
         {{// Type Fo 1+2
           {{
               // East
               {{{1, 0}, {1, 1}, {1, 2}, {1, 1}}},  // gap 0
               {{{1, 0}, {1, 0}, {1, 1}, {1, 0}}},  // gap 1
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}}   // gap 2
           }},
           {{
               // South
               {{{2, 0}, {2, 1}, {0, 0}, {0, 0}}},  // gap 0
               {{{2, 0}, {2, 1}, {0, 0}, {0, 0}}},  // gap 1
               {{{2, 0}, {2, 1}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // Free
               {{{1, 0}, {2, 0}, {2, 1}, {2, 0}}},  // gap 0
               {{{1, 0}, {1, 0}, {2, 0}, {1, 0}}},  // gap 1
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}}   // gap 2
           }}}},
         {{// Type F1
           {{
               // East
               {{{0, 0}, {0, 1}, {0, 2}, {0, 1}}},  // gap 0
               {{{0, 0}, {0, 0}, {0, 1}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // South
               {{{3, 0}, {3, 1}, {0, 0}, {0, 0}}},  // gap 0
               {{{3, 0}, {3, 1}, {0, 0}, {0, 0}}},  // gap 1
               {{{3, 0}, {3, 1}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // Free
               {{{0, 0}, {3, 0}, {3, 1}, {3, 0}}},  // gap 0
               {{{0, 0}, {0, 0}, {3, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2
           }}}},
         {{// Type F2
           {{
               // East
               {{{1, 0}, {1, 0}, {1, 1}, {1, 1}}},  // gap 0
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}},  // gap 1
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}}   // gap 2
           }},
           {{
               // South
               {{{2, 0}, {2, 0}, {0, 0}, {0, 0}}},  // gap 0
               {{{2, 0}, {2, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{2, 0}, {2, 0}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // Free
               {{{1, 0}, {1, 0}, {2, 0}, {2, 0}}},  // gap 0
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}},  // gap 1
               {{{1, 0}, {1, 0}, {1, 0}, {1, 0}}}   // gap 2
           }}}},
         {{// Type F3
           {{
               // East
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}},  // gap 0
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // South
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}},  // gap 0
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{3, 0}, {3, 0}, {0, 0}, {0, 0}}}   // gap 2
           }},
           {{
               // Free
               {{{0, 0}, {0, 0}, {3, 0}, {3, 0}}},  // gap 0
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}},  // gap 1
               {{{0, 0}, {0, 0}, {0, 0}, {0, 0}}}   // gap 2
           }}}}}};
    return array;
}

/**
 * Computes the fan-out connection type of a node based on its successors' fan-in structures.
 * Assumes exactly two fan-outs, which are ordered by rank position.
 *
 * Returns:
 *  - 0: Both fan-outs have a single fan-in.
 *  - 1: First fan-out has one fan-in, second has multiple.
 *  - 2: Second fan-out has one fan-in, first has multiple.
 *  - 3: Both fan-outs have multiple fan-ins.
 *
 * @tparam Ntk Logic network type.
 * @param ntk  Logic network containing the node.
 * @param n    Node for which to determine the fan-out connection type.
 * @return     Integer code (0–3) indicating the fan-out connection pattern.
 */
template <typename Ntk>
uint64_t calculate_fanout_connection_type(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    // order the POs
    if (ntk.is_po(n))
    {
        return 0;
    }
    auto fo = ntk.fanout(n);
    assert(fo.size() == 2);
    std::sort(fo.begin(), fo.end(), [&ntk](int a, int b) { return ntk.rank_position(a) < ntk.rank_position(b); });
    if (ntk.fanin_size(fo[0]) == 1 && ntk.fanin_size(fo[1]) == 1)
    {
        return 0;
    }
    if (ntk.fanin_size(fo[0]) == 1)
    {
        return 1;
    }
    if (ntk.fanin_size(fo[1]) == 1)
    {
        return 2;
    }
    // both fan-puts are connected with a neighbour
    return 3;
}

/**
 * Computes the gap between the fan-in node and its preceding node, i.e., the node with a rank position one less than
 * the current node. This value indicates the available spacing for placement.
 *
 * @tparam Ntk Logic network type.
 * @tparam Lyt Layout type.
 * @param ntk       Logic network containing the node.
 * @param node2pos  Mapping from network nodes to layout tile positions.
 * @param lvl       Current level index of the node.
 * @param n         Node for which to compute the predecessor gap.
 * @return          Gap size (clamped to a maximum of 2).
 */

template <typename Ntk, typename Lyt>
uint64_t calculate_predecessor_gap(const Ntk& ntk, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                   const uint64_t lvl, const mockturtle::node<Ntk>& n)
{
    // return if in the PI level
    if (lvl == 0)
    {
        return 0;
    }

    // calculate the rank of the predecessor node
    auto fc = fanins(ntk, n);

    if (fc.fanin_nodes.size() == 2)
    {
        std::sort(fc.fanin_nodes.begin(), fc.fanin_nodes.end(),
                  [&ntk](int a, int b) { return ntk.rank_position(a) < ntk.rank_position(b); });
    }
    auto       pre = fc.fanin_nodes[0];
    const auto r   = ntk.rank_position(pre);

    // return if no neighbour
    if (r == 0)
    {
        return 0;
    }

    // calculate the level of the predecessor node
    const auto l = lvl - 1;

    // get the neighbour with lower rank of the predecessor
    const auto pre_neighbour = ntk.at_rank_position(l, r - 1);

    // calculate the gap size
    const auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre]);
    const auto pre2_t = static_cast<tile<Lyt>>(node2pos[pre_neighbour]);

    assert(pre1_t.y > pre2_t.y);
    return std::min<uint64_t>(pre1_t.y - pre2_t.y - 1, 2);
}

/**
 * Computes the buffer connection type for a given node.
 * Determines whether the node serves as the second (rightmost) fan-in of its successor node.
 *
 * @tparam Ntk Logic network type.
 * @param ntk  Logic network containing the node.
 * @param n    Node to analyze.
 * @return     1 if the node is the rightmost fan-in of its successor, otherwise 0.
 */
template <typename Ntk>
uint64_t calculate_buffer_connection_type(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    if (ntk.is_po(n))
    {
        return 0;
    }
    auto fo = ntk.fanout(n);
    assert(fo.size() == 1);
    if (ntk.fanin_size(fo[0]) == 2)
    {
        auto fc  = fanins(ntk, fo[0]);
        auto pre = fc.fanin_nodes;
        assert(pre.size() == 2);
        std::sort(pre.begin(), pre.end(), [&ntk](int a, int b) { return ntk.rank_position(a) < ntk.rank_position(b); });
        if (pre[1] == n)
        {
            return 1;
        }
    }
    return 0;
}

/**
 * Determines the allowed orientation of a node based on its predecessor.
 * For nodes driven by a fan-out, the orientation is defined by their relative rank position among the fan-out's
 * successors.
 *
 * Returns:
 *  - 0: Node is the first (east) successor of its predecessor.
 *  - 1: Node is the second (west) successor of its predecessor.
 *  - 2: Node is not driven by a fan-out.
 *
 * @tparam Ntk Logic network type.
 * @param ntk  Logic network containing the node.
 * @param n    Node for which to determine the allowed orientation.
 * @return     Orientation code (0–2) describing the node's relative position.
 */
template <typename Ntk>
uint64_t calculate_allowed_orientation(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    auto fc = fanins(ntk, n);
    assert(fc.fanin_nodes.size() == 1);
    auto pre = fc.fanin_nodes[0];
    if (ntk.is_fanout(pre))
    {
        auto fo = ntk.fanout(pre);
        assert(fo.size() == 2);
        std::sort(fo.begin(), fo.end(), [&ntk](int a, int b) { return ntk.rank_position(a) < ntk.rank_position(b); });
        if (n == fo[0])  // east
        {
            return 0;
        }
        assert(n == fo[1]);
        return 1;
    }
    return 2;
}

/**
 * Determines the initial orientation for a given network level.
 * The orientation is inferred from the structural pattern of nodes within the specified level, considering their fan-in
 * and fan-out relationships.
 *
 * Returns:
 *  - 0: Default orientation (no specific structure found).
 *  - 1: Level starts with a fan-out or buffer structure.
 *  - 3: Level starts with a two-input (binary) gate.
 *
 * @tparam Ntk Logic network type.
 * @param ntk  Logic network to analyze.
 * @param lvl  Level index for which to compute the starting orientation.
 * @return     Orientation code (0, 1, or 3) defining the level's initial direction.
 */
template <typename Ntk>
int calculate_start_orientation(const Ntk& ntk, const uint32_t lvl)
{
    int orientation = 0;
    if (lvl == 0)
    {
        return orientation;
    }
    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &orientation](const auto& n)
                             {
                                 if (orientation > 0)
                                 {
                                     return;
                                 }
                                 if (ntk.fanin_size(n) == 2)
                                 {
                                     orientation = 4;
                                     return;
                                 }
                                 if (const auto fc = fanins(ntk, n); fc.fanin_nodes.size() == 1)
                                 {
                                     const auto& pre = fc.fanin_nodes[0];
                                     if (ntk.is_fanout(pre) && ntk.fanout_size(pre) == 2)
                                     {
                                         orientation = 1;
                                         return;
                                     }
                                 }
                                 if (ntk.fanout_size(n) == 2)
                                 {
                                     orientation = 1;
                                     return;
                                 }
                             });
    return (orientation == 0) ? 0 : (orientation - 1);
    // instead of just returning 0, here the buffers could be wired in a way they are closer together and hence overhead
    // produced by two input nodes is reduced
}

/**
 * Computes orientation and routing line variables for a given network level.
 *
 * For each node in the specified level, the function determines:
 *  - The node's orientation, based on its fan-in structure, gap spacing, and connection type.
 *  - Whether new routing lines must be introduced, derived from lookup tables for buffer and fan-out configurations.
 *
 * @tparam Ntk  Logic network type.
 * @tparam Lyt  Layout type.
 * @param ntk       Logic network to analyze.
 * @param node2pos  Mapping from network nodes to layout tile positions.
 * @param lvl       Level index for which to compute the variables.
 * @return          Tuple containing:
 *                   - `orientation`: orientation values for each node in the level.
 *                   - `new_lines`: flags indicating newly added routing lines.
 */
template <typename Ntk, typename Lyt>
std::tuple<std::vector<uint64_t>, std::vector<uint64_t>>
compute_pr_variables(const Ntk& ntk, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                     const uint32_t& lvl)
{
    std::vector<uint64_t> orientation(ntk.rank_width(lvl));
    std::vector<uint64_t> new_lines(ntk.rank_width(lvl));
    // get the lookup tables for the gate types
    const auto& buffer_lu = get_buffer_lookup();
    const auto& fanout_lu = get_fanout_lookup();

    ntk.foreach_node_in_rank(
        lvl,
        [&ntk, &node2pos, &lvl, &orientation, &new_lines, &buffer_lu, &fanout_lu](const auto& n, const auto i)
        {
            // calculate the gap between the predecessors
            const auto gap = calculate_predecessor_gap<Ntk, Lyt>(ntk, node2pos, lvl, n);
            // calculate the orientation and new_lines depending on the type of nodes and available space
            if (ntk.fanin_size(n) == 2)
            {
                if (i != 0)
                {
                    if (orientation[i - 1] == 2 && gap == 0)
                    {
                        orientation[i] = 1;
                    }
                }
            }
            else if (ntk.fanin_size(n) == 1)
            {
                // allowed orientation Flag e = 0, s = 1, free = 2
                const auto allowed_orientation = calculate_allowed_orientation(ntk, n);
                assert(allowed_orientation < 3);

                // needs the type of connection (F1+2, F1, F2), allowed_orientation, gap,
                // orientation, new_line as input
                if (ntk.is_fanout(n))
                {
                    // calculate the type of connection F1+2 = 0, F1 = 1, F2 = 2, F0 = 3;
                    const auto fanout_connection_type = calculate_fanout_connection_type(ntk, n);
                    assert(fanout_connection_type < 4);
                    if (i != 0)
                    {
                        const auto& pair =
                            fanout_lu.at(fanout_connection_type).at(allowed_orientation).at(gap).at(orientation[i - 1]);
                        orientation[i] = pair.first;
                        new_lines[i]   = pair.second;
                    }
                    else if (fanout_connection_type == 0 || fanout_connection_type == 2)
                    {
                        orientation[i] = 1;
                    }
                }
                // needs the type of connection (connected, unconnected), allowed_orientation, gap,
                // orientation, new_line as input
                else
                {
                    if (i != 0)
                    {
                        // Connected Flag
                        const auto  buffer_connection_type = calculate_buffer_connection_type(ntk, n);
                        const auto& pair =
                            buffer_lu.at(buffer_connection_type).at(allowed_orientation).at(gap).at(orientation[i - 1]);

                        orientation[i] = pair.first;
                        new_lines[i]   = pair.second;
                    }
                    else
                    {
                        orientation[i] = calculate_start_orientation(ntk, lvl);
                    }
                }
            }
        });

    const auto ret = std::make_tuple(orientation, new_lines);
    return ret;
}

/**
 * Collects the indices of all two-input nodes in a given level.
 *
 * @tparam Ntk Logic network type.
 * @param ntk  Logic network to analyze.
 * @param lvl  Level index to inspect.
 * @return     Vector of node indices with two fan-ins in the specified level.
 */
template <typename Ntk>
std::vector<std::size_t> compute_two_input_indices(const Ntk& ntk, const uint64_t& lvl)
{
    std::vector<std::size_t> two_input_indices{};
    two_input_indices.reserve(ntk.rank_width(lvl));
    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &two_input_indices, &lvl](const auto& n, const auto& i)
                             {
                                 if (ntk.fanin_size(n) == 2)
                                 {
                                     two_input_indices.emplace_back(i);
                                 }
                             });
    return two_input_indices;
}

/**
 * Computes the number of new routing lines required.
 *
 * @tparam Ntk  Logic network type.
 * @tparam Lyt  Layout type.
 * @param ntk       Logic network to analyze.
 * @param node2pos  Mapping from network nodes to layout tile positions.
 * @param lvl       Level index to inspect.
 * @return          Vector of gap sizes (new line counts) for all two-input nodes in the level.
 */
template <typename Ntk, typename Lyt>
std::vector<uint64_t> calculate_two_input_new_lines(const Ntk&                                                ntk,
                                                    const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                                    const uint32_t&                                           lvl)
{
    std::vector<uint64_t> cluster_new_lines{};
    cluster_new_lines.reserve(ntk.rank_width(lvl));
    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &node2pos, &cluster_new_lines](const auto& n)
                             {
                                 auto fc = fanins(ntk, n);
                                 // calculate gaps due to AND gates
                                 if (fc.fanin_nodes.size() == 2)
                                 {
                                     std::sort(fc.fanin_nodes.begin(), fc.fanin_nodes.end(), [&ntk](int a, int b)
                                               { return ntk.rank_position(a) < ntk.rank_position(b); });
                                     // compute the max_gap for two fan-ins of  anode
                                     const auto &pre1 = fc.fanin_nodes[0], &pre2 = fc.fanin_nodes[1];

                                     const auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                                                pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                                     cluster_new_lines.emplace_back(static_cast<uint64_t>(pre2_t.y - pre1_t.y - 1));
                                 }
                             });
    return cluster_new_lines;
}

/**
 * Balances the final x and y wiring coordinates across all nodes in a level.
 *
 * The function identifies the minimal routing demand, which means the closest diagonal, where all nodes can be placed
 * and can be routed without conflicts.
 *
 * @param x                     Vector of x-coordinates to be adjusted.
 * @param y                     Vector of y-coordinates to be adjusted.
 * @param two_input_indices     Indices of two-input nodes within the level.
 * @param two_input_new_lines   Number of new routing lines associated with each two-input node.
 */
inline void adjust_final_values(std::vector<uint64_t>& x, std::vector<uint64_t>& y,
                                const std::vector<std::size_t>& two_input_indices,
                                const std::vector<uint64_t>&    two_input_new_lines)
{
    // Max element in two_input_new_lines and its index
    const auto        max_it = std::max_element(two_input_new_lines.begin(), two_input_new_lines.end());
    const uint64_t    max_new_lines_two_inputs = (max_it != two_input_new_lines.end()) ? *max_it : 0;
    const std::size_t max_two_inputs_idx =
        (max_it != two_input_new_lines.end()) ?
            static_cast<std::size_t>(std::distance(two_input_new_lines.begin(), max_it)) :
            0;
    // Find index with max x+y
    std::size_t max_xy_idx = 0;
    uint64_t    max_xy_sum = 0;
    for (std::size_t i = 0; i < x.size(); ++i)
    {
        const uint64_t s = x[i] + y[i];
        if (i == 0 || s > max_xy_sum)
        {
            max_xy_sum = s;
            max_xy_idx = i;
        }
    }
    // Determine the center index
    std::size_t center = 0;
    uint64_t    max    = 0;
    if (max_xy_sum > max_new_lines_two_inputs)
    {
        center = max_xy_idx;
        max    = max_xy_sum;
    }
    else
    {
        center = two_input_indices.empty() ? 0 : two_input_indices[max_two_inputs_idx];
        max    = max_new_lines_two_inputs;
    }
    // Create a lookup map for O(1) access instead of O(n) std::find()
    std::unordered_map<std::size_t, std::size_t> two_input_map;
    for (std::size_t i = 0; i < two_input_indices.size(); ++i)
    {
        two_input_map[two_input_indices[i]] = i;
    }
    // Iterate through x and y to update values efficiently
    for (std::size_t i = 0; i < x.size(); ++i)
    {
        auto it = two_input_map.find(i);
        if (it != two_input_map.end())
        {  // Two fan-in node
            const auto idx  = it->second;
            const auto diff = max - x[i] - y[i] - two_input_new_lines[idx];
            if (i < center)
            {
                y[i] += diff;
            }
            else if (i > center)
            {
                x[i] += diff;
            }
            else
            {
                assert(x[i] + y[i] + two_input_new_lines[idx] == max);
            }
        }
        else
        {  // One fan-in node
            const auto diff = max - x[i] - y[i];
            if (i < center)
            {
                y[i] += diff;
            }
            else if (i > center)
            {
                x[i] += diff;
            }
            else
            {
                assert(x[i] + y[i] == max);
            }
        }
    }
}

/**
 * Computes the x and y wiring coordinates for nodes in a given level.
 *
 * The function divides the level into clusters separated by two-input gates and computes the relative wire offsets
 * within and between these clusters. It accounts for new routing lines, right and left propagation, and spacing
 * adjustments between connected clusters.
 *
 * @tparam Ntk  Logic network type.
 * @tparam Lyt  Layout type.
 * @param ntk       Logic network to analyze.
 * @param node2pos  Mapping from network nodes to layout tile positions.
 * @param new_lines Vector of newly introduced routing lines for each node.
 * @param lvl       Level index to process.
 * @return          Pair of vectors representing x and y wiring coordinates for each node in the level.
 */
template <typename Ntk, typename Lyt>
std::pair<std::vector<uint64_t>, std::vector<uint64_t>>
compute_wiring(const Ntk& ntk, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
               const std::vector<uint64_t>& new_lines, const uint64_t& lvl)
{
    // Initialize 2-input indices
    const auto two_input_indices   = compute_two_input_indices(ntk, lvl);
    const auto two_input_new_lines = calculate_two_input_new_lines<Ntk, Lyt>(ntk, node2pos, lvl);

    // Initialize cluster indices
    std::size_t cluster_index_start = 0;

    // Initialize the x and y vectors
    std::vector<uint64_t> x(ntk.rank_width(lvl));
    std::vector<uint64_t> y(ntk.rank_width(lvl));

    // Initialize the x and y vectors
    std::vector<uint64_t> cluster_new_lines(two_input_indices.size() + 1);

    // if there is no two input gate, then there is only one cluster
    if (two_input_indices.empty())
    {
        for (std::size_t j = 0; j < ntk.rank_width(lvl); ++j)
        {
            x[j] = std::accumulate(new_lines.begin() + static_cast<std::ptrdiff_t>(j + 1),
                                   new_lines.begin() + ntk.rank_width(lvl), 0UL);
            y[j] = (j == 0) ? new_lines[j] : y[j - 1] + new_lines[j];
        }
        return std::make_pair(x, y);
    }

    // Iterate over all cluster indices
    uint64_t propagate_right = 0;
    for (std::size_t i = 0; i < two_input_indices.size() + 1; ++i)
    {
        std::size_t cluster_index_end = 0;
        if (i == two_input_indices.size())
        {
            cluster_index_end = new_lines.size();
        }
        else
        {
            cluster_index_end = two_input_indices[i];
        }

        // Compute x and y for the current cluster
        for (auto j = cluster_index_start; j < cluster_index_end; ++j)
        {
            // adjust x values
            x[j] = std::accumulate(new_lines.begin() + static_cast<std::ptrdiff_t>(j + 1),
                                   new_lines.begin() + static_cast<std::ptrdiff_t>(cluster_index_end), 0UL);

            // adjust y values with propagation to the right (direction based on a 1D vector)
            y[j] = (j == 0) ? new_lines[j] : y[j - 1] + new_lines[j];
            y[j] += (j == cluster_index_start) ? propagate_right : 0;
        }

        // Also set the new lines for two input nodes
        if (cluster_index_start != 0)
        {
            y[cluster_index_start - 1] += propagate_right;
        }

        // Save the number of new lines in a cluster
        if (cluster_index_start != cluster_index_end)
        {
            cluster_new_lines[i] = x[cluster_index_start];
        }

        // Save the right propagated new_lines
        const uint64_t til = (i < two_input_new_lines.size()) ? two_input_new_lines[i] : 0;
        propagate_right =
            (til > cluster_new_lines[i] + propagate_right) ? 0 : propagate_right + cluster_new_lines[i] - til;

        // Move to the next cluster
        cluster_index_start = cluster_index_end + 1;
    }

    uint64_t propagate_left = 0;
    // propagate left (direction based on a 1D vector)
    for (auto i = two_input_indices.size(); i > 0; --i)
    {
        std::size_t cluster_index_end = two_input_indices[i - 1];

        if (i == 1)
        {
            cluster_index_start = 0;
        }
        else
        {
            cluster_index_start = two_input_indices[i - 2] + 1;
        }

        // Save the left propagated new_lines
        propagate_left = (two_input_new_lines[i - 1] > cluster_new_lines[i] + propagate_left) ?
                             0 :
                             propagate_left + cluster_new_lines[i] - two_input_new_lines[i - 1];

        // ALso set the new lines for two input nodes
        if (cluster_index_end != new_lines.size() - 1)
        {
            x[cluster_index_end] += propagate_left;
        }

        for (auto j = cluster_index_start; j < cluster_index_end; ++j)
        {
            x[j] += propagate_left;
        }
    }

    adjust_final_values(x, y, two_input_indices, two_input_new_lines);

    return std::make_pair(x, y);
}

/**
 * Implements the general planar layout generation algorithm.
 *
 * The algorithm performs placement and routing level by level, starting from the primary inputs and proceeding toward
 * the outputs. For each level, node placement depends on the orientation and excess wiring computed in auxiliary
 * routines such as `compute_pr_variables` and `compute_wiring`. Nodes are positioned according to their fan-in
 * structure and routing constraints to ensure planarity of the resulting layout.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param src Source network to be placed and routed.
 * @param p Parameters controlling layout generation and clocking.
 * @param st Statistics object used to collect runtime and layout information.
 */

template <typename Lyt, typename Ntk>
class plane_impl
{
  public:
    plane_impl(const Ntk& src, const planar_layout_from_network_embedding_params& p,
               planar_layout_from_network_embedding_stats& st) :
            ntk{mockturtle::fanout_view{src}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // initialize mapping from nodes to positions
        mockturtle::node_map<mockturtle::signal<Lyt>, mockturtle::fanout_view<Ntk>> node2pos{ntk};
        // initialize the aspect ratio
        aspect_ratio<Lyt> aspect_ratio = {0, 0};
        // instantiate the layout
        Lyt layout{aspect_ratio, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};
        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ntk);
        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{ntk.depth() + 1, "[i] arranging layout: |{0}|"};
#endif

        tile<Lyt> place_t{0, 0};
        assert(ntk.num_pis() > 0);
        tile<Lyt> first_pos = {ntk.num_pis() - 1, 0};
        // place and route the nodes in ascending level order
        for (uint32_t lvl = 0; lvl < ntk.depth() + 1; lvl++)
        {
            const auto variable_tuple = compute_pr_variables<mockturtle::fanout_view<Ntk>, Lyt>(ntk, node2pos, lvl);
            const auto orientation    = std::get<0>(variable_tuple);
            const auto new_lines      = std::get<1>(variable_tuple);

            const auto  wiring = compute_wiring<decltype(ntk), Lyt>(ntk, node2pos, new_lines, lvl);
            const auto& x      = wiring.first;
            const auto& y      = wiring.second;
            // place and route the nodes in ascending rank order
            ntk.foreach_node_in_rank(
                lvl,
                [this, &layout, &pi2node, &node2pos, &orientation, &first_pos, &place_t, &x, &y](const auto& n,
                                                                                                 const auto& i)
                {
                    if (!ntk.is_constant(n))
                    {
                        // if node is a PI, move it to its correct position
                        if (ntk.is_pi(n))
                        {
                            if (ntk.rank_position(n) == 0)
                            {
                                node2pos[n] = layout.move_node(pi2node[n], first_pos);
                                place_t     = first_pos;
                            }
                            else
                            {
                                place_t = {place_t.x - 1, place_t.y + 1};
                                // node2pos[n] = layout.move_node(pi2node[n], prec_pos);
                                if (place_t.x == 0)
                                {
                                    node2pos[n] = layout.move_node(pi2node[n], place_t);
                                }
                                else if (place_t.x < (first_pos.x / 2))
                                {
                                    node2pos[n] = layout.move_node(pi2node[n], {0, place_t.y});

                                    node2pos[n] =
                                        layout.create_buf(wire_east(layout, {0, place_t.y}, place_t), place_t);
                                }
                                else
                                {
                                    node2pos[n] = layout.move_node(pi2node[n], {place_t.x, 0});

                                    node2pos[n] =
                                        layout.create_buf(wire_south(layout, {place_t.x, 0}, place_t), place_t);
                                }
                            }
                        }
                        // if n has only one fanin
                        else if (const auto fc = fanins(ntk, n); fc.fanin_nodes.size() == 1)
                        {
                            const auto& pre   = fc.fanin_nodes[0];
                            auto        pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                            // Resolve new lines. Special case for the second fan-out of a fan-out node
                            if (!(ntk.is_fanout(pre) && (orientation[i] == 2 || orientation[i] == 3)))
                            {
                                if (x[i] != 0)
                                {
                                    wire_east(layout, pre_t, {pre_t.x + x[i] + 1, pre_t.y});
                                    pre_t.x += x[i];
                                }
                                if (y[i] != 0)
                                {
                                    wire_south(layout, pre_t, {pre_t.x, pre_t.y + y[i] + 1});
                                    pre_t.y += y[i];
                                }
                            }
                            // horizontal (corresponding to colored east)
                            if (orientation[i] == 0 || orientation[i] == 1)
                            {
                                place_t.y = pre_t.y;
                                place_t.x = pre_t.x + 1;
                            }
                            else
                            {
                                assert(orientation[i] == 2 || orientation[i] == 3);

                                // Resolve new lines. Special case for the second fan-out of a fan-out node
                                if (ntk.is_fanout(pre) && (orientation[i] == 2 || orientation[i] == 3))
                                {
                                    // n the special case the
                                    if (x[i] != 0)
                                    {
                                        pre_t.x += x[i];
                                    }
                                    if (y[i] != 0)
                                    {
                                        wire_south(layout, pre_t, {pre_t.x, pre_t.y + y[i] + 1});
                                        pre_t.y += y[i];
                                    }
                                }
                                place_t.y = pre_t.y + 1;
                                place_t.x = pre_t.x;
                            }

                            node2pos[n] = connect_and_place(layout, place_t, ntk, n, pre_t);

                            if (ntk.rank_position(n) == 0)
                            {
                                first_pos = place_t;
                            }
                        }
                        // if node has two fanins (or three fanins with one of them being constant)
                        else
                        {
                            const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                            auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                                 pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                            // Resolve new lines
                            if (x[i] != 0)
                            {
                                wire_east(layout, pre1_t, {pre1_t.x + x[i] + 1, pre1_t.y});
                                wire_east(layout, pre2_t, {pre2_t.x + x[i] + 1, pre2_t.y});
                                pre1_t.x += x[i];
                                pre2_t.x += x[i];
                            }
                            if (y[i] != 0)
                            {
                                wire_south(layout, pre1_t, {pre1_t.x, pre1_t.y + y[i] + 1});
                                wire_south(layout, pre2_t, {pre2_t.x, pre2_t.y + y[i] + 1});
                                pre1_t.y += y[i];
                                pre2_t.y += y[i];
                            }

                            // pre1_t is the northwards/eastern tile.
                            if (pre2_t.y < pre1_t.y)
                            {
                                std::swap(pre1_t, pre2_t);
                            }

                            place_t = {pre1_t.x, pre2_t.y};

                            node2pos[n] = connect_and_place(layout, place_t, ntk, n, pre1_t, pre2_t, fc.constant_fanin);

                            if (ntk.rank_position(n) == 0)
                            {
                                first_pos = place_t;
                            }
                        }
                    }
                });
#if (PROGRESS_BARS)
            // update progress
            bar(lvl);
#endif
        }

        // place and route POs
        std::unordered_map<typename mockturtle::fanout_view<Ntk>::node, int> count_map;
        int                                                                  add_line = 0;
        // the number of outputs on a node is limited to 2, due to fanout substitution
        ntk.foreach_po(
            [this, &layout, &first_pos, &place_t, &node2pos, &count_map, &add_line](const auto& po)
            {
                if (!ntk.is_constant(po))
                {
                    const auto drv     = ntk.get_node(po);
                    auto       po_tile = static_cast<tile<Lyt>>(node2pos[drv]);
                    auto&      cnt     = count_map[drv];
                    if (cnt < 2)  // Check if the count is less than 2
                    {
                        // Adjust the position based on whether it's the first or second occurrence
                        if (cnt == 1)
                        {
                            if (po_tile.y == place_t.y)
                            {
                                add_line = 1;
                            }
                            po_tile = static_cast<tile<Lyt>>(wire_south(layout, po_tile, {po_tile.x, po_tile.y + 2}));
                        }
                        const tile<Lyt> anker{po_tile};
                        po_tile.x = first_pos.x + 1;

                        // Create PO and increment the count
                        if constexpr (mockturtle::has_has_output_name_v<Ntk>)
                        {
                            layout.create_po(wire_east(layout, anker, po_tile),
                                             ntk.has_output_name(po_counter) ? ntk.get_output_name(po_counter++) :
                                                                               fmt::format("po{}", po_counter++),
                                             po_tile);
                        }
                        else
                        {
                            layout.create_po(wire_east(layout, anker, po_tile), fmt::format("po{}", po_counter++),
                                             po_tile);
                        }

                        ++cnt;
                    }
                    else
                    {
                        throw std::logic_error("Node has more than 2 primary outputs after fanout substitution");
                    }
                }
            });

        layout.resize({first_pos.x + 1, place_t.y + add_line, 0});

        // restore possibly set signal names
        restore_names(ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        if (ps.verbose)
        {
            std::cout << "\n[i] Layout generated:\n";

            std::cout << fmt::format("[i]   Layout dimension: {} × {} = {}\n", pst.x_size, pst.y_size,
                                     pst.x_size * pst.y_size);

            std::cout << fmt::format("[i]   #Gates:      {}\n", pst.num_gates);
            std::cout << fmt::format("[i]   #Wires:      {}\n", pst.num_wires);
            std::cout << fmt::format("[i]   #Crossings:  {}\n", pst.num_crossings);
        }

        return layout;
    }

  private:
    mockturtle::fanout_view<Ntk> ntk;

    planar_layout_from_network_embedding_params ps;
    planar_layout_from_network_embedding_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail

/**
 * This algorithm constructs a planar layout from the planar embedding of a logic network, forming the Planar Layout
 * from Network Embedding (PLANE) methodology. It provides a fully planar physical design flow for Field-Coupled
 * Nanocomputing (FCN) circuits. The algorithm operates on a logic network with an existing planar embedding,
 * represented as a `mutable_rank_view`, and preserves this embedding during placement and routing.
 *
 * In this approach, each logic level of the network is mapped to a diagonal in the layout, while nodes within the same
 * level are placed according to their rank positions in the planar embedding. This ensures a crossing-free, scalable,
 * and layout-consistent mapping from logic to physical design.
 *
 * @tparam Lyt  Gate-level layout type.
 * @tparam Ntk  Logic network type.
 * @param ntk   Planar logic network to be placed and routed.
 * @param ps    Configuration parameters for the physical design process.
 * @param pst   Optional statistics object to collect runtime and layout metrics.
 * @return      A fully planar gate-level layout of type `Lyt`.
 */
template <typename Lyt, typename Ntk>
Lyt plane(const Ntk& ntk, planar_layout_from_network_embedding_params ps = {},
          planar_layout_from_network_embedding_stats* pst = nullptr)
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

    // check for planarity
    mincross_stats  st_min{};
    mincross_params p_min{};
    p_min.optimize = false;

    auto ntk_min = mincross(ntk, p_min, &st_min);  // counts crossings
    if (st_min.num_crossings != 0)
    {
        throw std::invalid_argument("Input network has to be planar");
    }

    planar_layout_from_network_embedding_stats st{};
    detail::plane_impl<Lyt, Ntk>               p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction
#endif  // FICTION_PLANAR_LAYOUT_FROM_NETWORK_EMBEDDING_HPP
