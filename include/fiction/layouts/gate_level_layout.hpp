//
// Created by marcel on 14.05.21.
//

#ifndef FICTION_GATE_LEVEL_LAYOUT_HPP
#define FICTION_GATE_LEVEL_LAYOUT_HPP

#include "fiction/algorithms/verification/design_rule_violations.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/mockturtle_utils.hpp"
#include "fiction/utils/range.hpp"

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/networks/events.hpp>
#include <mockturtle/networks/storage.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/algorithm.hpp>
#include <mockturtle/utils/truth_table_cache.hpp>
#include <phmap.h>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * A layout type to layer on top of a clocked layout that allows the assignment of gates to clock zones (aka tiles in
 * this context). This class represents a gate-level FCN layout and, thus, adds a notion of Boolean logic. The
 * gate_level_layout class fulfills the requirements of a `mockturtle` logic network so that it can be used in many of
 * `mockturtle`'s algorithms. Since a layout has to assign fixed positions to its gates (logic nodes), most generative
 * member functions like `create_pi`, `create_po`, `create_and`, etc. require additional coordinate parameters.
 * Consequently, `mockturtle`'s algorithms cannot be used to generate gate_level_layout networks. To make the class
 * compliant with the API anyways, these member functions have their parameters defaulted but they are, in fact required
 * to create meaningful layouts.
 *
 * The following notion is utilized in this implementation:
 * - a node `n` is an index representing the `n`th created gate. All properties of said gate, e.g., its type and
 * position, are stored independently and can be requested from the layout. An empty layout has 2 nodes, namely `const0`
 * and `const1` as required by `mockturtle`. At the moment, they are not used for anything meaningful but could be.
 *
 * - a signal is an unsigned integer representation of a `tile`, i.e., a coordinate in the layout. It can be seen as a
 * pointer to a position. Consequently, the utilized coordinates need to be convertible to `uint64_t`.
 *
 * - the creation of PIs and POs creates nodes (the latter in contrast to other `mockturtle` networks) that have a
 * position on the layout.
 *
 * - the creation of buffers (`create_buf`) creates nodes as well. A buffer with more than one output is a fanout such
 * that `is_fanout` will return `true` on it. However, it is also still a buffer (`is_buf` returns `true` as well).
 * Buffers and wires are used interchangeably.
 *
 * - each node has an associated gate function. PIs, POs, and buffers compute the identity function.
 *
 * - signals (pointers to tiles) cannot be inverting. Thereby, inverter nodes (gates) have to be created that can be
 * checked for via is_inv.
 *
 * - each `create_...` function requires a tile parameter that determines its placement. If the provided tile is dead,
 * the location will not be stored and the node will not count towards number of gates or wires.
 *
 * - a node can be overwritten by creating another node on its location. This can, however, lead to unwanted effects and
 * should be avoided.
 *
 * - nodes can be moved via the `move_node` function. This function can also be used to update their children, i.e.,
 * incoming signals.
 *
 * Most implementation details regarding `mockturtle`-specific functions are borrowed from
 * `mockturtle/networks/klut.hpp`. Therefore, `mockturtle` API functions are only sporadically documented where their
 * behavior might differ. Information on their functionality can be found in `mockturtle`'s docs.
 *
 * @tparam ClockedLayout The clocked layout that is to be extended by gate functions.
 */
template <typename ClockedLayout>
class gate_level_layout : public ClockedLayout
{
  public:
#pragma region Types and constructors

    using tile = typename ClockedLayout::clock_zone;

    template <typename Node, typename Tile>
    struct gate_level_layout_storage_data
    {
        mockturtle::truth_table_cache<kitty::dynamic_truth_table> fn_cache;

        const Tile const0{0x8000000000000000ull};
        const Tile const1{0xc000000000000000ull};

        // these maps grow large! use parallel_flat_hashmap for better performance
        phmap::parallel_flat_hash_map<Tile, Node> tile_node_map{
            {{const0, static_cast<Node>(0ull)}, {const1, static_cast<Node>(1ull)}}};
        phmap::parallel_flat_hash_map<Node, Tile> node_tile_map{
            {{static_cast<Node>(0ull), const0}, {static_cast<Node>(1ull), const1}}};

        uint32_t num_gates     = 0ull;
        uint32_t num_wires     = 0ull;
        uint32_t num_crossings = 0ull;

        uint32_t trav_id = 0ul;

        std::string layout_name{};

        // usually quite a small map, use flat_hash_map
        phmap::flat_hash_map<Node, std::string> node_names{};
    };

    /*! \brief gate-level layout node
     *
     * `data[0].h1`: Internal (data-flow independent) fan-out size (MSB indicates dead nodes)
     * `data[0].h2`: Application-specific value
     * `data[1].h1`: Function literal in truth table cache
     * `data[2].h2`: Visited flags
     */
    struct gate_level_layout_storage_node : mockturtle::mixed_fanin_node<2>
    {
        bool operator==(const gate_level_layout_storage_node& other) const
        {
            return data[1].h1 == other.data[1].h1 && children == other.children;
        }
    };

    static constexpr auto min_fanin_size = std::max(ClockedLayout::min_fanin_size, 1u);  // NOLINT(*-identifier-naming)
    static constexpr auto max_fanin_size = ClockedLayout::max_fanin_size;                // NOLINT(*-identifier-naming)

    using base_type = gate_level_layout;
    using node      = uint32_t;
    using signal    = uint64_t;

    using event_storage = std::shared_ptr<mockturtle::network_events<base_type>>;

    /*! \brief tile-based layout storage container */
    using gate_level_layout_storage =
        mockturtle::storage<gate_level_layout_storage_node, gate_level_layout_storage_data<node, signal>>;

    using storage = std::shared_ptr<gate_level_layout_storage>;

