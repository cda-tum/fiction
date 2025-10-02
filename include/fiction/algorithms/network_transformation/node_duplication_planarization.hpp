//
// Created by benjamin on 11.06.24.
//

#ifndef FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
#define FICTION_NODE_DUPLICATION_PLANARIZATION_HPP

#include "fiction/algorithms/properties/check_planarity.hpp"
#include "fiction/networks/views/mutable_rank_view.hpp"
#include "fiction/networks/virtual_pi_network.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

/**
 * Parameters for the node duplication algorithm.
 */
struct node_duplication_planarization_params
{
    /**
     * The output order determines the starting layer for this algorithm. If this option is turned off, the output order
     * remains the same as in the provided network. If it is turned on, the outputs are ordered randomly.
     */
    enum class output_order : uint8_t
    {
        /**
         * Keep the PO order from the input network.
         */
        KEEP_PO_ORDER,
        /**
         * Randomize the PO order.
         */
        RANDOM_PO_ORDER
    };
    // bool random_output_order = false;
    output_order po_order = output_order::KEEP_PO_ORDER;
};

namespace detail
{

/**
 * A structure representing a pair of nodes in an H-graph.
 *
 * The nodes stored in this struct describe the fanin-edges of a node in an H-graph.
 * A node pair object holds two nodes, which are saved in the member 'pair'.
 * These two outer nodes are connected through zero or more 'middle_nodes'.
 * The fanin order starts with the first node in 'pair', then proceeds through the 'middle_nodes', and ends with the
 * second node in 'pair'. The order of 'middle_nodes' is arbitrary as they cannot be further connected to any other
 * nodes. For the planarization, only the nodes inside the 'pair' are relevant.
 *
 * @tparam Ntk Network type for the nodes in the pair.
 */
template <typename Ntk>
struct node_pair
{
    /**
     * Defines the beginning and end of the fanin-edged node.
     */
    std::pair<mockturtle::node<Ntk>, mockturtle::node<Ntk>> pair;
    /**
     * Contains the nodes between the fanin-edges node; cannot be connected to any other node.
     */
    std::vector<mockturtle::node<Ntk>> middle_nodes;
    /**
     * Shared pointer to another instance of node_pair detailing fanin-edge alignment.
     */
    node_pair<Ntk>* fanin_pair;
    /**
     * Specifies the delay value for the node.
     */
    uint64_t delay;
    /**
     * Standard constructor.
     *
     * @param node1 The first node of the fanin-edged node.
     * @param node2 The second node of the fanin-edged node.
     * @param delay_value The delay value for the node.
     */
    node_pair(mockturtle::node<Ntk> node1, mockturtle::node<Ntk> node2, uint64_t delay_value) :
            pair(node1, node2),
            delay(delay_value),
            fanin_pair(nullptr)
    {}
};

/**
 * Variant of the mockturtle::initialize_copy_network. This function helps with creating new networks from old
 * networks. In the mockturtle/original version `old2new` is used to map nodes from the old network to nodes in the new
 * network in a one to one relation. This variant allows old nodes to map to multiple nodes in order to represent
 * relations to dulicated nodes.

 * A map (old2new) is created where old nodes from source network are mapped to new nodes in destination network.
 * A destination network is created as a virtual_pi_network<NtkSrc>.
 *
 * @tparam Ntk Type of the network.
 * @param src The source network.
 *
 * @return A pair of the destination network and a node map from the source to the destination network.
 */
template <typename Ntk>
std::pair<virtual_pi_network<Ntk>, mockturtle::node_map<std::vector<mockturtle::signal<virtual_pi_network<Ntk>>>, Ntk>>
initialize_copy_network_duplicates(Ntk const& src)
{
    static_assert(mockturtle::is_network_type_v<virtual_pi_network<Ntk>>, "Ntk is not a network type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_get_constant_v<virtual_pi_network<Ntk>>,
                  "Ntk does not implement the get_constant method");
    static_assert(mockturtle::has_create_pi_v<virtual_pi_network<Ntk>>, "Ntk does not implement the create_pi method");
    static_assert(mockturtle::has_is_pi_v<virtual_pi_network<Ntk>>, "Ntk does not implement the is_pi method");
    static_assert(mockturtle::has_create_not_v<virtual_pi_network<Ntk>>,
                  "Ntk does not implement the create_not method");
    static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant method");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi method");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
    static_assert(mockturtle::has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented method");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");

