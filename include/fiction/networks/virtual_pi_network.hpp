//
// Created by benjamin on 14.06.24.
//

#ifndef FICTION_VIRTUAL_PI_NETWORK_HPP
#define FICTION_VIRTUAL_PI_NETWORK_HPP

#include "fiction/types.hpp"

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <parallel_hashmap/phmap.h>

namespace fiction
{

template <typename Ntk>
struct virtual_storage
{
    /**
     * Shared pointer vector storage for virtual_inputs.
     */
    std::vector<typename Ntk::signal> virtual_inputs;
    /**
     * Map from virtual_pis to real_pis.
     */
    phmap::parallel_flat_hash_map<typename Ntk::signal, typename Ntk::signal> map_virt_to_real_pi;
};

/* Network with additional "virtual" PIs.
 *
 * "Virtual" PIs (Primary Inputs) are used to manage the duplication of PIs in the network. Each "real" PI can have
 * an arbitrary number of "virtual" PIs, which are copies of the original "real" PI.
 * A "virtual" PI can be created by duplicating a "real" PI.
 * To keep track of this relationship, there is a mapping of each "virtual" PI to its corresponding "real" PI in the
 * network.
 */
template <typename Ntk>
class virtual_pi_network : public Ntk
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

  public:
    /**
     * Default constructor for the `virtual_pi_network` class.
     * Initializes `_storage` as a shared pointer.
     */
    explicit virtual_pi_network() : _storage(std::make_shared<virtual_storage<Ntk>>()) {}

    /**
     * Copy constructor for the `virtual_pi_network` class.
     * Given a network `ntk`, constructs a new `virtual_pi_network` as a clone of `ntk`.
     * Initializes `_storage` as a shared pointer.
     *
     * @param ntk The network to clone into this object.
     */
    explicit virtual_pi_network(const Ntk& ntk) : Ntk(ntk.clone()), _storage(std::make_shared<virtual_storage<Ntk>>())
    {}

    /**
     * Variant of the mockturtle::initialize_copy_network. This function helps with creating new networks from old
     * networks. In the original version `old2new` is used to map nodes from the old network to nodes in the new network
     * in a one to one relation.This variant allows old nodes not only to a single node, but to multiple nodes.
     *
     * NtkDest and NtkSrc must be network types (satisfy is_network_type_v).
     * NtkDest and NtkSrc must implement the methods get_constant, get_node, create_pi and foreach_pi (satisfy
     * has_get_constant_v, has_get_node_v, has_create_pi_v and has_foreach_pi_v).
     *
     * A map (old2new) is created where old nodes from source network are mapped to new nodes in destination network.
     * A destination network is created with the same structure as the source network.
     *
     * @tparam NtkDest Type of the destination network.
     * @tparam NtkSrc Type of the source network.
     * @param src The source network.
     *
     * @return A pair of the destination network and a node map from the source to the destination network.
     */
    template <class NtkDest, class NtkSrc>
    std::pair<NtkDest, mockturtle::node_map<std::vector<mockturtle::signal<NtkDest>>, NtkSrc>>
    initialize_copy_network_virtual(NtkSrc const& src)
    {
        static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");
        static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");

        static_assert(mockturtle::has_get_constant_v<NtkDest>, "NtkDest does not implement the get_constant method");
        static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method");
        static_assert(mockturtle::has_is_pi_v<NtkDest>, "NtkDest does not implement the is_pi method");
        static_assert(mockturtle::has_create_not_v<NtkDest>, "NtkDest does not implement the create_not method");
        static_assert(mockturtle::has_get_constant_v<NtkSrc>, "NtkSrc does not implement the get_constant method");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node method");
        static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi method");
        static_assert(mockturtle::has_foreach_po_v<NtkSrc>, "NtkSrc does not implement the foreach_po method");
        static_assert(mockturtle::has_is_complemented_v<NtkSrc>,
                      "NtkSrc does not implement the is_complemented method");
        static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin method");

        mockturtle::node_map<std::vector<mockturtle::signal<NtkDest>>, NtkSrc> old2new(src);
        NtkDest                                                                dest;

        old2new[src.get_constant(false)].push_back(dest.get_constant(false));
        if (src.get_node(src.get_constant(true)) != src.get_node(src.get_constant(false)))
        {
            old2new[src.get_constant(true)].push_back(dest.get_constant(true));
        }
        src.foreach_pi([&](auto const& n) { old2new[n].push_back(dest.create_pi()); });
        return {dest, old2new};
    }

