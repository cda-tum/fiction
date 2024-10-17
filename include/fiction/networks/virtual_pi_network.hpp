//
// Created by benjamin on 14.06.24.
//

#ifndef FICTION_VIRTUAL_PI_NETWORK_HPP
#define FICTION_VIRTUAL_PI_NETWORK_HPP

#include "fiction/utils/name_utils.hpp"

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

#include <parallel_hashmap/phmap.h>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{
/* Network with additional "virtual" PIs.
 *
 * "Virtual" PIs are used to manage the duplication of PIs in the network.
 * Each "real" PI can have an arbitrary number of "virtual" PIs, which are copies of the original "real" PI.
 * A "virtual" PI can be created by passing a "real" PI to `create_virtual_pi`.
 * To keep track of this relationship, there is a mapping of each "virtual" PI to its corresponding "real" PI in the
 * network.
 *
 * @tparam Ntk Network type.
 */
template <typename Ntk>
class virtual_pi_network : public Ntk
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

    struct virtual_storage
    {
        /**
         * Vector storing virtual_inputs.
         */
        std::vector<node> virtual_inputs{};
        /**
         * Map from virtual_pis to real_pis.
         */
        phmap::parallel_flat_hash_map<node, node> map_virtual_to_real_pi{};
    };

    using v_strg = std::shared_ptr<virtual_storage>;

    /**
     * Default constructor for the `virtual_pi_network` class.
     * Initializes `v_storage` as a shared pointer.
     */
    virtual_pi_network() : Ntk(), v_storage(std::make_shared<virtual_storage>())
    {
        static_assert(mockturtle::has_create_pi_v<Ntk>, "NtkSrc does not implement the create_pi function");
        static_assert(mockturtle::has_clone_v<Ntk>, "NtkSrc does not implement the clone function");
        static_assert(mockturtle::has_size_v<Ntk>, "NtkSrc does not implement the size function");
        static_assert(mockturtle::has_get_node_v<Ntk>, "NtkSrc does not implement the get_node function");
        static_assert(mockturtle::has_create_pi_v<Ntk>, "NtkSrc does not implement the is_pi function");
        static_assert(mockturtle::has_is_pi_v<Ntk>, "NtkSrc does not implement the is_pi function");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "NtkSrc does not implement the is_ci function");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "NtkSrc does not implement the num_pis function");
        static_assert(mockturtle::has_num_cis_v<Ntk>, "NtkSrc does not implement the num_cis function");
    }

    /**
     * Constructor for the `virtual_pi_network` class that takes a network and a shared pointer to a `virtual_storage`
     * object. This is used for cloning.
     *
     * @tparam Ntk Network type.
     * @param ntk Input network.
     * @param s Shared pointer to the `virtual_storage` object to be used by this `virtual_pi_network`.
     */
    explicit virtual_pi_network(const Ntk& ntk, std::shared_ptr<virtual_storage> s) : Ntk(ntk), v_storage(std::move(s))
    {
        static_assert(mockturtle::has_create_pi_v<Ntk>, "NtkSrc does not implement the create_pi function");
        static_assert(mockturtle::has_clone_v<Ntk>, "NtkSrc does not implement the clone function");
        static_assert(mockturtle::has_size_v<Ntk>, "NtkSrc does not implement the size function");
        static_assert(mockturtle::has_get_node_v<Ntk>, "NtkSrc does not implement the get_node function");
        static_assert(mockturtle::has_create_pi_v<Ntk>, "NtkSrc does not implement the is_pi function");
        static_assert(mockturtle::has_is_pi_v<Ntk>, "NtkSrc does not implement the is_pi function");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "NtkSrc does not implement the is_ci function");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "NtkSrc does not implement the num_pis function");
        static_assert(mockturtle::has_num_cis_v<Ntk>, "NtkSrc does not implement the num_cis function");
    }

    /**
     * Clones the virtual_pi_network object.
     */
    [[nodiscard]] virtual_pi_network clone() const
    {
        return virtual_pi_network(
            // Clone the parent network
            Ntk::clone(),
            // Create a new shared_ptr to virtual_storage using a copy of the current _storage
            std::make_shared<virtual_storage>(*v_storage));
    }

    /**
     * Calculate the real size of the virtual_pi_network.
     *
     * The real size of the network is considered the size without virtual PIs.
     *
     * @return The real size of the virtual_pi_network as a uint32_t.
     */
    [[nodiscard]] auto real_size() const
    {
        return static_cast<uint32_t>(Ntk::size() - v_storage->virtual_inputs.size());
    }

    /**
     * Check if a given node is a virtual PI. Virtual PIs are created with create_virtual_pi().
     *
     * @param n The node to check.
     * @return True if the node is a virtual PI, false otherwise.
     */
    [[nodiscard]] bool is_virtual_pi(node const& n) const
    {
        return std::find(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), n) !=
               v_storage->virtual_inputs.cend();
    }

    /**
     * Check if a given node is a real PI. Real PIs are created with create_pi().
     *
     * @param n The node to check.
     * @return True if the node is a real PI, false otherwise.
     */
    [[nodiscard]] bool is_real_pi(node const& n) const
    {
        return (Ntk::is_pi(n) && !is_virtual_pi(n));
    }

    /**
     * Create a virtual PI, which is a mapping to a real PI.
     *
     * This function adds a PI to the parent network, but marks it as virtual and stores a mapping to a real PI.
     *
     * @param real_pi The node representing the real PI in the network.
     * @return The signal of the newly created virtual PI.
     */
    signal create_virtual_pi(const signal& real_pi)
    {
        assert(is_real_pi(Ntk::get_node(real_pi)) && "Error: virtual PIs can only be created as copies of real PIs");
        const signal s = Ntk::create_pi();
        v_storage->virtual_inputs.emplace_back(Ntk::get_node(s));
        v_storage->map_virtual_to_real_pi.insert({Ntk::get_node(s), Ntk::get_node(real_pi)});
        return s;
    }

    /**
     * Check if a given node is a virtual CI in the virtual_pi_network.
     *
     * @param n The node to check.
     * @return True if the node is a virtual CI, false otherwise.
     */
    [[nodiscard]] bool is_virtual_ci(node const& n) const
    {
        return std::find(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), n) !=
               v_storage->virtual_inputs.cend();
    }

    /**
     * Check if a given node is a real CI in the virtual_pi_network.
     *
     * @param n The node to check.
     * @return True if the node is a real CI, false otherwise.
     */
    [[nodiscard]] bool is_real_ci(node const& n) const
    {
        return (Ntk::is_ci(n) && !is_virtual_ci(n));
    }

    /**
     * Get the number of virtual CIs in the virtual_pi_network.
     *
     * @return The number of virtual CIs as a uint32_t.
     */
    [[nodiscard]] auto num_virtual_cis() const
    {
        return static_cast<uint32_t>(v_storage->virtual_inputs.size());
    }

    /**
     * Get the number of real CIs in the virtual_pi_network.
     *
     * @return The number of real CIs as a uint32_t.
     */
    [[nodiscard]] auto num_real_cis() const
    {
        return static_cast<uint32_t>(Ntk::num_cis() - num_virtual_cis());
    }

    /**
     * Get the number of virtual PIs in the virtual_pi_network.
     *
     * @return The number of virtual PIs as a uint32_t.
     */
    [[nodiscard]] auto num_virtual_pis() const
    {
        return static_cast<uint32_t>(v_storage->virtual_inputs.size());
    }

    /**
     * Get the number of real PIs in the virtual_pi_network.
     *
     * @return The number of real PIs as a uint32_t.
     */
    [[nodiscard]] auto num_real_pis() const
    {
        return static_cast<uint32_t>(Ntk::num_pis() - num_virtual_pis());
    }

    /**
     * Get the real PI associated with a virtual PI node.
     *
     * @param v_pi The virtual pi node to retrieve the real pi for.
     * @return The real pi associated with the virtual pi node.
     */
    [[nodiscard]] auto get_real_pi(const node& v_pi) const
    {
        const auto it = v_storage->map_virtual_to_real_pi.find(v_pi);

        assert(it != v_storage->map_virtual_to_real_pi.end() && "Error: node is not a virtual pi");

        return it->second;
    }

    /**
     * Iterates over the real PIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_real_pi(Fn&& fn) const
    {
        static_cast<const Ntk*>(this)->foreach_pi(
            [this, &fn](const auto& i)
            {
                if (!is_virtual_pi(i))
                {
                    fn(i);
                }
            });
    }

    /**
     * Iterates over the virtual PIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_virtual_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), fn);
    }

    /**
     * Iterates over the virtual CIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_real_ci(Fn&& fn)
    {
        static_cast<Ntk*>(this)->foreach_ci(
            [this, &fn](const auto& i)
            {
                if (!is_virtual_ci(i))
                {
                    fn(i);
                }
            });
    }

    /**
     * Iterates over the virtual CIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_virtual_ci(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), fn);
    }

  private:
    /**
     * Shared pointer of the virtual PI storage.
     */
    v_strg v_storage;
};

