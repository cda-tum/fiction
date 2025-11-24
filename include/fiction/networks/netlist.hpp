//
// Created by marcel on 20.05.21.
//

#ifndef FICTION_NETLIST_HPP
#define FICTION_NETLIST_HPP

#include <mockturtle/networks/block.hpp>

namespace fiction
{

/**
 * A `mockturtle` logic network type that extends `block_network`. It reimplements all node creation functions to
 * disable structural hashing. Furthermore, `create_buf` is overwritten such that it creates real buffer nodes that
 * count towards the size of the network and compute the identity function. In the same way, a buffer with more than one
 * fanout node is considered a fanout node (or splitter in some contexts) that copies an incoming signal to two or more
 * data paths.
 *
 * This network is used internally for most implemented FCN physical design algorithms and extends `block_network` by
 * some further convenience functions but is still fully compatible with the mockturtle network API. The function
 * convert_network can be used to convert any other mockturtle network to this type.
 *
 * Key differences from `block_network`:
 * - **Structural hashing disabled**: Unlike `block_network`, which already uses `storage_no_hash`, this class ensures
 *   that `_create_node` and `create_node` always create new nodes even if identical nodes exist. This is important for
 *   physical design where each gate instance must be tracked separately.
 * - **Real buffer nodes**: Buffers are actual nodes that contribute to network size and depth, not just wire aliases.
 * - **Fanout tracking**: Buffers with multiple outputs are explicitly tracked as fanout/splitter nodes.
 * - **Extended function library**: Adds specialized ternary and custom functions (dot, and3, xor_and, or_and, onehot,
 *   gamble, and_xor) beyond standard `block_network` operations.
 *
 * Key differences from `klut_network` (used in `technology_network`):
 * - **Storage structure**: Uses `block_storage` with `block_storage_node` instead of `klut_storage` with
 *   `klut_storage_node`. In `block_network`, function literals are stored at `data[2+i].h1` (where i is the output
 *   index) to support multi-output nodes, whereas `klut_network` stores them at `data[1].h1` since it only supports
 *   single-output nodes.
 * - **Signal representation**: Uses a complex signal struct with bit fields (complement:1, output:1, index:61) to
 *   support multi-output nodes and signal inversion, whereas `klut_network` uses simple `uint64_t` signals.
 * - **Fanout counting**: Uses `data[1].h2` for total fanout and `data[2+i].h2` for per-output fanout, with MSB of
 *   `data[1].h2` reserved as a dead flag. `klut_network` uses `data[0].h1` for total fanout only.
 * - **Multi-output support**: Can create nodes with multiple outputs (e.g., half adders with carry and sum), which
 *   `klut_network` cannot do.
 * - **No structural hashing**: `block_network` uses `storage_no_hash`, avoiding the hash table overhead present in
 *   `klut_network`.
 *
 * Most functions in this class are borrowed and reimplemented directly from `block_network` and are, therefore,
 * documented to explain their behavior and any differences. For additional information about the base `block_network`
 * API, consider `mockturtle/networks/block.hpp`.
 */
class netlist : public mockturtle::block_network
{
  public:
#pragma region Types and constructors

    /**
     * Default constructor. Initializes an empty netlist and adds custom Boolean functions to the truth table cache.
     */
    netlist()
    {
        add_additional_functions();
    }

    /**
     * Constructor from shared storage. Allows creating a netlist from existing `block_storage`.
     *
     * @param s Shared pointer to `block_storage` containing the network data.
     */
    explicit netlist(std::shared_ptr<mockturtle::block_storage> s) : mockturtle::block_network(std::move(s))
    {
        add_additional_functions();
    }

    /**
     * Creates a deep copy of this netlist by cloning the underlying storage.
     *
     * @return A new netlist instance with copied storage.
     */
    [[nodiscard]] netlist clone() const
    {
        return netlist{std::make_shared<mockturtle::block_storage>(*_storage)};
    }

#pragma endregion

#pragma region Primary I / O and functions

    /**
     * Checks whether a node is primary output by checking if any output signal points to it.
     *
     * Unlike `klut_network` which can compare signals directly, this implementation must check both the node index
     * and potentially the output port of the signal since `block_network` supports multi-output nodes.
     *
     * @param n Node to consider.
     * @return `true` iff `n`'s node index is referenced by any primary output signal.
     */
    [[nodiscard]] bool is_po(const node& n) const
    {
        return std::find_if(_storage->outputs.cbegin(), _storage->outputs.cend(),
                            [&n](const auto& p) { return p.index == n; }) != _storage->outputs.cend();
    }

