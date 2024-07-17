//
// Created by benjamin on 6/11/24.
//

#ifndef FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
#define FICTION_NODE_DUPLICATION_PLANARIZATION_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/network_transformation/network_balancing.hpp"
#include "fiction/algorithms/properties/check_planarity.hpp"
#include "fiction/networks/views/extended_rank_view.hpp"
#include "fiction/networks/virtual_pi_network.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/views/rank_view.hpp>

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

/**
 * Parameters for the fanout substitution algorithm.
 */
struct node_duplication_params
{
    /*
     * The output order determines the starting layer for this algorithm. If this option is turned off, the output order
     * remains the same as in the provided network. If it is turned on, the outputs are ordered randomly.
     * */
    bool random_output_order = false;
};

namespace detail
{

/**
 * @brief A structure representing a pair of nodes in an H-graph
 *
 * @tparam Ntk Network type for the nodes in the pair
 *
 * The nodes stored in this struct describe the fanin-edges of a node in an H-graph.
 * A node pair object holds two nodes, which are saved in the member 'pair'.
 * These two outer nodes are connected through zero or more 'middle_nodes'.
 * The fanin order starts with the first node in 'pair', then proceeds through the 'middle_nodes', and ends with the
 * second node in 'pair'. The order of 'middle_nodes' is arbitrary as they cannot be further connected to any other
 * nodes. For the planarization, only the nodes inside the 'pair' are relevant.
 *
 *
 * @note The edges connecting to the nodes in 'pair' effectively block the 'middle_nodes'.
 */
template <typename Ntk>
struct node_pair
{
    /**
     * Defines the beginning and end of the fanin-edged node
     */
    std::pair<mockturtle::node<Ntk>, mockturtle::node<Ntk>> pair;
    /**
     * Contains the nodes between the fanin-edges node; cannot be connected to any other node
     */
    std::vector<mockturtle::node<Ntk>> middle_nodes;

    /**
     * Shared pointer to another instance of node_pair detailing fanin-edge alignment
     */
    std::shared_ptr<node_pair<Ntk>> fanin_pair;

    /**
     * Specifies the delay value for the node
     */
    uint64_t delay;

