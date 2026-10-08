/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Gate-level layout with clocking, synchronization, and obstructions.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/clocking_state.hpp"
#include "fiction/layouts/obstructions.hpp"

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/truth_table_cache.hpp>
#include <phmap.h>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction::layouts
{
/**
 * @brief Layout-local object identity. A removed object's generation cannot identify its replacement.
 *
 * Copies preserve identities; use an identity only with the layout that supplied it or its copy.
 * Generations detect slot reuse within that contents lifetime, not IDs from unrelated layouts.
 * Whole-layout assignment invalidates destination handles. Callers must not use transferred IDs with a moved-from
 * layout after its reuse.
 */
struct layout_object_id
{
    /** @brief Storage slot. */
    uint32_t index{};
    /** @brief Slot generation; zero does not identify a live object. */
    uint32_t generation{};
    /** @brief Compares object identities. */
    constexpr auto operator<=>(const layout_object_id&) const noexcept = default;
};
/** @brief Input endpoint. Its index is the truth-table argument index. */
struct layout_input_port
{
    /** @brief Destination object. */
    layout_object_id object{};
    /** @brief Truth-table argument index. */
    uint32_t index{};
    /** @brief Compares input endpoints. */
    constexpr auto operator<=>(const layout_input_port&) const noexcept = default;
};
}  // namespace fiction::layouts

namespace std
{
/** @brief Hashes both parts of an object identity. */
template <>
struct hash<fiction::layouts::layout_object_id>
{
    /** @brief Returns the identity's hash. */
    size_t operator()(const fiction::layouts::layout_object_id id) const noexcept
    {
        return hash<uint64_t>{}((static_cast<uint64_t>(id.generation) << 32u) | id.index);
    }
};
}  // namespace std

namespace fiction::layouts
{
/**
 * @brief Placed FCN objects, ordered ports, clocking, and obstructions.
 *
 * Objects have stable identities independent of their coordinates. Connections describe declared topology;
 * physical validation checks adjacency, clocking, and geometry separately. Copies own independent state.
 * Visitors may edit coordinates, names, and capabilities. Object and terminal visitors must not create
 * or remove objects, change terminal order, or replace the layout during traversal. Connection visitors
 * must also preserve the traversed input or sink connections, as specified on each visitor.
 * @tparam CoordinateLayout Coordinate geometry used for placement.
 */
template <typename CoordinateLayout>
class gate_level_layout : public CoordinateLayout
{
  public:
    /** @brief Coordinate identifying a clock zone. */
    using clock_zone = typename CoordinateLayout::coordinate;
    /** @brief Clocking scheme. */
    using clocking_scheme_t = clocking::scheme;
    /** @brief Clock phase. */
    using clock_number_t = typename clocking_scheme_t::clock_number;
    /** @brief Number of clocked neighbors. */
    using degree_t = uint8_t;
    /** @brief Hold extension in full cycles. */
    using sync_elem_t = clocking::state::sync_elem_t;
    /** @brief Placement coordinate. */
    using tile = typename CoordinateLayout::coordinate;
    /** @brief Stable layout-local identity. */
    using object_id = layout_object_id;
    /** @brief Ordered destination endpoint. */
    using input_port = layout_input_port;
    /** @brief Concrete layout type. */
    using base_type = gate_level_layout;

    /** @brief Creates an empty layout with the given geometry and name. */
    explicit gate_level_layout(const typename CoordinateLayout::extent& size = {}, std::string name = {})
        requires std::constructible_from<CoordinateLayout, const typename CoordinateLayout::extent&>
            : CoordinateLayout{size}, layout_name{std::move(name)}
    {
        initialize_functions();
    }
    /** @brief Creates an empty layout with the given geometry, clocking, and name. */
    gate_level_layout(const typename CoordinateLayout::extent& size, const clocking::scheme& scheme,
                      const std::string& name = {})
        requires std::constructible_from<CoordinateLayout, const typename CoordinateLayout::extent&>
            : gate_level_layout{size, name}
    {
        replace_clocking_scheme(scheme);
    }
    /** @brief Creates an empty layout with shifted rows or columns. */
    explicit gate_level_layout(const layouts::arrangement a, const typename CoordinateLayout::extent& size = {},
                               std::string name = {})
        requires std::constructible_from<CoordinateLayout, layouts::arrangement,
                                         const typename CoordinateLayout::extent&>
            : CoordinateLayout{a, size}, layout_name{std::move(name)}
    {
        initialize_functions();
    }
    /** @brief Creates an empty layout with shifted rows or columns and clocking. */
    gate_level_layout(const layouts::arrangement a, const typename CoordinateLayout::extent& size,
                      const clocking::scheme& scheme, const std::string& name = {})
        requires std::constructible_from<CoordinateLayout, layouts::arrangement,
                                         const typename CoordinateLayout::extent&>
            : gate_level_layout{a, size, name}
    {
        replace_clocking_scheme(scheme);
    }
    /** @brief Creates an empty layout with an independent copy of the geometry. */
    explicit gate_level_layout(const CoordinateLayout& geometry) : CoordinateLayout{geometry.clone()}
    {
        initialize_functions();
    }
    /** @brief Copies geometry, identities, connections, and owned capabilities independently. */
    gate_level_layout(const gate_level_layout& other) :
            CoordinateLayout{static_cast<const CoordinateLayout&>(other).clone()},
            objects{other.objects},
            spilled_inputs{other.spilled_inputs},
            edges{other.edges},
            occupancy{other.occupancy},
            functions{other.functions},
            names{other.names},
            inputs{other.inputs},
            outputs{other.outputs},
            clocking_state{other.clocking_state},
            obstruction_state{other.obstruction_state},
            layout_name{other.layout_name},
            free_object{other.free_object},
            free_edge{other.free_edge},
            live_count{other.live_count},
            wire_count{other.wire_count}
    {}
    /** @brief Replaces this layout with an independent value copy. */
    gate_level_layout& operator=(const gate_level_layout& other)
    {
        if (this != &other)
        {
            auto copy = other;
            *this     = std::move(copy);
        }
        return *this;
    }
    /** @brief Moves owned state and leaves an empty reusable source with its original geometry. */
    gate_level_layout(gate_level_layout&& other) noexcept :
            CoordinateLayout{static_cast<const CoordinateLayout&>(other).clone()}
    {
        swap_owned_state(other);
    }
    /** @brief Moves owned state and leaves an empty reusable source. */
    gate_level_layout& operator=(gate_level_layout&& other) noexcept
    {
        if (this != &other)
        {
            gate_level_layout moved{std::move(other)};
            static_cast<CoordinateLayout&>(*this) = std::move(static_cast<CoordinateLayout&>(moved));
            swap_owned_state(moved);
        }
        return *this;
    }
    /** @brief Releases owned layout state. */
    ~gate_level_layout() = default;
    /** @brief Returns an independent value copy. */
    [[nodiscard]] gate_level_layout clone() const
    {
        return *this;
    }