    /**
     * Special constructor to construct a planar virtual_pi_network from any other network type with given ranks.
     *
     * @tparam Ntk Network type.
     * @param ntk Source network to be utilized for the creation of the virtual_pi_network.
     * @param ntk_lvls Levels of nodes in the source network.
     * @param ntk_lvls_new Levels of newly created nodes in the virtual_pi_network.
     *
     * This constructor creates a virtual_pi_network by using the nodes from the original network stored in ntk_lvls.
     * It handles duplicated nodes and stores them with their newly created nodes in ntk_lvls_new.
     * If the duplicated nodes are PIs (Primary Inputs), virtual PIs are created and the original PI is stored in a map.
     * To manage the connections between nodes, this also uses an auxiliary function gather_fanin_signals, that
     * collects the fanin data for a node and matches them in the virtual_pi_network.
     */
    explicit virtual_pi_network(Ntk& ntk, std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls,
                                std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls_new)
    {
        std::unordered_map<mockturtle::node<Ntk>, bool> node_status;
        ntk_lvls_new.resize(ntk_lvls.size());

        auto  init_v     = initialize_copy_network_virtual<virtual_pi_network>(ntk);
        auto& ntk_dest_v = init_v.first;
        auto& old2new_v  = init_v.second;

        /**
         * The function gather_fanin_signals collects the fanin data for node n from the original ntk.
         * For each node n there are the possible fanin candidates old2new_v[fn], which are the original node and all
         * the nodes which are duplicates of this node.
         *
         * lvl[edge_it] gives the current iterator at where the edge can be connected. To get the right signal,
         * all nodes at old2new[n] need to be viewed. Match lvl[edge_it] against all entries in old2new[n],
         * then try lvl[edge_it+1] then try lvl[edge_it+2].
         *
         * @param n Variable to process.
         * @param lvl Level to process.
         * @param edge_it Iterator for edge.
         * @return Vector of fanins in the virtual_pi_network connected to the processed node.
         */
        const auto gather_fanin_signals = [&](const auto& n, const auto& lvl, std::size_t& edge_it)
        {
            std::vector<typename Ntk::signal> children{};
            const auto                        edge_it_int      = edge_it;
            int                               first_fi_edge_it = -1;

            ntk.foreach_fanin(n,
                              [&](const auto& f)
                              {
                                  const auto fn           = ntk.get_node(f);
                                  auto       tgt_signal_v = old2new_v[fn];

                                  assert(edge_it_int < lvl.size());

                                  for (const auto& possible_node : tgt_signal_v)
                                  {
                                      const auto it = ntk.is_maj(n) ? 4 : 3;
                                      if (ntk.fanin_size(n) == children.size())
                                      {
                                          break;
                                      }
                                      for (std::size_t i = 0; i < it; i++)
                                      {
                                          if (edge_it_int + i < lvl.size() && lvl[edge_it_int + i] == possible_node)
                                          {
                                              if (first_fi_edge_it != -1)
                                              {
                                                  if (!ntk.is_maj(n))
                                                  {
                                                      assert(edge_it_int + i == first_fi_edge_it + 1 ||
                                                             edge_it_int + i == first_fi_edge_it - 1);
                                                  }
                                              }
                                              children.emplace_back(possible_node);
                                              first_fi_edge_it = static_cast<int>(edge_it_int + i);
                                              if (edge_it_int + i > edge_it)
                                              {
                                                  edge_it = edge_it_int + i;
                                              }
                                              break;
                                          }
                                      }
                                  }
                              });
            return children;
        };

        std::size_t edge_it = 0;
        for (std::size_t i = ntk_lvls.size(); i-- > 0;)
        {
            edge_it             = 0;
            const auto& lvl     = ntk_lvls[i];
            auto&       lvl_new = ntk_lvls_new[i];
            for (const auto& nd : lvl)
            {
                if (ntk.is_pi(nd))
                {
                    if (node_status[nd])
                    {
                        const auto& new_node = ntk_dest_v.create_virtual_pi(nd);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                    }
                    else
                    {
                        lvl_new.push_back(nd);
                        node_status[nd] = true;
                    }
                }
                else
                {
                    const auto children = gather_fanin_signals(nd, ntk_lvls_new[i + 1], edge_it);

                    if (ntk.is_and(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_and(children[0], children[1]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                    if (ntk.is_or(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_or(children[0], children[1]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                    if (ntk.is_xor(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_xor(children[0], children[1]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                    if (ntk.is_maj(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_maj(children[0], children[1], children[2]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                    if (ntk.is_buf(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_buf(children[0]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                    if (ntk.is_inv(nd))
                    {
                        const auto& new_node = ntk_dest_v.create_not(children[0]);
                        lvl_new.push_back(new_node);
                        old2new_v[nd].push_back(new_node);
                        continue;
                    }
                }
            }
        }

        ntk.foreach_po(
            [&ntk, &ntk_dest_v, &old2new_v](const auto& po)
            {
                const auto tgt_signal_v = old2new_v[ntk.get_node(po)];
                // POs do not get duplicated since the algorithm starts at the POs and duplicates other nodes according
                // to their order
                assert(tgt_signal_v.size() == 1 && "Multiple nodes mapped to PO");
                const auto tgt_signal = tgt_signal_v[0];

                const auto tgt_po = ntk.is_complemented(po) ? ntk_dest_v.create_not(tgt_signal) : tgt_signal;

                ntk_dest_v.create_po(tgt_po);
            });

        *this = ntk_dest_v;
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
        return static_cast<uint32_t>(Ntk::size() - _storage->virtual_inputs.size());
    }

    /**
     * Create a virtual PI, which is a mapping to a real PI.
     *
     * This function creates a virtual PI mapping to a real PI in the network. It adds a PI to the underlying network,
     * but marks it as virtual and stores a mapping to a real PI.
     *
     * @param real_pi The node representing the real PI in the network.
     * @return The signal of the newly created virtual PI.
     */
    signal create_virtual_pi(const signal& real_pi)
    {
        signal s = Ntk::create_pi();
        _storage->virtual_inputs.emplace_back(s);
        _storage->map_virt_to_real_pi.insert({s, real_pi});
        return s;
    }

    /**
     * Check if a given node is a virtual PI. Virtual PIs are created with create_virtual_pi().
     *
     * @param n The node to check.
     * @return True if the node is a virtual PI, false otherwise.
     */
    [[nodiscard]] bool is_virtual_pi(node const& n) const
    {
        return std::find(_storage->virtual_inputs.cbegin(), _storage->virtual_inputs.cend(), n) !=
               _storage->virtual_inputs.cend();
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
     * Check if a given node is a virtual CI in the virtual_pi_network.
     *
     * @param n The node to check.
     * @return True if the node is a virtual CI, false otherwise.
     */
    [[nodiscard]] bool is_virtual_ci(node const& n) const
    {
        return std::find(_storage->virtual_inputs.cbegin(), _storage->virtual_inputs.cend(), n) !=
               _storage->virtual_inputs.cend();
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
        return static_cast<uint32_t>(_storage->virtual_inputs.size());
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
        return static_cast<uint32_t>(_storage->virtual_inputs.size());
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
     * @param v_pi The virtual pi node to retrieve the real pi for.
     * @return The real pi associated with the virtual pi node.
     */
    [[nodiscard]] auto get_real_pi(const node& v_pi) const
    {
        auto it = _storage->map_virt_to_real_pi.find(v_pi);

        assert(it != _storage->map_virt_to_real_pi.end() && "Error: node is not a virtual pi");

        return it->second;
    }

    /**
     * Iterates over the real PIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function.
     * @param fn The function to be applied to each primary input.
     */
    template <typename Fn>
    void foreach_real_pi(Fn&& fn)
    {
        static_cast<Ntk*>(this)->foreach_pi(
            [&](const auto& i)
            {
                if (!is_virtual_pi(i))
                {
                    std::forward<Fn>(fn)(i);
                }
            });
    }

    /**
     * Iterates over the virtual PIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function.
     * @param fn The function to be applied to each primary input.
     */
    template <typename Fn>
    void foreach_virtual_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->virtual_inputs.cbegin(), _storage->virtual_inputs.cend(), fn);
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
            [&](const auto& i)
            {
                if (!is_virtual_ci(i))
                {
                    std::forward<Fn>(fn)(i);
                }
            });
    }

    /**
     * Iterates over the virtual CIs of the circuit and applies a given function.
     *
     * @tparam Fn The type of the function.
     * @param fn The function to be applied to each primary input.
     */
    template <typename Fn>
    void foreach_virtual_ci(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->virtual_inputs.cbegin(), _storage->virtual_inputs.cend(), fn);
    }

    /**
     * Removes virtual input nodes from the virtual_pi_network.
     *
     * This function removes the virtual input nodes from the network by substituting them with their corresponding
     * real input nodes. It then performs a cleanup to remove any dangling PIs..
     * Finally, it clears the virtual_inputs and map_virt_to_real_pi data structures in the _storage object.
     */
    void remove_virtual_input_nodes()
    {
        for (const auto& map_item : _storage->map_virt_to_real_pi)
        {
            Ntk::substitute_node(Ntk::get_node(map_item.first), map_item.second);
        }

        *this = mockturtle::cleanup_dangling(*this, 1);

        // Clear virtual_inputs after using it
        _storage->virtual_inputs.clear();
        _storage->map_virt_to_real_pi.clear();
    }

  protected:
    /**
     * Shared pointer if the virtual PI storage.
     */
    std::shared_ptr<virtual_storage<Ntk>> _storage;
};

}  // namespace fiction

#endif  // FICTION_VIRTUAL_PI_NETWORK_HPP
