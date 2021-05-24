//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_NETWORK_CONVERSION_HPP
#define FICTION_NETWORK_CONVERSION_HPP

#include "traits.hpp"
#include "types.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

namespace fiction
{

template <typename NtkDest, typename NtkSrc>
NtkDest convert(NtkSrc ntk)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");

    static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node method");
    static_assert(mockturtle::has_is_complemented_v<NtkSrc>, "NtkSrc does not implement the is_complemented method");
    static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi method");
    static_assert(mockturtle::has_foreach_gate_v<NtkSrc>, "NtkSrc does not implement the foreach_gate method");
    static_assert(mockturtle::has_foreach_po_v<NtkSrc>, "NtkSrc does not implement the foreach_po method");
    static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin method");

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

    using TopoSrcNtk = typename mockturtle::topo_view<NtkSrc>;
    TopoSrcNtk topo{ntk};

    auto  init_capture   = mockturtle::initialize_copy_network<NtkDest>(topo);
    auto& target         = init_capture.first;
    auto& node_to_signal = init_capture.second;

    const auto gather_fanin_signals = [&topo, &target, &node_to_signal](const auto n)
    {
        std::vector<typename NtkDest::signal> children{};

        topo.foreach_fanin(n,
                           [&topo, &target, &node_to_signal, &children](const auto f)
                           {
                               const auto tgt_signal = node_to_signal[topo.get_node(f)];
                               children.emplace_back(topo.is_complemented(f) ? target.create_not(tgt_signal) :
                                                                               tgt_signal);
                           });

        return children;
    };

    topo.foreach_gate(
        [&topo, &target, &gather_fanin_signals, &node_to_signal](const auto g)
        {
            auto children = gather_fanin_signals(g);

            if (topo.is_and(g))
            {
                node_to_signal[g] = target.create_and(children[0], children[1]);
            }
            else if (topo.is_or(g))
            {
                node_to_signal[g] = target.create_or(children[0], children[1]);
            }
            else if (topo.is_xor(g))
            {
                node_to_signal[g] = target.create_xor(children[0], children[1]);
            }
            else if (topo.is_maj(g))
            {
                node_to_signal[g] = target.create_maj(children[0], children[1], children[2]);
            }
            else
            {
                if constexpr (mockturtle::has_is_nary_and_v<TopoSrcNtk> && mockturtle::has_create_nary_and_v<NtkDest>)
                {
                    if (topo.is_nary_and(g))
                    {
                        node_to_signal[g] = target.create_nary_and(children);
                    }
                }
                else if constexpr (mockturtle::has_is_nary_or_v<TopoSrcNtk> &&
                                   mockturtle::has_create_nary_or_v<NtkDest>)
                {
                    if (topo.is_nary_or(g))
                    {
                        node_to_signal[g] = target.create_nary_or(children);
                    }
                }
                else if constexpr (mockturtle::has_is_nary_xor_v<TopoSrcNtk> &&
                                   mockturtle::has_create_nary_xor_v<NtkDest>)
                {
                    if (topo.is_nary_xor(g))
                    {
                        node_to_signal[g] = target.create_nary_xor(children);
                    }
                }
                else if constexpr (fiction::has_is_wire_v<TopoSrcNtk> && mockturtle::has_create_buf_v<NtkDest>)
                {
                    if (topo.is_wire(g))
                    {
                        node_to_signal[g] = target.create_buf(children[0]);
                    }
                }
                else if constexpr (mockturtle::has_node_function_v<TopoSrcNtk> &&
                                   mockturtle::has_create_node_v<NtkDest>)
                {
                    node_to_signal[g] = target.create_node(children, topo.node_function(g));
                }
            }
        });

    topo.foreach_po(
        [&topo, &target, &node_to_signal](const auto po)
        {
            const auto tgt_signal = node_to_signal[topo.get_node(po)];
            const auto tgt_po     = topo.is_complemented(po) ? target.create_not(tgt_signal) : tgt_signal;

            if constexpr (mockturtle::has_get_name_v<TopoSrcNtk>)
            {
                target.create_po(tgt_po, topo.get_name(po));
            }
            else
            {
                target.create_po(tgt_po);
            }
        });

    return target;
}

}  // namespace fiction

#endif  // FICTION_NETWORK_CONVERSION_HPP
