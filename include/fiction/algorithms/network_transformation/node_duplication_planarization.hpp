//
// Created by benjamin on 6/11/24.
//

#ifndef FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
#define FICTION_NODE_DUPLICATION_PLANARIZATION_HPP

#include "fiction/traits.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"

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
class node_duplication_planarization_impl
{
  public:
    node_duplication_planarization_impl(const Ntk& src, const node_duplication_params p) :
            ntk(mockturtle::rank_view<Ntk>(src))
    { }

    mockturtle::rank_view<Ntk> run()
    {
        // Create random node ranks for POs: maybe here a heuristic or some clustering algorithm can assist
        // Iterate from POs to PIs and watch the current rank plus the fanin nodes at one time
        // A between these two ranks: compute the Graph H and compute the shortest path
        // B this might be simplified by just collecting all fanins also from the node with rank current_rank - 1
        // the order can then has to be applied before going to the next iteration step
        std::cout << "width: " <<ntk.width() << std::endl;
        std::cout << "depth: " <<ntk.depth() << std::endl;
        return ntk;
    }

  private:
    mockturtle::rank_view<Ntk> ntk;

};

}  // namespace detail

/**
 * ToDo: Description
 */
template <typename NtkDest, typename NtkSrc>
mockturtle::rank_view<NtkDest> node_duplication_planarization(const NtkSrc& ntk_src, node_duplication_params ps = {})
{
    /*static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
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
    static_assert(mockturtle::has_foreach_po_v<NtkDest>, "NtkDest does not implement the foreach_po function");*/

    auto t_ntk = convert_network<NtkDest>(ntk_src);

    if (!is_fanout_substituted(t_ntk)) {
        t_ntk =  fanout_substitution<NtkDest>(t_ntk);   // Apply fanout substitution if needed
    }

    detail::node_duplication_planarization_impl<NtkDest> p{t_ntk, ps};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_PLANARIZATION_HPP
