//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_NETWORK_CONVERSION_HPP
#define FICTION_NETWORK_CONVERSION_HPP

#include "name_restoration.hpp"
#include "traits.hpp"
#include "types.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

namespace fiction
{

namespace detail
{

template <typename NtkDest, typename NtkSrc>
class convert_network_impl
{
  public:
    explicit convert_network_impl(const NtkSrc& ntk_src) : ntk_topo{ntk_src} {}

    NtkDest run()
    {
        auto  init     = mockturtle::initialize_copy_network<NtkDest>(ntk_topo);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename NtkDest::signal> children{};

            ntk_topo.foreach_fanin(n,
                                   [this, &ntk_dest, &old2new, &children](const auto& f)
                                   {
                                       const auto tgt_signal = old2new[ntk_topo.get_node(f)];
                                       children.emplace_back(
                                           ntk_topo.is_complemented(f) ? ntk_dest.create_not(tgt_signal) : tgt_signal);
                                   });

            return children;
        };

        ntk_topo.foreach_gate(
            [this, &ntk_dest, &gather_fanin_signals, &old2new](const auto& g)
            {
                auto children = gather_fanin_signals(g);

                if (ntk_topo.is_and(g))
                {
                    old2new[g] = ntk_dest.create_and(children[0], children[1]);
                }
                else if (ntk_topo.is_or(g))
                {
                    old2new[g] = ntk_dest.create_or(children[0], children[1]);
                }
                else if (ntk_topo.is_xor(g))
                {
                    old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                }
                else if (ntk_topo.is_maj(g))
                {
                    old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                }
                else
                {
                    if constexpr (mockturtle::has_is_nary_and_v<TopoNtkSrc> &&
                                  mockturtle::has_create_nary_and_v<NtkDest>)
                    {
                        if (ntk_topo.is_nary_and(g))
                        {
                            old2new[g] = ntk_dest.create_nary_and(children);
                        }
                    }
                    else if constexpr (mockturtle::has_is_nary_or_v<TopoNtkSrc> &&
                                       mockturtle::has_create_nary_or_v<NtkDest>)
                    {
                        if (ntk_topo.is_nary_or(g))
                        {
                            old2new[g] = ntk_dest.create_nary_or(children);
                        }
                    }
                    else if constexpr (mockturtle::has_is_nary_xor_v<TopoNtkSrc> &&
                                       mockturtle::has_create_nary_xor_v<NtkDest>)
                    {
                        if (ntk_topo.is_nary_xor(g))
                        {
                            old2new[g] = ntk_dest.create_nary_xor(children);
                        }
                    }
                    else if constexpr (fiction::has_is_wire_v<TopoNtkSrc> && mockturtle::has_create_buf_v<NtkDest>)
                    {
                        if (ntk_topo.is_wire(g))
                        {
                            old2new[g] = ntk_dest.create_buf(children[0]);
                        }
                    }
                    else if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> &&
                                       mockturtle::has_create_node_v<NtkDest>)
                    {
                        old2new[g] = ntk_dest.create_node(children, ntk_topo.node_function(g));
                    }
                }
            });

        ntk_topo.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                const auto tgt_signal = old2new[ntk_topo.get_node(po)];
                const auto tgt_po     = ntk_topo.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;

                ntk_dest.create_po(tgt_po);
            });

        // restore signal names if applicable
        fiction::restore_names(ntk_topo, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<NtkSrc>;
    TopoNtkSrc ntk_topo;
};
}  // namespace detail

// TODO overload for NtkDest = NtkSrc

template <typename NtkDest, typename NtkSrc>
NtkDest convert_network(const NtkSrc& ntk)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");
    static_assert(mockturtle::has_is_complemented_v<NtkSrc>, "NtkSrc does not implement the is_complemented function");
    static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi function");
    static_assert(mockturtle::has_foreach_gate_v<NtkSrc>, "NtkSrc does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_po_v<NtkSrc>, "NtkSrc does not implement the foreach_po function");
    static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");

    static_assert(mockturtle::has_get_constant_v<NtkSrc>, "NtkSrc does not implement the get_constant function");
    static_assert(mockturtle::has_get_constant_v<NtkDest>, "NtkDest does not implement the get_constant function");

    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi function");
    static_assert(mockturtle::has_create_po_v<NtkDest>, "NtkDest does not implement the create_po function");
    static_assert(mockturtle::has_create_not_v<NtkDest>, "NtkDest does not implement the create_not function");
    static_assert(mockturtle::has_create_and_v<NtkDest>, "NtkDest does not implement the create_and function");
    static_assert(mockturtle::has_create_or_v<NtkDest>, "NtkDest does not implement the create_or function");
    static_assert(mockturtle::has_create_xor_v<NtkDest>, "NtkDest does not implement the create_xor function");
    static_assert(mockturtle::has_create_maj_v<NtkDest>, "NtkDest does not implement the create_maj function");
    // TODO finalize asserts
    // TODO handle ci/ro/etc...

    assert(ntk.is_combinational() && "Network has to be combinational");

    detail::convert_network_impl<NtkDest, NtkSrc> p{ntk};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_NETWORK_CONVERSION_HPP