    mockturtle::node_map<std::vector<mockturtle::signal<virtual_pi_network<Ntk>>>, Ntk> old2new(src);
    virtual_pi_network<Ntk>                                                             dest;

    old2new[src.get_constant(false)].push_back(dest.get_constant(false));
    if (src.get_node(src.get_constant(true)) != src.get_node(src.get_constant(false)))
    {
        old2new[src.get_constant(true)].push_back(dest.get_constant(true));
    }
    src.foreach_pi([&](auto const& n) { old2new[n].push_back(dest.create_pi()); });
    return {dest, old2new};
}

/**
 * The function gather_fanin_signals collects the fanin data for node n from the original ntk.
 * For each node n there are the possible fanin candidates old2new_v[fn], which are the original node and all
 * the nodes which are duplicates of this node.
 *
 * lvl[node_index] gives the current iterator at where the edge can be connected. To get the right signal,
 * all nodes at old2new[n] need to be viewed. Match lvl[node_index] against all entries in old2new[n],
 * then try lvl[node_index+1] then try lvl[node_index+2].
 *
 * @param n Variable to process.
 * @param lvl Level to process.
 * @param edge_it Iterator for edge.
 * @return Vector of fanins in the virtual_pi_network connected to the processed node.
 */
template <typename Ntk, typename NtkDest>
std::vector<mockturtle::signal<Ntk>>
gather_fanin_signals(Ntk& ntk, NtkDest& ntk_dest_v, mockturtle::node<Ntk> n,
                     mockturtle::node_map<std::vector<mockturtle::signal<Ntk>>, Ntk>& old2new_v,
                     std::vector<mockturtle::node<Ntk>>& lvl, std::size_t& node_index)
{
    // Initialize variables
    std::vector<mockturtle::signal<Ntk>> children{};
    children.reserve(ntk.fanin_size(n));
    std::size_t local_node_index = 0;

    ntk.foreach_fanin(
        n,
        [&n, &ntk, &ntk_dest_v, &lvl, &old2new_v, &children, &node_index, &local_node_index](const auto& f,
                                                                                             const auto  i)
        {
            // Get the vector of duplicated nodes of the original fan-in node fn.
            const auto fn           = ntk.get_node(f);
            auto       tgt_signal_v = old2new_v[fn];

            assert(node_index < lvl.size() && "The fanin iterator is out of scope");

            // The range indicates the number of candidate fan-ins.
            std::size_t range = ntk.fanin_size(n) + 1;

            // Iterate through the candidate fan-ins. If a candidate fan-in matches the original fan-in or is a
            // duplicate of it, add it to the children of the node n.
            std::size_t end_index = std::min(node_index + range, lvl.size());
            for (std::size_t j = node_index; j < end_index; ++j)
            {
                // get the node from the newly generated network.
                auto node_at_index = lvl[j];

                // Check if the candidate matches the original fan-in or a duplicate.
                // Also, verify if the candidate has already reached its fan-out limit.
                if ((std::find(tgt_signal_v.cbegin(), tgt_signal_v.cend(), node_at_index) != tgt_signal_v.cend()) &&
                    (ntk_dest_v.fanout_size(node_at_index) < ntk.fanout_size(fn)))
                {
                    // Set the local node_index.
                    if (i == 0)
                    {
                        local_node_index = j;
                    }
                    else
                    {
                        local_node_index = std::max(local_node_index, j);
                    }

                    // Add the matched candidate fan-in to the children.
                    children.emplace_back(node_at_index);
                    break;
                }
            }
        });

    // Set the node_index.
    node_index = local_node_index;

    // Return the children of the node.
    return children;
};

/**
 * Constructs a planar `virtual_pi_network` based on the `ntk_lvls` array, which holds the ranks of the duplicated nodes
 * for each level in the new network. This function creates new nodes for the duplicated ones and restores their fanin
 * relations using the `gather_fanin_signals` function.
 *
 * For duplicated PIs (Primary Inputs), virtual PIs are created, and the original PI is stored in a map.
 *
 * The auxiliary function `gather_fanin_signals` collects fanin data for a node and matches it in the
 * `virtual_pi_network`.
 *
 * Example: For a level (2, 3, 2, 4, 2), new nodes are created for duplications (e.g., 2) and stored in the `old2new_v`
 * node_map. This map is used by `gather_fanin_signals` to establish the correct fanin relations.
 *
 * @tparam Ntk Network type.
 * @param ntk Source network to be utilized for the creation of the virtual_pi_network.
 * @param ntk_lvls Levels of nodes in the source network.
 * @param ntk_lvls_new Levels of newly created nodes in the virtual_pi_network.
 */