    /**
     * Standard constructor. Creates a named gate-level layout of the given aspect ratio. To this end, it calls
     * `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param name Layout name.
     */
    explicit gate_level_layout(const typename ClockedLayout::aspect_ratio& ar = {}, const std::string& name = {}) :
            ClockedLayout(ar),
            strg{std::make_shared<gate_level_layout_storage>()},
            evnts{std::make_shared<typename event_storage::element_type>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");

        initialize_truth_table_cache();
        strg->data.layout_name = name;
    }
    /**
     * Standard constructor. Creates a gate-level layout of the given aspect ratio and clocks it via the given clocking
     * scheme. To this end, it calls `ClockedLayout`'s standard constructor.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     * @param name Layout name.
     */
    gate_level_layout(const typename ClockedLayout::aspect_ratio& ar, const clocking_scheme<tile>& scheme,
                      const std::string& name = {}) :
            ClockedLayout(ar, scheme),
            strg{std::make_shared<gate_level_layout_storage>()},
            evnts{std::make_shared<typename event_storage::element_type>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");

        initialize_truth_table_cache();
        strg->data.layout_name = name;
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another gate_level_layout.
     */
    explicit gate_level_layout(storage s) :
            strg{std::move(s)},
            evnts{std::make_shared<typename event_storage::element_type>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another gate_level_layout.
     * @param e Event storage of another gate_level_layout.
     */
    gate_level_layout(storage s, event_storage e) : strg{std::move(s)}, evnts{std::move(e)}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Copy constructor from another `ClockedLayout`.
     *
     * @param lyt Clocked layout.
     */
    explicit gate_level_layout(const ClockedLayout& lyt) :
            ClockedLayout(lyt),
            strg{std::make_shared<gate_level_layout_storage>()},
            evnts{std::make_shared<typename event_storage::element_type>()}
    {
        static_assert(is_clocked_layout_v<ClockedLayout>, "ClockedLayout is not a clocked layout type");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] gate_level_layout clone() const noexcept
    {
        gate_level_layout copy{ClockedLayout::clone()};
        copy.strg  = std::make_shared<gate_level_layout_storage>(*strg);
        copy.evnts = std::make_shared<mockturtle::network_events<base_type>>(*evnts);

        return copy;
    }

#pragma endregion

#pragma region Primary I / O and constants

    [[nodiscard]] signal get_constant(bool value = false) const noexcept
    {
        // tiles reserved for constants: const0 = (1,0,0,0), const1 = (1,1,0,0)
        return value ? strg->data.const1 : strg->data.const0;
    }

    [[nodiscard]] bool is_constant(const node n) const noexcept
    {
        return n <= 1;
    }

    [[nodiscard]] bool constant_value(const node n) const noexcept
    {
        return n == 1;
    }

    signal create_pi(const std::string& name = {}, const tile& t = {})
    {
        const auto n = static_cast<node>(strg->nodes.size());
        strg->nodes.emplace_back();     // empty node data
        strg->nodes[n].data[1].h1 = 2;  // assign identity function
        strg->inputs.emplace_back(n);
        strg->data.node_names[n] = name.empty() ? fmt::format("pi{}", num_pis()) : name;
        assign_node(t, n);

        return static_cast<signal>(t);
    }

    signal create_po(const signal& s, [[maybe_unused]] const std::string& name = {}, const tile& t = {})
    {
        const auto n = static_cast<node>(strg->nodes.size());
        strg->nodes.emplace_back();     // empty node data
        strg->nodes[n].data[1].h1 = 2;  // assign identity function
        strg->outputs.emplace_back(static_cast<signal>(t));
        strg->data.node_names[n] = name.empty() ? fmt::format("po{}", num_pos()) : name;
        assign_node(t, n);

        /* increase ref-count to child */
        strg->nodes[get_node(s)].data[0].h1++;
        strg->nodes[n].children.push_back(s);

        return static_cast<signal>(t);
    }

    [[nodiscard]] bool is_pi(const node n) const noexcept
    {
        return std::find(strg->inputs.cbegin(), strg->inputs.cend(), n) != strg->inputs.cend();
    }
    [[nodiscard]] bool is_ci(const node n) const noexcept
    {
        return is_pi(n);
    }
    /**
     * Check whether tile `t` hosts a primary input.
     *
     * @param t Tile to be checked.
     * @return `true` iff the node located at tile `t` is a PI.
     */
    [[nodiscard]] bool is_pi_tile(const tile& t) const noexcept
    {
        return is_pi(get_node(t));
    }

    [[nodiscard]] bool is_po(const node n) const noexcept
    {
        return std::find_if(strg->outputs.cbegin(), strg->outputs.cend(),
                            [this, &n](const auto& p) { return this->get_node(p.index) == n; }) != strg->outputs.cend();
    }

    [[nodiscard]] bool is_co(const node n) const noexcept
    {
        return is_po(n);
    }
    /**
     * Check whether tile `t` hosts a primary output.
     *
     * @param t Tile to be checked.
     * @return `true` iff the node located at tile `t` is a PO.
     */
    [[nodiscard]] bool is_po_tile(const tile& t) const noexcept
    {
        return is_po(get_node(t));
    }

    [[nodiscard]] node pi_at(const uint32_t index) const noexcept
    {
        assert(index < num_pis());
        return static_cast<node>(strg->inputs[index]);
    }

    [[nodiscard]] signal po_at(const uint32_t index) const noexcept
    {
        assert(index < num_pos());
        return strg->outputs[index].index;
    }

    [[nodiscard]] bool is_combinational() const noexcept
    {
        return true;
    }

#pragma endregion

#pragma region node names

    void set_layout_name(const std::string& name) noexcept
    {
        strg->data.layout_name = name;
    }

    [[nodiscard]] std::string get_layout_name() const noexcept
    {
        return strg->data.layout_name;
    }

    void set_name(const node n, const std::string& name) noexcept
    {
        strg->data.node_names[n] = name;
    }

    void set_name(const signal s, const std::string& name) noexcept
    {
        set_name(get_node(s), name);
    }

    [[nodiscard]] std::string get_name(const node n) const noexcept
    {
        if (auto it = strg->data.node_names.find(n); it != strg->data.node_names.cend())
        {
            return it->second;
        }

        return {};
    }

    [[nodiscard]] std::string get_name(const signal s) const noexcept
    {
        return get_name(get_node(s));
    }

    [[nodiscard]] bool has_name(const node n) const noexcept
    {
        return !get_name(n).empty();
    }

    [[nodiscard]] bool has_name(const signal s) const noexcept
    {
        return !get_name(s).empty();
    }

    void set_input_name(const uint32_t index, const std::string& name) noexcept
    {
        if (index < num_pis())
        {
            strg->data.node_names[static_cast<node>(strg->inputs[index])] = name;
        }
    }

    [[nodiscard]] std::string get_input_name(const uint32_t index) const noexcept
    {
        if (index < num_pis())
        {
            return get_name(static_cast<node>(strg->inputs[index]));
        }

        return {};
    }

    [[nodiscard]] bool has_input_name(const uint32_t index) const noexcept
    {
        return !get_input_name(index).empty();
    }

    void set_output_name(const uint32_t index, const std::string& name) noexcept
    {
        if (index < num_pos())
        {
            strg->data.node_names[get_node(strg->outputs[index].index)] = name;
        }
    }

    [[nodiscard]] std::string get_output_name(const uint32_t index) const noexcept
    {
        if (index < num_pos())
        {
            return get_name(get_node(strg->outputs[index].index));
        }

        return {};
    }

    [[nodiscard]] bool has_output_name(const uint32_t index) const noexcept
    {
        return !get_output_name(index).empty();
    }

#pragma endregion

#pragma region Create function tiles

    signal create_buf(signal const& a, const tile& t = {})
    {
        return create_node_from_literal({a}, 2, t);
    }

    signal create_not(signal const& a, const tile& t = {})
    {
        return create_node_from_literal({a}, 3, t);
    }

    signal create_and(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 4, t);
    }

    signal create_nand(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 5, t);
    }

