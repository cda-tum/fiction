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
 * With virtual PIs PI-duplications are handled.
 */
class virtual_pi_network : public technology_network
{
  public:
    using node   = typename technology_network::node;
    using signal = typename technology_network::signal;

  public:
    explicit virtual_pi_network() = default;

    explicit virtual_pi_network(const technology_network& other)
    {
        _storage = std::make_shared<mockturtle::klut_storage>(*(other._storage));  // create deep copy of storage
    }

    auto num_gates() const
    {
        return static_cast<uint32_t>(_storage->nodes.size() - virtual_inputs.size() - _storage->inputs.size() - 2);
    }

    auto size() const
    {
        return static_cast<uint32_t>(_storage->nodes.size() - virtual_inputs.size());
    }

    [[nodiscard]] auto size_virtual() const
    {
        return static_cast<uint32_t>(_storage->nodes.size());
    }

    /*! \brief Creates a new virtual PI */
    signal create_virtual_pi(const node& real_pi)
    {
        const auto index = _storage->nodes.size();
        _storage->nodes.emplace_back();
        virtual_inputs.emplace_back(index);
        _storage->nodes[index].data[1].h1 = 2;
        map.emplace_back(index, real_pi);
        return index;
    }

    [[nodiscard]] bool is_virtual_pi(node const& n) const
    {
        return std::find(virtual_inputs.begin(), virtual_inputs.end(), n) != virtual_inputs.end();
    }

    [[nodiscard]] bool is_virtual_ci(node const& n) const
    {
        return std::find(virtual_inputs.begin(), virtual_inputs.end(), n) != virtual_inputs.end();
    }

    [[nodiscard]] auto num_cis_virtual() const
    {
        return static_cast<uint32_t>(virtual_inputs.size());
    }

    [[nodiscard]] auto num_pis_virtual() const
    {
        return static_cast<uint32_t>(virtual_inputs.size());
    }

    [[nodiscard]] auto num_cis_all() const
    {
        return static_cast<uint32_t>(_storage->inputs.size() + virtual_inputs.size());
    }

    [[nodiscard]] auto num_pis_all() const
    {
        return static_cast<uint32_t>(_storage->inputs.size() + virtual_inputs.size());
    }

    [[nodiscard]] auto get_real_pi(const node& v_pi) const
    {
        auto it = std::find_if(map.begin(), map.end(), [v_pi](const auto& pair) { return pair.first == v_pi; });

        if (it != map.end())
            return it->second;

        throw std::runtime_error("Error: node is not a virtual pi");
    }

    // exclude virtual_inputs
    template <typename Fn>
    void foreach_pi_virtual(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(virtual_inputs.begin(), virtual_inputs.end(), fn);
    }

    template <typename Fn>
    void foreach_ci_virtual(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(virtual_inputs.begin(), virtual_inputs.end(), fn);
    }

    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        auto r = mockturtle::range<uint64_t>(2u, _storage->nodes.size()); /* start from 2 to avoid constants */
        mockturtle::detail::foreach_element_if(
            r.begin(), r.end(), [this](auto n) { return (!is_ci(n) && !is_virtual_ci(n)); }, fn);
    }

    // This function removed the virtual PIs and connects edges to the real PIs they are mapped to. Like this
    // equivalence checking can be done.
    void remove_virtual_input_nodes()
    {
        std::sort(virtual_inputs.begin(), virtual_inputs.end(), std::greater<uint64_t>());
        for (uint32_t i : virtual_inputs) {
            if(i < _storage->nodes.size()) {
                uint64_t new_node = ( _storage->outputs.begin() + i )->index;
                for (const auto& pair : map) {
                    if (pair.first == i) {
                        new_node = pair.second;
                        break;
                    }
                }
                substitute_node(i, new_node);
                // The outputs iterators also need to be adjusted with -1
                for ( auto& output : _storage->outputs )
                {
                    --output.index;
                }
                _storage->nodes.erase(_storage->nodes.begin() + i);
            }
        }

        // Clear virtual_inputs after using it
        virtual_inputs.clear();
        map.clear();
    }

  protected:
    std::vector<uint32_t>                      virtual_inputs;
    std::vector<std::pair<uint64_t, uint64_t>> map;
}; /* color_view */

}  // namespace fiction

#endif  // FICTION_NODE_DUPLICATION_VIEW_HPP
