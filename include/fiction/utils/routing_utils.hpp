//
// Created by marcel on 07.02.22.
//

#ifndef FICTION_ROUTING_UTILS_HPP
#define FICTION_ROUTING_UTILS_HPP

#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <functional>
#include <vector>

namespace fiction
{

/**
 * Routing objectives are source-target pairs.
 *
 * @tparam Lyt Layout type whose coordinates are to be used.
 */
template <typename Lyt>
struct routing_objective
{
    /**
     * Source and target of the objective.
     */
    const coordinate<Lyt> source, target;
    /**
     * Equality operator.
     * @tparam OtherLyt Type of other layout.
     * @param other Routing objective to compare to.
     * @return True iff the given objective is equal to this one.
     */
    template <typename OtherLyt>
    bool operator==(const routing_objective<OtherLyt>& other) const noexcept
    {
        return source == other.source && target == other.target;
    }
};
/**
 * A path in a layout defined as a ordered sequence of coordinates.
 *
 * @tparam Lyt Coordinate layout type.
 */
template <typename Lyt>
class layout_coordinate_path : public std::vector<coordinate<Lyt>>
{
  public:
    void append(const coordinate<Lyt>& c) noexcept
    {
        this->push_back(c);
    }

    [[nodiscard]] coordinate<Lyt> source() const noexcept
    {
        return this->empty() ? coordinate<Lyt>{} : this->front();
    }

    [[nodiscard]] coordinate<Lyt> target() const noexcept
    {
        return this->empty() ? coordinate<Lyt>{} : this->back();
    }
};
/**
 * A collection of multiple paths in a layout.
 *
 * @tparam Path Path type.
 */
template <typename Path>
class path_collection : public std::vector<Path>
{
  public:
    void add(const Path& p) noexcept
    {
        this->push_back(p);
    }

    [[nodiscard]] bool contains(const Path& p) const noexcept
    {
        return std::find(std::cbegin(*this), std::cend(*this), p) != std::cend(*this);
    }
};
/**
 * Establishes a wire routing along the given path in the given layout. To this end, the given path's source and target
 * coordinates are assumed to be populated by other gates or wires that the new path shall connect to.
 *
 * This function overwrites any nodes that might already be placed at any of the path's coordinates other than source
 * and target. If path contains exactly source and target, no wires are created but source and target are connected.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Path Path type.
 * @param lyt Gate-level layout in which a wire path is to be established.
 * @param path Path to route wires along.
 */
template <typename Lyt, typename Path>
void route_path(Lyt& lyt, const Path& path) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    auto incoming_signal = static_cast<mockturtle::signal<Lyt>>(path.source());

    // exclude source and target
    std::for_each(path.cbegin() + 1, path.cend() - 1,
                  [&lyt, &incoming_signal](const auto& coord)
                  { incoming_signal = lyt.create_buf(incoming_signal, coord); });

    // establish final connection to target node
    lyt.connect(incoming_signal, lyt.get_node(path.target()));
}
/**
 * Extracts all routing objectives from the given layout. To this end, all routing paths in the layout are traversed,
 * starting at each PI. Whenever the next regular node (non-IO, non-constant, non-wire) is encountered, this connection
 * is added to the list of all objectives.
 *
 * Example: Let a layout have connections from (0,0) to (2,3) via a cascade of wires and a direct connection from (2,2)
 * to (2,3). The list of routing objectives extracted from that layout would contain {(0,0), (2,3)} and {(2,2), (2,3)}.
 *
 * In other words, if all wires were removed from the layout and all connections ripped-up, an equivalent layout could
 * be recreated from the list of routing objectives.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout whose routing objectives are to be extracted.
 * @return List of all routing objectives in the given layout.
 */
template <typename Lyt>
std::vector<routing_objective<Lyt>> extract_routing_objectives(const Lyt& lyt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    // extracted routing objectives
    std::vector<routing_objective<Lyt>> objectives{};
    // list of visited nodes
    std::vector<bool> visited(lyt.size(), false);
    // checks if a node is an intermediate routing wire
    const auto is_connection_wire = [&lyt](const auto& n)
    { return lyt.is_wire(n) && !lyt.is_fanout(n) && !lyt.is_pi(n) && !lyt.is_po(n); };
    // recursively traverse the layout paths and gather routing objectives
    const std::function<void(const tile<Lyt>&, const mockturtle::node<Lyt>&)> recursively_traverse_paths =
        [&](const auto& recent_gate_tile, const auto& current_node)
    {
        auto current_gate_tile = recent_gate_tile;

        if (!is_connection_wire(current_node))  // is regular gate, fan-out, or I/O
        {
            current_gate_tile = lyt.get_tile(current_node);
            // objective found
            objectives.push_back({recent_gate_tile, current_gate_tile});
        }

        // node already visited
        if (visited[current_node])
        {
            return;
        }
        // mark node as visited
        visited[current_node] = true;

        // recursively traverse successors
        lyt.foreach_fanout(current_node, [&](const auto& fon) { recursively_traverse_paths(current_gate_tile, fon); });
    };

    // start recursion at each PI
    lyt.foreach_pi(
        [&](const auto& pi)
        {
            const auto pi_tile = lyt.get_tile(pi);
            lyt.foreach_fanout(pi, [&](const auto& fon) { recursively_traverse_paths(pi_tile, fon); });
        });

    return objectives;
}

}  // namespace fiction

#endif  // FICTION_ROUTING_UTILS_HPP