    signal create_or(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 6, t);
    }

    signal create_nor(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 7, t);
    }

    signal create_lt(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 8, t);
    }

    signal create_ge(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 9, t);
    }

    signal create_gt(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 10, t);
    }

    signal create_le(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 11, t);
    }

    signal create_xor(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 12, t);
    }

    signal create_xnor(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 13, t);
    }

    signal create_maj(signal a, signal b, signal c, const tile& t = {})
    {
        return create_node_from_literal({a, b, c}, 14, t);
    }

    signal create_node(const std::vector<signal>& children, const kitty::dynamic_truth_table& function,
                       const tile& t = {})
    {
        if (children.empty())
        {
            assert(function.num_vars() == 0u);
            return get_constant(!kitty::is_const0(function));
        }

        return create_node_from_literal(children, strg->data.fn_cache.insert(function), t);
    }

#pragma endregion

#pragma region Functional properties

    [[nodiscard]] kitty::dynamic_truth_table node_function(const node n) const
    {
        return strg->data.fn_cache[strg->nodes[n].data[1].h1];
    }

#pragma endregion

#pragma region Structural properties

    /**
     * Does NOT return the layout dimensions but the number of nodes (including constants and dead ones) in accordance
     * with the `mockturtle` API.
     *
     * @return Number of all nodes.
     */
    [[nodiscard]] auto size() const noexcept
    {
        return static_cast<uint32_t>(strg->nodes.size());
    }

    [[nodiscard]] auto num_cis() const noexcept
    {
        return num_pis();
    }

    [[nodiscard]] auto num_pis() const noexcept
    {
        return strg->inputs.size();
    }

    [[nodiscard]] auto num_cos() const noexcept
    {
        return num_pos();
    }

    [[nodiscard]] auto num_pos() const noexcept
    {
        return strg->outputs.size();
    }

    [[nodiscard]] uint32_t num_latches() const
    {
        return 0u;
    }

    [[nodiscard]] uint32_t num_registers() const
    {
        return 0u;
    }
    /**
     * Returns the number of placed nodes in the layout that do not compute the identity function.
     *
     * @return Number of gates in the layout.
     */
    [[nodiscard]] auto num_gates() const noexcept
    {
        return strg->data.num_gates;
    }
    /**
     * Returns the number of placed nodes in the layout that compute the identity function including PIs and POs.
     *
     * @return Number of wires in the layout.
     */
    [[nodiscard]] auto num_wires() const noexcept
    {
        return strg->data.num_wires;
    }
    /**
     * Returns the number of placed nodes in the layout that compute the identity function and cross other nodes.
     *
     * @return Number of crossings in the layout.
     */
    [[nodiscard]] auto num_crossings() const noexcept
    {
        return strg->data.num_crossings;
    }
    /**
     * Checks whether there are no gates or wires assigned to the layout's coordinates.
     *
     * @return `true` iff the layout is empty.
     */
    [[nodiscard]] bool is_empty() const noexcept
    {
        return num_gates() + num_wires() == 0;
    }
    /**
     * Returns the number of incoming, adjacently placed, and properly clocked signals to the given node.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param n Node to check.
     * @return Number of fanins to `n`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] auto fanin_size(const node n) const
    {
        uint32_t fin_size{0u};
        auto     fanin_counter = [&fin_size](auto const&) { ++fin_size; };

        foreach_fanin<decltype(fanin_counter), RespectClocking>(n, std::move(fanin_counter));

        return fin_size;
    }
    /**
     * Returns the number of outgoing, adjacently placed, and properly clocked signals of the given node.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param n Node to check.
     * @return Number of fanouts to `n`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] auto fanout_size(const node n) const
    {
        uint32_t fout_size{0u};
        auto     fanout_counter = [&fout_size](auto const&) { ++fout_size; };

        foreach_fanout<decltype(fanout_counter), RespectClocking>(n, std::move(fanout_counter));

        return fout_size;
    }

#pragma endregion

#pragma region Nodes and signals

    /**
     * Fetches the node that is placed onto a tile pointed to by a given signal. If no node is placed there, the
     * `const0` node is returned.
     *
     * @param s Pointer to a tile.
     * @return Node at position `t` where `s` points at `t`; or 0 if no node is placed at `t`.
     */
    [[nodiscard]] node get_node(const signal& s) const noexcept
    {
        if (const auto it = strg->data.tile_node_map.find(s); it != strg->data.tile_node_map.cend())
        {
            return it->second;
        }

        return 0;
    }
    /**
     * Fetches the node that is placed onto the provided tile If no node is placed there, the `const0` node is returned.
     *
     * @param t Tile in the layout.
     * @return Node at position `t`; or 0 if no node is placed at `t`.
     */
    [[nodiscard]] node get_node(const tile& t) const noexcept
    {
        return get_node(static_cast<signal>(t));
    }
    /**
     * The inverse function of `get_node`. Fetches the tile that the provided node is placed on. Returns a default dead
     * tile if the node is not placed.
     *
     * @param n Node whose location is desired.
     * @return Tile at which `n` is placed or a default dead tile if `n` is not placed.
     */
    [[nodiscard]] tile get_tile(const node n) const noexcept
    {
        if (auto it = strg->data.node_tile_map.find(n); it != strg->data.node_tile_map.cend())
        {
            return static_cast<tile>(it->second);
        }

        return {};
    }
    /**
     * Checks whether a node (not its assigned tile) is dead. Nodes can be dead for a variety of reasons. For instance
     * if they are dangling (see the `mockturtle` API). In this layout type, nodes are also marked dead when they are
     * not assigned to a tile (which is considered equivalent to dangling).
     *
     * @param n Node to check for liveliness.
     * @return `true` iff `n` is dead.
     */
    [[nodiscard]] bool is_dead(const node n) const noexcept
    {
        return static_cast<bool>((strg->nodes[n].data[0].h1 >> 31) & 1);
    }
    /**
     * Invokes the same behavior as `get_tile(n)` but additionally casts the return value to a signal. That is, this
     * function returns the signal representation of the tile that the node `n` is assigned to.
     *
     * @param n Node whose signal is desired.
     * @return Signal that points to `n`.
     */
    [[nodiscard]] signal make_signal(const node n) const noexcept
    {
        return static_cast<signal>(get_tile(n));
    }
    /**
     * Moves a given node to a new position and also updates its children, i.e., incoming signals.
     *
     * @param n Node to move.
     * @param t Tile to move `n` to.
     * @param new_children New incoming signals to `n`.
     * @return Signal pointing to `n`'s new tile.
     */
    signal move_node(const node n, const tile& t, const std::vector<signal>& new_children = {}) noexcept
    {
        // n's current position
        const auto old_t = get_tile(n);
        // n's children
        auto& children = strg->nodes[n].children;
        // decrease ref-count of children
        std::for_each(children.cbegin(), children.cend(),
                      [this](const auto& c) { strg->nodes[get_node(c.index)].data[0].h1--; });
        // clear n's children
        children.clear();

        // clear old_t only if it is different from t (this function can also be used to simply update n's children)
        if (t != old_t)
        {
            if (!t.is_dead())
            {
                // if n lived on a tile that was marked as PO, update it with the new tile t
                std::replace(strg->outputs.begin(), strg->outputs.end(), static_cast<signal>(old_t),
                             static_cast<signal>(t));
            }

            // clear n's position
            clear_tile(old_t);
            // assign n to its new position
            assign_node(t, n);
            // since clear_tile marks n as dead, it has to be revived
            revive_node(n);
        }

        // assign new children
        std::copy(new_children.cbegin(), new_children.cend(), std::back_inserter(children));
        // increase ref-count to new children
        std::for_each(new_children.cbegin(), new_children.cend(),
                      [this](const auto& nc) { strg->nodes[get_node(nc)].data[0].h1++; });

        return static_cast<signal>(t);
    }
    /**
     * Connects the given signal `s` to the given node `n` as a child. The new child `s` is appended at the end of `n`'s
     * list of children. Thus, if the order of children is important, `move_node()` should be used instead. Otherwise,
     * this function has a smaller overhead and is to be preferred.
     *
     * @param s New incoming signal to `n`.
     * @param n Node that should add `s` as its child.
     * @return Signal pointing to `n`.
     */
    signal connect(const signal& s, const node n) noexcept
    {
        if (!is_constant(n))
        {
            strg->nodes[n].children.push_back(s);
        }

        return make_signal(n);
    }
    /**
     * Removes all assigned nodes from the given tile and marks them as dead.
     *
     * @note This function does not reduce the number of nodes in the layout nor does it reduce the number of PIs
     * that are being returned via `num_pis()` even if the tile to clear is an input tile. However, the number of POs is
     * reduced if the tile to clear is an output tile. While this seems counter-intuitive and inconsistent, it is in
     * line with mockturtle's understanding of nodes and primary outputs.
     *
     * @param t Tile whose nodes are to be removed.
     */
    void clear_tile(const tile& t) noexcept
    {
        if (const auto it = strg->data.tile_node_map.find(static_cast<signal>(t)); it != strg->data.tile_node_map.end())
        {
            const auto n = it->second;

            if (!t.is_dead())
            {
                // decrease wire count
                if (is_wire(n))
                {
                    strg->data.num_wires--;

                    // decrease crossing count
                    if (ClockedLayout::is_crossing_layer(t) && !is_empty_tile(ClockedLayout::below(t)))
                    {
                        strg->data.num_crossings--;
                    }

                    if (ClockedLayout::is_ground_layer(t) &&
                        ClockedLayout::is_crossing_layer(ClockedLayout::above(t)) &&
                        !is_empty_tile(ClockedLayout::above(t)))
                    {
                        strg->data.num_crossings--;
                    }

                    // find PO entry and remove it if present
                    if (const auto po_it =
                            std::find_if(strg->outputs.cbegin(), strg->outputs.cend(),
                                         [this, &n](const auto& p) { return this->get_node(p.index) == n; });
                        po_it != strg->outputs.cend())
                    {
                        strg->outputs.erase(po_it);
                    }
                }
                else  // decrease gate count
                {
                    strg->data.num_gates--;
                }
            }
            // mark node as dead
            kill_node(n);

            // remove node-tile
            strg->data.node_tile_map.erase(n);
            // remove tile-node
            strg->data.tile_node_map.erase(it);
        }
    }
    /**
     * Necessary function in the `mockturtle` API. However, in this layout type, signals cannot be complemented.
     *
     * @param s Signal to check.
     * @return `false`.
     */
    [[nodiscard]] bool is_complemented([[maybe_unused]] const signal& s) const noexcept
    {
        return false;
    }

    [[nodiscard]] uint32_t node_to_index(const node n) const noexcept
    {
        return static_cast<uint32_t>(n);
    }

    [[nodiscard]] node index_to_node(const uint32_t index) const noexcept
    {
        return index;
    }
    /**
     * Returns whether a given node is a gate in accordance with `mockturtle`'s definition, i.e., whether it not a
     * constant and not a PI. Thereby, any wire/buffer (including POs) is a gate if this function is used to check for
     * it. This poses an inconsistency but is required to comply with certain `mockturtle` algorithms.
     *
     * @param n Node to check.
     * @return `true` iff `n` is neither a constant nor a PI.
     */
    [[nodiscard]] bool is_gate(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 >= 2 && !is_pi(n);
    }
    /**
     * Returns whether `n` computes the identity function.
     *
     * @param n Node to check.
     * @return `true` iff `n` computes the identity.
     */
    [[nodiscard]] bool is_buf(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 2;
    }
    /**
     * Equivalent to `is_buf`.
     */
    [[nodiscard]] bool is_wire(const node n) const noexcept
    {
        return is_buf(n);
    }
    /**
     * Returns whether `n` computes the binary inversion (NOT gate).
     *
     * @param n Node to check.
     * @return `true` iff `n` is a NOT gate.
     */
    [[nodiscard]] bool is_inv(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 3;
    }

    [[nodiscard]] bool is_and(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 4;
    }

    [[nodiscard]] bool is_nand(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 5;
    }

    [[nodiscard]] bool is_or(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 6;
    }

    [[nodiscard]] bool is_nor(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 7;
    }

    [[nodiscard]] bool is_lt(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 8;
    }

    [[nodiscard]] bool is_ge(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 9;
    }

    [[nodiscard]] bool is_gt(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 10;
    }

    [[nodiscard]] bool is_le(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 11;
    }

    [[nodiscard]] bool is_xor(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 12;
    }

    [[nodiscard]] bool is_xnor(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 13;
    }

    [[nodiscard]] bool is_maj(const node n) const noexcept
    {
        return strg->nodes[n].data[1].h1 == 14;
    }
    /**
     * Returns whether `n` is a wire and has multiple outputs, thereby, acting as a fanout gate. Note that a fanout will
     * return `true` for both `is_wire` and `is_fanout`.
     *
     * @param n Node to check.
     * @return `true` iff `n` is a fanout gate.
     */
    [[nodiscard]] bool is_fanout(const node n) const noexcept
    {
        return is_wire(n) && fanout_size(n) > 1;
    }
    /**
     * Returns whether `n`ode `n` computes a function. That is, this function returns `true` iff `n` is not a constant.
     *
     * @param n Node to check.
     * @return `true` iff `n` is not a constant.
     */
    [[nodiscard]] bool is_function(const node n) const
    {
        return n > 1;
    }
    /**
     * Returns whether the node assigned to `t` fulfills `is_gate` (in accordance with `mockturtle`'s definition of
     * gates).
     *
     * @param t Tile to check.
     * @return `true` iff `t` hosts a node that is a neither a constant nor a PI.
     */
    [[nodiscard]] bool is_gate_tile(const tile& t) const noexcept
    {
        return is_gate(get_node(static_cast<signal>(t)));
    }
    /**
     * Returns whether the node assigned to `t` fulfills `is_wire`.
     *
     * @param t Tile to check.
     * @return `true` iff `t` hosts a node that computes the identity.
     */
    [[nodiscard]] bool is_wire_tile(const tile& t) const noexcept
    {
        return is_wire(get_node(static_cast<signal>(t)));
    }
    /**
     * Returns whether `t` does not have a node assigned to it.
     *
     * @param t Tile to check.
     * @return `true` iff `t` is an empty tile.
     */
    [[nodiscard]] bool is_empty_tile(const tile& t) const noexcept
    {
        return !get_node(static_cast<signal>(t));
    }

