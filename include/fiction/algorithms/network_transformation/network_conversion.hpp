//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_NETWORK_CONVERSION_HPP
#define FICTION_NETWORK_CONVERSION_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/network_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/rank_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <cassert>
#include <type_traits>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

template <typename NtkDest, typename NtkSrc, bool is_same_ntk_type = std::is_same_v<NtkDest, NtkSrc>>
class convert_network_impl
{};

template <typename NtkDest, typename NtkSrc>
class convert_network_impl<NtkDest, NtkSrc, true>
{
  public:
    explicit convert_network_impl(const NtkSrc& ntk_src) : ntk{ntk_src} {}

    NtkDest run()
    {
        return ntk;
    }

  private:
    NtkDest ntk;
};

template <typename NtkDest, typename NtkSrc>
class convert_network_impl<NtkDest, NtkSrc, false>
{
  public:
    explicit convert_network_impl(const NtkSrc& ntk_src) :
            ntk{ntk_src},
            init{mockturtle::initialize_copy_network<NtkDest>(ntk)}
    {}

    NtkDest run()
    {
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<mockturtle::signal<NtkDest>> children{};

            ntk.foreach_fanin(n,
                              [this, &ntk_dest, &old2new, &children](const auto& f)
                              {
                                  const auto fn         = ntk.get_node(f);
                                  const auto tgt_signal = old2new[fn];

                                  children.emplace_back(ntk.is_complemented(f) ? ntk_dest.create_not(tgt_signal) :
                                                                                 tgt_signal);
                              });

            return children;
        };

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] network conversion: |{0}|"};
#endif

        ntk.foreach_gate(
            [&, this](const auto& g, [[maybe_unused]] auto i)
            {
                const auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                if constexpr (mockturtle::has_is_and_v<TopoNtkSrc> && mockturtle::has_create_and_v<NtkDest>)
                {
                    if (ntk.is_and(g))
                    {
                        old2new[g] = ntk_dest.create_and(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_or_v<TopoNtkSrc> && mockturtle::has_create_or_v<NtkDest>)
                {
                    if (ntk.is_or(g))
                    {
                        old2new[g] = ntk_dest.create_or(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_xor_v<TopoNtkSrc> && mockturtle::has_create_xor_v<NtkDest>)
                {
                    if (ntk.is_xor(g))
                    {
                        old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_maj_v<TopoNtkSrc> && mockturtle::has_create_maj_v<NtkDest>)
                {
                    if (ntk.is_maj(g))
                    {
                        old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_and_v<TopoNtkSrc> && mockturtle::has_create_nary_and_v<NtkDest>)
                {
                    if (ntk.is_nary_and(g))
                    {
                        old2new[g] = ntk_dest.create_nary_and(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_or_v<TopoNtkSrc> && mockturtle::has_create_nary_or_v<NtkDest>)
                {
                    if (ntk.is_nary_or(g))
                    {
                        old2new[g] = ntk_dest.create_nary_or(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_xor_v<TopoNtkSrc> && mockturtle::has_create_nary_xor_v<NtkDest>)
                {
                    if (ntk.is_nary_xor(g))
                    {
                        old2new[g] = ntk_dest.create_nary_xor(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_not_v<TopoNtkSrc> && mockturtle::has_create_not_v<NtkDest>)
                {
                    if (ntk.is_not(g))
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_buf_v<NtkDest>)
                {
                    if (ntk.is_buf(g))
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<NtkDest>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;
                }

                return true;
            });

        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                const auto tgt_signal = old2new[ntk.get_node(po)];
                const auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;

                ntk_dest.create_po(tgt_po);
            });

        // TODO this is the code like you actually want it
        //        if constexpr (is_gate_level_layout_v<TopoNtkSrc> && mockturtle::has_sort_rank_v<NtkDest>)
        //        {
        //            static_assert(mockturtle::has_depth_v<NtkDest>, "NtkDest does not implement the depth function");
        //
        //            // need to flip the old2new map, unfortunately
        //            mockturtle::node_map<mockturtle::node<TopoNtkSrc>, NtkDest> new2old{ntk_dest};
        //            ntk.foreach_node([&old2new, &new2old, &ntk_dest](const auto& n)
        //                             { new2old[ntk_dest.get_node(old2new[n])] = n; });
        //
        //            for (auto l = 0; l < ntk_dest.depth(); ++l)
        //            {
        //                ntk_dest.sort_rank(l,
        //                                   [this, &new2old](const auto& n1, const auto& n2)
        //                                   {
        //                                       const auto t1 = ntk.ground(ntk.get_tile(new2old[n1]));
        //                                       const auto t2 = ntk.ground(ntk.get_tile(new2old[n2]));
        //
        //                                       return t1 < t2;
        //                                   });
        //            }
        //        }
        // if TopoNtkSrc is a gate-level layout and NtkDest supports crossings, we can merge them here
        //    if constexpr (is_gate_level_layout_v<TopoNtkSrc> && mockturtle::has_merge_into_crossing_v<NtkDest>)
        //    {
        //        ntk.foreach_crossing(
        //            [this, &ntk_dest, &old2new](const auto& cx)
        //            {
        //                const auto cxn = ntk.get_node(cx);
        //                const auto gn  = ntk.get_node(ntk.below(ntk.get_tile(cxn)));
        //                assert(cxn != gn);
        //
        //                const auto buf1 = ntk_dest.get_node(old2new[cxn]);
        //                const auto buf2 = ntk_dest.get_node(old2new[gn]);
        //                assert(buf1 != buf2);
        //
        //                [[maybe_unused]] const auto cross_node = ntk_dest.merge_into_crossing(buf1, buf2);
        //
        //                if constexpr (mockturtle::has_swap_v<NtkDest>)  // if rank information is available
        //                {
        //                    // insert the crossing node at the same position as the original buffer node
        //                    ntk_dest.swap(buf1, cross_node);
        //                }
        //            });
        //    }

        //        if constexpr (is_gate_level_layout_v<TopoNtkSrc> && mockturtle::has_sort_rank_v<NtkDest>)
        //        {
        //            static_assert(mockturtle::has_depth_v<NtkDest>, "NtkDest does not implement the depth function");

        // restore signal names if applicable
        fiction::restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

    mockturtle::rank_view<NtkDest> preserve_ranks() const
    {
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        mockturtle::rank_view ranks{ntk_dest};

        // need to flip the old2new map, unfortunately
        mockturtle::node_map<mockturtle::node<TopoNtkSrc>, NtkDest> new2old{ranks};
        ntk.foreach_node([&old2new, &new2old, &ntk_dest](const auto& n)
                         { new2old[ntk_dest.get_node(old2new[n])] = n; });

        // restore the ranks like they were in the layout
        for (auto l = 0; l < ranks.depth(); ++l)
        {
            ranks.sort_rank(l,
                            [this, &new2old](const auto& n1, const auto& n2)
                            {
                                const auto t1 = ntk.ground(ntk.get_tile(new2old[n1]));
                                const auto t2 = ntk.ground(ntk.get_tile(new2old[n2]));

                                return t1 < t2;
                            });
        }

        // if TopoNtkSrc is a gate-level layout and NtkDest supports crossings, we can merge them here
        if constexpr (is_gate_level_layout_v<TopoNtkSrc> && mockturtle::has_merge_into_crossing_v<NtkDest>)
        {
            ntk.foreach_crossing(
                [this, &ranks, &old2new](const auto& cx)
                {
                    const auto cxn = ntk.get_node(cx);
                    const auto gn  = ntk.get_node(ntk.below(ntk.get_tile(cxn)));
                    assert(cxn != gn);

                    const auto buf1 = ranks.get_node(old2new[cxn]);
                    const auto buf2 = ranks.get_node(old2new[gn]);
                    assert(buf1 != buf2);

                    [[maybe_unused]] const auto cross_node = ranks.merge_into_crossing(buf1, buf2);

                    // insert the crossing node at the same position as the original buffer node
                    ranks.swap(buf1, cross_node);
                });
        }

        return ranks;
    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<NtkSrc>;
    TopoNtkSrc ntk;

    std::pair<NtkDest, mockturtle::node_map<mockturtle::signal<NtkDest>, TopoNtkSrc>> init;
};

}  // namespace detail

/**
 * Converts a logic network into an equivalent one of another type. Thereby, this function is very similar to
 * mockturtle::cleanup_dangling. However, it supports real buffer nodes used for fanouts and path balancing in fiction.
 *
 * NOTE: In contrast to mockturtle::cleanup_dangling, this function returns ntk if NtkDest and NtkSrc are of the same
 * type.
 *
 * @tparam NtkDest Type of the returned logic network.
 * @tparam NtkSrc Type of the input logic network.
 * @param ntk The input logic network.
 * @return A logic network of type NtkDest that is logically equivalent to ntk.
 */
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
    // TODO handle ci/ro/etc...

    assert(ntk.is_combinational() && "Network has to be combinational");

    detail::convert_network_impl<NtkDest, NtkSrc> p{ntk};

    auto result = p.run();

    return result;
}

/**
 * Converts a gate-level layout into an equivalent logic network of some type. The above convert_network function is
 * also capable of doing so. However, this implementation additionally preserves crossings and rank orders.
 *
 * @tparam NtkDest Type of the returned logic network.
 * @tparam NtkSrc Type of the input gate-level layout.
 * @param ntk The input gate-level Layout.
 * @return A logic network of type NtkDest wrapped inside a rank_view that is logically equivalent to ntk.
 */
template <typename NtkDest, typename NtkSrc>
mockturtle::rank_view<NtkDest> convert_layout_to_network(const NtkSrc& ntk)
{
    static_assert(fiction::is_gate_level_layout_v<NtkSrc>, "NtkSrc is not a network type");
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
    // TODO handle ci/ro/etc...

    assert(ntk.is_combinational() && "Network has to be combinational");

    detail::convert_network_impl<NtkDest, NtkSrc> p{ntk};

    p.run();
    const auto result = p.preserve_ranks();

    return result;
}

}  // namespace fiction

#endif  // FICTION_NETWORK_CONVERSION_HPP