namespace detail
{

template <typename Ntk>
class delete_virtual_pis_impl
{
  public:
    explicit delete_virtual_pis_impl(const Ntk& ntk_src) : ntk{ntk_src}, ntk_topo{ntk_src} {}

    auto run() -> decltype(std::declval<Ntk>().clone())
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
                if constexpr (mockturtle::has_is_nary_and_v<Ntk> && mockturtle::has_create_nary_and_v<Ntk>)
                {
                    if (ntk.is_nary_and(g))
                    {
                        old2new[g] = ntk_dest.create_nary_and(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_or_v<Ntk> && mockturtle::has_create_nary_or_v<Ntk>)
                {
                    if (ntk.is_nary_or(g))
                    {
                        old2new[g] = ntk_dest.create_nary_or(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_xor_v<Ntk> && mockturtle::has_create_nary_xor_v<Ntk>)
                {
                    if (ntk.is_nary_xor(g))
                    {
                        old2new[g] = ntk_dest.create_nary_xor(children);
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
        fiction::restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;
    Ntk        ntk;
    TopoNtkSrc ntk_topo;

    [[nodiscard]] std::pair<Ntk, mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>>
    initialize_copy_virtual_pi_network(Ntk& src)
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
auto delete_virtual_pis(const Ntk& ntk) -> decltype(std::declval<Ntk>().clone())
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
    static_assert(fiction::has_get_real_pi_v<Ntk>, "Ntk does not implement the get_real_pi function");
    static_assert(fiction::has_is_virtual_pi_v<Ntk>, "Ntk does not implement the is_vitual_pi function");
    static_assert(fiction::has_num_real_pis_v<Ntk>, "Ntk does not implement the has_num_real_pis function");
    static_assert(fiction::has_num_virtual_pis_v<Ntk>, "Ntk does not implement the num_virtual_pis function");
    static_assert(fiction::has_foreach_real_pi_v<Ntk>, "Ntk does not implement the foreach_real_pi function");

    assert(ntk.is_combinational() && "Network has to be combinational");

    if (ntk.num_virtual_pis() == 0)
    {
        return ntk;
    }

    detail::delete_virtual_pis_impl<Ntk> p{ntk};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_VIRTUAL_PI_NETWORK_HPP