#pragma endregion

#pragma region Iteration

    /**
     * Applies a function to all primary input nodes (including dead ones) in the layout.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @param fn Functor to apply to each primary input node.
     */
    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        using iterator_type = decltype(strg->inputs.cbegin());
        mockturtle::detail::foreach_element_transform<iterator_type, node>(
            strg->inputs.cbegin(), strg->inputs.cend(), [](const auto& i) { return static_cast<node>(i); }, fn);
    }
    /**
     * Applies a function to all primary output signals (including those that point to dead nodes) in the layout. Note
     * the difference to `foreach_pi` in the signature of `fn`. This function applies to all POs as signals whereas
     * `foreach_pi` applies to all PIs as nodes. This is with respect to `mockturtle`'s API.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @param fn Functor to apply to each primary output signal.
     */
    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        using iterator_type = decltype(strg->outputs.cbegin());
        mockturtle::detail::foreach_element_transform<iterator_type, signal>(
            strg->outputs.cbegin(), strg->outputs.end(), [](const auto& o) { return o.index; }, fn);
    }
    /**
     * Applies a function to all nodes (excluding dead ones) in the layout.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_if`.
     * @param fn Functor to apply to each node that is not dead.
     */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        auto r = mockturtle::range<node>(static_cast<node>(strg->nodes.size()));
        mockturtle::detail::foreach_element_if(r.begin(), r.end(), [this](const auto& n) { return !is_dead(n); }, fn);
    }
    /**
     * Applies a function to all gates (excluding dead ones) in the layout. Uses `is_gate` to check whether a node is a
     * gate.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_if`.
     * @param fn Functor to apply to each gate that is not dead.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        auto r = mockturtle::range<node>(2u, static_cast<node>(strg->nodes.size()));  // start from 2 to avoid constants
        mockturtle::detail::foreach_element_if(
            r.begin(), r.end(), [this](const auto n) { return is_gate(n) && !is_dead(n); }, fn);
    }
    /**
     * Applies a function to all wires (excluding dead ones) in the layout. Uses `is_wire` to check whether a node is a
     * wire.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_if`.
     * @param fn Functor to apply to each wire that is not dead.
     */
    template <typename Fn>
    void foreach_wire(Fn&& fn) const
    {
        auto r = mockturtle::range<node>(2u, static_cast<node>(strg->nodes.size()));  // start from 2 to avoid constants
        mockturtle::detail::foreach_element_if(
            r.begin(), r.end(), [this](const auto n) { return is_wire(n) && !is_dead(n); }, fn);
    }
    /**
     * Applies a function to all nodes that are incoming to a given one. Thereby, only incoming clocked zones (+/- one
     * layer to include crossings) are being considered whose data flow connections are respectively established. That
     * is, the given function is applied to all nodes that are connected to the one assigned to `t` as fanins on
     * neighboring tiles.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param n Node whose fanins are desired.
     * @param fn Functor to apply to each of `n`'s fanins.
     */
    template <typename Fn, bool RespectClocking = true>
    void foreach_fanin(const node n, Fn&& fn) const
    {
        if (n <= 1)
        {  // const-0 or const-1
            return;
        }

        const auto nt = get_tile(n);

        using iterator_type = decltype(strg->nodes[n].children.cbegin());
        mockturtle::detail::foreach_element_if_transform<iterator_type, signal>(
            strg->nodes[n].children.cbegin(), strg->nodes[n].children.cend(),
            [this, &nt](const auto& c)
            {
                const auto ct = get_tile(get_node(c.index));

                if constexpr (RespectClocking)
                {
                    return ClockedLayout::is_adjacent_elevation_of(nt, ct) &&
                           ClockedLayout::is_incoming_clocked(nt, ct);
                }
                else
                {
                    return ClockedLayout::is_adjacent_elevation_of(nt, ct);
                }
            },
            [this](const auto& c) -> signal { return make_signal(get_node(c.index)); }, std::forward<Fn>(fn));
    }
    /**
     * Returns a container that contains all tiles that feed information to the given one. Thereby, only
     * incoming clocked zones (+/- one layer to include crossings) are being considered whose data flow connections are
     * respectively established. That is, the returned container contains all tiles that host nodes that are connected
     * to the one assigned to `t` as fanins.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Tile whose incoming data flow ones are desired.
     * @return A container that contains all of `t`'s incoming data flow tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] auto incoming_data_flow(const tile& t) const noexcept
    {
        std::vector<tile> data_flow{};
        data_flow.reserve(ClockedLayout::get_clocking_scheme().max_in_degree);  // reserve memory

        auto fanin_collector = [&data_flow](const auto& fin) { data_flow.push_back(static_cast<tile>(fin)); };

        foreach_fanin<decltype(fanin_collector), RespectClocking>(get_node(t), std::move(fanin_collector));

        return data_flow;
    }
    /**
     * Applies a function to all nodes that are outgoing from a given one. Thereby, only outgoing clocked zones (+/- one
     * layer to include crossings) are being considered whose data flow connections are respectively established. That
     * is, the given function is applied to all nodes that are connected to the one assigned to `t` as fanouts on
     * neighboring tiles.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by
     * `mockturtle::foreach_element_transform`.
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param n Node whose fanouts are desired.
     * @param fn Functor to apply to each of `n`'s fanouts.
     */
    template <typename Fn, bool RespectClocking = true>
    void foreach_fanout(const node n, Fn&& fn) const
    {
        if (n <= 1)
        {  // const-0 or const-1
            return;
        }

        const auto nt = get_tile(n);

        auto fanout_collector = [this, &fn, &nt](const auto& out_t)
        {
            const auto apply_functor = [this, &fn](const auto& parent_t)
            {
                const auto parent_index = node_to_index(parent_t);
                auto       parents      = mockturtle::range(parent_index, parent_index + 1);
                using iterator_type     = decltype(parents.begin());
                mockturtle::detail::foreach_element_transform<iterator_type, node>(
                    parents.begin(), parents.end(), [this](const auto& p) -> node { return index_to_node(p); },
                    std::forward<Fn>(fn));
            };

            const auto apply_if_parent = [this, &nt, &apply_functor](const auto& adj_t)
            {
                if (const auto adj_n = get_node(adj_t); is_child(adj_n, static_cast<signal>(nt)))
                {
                    apply_functor(adj_n);
                }
            };

            apply_if_parent(out_t);

            if (const auto above_t = ClockedLayout::above(out_t); above_t != out_t)
            {
                apply_if_parent(above_t);
            }
            if (const auto below_t = ClockedLayout::below(out_t); below_t != out_t)
            {
                apply_if_parent(below_t);
            }
        };

        if constexpr (RespectClocking)
        {
            ClockedLayout::foreach_outgoing_clocked_zone(nt, std::move(fanout_collector));
        }
        else
        {
            ClockedLayout::foreach_adjacent_coordinate(nt, std::move(fanout_collector));
        }
    }
    /**
     * Returns a container that contains all tiles that accept information from the given one. Thereby,
     * only outgoing clocked zones (+/- one layer to include crossings) are being considered whose data flow connections
     * are respectively established. That is, the returned container contains all tiles that host nodes that are
     * connected to the one assigned to `t` as fanouts.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Tile whose outgoing data flow ones are desired.
     * @return A container that contains all of `t`'s outgoing data flow tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] auto outgoing_data_flow(const tile& t) const noexcept
    {
        std::vector<tile> data_flow{};
        data_flow.reserve(RespectClocking ? ClockedLayout::get_clocking_scheme().max_out_degree :
                                            ClockedLayout::max_fanin_size);  // reserve memory

        const auto fanout_collector = [this, &data_flow](const auto& fout) { data_flow.push_back(get_tile(fout)); };

        foreach_fanout<decltype(fanout_collector), RespectClocking>(get_node(t), std::move(fanout_collector));

        return data_flow;
    }

    template <typename Fn>
    void foreach_ci(Fn&& fn) const
    {
        foreach_pi(fn);
    }

    template <typename Fn>
    void foreach_co(Fn&& fn) const
    {
        foreach_po(fn);
    }

#pragma endregion

#pragma region Simulate values

    template <typename Iterator>
    mockturtle::iterates_over_t<Iterator, bool> compute(const node n, Iterator begin, Iterator end) const
    {
        uint32_t index{0};
        while (begin != end)
        {
            index <<= 1u;
            index ^= *begin++ ? 1 : 0;
        }

        return kitty::get_bit(strg->data.cache[strg->nodes[n].data[1].h1], index);
    }

    template <typename Iterator>
    mockturtle::iterates_over_truth_table_t<Iterator> compute(const node n, Iterator begin, Iterator end) const
    {
        std::vector<typename Iterator::value_type> tts{begin, end};

        const auto num_fanin = fanin_size(n);
        assert(tts.size() == num_fanin);
        assert(num_fanin != 0ul);

        /* resulting truth table has the same size as any of the children */
        auto       result  = tts.front().construct();
        const auto gate_tt = strg->data.fn_cache[strg->nodes[n].data[1].h1];

        for (uint32_t i = 0u; i < static_cast<uint32_t>(result.num_bits()); ++i)
        {
            uint32_t pattern = 0u;
            for (uint32_t j = 0u; j < num_fanin; ++j)
            {
                pattern |= static_cast<uint32_t>(kitty::get_bit(tts[j], i)) << j;
            }

            if (kitty::get_bit(gate_tt, pattern))
            {
                kitty::set_bit(result, i);
            }
        }

        return result;
    }