    /**
     * Checks whether a signal points to a primary output node.
     *
     * @param s Signal to consider.
     * @return `true` iff the node pointed to by `s` is primary output.
     */
    [[nodiscard]] bool is_po(const signal& s) const
    {
        return is_po(get_node(s));
    }

#pragma endregion

#pragma region Create unary functions

    /**
     * Creates a buffer node in the network that computes the identity function and counts towards the network size and
     * depth. A buffer node with more than one output is called a fanout.
     *
     * This overrides the base `block_network::create_buf` behavior. In the base class, buffers are actual nodes
     * (unlike `klut_network` where they are aliases). This implementation explicitly uses literal 2, which corresponds
     * to the identity function in the truth table cache.
     *
     * @param a Incoming signal to the newly created buffer node.
     * @return Outgoing signal of the newly created buffer node pointing to the new node with index determined by
     * current storage size.
     */
    signal create_buf(const signal& a)
    {
        return _create_node({a}, 2);
    }

    /**
     * Creates an inverter (NOT) node in the network.
     *
     * Handles constant propagation: if the input signal points to a constant node, returns the complemented constant
     * directly. Otherwise, creates a new inverter node with literal 3 (NOT function in truth table cache).
     *
     * @param a Input signal to invert.
     * @return Output signal of the inverter node, or the complemented constant if input is constant.
     */
    signal create_not(const signal& a)
    {
        return is_constant(get_node(a)) ? get_constant(!constant_value(get_node(a))) : _create_node({a}, 3);
    }

#pragma endregion

#pragma region Create binary functions

    /**
     * Creates an AND gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created AND gate (literal 4).
     */
    signal create_and(signal a, signal b)
    {
        return _create_node({a, b}, 4);
    }

    /**
     * Creates a NAND gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created NAND gate (literal 5).
     */
    signal create_nand(signal a, signal b)
    {
        return _create_node({a, b}, 5);
    }

    /**
     * Creates an OR gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created OR gate (literal 6).
     */
    signal create_or(signal a, signal b)
    {
        return _create_node({a, b}, 6);
    }

    /**
     * Creates a NOR gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created NOR gate (literal 7).
     */
    signal create_nor(signal a, signal b)
    {
        return _create_node({a, b}, 7);
    }

    /**
     * Creates a less-than comparison gate (a < b).
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal that is true when a < b (literal 8).
     */
    signal create_lt(signal a, signal b)
    {
        return _create_node({a, b}, 8);
    }

    /**
     * Creates a greater-or-equal comparison gate (a >= b).
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal that is true when a >= b (literal 9).
     */
    signal create_ge(signal a, signal b)
    {
        return _create_node({a, b}, 9);
    }

    /**
     * Creates a greater-than comparison gate (a > b).
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal that is true when a > b (literal 10).
     */
    signal create_gt(signal a, signal b)
    {
        return _create_node({a, b}, 10);
    }

    /**
     * Creates a less-or-equal comparison gate (a <= b).
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal that is true when a <= b (literal 11).
     */
    signal create_le(signal a, signal b)
    {
        return _create_node({a, b}, 11);
    }

    /**
     * Creates an XOR gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created XOR gate (literal 12).
     */
    signal create_xor(signal a, signal b)
    {
        return _create_node({a, b}, 12);
    }

    /**
     * Creates an XNOR gate node with two inputs.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @return Output signal of the newly created XNOR gate (literal 13).
     */
    signal create_xnor(signal a, signal b)
    {
        return _create_node({a, b}, 13);
    }
#pragma endregion

#pragma region Create ternary functions

    /**
     * Creates a majority-of-three (MAJ) gate. Output is true when at least two of the three inputs are true.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @param c Third input signal.
     * @return Output signal of the MAJ gate (literal 14).
     */
    signal create_maj(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 14);
    }

    /**
     * Creates an if-then-else (ITE/MUX) gate. Output is b if a is true, otherwise c.
     *
     * @param a Selector/condition input signal.
     * @param b Then-branch input signal.
     * @param c Else-branch input signal.
     * @return Output signal of the ITE gate (literal 16).
     */
    signal create_ite(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 16);
    }

    /**
     * Creates a three-input XOR gate. Output is true when an odd number of inputs are true.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @param c Third input signal.
     * @return Output signal of the XOR3 gate (literal 18).
     */
    signal create_xor3(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 18);
    }

    /**
     * Creates a DOT gate implementing the function: a XOR (c OR (a AND b)).
     * This is a specialized function useful for certain logic optimizations.
     *
     * @param a First input signal.
     * @param b Second input signal.
     * @param c Third input signal.
     * @return Output signal of the DOT gate (literal 20).
     */
    signal create_dot(signal a, signal b, signal c)
    {
        return _create_node({a, b, c}, 20);
    }