template <typename Ntk>
virtual_pi_network<Ntk>
create_virtual_pi_ntk_from_duplicated_nodes(Ntk& ntk, std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls,
                                            std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls_new)
{
    std::unordered_map<mockturtle::node<Ntk>, bool> node_status;
    ntk_lvls_new.resize(ntk_lvls.size());

    auto  init_v     = initialize_copy_network_duplicates(ntk);
    auto& ntk_dest_v = init_v.first;
    auto& old2new_v  = init_v.second;

    for (std::size_t i = ntk_lvls.size(); i-- > 0;)
    {
        // The index of the node in the current node level.
        std::size_t node_index = 0;

        // The current node level with duplicated nodes.
        // Example vector: {3, 2, 3}
        const auto& lvl = ntk_lvls[i];

        // The current node level in the new network, where duplicated nodes are created as new nodes.
        // Example vector {3, 2, 4}
        auto& lvl_new = ntk_lvls_new[i];

        // Create a node in the new network for each node contained in 'lvl'.
        for (const auto& nd : lvl)
        {
            // If the node is a PI create virtual PIs for duplicates.
            if (ntk.is_pi(nd))
            {
                if (node_status[nd])
                {
                    const auto& new_node = ntk_dest_v.create_virtual_pi(nd);
                    lvl_new.push_back(new_node);
                    old2new_v[nd].push_back(new_node);
                }
                else
                {
                    lvl_new.push_back(nd);
                    node_status[nd] = true;
                }
            }
            else
            {
                const auto children =
                    gather_fanin_signals(ntk, ntk_dest_v, nd, old2new_v, ntk_lvls_new[i + 1], node_index);

                assert(!children.empty() && "The node has to have children");

                const auto& new_node = ntk_dest_v.create_node(children, ntk.node_function(nd));
                lvl_new.push_back(new_node);
                old2new_v[nd].push_back(new_node);
            }
        }
    }

    ntk.foreach_po(
        [&ntk, &ntk_dest_v, &old2new_v](const auto& po)
        {
            const auto tgt_signal_v = old2new_v[ntk.get_node(po)];

            // POs are not duplicated as the algorithm starts at POs and duplicates other nodes based on their order
            assert(tgt_signal_v.size() == 1 && "Multiple nodes mapped to PO");

            const auto tgt_signal = tgt_signal_v[0];

            const auto tgt_po = ntk.is_complemented(po) ? ntk_dest_v.create_not(tgt_signal) : tgt_signal;

            ntk_dest_v.create_po(tgt_po);
        });

    return ntk_dest_v;
}

/**
 * Calculates pairs of nodes from a given vector of nodes.
 *
 * This function takes a vector of nodes and returns a vector of node pairs. Each node pair consists of two nodes from
 * the input vector and an optional vector of middle nodes. The delay of each node pair is initialized to infinity.
 *
 * @tparam Ntk The network type.
 * @param nodes The vector of nodes.
 * @return The vector of node pairs.
 */