#pragma endregion

#pragma region Cardinal operations

    /**
     * Checks whether signal `s` is incoming to tile `t`. That is, whether tile `t` hosts a node that has a fanin
     * assigned to the tile that signal `s` points to.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @param s Signal pointing to a potential incoming tile to `t`.
     * @return `true` iff `s` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool is_incoming_signal(const tile& t, const signal& s) const noexcept
    {
        bool incoming_signal   = false;
        auto in_signal_checker = [this, &s, &incoming_signal](const auto& i)
        {
            if (const auto it = static_cast<tile>(i);
                i == s || ClockedLayout::above(it) == s || ClockedLayout::below(it) == s)
            {
                incoming_signal = true;
                return false;  // abort iteration
            }

            return true;  // keep looping
        };

        foreach_fanin<decltype(in_signal_checker), RespectClocking>(get_node(t), std::move(in_signal_checker));

        return incoming_signal;
    }
    /**
     * Checks whether the given tile has an incoming one in northern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_northern_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in north-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north_east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_eastern_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north_east(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_eastern_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::east(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in south-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south_east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_eastern_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south_east(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in southern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_southern_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in south-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south_west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_western_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south_west(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_western_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::west(t)));
    }
    /**
     * Checks whether the given tile has an incoming one in north-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north_west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_western_incoming_signal(const tile& t) const noexcept
    {
        return is_incoming_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north_west(t)));
    }
    /**
     * Checks whether the given tile has no incoming tiles.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `t` does not have incoming tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_no_incoming_signal(const tile& t) const noexcept
    {
        return fanin_size<RespectClocking>(get_node(t)) == 0u;
    }
    /**
     * Checks whether signal `s` is outgoing from tile `t`. That is, whether tile `t` hosts a node that has a fanout
     * assigned to the tile that signal `s` points to.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @param s Signal pointing to a potential outgoing tile of `t`.
     * @return `true` iff `s` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool is_outgoing_signal(const tile& t, const signal& s) const noexcept
    {
        bool outgoing_signal    = false;
        auto out_signal_checker = [this, &s, &outgoing_signal](const auto& o)
        {
            if (const auto ot = get_tile(o); ot == s || ClockedLayout::above(ot) == s || ClockedLayout::below(ot) == s)
            {
                outgoing_signal = true;
                return false;  // abort iteration
            }

            return true;  // keep looping
        };

        foreach_fanout<decltype(out_signal_checker), RespectClocking>(get_node(t), std::move(out_signal_checker));

        return outgoing_signal;
    }
    /**
     * Checks whether the given tile has an outgoing one in northern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_northern_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in north-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north_east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_eastern_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north_east(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_eastern_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::east(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in south-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south_east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_eastern_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south_east(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in southern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_southern_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in south-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south_west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_western_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::south_west(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_western_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::west(t)));
    }
    /**
     * Checks whether the given tile has an outgoing one in north-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north_west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_western_outgoing_signal(const tile& t) const noexcept
    {
        return is_outgoing_signal<RespectClocking>(t, static_cast<signal>(ClockedLayout::north_west(t)));
    }
    /**
     * Checks whether the given tile has no outgoing tiles.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `t` does not have outgoing tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_no_outgoing_signal(const tile& t) const noexcept
    {
        return fanout_size<RespectClocking>(get_node(t)) == 0u;
    }
    /**
     * Checks whether the given tile `t` has its incoming and outgoing signals on opposite sides of the tile. For this
     * purpose, the function relies on `foreach_adjacent_opposite_coordinates` of the underlying `ClockedLayout`.
     *
     * This function is very helpful for many gate libraries to check for (non-)straight gates, which might look
     * different.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins
     * and fanouts.
     * @param t Base tile.
     * @return `true` iff `t` has incoming and outgoing signals on opposite sides.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_opposite_incoming_and_outgoing_signals(const tile& t) const noexcept
    {
        auto opposite_signals = false;

        ClockedLayout::foreach_adjacent_opposite_coordinates(
            t,
            [this, &t, &opposite_signals](const auto& sp)
            {
                const auto s1 = static_cast<signal>(std::get<0>(sp)), s2 = static_cast<signal>(std::get<1>(sp));

                if ((is_incoming_signal<RespectClocking>(t, s1) && is_outgoing_signal<RespectClocking>(t, s2)) ||
                    (is_incoming_signal<RespectClocking>(t, s2) && is_outgoing_signal<RespectClocking>(t, s1)))
                {
                    opposite_signals = true;

                    return false;  // break loop
                }

                return true;  // continue looping
            });

        return opposite_signals;
    }

#pragma endregion

#pragma region Custom node values

    void clear_values() const noexcept
    {
        std::for_each(strg->nodes.begin(), strg->nodes.end(), [](auto& n) { n.data[0].h2 = 0; });
    }

    [[nodiscard]] uint32_t value(const node n) const
    {
        return strg->nodes[n].data[0].h2;
    }

    void set_value(const node n, uint32_t v) const
    {
        strg->nodes[n].data[0].h2 = v;
    }

    [[nodiscard]] uint32_t incr_value(const node n) const
    {
        return static_cast<uint32_t>(strg->nodes[n].data[0].h2++);
    }

    [[nodiscard]] uint32_t decr_value(const node n) const
    {
        return static_cast<uint32_t>(--strg->nodes[n].data[0].h2);
    }

#pragma endregion

#pragma region Visited flags

    void clear_visited() const
    {
        std::for_each(strg->nodes.begin(), strg->nodes.end(), [](auto& n) { n.data[1].h2 = 0; });
    }

    [[nodiscard]] auto visited(const node n) const
    {
        return strg->nodes[n].data[1].h2;
    }

    void set_visited(const node n, uint32_t v) const
    {
        strg->nodes[n].data[1].h2 = v;
    }

    [[nodiscard]] uint32_t trav_id() const
    {
        return strg->data.trav_id;
    }

    void incr_trav_id() const
    {
        strg->data.trav_id++;
    }

#pragma endregion

#pragma region General methods

    auto& events() const
    {
        return *evnts;
    }

#pragma endregion

  private:
    storage strg;

    event_storage evnts;

    template <typename>
    friend class detail::gate_level_drvs_impl;

    inline void initialize_truth_table_cache()
    {
        /* reserve the second node for constant 1 */
        strg->nodes.emplace_back();

        kitty::dynamic_truth_table tt_zero(0);
        strg->data.fn_cache.insert(tt_zero);

        strg->nodes[0].data[1].h1 = 0;
        strg->nodes[1].data[1].h1 = 1;

        /* reserve some truth tables for nodes */
        const auto create_and_cache = [this](const auto& literal, auto n)
        {
            kitty::dynamic_truth_table tt(static_cast<uint32_t>(n));
            kitty::create_from_words(tt, &literal, &literal + 1);
            strg->data.fn_cache.insert(tt);
        };

        static constexpr const uint64_t lit_not = 0x1, lit_and = 0x8, lit_or = 0xe, lit_lt = 0x2, lit_le = 0xb,
                                        lit_xor = 0x6, lit_maj = 0xe8;

        create_and_cache(lit_not, 1);  // since NOT is not normal, its complement, i.e., the identity, is stored
        create_and_cache(lit_and, 2);
        create_and_cache(lit_or, 2);
        create_and_cache(lit_lt, 2);  // since GE is not normal, it is covered as LT's complement
        create_and_cache(lit_le, 2);  // since GT is not normal, it is covered as LE's complement
        create_and_cache(lit_xor, 2);
        create_and_cache(lit_maj, 3);
    }

    void assign_node(const tile& t, const node n)
    {
        if (!t.is_dead())
        {
            clear_tile(t);

            strg->data.tile_node_map[static_cast<signal>(t)] = n;

            strg->data.node_tile_map[n] = static_cast<signal>(t);

            // keep track of number of gates and wire segments
            if (is_wire(n))
            {
                strg->data.num_wires++;

                if (ClockedLayout::is_crossing_layer(t) && !is_empty_tile(ClockedLayout::below(t)))
                {
                    strg->data.num_crossings++;
                }

                if (ClockedLayout::is_ground_layer(t) && ClockedLayout::is_crossing_layer(ClockedLayout::above(t)) &&
                    !is_empty_tile(ClockedLayout::above(t)))
                {
                    strg->data.num_crossings++;
                }
            }
            else  // is gate
            {
                strg->data.num_gates++;
            }
        }
    }

    void kill_node(const node n)
    {
        if (!is_constant(n))
        {
            strg->nodes[n].data[0].h1 |= UINT32_C(0x80000000);
        }
    }

    void revive_node(const node n)
    {
        if (!is_constant(n))
        {
            strg->nodes[n].data[0].h1 &= ~UINT32_C(0x80000000);
        }
    }

    signal create_node_from_literal(const std::vector<signal>& children, uint32_t literal, const tile& t)
    {
        typename storage::element_type::node_type node_data;
        std::copy(children.begin(), children.end(), std::back_inserter(node_data.children));
        node_data.data[1].h1 = literal;

        const auto n = static_cast<node>(strg->nodes.size());
        strg->nodes.push_back(node_data);

        /* increase ref-count to children */
        for (const auto& c : children)
        {
            strg->nodes[get_node(c)].data[0].h1++;
        }

        set_value(n, 0);

        assign_node(t, n);

        for (auto const& fn : evnts->on_add)
        {
            (*fn)(n);
        }

        return static_cast<signal>(t);
    }

    [[nodiscard]] bool is_child(const node n, const signal& s) const noexcept
    {
        const auto& node_data = strg->nodes[n];
        return std::find(node_data.children.cbegin(), node_data.children.cend(), s) != node_data.children.cend();
    }
};

}  // namespace fiction

#endif  // FICTION_GATE_LEVEL_LAYOUT_HPP
