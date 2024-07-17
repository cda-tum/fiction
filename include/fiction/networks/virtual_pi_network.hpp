//
// Created by benjamin on 6/14/24.
//

#ifndef FICTION_NODE_DUPLICATION_VIEW_HPP
#define FICTION_NODE_DUPLICATION_VIEW_HPP

#include "fiction/algorithms/network_transformation/network_conversion.hpp"

#include <mockturtle/traits.hpp>
// #include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/detail/foreach.hpp>

#include <cstdint>

namespace fiction
{

/*!\brief Manager view for virtual PIs
 *
 * Virtual PIs are mapping of a "virtual" PI onto a "real" PI in the network.
 * Virtual PIs handle PI-duplications.
 */
class virtual_pi_network : public technology_network
{
  public:
    using node   = typename technology_network::node;
    using signal = typename technology_network::signal;

  public:
    /**
     * A default constructor for the `virtual_pi_network` class.
     *
     * This constructor initializes `virtual_inputs` with a shared pointer to an empty
     * std::vector of uint32_t.
     */
    explicit virtual_pi_network() : virtual_inputs(std::make_shared<std::vector<uint32_t>>()) {}

    /**
     * A parameterized constructor for the `virtual_pi_network` class.
     *
     * This constructor creates a `virtual_pi_network` using the `other` `technology_network`
     * and initializes `virtual_inputs` with a shared pointer to an empty std::vector of uint32_t.
     *
     * @param other A `technology_network` object to create an instance of `virtual_pi_network`.
     */
    explicit virtual_pi_network(const technology_network& other) :
            virtual_inputs(std::make_shared<std::vector<uint32_t>>())
    {
        _storage = std::make_shared<mockturtle::klut_storage>(*(other._storage));  // create deep copy of storage
    }

    /**
     * @brief A variant of the mockturtle::initialize_copy_network. It maps old nodes not only to a single node, but to
     * multiple or duplicated nodes.
     *
     * @tparam NtkDest Type of the destination network
     * @tparam NtkSrc Type of the source network
     * @param src The source network
     *
     * @return A pair of the destination network and a node map from the source to the destination network.
     *
     * @pre NtkDest and NtkSrc must be network types (satisfy is_network_type_v).
     * @pre NtkDest and NtkSrc must implement the methods get_constant, get_node, create_pi and foreach_pi (satisfy
     * has_get_constant_v, has_get_node_v, has_create_pi_v and has_foreach_pi_v).
     *
     * @post A map (old2new) is created where old nodes from source network are mapped to new nodes in destination
     * network.
     * @post A destination network is created with the same structure as the source network.
     *
     */
    template <class NtkDest, class NtkSrc>
    std::pair<NtkDest, mockturtle::node_map<std::vector<mockturtle::signal<NtkDest>>, NtkSrc>>
    initialize_copy_network_v(NtkSrc const& src)
    {
        static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");
        static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");

        static_assert(mockturtle::has_get_constant_v<NtkDest>, "NtkDest does not implement the get_constant method");
        static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method");
        static_assert(mockturtle::has_get_constant_v<NtkSrc>, "NtkSrc does not implement the get_constant method");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node method");
        static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi method");

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
     * @brief Special constructor to construct a planar virtual_pi_network from any other network type with given ranks.
     *
     * @tparam Ntk Network type
     * @param ntk Source network to be utilized for the creation of the virtual_pi_network
     * @param ntk_lvls Levels of nodes in the source network
     * @param ntk_lvls_new Levels of newly created nodes in the virtual_pi_network
     *
     * This constructor creates a virtual_pi_network by using the nodes from the original network stored in ntk_lvls.
     * It handles duplicated nodes and stores them with their newly created nodes in ntk_lvls_new.
     * If the duplicated nodes are PIs (Primary Inputs), virtual PIs are created and the original PI is stored in a map.
     * To manage the connections between nodes, this also uses an auxiliary function gather_fanin_signals, that
     * collects the fanin data for a node and matches them in the virtual_pi_network.
     *
     */
    template <typename Ntk>
    explicit virtual_pi_network(Ntk& ntk, std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls,
                                std::vector<std::vector<mockturtle::node<Ntk>>>& ntk_lvls_new)
    {
        std::unordered_map<mockturtle::node<Ntk>, bool> node_status;
        ntk_lvls_new.resize(ntk_lvls.size());

        auto  init_v     = initialize_copy_network_v<virtual_pi_network>(ntk);
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
        const auto gather_fanin_signals = [&](const auto& n, const auto& lvl, size_t& edge_it)
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
                                      auto it = ntk.is_maj(n) ? 4 : 3;
                                      for (size_t i = 0; i < it; i++)
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

        size_t edge_it = 0;
        for (size_t i = ntk_lvls.size(); i-- > 0;)
        {
            edge_it       = 0;
            auto& lvl     = ntk_lvls[i];
            auto& lvl_new = ntk_lvls_new[i];
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
                    auto children = gather_fanin_signals(nd, ntk_lvls_new[i + 1], edge_it);

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
                assert(tgt_signal_v.size() == 1);
                const auto tgt_signal = tgt_signal_v[0];

                const auto tgt_po = ntk.is_complemented(po) ? ntk_dest_v.create_not(tgt_signal) : tgt_signal;

                ntk_dest_v.create_po(tgt_po);
            });

        *this = ntk_dest_v;
    }

