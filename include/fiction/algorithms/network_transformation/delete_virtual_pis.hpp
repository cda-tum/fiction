//
// Created by benjamin on 14.06.24.
//

#ifndef FICTION_DELETE_VIRTUAL_PIS_HPP
#define FICTION_DELETE_VIRTUAL_PIS_HPP

#include "fiction/utils/name_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <cassert>
#include <cstdlib>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

template <typename Ntk>
class delete_virtual_pis_impl
{
  public:
    explicit delete_virtual_pis_impl(const Ntk& ntk_src) : ntk{ntk_src}, ntk_topo{ntk_src} {}

    [[nodiscard]] auto run() -> decltype(std::declval<Ntk>().clone())
    {
        auto  init         = initialize_copy_virtual_pi_network(ntk);
        auto& ntk_dest_ref = init.first;
        // Cloning resolves runtime issues (in debug mode) with views due to add_event, but might return a different
        // network type. Deleting virtual_pis should only be used for equivalence checking.
        auto  ntk_dest = ntk_dest_ref.clone();
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename Ntk::signal> children{};

            ntk.foreach_fanin(n,
                              [this, &ntk_dest, &old2new, &children](const auto& f)
                              {
                                  auto fn = ntk.get_node(f);

                                  if (ntk.is_virtual_pi(fn))
                                  {
                                      fn = ntk.get_real_pi(fn);
                                  }
                                  auto tgt_signal = old2new[fn];

                                  children.emplace_back(ntk.is_complemented(f) ? ntk_dest.create_not(tgt_signal) :
                                                                                 tgt_signal);
                              });

            return children;
        };

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] network conversion: |{0}|"};
#endif

        ntk_topo.foreach_gate(
            [&, this](const auto& g, [[maybe_unused]] auto i)
            {
                auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                if constexpr (mockturtle::has_is_and_v<Ntk> && mockturtle::has_create_and_v<Ntk>)
                {
                    if (ntk.is_and(g))
                    {
                        old2new[g] = ntk_dest.create_and(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_or_v<Ntk> && mockturtle::has_create_or_v<Ntk>)
                {
                    if (ntk.is_or(g))
                    {
                        old2new[g] = ntk_dest.create_or(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_xor_v<Ntk> && mockturtle::has_create_xor_v<Ntk>)
                {
                    if (ntk.is_xor(g))
                    {
                        old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_maj_v<Ntk> && mockturtle::has_create_maj_v<Ntk>)
                {
                    if (ntk.is_maj(g))
                    {
                        old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<Ntk> && mockturtle::has_create_buf_v<Ntk>)
                {
                    if (ntk.is_buf(g))
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_node_function_v<Ntk> && mockturtle::has_create_node_v<Ntk>)
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

        // restore signal names if applicable
        restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    /**
     * Type alias for the topological view of the network.
     */
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;
    /**
     * Network without virtual PIs.
     */
    Ntk ntk;
    /**
     * Topological view of the input network.
     */
    TopoNtkSrc ntk_topo;

    [[nodiscard]] std::pair<Ntk, mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>>
    initialize_copy_virtual_pi_network(const Ntk& src)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant method");
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(fiction::has_foreach_real_pi_v<Ntk>, "Ntk does not implement the foreach_pi method");

        mockturtle::node_map<mockturtle::signal<Ntk>, Ntk> old2new(src);
        Ntk                                                dest;

        old2new[src.get_constant(false)] = dest.get_constant(false);
        if (src.get_node(src.get_constant(true)) != src.get_node(src.get_constant(false)))
        {
            old2new[src.get_constant(true)] = dest.get_constant(true);
        }
        src.foreach_real_pi([&](auto const& n) { old2new[n] = dest.create_pi(); });

        return {dest, old2new};
    }
};
}  // namespace detail

/**
 * Deletes virtual primary inputs from a network. This can mainly be used for equivalence checking.
 * If the network does not have any virtual PIs stored, the network is returned.
 *
 * @tparam Ntk The type of network.
 * @param ntk The input network.
 * @return The resulting network after virtual primary inputs are deleted.
 */
template <typename Ntk>
[[nodiscard]] auto delete_virtual_pis(const Ntk& ntk) noexcept -> decltype(std::declval<Ntk>().clone())
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_clone_v<Ntk>, "Ntk does not implement the clone constructor");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function");
    static_assert(mockturtle::has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented function");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
    static_assert(mockturtle::has_foreach_gate_v<Ntk>, "Ntk does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");
    static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");
    static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");
    static_assert(mockturtle::has_create_po_v<Ntk>, "Ntk does not implement the create_po function");
    static_assert(mockturtle::has_create_not_v<Ntk>, "Ntk does not implement the create_not function");
    static_assert(fiction::is_virtual_network_type_v<Ntk>, "Ntk does not implement the get_real_pi function");

    assert(ntk.is_combinational() && "Network has to be combinational");

    // to match the return type the network has to be cloned.
    if (ntk.num_virtual_pis() == 0)
    {
        return ntk.clone();
    }

    detail::delete_virtual_pis_impl<Ntk> p{ntk};

    const auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_DELETE_VIRTUAL_PIS_HPP
