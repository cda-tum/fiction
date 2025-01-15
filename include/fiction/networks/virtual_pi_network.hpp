//
// Created by benjamin on 14.06.24.
//

#ifndef FICTION_VIRTUAL_PI_NETWORK_HPP
#define FICTION_VIRTUAL_PI_NETWORK_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

#include <parallel_hashmap/phmap.h>

namespace fiction
{
/* Network with additional "virtual" PIs.
 *
 * "Virtual" PIs are used to manage the duplication of PIs in the network.
 * Each "real" PI can have an arbitrary number of "virtual" PIs, which are copies of the original "real" PI.
 * A "virtual" PI can be created by passing a "real" PI to `create_virtual_pi()`.
 * To keep track of this relationship, there is a mapping of each "virtual" PI to its corresponding "real" PI in the
 * network.
 *
 * @tparam Ntk Network type.
 */
template <typename Ntk>
class virtual_pi_network : public Ntk
{
  public:
    static constexpr bool is_virtual_network_type = true;

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
    explicit virtual_pi_network() : Ntk(), v_storage{std::make_shared<virtual_storage>()}
    {
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");
        static_assert(mockturtle::has_clone_v<Ntk>, "Ntk does not implement the clone function");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function");
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the is_pi function");
        static_assert(mockturtle::has_is_pi_v<Ntk>, "Ntk does not implement the is_pi function");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "Ntk does not implement the is_ci function");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis function");
        static_assert(mockturtle::has_num_cis_v<Ntk>, "Ntk does not implement the num_cis function");
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
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");
        static_assert(mockturtle::has_clone_v<Ntk>, "Ntk does not implement the clone function");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size function");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function");
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the is_pi function");
        static_assert(mockturtle::has_is_pi_v<Ntk>, "Ntk does not implement the is_pi function");
        static_assert(mockturtle::has_is_ci_v<Ntk>, "Ntk does not implement the is_ci function");
        static_assert(mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis function");
        static_assert(mockturtle::has_num_cis_v<Ntk>, "Ntk does not implement the num_cis function");
    }

    /**
     * Clones the `virtual_pi_network` object.
     */
    [[nodiscard]] virtual_pi_network clone() const
    {
        return virtual_pi_network(
            // Clone the parent network
            Ntk::clone(),
            // Create a new shared pointer to `virtual_storage` using a copy of the current `_storage`
            std::make_shared<virtual_storage>(*v_storage));
    }

    /**
     * Calculate the real size of the virtual_pi_network`.
     *
     * The real size of the network is considered the size without virtual PIs.
     *
     * @return The real size of the `virtual_pi_network` as a `uint32_t`.
     */
    [[nodiscard]] uint32_t real_size() const
    {
        return Ntk::size() - num_virtual_pis();
    }

    /**
     * Check if a given node is a virtual PI. Virtual PIs are created with `create_virtual_pi()`.
     *
     * @param n The node to check.
     * @return `true` if the node is a virtual PI, `false` otherwise.
     */
    [[nodiscard]] bool is_virtual_pi(node const& n) const
    {
        return std::find(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), n) !=
               v_storage->virtual_inputs.cend();
    }

    /**
     * Check if a given node is a real PI. Real PIs are created with `create_pi()`.
     *
     * @param n The node to check.
     * @return `true` if the node is a real PI, `false` otherwise.
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
    [[nodiscard]] signal create_virtual_pi(const signal& real_pi)
    {
        assert(is_real_pi(Ntk::get_node(real_pi)) && "Error: virtual PIs can only be created as copies of real PIs");

        const signal s = Ntk::create_pi();
        v_storage->virtual_inputs.emplace_back(Ntk::get_node(s));
        v_storage->map_virtual_to_real_pi.insert({Ntk::get_node(s), Ntk::get_node(real_pi)});

        return s;
    }

    /**
     * Check if a given node is a virtual CI in the `virtual_pi_network`.
     *
     * @param n The node to check.
     * @return `true` if the node is a virtual CI, `false` otherwise.
     */
    [[nodiscard]] bool is_virtual_ci(node const& n) const
    {
        return std::find(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(), n) !=
               v_storage->virtual_inputs.cend();
    }

    /**
     * Check if a given node is a real CI in the `virtual_pi_network`.
     *
     * @param n The node to check.
     * @return `true` if the node is a real CI, `false` otherwise.
     */
    [[nodiscard]] bool is_real_ci(node const& n) const
    {
        return (Ntk::is_ci(n) && !is_virtual_ci(n));
    }

    /**
     * Get the number of virtual CIs in the `virtual_pi_network`.
     *
     * @return The number of virtual CIs as a `uint32_t`.
     */
    [[nodiscard]] uint32_t num_virtual_cis() const
    {
        return static_cast<uint32_t>(v_storage->virtual_inputs.size());
    }

    /**
     * Get the number of real CIs in the `virtual_pi_network`.
     *
     * @return The number of real CIs as a `uint32_t`.
     */
    [[nodiscard]] uint32_t num_real_cis() const
    {
        return static_cast<uint32_t>(Ntk::num_cis() - num_virtual_cis());
    }

    /**
     * Get the number of virtual PIs in the `virtual_pi_network`.
     *
     * @return The number of virtual PIs as a `uint32_t`.
     */
    [[nodiscard]] uint32_t num_virtual_pis() const
    {
        return static_cast<uint32_t>(v_storage->virtual_inputs.size());
    }

    /**
     * Get the number of real PIs in the `virtual_pi_network`.
     *
     * @return The number of real PIs as a `uint32_t`.
     */
    [[nodiscard]] uint32_t num_real_pis() const
    {
        return static_cast<uint32_t>(Ntk::num_pis() - num_virtual_pis());
    }

    /**
     * Get the real PI associated with a virtual PI node.
     *
     * @param v_pi The virtual pi node to retrieve the real PI for.
     * @return The real pi associated with the virtual PI node.
     */
    [[nodiscard]] node get_real_pi(const node& v_pi) const
    {
        const auto it = v_storage->map_virtual_to_real_pi.find(v_pi);

        assert(it != v_storage->map_virtual_to_real_pi.cend() && "Error: node is not a virtual pi");

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
            [this, fn = std::forward<Fn>(fn)](const auto& i)
            {
                if (!is_virtual_pi(i))
                {
                    std::invoke(fn, i);
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
        mockturtle::detail::foreach_element(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(),
                                            std::forward<Fn>(fn));
    }

    /**
     * Iterates over the virtual CIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_real_ci(Fn&& fn) const
    {
        static_cast<const Ntk*>(this)->foreach_ci(
            [this, fn = std::forward<Fn>(fn)](const auto& i)
            {
                if (!is_virtual_ci(i))
                {
                    std::invoke(fn, i);
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
        mockturtle::detail::foreach_element(v_storage->virtual_inputs.cbegin(), v_storage->virtual_inputs.cend(),
                                            std::forward<Fn>(fn));
    }

  protected:
    /**
     * Shared pointer of the virtual PI storage.
     */
    v_strg v_storage;
};

}  // namespace fiction

#endif  // FICTION_VIRTUAL_PI_NETWORK_HPP