    /**
     * @brief Reimplementation of num_gates(). Retrieves the number of gates in the virtual_pi_network.
     *
     * @return The number of gates in the virtual_pi_network.
     */
    auto num_gates() const
    {
        return static_cast<uint32_t>(_storage->nodes.size() - virtual_inputs->size() - _storage->inputs.size() - 2);
    }

    /**
     * @brief Reimplementation of size(). Get the size of the virtual_pi_network.
     *
     * @return The size of the virtual_pi_network as a uint32_t.
     */
    auto size() const
    {
        return static_cast<uint32_t>(_storage->nodes.size());
    }

    /**
     * \brief Calculate the real size of the virtual_pi_network.
     *
     * This function calculates the real size of the virtual_pi_network by subtracting the size of the virtual_inputs
     * vector from the size of the _storage->nodes vector.
     *
     * \return The real size of the virtual_pi_network as a uint32_t.
     */
    [[nodiscard]] auto size_real() const
    {
        return static_cast<uint32_t>(_storage->nodes.size() - virtual_inputs->size());
    }

    /**
     * @brief Create a virtual PI mapping to a real PI.
     *
     * This function creates a virtual PI mapping to a real PI in the network. It adds a new node to the
     * `_storage->nodes` vector and inserts the index of the new node into the `virtual _inputs` vector. It also updates
     * the data of the new node.
     *
     * @param real_pi The node representing the real PI in the network.
     * @return The index of the newly created virtual PI.
     */
    signal create_virtual_pi(const node& real_pi)
    {
        const auto index = _storage->nodes.size();
        _storage->nodes.emplace_back();
        virtual_inputs->emplace_back(index);
        _storage->nodes[index].data[1].h1 = 2;
        map.emplace_back(index, real_pi);
        return index;
    }

    /**
     * @brief Reimplementation of is_pi(). Check if a given node is a primary input (PI) in the virtual_pi_network.
     *
     * A node is considered a PI if it exists in the virtual_inputs vector or the inputs vector of the _storage object.
     *
     * @param n The node to check.
     * @return True if the node is a PI, false otherwise.
     */
    [[nodiscard]] bool is_pi(node const& n) const
    {
        return std::find(virtual_inputs->begin(), virtual_inputs->end(), n) != virtual_inputs->end() ||
               std::find(_storage->inputs.begin(), _storage->inputs.end(), n) != _storage->inputs.end();
    }

