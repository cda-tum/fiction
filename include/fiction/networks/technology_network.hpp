//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_TECHNOLOGY_NETWORK_HPP
#define FICTION_TECHNOLOGY_NETWORK_HPP

#include <mockturtle/networks/klut.hpp>

namespace fiction
{

/**
 * A `mockturtle` logic network type that extends `klut_network`. It reimplements all node creation functions to disable
 * structural hashing. Furthermore, `create_buf` is overwritten such that it creates real buffer nodes that count
 * towards the size of the network and compute the identity function. In the same way, a buffer with more than one
 * fanout node is considered a fanout node (or splitter in some contexts) that copies an incoming signal to two or more
 * data paths.
 *
 * This network is used internally for most implemented FCN physical design algorithms and extends `klut_network` by
 * some further convenience functions but is still fully compatible with the mockturtle network API. The function
 * convert_network can be used to convert any other mockturtle network to this type.
 *
 * Most functions in this class are borrowed and reimplemented directly from `klut_network` and are, therefore, only
 * sporadically documented where their behavior might differ. For information consider `mockturtle/networks/klut.hpp`.
 */
class technology_network : public mockturtle::klut_network
{
  public:
#pragma region Types and constructors

    technology_network()
    {
        add_additional_functions();
    }

    explicit technology_network(std::shared_ptr<mockturtle::klut_storage> s) : mockturtle::klut_network(std::move(s))
    {
        add_additional_functions();
    }

    [[nodiscard]] technology_network clone() const
    {
        return technology_network{std::make_shared<mockturtle::klut_storage>(*_storage)};
    }

#pragma endregion

#pragma region Primary I / O and functions

    /**
     * Checks whether a node is primary output by considering the signal pointing to it. This is possible because this
     * network does not use complemented signals.
     *
     * @param n Node to consider.
     * @return `true` iff `n`'s signal is primary output.
     */
    [[nodiscard]] bool is_po(const node& n) const
    {
        return std::find_if(_storage->outputs.cbegin(), _storage->outputs.cend(), [this, &n](const auto& p)
                            { return this->get_node(p.index) == n; }) != _storage->outputs.cend();
    }

#pragma endregion

#pragma region Create unary functions

    /**
     * Creates a buffer node in the network that computes the identity function and counts towards the network size and
     * depth. A buffer node with more than one output is called a fanout.
     *
     * @param a Incoming signal to the newly created buffer node.
     * @return Outgoing signal of the newly created buffer node.
     */
    signal create_buf(const signal& a)
    {
        return _create_node({a}, 2);
    }

    signal create_not(const signal& a)
    {
        return is_constant(a) ? get_constant(!constant_value(a)) : _create_node({a}, 3);
    }

#pragma endregion

#pragma region Create binary functions

    signal create_and(signal a, signal b)
    {
        return _create_node({a, b}, 4);
    }

    signal create_nand(signal a, signal b)
    {
        return _create_node({a, b}, 5);
    }

    signal create_or(signal a, signal b)
    {
        return _create_node({a, b}, 6);
    }

    signal create_nor(signal a, signal b)
    {
        return _create_node({a, b}, 7);
    }

    signal create_lt(signal a, signal b)
    {
        return _create_node({a, b}, 8);
    }

    signal create_le(signal a, signal b)
    {
        return _create_node({a, b}, 11);
    }

    signal create_xor(signal a, signal b)
    {
        return _create_node({a, b}, 12);
    }

    signal create_xnor(signal a, signal b)
    {
        return _create_node({a, b}, 13);
    }
#pragma endregion

#pragma region Create ternary functions

    signal create_maj(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 14);
    }

    signal create_ite(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 16);
    }

    signal create_xor3(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 18);
    }

    signal create_dot(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 20);
    }
#pragma endregion

#pragma region Create nary functions

    signal create_nary_and(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(true),
                                       [this](auto const& a, auto const& b) { return create_and(a, b); });
    }

    signal create_nary_or(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(false),
                                       [this](auto const& a, auto const& b) { return create_or(a, b); });
    }

    signal create_nary_xor(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(false),
                                       [this](auto const& a, auto const& b) { return create_xor(a, b); });
    }
#pragma endregion

#pragma region Create arbitrary functions

    /**
     * Disable structural hashing.
     */
    signal _create_node(const std::vector<signal>& children, uint32_t literal)
    {
        storage::element_type::node_type node_data;
        std::copy(children.begin(), children.end(), std::back_inserter(node_data.children));
        node_data.data[1].h1 = literal;

        const auto index = _storage->nodes.size();
        _storage->nodes.push_back(node_data);

        /* increase ref-count to children */
        for (auto c : children)
        {
            _storage->nodes[c].data[0].h1++;
        }

        set_value(index, 0);

        for (auto const& fn : _events->on_add)
        {
            (*fn)(index);
        }

        return index;
    }
    /**
     * Disable structural hashing.
     */
    signal create_node(const std::vector<signal>& children, const kitty::dynamic_truth_table& function)
    {
        if (children.empty())
        {
            assert(function.num_vars() == 0u);
            return get_constant(!kitty::is_const0(function));
        }
        return _create_node(children, _storage->data.cache.insert(function));
    }

    signal clone_node(const klut_network& other, const node& source, const std::vector<signal>& children)
    {
        assert(!children.empty());
        const auto tt = other._storage->data.cache[other._storage->nodes[source].data[1].h1];
        return create_node(children, tt);
    }

