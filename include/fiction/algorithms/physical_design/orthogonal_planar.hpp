//
// Created by benjamin on 21.01.25.
//

#ifndef FICTION_ORTHOGONAL_PLANAR_HPP
#define FICTION_ORTHOGONAL_PLANAR_HPP

#include "fiction/algorithms/properties/check_planarity.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/network_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

// Define a 3D array using std::array and encapsulate it in a function
std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 2>& get_buffer_lookup()
{
    static std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 2> array = {
        {    // Array
         {{  // Unconnected
           {{// East
             {{
                 {0, 0},
                 {1, 0},
                 {1, 1},
                 {0, 1}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {1, 0},
                 {0, 0},  // gap 1

             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0},  // gap 2 and higher

             }}}},
           {{// South
             {{
                 {3, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 0; only first two entries used
             }},
             {{
                 {3, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {3, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// Free
             {{
                 {0, 0},
                 {1, 0},
                 {2, 0},
                 {3, 0}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {1, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2 and higher
             }}}}}},
         {{  // Connected
           {{// East
             {{
                 {0, 0},
                 {0, 0},
                 {0, 1},
                 {0, 1}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2 and higher
             }}}},
           {{// South
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  // gap 2 and higher
             }}}},
           {{// Free
             {{
                 {0, 0},
                 {0, 0},
                 {3, 0},
                 {3, 0}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}}}}}};
    return array;
}

// Define a 3D array using std::array and encapsulate it in a function
std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 4>& get_fanout_lookup()
{
    static std::array<std::array<std::array<std::array<std::pair<uint64_t, uint64_t>, 4>, 3>, 3>, 4> array = {
        {    // Array
         {{  // Type Fo 1+2
           {{// East
             {{
                 {1, 0},
                 {1, 1},
                 {1, 2},
                 {1, 1}  // gap 0
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 1},
                 {1, 0}  // gap 1
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 2
             }}}},
           {{// South
             {{
                 {2, 0},
                 {2, 1},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {2, 0},
                 {2, 1},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {2, 0},
                 {2, 1},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// Free
             {{
                 {1, 0},
                 {2, 0},
                 {2, 1},
                 {2, 0}  // gap 0
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {2, 0},
                 {1, 0}  // gap 1
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 2
             }}}}}},
         {{  // Type F1
           {{// East
             {{
                 {0, 0},
                 {0, 1},
                 {0, 2},
                 {0, 1}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 1},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// South
             {{
                 {3, 0},
                 {3, 1},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {3, 0},
                 {3, 1},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {3, 0},
                 {3, 1},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// Free
             {{
                 {0, 0},
                 {3, 0},
                 {3, 1},
                 {3, 0}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {3, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}}}},
         {{  // Type F2
           {{// East
             {{
                 {1, 0},
                 {1, 0},
                 {1, 1},
                 {1, 1}  // gap 0
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 1
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 2
             }}}},
           {{// South
             {{
                 {2, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {2, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {2, 0},
                 {2, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// Free
             {{
                 {1, 0},
                 {1, 0},
                 {2, 0},
                 {2, 0}  // gap 0
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 1
             }},
             {{
                 {1, 0},
                 {1, 0},
                 {1, 0},
                 {1, 0}  // gap 2
             }}}}}},
         {{  // Type F3
           {{// East
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}},
           {{// South
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  // gap 0
             }},
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {3, 0},
                 {3, 0},
                 {0, 0},
                 {0, 0}  /// gap 2
             }}}},
           {{// Free
             {{
                 {0, 0},
                 {0, 0},
                 {3, 0},
                 {3, 0}  // gap 0
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 1
             }},
             {{
                 {0, 0},
                 {0, 0},
                 {0, 0},
                 {0, 0}  // gap 2
             }}}}}}}};
    return array;
}

template <typename Ntk>
uint64_t calculate_fanout_connection_type(const Ntk& ntk, mockturtle::node<Ntk> n)
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

template <typename Ntk, typename Lyt>
uint64_t calculate_predecessor_gap(const Ntk& ntk, mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                   uint64_t lvl, mockturtle::node<Ntk> n)
{
    // return if in the PI level
    if (lvl == 0)
    {
        return 0;
    }

    // calculate the rank of the predecessor node
    auto fc = fanins(ntk, n);

    mockturtle::node<Ntk> nd = n;
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
    auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre]);
    auto pre2_t = static_cast<tile<Lyt>>(node2pos[pre_neighbour]);

    assert(pre1_t.y > pre2_t.y);
    return std::min(pre1_t.y - pre2_t.y - 1, 2);
}

template <typename Ntk>
uint64_t calculate_connection(const Ntk& ntk, mockturtle::node<Ntk> n)
{
    if (ntk.is_po(n))
    {
        return false;
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

template <typename Ntk>
uint64_t calculate_allowed_orientation(const Ntk& ntk, mockturtle::node<Ntk> n)
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

template <typename Ntk>
int calculate_start_orientation(Ntk& ntk, uint32_t lvl)
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
                                     // it is 3 but return makes -1
                                     orientation = 4;
                                     return;
                                 }
                                 if (const auto fc = fanins(ntk, n); fc.fanin_nodes.size() == 1)
                                 {
                                     const auto& pre = fc.fanin_nodes[0];
                                     if (ntk.is_fanout(pre) && ntk.fanout_size(pre) == 2)
                                     {
                                         // it is 0 but return makes -1
                                         orientation = 1;
                                         return;
                                     }
                                 }
                                 if(ntk.fanout_size(n) == 2)
                                 {
                                     // it is 0 but return makes -1
                                     orientation = 1;
                                     return;
                                 }

                             });
    return (orientation == 0) ? 0 : (orientation - 1);
    // instead of just returning 0, here the buffers could be wired in a way they are closer together and hence overhead
    // produced by two input nodes is reduced
}

template <typename Ntk, typename Lyt>
std::tuple<std::vector<uint64_t>, std::vector<uint64_t>>
compute_pr_variables(const Ntk& ntk, const Lyt& lyt, mockturtle::node_map<mockturtle::signal<Lyt>, Ntk> node2pos,
                     uint32_t lvl)
{
    // ToDo: compute the starting orientation
    std::vector<uint64_t> orientation(ntk.rank_width(lvl));
    std::vector<uint64_t> new_lines(ntk.rank_width(lvl));
    // get the lookup tables for the gate types
    const auto& buffer_lu = get_buffer_lookup();
    const auto& fanout_lu = get_fanout_lookup();

    ntk.foreach_node_in_rank(
        lvl,
        [&ntk, &node2pos, &lvl, &orientation, &new_lines, &buffer_lu, &fanout_lu](const auto& n, const auto& i)
        {
            // calculate the gap between the predecessors
            uint64_t gap = calculate_predecessor_gap<Ntk, Lyt>(ntk, node2pos, lvl, n);

            // needs gap and orientation as input
            if (ntk.fanin_size(n) == 2)  // complete
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
                const uint64_t allowed_orientation = calculate_allowed_orientation(ntk, n);
                assert(allowed_orientation < 3);

                // needs the type of connection (F1+2, F1, F2), allowed_orientation, gap,
                // orientation, new_line as input
                if (ntk.is_fanout(n))
                {
                    // calculate the type of connection F1+2 = 0, F1 = 1, F2 = 2, F0 = 3;
                    const uint64_t fanout_connection_type = calculate_fanout_connection_type(ntk, n);
                    assert(fanout_connection_type < 4);
                    if (i != 0)
                    {
                        const std::pair<uint64_t, uint64_t> pair =
                            fanout_lu[fanout_connection_type][allowed_orientation][gap][orientation[i - 1]];
                        orientation[i] = pair.first;
                        new_lines[i]   = pair.second;
                    }
                    else if (fanout_connection_type == 0 || fanout_connection_type == 2)
                    {
                        // i == 0
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
                        uint64_t                            connected = calculate_connection(ntk, n);
                        const std::pair<uint64_t, uint64_t> pair =
                            buffer_lu[connected][allowed_orientation][gap][orientation[i - 1]];
                        orientation[i] = pair.first;
                        new_lines[i]   = pair.second;
                    }
                    else
                    {
                        // i == 0
                        orientation[i] = calculate_start_orientation(ntk, lvl);
                    }
                }
            }
        });

    const auto ret = std::make_tuple(orientation, new_lines);
    return ret;
}

template <typename Ntk>
std::vector<std::size_t> compute_two_input_indices(const Ntk& ntk, uint64_t lvl)
{
    std::vector<std::size_t> two_input_indices;
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

template <typename Ntk, typename Lyt>
std::vector<uint64_t> calculate_two_input_new_lines(const Ntk&                                          ntk,
                                                    mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                                    uint32_t                                            lvl)
{
    std::vector<uint64_t> cluster_new_lines;
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

                                     auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                                          pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                                     cluster_new_lines.emplace_back(static_cast<uint64_t>(pre2_t.y - pre1_t.y - 1));
                                 }
                             });
    return cluster_new_lines;
}

void adjust_final_values(std::vector<uint64_t>& x, std::vector<uint64_t>& y,
                         const std::vector<uint64_t>&    two_input_indices,
                         const std::vector<std::size_t>& two_input_new_lines)
{
    // Find the max element in two_input_new_lines and its index
    auto           max_it                   = std::max_element(two_input_new_lines.begin(), two_input_new_lines.end());
    const uint64_t max_new_lines_two_inputs = *max_it;
    const auto     max_new_lines_two_inputs_index =
        static_cast<const size_t>(std::distance(two_input_new_lines.begin(), max_it));

    // Find max sum in x + y and its index
    auto max_xy_it = std::max_element(x.begin(), x.end(),
                                      [&](std::size_t i, std::size_t j) { return (x[i] + y[i]) < (x[j] + y[j]); });

    const uint64_t    max_new_lines = x[*max_xy_it] + y[*max_xy_it];
    const std::size_t mnl_iterator  = *max_xy_it;

    // Determine the center index
    uint64_t center = 0, max = 0;
    if (max_new_lines > max_new_lines_two_inputs)
    {
        center = mnl_iterator;
        max    = max_new_lines;
    }
    else
    {
        center = two_input_indices[max_new_lines_two_inputs_index];
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
        if (it != two_input_map.end())  // Two fan-in node (found in two_input_indices)
        {
            const std::size_t idx  = it->second;  // Get the corresponding index
            const uint64_t    diff = max - x[i] - y[i] - two_input_new_lines[idx];

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
        else  // One fan-in node
        {
            const uint64_t diff = max - x[i] - y[i];

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

template <typename Ntk, typename Lyt>
std::pair<std::vector<uint64_t>, std::vector<uint64_t>>
compute_wiring(const Ntk& ntk, mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
               const std::vector<uint64_t>& new_lines, uint64_t lvl)
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

    // Example
    /*std::vector<uint64_t> two_input_indices = {0, 4, 9, 16, 20};
    std::vector<uint64_t> two_input_new_lines = {0, 0, 2, 5, 0};
    std::vector<uint64_t> x(21);
    std::vector<uint64_t> y(21);
    std::vector<uint64_t> cluster_new_lines(two_input_indices.size() + 1);
    std::size_t cluster_index_start = 0;
    std::vector<uint64_t> new_lines = {0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0};*/

    // if there is no two input gate, then there is only one cluster
    if (two_input_indices.empty())
    {
        for (std::size_t j = 0; j < ntk.rank_width(lvl); ++j)
        {
            x[j] = std::accumulate(new_lines.begin() + static_cast<int>(j) + 1,
                                   new_lines.begin() + static_cast<int>(ntk.rank_width(lvl)), 0UL);
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
        for (std::size_t j = cluster_index_start; j < cluster_index_end; ++j)
        {
            // adjust x values
            x[j] = std::accumulate(new_lines.begin() + static_cast<int>(j) + 1,
                                   new_lines.begin() + static_cast<int>(cluster_index_end), 0UL);

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
        propagate_right = (two_input_new_lines[i] > cluster_new_lines[i] + propagate_right) ?
                              0 :
                              propagate_right + cluster_new_lines[i] - two_input_new_lines[i];

        // Move to the next cluster
        cluster_index_start = cluster_index_end + 1;
    }

    uint64_t propagate_left = 0;
    // propagate left (direction based on a 1D vector)
    for (std::size_t i = two_input_indices.size(); i > 0; --i)
    {
        std::size_t cluster_index_end = 0;

        cluster_index_end = two_input_indices[i - 1];

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

        for (std::size_t j = cluster_index_start; j < cluster_index_end; ++j)
        {
            x[j] += propagate_left;
        }
    }

    adjust_final_values(x, y, two_input_indices, two_input_new_lines);

    return std::make_pair(x, y);
}

template <typename Lyt, typename Ntk>
class orthogonal_planar_v2_impl
{
  public:
    orthogonal_planar_v2_impl(const Ntk& src, const orthogonal_physical_design_params& p,
                              orthogonal_physical_design_stats& st) :
            ntk{mockturtle::names_view(mockturtle::fanout_view(src))},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        //
        using node = typename Ntk::node;

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ntk)> node2pos{ntk};

        aspect_ratio<Lyt> size_ = {0, 0};

        // instantiate the layout
        Lyt layout{size_, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{ctn.color_ntk.size(), "[i] arranging layout: |{0}|"};
#endif

        tile<Lyt> place_t{0, 0};
        tile<Lyt> first_pos = {ntk.num_pis() - 1, 0};

        for (uint32_t lvl = 0; lvl < ntk.depth() + 1; lvl++)
        {
            const auto variable_tuple = compute_pr_variables(ntk, layout, node2pos, lvl);
            const auto orientation    = std::get<0>(variable_tuple);
            const auto new_lines      = std::get<1>(variable_tuple);

            const auto  wiring = compute_wiring<decltype(ntk), Lyt>(ntk, node2pos, new_lines, lvl);
            const auto& x      = wiring.first;
            const auto& y      = wiring.second;

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
                        else  // if node has two fanins (or three fanins with one of them being constant)
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
        }

        // layout.resize({first_pos.x + 1, place_t.y +3, 0});
        // debug::write_dot_layout(layout);

        std::unordered_map<int, int> countMap;
        int                          add_line = 0;
        // the number of outputs on a node is limited to 2, due to fanout substitution
        ntk.foreach_po(
            [&](const auto& po)
            {
                if (!ntk.is_constant(po))
                {
                    const auto n_s     = node2pos[po];
                    auto       po_tile = static_cast<tile<Lyt>>(n_s);
                    if (countMap[po] < 2)  // Check if the count is less than 2
                    {
                        // Adjust the position based on whether it's the first or second occurrence
                        if (countMap[po] == 1)
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
                        layout.create_po(wire_east(layout, anker, po_tile),
                                         ntk.has_output_name(po_counter) ? ntk.get_output_name(po_counter++) :
                                                                           fmt::format("po{}", po_counter++),
                                         po_tile);
                        countMap[po]++;
                    }
                    else
                    {
                        assert(false);
                    }
                }
            });

        layout.resize({first_pos.x + 1, place_t.y + add_line, 0});

        // layout.resize({10, 10, 0});

        // restore possibly set signal names
        restore_names(ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        return layout;
    }

  private:
    mockturtle::names_view<mockturtle::fanout_view<Ntk>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail

/**
 * Description
 */
template <typename Lyt, typename Ntk>
Lyt orthogonal_planar_v2(const Ntk& ntk, orthogonal_physical_design_params ps = {},
                         orthogonal_physical_design_stats* pst = nullptr)
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
    if (!check_planarity(ntk))
    {
        throw std::runtime_error("Input network has to be planar");
    }

    orthogonal_physical_design_stats            st{};
    detail::orthogonal_planar_v2_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction
#endif  // FICTION_ORTHOGONAL_PLANAR_HPP