    /**
     * @brief Check if a given node is a virtual primary input (PI) in the virtual_pi_network.
     *
     * A node is considered a virtual PI if it exists in the virtual_inputs vector of the virtual_pi_network.
     *
     * @param n The node to check.
     * @return True if the node is a virtual PI, false otherwise.
     */
    [[nodiscard]] bool is_pi_virtual(node const& n) const
    {
        return std::find(virtual_inputs->begin(), virtual_inputs->end(), n) != virtual_inputs->end();
    }

    /**
     * @brief Check if a given node is a real primary input (PI) in the virtual_pi_network.
     *
     * A node is considered a real PI if it exists in the inputs vector of the `_storage` object.
     *
     * @param n The node to check.
     * @return True if the node is a real PI, false otherwise.
     */
    [[nodiscard]] bool is_pi_real(node const& n) const
    {
        return std::find(_storage->inputs.begin(), _storage->inputs.end(), n) != _storage->inputs.end();
    }

    /**
     * Check if a node is a const input in the virtual_pi_network.
     *
     * A node is considered a const input if it exists in the virtual_inputs vector or the inputs vector of the _storage
     * object.
     *
     * @param n The node to check.
     * @return True if the node is a const input, false otherwise.
     */
    [[nodiscard]] bool is_ci(node const& n) const
    {
        return std::find(virtual_inputs->begin(), virtual_inputs->end(), n) != virtual_inputs->end() ||
               std::find(_storage->inputs.begin(), _storage->inputs.end(), n) != _storage->inputs.end();
    }

    /**
     * @brief Check if a given node is a virtual constant input (CI) in the virtual_pi_network.
     *
     * A node is considered a virtual CI if it exists in the virtual_inputs vector.
     *
     * @param n The node to check.
     * @return True if the node is a virtual CI, false otherwise.
     */
    [[nodiscard]] bool is_ci_virtual(node const& n) const
    {
        return std::find(virtual_inputs->begin(), virtual_inputs->end(), n) != virtual_inputs->end();
    }

    /**
     * @brief Check if a given node is a real combinatorial input (CI) in the virtual_pi_network.
     *
     * A node is considered a real CI if it exists in the inputs vector of the `_storage` object.
     *
     * @param n The node to check.
     * @return True if the node is a real CI, false otherwise.
     */
    [[nodiscard]] bool is_ci_real(node const& n) const
    {
        return std::find(_storage->inputs.begin(), _storage->inputs.end(), n) != _storage->inputs.end();
    }

    /**
     * @brief Retrieve the number of combinatorial inputs (CIs) in the virtual_pi_network.
     *
     * The num_cis() function returns the total number of CIs in the virtual_pi_network.
     * It includes both virtual constant inputs and real constant inputs.
     *
     * @return The number of CIs as a uint32_t.
     */
    [[nodiscard]] auto num_cis() const
    {
        return static_cast<uint32_t>(_storage->inputs.size() + virtual_inputs->size());
    }

    /**
     * @brief Get the number of virtual combinatorial inputs (CIs) in the virtual_pi_network.
     *
     * The num_cis_virtual() function returns the total number of virtual CIs in the virtual_pi_network.
     * Virtual constant inputs are those which exist in the virtual_inputs vector.
     *
     * @return The number of virtual CIs as a uint32_t.
     */
    [[nodiscard]] auto num_cis_virtual() const
    {
        return static_cast<uint32_t>(virtual_inputs->size());
    }

    /**
     * @brief Get the number of real constant inputs (CIs) in the virtual_pi_network.
     *
     * The num_cis_real() function returns the total number of real CIs in the virtual_pi_network.
     * Real constant inputs are those which exist in the inputs vector of the `_storage` object.
     *
     * @return The number of real CIs as a uint32_t.
     */
    [[nodiscard]] auto num_cis_real() const
    {
        return static_cast<uint32_t>(_storage->inputs.size());
    }

