//
// Created by benjamin on 6/11/24.
//

#ifndef FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
#define FICTION_NODE_DUPLICATION_PLANARIZATION_HPP

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/views/rank_view.hpp>

#include <algorithm>
#include <cmath>
#include <deque>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

/*template <typename Ntk>
class RankViewNtk : public Ntk {
  public:
    using Ntk::Ntk; // Inherit constructors

    RankViewNtk(const Ntk& ntk): Ntk(mockturtle::rank_view(ntk)) {} // Convert from Ntk
};*/

/**
 * Parameters for the fanout substitution algorithm.
 */
struct node_duplication_params
{
    /**
     * Breadth-first vs. depth-first fanout-tree substitution strategies.
     */
    enum substitution_strategy
    {
        /**
         * Breadth-first substitution. Creates balanced fanout trees.
         */
        BREADTH,
        /**
         * Depth-first substitution. Creates fanout trees with one deep branch.
         */
        DEPTH
    };

    /**
     * Substitution strategy of high-degree fanout networks (depth-first vs. breadth-first).
     */
    substitution_strategy strategy = BREADTH;
    /**
     * Maximum output degree of each fan-out node.
     */
    uint32_t degree = 2ul;
    /**
     * Maximum number of outputs any gate is allowed to have before substitution applies.
     */
    uint32_t threshold = 1ul;
};

namespace detail
{

template <typename Ntk>
struct node_pair
{
    std::pair<mockturtle::node<Ntk>, mockturtle::node<Ntk>> pair;
    std::vector<mockturtle::node<Ntk>>                      middle_nodes;
    std::shared_ptr<node_pair<Ntk>>                         fanin_pair;
    uint64_t                                                delay;
    // constructor
    node_pair(mockturtle::node<Ntk> node1, mockturtle::node<Ntk> node2, uint64_t delayValue) :
            pair(node1, node2),
            delay(delayValue)
    {}
};

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
    node_duplication_planarization_impl(const Ntk& src, const node_duplication_params p) :
            ntk(mockturtle::rank_view<Ntk>(src))
    {}