    /** @brief Creates a primary input at `t`. Occupied coordinates reject without mutation. */
    object_id create_pi(const std::string& name, const tile& t)
    {
        const auto p = create_object({}, 2, object_kind::PI, 0, t);
        try
        {
            set_name(p, name);
            inputs.push_back(p.index);
        }
        catch (...)
        {
            remove(p);
            throw;
        }
        return p;
    }
    /** @brief Creates a primary output driven by `s` at `t`. */
    object_id create_po(const object_id s, const std::string& name, const tile& t)
    {
        return create_terminal(std::array{s}, name, t);
    }
    /** @brief Creates a primary output with its input disconnected. */
    object_id create_po(const std::string& name, const tile& t)
    {
        return create_terminal({}, name, t);
    }
    /** @brief Creates a wire driven by `a`. */
    object_id create_buf(const object_id a, const tile& t)
    {
        return create_object(std::array{a}, 2, object_kind::WIRE, 1, t);
    }
    /** @brief Creates a wire with its input disconnected. */
    object_id create_buf(const tile& t)
    {
        return create_object({}, 2, object_kind::WIRE, 1, t);
    }
    /** @brief Creates a NOT gate. */
    object_id create_not(const object_id a, const tile& t)
    {
        return create_object(std::array{a}, 3, object_kind::GATE, 1, t);
    }
    /** @brief Creates a AND gate. */
    object_id create_and(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 4, object_kind::GATE, 2, t);
    }
    /** @brief Creates a NAND gate. */
    object_id create_nand(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 5, object_kind::GATE, 2, t);
    }
    /** @brief Creates a OR gate. */
    object_id create_or(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 6, object_kind::GATE, 2, t);
    }
    /** @brief Creates a NOR gate. */
    object_id create_nor(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 7, object_kind::GATE, 2, t);
    }
    /** @brief Creates a LT gate. */
    object_id create_lt(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 8, object_kind::GATE, 2, t);
    }
    /** @brief Creates a GE gate. */
    object_id create_ge(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 9, object_kind::GATE, 2, t);
    }
    /** @brief Creates a GT gate. */
    object_id create_gt(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 10, object_kind::GATE, 2, t);
    }
    /** @brief Creates a LE gate. */
    object_id create_le(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 11, object_kind::GATE, 2, t);
    }
    /** @brief Creates a XOR gate. */
    object_id create_xor(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 12, object_kind::GATE, 2, t);
    }
    /** @brief Creates a XNOR gate. */
    object_id create_xnor(const object_id a, const object_id b, const tile& t)
    {
        return create_object(std::array{a, b}, 13, object_kind::GATE, 2, t);
    }

    /** @brief Creates a majority gate. */
    object_id create_maj(const object_id a, const object_id b, const object_id c, const tile& t)
    {
        return create_object(std::array{a, b, c}, 14, object_kind::GATE, 3, t);
    }
    /**
     * @brief Creates a gate with an ordered truth table and initial input connections.
     *
     * Unspecified trailing inputs remain disconnected. Constant functions require an explicit placed object.
     * @throws std::invalid_argument If placement is occupied, or children exceed the function arity.
     */
    object_id create_gate(const std::vector<object_id>& children, const kitty::dynamic_truth_table& function,
                          const tile& t)
    {
        check_placement(t);
        if (children.size() > function.num_vars())
        {
            throw std::invalid_argument("Connections exceed the gate function's input count");
        }
        for (const auto child : children)
        {
            static_cast<void>(checked_object(child));
        }
        ensure_functions();
        const auto literal = functions.insert(function);
        return create_object(children, literal, literal == 2 ? object_kind::WIRE : object_kind::GATE,
                             function.num_vars(), t);
    }
    /** @brief Returns whether this identity names a live object. */
    [[nodiscard]] bool contains(const object_id id) const noexcept
    {
        return id.generation != 0 && id.index < objects.size() && objects[id.index].kind != object_kind::REMOVED &&
               objects[id.index].generation == id.generation;
    }
    /** @brief Finds the object at a coordinate; empty coordinates have no identity. */
    [[nodiscard]] std::optional<object_id> find_object(const tile& t) const noexcept
    {
        const auto it = occupancy.find(t);
        return it == occupancy.end() ? std::nullopt : std::optional{identity(it->second)};
    }
    /** @brief Returns the object's coordinate. @throws std::invalid_argument If the identity is stale. */
    [[nodiscard]] tile get_tile(const object_id id) const
    {
        return checked_object(id).position;
    }
    /** @brief Returns the truth table in logical input-index order. */
    [[nodiscard]] kitty::dynamic_truth_table object_function(const object_id id) const
    {
        return functions[checked_object(id).function];
    }
    /** @brief Returns the number of input slots, including disconnected slots. */
    [[nodiscard]] uint32_t input_count(const object_id id) const
    {
        return checked_object(id).input_count;
    }
    /** @brief Returns the declared source of an input, or no source if disconnected. */
    [[nodiscard]] std::optional<object_id> source(const input_port port) const
    {
        const auto edge = checked_input(port);
        return edge == NO_INDEX ? std::nullopt : std::optional{identity(edges[edge].source)};
    }
    /**
     * @brief Connects an output to an ordered input, replacing the input's existing source.
     *
     * Port and identity checks precede mutation. Adjacency, clocking, and geometry need not be valid during editing.
     */
    void connect(const object_id src, const input_port dst)
    {
        static_cast<void>(checked_object(src));
        const auto old_edge = checked_input(dst);
        if (old_edge != NO_INDEX && edges[old_edge].source == src.index)
        {
            return;
        }
        if (old_edge != NO_INDEX)
        {
            unlink_edge(old_edge);
        }
        const auto edge_id    = allocate_edge();
        auto&      edge       = edges[edge_id];
        auto&      src_object = objects[src.index];
        edge                  = {src.index, dst.object.index, dst.index, NO_INDEX, src_object.first_sink};
        if (edge.next != NO_INDEX)
        {
            edges[edge.next].previous = edge_id;
        }
        src_object.first_sink = edge_id;
        ++src_object.sink_count;
        input_edges(dst.object.index)[dst.index] = edge_id;
    }
    /** @brief Disconnects one input without changing the indices of other inputs. */
    void disconnect(const input_port port)
    {
        if (const auto edge = checked_input(port); edge != NO_INDEX)
        {
            unlink_edge(edge);
        }
    }
    /** @brief Moves an object without changing its identity or connections. */
    object_id move_object(const object_id id, const tile& t)
    {
        auto& object = checked_object(id);
        if (object.position == t)
        {
            return id;
        }
        check_placement(t);
        occupancy.emplace(t, id.index);
        occupancy.erase(object.position);
        object.position = t;
        return id;
    }
    /** @brief Removes an object and disconnects all inputs and sinks. Stale identities reject. */
    void remove(const object_id id)
    {
        auto& object = checked_object(id);
        for (const auto edge : input_edges(id.index))
        {
            if (edge != NO_INDEX)
            {
                unlink_edge(edge);
            }
        }
        while (object.first_sink != NO_INDEX)
        {
            unlink_edge(object.first_sink);
        }
        occupancy.erase(object.position);
        names.erase(id);
        if (object.kind == object_kind::PI)
        {
            std::erase(inputs, id.index);
        }
        if (object.kind == object_kind::PO)
        {
            std::erase(outputs, id.index);
        }
        --live_count;
        wire_count -= object.function == 2;
        spilled_inputs.erase(id.index);
        object.input_count = 0;
        object.kind        = object_kind::REMOVED;
        if (object.generation == std::numeric_limits<uint32_t>::max())
        {
            object.generation = 0;  // Exhausted generations retire the slot rather than revive a stale identity.
        }
        else
        {
            ++object.generation;
            object.first_sink = free_object;
            free_object       = id.index;
        }
    }
    /** @brief Removes the occupant of a coordinate if present. */
    void clear_tile(const tile& t)
    {
        if (const auto id = find_object(t))
        {
            remove(*id);
        }
    }
    /** @brief Counts live objects. */
    [[nodiscard]] uint32_t size() const noexcept
    {
        return live_count;
    }
    /** @brief Counts primary inputs. */
    [[nodiscard]] uint32_t num_pis() const noexcept
    {
        return static_cast<uint32_t>(inputs.size());
    }
    /** @brief Counts primary outputs. */
    [[nodiscard]] uint32_t num_pos() const noexcept
    {
        return static_cast<uint32_t>(outputs.size());
    }
    /** @brief Counts non-identity objects. */
    [[nodiscard]] uint32_t num_gates() const noexcept
    {
        return live_count - wire_count;
    }
    /** @brief Counts identity objects, including terminals. */
    [[nodiscard]] uint32_t num_wires() const noexcept
    {
        return wire_count;
    }
    /** @brief Returns whether the layout has no objects. */
    [[nodiscard]] bool is_empty() const noexcept
    {
        return live_count == 0;
    }
    /** @brief Counts crossing-layer wires above occupied ground-layer tiles. */
    [[nodiscard]] uint32_t num_crossings() const
    {
        uint32_t count{};
        foreach_wire(
            [&](const auto id)
            {
                const auto t = get_tile(id);
                count += t.z == 1 && find_object({t.x, t.y, 0}).has_value();
            });
        return count;
    }
    /** @brief Counts connected input slots, irrespective of physical legality. */
    [[nodiscard]] uint32_t fanin_size(const object_id id) const
    {
        static_cast<void>(checked_object(id));
        const auto ins = input_edges(id.index);
        return static_cast<uint32_t>(std::ranges::count_if(ins, [](const auto edge) { return edge != NO_INDEX; }));
    }
    /** @brief Counts sink input ports, including multiple ports on one object. */
    [[nodiscard]] uint32_t fanout_size(const object_id id) const
    {
        return checked_object(id).sink_count;
    }
    /** @brief Returns a primary input in declared interface order. */
    [[nodiscard]] object_id pi_at(const uint32_t index) const
    {
        return identity(inputs.at(index));
    }
    /** @brief Returns a primary output in declared interface order. */
    [[nodiscard]] object_id po_at(const uint32_t index) const
    {
        return identity(outputs.at(index));
    }
    /** @brief Sets the complete input permutation. Invalid orders reject without mutation. */
    void set_input_order(const std::span<const object_id> order)
    {
        set_terminal_order(inputs, order, object_kind::PI);
    }
    /** @brief Sets the complete output permutation. Invalid orders reject without mutation. */
    void set_output_order(const std::span<const object_id> order)
    {
        set_terminal_order(outputs, order, object_kind::PO);
    }
    /** @brief Returns whether an object is a primary input. */
    [[nodiscard]] bool is_pi(const object_id id) const
    {
        return checked_object(id).kind == object_kind::PI;
    }
    /** @brief Returns whether an object is a primary output. */
    [[nodiscard]] bool is_po(const object_id id) const
    {
        return checked_object(id).kind == object_kind::PO;
    }
    /** @brief Returns whether an object is a logic gate rather than a wire or terminal. */
    [[nodiscard]] bool is_gate(const object_id id) const
    {
        return checked_object(id).kind == object_kind::GATE;
    }
    /** @brief Returns whether an object computes the identity function. */
    [[nodiscard]] bool is_buf(const object_id id) const
    {
        return checked_object(id).function == 2;
    }
    /** @brief Returns whether an object computes the identity function. */
    [[nodiscard]] bool is_wire(const object_id id) const
    {
        return is_buf(id);
    }
    /** @brief Returns whether an identity object drives more than one input port. */
    [[nodiscard]] bool is_fanout(const object_id id) const
    {
        return is_wire(id) && fanout_size(id) > 1;
    }
    /** @brief Returns whether the object computes INV. */
    [[nodiscard]] bool is_inv(const object_id id) const
    {
        return checked_object(id).function == 3;
    }
    /** @brief Returns whether the object computes AND. */
    [[nodiscard]] bool is_and(const object_id id) const
    {
        return checked_object(id).function == 4;
    }
    /** @brief Returns whether the object computes NAND. */
    [[nodiscard]] bool is_nand(const object_id id) const
    {
        return checked_object(id).function == 5;
    }
    /** @brief Returns whether the object computes OR. */
    [[nodiscard]] bool is_or(const object_id id) const
    {
        return checked_object(id).function == 6;
    }
    /** @brief Returns whether the object computes NOR. */
    [[nodiscard]] bool is_nor(const object_id id) const
    {
        return checked_object(id).function == 7;
    }
    /** @brief Returns whether the object computes LT. */
    [[nodiscard]] bool is_lt(const object_id id) const
    {
        return checked_object(id).function == 8;
    }
    /** @brief Returns whether the object computes GE. */
    [[nodiscard]] bool is_ge(const object_id id) const
    {
        return checked_object(id).function == 9;
    }
    /** @brief Returns whether the object computes GT. */
    [[nodiscard]] bool is_gt(const object_id id) const
    {
        return checked_object(id).function == 10;
    }
    /** @brief Returns whether the object computes LE. */
    [[nodiscard]] bool is_le(const object_id id) const
    {
        return checked_object(id).function == 11;
    }
    /** @brief Returns whether the object computes XOR. */
    [[nodiscard]] bool is_xor(const object_id id) const
    {
        return checked_object(id).function == 12;
    }
    /** @brief Returns whether the object computes XNOR. */
    [[nodiscard]] bool is_xnor(const object_id id) const
    {
        return checked_object(id).function == 13;
    }
    /** @brief Returns whether the object computes MAJ. */
    [[nodiscard]] bool is_maj(const object_id id) const
    {
        return checked_object(id).function == 14;
    }
    /** @brief Returns whether the coordinate hosts a pi. */
    [[nodiscard]] bool is_pi_tile(const tile& t) const
    {
        const auto id = find_object(t);
        return id && is_pi(*id);
    }
    /** @brief Returns whether the coordinate hosts a po. */
    [[nodiscard]] bool is_po_tile(const tile& t) const
    {
        const auto id = find_object(t);
        return id && is_po(*id);
    }
    /** @brief Returns whether the coordinate hosts a gate. */
    [[nodiscard]] bool is_gate_tile(const tile& t) const
    {
        const auto id = find_object(t);
        return id && is_gate(*id);
    }
    /** @brief Returns whether the coordinate hosts a wire. */
    [[nodiscard]] bool is_wire_tile(const tile& t) const
    {
        const auto id = find_object(t);
        return id && is_wire(*id);
    }

    /** @brief Returns whether a coordinate has no occupant. */
    [[nodiscard]] bool is_empty_tile(const tile& t) const noexcept
    {
        return !find_object(t);
    }
    /**
     * @brief Visits live objects. Callbacks may accept an object and enumeration index and return false to stop.
     * Callbacks must not create or remove objects, change terminal order, or replace the layout.
     * Traversal scans retained storage slots, including removed objects.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_object(Fn&& fn) const
    {
        uint32_t index{};
        for (uint32_t slot{}; slot < objects.size(); ++slot)
        {
            if (objects[slot].kind != object_kind::REMOVED && !visit(fn, identity(slot), index++))
            {
                break;
            }
        }
    }
    /**
     * @brief Visits primary inputs in declared interface order.
     * Callbacks must not create or remove objects, change terminal order, or replace the layout.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_pi(Fn&& fn) const
    {
        foreach_terminal(inputs, fn);
    }
    /**
     * @brief Visits primary outputs in declared interface order.
     * Callbacks must not create or remove objects, change terminal order, or replace the layout.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_po(Fn&& fn) const
    {
        foreach_terminal(outputs, fn);
    }
    /**
     * @brief Visits logic gates.
     * Callbacks must not create or remove objects, change terminal order, or replace the layout.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_gate(Fn&& fn) const
    {
        uint32_t index{};
        foreach_object([&](const auto id) { return !is_gate(id) || visit(fn, id, index++); });
    }
    /**
     * @brief Visits identity objects, including terminals.
     * Callbacks must not create or remove objects, change terminal order, or replace the layout.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_wire(Fn&& fn) const
    {
        uint32_t index{};
        foreach_object([&](const auto id) { return !is_wire(id) || visit(fn, id, index++); });
    }
    /**
     * @brief Visits declared sources in input-index order. Disconnected inputs retain their indices.
     * Callbacks must not remove the traversed object or change its input connections.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_fanin(const object_id id, Fn&& fn) const
    {
        const auto count = input_count(id);
        for (uint32_t input{}; input < count; ++input)
        {
            const auto edge = input_edges(id.index)[input];
            if (edge != NO_INDEX && !visit(fn, identity(edges[edge].source), input))
            {
                break;
            }
        }
    }
    /**
     * @brief Visits sink input ports of an output, irrespective of physical legality.
     * Sink order is unspecified. Callbacks must not remove the source object or change its sink connections.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_sink(const object_id src, Fn&& fn) const
    {
        static_cast<void>(checked_object(src));
        uint32_t index{};
        for (auto edge = objects[src.index].first_sink; edge != NO_INDEX; edge = edges[edge].next)
        {
            const auto& connection = edges[edge];
            if (!visit(fn, input_port{identity(connection.destination), connection.input}, index++))
            {
                break;
            }
        }
    }
    /** @brief Visits destination objects once per connected input port in unspecified order. */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_fanout(const object_id id, Fn&& fn) const
    {
        foreach_sink(id, [&](const auto port, const auto index) { return visit(fn, port.object, index); });
    }
    /** @brief Returns coordinates of declared sources; optionally filters physical clocking and adjacency. */
    template <bool RespectClocking = true>
    [[nodiscard]] std::vector<tile> incoming_data_flow(const tile& t) const
    {
        std::vector<tile> result{};
        if (const auto id = find_object(t))
        {
            foreach_fanin(*id,
                          [&](const auto src)
                          {
                              const auto c = get_tile(src);
                              if (this->is_adjacent_elevation_of(t, c) &&
                                  (!RespectClocking || is_incoming_clocked(t, c)))
                              {
                                  result.push_back(c);
                              }
                          });
        }
        return result;
    }
    /** @brief Returns coordinates of declared sinks; optionally filters physical clocking and adjacency. */
    template <bool RespectClocking = true>
    [[nodiscard]] std::vector<tile> outgoing_data_flow(const tile& t) const
    {
        std::vector<tile> result{};
        if (const auto id = find_object(t))
        {
            foreach_fanout(*id,
                           [&](const auto dst)
                           {
                               const auto c = get_tile(dst);
                               if (this->is_adjacent_elevation_of(t, c) &&
                                   (!RespectClocking || is_outgoing_clocked(t, c)))
                               {
                                   result.push_back(c);
                               }
                           });
        }
        return result;
    }
    /** @brief Sets an object's name. */
    void set_name(const object_id id, const std::string& name)
    {
        static_cast<void>(checked_object(id));
        if (name.empty())
        {
            names.erase(id);
        }
        else
        {
            names[id] = name;
        }
    }
    /** @brief Returns an object's name, or an empty string for an unnamed object. */
    [[nodiscard]] std::string get_name(const object_id id) const
    {
        static_cast<void>(checked_object(id));
        const auto it = names.find(id);
        return it == names.end() ? std::string{} : it->second;
    }
    /** @brief Returns whether an object has a name. */
    [[nodiscard]] bool has_name(const object_id id) const
    {
        return !get_name(id).empty();
    }
    /** @brief Returns the input name at an interface index. */
    [[nodiscard]] std::string get_input_name(const uint32_t index) const
    {
        return get_name(pi_at(index));
    }
    /** @brief Sets the input name at an interface index. */
    void set_input_name(const uint32_t index, const std::string& name)
    {
        set_name(pi_at(index), name);
    }
    /** @brief Returns whether an input has a name. */
    [[nodiscard]] bool has_input_name(const uint32_t index) const
    {
        return !get_input_name(index).empty();
    }
    /** @brief Returns the output name at an interface index. */
    [[nodiscard]] std::string get_output_name(const uint32_t index) const
    {
        return get_name(po_at(index));
    }
    /** @brief Sets the output name at an interface index. */
    void set_output_name(const uint32_t index, const std::string& name)
    {
        set_name(po_at(index), name);
    }
    /** @brief Returns whether an output has a name. */
    [[nodiscard]] bool has_output_name(const uint32_t index) const
    {
        return !get_output_name(index).empty();
    }
    /** @brief Returns the layout name. */
    [[nodiscard]] std::string get_layout_name() const
    {
        return layout_name;
    }
    /** @brief Sets the layout name. */
    void set_layout_name(const std::string& name)
    {
        layout_name = name;
    }
    /** @brief Checks for a physical incoming connection from the given x/y location. */
    template <bool RespectClocking = true>
    [[nodiscard]] bool is_incoming_signal(const tile& t, const std::optional<tile>& source_tile) const
    {
        if (!source_tile)
        {
            return false;
        }
        bool found{};
        if (const auto id = find_object(t))
        {
            foreach_fanin(*id,
                          [&](const auto port)
                          {
                              const auto c = get_tile(port);
                              found        = this->is_adjacent_elevation_of(t, c) &&
                                             (!RespectClocking || is_incoming_clocked(t, c)) && c.x == source_tile->x &&
                                             c.y == source_tile->y &&
                                             std::abs(static_cast<int64_t>(c.z) - source_tile->z) <= 1;
                              return !found;
                          });
        }
        return found;
    }
    /** @brief Checks for a physical outgoing connection to the given x/y location. */
    template <bool RespectClocking = true>
    [[nodiscard]] bool is_outgoing_signal(const tile& t, const std::optional<tile>& target_tile) const
    {
        if (!target_tile)
        {
            return false;
        }
        bool found{};
        if (const auto id = find_object(t))
        {
            foreach_fanout(*id,
                           [&](const auto dst)
                           {
                               const auto c = get_tile(dst);
                               found = this->is_adjacent_elevation_of(t, c) &&
                                       (!RespectClocking || is_outgoing_clocked(t, c)) && c.x == target_tile->x &&
                                       c.y == target_tile->y &&
                                       std::abs(static_cast<int64_t>(c.z) - target_tile->z) <= 1;
                               return !found;
                           });
        }
        return found;
    }
    /**
     * Checks whether the given tile has an incoming one in northern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_northern_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::north(t));
    }
    /**
     * Checks whether the given tile has an incoming one in north-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north_east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_eastern_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::north_east(t));
    }
    /**
     * Checks whether the given tile has an incoming one in eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_eastern_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::east(t));
    }
    /**
     * Checks whether the given tile has an incoming one in south-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south_east(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_eastern_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::south_east(t));
    }
    /**
     * Checks whether the given tile has an incoming one in southern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_southern_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::south(t));
    }
    /**
     * Checks whether the given tile has an incoming one in south-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `south_west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_western_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::south_west(t));
    }
    /**
     * Checks whether the given tile has an incoming one in western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_western_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::west(t));
    }
    /**
     * Checks whether the given tile has an incoming one in north-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `north_west(t)` is incoming to `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_western_incoming_signal(const tile& t) const
    {
        return is_incoming_signal<RespectClocking>(t, CoordinateLayout::north_west(t));
    }
    /**
     * Checks whether the given tile has no incoming tiles.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanins.
     * @param t Base tile.
     * @return `true` iff `t` does not have incoming tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_no_incoming_signal(const tile& t) const
    {
        bool found{};
        if (const auto id = find_object(t))
        {
            foreach_fanin(*id,
                          [&](const auto port)
                          {
                              const auto adjacent = get_tile(port);
                              found               = this->is_adjacent_elevation_of(t, adjacent) &&
                                                    (!RespectClocking || is_incoming_clocked(t, adjacent));
                              return !found;
                          });
        }
        return !found;
    }
    /**
     * Checks whether the given tile has an outgoing one in northern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_northern_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::north(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in north-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north_east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_eastern_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::north_east(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_eastern_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::east(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in south-eastern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south_east(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_eastern_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::south_east(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in southern direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_southern_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::south(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in south-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `south_west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_south_western_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::south_west(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_western_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::west(t));
    }
    /**
     * Checks whether the given tile has an outgoing one in north-western direction.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `north_west(t)` is outgoing from `t`.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_north_western_outgoing_signal(const tile& t) const
    {
        return is_outgoing_signal<RespectClocking>(t, CoordinateLayout::north_west(t));
    }
    /**
     * Checks whether the given tile has no outgoing tiles.
     *
     * @tparam RespectClocking Flag to indicate that the underlying clocking is to be respected when evaluating fanouts.
     * @param t Base tile.
     * @return `true` iff `t` does not have outgoing tiles.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_no_outgoing_signal(const tile& t) const
    {
        bool found{};
        if (const auto id = find_object(t))
        {
            foreach_fanout(*id,
                           [&](const auto port)
                           {
                               const auto adjacent = get_tile(port);
                               found               = this->is_adjacent_elevation_of(t, adjacent) &&
                                                     (!RespectClocking || is_outgoing_clocked(t, adjacent));
                               return !found;
                           });
        }
        return !found;
    }
    /**
     * @brief Checks whether incoming and outgoing signals lie on opposite sides of `t`.
     *
     * Uses `foreach_adjacent_opposite_coordinates` of the underlying coordinate layout.
     * @tparam RespectClocking Whether signal queries respect the clocking scheme.
     * @param t Base tile.
     * @return Whether `t` has incoming and outgoing signals on opposite sides.
     */
    template <bool RespectClocking = true>
    [[nodiscard]] bool has_opposite_incoming_and_outgoing_signals(const tile& t) const
    {
        auto opposite_signals = false;

        CoordinateLayout::foreach_adjacent_opposite_coordinates(
            t,
            [this, &t, &opposite_signals](const auto& sp)
            {
                const auto s1 = std::get<0>(sp), s2 = std::get<1>(sp);

                if ((this->template is_incoming_signal<RespectClocking>(t, s1) &&
                     this->template is_outgoing_signal<RespectClocking>(t, s2)) ||
                    (this->template is_incoming_signal<RespectClocking>(t, s2) &&
                     this->template is_outgoing_signal<RespectClocking>(t, s1)))
                {
                    opposite_signals = true;

                    return false;  // break loop
                }

                return true;  // continue looping
            });

        return opposite_signals;
    }

    /**
     * Replaces the stored clocking scheme with the provided one.
     *
     * @param scheme New clocking scheme.
     */
    void replace_clocking_scheme(const clocking_scheme_t& scheme)
    {
        clocking_state.replace_clocking_scheme(scheme);
    }
    /**
     * Overrides the clock number of a tile in the stored scheme. The clock number applies to every layer of the tile,
     * so the z-coordinate of `cz` is ignored.
     *
     * @param cz Clock zone to override.
     * @param cn New clock number for `cz`.
     */
    void assign_clock_number(const clock_zone& cz, const clock_number_t cn)
    {
        clocking_state.assign_clock_number(cz, cn);
    }
    /**
     * Returns the clock number of a tile. Every layer of a tile has the same clock number, so the z-coordinate of `cz`
     * is ignored.
     *
     * @param cz Clock zone.
     * @return Clock number of `cz`.
     */
    [[nodiscard]] clock_number_t get_clock_number(const clock_zone& cz) const
    {
        return clocking_state.get_clock_number(cz);
    }
    /**
     * Returns the number of clock phases in the layout. Each clock cycle is divided into n phases. In QCA, the number
     * of phases is usually 4. In iNML it is 3. Clocking schemes support 3 or 4 phases.
     *
     * @return The number of different clock signals in the layout.
     */
    [[nodiscard]] clock_number_t num_clocks() const
    {
        return clocking_state.num_clocks();
    }
    /**
     * Returns whether the layout is clocked by a regular clocking scheme with no overwritten zones.
     *
     * @return `true` iff the layout is clocked by a regular scheme and no zones have been overwritten.
     */
    [[nodiscard]] bool is_regularly_clocked() const
    {
        return clocking_state.is_regularly_clocked();
    }
    /**
     * Compares the stored clocking scheme against the provided name. Predefined names are constants in
     * `fiction::layouts::clocking`.
     *
     * @param name Clocking scheme name.
     * @return `true` iff the layout is clocked by a clocking scheme of name `name`.
     */
    [[nodiscard]] bool is_clocking_scheme(const std::string_view& name) const
    {
        return clocking_state.is_clocking_scheme(name);
    }
    /**
     * Returns a read-only reference to the stored clocking scheme object. Clock overrides and scheme replacements
     * update the referenced object. Assignment or moving from the layout replaces the referenced contents;
     * the reference stays attached to the layout that supplied it.
     *
     * @return A reference valid for the lifetime of this layout.
     */
    [[nodiscard]] const clocking_scheme_t& get_clocking_scheme() const noexcept
    {
        return clocking_state.get_clocking_scheme();
    }
    /**
     * Evaluates whether clock zone `cz2` feeds information to clock zone `cz1`, i.e., whether `cz2` is clocked with a
     * clock number that is lower by 1 modulo `num_clocks()`, or either zone is a synchronization element.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is lower by 1.
     * @return `true` iff `cz2` can feed information to `cz1`.
     */
    [[nodiscard]] bool is_incoming_clocked(const clock_zone& cz1, const clock_zone& cz2) const
    {
        return clocking_state.is_incoming_clocked(cz1, cz2);
    }
    /**
     * Evaluates whether clock zone `cz2` accepts information from clock zone `cz1`, i.e., whether `cz2` is clocked with
     * a clock number that is higher by 1 modulo `num_clocks()`, or either zone is a synchronization element.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is higher by 1.
     * @return `true` iff `cz2` can accept information from `cz1`.
     */
    [[nodiscard]] bool is_outgoing_clocked(const clock_zone& cz1, const clock_zone& cz2) const
    {
        return clocking_state.is_outgoing_clocked(cz1, cz2);
    }

    /**
     * Assigns a synchronization element to the provided clock zone.
     *
     * @param cz Clock zone to turn into a synchronization element.
     * @param se Number of full clock cycles to extend `cz`'s Hold phase by. If this value is 0, `cz` is turned back
     * into a normal clock zone.
     */
    void assign_synchronization_element(const clock_zone& cz, const sync_elem_t se)
    {
        clocking_state.assign_synchronization_element(cz, se);
    }
    /**
     * Check whether the provided clock zone is a synchronization element.
     *
     * @param cz Clock zone to check.
     * @return `true` iff `cz` is a synchronization element.
     */
    [[nodiscard]] bool is_synchronization_element(const clock_zone& cz) const
    {
        return clocking_state.is_synchronization_element(cz);
    }
    /**
     * Returns the Hold phase extension in clock cycles of clock zone `cz`.
     *
     * @param cz Clock zone to check.
     * @return Synchronization element value, i.e., Hold phase extension, of clock zone `cz`.
     */
    [[nodiscard]] sync_elem_t get_synchronization_element(const clock_zone& cz) const
    {
        return clocking_state.get_synchronization_element(cz);
    }

    /** @brief Counts zones with a nonzero Hold-phase extension. @return Synchronization element count. */
    [[nodiscard]] uint32_t num_se() const
    {
        return clocking_state.num_se();
    }
    /**
     * Marks the given coordinate as obstructed.
     *
     * @param c clock_zone to obstruct.
     */
    void obstruct_coordinate(const clock_zone& c)
    {
        obstruction_state.obstruct_coordinate(c);
    }
    /**
     * Marks the connection from coordinate `src` to coordinate `tgt` as obstructed.
     *
     * @note clock_zones marked this way will not be crossed with wires by path finding algorithms.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     */
    void obstruct_connection(const clock_zone& src, const clock_zone& tgt)
    {
        obstruction_state.obstruct_connection(src, tgt);
    }
    /**
     * Clears the obstruction status of the given coordinate `c` if the obstruction was manually marked via
     * `obstruct_coordinate`.
     *
     * @param c clock_zone to clear.
     */
    void clear_obstructed_coordinate(const clock_zone& c)
    {
        obstruction_state.clear_obstructed_coordinate(c);
    }
    /**
     * Clears the obstruction status of the connection from coordinate `src` to coordinate `tgt` if the obstruction was
     * manually marked via `obstruct_connection`.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     */
    void clear_obstructed_connection(const clock_zone& src, const clock_zone& tgt)
    {
        obstruction_state.clear_obstructed_connection(src, tgt);
    }
    /**
     * Clears all obstructed coordinates that were manually marked via `obstruct_coordinate`.
     */
    void clear_obstructed_coordinates()
    {
        obstruction_state.clear_obstructed_coordinates();
    }
    /**
     * Clears all obstructed connections that were manually marked via `obstruct_connection`.
     */
    void clear_obstructed_connections()
    {
        obstruction_state.clear_obstructed_connections();
    }

    /**
     * Visits manual coordinate obstructions without implicit occupancy.
     * @tparam Fn Callable accepting one coordinate.
     * @param fn Callback for each manual obstruction.
     */
    template <typename Fn>
    void foreach_obstructed_coordinate(Fn&& fn) const
    {
        obstruction_state.foreach_obstructed_coordinate(std::forward<Fn>(fn));
    }
    /**
     * Visits manual directed-connection obstructions without implicit physical connections.
     * @tparam Fn Callable accepting source and target coordinates.
     * @param fn Callback for each manual obstruction.
     */
    template <typename Fn>
    void foreach_obstructed_connection(Fn&& fn) const
    {
        obstruction_state.foreach_obstructed_connection(std::forward<Fn>(fn));
    }
    /**
     * Checks if the given coordinate is obstructed of some sort.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is obstructed.
     */
    [[nodiscard]] bool is_obstructed_coordinate(const clock_zone& c) const
    {
        return obstruction_state.is_obstructed_coordinate(c) || !is_empty_tile(c);
    }
    /**
     * Checks if the given coordinate-coordinate connection is obstructed of some sort.
     *
     * @param src Source coordinate.
     * @param tgt Target coordinate.
     * @return `true` iff the connection from `src` to `tgt` is obstructed.
     */
    [[nodiscard]] bool is_obstructed_connection(const clock_zone& src, const clock_zone& tgt) const
    {
        return obstruction_state.is_obstructed_connection(src, tgt) || is_incoming_signal(tgt, src) ||
               is_outgoing_signal(src, tgt);
    }