#pragma endregion

#pragma region Create nary functions

    /**
     * Creates a multi-input AND gate by building a balanced tree of binary AND gates.
     *
     * Uses `mockturtle::tree_reduce` to construct a balanced tree, which minimizes circuit depth. If the input vector
     * is empty or has one element, appropriate simplification is applied via the initial constant value.
     *
     * @param fs Vector of input signals to AND together.
     * @return Output signal representing the AND of all inputs. Returns constant true if fs is empty.
     */
    signal create_nary_and(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(true),
                                       [this](auto const& a, auto const& b) { return create_and(a, b); });
    }

    /**
     * Creates a multi-input OR gate by building a balanced tree of binary OR gates.
     *
     * Uses `mockturtle::tree_reduce` to construct a balanced tree, which minimizes circuit depth. If the input vector
     * is empty or has one element, appropriate simplification is applied via the initial constant value.
     *
     * @param fs Vector of input signals to OR together.
     * @return Output signal representing the OR of all inputs. Returns constant false if fs is empty.
     */
    signal create_nary_or(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(false),
                                       [this](auto const& a, auto const& b) { return create_or(a, b); });
    }

    /**
     * Creates a multi-input XOR gate by building a balanced tree of binary XOR gates.
     *
     * Uses `mockturtle::tree_reduce` to construct a balanced tree, which minimizes circuit depth. If the input vector
     * is empty or has one element, appropriate simplification is applied via the initial constant value.
     *
     * @param fs Vector of input signals to XOR together.
     * @return Output signal representing the XOR of all inputs. Returns constant false if fs is empty.
     */
    signal create_nary_xor(const std::vector<signal>& fs)
    {
        return mockturtle::tree_reduce(fs.begin(), fs.end(), get_constant(false),
                                       [this](auto const& a, auto const& b) { return create_xor(a, b); });
    }
#pragma endregion

#pragma region Create arbitrary functions

    /**
     * Internal node creation function with structural hashing disabled.
     *
     * This is the core node creation primitive that bypasses structural hashing. Unlike `klut_network`'s
     * implementation which checks a hash table before creating nodes, this always creates a new node. This is critical
     * for physical design where each logic gate instance must be tracked individually even if functionally equivalent.
     *
     * Implementation details specific to `block_network` storage:
     * - Copies children signals into `node_data.children`
     * - Stores function literal in `node_data.data[2].h1` (first output's function)
     * - Increments reference counts in both `data[1].h2` (total fanout) and `data[2+output].h2` (per-output fanout)
     * - Initializes node value to 0 via `set_value`
     * - Triggers `on_add` event callbacks
     *
     * @param children Vector of input signals to the new node.
     * @param literal Index into the truth table cache identifying the node's Boolean function.
     * @return Signal pointing to the newly created node (index with output=0, complement=0).
     */
    signal _create_node(const std::vector<signal>& children, uint32_t literal)
    {
        storage::element_type::node_type node_data;
        std::copy(children.begin(), children.end(), std::back_inserter(node_data.children));
        node_data.data[2].h1 = literal;

        const auto index = _storage->nodes.size();
        _storage->nodes.push_back(node_data);

        /* increase ref-count to children */
        for (auto c : children)
        {
            _storage->nodes[c.index].data[1].h2++;
            _storage->nodes[c.index].data[2 + c.output].h2++;
        }

        set_value(index, 0);

        for (auto const& fn : _events->on_add)
        {
            (*fn)(index);
        }

        return {index, 0};
    }
    /**
     * Creates a node with an arbitrary Boolean function specified by a truth table.
     *
     * This function with structural hashing disabled. Unlike the base `block_network` which would use structural
     * hashing if available, this always creates a new node instance.
     *
     * The truth table is inserted into the shared cache (if not already present) and the resulting literal index is
     * used to create the node. If children is empty (0-ary function), the function must be a constant and is handled
     * by returning the appropriate constant signal.
     *
     * @param children Vector of input signals to the new node.
     * @param function Truth table specifying the Boolean function to implement.
     * @return Signal pointing to the newly created node, or a constant signal if children is empty.
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

    /**
     * Clones a node from another `block_network`, creating an equivalent node in this network.
     *
     * Retrieves the truth table from the source network's cache and creates a new node with the same function but
     * connected to the provided children signals in this network. This is used for network conversion and copying.
     *
     * @param other Source `block_network` containing the node to clone.
     * @param source Node index in the source network to clone.
     * @param children New input signals in this network to connect to the cloned node.
     * @return Signal pointing to the newly created clone node in this network.
     */
    signal clone_node(const block_network& other, const node& source, const std::vector<signal>& children)
    {
        assert(!children.empty());
        const auto tt = other._storage->data.cache[other._storage->nodes[source].data[2].h1];
        return create_node(children, tt);
    }