    /**
     * @brief Reimplementation of num_pis(). Get the number of primary inputs (PIs) in the virtual_pi_network.
     *
     * The num_pis() function returns the total number of PIs in the virtual_pi_network.
     * It includes both virtual PIs and real PIs.
     *
     * @return The number of PIs as a uint32_t.
     */
    [[nodiscard]] auto num_pis() const
    {
        return static_cast<uint32_t>(_storage->inputs.size() + virtual_inputs->size());
    }

    /**
     * @brief Get the number of virtual primary inputs (PIs) in the virtual_pi_network.
     *
     * The num_pis_virtual() function returns the total number of virtual PIs in the virtual_pi_network.
     * Virtual PIs are those which exist in the virtual_inputs vector.
     *
     * @return The number of virtual PIs as a uint32_t.
     */
    [[nodiscard]] auto num_pis_virtual() const
    {
        return static_cast<uint32_t>(virtual_inputs->size());
    }

    /**
     * @brief Retrieve the number of real primary inputs (PIs) in the virtual_pi_network.
     *
     * The num_pis_real() function returns the total number of real PIs in the virtual_pi_network.
     * Real PIs are those which exist in the inputs vector of the `_storage` object.
     *
     * @return The number of real PIs as a uint32_t.
     */
    [[nodiscard]] auto num_pis_real() const
    {
        return static_cast<uint32_t>(_storage->inputs.size());
    }

    /**
     * @brief Retrieve the real PI mapped to the given virtual PI.
     *
     * This function searches for the given virtual PI in the mapping table and returns the corresponding real PI.
     * It throws a std::runtime_error if the virtual PI is not found in the mapping table.
     *
     * @param v_pi The virtual PI to find the corresponding real PI for.
     * @return The real PI mapped to the virtual PI.
     * @throws std::runtime_error If the virtual PI is not found in the mapping table.
     */
    [[nodiscard]] auto get_real_pi(const node& v_pi) const
    {
        auto it = std::find_if(map.begin(), map.end(), [v_pi](const auto& pair) { return pair.first == v_pi; });

        if (it != map.end())
            return it->second;

        throw std::runtime_error("Error: node is not a virtual pi");
    }

    /**
     * @brief Reimplementation of foreach_pi(). Apply a given function to each primary input in the virtual_pi_network.
     *
     * This function iterates over all primary inputs in the virtual_pi_network and applies the given function to each
     * primary input.
     *
     * @tparam Fn The type of function to apply to each primary input.
     * @param fn The function to apply to each primary input.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->inputs.begin(), _storage->inputs.end(), fn);
        mockturtle::detail::foreach_element(virtual_inputs->begin(), virtual_inputs->end(), fn);
    }

    /**
     * @brief Iterates over the primary inputs of the circuit and applies a given
     * function.
     *
     * This function iterates over the primary inputs of the circuit and applies a
     * given function to each primary input.
     *
     * @tparam Fn The type of the function.
     * @param fn The function to be applied to each primary input.
     */
    template <typename Fn>
    void foreach_pi_real(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->inputs.begin(), _storage->inputs.end(), fn);
    }

    /**
     * @brief Applies a given function to each element in a container.
     *
     * This function applies the provided function to each element in the container `Fn`. The container must support
     * iteration using `begin()` and `end()` methods.
     *
     * @tparam Fn The container type.
     * @param fn The function to apply to each element.
     *
     * @see mockturtle::detail::foreach_element
     */
    template <typename Fn>
    void foreach_pi_virtual(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(virtual_inputs->begin(), virtual_inputs->end(), fn);
    }