#pragma endregion

#pragma region Structural properties

    /**
     * Returns whether the given node `n` is a buffer node, i.e., whether `n` computes the identity function and is not
     * a PI. This also returns `true` on fanout nodes.
     *
     * @param n Node to consider.
     * @return `true` iff `n` is a buffer node.
     */
    [[nodiscard]] bool is_buf(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 2 && !is_pi(n);
    }
    /**
     * Returns whether the given node `n` is a fanout node, i.e., whether `n` is a buffer node and has more than one
     * output signal.
     *
     * @param n Node to consider.
     * @return `true` iff `n` is a fanout node.
     */
    [[nodiscard]] bool is_fanout(const node& n) const noexcept
    {
        return is_buf(n) && fanout_size(n) > 1;
    }
    /**
     * Returns whether the given node `n` is an inverter node.
     *
     * @param n Node to consider.
     * @return `true` iff `n` is an inverter node.
     */
    [[nodiscard]] bool is_inv(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 3;
    }

    [[nodiscard]] bool is_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 4;
    }

    [[nodiscard]] bool is_nand(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 5;
    }

    [[nodiscard]] bool is_or(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 6;
    }

    [[nodiscard]] bool is_nor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 7;
    }

    [[nodiscard]] bool is_xor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 12;
    }

    [[nodiscard]] bool is_xnor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 13;
    }

    [[nodiscard]] bool is_maj(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 14;
    }

    [[nodiscard]] bool is_ite(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 16;
    }

    [[nodiscard]] bool is_xor3(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 18;
    }

    [[nodiscard]] bool is_dot(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 20;
    }

    [[nodiscard]] bool is_and3(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 22;
    }

    [[nodiscard]] bool is_xor_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 24;
    }

    [[nodiscard]] bool is_or_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 26;
    }

    [[nodiscard]] bool is_onehot(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 28;
    }

    [[nodiscard]] bool is_gamble(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 31;  // 31 since gamble is not normal
    }

    [[nodiscard]] bool is_and_xor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h1 == 32;
    }

#pragma endregion

#pragma region Structural manipulation

    /**
     * Adds additional buffer nodes for each primary output that does not already point to a buffer.
     */
    void substitute_po_signals() noexcept
    {
        foreach_po(
            [this](const auto& po, auto index)
            {
                if (!is_buf(get_node(po)) || is_fanout(get_node(po)))
                {
                    // decrease ref-count
                    _storage->nodes[po].data[0].h1--;
                    // create a new buf node
                    const auto b = create_buf(po);
                    // bend PO signal to point to the new node
                    _storage->outputs[index] = b;
                }
            });
    }

#pragma endregion

  protected:
    /**
     * Adds some further Boolean functions to the truth table cache for fixed indexing.
     */
    void add_additional_functions() noexcept
    {
        // NOLINTBEGIN(*-pointer-arithmetic): mockturtle requirement

        // create dot function: a xor (c or a and b)
        static uint64_t dot = 0x52;

        kitty::dynamic_truth_table tt_dot(3);
        kitty::create_from_words(tt_dot, &dot, &dot + 1);
        _storage->data.cache.insert(tt_dot);

        // create and3 function: a and b and c
        static uint64_t and3 = 0x80;

        kitty::dynamic_truth_table tt_and3(3);
        kitty::create_from_words(tt_and3, &and3, &and3 + 1);
        _storage->data.cache.insert(tt_and3);

        // create xor_and function: a and (b xor c)
        static uint64_t xor_and = 0x28;

        kitty::dynamic_truth_table tt_xor_and(3);
        kitty::create_from_words(tt_xor_and, &xor_and, &xor_and + 1);
        _storage->data.cache.insert(tt_xor_and);

        // create or_and function: a and (b or c)
        static uint64_t or_and = 0xa8;

        kitty::dynamic_truth_table tt_or_and(3);
        kitty::create_from_words(tt_or_and, &or_and, &or_and + 1);
        _storage->data.cache.insert(tt_or_and);

        // create onehot function: exactly one of a,b,c
        static uint64_t onehot = 0x16;

        kitty::dynamic_truth_table tt_onehot(3);
        kitty::create_from_words(tt_onehot, &onehot, &onehot + 1);
        _storage->data.cache.insert(tt_onehot);

        // create gamble function: all or nothing of a,b,c
        static uint64_t gamble = 0x81;

        kitty::dynamic_truth_table tt_gamble(3);
        kitty::create_from_words(tt_gamble, &gamble, &gamble + 1);
        _storage->data.cache.insert(tt_gamble);

        // create and_xor function: a xor b and c
        static uint64_t and_xor = 0x6a;

        kitty::dynamic_truth_table tt_and_xor(3);
        kitty::create_from_words(tt_and_xor, &and_xor, &and_xor + 1);
        _storage->data.cache.insert(tt_and_xor);

        // NOLINTEND(*-pointer-arithmetic): mockturtle requirement
    }
};

}  // namespace fiction

#endif  // FICTION_TECHNOLOGY_NETWORK_HPP