    void compute_slice_delays(mockturtle::node<Ntk> nd, const bool border_pis)
    {
        // Pis need to be propagated into the next level, since they have to be connected without crossings
        if (ntk.is_pi(nd) && border_pis)
        {
            cur_fis.push_back(nd);
        }
        ntk.foreach_fanin(nd, [this](auto fi) { cur_fis.push_back(fi); });
        // Compute the combinations in one slice
        auto combinations = calculate_pairs<Ntk>(cur_fis);
        if (combinations.empty())
        {
            throw std::runtime_error("Combinations are empty. There might be a dangling node");
        }
        std::vector<node_pair<Ntk>> combinations_last;
        if (!cur_lvl_pairs.empty())
        {
            combinations_last = cur_lvl_pairs.back();
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

        for (const auto& node_pair : combinations)
        {
            std::cout << "Fanin Pair: (" << node_pair.pair.first << ", " << node_pair.pair.second
                      << "), Delay: " << node_pair.delay << std::endl;
        }

        cur_lvl_pairs.push_back(combinations);
    }

    void insert_if_unique(mockturtle::node<Ntk> node, std::vector<mockturtle::node<Ntk>>& vec)
    {
        if (vec.empty() || vec.front() != node)
        {
            vec.insert(vec.begin(), node);
        }
    }

    void compute_node_order_next_level(std::vector<mockturtle::node<Ntk>>& next_level)
    {
        const auto& combinations = cur_lvl_pairs.back();
        // select the path with the least delay and follow it via fanin relations
        auto minimum_it =
            std::min_element(combinations.begin(), combinations.end(),
                             [](const node_pair<Ntk>& a, const node_pair<Ntk>& b) { return a.delay < b.delay; });
        if (minimum_it != combinations.end())
        {
            const auto& min_combination = *minimum_it;
            std::cout << "Minimum delay: " << min_combination.delay << std::endl;

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

    void check_final_level(std::vector<mockturtle::node<Ntk>>& v_next_level, bool& f_final_level )
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

    mockturtle::rank_view<Ntk> run()
    {
        const bool border_pis = true;
        // ToDo: Determine the PO order
        /*static std::mt19937                     generator(std::random_device{}());
        std::uniform_int_distribution<uint32_t> distribution(0, ntk.rank_width(r) - 1);*/
        std::vector<mockturtle::node<Ntk>> v_level;  // save the nodes of the next level
        // Process the first level
        ntk.foreach_po(
            [this, &v_level](auto po)
            {
                // Recalculate the levels to start from the pos
                std::cout << ntk.level(po) << std::endl;
                ntk.set_level(po, ntk.depth());
                cur_fis.clear();
                compute_slice_delays(po, border_pis);
                v_level.push_back(po);
            });
        int level = ntk.depth();
        std::cout << "push lvl: " << level << std::endl;
        ntk_lvls.push_back(v_level);
        v_level.clear();

        // ToDo: try to push PIs to the beginning or end of he vector, since they have to be propagated with buffers
        // until the last levels this should only happen if border_pis == true
        compute_node_order_next_level(v_level);

        bool f_final_level = true;
        check_final_level(v_level, f_final_level);

        // Process all other levels
        while (!v_level.empty() && !f_final_level)
        {
            level--;
            std::cout << "push lvl: " << level << std::endl;
            // Push the level to the network
            ntk_lvls.push_back(v_level);
            cur_lvl_pairs.clear();
            // We need to create a new vector to store the nodes of the next level
            for (const auto& cur_node : v_level)
            {
                cur_fis.clear();
                // There is one slice in the H-Graph per node in the level
                compute_slice_delays(cur_node, border_pis);
            }
            // Clear before starting computations on the enxt level
            v_level.clear();
            // Compute the next level
            compute_node_order_next_level(v_level);
            // Check if we are at the final level
            f_final_level = true;
            check_final_level(v_level, f_final_level);
        }

        if (f_final_level)
        {
            std::cout << "push lvl: " << level - 1 << std::endl;
            ntk_lvls.push_back(v_level);
        }

        std::cout << "width: " << ntk.width() << std::endl;
        std::cout << "depth: " << ntk.depth() << std::endl;
        return ntk;
    }

  private:
    mockturtle::rank_view<Ntk>                      ntk;
    std::vector<std::vector<node_pair<Ntk>>>        cur_lvl_pairs;
    std::vector<mockturtle::node<Ntk>>              cur_fis;
    std::vector<std::vector<mockturtle::node<Ntk>>> ntk_lvls;
};

}  // namespace detail

/**
 * ToDo: Description
 * This implementation utilizes an H-Graph to capture all edge relations between two layers of a graph.
 * The Node Duplication Crossing Minimization (NDCE) problem is addressed by finding the shortest x-y path on the
 * H-Graph. Note that this minimization is valid only under a specific order at the primary output (PO) level. The
 * algorithm traverses from primary outputs (POs) to primary inputs (PIs), constructing the H-Graph and computing the
 * shortest x-y paths at each level of the graph.
 */
template <typename NtkDest, typename NtkSrc>
mockturtle::rank_view<NtkDest> node_duplication_planarization(const NtkSrc& ntk_src, node_duplication_params ps = {})
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

    // check for input degree
    /*if (has_high_degree_fanin_nodes(ntk_src, 2))
    {
        throw high_degree_fanin_exception();
    }*/

    auto t_ntk = convert_network<NtkDest>(ntk_src);

    /*if (!is_fanout_substituted(t_ntk)) {
        t_ntk =  fanout_substitution<NtkDest>(t_ntk);   // Apply fanout substitution if needed
    }*/

    detail::node_duplication_planarization_impl<NtkDest> p{t_ntk, ps};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