    /**
     * @brief Applies a function to each combinatorial input.
     *
     * This function applies the given function to each input stored in the circuit.
     * It also applies the function to each virtual input.
     * The data types of the function passed to this function must be compatible
     * with the data types stored in the inputs and virtual_inputs of the circuit.
     *
     * @tparam Fn The type of the function to apply to each input.
     * @param fn The function to apply to each input.
     */
    template <typename Fn>
    void foreach_ci(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->inputs.begin(), _storage->inputs.end(), fn);
        mockturtle::detail::foreach_element(virtual_inputs->begin(), virtual_inputs->end(), fn);
    }

    /**
     * @brief Applies a given function to each input element in the circuit
     *
     * This function iterates over each input element in the circuit and applies
     * the given function to it.
     *
     * @tparam Fn The type of the function to be applied
     * @param fn The function to be applied
     */
    template <typename Fn>
    void foreach_ci_real(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(_storage->inputs.begin(), _storage->inputs.end(), fn);
    }

    /**
     * @brief Executes a given function for each element in a container of virtual inputs.
     *
     * This function iterates over every element in the container of virtual inputs, and
     * invokes the provided callable object on each element.
     *
     * @tparam Fn  Type of the callable object.
     * @param fn   The callable object to be invoked on each element.
     *
     * @note The provided callable object must accept a single argument of the same type as
     *       the elements in the container.
     */
    template <typename Fn>
    void foreach_ci_virtual(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(virtual_inputs->begin(), virtual_inputs->end(), fn);
    }

    /**
     * @brief Applies a function to each gate in the circuit.
     *
     * This function iterates over each gate in the circuit, excluding the constants.
     *
     * @tparam Fn The type of the function.
     * @param fn The function to apply to each gate.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        auto r = mockturtle::range<uint64_t>(2u, _storage->nodes.size()); /* start from 2 to avoid constants */
        mockturtle::detail::foreach_element_if(
            r.begin(), r.end(), [this](auto n) { return (!is_ci(n) && !is_virtual_ci(n)); }, fn);
    }

    /**
     * @brief Removes virtual input nodes from the storage. This enables equivalence checking.
     *
     * This function removes virtual input nodes from the storage by performing the following steps:
     * 1. Sorts the virtual input nodes in descending order.
     * 2. For each virtual input node, finds the corresponding real_pi index mapped to it from the map.
     * 3. Substitute the virtual input node with the new node index, which corresponds to the real_pi.
     * 4. Adjusts the inputs, nodes vector, and outputs.
     * 5. Erases the virtual input node from the nodes vector.
     *
     * After removing the virtual input nodes, this function clears the virtual_inputs and map containers.
     */
    void remove_virtual_input_nodes()
    {
        std::sort(virtual_inputs->begin(), virtual_inputs->end(), std::greater<uint64_t>());
        for (uint32_t i : *virtual_inputs)
        {
            if (i < _storage->nodes.size())
            {
                uint64_t new_node = 0;  // = ( _storage->outputs.begin() + i )->index
                for (const auto& pair : map)
                {
                    if (pair.first == i)
                    {
                        new_node = pair.second;
                        break;
                    }
                }
                substitute_node(i, new_node);
                // Adjust inputs
                for (auto& input : _storage->inputs)
                {
                    if (input > i)
                    {
                        --input;
                    }
                }
                // Adjust nodes vector in the technology_network _storage
                for (auto j = 0u; j < _storage->nodes.size(); ++j)
                {
                    auto& n = _storage->nodes[j];
                    for (auto& child : n.children)
                    {
                        if (child.data > i)
                        {
                            --child.data;
                        }
                    }
                }
                // Adjust outputs
                for (auto& output : _storage->outputs)
                {
                    --output.index;
                }
                _storage->nodes.erase(_storage->nodes.begin() + i);
            }
        }

        // Clear virtual_inputs after using it
        virtual_inputs->clear();
        map.clear();
    }

  protected:
    /*
     * Shared pointer vector storage for virtual_inputs.
     * */
    std::shared_ptr<std::vector<uint32_t>> virtual_inputs;
    /*
     * Map from virtual_pis to real_pis.
     * */
    std::vector<std::pair<uint64_t, uint64_t>> map;
}; /* color_view */

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_VIEW_HPP
