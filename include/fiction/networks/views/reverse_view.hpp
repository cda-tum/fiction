//
// Created by benjamin on 11.04.23.
//

#ifndef FICTION_REVERSE_VIEW_HPP
#define FICTION_REVERSE_VIEW_HPP
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/immutable_view.hpp>
#include <mockturtle/views/topo_view.hpp>
#include "mockturtle/networks/aig.hpp"
#include <mockturtle/views/fanout_view.hpp>
#include "mockturtle/views/names_view.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"


#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

namespace fiction{

/*template <class TopoSorted, bool sorted = mockturtle::is_topologically_sorted_v<TopoSorted>>
class input_sort_view
{};*/

template<typename Ntk>
class reverse_view : public Ntk{
  public:
    using storage = typename Ntk::storage;
    using node = typename Ntk::node;
    using signal = typename Ntk::signal;

    explicit reverse_view(Ntk const& ntk) : Ntk(ntk), num_p{ntk.num_pis()}
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");
        static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");
        static_assert(mockturtle::has_incr_trav_id_v<Ntk>, "Ntk does not implement the incr_trav_id function");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited function");
        static_assert(mockturtle::has_trav_id_v<Ntk>, "Ntk does not implement the trav_id function");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited function");
        static_assert(mockturtle::has_foreach_fanout_v<Ntk>, "Ntk does not implement the has_foreach_fanout function");

        copy_and_sort(ntk);
    }

    /*! \brief Reimplementation of `node_to_index`. */
    uint32_t node_to_index( node const& n ) const
    {
        return std::distance( std::begin( reverse_order ), std::find( std::begin( reverse_order ), std::end( reverse_order ), n ) );
    }

    /*! \brief Reimplementation of `index_to_node`. */
    node index_to_node( uint32_t index ) const
    {
        return reverse_order.at( index );
    }

    /*! \brief Reimplementation of `foreach_node`. */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(reverse_order.begin(), reverse_order.end(), fn);
    }

    template<typename Fn>
    void foreach_gate( Fn&& fn ) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(reverse_order.begin(), reverse_order.end() - offset, fn);
    }

    [[nodiscard]] bool isFo_one_inv_flag() const
    {
        return fo_one_inv_flag;
    }

    [[nodiscard]] bool isFo_two_inv_flag() const
    {
        return fo_two_inv_flag;
    }

    void copy_and_sort(Ntk const& ntk)
    {
        ntk.foreach_node([&](const auto& n){reverse_order.push_back(n);});
        std::reverse(reverse_order.begin(), reverse_order.end());
    }

    const std::vector<node>& getTopo_order_input_sort() const
    {
        return reverse_order;
    }

  protected:
    std::vector<node> reverse_order;
    uint32_t num_p;
    uint32_t num_c = 0u;
    bool fo_one_inv_flag = false;
    bool fo_two_inv_flag = false;
    std::vector<node> output_node;
    std::vector<node> wait_fo;
    std::vector<node> wait;
};

}
#endif  // FICTION_REVERSE_VIEW_HPP
