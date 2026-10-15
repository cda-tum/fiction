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
 * @brief Routing objective and path types plus helpers to lay and remove wire routing.
 * @author Marcel Walter (marcelwa)
 */

#pragma once
#include "fiction/layouts/gate_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/obstructions.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace fiction::physical_design
{
namespace detail
{
/** @brief Combines layout occupancy with explicit search constraints.
 * @tparam Lyt Layout type. @param lyt Layout. @param c Position. @param extra Search constraints.
 * @return Whether the position is obstructed.
 */
template <typename Lyt>
[[nodiscard]] bool routing_coordinate_obstructed(const Lyt& lyt, const fiction::layouts::layout_base::coordinate& c,
                                                 const layouts::obstructions& extra) noexcept
{
    if constexpr (requires { requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>; })
    {
        return extra.is_obstructed_coordinate(c) || lyt.is_obstructed_coordinate(c);
    }
    else
    {
        return extra.is_obstructed_coordinate(c);
    }
}
/** @brief Combines layout connections with explicit search constraints.
 * @tparam Lyt Layout type. @param lyt Layout. @param src Source. @param tgt Target. @param extra Search constraints.
 * @return Whether the directed connection is obstructed.
 */
template <typename Lyt>
[[nodiscard]] bool routing_connection_obstructed(const Lyt& lyt, const fiction::layouts::layout_base::coordinate& src,
                                                 const fiction::layouts::layout_base::coordinate& tgt,
                                                 const layouts::obstructions&                     extra) noexcept
{
    if constexpr (requires { requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>; })
    {
        return extra.is_obstructed_connection(src, tgt) || lyt.is_obstructed_connection(src, tgt);
    }
    else
    {
        return extra.is_obstructed_connection(src, tgt);
    }
}
}  // namespace detail

/**
 * Routing objectives identify a geometric source and an ordered destination input.
 *
 */
struct routing_objective
{
    /** @brief Source coordinate. */
    const layouts::layout_base::coordinate source;
    /** @brief Target coordinate. */
    const layouts::layout_base::coordinate target;
    /** @brief Logical input index at the target; geometric searches ignore this field. */
    uint32_t input_index{};
    /**
     * Equality operator.
     * @param other Routing objective to compare to.
     * @return `true` iff the given objective is equal to this one.
     */
    bool operator==(const routing_objective& other) const noexcept
    {
        return source == other.source && target == other.target && input_index == other.input_index;
    }
};
/**
 * A path in a layout defined as an ordered sequence of coordinates.
 *
 */
class layout_coordinate_path : public std::vector<layouts::layout_base::coordinate>
{
  public:
    /** @brief Appends a coordinate. @param c Coordinate to append. @throws std::bad_alloc If storage allocation fails.
     */
    void append(const layouts::layout_base::coordinate& c)
    {
        this->push_back(c);
    }

    /** @brief Returns the first coordinate, or the origin for an empty path. @return Source coordinate. */
    [[nodiscard]] layouts::layout_base::coordinate source() const noexcept
    {
        return this->empty() ? layouts::layout_base::coordinate{} : this->front();
    }

    /** @brief Returns the last coordinate, or the origin for an empty path. @return Target coordinate. */
    [[nodiscard]] layouts::layout_base::coordinate target() const noexcept
    {
        return this->empty() ? layouts::layout_base::coordinate{} : this->back();
    }

  protected:
    /** @brief Coordinate storage. */
    using base = std::vector<layouts::layout_base::coordinate>;

  public:
    /** @brief Inherits coordinate sequence constructors. */
    using base::base;
};
/**
 * An ordered collection of multiple paths in a layout.
 */
class path_collection : public std::vector<layout_coordinate_path>
{
  public:
    /** @brief Adds a path. @param p Path to add. @throws std::bad_alloc If storage allocation fails. */
    void add(const layout_coordinate_path& p)
    {
        this->push_back(p);
    }

    /**
     * Checks whether a given path is contained in the collection.
     *
     * @param p Path to search for.
     * @return `true` iff `p` is contained in the collection.
     */
    [[nodiscard]] bool contains(const layout_coordinate_path& p) const noexcept
    {
        return std::ranges::find(*this, p) != std::cend(*this);
    }

  protected:
    /** @brief Path storage. */
    using base = std::vector<layout_coordinate_path>;

  public:
    /** @brief Inherits path container constructors. */
    using base::base;
};
/**
 * A set of multiple paths in a layout.
 */
class path_set : public std::set<layout_coordinate_path>
{
  public:
    /** @brief Adds a path. @param p Path to add. @throws std::bad_alloc If storage allocation fails. */
    void add(const layout_coordinate_path& p)
    {
        this->insert(p);
    }

    /** @brief Checks membership. @param p Path to find. @return Whether the set contains the path. */
    [[nodiscard]] bool contains(const layout_coordinate_path& p) const noexcept
    {
        return base::contains(p);
    }

  protected:
    /** @brief Unique path storage. */
    using base = std::set<layout_coordinate_path>;

  public:
    /** @brief Inherits path container constructors. */
    using base::base;
};

/**
 * Checks whether a given coordinate `successor` hosts a crossable wire when coming from coordinate `src` in a given
 * layout. A wire is said to be crossable if a potential cross-over would not result in running along the same
 * information flow direction. For example, a wire segment hosted by `successor` that is horizontal and runs from west
 * to east is crossable by a wire segment coming from `src` that is vertical and runs from north to south. However, if
 * the wire segment coming from `src` were also horizontal and ran from west to east, the cross-over would be
 * prohibited.
 *
 * @Note This function can be called on layout types other than gate-level layouts, but will then always return
 * `false`. This is helpful for general routing in, e.g., clocked layouts.
 *
 * @tparam Lyt Layout type.
 * @param lyt The layout.
 * @param src Source coordinate in `lyt`.
 * @param successor Successor coordinate in lyt reachable from `src`.
 * @return `true` iff `successor` hosts a wire that is crossable from `src`.
 */
template <typename Lyt>
    requires std::derived_from<Lyt, layouts::layout_base>
[[nodiscard]] bool is_crossable_wire(const Lyt& lyt, const fiction::layouts::layout_base::coordinate& src,
                                     const fiction::layouts::layout_base::coordinate& successor) noexcept
{
    assert(lyt.is_adjacent_elevation_of(src, successor));

    if constexpr (requires { requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>; })
    {
        const auto successor_node = lyt.find_object(successor);

        // one can only cross over wire segments, but not over I/Os
        if (successor_node && lyt.is_wire(*successor_node))
        {
            // if wire has missing connections, it is up to no good (could be a dangling fanout)
            if (lyt.has_no_incoming_signal(successor) || lyt.has_no_outgoing_signal(successor))
            {
                // don't cross over weird wires
                return false;
            }
            // if src is in the ground layer, crossing is easily possible
            if (lyt.is_ground_layer(src))
            {
                return true;
            }
            // otherwise, decide based on the information flow direction
            if (const auto below_source = lyt.below(src);
                below_source && !lyt.is_incoming_signal(successor, *below_source))
            {
                return true;
            }
        }
    }

    return false;
}

namespace detail
{
/**
 * @brief Resolves the coordinate that a path search enters when it steps from `current` to the adjacent coordinate
 * `successor`. The search returns to the ground layer, switches to the crossing layer to pass over a crossable wire if
 * `crossings` is set, and rejects obstructed coordinates and connections. The target is never obstructed.
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param current Coordinate that the search expands.
 * @param successor Coordinate adjacent to `current`.
 * @param target Target coordinate of the search.
 * @param crossings Whether paths may cross wires on the crossing layer.
 * @param extra Search constraints.
 * @return The coordinate to enter, or `std::nullopt` if the step is obstructed.
 */
template <typename Lyt>
[[nodiscard]] std::optional<fiction::layouts::layout_base::coordinate>
routing_successor(const Lyt& lyt, const fiction::layouts::layout_base::coordinate& current,
                  fiction::layouts::layout_base::coordinate        successor,
                  const fiction::layouts::layout_base::coordinate& target, const bool crossings,
                  const layouts::obstructions& extra) noexcept
{
    // return to ground layer to avoid getting stuck in crossing layer
    if (successor.z != 0)
    {
        const auto ground = lyt.below(successor);
        if (!ground)
        {
            return std::nullopt;
        }
        successor = *ground;
    }

    if (routing_coordinate_obstructed(lyt, successor, extra) && successor != target)
    {
        // an obstructed successor can only be passed on a free crossing layer above a crossable wire
        const auto above_successor = lyt.above(successor);

        if (!above_successor || !crossings ||
            !(is_crossable_wire(lyt, current, successor) || *above_successor == target) ||
            (routing_coordinate_obstructed(lyt, *above_successor, extra) && *above_successor != target))
        {
            return std::nullopt;
        }

        successor = *above_successor;
    }

    if (routing_connection_obstructed(lyt, current, successor, extra))
    {
        return std::nullopt;
    }

    return successor;
}
}  // namespace detail

/**
 * @brief Routes a path to one explicit logical input without changing other input slots.
 *
 * Occupied intermediate ground coordinates use their free crossing layer. Endpoints and all intermediate
 * placements are checked before mutation. A failed allocation removes newly created wires.
 * @tparam Geometry Coordinate geometry of the gate-level layout.
 * @param lyt Layout to edit.
 * @param path Path containing at least its occupied source and target coordinates.
 * @param destination Ordered destination input.
 * @throws std::invalid_argument If an endpoint or intermediate placement is unavailable.
 * @throws std::out_of_range If the destination input index is invalid.
 */
template <typename Geometry>
void route_path(fiction::layouts::gate_layout<Geometry>& lyt, const layout_coordinate_path& path,
                const typename fiction::layouts::gate_layout<Geometry>::input_port destination)
{
    if (path.size() < 2 || path.target() != lyt.get_tile(destination.object))
    {
        throw std::invalid_argument("A routing path requires matching occupied endpoints");
    }
    static_cast<void>(lyt.source(destination));
    const auto source = lyt.find_object(path.source());
    if (!source)
    {
        throw std::invalid_argument("The routing source is empty");
    }
    std::vector<fiction::layouts::layout_base::coordinate> positions{};
    positions.reserve(path.size() - 2);
    std::unordered_set<fiction::layouts::layout_base::coordinate> unique{};
    for (auto it = path.cbegin() + 1; it != path.cend() - 1; ++it)
    {
        auto position = *it;
        if (!lyt.is_empty_tile(position))
        {
            const auto crossing = lyt.above(position);
            if (!crossing)
            {
                throw std::invalid_argument("The crossing layer is unavailable");
            }
            position = *crossing;
        }
        if (!lyt.is_empty_tile(position) || !unique.insert(position).second)
        {
            throw std::invalid_argument("A routing path contains an occupied or repeated intermediate position");
        }
        positions.push_back(position);
    }
    auto                                                                     incoming = *source;
    std::vector<typename fiction::layouts::gate_layout<Geometry>::object_id> created{};
    created.reserve(positions.size());
    try
    {
        for (const auto& position : positions)
        {
            incoming = lyt.create_buf(incoming, position);
            created.push_back(incoming);
        }
        lyt.connect(incoming, destination);
    }
    catch (...)
    {
        for (const auto id : created)
        {
            lyt.remove(id);
        }
        throw;
    }
}
/**
 * @brief Extracts connections between retained gates, fanouts, and terminals with destination input indices.
 *
 * Intermediate single-sink wires are followed through declared topology. Missing inputs produce no objective;
 * disconnected input slots keep their indices. Cycles of intermediate wires reject.
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to inspect.
 * @return Routing objectives that preserve logical input numbering.
 * @throws std::invalid_argument If an intermediate wire chain contains a cycle.
 */
template <typename Lyt>
    requires requires { requires std::same_as<typename Lyt::object_id, layouts::layout_object_id>; }
std::vector<routing_objective> extract_routing_objectives(const Lyt& lyt)
{
    const auto intermediate = [&lyt](const auto id) { return lyt.is_wire(id) && !lyt.is_fanout(id); };
    std::vector<routing_objective> objectives{};
    lyt.foreach_object(
        [&](const auto id)
        {
            if (intermediate(id))
            {
                return;
            }
            lyt.foreach_fanin(id,
                              [&](const auto port, const auto input)
                              {
                                  auto                                        source = port;
                                  std::unordered_set<typename Lyt::object_id> visited{};
                                  while (intermediate(source))
                                  {
                                      if (!visited.insert(source).second)
                                      {
                                          throw std::invalid_argument("A routing wire chain contains a cycle");
                                      }
                                      const auto previous = lyt.source({source, 0});
                                      if (!previous)
                                      {
                                          return;
                                      }
                                      source = *previous;
                                  }
                                  objectives.push_back({lyt.get_tile(source), lyt.get_tile(id), input});
                              });
        });
    return objectives;
}
/**
 * @brief Removes routing wires and disconnects retained objects while preserving identities and input indices.
 * @tparam Geometry Coordinate geometry of the gate-level layout.
 * @param lyt Layout to edit.
 */
template <typename Geometry>
void clear_routing(fiction::layouts::gate_layout<Geometry>& lyt)
{
    std::vector<typename fiction::layouts::gate_layout<Geometry>::object_id> wires{};
    lyt.foreach_object(
        [&](const auto id)
        {
            if (lyt.is_wire(id) && !lyt.is_fanout(id))
            {
                wires.push_back(id);
            }
        });
    for (const auto id : wires)
    {
        lyt.remove(id);
    }
    lyt.foreach_object(
        [&](const auto id)
        {
            for (uint32_t input{}; input < lyt.input_count(id); ++input)
            {
                lyt.disconnect({id, input});
            }
        });
}

}  // namespace fiction::physical_design