template <typename Ntk>
[[nodiscard]] std::vector<node_pair<Ntk>> calculate_pairs(const std::vector<mockturtle::node<Ntk>>& nodes) noexcept
{
    std::vector<node_pair<Ntk>> pairwise_combinations{};
    pairwise_combinations.reserve(nodes.size() * (nodes.size() - 1));

    if (nodes.size() == 1)
    {
        const node_pair<Ntk> pair = {nodes[0], nodes[0],
                                     std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf
        pairwise_combinations.push_back(pair);
        return pairwise_combinations;
    }

    for (auto it1 = nodes.cbegin(); it1 != nodes.cend(); ++it1)
    {
        for (auto it2 = it1 + 1; it2 != nodes.cend(); ++it2)
        {
            std::vector<mockturtle::node<Ntk>> middle_nodes{};
            middle_nodes.reserve(nodes.size() - 2);

            // fill middle_nodes with non-pair members
            for (auto it = nodes.cbegin(); it != nodes.cend(); ++it)
            {
                if (it != it1 && it != it2)
                {
                    middle_nodes.push_back(*it);
                }
            }

            node_pair<Ntk> pair1 = {*it1, *it2, std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf
            node_pair<Ntk> pair2 = {*it2, *it1, std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf

            // Add middle_nodes to pairs
            pair1.middle_nodes = middle_nodes;
            pair2.middle_nodes = middle_nodes;

            pairwise_combinations.push_back(pair1);
            pairwise_combinations.push_back(pair2);
        }
    }

    return pairwise_combinations;
}

template <typename Ntk>
class node_duplication_planarization_impl
{
  public:
    node_duplication_planarization_impl(const Ntk& src, const node_duplication_planarization_params& p) :
            ntk(src),
            ps{p}
    {}

    /**
     * The H-graph represents all possible orderings of node pairs within a single network level.
     * A "slice" is created by adding all possible combinations of a `node_pair` to the H-graph of the level.
     * These combinations are formed by selecting pairs of nodes from the fan-ins of the input node:
     * - If the input node has only one fan-in, it is treated as a single combination.
     * - If the input node has two fan-ins, there are two possible combinations.
     *
     * Each `node_pair` consists of a first and second element. The objective is to find an ordering of node pairs that
     * maximizes the instances where the first element of a node_pair matches the second element of the preceding
     * node_pair. This ordering is given as a linked list.
     *
     * This function computes the optimal ordering by calculating delays as follows:
     * - All combinations of node pairs are iteratively added to a linked list.
     * - For each combination, the first element of the current node_pair is compared with the last element of the
     * preceding node_pairs.
     * - If a connection exists between two node_pairs, the delay increases by 1; otherwise, it increases by 2. The
     * default delay for the first node is 1.
     * - If a node_pair lacks a connection, and its updated delay (increased by 2) is less than the existing delay, the
     * node_pair's delay is updated accordingly.
     *
     * Processed node_pairs are stored in the `lvl_pairs` member for subsequent delay calculations.
     *
     * @param nd Node in the H-graph.
     * @param border_pis A boolean indicating whether the input PIs (Primary Inputs) should be propagated to the next
     */

    void compute_slice_delays(const mockturtle::node<Ntk>& nd)
    {
        // Pis need to be propagated into the next level, since they have to be connected without crossings
        if (ntk.is_pi(nd))
        {
            fis.push_back(nd);
        }

        ntk.foreach_fanin(nd,
                          [this](auto fi)
                          {
                              if (!ntk.is_constant(fi))
                              {
                                  fis.push_back(fi);
                              }
                          });

        assert(fis.empty() == 0 && "Node is a buffered PI that is a PO");

        // Compute the combinations in one slice
        auto combinations = calculate_pairs<Ntk>(fis);
        assert(!combinations.empty() && "Combinations are empty. There might be a dangling node");

        if (!lvl_pairs.empty())
        {
            std::vector<node_pair<Ntk>>* combinations_last = &lvl_pairs.back();

            for (auto& node_pair_cur : combinations)
            {
                for (auto& node_pair_last : *combinations_last)
                {
                    // If there is a connection between the two node pairs the delay is calculated like this
                    if ((node_pair_cur.pair.first == node_pair_last.pair.second &&
                         node_pair_last.delay + 1 < node_pair_cur.delay))
                    {
                        node_pair_cur.fanin_pair = &node_pair_last;
                        node_pair_cur.delay      = node_pair_last.delay + 1;
                    }
                    // If there is no connection between the two node pairs the delay is calculated like this
                    else if (node_pair_last.delay + 2 < node_pair_cur.delay)
                    {
                        node_pair_cur.fanin_pair = &node_pair_last;
                        node_pair_cur.delay      = node_pair_last.delay + 2;
                    }
                    else if (node_pair_last.delay + 2 == node_pair_cur.delay)
                    {
                        // ToDo: If order doesnt matter, decide on minimal crossing view (mincross.c from graphviz)

                        // This solves equal path delays, if they are connected in the next layer via a fanout
                        const auto fc0 = fanins(ntk, node_pair_last.pair.first);
                        if (node_pair_last.fanin_pair != nullptr)
                        {
                            const auto fc1 = fanins(ntk, node_pair_last.fanin_pair->pair.second);
                            for (const auto f0 : fc0.fanin_nodes)
                            {
                                for (const auto f1 : fc1.fanin_nodes)
                                {
                                    if (f0 == f1)
                                    {
                                        node_pair_cur.fanin_pair = &node_pair_last;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // The delay for the first node in the level is set to 1
            for (auto& node_pair : combinations)
            {
                node_pair.delay = 1;
            }
        }

        lvl_pairs.push_back(combinations);
    }

    /**
     * Inserts a node into a vector if it is unique.
     *
     * `This function inserts a node into a vector only if the vector is empty or the node is not equal to the first
     * element of the vector. If the vector is not empty and the node is equal to the first element, it does nothing.
     * An exception occurs if the node was skipped on the previous insertion attempt due to `vec.front() == node`; in
     * that case, the node will be inserted this time.
     *
     * @param node The node to be inserted.
     * @param vec The vector to insert the node into.
     */
    void insert_if_not_first(const mockturtle::node<Ntk>& node, std::vector<mockturtle::node<Ntk>>& vec,
                             int& saturated_fanout_flag, int position)
    {
        if (vec.empty() || vec.front() != node)
        {
            vec.insert(vec.begin(), node);
            saturated_fanout_flag = 0;
        }
        else if (position == 0)
        {
            if (saturated_fanout_flag == 1)
            {
                vec.insert(vec.begin(), node);
                saturated_fanout_flag = 0;
            }
            else
            {
                saturated_fanout_flag = 1;
            }
        }
    }

    /**
     * Computes the order of nodes in the next level based on the delay (shortest path) in the H-graph of the level.
     *
     * This function computes the order of nodes in the next level based on their delay in the H-graph of the level. It
     * selects the path with the least delay from the current level pairs and follows it via fanin relations. The nodes
     * are inserted into the next level vector in the order they are encountered.
     *
     * @return The order of nodes in `next_level`
     */
    std::vector<mockturtle::node<Ntk>> compute_node_order()
    {
        std::vector<mockturtle::node<Ntk>> next_level;
        int                                saturated_fanout_flag = 0;
        const auto&                        combinations          = lvl_pairs.back();
        // select the path with the least delay and follow it via fanin relations
        const auto minimum_it =
            std::min_element(combinations.cbegin(), combinations.cend(),
                             [](const node_pair<Ntk>& a, const node_pair<Ntk>& b) { return a.delay < b.delay; });
        if (minimum_it != combinations.cend())
        {
            const auto& min_combination = *minimum_it;

            // Insert the terminal node
            insert_if_not_first(min_combination.pair.second, next_level, saturated_fanout_flag, 0);

            // insert middle_nodes
            for (const auto& node : min_combination.middle_nodes)
            {
                insert_if_not_first(node, next_level, saturated_fanout_flag, 1);
            }

            // Insert the first node
            insert_if_not_first(min_combination.pair.first, next_level, saturated_fanout_flag, 1);

            auto fanin_combination = minimum_it->fanin_pair;

            while (fanin_combination)
            {
                // Insert the terminal node
                if (ntk.is_pi(fanin_combination->pair.second))
                {
                    saturated_fanout_flag = 1;
                }
                insert_if_not_first(fanin_combination->pair.second, next_level, saturated_fanout_flag, 0);

                // Insert middle_nodes
                for (const auto& node : fanin_combination->middle_nodes)
                {
                    insert_if_not_first(node, next_level, saturated_fanout_flag, 1);
                }

                // insert the first node
                insert_if_not_first(fanin_combination->pair.first, next_level, saturated_fanout_flag, 1);

                fanin_combination = fanin_combination->fanin_pair;
            }
        }
        return next_level;
    }

    /**
     * Checks if the given vector of nodes contains any non-primary inputs.
     *
     * This function iterates through each node in the vector and checks if it is a primary input.
     * If a non-primary input is found, the `f_final_level` parameter is set to false and the loop is exited.
     *
     * @param v_next_level The vector of nodes to be checked.
     */
    [[nodiscard]] bool check_final_level(const std::vector<mockturtle::node<Ntk>>& v_next_level)
    {
        for (const auto& nd : v_next_level)
        {
            if (!ntk.is_pi(nd))
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] virtual_pi_network<Ntk>
    run(std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls_new)
    {
        // Initialize the POs
        std::vector<mockturtle::node<Ntk>> pos{};
        pos.reserve(ntk.num_pos());
        ntk.foreach_node(
            [this, &pos](const auto n)
            {
                if(ntk.is_po(n))
                {
                    const auto po = ntk.get_node(n);
                    if (std::find(pos.begin(), pos.end(), po) == pos.end())
                    {
                        pos.push_back(po);
                    }
                }
            });

        // Randomize the PO order
        if (ps.po_order == node_duplication_planarization_params::output_order::RANDOM_PO_ORDER)
        {
            // Generate a random engine
            static std::mt19937_64 generator(std::random_device{}());
            // Shuffle the pos vector
            std::shuffle(pos.begin(), pos.end(), generator);
        }

        // save the nodes of the next level
        std::vector<mockturtle::node<Ntk>> next_level{};
        next_level.reserve(pos.size());

        // Process the first level
        for (const auto& po : pos)
        {
            fis.clear();
            compute_slice_delays(po);
            next_level.push_back(po);
        }

        ntk_lvls.push_back(next_level);
        next_level.clear();

        next_level = compute_node_order();

        // check if the final/PI level is reached
        bool f_final_level = check_final_level(next_level);

        // Process all other levels
        while (!next_level.empty() && !f_final_level)
        {
            // Push the level to the node array
            ntk_lvls.push_back(next_level);
            lvl_pairs.clear();

            // Store the nodes of the next level
            for (const auto& cur_node : next_level)
            {
                fis.clear();

                // There is one slice in the H-Graph for each node in the level
                compute_slice_delays(cur_node);
            }
            // Clear before starting computations on the next level
            next_level.clear();
            // Compute the next level
            next_level = compute_node_order();
            // Check if we are at the final level
            f_final_level = check_final_level(next_level);
        }
        // Push the final level (PIs)
        if (f_final_level)
        {
            ntk_lvls.push_back(next_level);
        }

        // create virtual pi network
        auto virtual_ntk = create_virtual_pi_ntk_from_duplicated_nodes(ntk, ntk_lvls, ntk_lvls_new);

        // the ntk_levels were created in reverse order
        std::reverse(ntk_lvls_new.begin(), ntk_lvls_new.end());

        // assign the ranks in the virtual network based on ntk_lvls_new
        virtual_ntk.update_ranks();
        virtual_ntk.set_all_ranks(ntk_lvls_new);

        return virtual_ntk;
    }

  private:
    /*
     * The input network.
     */
    Ntk ntk{};
    /*
     * The currently node_pairs used in the current level.
     */
    std::vector<std::vector<node_pair<Ntk>>> lvl_pairs{};
    /*
     * The fanin nodes.
     */
    std::vector<mockturtle::node<Ntk>> fis{};
    /*
     * The network stored as levels.
     */
    std::vector<std::vector<mockturtle::node<Ntk>>> ntk_lvls{};
    /*
     * The stats of the node_duplication class.
     */
    node_duplication_planarization_params ps{};
};

}  // namespace detail

/**
 * Implements a planarization mechanism for networks using a H-Graph strategy for node duplication.
 *
 * The planarization achieved by this function solves the Node Duplication Crossing Minimization (NDCE) problem by
 * finding the shortest x-y path in the H-graph for every level in the network. An H-graph describes edge relations
 * between two levels in a network, with one level assumed as fixed, starting at the Primary Outputs (POs). By finding
 * the shortest path from the source (x) to the sink (y) in this H-graph, an optimal solution for the NDCE problem for
 * each level is found. The function constructs an H-graph that captures edge relations between two levels within the
 * graph and computes the shortest x-y paths on the H-graph, traversing from the POs towards the Primary Inputs (PIs).
 *
 * @tparam NtkDest Destination network type.
 * @tparam NtkSrc Source network type.
 * @param ntk_src Source network to be utilized for the planarization.
 * @param ps Node duplication parameters used in the computation.
 *
 * @return A view of the planarized virtual_pi_network created in the format of mutable_rank_view.
 */
template <typename NtkSrc>
[[nodiscard]] virtual_pi_network<NtkSrc>
node_duplication_planarization(const NtkSrc& ntk_src, const node_duplication_planarization_params& ps = {})
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::has_create_node_v<NtkSrc>, "NtkSrc does not implement the create_node function");
    static_assert(mockturtle::has_rank_position_v<NtkSrc>, "NtkSrc does not implement the rank_position function");

    assert(is_balanced(ntk_src) && "Networks have to be balanced for this duplication");

    detail::node_duplication_planarization_impl p{ntk_src, ps};

    std::vector<std::vector<mockturtle::node<NtkSrc>>> ntk_lvls_new;

    auto result = p.run(ntk_lvls_new);

    assert(check_planarity(result) && "Planarization failed: Network should be planar");

    return result;
}

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
