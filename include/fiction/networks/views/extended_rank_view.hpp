//
// Created by benjamin on 6/18/24.
//

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/rank_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#ifndef FICTION_VIRTUAL_RANK_VIEW_HPP
#define FICTION_VIRTUAL_RANK_VIEW_HPP

// this rank view can assign specific ranks to nodes in one level
// to check validity of the rank view you can use check_rank_validity()

// add the trait that is added in the virtual rank view
template <class Ntk, bool has_virtual_rank_interface =
                         mockturtle::has_rank_position_v<Ntk> && mockturtle::has_at_rank_position_v<Ntk> &&
                         mockturtle::has_swap_v<Ntk> && mockturtle::has_width_v<Ntk> &&
                         mockturtle::has_foreach_node_in_rank_v<Ntk> && mockturtle::has_foreach_gate_in_rank_v<Ntk>>
class extended_rank_view
{};

/*ToDo: Reverse true and false and implement the traits for the new implemented functions
 * remove_virtual_input_nodes has to be modified so that it is deleting also the nodes out of the ranks
 * -> this impacts the behaviour of foreach_node etc.
 * include a new function, which allows to just assign a rank to a node
 * include a function which tests for validity of ranks. meaning that there is a increasing order of ranks in each level
 * -> this is good to verify if the ranks are still valid after deleting the virtual nodes
 * */
template <class Ntk>
class extended_rank_view<Ntk, true> : public mockturtle::rank_view<Ntk, true>
{
  public:
    extended_rank_view(Ntk const& ntk) : mockturtle::rank_view<Ntk>(ntk) {}
};

template <class Ntk>
class extended_rank_view<Ntk, false> : public mockturtle::rank_view<Ntk>
{
  public:
    using node = typename Ntk::node;
    extended_rank_view(Ntk const& ntk) : mockturtle::rank_view<Ntk>(ntk) {}

    void modify_rank(node const& n, uint32_t new_rank) {

    }
};

//deduction guide
template<class T>
extended_rank_view( T const& ) -> extended_rank_view<T>;

#endif  // FICTION_VIRTUAL_RANK_VIEW_HPP
