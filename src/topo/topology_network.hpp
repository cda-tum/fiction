//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_TOPOLOGY_NETWORK_HPP
#define FICTION_TOPOLOGY_NETWORK_HPP

#include <mockturtle/networks/klut.hpp>

namespace fiction
{

class topology_network : public mockturtle::klut_network
{
  public:
#pragma region Create unary functions

    /**
     * Make buffers real nodes.
     */
    signal create_buf(signal const& a)
    {
        return _create_node({a}, 2);
    }

#pragma endregion

#pragma region Create arbitrary functions

    /**
     * Disable structural hashing.
     */
    signal _create_node(std::vector<signal> const& children, uint32_t literal)
    {
        storage::element_type::node_type node;
        std::copy(children.begin(), children.end(), std::back_inserter(node.children));
        node.data[1].h1 = literal;

        const auto index = _storage->nodes.size();
        _storage->nodes.push_back(node);
        _storage->hash[node] = index;

        /* increase ref-count to children */
        for (auto c : children) { _storage->nodes[c].data[0].h1++; }

        set_value(index, 0);

        for (auto const& fn : _events->on_add) { (*fn)(index); }

        return index;
    }

#pragma endregion

#pragma region Structural properties

    [[nodiscard]] bool is_and(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 4;
    }

    [[nodiscard]] bool is_or(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 6;
    }

    [[nodiscard]] bool is_xor(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 12;
    }

    [[nodiscard]] bool is_maj(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 14;
    }

    [[nodiscard]] bool is_ite(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 16;
    }

    [[nodiscard]] bool is_xor3(node const& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 18;
    }

#pragma endregion
};

}  // namespace fiction

#endif  // FICTION_TOPOLOGY_NETWORK_HPP