#pragma endregion

#pragma region Structural properties

    /**
     * Returns whether the given node `n` is a buffer node, i.e., whether `n` computes the identity function and is not
     * a PI. This also returns `true` on fanout nodes.
     *
     * Checks if the function literal stored in `data[2].h1` equals 2 (the identity function) and verifies the node is
     * not a primary input. Buffers with multiple outputs (fanout > 1) are still considered buffers by this function.
     *
     * @param n Node to consider.
     * @return `true` iff `n` is a buffer node (including fanout nodes).
     */
    [[nodiscard]] bool is_buf(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 2 && !is_pi(n);
    }
    /**
     * Returns whether the given node `n` is a fanout node, i.e., whether `n` is a buffer node and has more than one
     * output signal.
     *
     * A fanout node (also called a splitter) is a buffer that drives multiple paths in the circuit. This is important
     * for physical design as fanout nodes may require special handling (e.g., buffer insertion, routing constraints).
     *
     * @param n Node to consider.
     * @return `true` iff `n` is a fanout node (buffer with fanout_size > 1).
     */
    [[nodiscard]] bool is_fanout(const node& n) const noexcept
    {
        return is_buf(n) && fanout_size(n) > 1;
    }
    /**
     * Returns whether the given node `n` is an inverter node.
     *
     * Checks if the function literal equals 3 (NOT function).
     *
     * @param n Node to consider.
     * @return `true` iff `n` is an inverter node.
     */
    [[nodiscard]] bool is_inv(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 3;
    }

    /**
     * Returns whether the given node `n` is an AND gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the AND function (literal 4).
     */
    [[nodiscard]] bool is_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 4;
    }

    /**
     * Returns whether the given node `n` is a NAND gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the NAND function (literal 5).
     */
    [[nodiscard]] bool is_nand(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 5;
    }

    /**
     * Returns whether the given node `n` is an OR gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the OR function (literal 6).
     */
    [[nodiscard]] bool is_or(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 6;
    }

    /**
     * Returns whether the given node `n` is a NOR gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the NOR function (literal 7).
     */
    [[nodiscard]] bool is_nor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 7;
    }

    /**
     * Returns whether the given node `n` is a less-than comparator (a < b).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the LT function (literal 8).
     */
    [[nodiscard]] bool is_lt(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 8;
    }

    /**
     * Returns whether the given node `n` is a greater-or-equal comparator (a >= b).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the GE function (literal 9).
     */
    [[nodiscard]] bool is_ge(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 9;
    }

    /**
     * Returns whether the given node `n` is a greater-than comparator (a > b).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the GT function (literal 10).
     */
    [[nodiscard]] bool is_gt(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 10;
    }

    /**
     * Returns whether the given node `n` is a less-or-equal comparator (a <= b).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the LE function (literal 11).
     */
    [[nodiscard]] bool is_le(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 11;
    }

    /**
     * Returns whether the given node `n` is an XOR gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the XOR function (literal 12).
     */
    [[nodiscard]] bool is_xor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 12;
    }

    /**
     * Returns whether the given node `n` is an XNOR gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the XNOR function (literal 13).
     */
    [[nodiscard]] bool is_xnor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 13;
    }

    /**
     * Returns whether the given node `n` is a majority gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the MAJ function (literal 14).
     */
    [[nodiscard]] bool is_maj(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 14;
    }

    /**
     * Returns whether the given node `n` is an if-then-else (multiplexer) gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the ITE function (literal 16).
     */
    [[nodiscard]] bool is_ite(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 16;
    }

    /**
     * Returns whether the given node `n` is a three-input XOR gate.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the XOR3 function (literal 18).
     */
    [[nodiscard]] bool is_xor3(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 18;
    }

    /**
     * Returns whether the given node `n` is a DOT gate (a XOR (c OR (a AND b))).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the DOT function (literal 20).
     */
    [[nodiscard]] bool is_dot(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 20;
    }

    /**
     * Returns whether the given node `n` is a three-input AND gate (a AND b AND c).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the AND3 function (literal 22).
     */
    [[nodiscard]] bool is_and3(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 22;
    }

    /**
     * Returns whether the given node `n` is an XOR-AND gate (a AND (b XOR c)).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the XOR_AND function (literal 24).
     */
    [[nodiscard]] bool is_xor_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 24;
    }

    /**
     * Returns whether the given node `n` is an OR-AND gate (a AND (b OR c)).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the OR_AND function (literal 26).
     */
    [[nodiscard]] bool is_or_and(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 26;
    }

    /**
     * Returns whether the given node `n` is a one-hot gate (exactly one of a, b, c is true).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the ONEHOT function (literal 28).
     */
    [[nodiscard]] bool is_onehot(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 28;
    }

    /**
     * Returns whether the given node `n` is a gamble gate (all or nothing: (a AND b AND c) OR (!a AND !b AND !c)).
     *
     * Note: Uses literal 31 instead of following the sequential pattern, as noted in the original code.
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the GAMBLE function (literal 31).
     */
    [[nodiscard]] bool is_gamble(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 31;  // 31 since gamble is not normal
    }

    /**
     * Returns whether the given node `n` is an AND-XOR gate ((a XOR b) AND c).
     *
     * @param n Node to consider.
     * @return `true` iff `n` implements the AND_XOR function (literal 32).
     */
    [[nodiscard]] bool is_and_xor(const node& n) const noexcept
    {
        return _storage->nodes[n].data[2].h1 == 32;
    }

    /**
     * Returns the fanout size of a node, accounting for block_network's structure.
     *
     * In `block_network`, the total fanout count is stored in `data[1].h2`. The MSB (most significant bit) of this
     * field is used as a "dead" flag to mark deleted nodes, so it must be masked out. This is different from
     * `klut_network` which stores fanout in `data[0].h1` without such masking.
     *
     * @param n Node to consider.
     * @return Fanout size of `n` (number of nodes that use `n` as an input).
     */
    [[nodiscard]] uint32_t fanout_size(const node& n) const noexcept
    {
        return _storage->nodes[n].data[1].h2 & UINT32_C(0x7FFFFFFF);
    }