#pragma region Clocked neighbor iteration

    /**
     * Returns a container with all clock zones that are incoming to the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are incoming to `cz`.
     */
    [[nodiscard]] auto incoming_clocked_zones(const clock_zone& cz) const
    {
        std::vector<clock_zone> incoming{};

        foreach_incoming_clocked_zone(cz, [&incoming](const auto& ct) { incoming.push_back(ct); });

        return incoming;
    }
    /**
     * Applies a function as an lvalue to all incoming clock zones of a given one.
     *
     * @tparam Fn Functor type.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s incoming clock zones.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_incoming_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(cz,
                                                      [this, &cz, &fn](const auto& ct)
                                                      {
                                                          if (is_incoming_clocked(cz, ct))
                                                          {
                                                              std::invoke(fn, ct);
                                                          }
                                                      });
    }
    /**
     * Returns a container with all clock zones that are outgoing from the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are outgoing from `cz`.
     */
    [[nodiscard]] auto outgoing_clocked_zones(const clock_zone& cz) const
    {
        std::vector<clock_zone> outgoing{};

        foreach_outgoing_clocked_zone(cz, [&outgoing](const auto& ct) { outgoing.push_back(ct); });

        return outgoing;
    }
    /**
     * Applies a function as an lvalue to all outgoing clock zones of a given one.
     *
     * @tparam Fn Functor type.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s outgoing clock zones.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_outgoing_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(cz,
                                                      [this, &cz, &fn](const auto& ct)
                                                      {
                                                          if (is_outgoing_clocked(cz, ct))
                                                          {
                                                              std::invoke(fn, ct);
                                                          }
                                                      });
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the number of incoming clock zones to the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s incoming clock zones.
     */
    [[nodiscard]] degree_t in_degree(const clock_zone& cz) const
    {
        degree_t idg{0};
        foreach_incoming_clocked_zone(cz, [&idg](const auto&) { ++idg; });

        return idg;
    }
    /**
     * Returns the number of outgoing clock zones from the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s outgoing clock zones.
     */
    [[nodiscard]] degree_t out_degree(const clock_zone& cz) const
    {
        degree_t odg{0};
        foreach_outgoing_clocked_zone(cz, [&odg](const auto&) { ++odg; });

        return odg;
    }
    /**
     * Returns the number of distinct incoming or outgoing neighboring clock zones.
     *
     * @param cz Base clock zone.
     * @return Number of distinct clocked neighbors of `cz`.
     */
    [[nodiscard]] degree_t degree(const clock_zone& cz) const
    {
        degree_t count{0};
        CoordinateLayout::foreach_adjacent_coordinate(cz,
                                                      [this, &cz, &count](const auto& adjacent)
                                                      {
                                                          if (is_incoming_clocked(cz, adjacent) ||
                                                              is_outgoing_clocked(cz, adjacent))
                                                          {
                                                              ++count;
                                                          }
                                                      });
        return count;
    }

