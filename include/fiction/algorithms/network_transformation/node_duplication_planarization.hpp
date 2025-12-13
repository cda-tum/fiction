//
// Created by benjamin on 11.06.24.
//

#ifndef FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
#define FICTION_NODE_DUPLICATION_PLANARIZATION_HPP

#include "fiction/algorithms/graph/mincross.hpp"
#include "fiction/algorithms/network_transformation/network_balancing.hpp"
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
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Parameters for the node duplication algorithm.
 */
struct node_duplication_planarization_params
{
    /**
     * Controls how output nodes are ordered before starting the algorithm.
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
    /**
     * The output order used. Defaults to KEEP_PO_ORDER.
     */
    output_order po_order = output_order::KEEP_PO_ORDER;
};

namespace detail
{

/**
 * Represents one node in the H-graph used for crossing minimization.
 *
 * For a node in level l of the input network, all possible orderings of its fanins from layer l−1 are enumerated.
 * Each such ordering is represented by an H-graph node. The first and last fanins of the ordering are stored, since
 * these determine the delay in the H-graph. The remaining fanins are placed in middle. Their mutual order is irrelevant
 * for this algorithm.
 *
 * @tparam Ntk Network type from which node types are drawn.
 */
template <typename Ntk>
struct hgraph_node
{
    /**
     * First and last fanin.
     */
    std::pair<mockturtle::node<Ntk>, mockturtle::node<Ntk>> outer_fanins;
    /**
     * All remaining fanins.
     */
    std::vector<mockturtle::node<Ntk>> middle_fanins;
    /**
     * Specifies the delay value for the hgraph_node.
     */
    uint64_t delay;
    /**
     * Index of the predecessor H-graph node.
     */
    std::size_t fanin_it{};
    /**
     * Constructs an H-graph node with given first and last fanins and delay.
     *
     * @param first The first (leftmost) fanin in the ordering.
     * @param last  The last (rightmost) fanin in the ordering.
     * @param delay_value The delay value for the node.
     */
    hgraph_node(const mockturtle::node<Ntk>& first, const mockturtle::node<Ntk>& last, const uint64_t delay_value) :
            outer_fanins(first, last),
            delay(delay_value)
    {}
};

/**
 * Variant of the `mockturtle::initialize_copy_network`. This function helps with creating new networks from old
 * networks. In the mockturtle/original version `old2new` is used to map nodes from the old network to nodes in the new
 * network in a one-to-one relation. This variant allows old nodes to map to multiple nodes in order to represent
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
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant method");
    static_assert(mockturtle::has_get_constant_v<virtual_pi_network<Ntk>>,
                  "virtual_pi_network<Ntk> does not implement get_constant");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi method");
    static_assert(mockturtle::has_create_pi_v<virtual_pi_network<Ntk>>,
                  "virtual_pi_network<Ntk> does not implement create_pi");

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
std::vector<mockturtle::signal<NtkDest>>
gather_fanin_signals(const Ntk& ntk, NtkDest& ntk_dest_v, const mockturtle::node<Ntk> n,
                     const mockturtle::node_map<std::vector<mockturtle::signal<NtkDest>>, Ntk>& old2new_v,
                     const std::vector<mockturtle::node<NtkDest>>& lvl, std::size_t& node_index)
{
    // Initialize variables
    std::vector<mockturtle::signal<NtkDest>> children{};
    children.reserve(ntk.fanin_size(n));
    std::size_t local_node_index = 0;

    ntk.foreach_fanin(
        n,
        [&n, &ntk, &ntk_dest_v, &lvl, &old2new_v, &children, &node_index, &local_node_index](const auto& f,
                                                                                             const auto  i)
        {
            // Get the vector of duplicated nodes of the original fan-in node fn.
            const auto  fn           = ntk.get_node(f);
            const auto& tgt_signal_v = old2new_v[fn];

            assert(node_index < lvl.size() && "The fanin iterator is out of scope");

            // The range indicates the number of candidate fan-ins.
            const std::size_t max_candidates = ntk.fanin_size(n) + 1;

            // Iterate through the candidate fan-ins. If a candidate fan-in matches the original fan-in or is a
            // duplicate of it, add it to the children of the node n.
            const std::size_t end_index = std::min(node_index + max_candidates, lvl.size());
            for (auto j = node_index; j < end_index; ++j)
            {
                // get the node from the newly generated network.
                const auto node_at_index = lvl[j];
                const auto candidate_sig = ntk_dest_v.make_signal(node_at_index);

                // Check if the candidate matches the original fan-in or a duplicate.
                // Also, verify if the candidate has already reached its fan-out limit.
                if ((std::find(tgt_signal_v.cbegin(), tgt_signal_v.cend(), candidate_sig) != tgt_signal_v.cend()) &&
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
                    children.emplace_back(ntk.is_complemented(f) ? ntk_dest_v.create_not(candidate_sig) :
                                                                   candidate_sig);
                    break;
                }
            }
        });

    // Set the node_index.
    node_index = local_node_index;

    // Return the children of the node.
    return children;
}

/**
 * Constructs a planar `virtual_pi_network` based on duplicated nodes derived from the source network.
 *
 * The input `ntk_lvls` contains per-level vectors of original node ranks in the source network. For each level, this
 * function creates corresponding nodes (including duplicates) in a new `virtual_pi_network` and restores their fanin
 * relations using the `gather_fanin_signals` helper function.
 *
 * For duplicated PIs (Primary Inputs), virtual PIs are created, and the original PI is stored in a mapping structure.
 * The auxiliary function `gather_fanin_signals` collects fanin data for each node and matches it to its corresponding
 * nodes in the `virtual_pi_network`.
 *
 * Example: For a level {2, 3, 2, 4, 2}, new nodes are created for each duplicated occurrence (e.g., node 2) and stored
 * in the `old2new_v` node map. This map is then used by `gather_fanin_signals` to correctly establish fanin
 * relationships between newly created nodes.
 *
 * @tparam Ntk Network type.
 * @param ntk Source network used to construct the `virtual_pi_network`.
 * @param ntk_lvls Per-level vectors of original node ranks in the source network used to derive node duplications.
 * @param ntk_lvls_new Per-level vectors of newly created nodes' ranks in the constructed `virtual_pi_network`.
 * @return The constructed planar `virtual_pi_network` containing duplicated nodes with restored fanin and fanout
 * relations.
 */
template <typename Ntk>
virtual_pi_network<Ntk> create_virtual_pi_ntk_from_duplicated_nodes(
    const Ntk& ntk, const std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls,
    std::vector<std::vector<mockturtle::node<virtual_pi_network<Ntk>>>>& ntk_lvls_new)
{
    static_assert(mockturtle::has_create_node_v<virtual_pi_network<Ntk>>, "virtual_pi_network<Ntk> lacks create_node");
    static_assert(mockturtle::has_get_node_v<virtual_pi_network<Ntk>>, "virtual_pi_network<Ntk> lacks get_node");
    static_assert(mockturtle::has_fanout_size_v<virtual_pi_network<Ntk>>, "virtual_pi_network<Ntk> lacks fanout_size");
    static_assert(mockturtle::has_create_po_v<virtual_pi_network<Ntk>>, "virtual_pi_network<Ntk> lacks create_po");
    static_assert(mockturtle::has_create_not_v<virtual_pi_network<Ntk>>, "virtual_pi_network<Ntk> lacks create_not");

    std::unordered_map<mockturtle::node<Ntk>, bool> node_status;
    ntk_lvls_new.resize(ntk_lvls.size());

    auto  init_v     = initialize_copy_network_duplicates(ntk);
    auto& ntk_dest_v = init_v.first;
    auto& old2new_v  = init_v.second;

    for (auto i = ntk_lvls.size(); i-- > 0;)
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
                    const auto new_sig = ntk_dest_v.create_virtual_pi(nd);
                    lvl_new.push_back(ntk_dest_v.get_node(new_sig));
                    old2new_v[nd].push_back(new_sig);
                }
                else
                {
                    const auto& sigs = old2new_v[nd];
                    assert(!sigs.empty());
                    lvl_new.push_back(ntk_dest_v.get_node(sigs.front()));
                    node_status[nd] = true;
                }
            }
            else
            {
                assert(i + 1 < ntk_lvls_new.size() && "Next level does not exist");

                const auto children =
                    gather_fanin_signals(ntk, ntk_dest_v, nd, old2new_v, ntk_lvls_new[i + 1], node_index);

                // Ensure child count matches function arity (including 0-fanin constants)
                assert(children.size() == ntk.fanin_size(nd) &&
                       "Mismatch between gathered children and node fanin count");

                const auto new_sig = ntk_dest_v.create_node(children, ntk.node_function(nd));
                lvl_new.push_back(ntk_dest_v.get_node(new_sig));
                old2new_v[nd].push_back(new_sig);
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
[[nodiscard]] std::vector<hgraph_node<Ntk>> calculate_pairs(const std::vector<mockturtle::node<Ntk>>& nodes) noexcept
{
    std::vector<hgraph_node<Ntk>> pairwise_combinations{};
    pairwise_combinations.reserve(nodes.size() * (nodes.size() - 1));

    if (nodes.size() == 1)
    {
        const hgraph_node<Ntk> pair = {nodes[0], nodes[0],
                                       std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf
        pairwise_combinations.push_back(pair);
        return pairwise_combinations;
    }

    for (auto it1 = nodes.cbegin(); it1 != nodes.cend(); ++it1)
    {
        for (auto it2 = it1 + 1; it2 != nodes.cend(); ++it2)
        {
            std::vector<mockturtle::node<Ntk>> middle_fanins{};
            middle_fanins.reserve(nodes.size() - 2);

            // fill middle_fanins with non-pair members
            for (auto it = nodes.cbegin(); it != nodes.cend(); ++it)
            {
                if (it != it1 && it != it2)
                {
                    middle_fanins.push_back(*it);
                }
            }

            hgraph_node<Ntk> pair1 = {*it1, *it2, std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf
            hgraph_node<Ntk> pair2 = {*it2, *it1, std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf

            // Add middle_fanins to pairs
            pair1.middle_fanins = middle_fanins;
            pair2.middle_fanins = middle_fanins;

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
    [[maybe_unused]] node_duplication_planarization_impl(const Ntk&                                   src,
                                                         const node_duplication_planarization_params& p) :
            ntk(src),
            ps{p}
    {}

    /**
     * A "slice" describes one vertical layer in the H-graph. It is created by adding all possible combinations of a
     * `node_pair` to the H-graph of the level. These combinations are formed by selecting pairs of nodes from the
     * fan-ins of the input node:
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
     */
    void compute_slice_delays(const mockturtle::node<Ntk>& nd)
    {
        // Pis need to be propagated into the next level, since they have to be connected without crossings
        if (ntk.is_pi(nd))
        {
            fis.push_back(nd);
        }

        // Respect the rank order. If two combinations have the same delay and have no seen advantage then the one from
        // the original ranking is used, since it is inserted and not overwritten afterward.
        ntk.foreach_fanin(nd,
                          [&](auto fi)
                          {
                              if (!ntk.is_constant(fi))
                              {
                                  auto n = ntk.get_node(fi);

                                  auto it =
                                      std::lower_bound(fis.begin(), fis.end(), n, [&](auto const& a, auto const& b)
                                                       { return ntk.rank_position(a) < ntk.rank_position(b); });

                                  fis.insert(it, n);
                              }
                          });

        assert(!fis.empty() && "There has to be at least one node in this level");

        // Compute the combinations in one slice
        auto combinations = calculate_pairs<Ntk>(fis);
        assert(!combinations.empty() && "Combinations are empty. There might be a dangling node");

        if (!lvl_pairs.empty())
        {
            std::vector<hgraph_node<Ntk>>* combinations_last = &lvl_pairs.back();

            for (std::size_t cur_idx = 0; cur_idx < combinations.size(); ++cur_idx)
            {
                auto& node_pair_cur = combinations[cur_idx];

                for (std::size_t last_idx = 0; last_idx < combinations_last->size(); ++last_idx)
                {
                    auto& node_pair_last = (*combinations_last)[last_idx];

                    // If there is a connection between the two node pairs the delay is calculated like this
                    if ((node_pair_cur.outer_fanins.first == node_pair_last.outer_fanins.second &&
                         node_pair_last.delay + 1 < node_pair_cur.delay))
                    {
                        node_pair_cur.fanin_it = last_idx;
                        node_pair_cur.delay    = node_pair_last.delay + 1;
                    }
                    // If there is no connection between the two node pairs the delay is calculated like this
                    else if (node_pair_last.delay + 2 < node_pair_cur.delay)
                    {
                        node_pair_cur.fanin_it = last_idx;
                        node_pair_cur.delay    = node_pair_last.delay + 2;
                    }
                    else if (node_pair_last.delay + 2 == node_pair_cur.delay)
                    {
                        // This solves equal path delays, if they are connected in the next layer via a fanout
                        const auto fc0 = fanins(ntk, node_pair_cur.outer_fanins.first);
                        if (node_pair_last.fanin_it < combinations_last->size())
                        {
                            const auto fc1 = fanins(ntk, node_pair_last.outer_fanins.second);

                            for (const auto f0 : fc0.fanin_nodes)
                            {
                                for (const auto f1 : fc1.fanin_nodes)
                                {
                                    if (f0 == f1)
                                    {
                                        node_pair_cur.fanin_it = last_idx;
                                        goto next_combination;
                                    }
                                }
                            }
                        }
                    }
                }
            next_combination:;
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
     * This function inserts a node into a vector only if the vector is empty or the node is not equal to the first
     * element of the vector. If the vector is not empty and the node is equal to the first element, insertion depends
     * on the `saturated_fanout_flag` and the node's `position`: when `position == 0`, a repeated insertion attempt will
     * succeed only if the node was previously skipped (indicated by `saturated_fanout_flag == 1`); otherwise, the flag
     * is set to 1 and the node is skipped for this call. No exception is thrown during this process.
     *
     * @param node The node to be inserted.
     * @param vec The vector to insert the node into.
     * @param saturated_fanout_flag A state flag toggled when consecutive duplicate insertions occur. Set to 1 when a
     * node is skipped and reset to 0 when a node is successfully inserted.
     * @param position The position of the node (0 indicates a terminal node; controls duplicate insertion behavior).
     */
    void insert_if_not_first(const mockturtle::node<Ntk>& node, std::vector<mockturtle::node<Ntk>>& vec,
                             int& saturated_fanout_flag, const int position)
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

        const auto& combinations = lvl_pairs.back();

        // Select the path with the least delay and follow it via fanin relations
        const auto minimum_it =
            std::min_element(combinations.cbegin(), combinations.cend(),
                             [](const hgraph_node<Ntk>& a, const hgraph_node<Ntk>& b) { return a.delay < b.delay; });

        if (minimum_it != combinations.cend())
        {
            const auto& min_combination = *minimum_it;

            // Insert the terminal node
            insert_if_not_first(min_combination.outer_fanins.second, next_level, saturated_fanout_flag, 0);

            // Insert middle_fanins
            for (const auto& node : min_combination.middle_fanins)
            {
                insert_if_not_first(node, next_level, saturated_fanout_flag, 1);
            }

            // Insert the first node
            insert_if_not_first(min_combination.outer_fanins.first, next_level, saturated_fanout_flag, 1);

            // Start with index instead of pointer
            std::size_t level    = lvl_pairs.size() - 1;
            std::size_t fanin_it = minimum_it->fanin_it;

            // Follow chain while index is valid
            while (level > 0 && fanin_it < lvl_pairs[level - 1].size())
            {
                const auto& fanin_combination = lvl_pairs[level - 1][fanin_it];

                // Insert the terminal node
                if (ntk.is_pi(fanin_combination.outer_fanins.second))
                {
                    saturated_fanout_flag = 1;
                }
                insert_if_not_first(fanin_combination.outer_fanins.second, next_level, saturated_fanout_flag, 0);

                // Insert middle_fanins
                for (const auto& node : fanin_combination.middle_fanins)
                {
                    insert_if_not_first(node, next_level, saturated_fanout_flag, 1);
                }

                // Insert the first node
                insert_if_not_first(fanin_combination.outer_fanins.first, next_level, saturated_fanout_flag, 1);

                // Move one level up
                --level;
                fanin_it = fanin_combination.fanin_it;
            }
        }

        return next_level;
    }

    /**
     * Checks if the given vector of nodes contains any non-primary inputs.
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

    [[nodiscard]] virtual_pi_network<Ntk> run()
    {
        // Initialize the POs with foreach_node to retain the rank_view order
        std::vector<mockturtle::node<Ntk>> pos{};
        pos.reserve(ntk.num_pos());
        ntk.foreach_node(
            [this, &pos](const auto n)
            {
                if (ntk.is_po(n))
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

        std::vector<std::vector<mockturtle::node<virtual_pi_network<Ntk>>>> ntk_lvls_new{};

        // create virtual pi network
        auto virtual_ntk = create_virtual_pi_ntk_from_duplicated_nodes(ntk, ntk_lvls, ntk_lvls_new);

        // the ntk_levels were created in reverse order
        std::reverse(ntk_lvls_new.begin(), ntk_lvls_new.end());

        // assign the ranks in the virtual network based on ntk_lvls_new
        virtual_ntk.update_ranks();
        virtual_ntk.set_all_ranks(ntk_lvls_new);

        // restore possibly set signal names
        restore_network_name(ntk, virtual_ntk);
        restore_output_names(ntk, virtual_ntk);

        return virtual_ntk;
    }

  private:
    /**
     * The input network.
     */
    Ntk ntk{};
    /**
     * The currently node_pairs used in the current level.
     */
    std::vector<std::vector<hgraph_node<Ntk>>> lvl_pairs{};
    /**
     * The fanin nodes.
     */
    std::vector<mockturtle::node<Ntk>> fis{};
    /**
     * The network stored as levels.
     */
    std::vector<std::vector<mockturtle::node<Ntk>>> ntk_lvls{};
    /**
     * The stats of the node_duplication class.
     */
    node_duplication_planarization_params ps{};
};

}  // namespace detail

/**
 * Implements a planarization mechanism for networks from the paper \"Fabricatable Interconnect and Molecular QCA
 * Circuits\" by Amitabh Chaudhary, Danny Ziyi Chen, Xiaobo Sharon Hu, Michael T. Niemier, Ramprasad Ravichandran and
 * Kevin Whitton in IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems, Volume 26, 2007.
 *
 * The planarization achieved by this function solves the Node Duplication Crossing Minimization (NDCE) problem by
 * finding the shortest x-y path in the H-graph for every level in the network. An H-graph describes edge relations
 * between two levels in a network, with one level assumed as fixed, starting at the Primary Outputs (POs). By finding
 * the shortest path from the source (x) to the sink (y) in this H-graph, an optimal solution for the NDCE problem for
 * each level is found. The function traverses from the Primary Outputs (POs) towards the Primary Inputs (PIs).
 *
 * @tparam NtkDest Destination network type.
 * @tparam NtkSrc Source network type.
 * @param ntk_src Source network to be utilized for the planarization.
 * @param ps Node duplication parameters used in the computation.
 *
 * @return A view of the planarized virtual_pi_network created in the format of mutable_rank_view.
 */
template <typename NtkSrc>
[[nodiscard]] virtual_pi_network<NtkSrc> node_duplication_planarization(const NtkSrc&                         ntk_src,
                                                                        node_duplication_planarization_params ps = {})
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::has_create_node_v<NtkSrc>, "NtkSrc does not implement the create_node function");
    static_assert(mockturtle::has_rank_position_v<NtkSrc>, "NtkSrc does not implement the rank_position function");

    if (!is_balanced(ntk_src))
    {
        throw std::invalid_argument("Networks have to be balanced for this duplication");
    }

    detail::node_duplication_planarization_impl p{ntk_src, ps};

    auto result = p.run();

    // check for planarity
    mincross_stats  st_min{};
    mincross_params p_min{};
    p_min.optimize = false;

    auto ntk_min = mincross(result, p_min, &st_min);  // counts crossings
    if (st_min.num_crossings != 0)
    {
        throw std::runtime_error("Planarization failed: resulting network is not planar");
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