#pragma endregion

#pragma region Structural manipulation

    /**
     * Adds additional buffer nodes for each primary output that does not already point to a buffer.
     *
     * This transformation ensures that every primary output is driven by a dedicated buffer node. If a PO already
     * points to a buffer that has no other fanouts, it is left unchanged. If a PO points to a non-buffer node or a
     * buffer with multiple fanouts (fanout node), a new buffer is inserted.
     *
     * The implementation:
     * 1. Iterates through all primary outputs
     * 2. For each PO that doesn't point to a non-fanout buffer:
     *    a. Decrements reference counts on the original node (both total and per-output)
     *    b. Creates a new buffer node
     *    c. Updates the PO signal to point to the new buffer
     *
     * This is useful for physical design where output ports may require dedicated driver gates.
     */
    void substitute_po_signals() noexcept
    {
        foreach_po(
            [this](const auto& po, auto index)
            {
                const auto po_node = get_node(po);
                if (!is_buf(po_node) || is_fanout(po_node))
                {
                    // decrease ref-count
                    _storage->nodes[po.index].data[1].h2--;
                    _storage->nodes[po.index].data[2 + po.output].h2--;
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
     *
     * This function populates the truth table cache with specialized ternary functions beyond the standard set
     * inherited from `block_network`. These functions are added at specific literal indices so that node creation
     * functions can reference them by known literal values.
     *
     * Functions added (in order):
     * - **DOT (literal 20)**: a XOR (c OR (a AND b)) - truth table 0x52
     * - **AND3 (literal 22)**: a AND b AND c - truth table 0x80
     * - **XOR_AND (literal 24)**: a AND (b XOR c) - truth table 0x28
     * - **OR_AND (literal 26)**: a AND (b OR c) - truth table 0xa8
     * - **ONEHOT (literal 28)**: exactly one of {a,b,c} is true - truth table 0x16
     * - **GAMBLE (literal 31)**: all or none of {a,b,c} are true - truth table 0x81
     * - **AND_XOR (literal 32)**: (a XOR b) AND c - truth table 0x6a
     *
     * The truth table cache is shared across the network and uses `kitty::dynamic_truth_table` for representation.
     * Each 3-input function requires 8 bits (2^3 entries) represented as a 64-bit word.
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

#endif  // FICTION_NETLIST_HPP