#pragma endregion
#pragma region Tile iteration

    /**
     * @brief Returns the tiles in the coordinate range.
     * @param start First tile.
     * @param stop Exclusive end tile; absence selects the layout end.
     * @return Tile range.
     */
    [[nodiscard]] auto tiles(const std::optional<tile>& start = std::nullopt,
                             const std::optional<tile>& stop  = std::nullopt) const
    {
        return CoordinateLayout::coordinates(start, stop);
    }

    /**
     * @brief Applies a function to each tile in the coordinate range.
     * @tparam Fn Functor type.
     * @param fn Functor applied to each tile.
     * @param start First tile.
     * @param stop Exclusive end tile; absence selects the layout end.
     */
    template <typename Fn>
    void foreach_tile(Fn&& fn, const std::optional<tile>& start = std::nullopt,
                      const std::optional<tile>& stop = std::nullopt) const
    {
        CoordinateLayout::foreach_coordinate(std::forward<Fn>(fn), start, stop);
    }

    /**
     * @brief Returns ground-layer tiles in the coordinate range.
     * @param start First tile.
     * @param stop Exclusive end tile; absence selects the layout end.
     * @return Tile range.
     */
    [[nodiscard]] auto ground_tiles(const std::optional<tile>& start = std::nullopt,
                                    const std::optional<tile>& stop  = std::nullopt) const
    {
        return CoordinateLayout::ground_coordinates(start, stop);
    }

    /**
     * @brief Applies a function to each ground-layer tile in the coordinate range.
     * @tparam Fn Functor type.
     * @param fn Functor applied to each tile.
     * @param start First tile.
     * @param stop Exclusive end tile; absence selects the layout end.
     */
    template <typename Fn>
    void foreach_ground_tile(Fn&& fn, const std::optional<tile>& start = std::nullopt,
                             const std::optional<tile>& stop = std::nullopt) const
    {
        CoordinateLayout::foreach_ground_coordinate(std::forward<Fn>(fn), start, stop);
    }

    /**
     * @brief Returns adjacent tiles.
     * @param t Base tile.
     * @return Adjacent tiles in the coordinate geometry.
     */
    std::vector<tile> adjacent_tiles(const tile& t) const
    {
        return CoordinateLayout::adjacent_coordinates(t);
    }

    /**
     * @brief Applies a function to each adjacent tile.
     * @tparam Fn Functor type.
     * @param t Base tile.
     * @param fn Functor applied to adjacent tiles.
     */
    template <typename Fn>
    void foreach_adjacent_tile(const tile& t, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(t, std::forward<Fn>(fn));
    }

    /**
     * @brief Returns pairs of opposite adjacent tiles.
     * @param t Base tile.
     * @return Adjacent tiles in the coordinate geometry.
     */
    std::vector<std::pair<tile, tile>> adjacent_opposite_tiles(const tile& t) const
    {
        return CoordinateLayout::adjacent_opposite_coordinates(t);
    }

    /**
     * @brief Applies a function to each pair of opposite adjacent tiles.
     * @tparam Fn Functor type.
     * @param t Base tile.
     * @param fn Functor applied to adjacent tiles.
     */
    template <typename Fn>
    void foreach_adjacent_opposite_tiles(const tile& t, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_opposite_coordinates(t, std::forward<Fn>(fn));
    }