    /**
     * @brief node_pair constructor
     *
     * @param node1 The first node of the fanin-edged node
     * @param node2 The second node of the fanin-edged node
     * @param delayValue The delay value for the node
     */
    node_pair(mockturtle::node<Ntk> node1, mockturtle::node<Ntk> node2, uint64_t delayValue) :
            pair(node1, node2),
            delay(delayValue)
    {}
};

/**
 * @brief Calculates pairs of nodes from a given vector of nodes.
 *
 * This function takes a vector of nodes and returns a vector of node pairs. Each node pair consists of two nodes from
 * the input vector and an optional vector of middle nodes. The delay of each node pair is initialized to infinity.
 *
 * @tparam Ntk The network type.
 * @param nodes The vector of nodes.
 * @return The vector of node pairs.
 */
template <typename Ntk>
std::vector<node_pair<Ntk>> calculate_pairs(const std::vector<mockturtle::node<Ntk>>& nodes)
{
    std::vector<node_pair<Ntk>>        pairwise_combinations;
    std::vector<mockturtle::node<Ntk>> middle_nodes;

    if (nodes.size() == 1)
    {
        node_pair<Ntk> pair = {nodes[0], nodes[0], std::numeric_limits<uint64_t>::max()};  // Initialize delay to inf
        pairwise_combinations.push_back(pair);
        return pairwise_combinations;
    }

    for (auto it1 = nodes.begin(); it1 != nodes.end(); ++it1)
    {
        for (auto it2 = it1 + 1; it2 != nodes.end(); ++it2)
        {
            // Clear middle_nodes before the next pair
            middle_nodes.clear();

            // fill middle_nodes with non-pair members
            for (auto it = nodes.begin(); it != nodes.end(); ++it)
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
    node_duplication_planarization_impl(const Ntk& src, const node_duplication_params p) : ntk(src), ps{p} {}

    /**
     * @brief Computes the delay in a given slice (each possible order of node_pairs) of an H-graph
     *
     * @param nd Node in the H-graph
     * @param border_pis A boolean indicating whether the input PIs (Primary Inputs) should be propagated to the next
     * level
     *
     * This function iterates over the fanins of the given node and computes the delay for all possible orders
     * of these nodes that form a node_pair. The delay computation depends on the node's connections and position
     * within the graph. If there is a connection between two node_pairs, the delay is incremented by 1. If not,
     * the delay is incremented by 2. Default delay for the first node is 1. If a node_pair doesn't have a connection
     * and its delay (when increased by two) is less than the existing delay, then this node_pair's delay is updated.
     *
     * The processed node_pairs are pushed back to the 'lvl_pairs' data member for subsequent delay calculations.
     *
     * @throws std::runtime_error if no combinations (possible node_pairs) are found, which might suggest a dangling
     * node
     */
    void compute_slice_delays(mockturtle::node<Ntk> nd, const bool border_pis)
    {
        // Pis need to be propagated into the next level, since they have to be connected without crossings
        if (ntk.is_pi(nd) && border_pis)
        {
            fis.push_back(nd);
        }
        ntk.foreach_fanin(nd, [this](auto fi) { fis.push_back(fi); });
        // Compute the combinations in one slice
        auto combinations = calculate_pairs<Ntk>(fis);
        if (combinations.empty())
        {
            throw std::runtime_error("Combinations are empty. There might be a dangling node");
        }
        std::vector<node_pair<Ntk>> combinations_last;
        if (!lvl_pairs.empty())
        {
            combinations_last = lvl_pairs.back();
            for (auto& node_pair_cur : combinations)
            {
                for (auto& node_pair_last : combinations_last)
                {
                    // If there is a connection between the two node pairs the delay is calculated like this
                    if ((node_pair_cur.pair.first == node_pair_last.pair.second &&
                         node_pair_last.delay + 1 < node_pair_cur.delay))
                    {
                        node_pair_cur.fanin_pair = std::make_shared<node_pair<Ntk>>(node_pair_last);
                        node_pair_cur.delay      = node_pair_last.delay + 1;
                    }
                    // If there is no connection between the two node pairs the delay is calculated like this
                    else if (node_pair_last.delay + 2 < node_pair_cur.delay)
                    {
                        node_pair_cur.fanin_pair = std::make_shared<node_pair<Ntk>>(node_pair_last);
                        node_pair_cur.delay      = node_pair_last.delay + 2;
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
     * @brief Inserts a node into a vector if it is unique
     *
     * This function inserts a node into a vector only if the vector is empty or the node is not equal to the first
     * element of the vector. If the vector is not empty and the node is equal to the first element, it does nothing.
     *
     * @tparam Ntk The network type
     * @param node The node to be inserted
     * @param vec The vector to insert the node into
     */
    void insert_if_unique(mockturtle::node<Ntk> node, std::vector<mockturtle::node<Ntk>>& vec)
    {
        if (vec.empty() || vec.front() != node)
        {
            vec.insert(vec.begin(), node);
        }
    }

    /**
     * @brief Computes the order of nodes in the next level based on delay
     *
     * @param next_level The vector to store the nodes in the next level
     *
     * This function computes the order of nodes in the next level based on their delay in the H-graph of the level. It
     * selects the path with the least delay from the current level pairs and follows it via fanin relations. The nodes
     * are inserted into the next level vector in the order they are encountered.
     */
    void compute_node_order_next_level(std::vector<mockturtle::node<Ntk>>& next_level)
    {
        const auto& combinations = lvl_pairs.back();
        // select the path with the least delay and follow it via fanin relations
        auto minimum_it =
            std::min_element(combinations.begin(), combinations.end(),
                             [](const node_pair<Ntk>& a, const node_pair<Ntk>& b) { return a.delay < b.delay; });
        if (minimum_it != combinations.end())
        {
            const auto& min_combination = *minimum_it;

            // Insert the terminal node
            insert_if_unique(min_combination.pair.second, next_level);

            // insert middle_nodes
            for (const auto& node : min_combination.middle_nodes)
            {
                insert_if_unique(node, next_level);
            }

            // Insert the first node
            insert_if_unique(min_combination.pair.first, next_level);

            std::shared_ptr<node_pair<Ntk>> fanin_combination = minimum_it->fanin_pair;

            while (fanin_combination)
            {
                // Insert the terminal node
                insert_if_unique(fanin_combination->pair.second, next_level);

                // Insert middle_nodes
                for (const auto& node : fanin_combination->middle_nodes)
                {
                    insert_if_unique(node, next_level);
                }

                // insert the first node
                insert_if_unique(fanin_combination->pair.first, next_level);

                fanin_combination = fanin_combination->fanin_pair;
            }
        }
    }

    /**
     * @brief Checks if the given vector of nodes contains any non-primary inputs.
     *
     * This function iterates through each node in the vector and checks if it is a primary input.
     * If a non-primary input is found, the `f_final_level` parameter is set to false and the loop is exited.
     *
     * @param v_next_level The vector of nodes to be checked
     * @param f_final_level A reference to a boolean indicating if the vector contains only primary inputs
     */
    void check_final_level(std::vector<mockturtle::node<Ntk>>& v_next_level, bool& f_final_level)
    {
        for (const auto& nd : v_next_level)
        {
            if (!ntk.is_pi(nd))
            {
                f_final_level = false;
                break;
            }
        }
    }

    virtual_pi_network run(std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls_new)
    {
        const bool border_pis = true;

        std::vector<typename Ntk::node> pos;
        ntk.foreach_po([&pos](auto po) { pos.push_back(po); });
        // Randomize the PO order
        if (ps.random_output_order)
        {
            // Generate a random engine
            std::random_device rd;
            std::mt19937       g(rd());
            // Shuffle the pos vector
            std::shuffle(pos.begin(), pos.end(), g);
        }

        // save the nodes of the next level
        std::vector<mockturtle::node<Ntk>> v_level;
        // Process the first level
        for (const auto& po : pos)
        {
            // Recalculate the levels to start from the pos
            fis.clear();
            compute_slice_delays(po, border_pis);
            v_level.push_back(po);
        }

        ntk_lvls.push_back(v_level);
        v_level.clear();

        // Try to push PIs to the beginning or end of the vector, since they have to be propagated with buffers
        // until the last levels this should only happen if border_pis == true
        compute_node_order_next_level(v_level);

        bool f_final_level = true;
        check_final_level(v_level, f_final_level);

        // Process all other levels
        while (!v_level.empty() && !f_final_level)
        {
            // Push the level to the network
            ntk_lvls.push_back(v_level);
            lvl_pairs.clear();
            // Store the nodes of the next level
            for (const auto& cur_node : v_level)
            {
                fis.clear();
                // There is one slice in the H-Graph for each node in the level
                compute_slice_delays(cur_node, border_pis);
            }
            // Clear before starting computations on the next level
            v_level.clear();
            // Compute the next level
            compute_node_order_next_level(v_level);
            // Check if we are at the final level
            f_final_level = true;
            check_final_level(v_level, f_final_level);
        }
        // Push the final level (PIs)
        if (f_final_level)
        {
            ntk_lvls.push_back(v_level);
        }

        // create virtual pi network
        auto new_ntk = virtual_pi_network(ntk, ntk_lvls, ntk_lvls_new);

        return new_ntk;  // mockturtle::rank_view<Ntk>(ntk);
    }

  private:
    /*
     * The input network.
     */
    Ntk ntk;
    /*
     * The currently node_pairs used in the current level.
     */
    std::vector<std::vector<node_pair<Ntk>>> lvl_pairs;
    /*
     * The fanin nodes.
     */
    std::vector<mockturtle::node<Ntk>> fis;
    /*
     * The network stored as levels.
     */
    std::vector<std::vector<mockturtle::node<Ntk>>> ntk_lvls;
    /*
     * The stats of the node_duplication class.
     */
    node_duplication_params ps;
};

}  // namespace detail

/**
 * @brief Implements a planarization mechanism for Mockturtle networks using a H-Graph strategy and node duplication.
 *
 * @tparam NtkDest Destination network type
 * @tparam NtkSrc Source network type
 * @param ntk_src Source network to be utilized for the planarization
 * @param ps Node duplication parameters used in the computation
 *
 * The planarization achieved by this function solves the Node Duplication Crossing Minimization (NDCE) problem by
 * finding the shortest x-y path in the H-graph for every level in the network. The function constructs an H-graph that
 * captures edge relations between two levels within the graph and computes the shortest x-y paths on the H-graph. The
 * graph is traversed from Primary Outputs (POs) towards Primary Inputs (PIs).
 *
 * @return A view of the planarized virtual_pi_network created in the format of extended_rank_view
 *
 * @throws std::runtime_error if input network not balanced, if no node combinations are found or if
 * the created network is non-planar
 *
 */
template <typename NtkDest, typename NtkSrc>
extended_rank_view<virtual_pi_network> node_duplication_planarization(const NtkSrc&           ntk_src,
                                                                      node_duplication_params ps = {})
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_is_constant_v<NtkDest>, "NtkSrc does not implement the is_constant function");
    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi function");
    static_assert(mockturtle::has_create_not_v<NtkDest>, "NtkDest does not implement the create_not function");
    static_assert(mockturtle::has_create_po_v<NtkDest>, "NtkDest does not implement the create_po function");
    static_assert(mockturtle::has_create_buf_v<NtkDest>, "NtkDest does not implement the create_buf function");
    static_assert(mockturtle::has_clone_node_v<NtkDest>, "NtkDest does not implement the clone_node function");
    static_assert(mockturtle::has_fanout_size_v<NtkDest>, "NtkDest does not implement the fanout_size function");
    static_assert(mockturtle::has_foreach_gate_v<NtkDest>, "NtkDest does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_fanin_v<NtkDest>, "NtkDest does not implement the foreach_fanin function");
    static_assert(mockturtle::has_foreach_po_v<NtkDest>, "NtkDest does not implement the foreach_po function");

    if (!is_balanced(ntk_src))
    {
        throw std::runtime_error("Combinations are empty. There might be a dangling node");
    }

    detail::node_duplication_planarization_impl<NtkDest> p{ntk_src, ps};

    std::vector<std::vector<mockturtle::node<NtkDest>>> ntk_lvls_new;

    auto result_ntk = p.run(ntk_lvls_new);

    std::reverse(ntk_lvls_new.begin(), ntk_lvls_new.end());

    auto result = extended_rank_view(result_ntk, ntk_lvls_new);

    if (!check_planarity(result))
    {
        throw std::runtime_error("Error: Network should be planar");
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