#pragma endregion

    /**
     * Visits zones with a nonzero synchronization delay.
     * @tparam Fn Callable accepting a clock zone and delay.
     * @param fn Callback for each synchronization element.
     */
    template <typename Fn>
    void foreach_synchronization_element(Fn&& fn) const
    {
        clocking_state.foreach_synchronization_element(std::forward<Fn>(fn));
    }

  private:
    /** @brief Missing slot or edge index. */
    static constexpr uint32_t NO_INDEX = std::numeric_limits<uint32_t>::max();
    /** @brief Object role; removed slots have no coordinate or connections visible through the API. */
    enum class object_kind : uint8_t
    {
        /** @brief Removed object slot. */
        REMOVED,
        /** @brief Primary input terminal. */
        PI,
        /** @brief Primary output terminal. */
        PO,
        /** @brief Identity wire. */
        WIRE,
        /** @brief Logic gate. */
        GATE
    };
    /** @brief Hot object data. Names and truth-table payloads are stored separately. */
    struct object_record
    {
        /** @brief Assigned coordinate. */
        tile position{};
        /** @brief Generation checked by object handles. */
        uint32_t generation{1};
        /** @brief Interned truth-table literal. */
        uint32_t function{};
        /** @brief First reverse connection, or next free object when removed. */
        uint32_t first_sink{NO_INDEX};
        /** @brief Number of connected sink input ports. */
        uint32_t sink_count{};
        /** @brief Physical role. */
        object_kind kind{object_kind::REMOVED};
        /** @brief Number of input ports, including disconnected ports. */
        uint32_t input_count{};
        /** @brief Inline input-index to connection mapping for every built-in gate. */
        std::array<uint32_t, 3> inputs{NO_INDEX, NO_INDEX, NO_INDEX};
    };
    /** @brief Mutable connection with constant-time removal from the source's sink list. */
    struct edge_record
    {
        /** @brief Source object slot. */
        uint32_t source{};
        /** @brief Destination object slot. */
        uint32_t destination{};
        /** @brief Destination input index. */
        uint32_t input{};
        /** @brief Previous sink edge. */
        uint32_t previous{NO_INDEX};
        /** @brief Next sink edge, or next free edge when disconnected. */
        uint32_t next{NO_INDEX};
    };
    /** @brief Reusable object slots. */
    std::vector<object_record> objects{};
    /** @brief Input-index to connection mapping for objects with more than three inputs. */
    phmap::flat_hash_map<uint32_t, std::vector<uint32_t>> spilled_inputs{};
    /** @brief Reusable contiguous connection storage. */
    std::vector<edge_record> edges{};
    /** @brief Coordinate lookup independent of identities and connections. */
    phmap::flat_hash_map<tile, uint32_t> occupancy{};
    /** @brief Deduplicated cold truth-table payloads. */
    mockturtle::truth_table_cache<kitty::dynamic_truth_table> functions{0};
    /** @brief Sparse cold names. */
    phmap::flat_hash_map<object_id, std::string> names{};
    /** @brief Declared primary input order. */
    std::vector<uint32_t> inputs{};
    /** @brief Declared primary output order. */
    std::vector<uint32_t> outputs{};
    /** @brief Clocking overrides and synchronization. */
    clocking::state clocking_state{clocking::open()};
    /** @brief Persistent manual obstructions. */
    layouts::obstructions obstruction_state{};
    /** @brief Layout name. */
    std::string layout_name{};
    /** @brief First reusable object slot. */
    uint32_t free_object{NO_INDEX};
    /** @brief First reusable connection slot. */
    uint32_t free_edge{NO_INDEX};
    /** @brief Live object count. */
    uint32_t live_count{};
    /** @brief Live identity-function count. */
    uint32_t wire_count{};

    /** @brief Swaps owned state without changing geometry. */
    void swap_owned_state(gate_level_layout& other) noexcept
    {
        using std::swap;
        swap(objects, other.objects);
        swap(spilled_inputs, other.spilled_inputs);
        swap(edges, other.edges);
        swap(occupancy, other.occupancy);
        swap(functions, other.functions);
        swap(names, other.names);
        swap(inputs, other.inputs);
        swap(outputs, other.outputs);
        swap(clocking_state, other.clocking_state);
        swap(obstruction_state, other.obstruction_state);
        swap(layout_name, other.layout_name);
        swap(free_object, other.free_object);
        swap(free_edge, other.free_edge);
        swap(live_count, other.live_count);
        swap(wire_count, other.wire_count);
    }
    /** @brief Initializes elementary functions when a moved-from layout is reused. */
    void ensure_functions()
    {
        if (functions.size() == 0)
        {
            initialize_functions();
        }
    }
    /** @brief Reconstructs a live slot's identity. */
    [[nodiscard]] object_id identity(const uint32_t slot) const noexcept
    {
        return {slot, objects[slot].generation};
    }
    /** @brief Validates an object identity. */
    [[nodiscard]] const object_record& checked_object(const object_id id) const
    {
        if (!contains(id))
        {
            throw std::invalid_argument("Object identity is stale or belongs to no live object");
        }
        return objects[id.index];
    }
    /** @brief Validates an object identity. */
    [[nodiscard]] object_record& checked_object(const object_id id)
    {
        static_cast<void>(std::as_const(*this).checked_object(id));
        return objects[id.index];
    }
    /** @brief Returns an object's ordered connection indices, including disconnected slots. */
    [[nodiscard]] std::span<const uint32_t> input_edges(const uint32_t slot) const
    {
        const auto& object = objects[slot];
        return object.input_count > object.inputs.size() ? std::span{spilled_inputs.at(slot)} :
                                                           std::span{object.inputs.data(), object.input_count};
    }
    /** @brief Returns an object's mutable ordered connection indices. */
    [[nodiscard]] std::span<uint32_t> input_edges(const uint32_t slot)
    {
        auto& object = objects[slot];
        return object.input_count > object.inputs.size() ? std::span{spilled_inputs.at(slot)} :
                                                           std::span{object.inputs.data(), object.input_count};
    }
    /** @brief Validates an input endpoint and returns its connection index. */
    [[nodiscard]] uint32_t checked_input(const input_port port) const
    {
        const auto& object = checked_object(port.object);
        if (port.index >= object.input_count)
        {
            throw std::out_of_range("Input index exceeds the object's arity");
        }
        return input_edges(port.object.index)[port.index];
    }
    /** @brief Rejects occupied placement before any object mutation. Coordinates outside the extent are valid during
     * editing. */
    void check_placement(const tile& t) const
    {
        if (occupancy.contains(t))
        {
            throw std::invalid_argument("The placement coordinate is occupied");
        }
    }
    /** @brief Allocates an edge from the free list or grows storage. */
    uint32_t allocate_edge()
    {
        if (free_edge != NO_INDEX)
        {
            const auto id = free_edge;
            free_edge     = edges[id].next;
            return id;
        }
        if (edges.size() == NO_INDEX)
        {
            throw std::length_error("Layout connection capacity exhausted");
        }
        edges.emplace_back();
        return static_cast<uint32_t>(edges.size() - 1);
    }
    /** @brief Removes a connection from both endpoints and recycles the edge slot. */
    void unlink_edge(const uint32_t id) noexcept
    {
        auto& edge = edges[id];
        auto& src  = objects[edge.source];
        if (edge.previous != NO_INDEX)
        {
            edges[edge.previous].next = edge.next;
        }
        else
        {
            src.first_sink = edge.next;
        }
        if (edge.next != NO_INDEX)
        {
            edges[edge.next].previous = edge.previous;
        }
        --src.sink_count;
        input_edges(edge.destination)[edge.input] = NO_INDEX;
        edge.next                                 = free_edge;
        free_edge                                 = id;
    }
    /** @brief Creates a validated object and its initial ordered connections. */
    object_id create_object(const std::span<const object_id> children, const uint32_t function, const object_kind kind,
                            const uint32_t arity, const tile& t)
    {
        check_placement(t);
        ensure_functions();
        if (children.size() > arity)
        {
            throw std::invalid_argument("Connections exceed the object's input count");
        }
        for (const auto child : children)
        {
            static_cast<void>(checked_object(child));
        }
        object_record record{};
        record.position    = t;
        record.function    = function;
        record.kind        = kind;
        record.input_count = arity;
        if (children.size() > NO_INDEX - edges.size())
        {
            throw std::length_error("Layout connection capacity exhausted");
        }
        const bool reuse = free_object != NO_INDEX;
        if (!reuse && objects.size() == NO_INDEX)
        {
            throw std::length_error("Layout object capacity exhausted");
        }
        const auto slot = reuse ? free_object : static_cast<uint32_t>(objects.size());
        if (arity > record.inputs.size())
        {
            spilled_inputs.emplace(slot, std::vector<uint32_t>(arity, NO_INDEX));
        }
        try
        {
            occupancy.emplace(t, slot);
            if (reuse)
            {
                record.generation = objects[slot].generation;
                free_object       = objects[slot].first_sink;
                objects[slot]     = std::move(record);
            }
            else
            {
                objects.push_back(std::move(record));
            }
        }
        catch (...)
        {
            occupancy.erase(t);
            spilled_inputs.erase(slot);
            throw;
        }
        ++live_count;
        wire_count += static_cast<uint32_t>(function == 2);
        const auto id = identity(slot);
        try
        {
            for (uint32_t input{}; input < children.size(); ++input)
            {
                connect(children[input], {id, input});
            }
        }
        catch (...)
        {
            remove(id);
            throw;
        }
        return id;
    }
    /** @brief Creates a named output terminal. */
    object_id create_terminal(const std::span<const object_id> children, const std::string& name, const tile& t)
    {
        const auto p = create_object(children, 2, object_kind::PO, 1, t);
        try
        {
            set_name(p, name);
            outputs.push_back(p.index);
        }
        catch (...)
        {
            remove(p);
            throw;
        }
        return p;
    }
    /** @brief Validates a terminal permutation before applying it. */
    void set_terminal_order(std::vector<uint32_t>& terminals, const std::span<const object_id> order,
                            const object_kind kind)
    {
        if (order.size() != terminals.size())
        {
            throw std::invalid_argument("Interface order must include every terminal");
        }
        std::vector<uint32_t> reordered{};
        reordered.reserve(order.size());
        for (const auto id : order)
        {
            if (checked_object(id).kind != kind)
            {
                throw std::invalid_argument("Interface order contains a different object role");
            }
            reordered.push_back(id.index);
        }
        auto sorted = reordered;
        std::ranges::sort(sorted);
        if (std::ranges::adjacent_find(sorted) != sorted.end())
        {
            throw std::invalid_argument("Interface order repeats a terminal");
        }
        terminals = std::move(reordered);
    }
    /** @brief Visits a value with an optional enumeration index and early stopping. */
    template <typename Fn, typename Value>
    static bool visit(Fn& fn, const Value value, const uint32_t index)
    {
        if constexpr (std::is_invocable_v<Fn&, Value, uint32_t>)
        {
            if constexpr (std::is_same_v<std::invoke_result_t<Fn&, Value, uint32_t>, bool>)
            {
                return std::invoke(fn, value, index);
            }
            else
            {
                std::invoke(fn, value, index);
            }
        }
        else
        {
            if constexpr (std::is_same_v<std::invoke_result_t<Fn&, Value>, bool>)
            {
                return std::invoke(fn, value);
            }
            else
            {
                std::invoke(fn, value);
            }
        }
        return true;
    }
    /** @brief Visits interface objects in their declared order. */
    template <typename Fn>
    void foreach_terminal(const std::vector<uint32_t>& terminals, Fn& fn) const
    {
        for (uint32_t index{}; index < terminals.size(); ++index)
        {
            if (!visit(fn, identity(terminals[index]), index))
            {
                break;
            }
        }
    }
    /** @brief Interns the elementary gate functions without allocating layout objects. */
    void initialize_functions()
    {
        /** @brief Complete elementary cache, committed after all allocations succeed. */
        mockturtle::truth_table_cache<kitty::dynamic_truth_table> initialized{16};
        initialized.insert(kitty::dynamic_truth_table{0});
        for (const auto [literal, arity] : std::array<std::pair<uint64_t, uint32_t>, 7>{
                 {{0x1, 1}, {0x8, 2}, {0xe, 2}, {0x2, 2}, {0xb, 2}, {0x6, 2}, {0xe8, 3}}})
        {
            kitty::dynamic_truth_table table{arity};
            /** @brief One truth-table word consumed by the gate-function constructor. */
            const std::array words{literal};
            kitty::create_from_words(table, words.cbegin(), words.cend());
            initialized.insert(table);
        }
        functions = std::move(initialized);
    }
};
}  // namespace fiction::layouts
