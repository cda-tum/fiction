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
 * @brief Planar Layout from Network Embedding (PLANE): placement and routing of a planar, ranked logic network.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/physical_design/placement_utils.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/graph/mincross.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::physical_design
{

/**
 * Parameters for the planar placement and routing.
 */
struct plane_params
{
    /**
     * Number of clock phases of the 2DDWave clocking scheme. Three and four are supported.
     */
    layouts::clocking::num_clks number_of_clock_phases = layouts::clocking::num_clks::FOUR;
    /**
     * Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
};
/**
 * Statistics of the planar placement and routing.
 */
struct plane_stats
{
    /**
     * Runtime of placement and routing.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Layout width.
     */
    uint64_t x_size{0ull};
    /**
     * Layout height.
     */
    uint64_t y_size{0ull};
    /**
     * Number of gates.
     */
    uint64_t num_gates{0ull};
    /**
     * Number of wires.
     */
    uint64_t num_wires{0ull};
    /**
     * Number of crossings; zero for every layout this algorithm returns.
     */
    uint64_t num_crossings{0ull};
    /**
     * Writes the statistics to a stream.
     *
     * @param out Stream to write to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time      = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size     = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates      = {}\n", num_gates);
        out << fmt::format("[i] num. wires      = {}\n", num_wires);
        out << fmt::format("[i] num. crossings  = {}\n", num_crossings);
    }
};

namespace detail
{

/**
 * Direction in which a node is placed relative to the end of its predecessor's wire. The lookup tables of the
 * placement model distinguish two variants per direction, which the placement treats alike; the variant only steers
 * the entries chosen for the following node.
 */
enum class orientation : uint8_t
{
    /**
     * East of the wire end.
     */
    EAST = 0,
    /**
     * East of the wire end, second variant.
     */
    EAST_SECOND = 1,
    /**
     * South of the wire end.
     */
    SOUTH = 2,
    /**
     * South of the wire end, second variant.
     */
    SOUTH_SECOND = 3
};
/**
 * Whether an orientation places a node east of its predecessor's wire end.
 *
 * @param o Orientation.
 * @return `true` iff the node goes east.
 */
[[nodiscard]] constexpr bool is_east(const orientation o) noexcept
{
    return o == orientation::EAST || o == orientation::EAST_SECOND;
}
/**
 * Directions a node may take because of the fanout that drives it.
 */
enum class allowed_orientation : uint8_t
{
    /**
     * The node is the first consumer of a fanout and goes east.
     */
    EAST = 0,
    /**
     * The node is the second consumer of a fanout and goes south.
     */
    SOUTH = 1,
    /**
     * The node is not driven by a fanout.
     */
    FREE = 2
};
/**
 * How the two consumers of a fanout node connect: whether each one is a single-fanin node or a two-input gate.
 */
enum class fanout_connection : uint8_t
{
    /**
     * Both consumers have a single fanin.
     */
    BOTH_SINGLE = 0,
    /**
     * The first consumer has a single fanin, the second is a two-input gate.
     */
    FIRST_SINGLE = 1,
    /**
     * The second consumer has a single fanin, the first is a two-input gate.
     */
    SECOND_SINGLE = 2,
    /**
     * Both consumers are two-input gates.
     */
    BOTH_GATES = 3
};
/**
 * Whether a single-fanin node drives the rightmost fanin of a two-input gate.
 */
enum class buffer_connection : uint8_t
{
    /**
     * The node drives a single-fanin node, the leftmost fanin of a gate, or an output.
     */
    UNCONNECTED = 0,
    /**
     * The node drives the rightmost fanin of a two-input gate.
     */
    CONNECTED = 1
};
/**
 * Largest number of free rows between a node's predecessor and the predecessor's left neighbour that the lookup
 * tables distinguish.
 */
inline constexpr std::size_t max_gap = 2u;
/**
 * One entry of a lookup table: the orientation of the node and the routing lines it needs.
 */
struct placement_rule
{
    /**
     * Orientation of the node.
     */
    orientation direction;
    /**
     * New routing lines the node needs.
     */
    uint64_t new_lines;
};
/**
 * Rules indexed by the orientation of the previous node in the rank.
 */
using rules_by_previous = std::array<placement_rule, 4>;
/**
 * Rules indexed by the predecessor gap, then by the previous orientation.
 */
using rules_by_gap = std::array<rules_by_previous, max_gap + 1>;
/**
 * Rules indexed by the allowed orientation, then by gap and previous orientation.
 */
using rules_by_allowed = std::array<rules_by_gap, 3>;
/**
 * Shorthand for the orientation enumerators of the tables below.
 */
using o = orientation;
/**
 * Placement rules for single-fanin nodes that are not fanouts, indexed by `buffer_connection`, `allowed_orientation`,
 * gap, and the orientation of the previous node.
 */
inline constexpr std::array<rules_by_allowed, 2> buffer_rules = {{
    // UNCONNECTED
    {{
        // allowed EAST
        {{
            {{{o::EAST, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 1}, {o::EAST, 1}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST_SECOND, 0}, {o::EAST, 0}}},         // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST, 0}, {o::EAST_SECOND, 0}, {o::SOUTH, 0}, {o::SOUTH_SECOND, 0}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST_SECOND, 0}, {o::EAST, 0}}},           // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                  // gap 2 and more
        }},
    }},
    // CONNECTED
    {{
        // allowed EAST
        {{
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 1}, {o::EAST, 1}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST, 0}, {o::EAST, 0}, {o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                  // gap 2 and more
        }},
    }},
}};
/**
 * Placement rules for fanout nodes, indexed by `fanout_connection`, `allowed_orientation`, gap, and the orientation
 * of the previous node.
 */
inline constexpr std::array<rules_by_allowed, 4> fanout_rules = {{
    // BOTH_SINGLE
    {{
        // allowed EAST
        {{
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 1}, {o::EAST_SECOND, 2}, {o::EAST_SECOND, 1}}},  // gap 0
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 1}, {o::EAST_SECOND, 0}}},  // gap 1
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH, 0}, {o::SOUTH, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH, 0}, {o::SOUTH, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH, 0}, {o::SOUTH, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST_SECOND, 0}, {o::SOUTH, 0}, {o::SOUTH, 1}, {o::SOUTH, 0}}},                    // gap 0
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::SOUTH, 0}, {o::EAST_SECOND, 0}}},        // gap 1
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 2 and more
        }},
    }},
    // FIRST_SINGLE
    {{
        // allowed EAST
        {{
            {{{o::EAST, 0}, {o::EAST, 1}, {o::EAST, 2}, {o::EAST, 1}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 1}, {o::EAST, 0}}},  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 1}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST, 0}, {o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 1}, {o::SOUTH_SECOND, 0}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}}},                  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                          // gap 2 and more
        }},
    }},
    // SECOND_SINGLE
    {{
        // allowed EAST
        {{
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 1}, {o::EAST_SECOND, 1}}},  // gap 0
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 1
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH, 0}, {o::SOUTH, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::SOUTH, 0}, {o::SOUTH, 0}}},              // gap 0
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 1
            {{{o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}, {o::EAST_SECOND, 0}}},  // gap 2 and more
        }},
    }},
    // BOTH_GATES
    {{
        // allowed EAST
        {{
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed SOUTH
        {{
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 0
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 1
            {{{o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}, {o::EAST, 0}, {o::EAST, 0}}},  // gap 2 and more
        }},
        // allowed FREE
        {{
            {{{o::EAST, 0}, {o::EAST, 0}, {o::SOUTH_SECOND, 0}, {o::SOUTH_SECOND, 0}}},  // gap 0
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                  // gap 1
            {{{o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}, {o::EAST, 0}}},                  // gap 2 and more
        }},
    }},
}};
/**
 * Converts an enumerator to the index it stands for in the lookup tables.
 *
 * @tparam E Enumeration type.
 * @param e Enumerator.
 * @return Index.
 */
template <typename E>
[[nodiscard]] constexpr std::size_t index_of(const E e) noexcept
{
    return static_cast<std::size_t>(e);
}
/**
 * Number of non-constant fanins of a node.
 *
 * @tparam Ntk Network type.
 * @param ntk Network.
 * @param n Node.
 * @return Non-constant fanins.
 */
template <typename Ntk>
[[nodiscard]] std::size_t num_fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    return networks::fanins(ntk, n).fanin_nodes.size();
}
/**
 * Non-constant fanins of a node sorted by rank position.
 *
 * @tparam Ntk Ranked network type.
 * @param ntk Network.
 * @param n Node.
 * @return Fanins of `n`, leftmost first.
 */
template <typename Ntk>
[[nodiscard]] std::vector<mockturtle::node<Ntk>> ranked_fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    auto fanin_nodes = networks::fanins(ntk, n).fanin_nodes;

    std::sort(fanin_nodes.begin(), fanin_nodes.end(),
              [&ntk](const auto& a, const auto& b) { return ntk.rank_position(a) < ntk.rank_position(b); });

    return fanin_nodes;
}
/**
 * Fanouts of a node sorted by rank position.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @param ntk Network.
 * @param n Node.
 * @return Fanouts of `n`, leftmost first.
 */
template <typename Ntk>
[[nodiscard]] std::vector<mockturtle::node<Ntk>> ranked_fanouts(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    auto fanout_nodes = ntk.fanout(n);

    std::sort(fanout_nodes.begin(), fanout_nodes.end(),
              [&ntk](const auto& a, const auto& b) { return ntk.rank_position(a) < ntk.rank_position(b); });

    return fanout_nodes;
}
/**
 * Number of free rows between a tile and another tile south of it.
 *
 * @tparam Lyt Layout type.
 * @param north Upper tile.
 * @param south Lower tile.
 * @return `south.y - north.y - 1`.
 * @throws std::logic_error If `south` is not south of `north`.
 */
template <typename Lyt>
[[nodiscard]] uint64_t rows_between(const tile<Lyt>& north, const tile<Lyt>& south)
{
    if (south.y <= north.y)
    {
        throw std::logic_error(fmt::format("tile ({},{}) is not south of tile ({},{})", static_cast<uint64_t>(south.x),
                                           static_cast<uint64_t>(south.y), static_cast<uint64_t>(north.x),
                                           static_cast<uint64_t>(north.y)));
    }

    return static_cast<uint64_t>(south.y) - static_cast<uint64_t>(north.y) - 1;
}
/**
 * How the two consumers of a fanout node connect.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @param ntk Network.
 * @param n Fanout node.
 * @return Connection type; `BOTH_SINGLE` for a fanout that drives an output.
 * @throws std::logic_error If `n` does not drive exactly two nodes.
 */
template <typename Ntk>
[[nodiscard]] fanout_connection fanout_connection_of(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    if (ntk.is_po(n))
    {
        return fanout_connection::BOTH_SINGLE;
    }

    const auto fo = ranked_fanouts(ntk, n);

    if (fo.size() != 2)
    {
        throw std::logic_error(fmt::format("fanout node {} drives {} nodes instead of two", n, fo.size()));
    }

    const bool first_single  = num_fanins(ntk, fo[0]) == 1;
    const bool second_single = num_fanins(ntk, fo[1]) == 1;

    if (first_single && second_single)
    {
        return fanout_connection::BOTH_SINGLE;
    }
    if (first_single)
    {
        return fanout_connection::FIRST_SINGLE;
    }
    if (second_single)
    {
        return fanout_connection::SECOND_SINGLE;
    }

    return fanout_connection::BOTH_GATES;
}
/**
 * Whether a single-fanin node drives the rightmost fanin of a two-input gate.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @param ntk Network.
 * @param n Single-fanin node.
 * @return Connection type.
 */
template <typename Ntk>
[[nodiscard]] buffer_connection buffer_connection_of(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    if (ntk.is_po(n))
    {
        return buffer_connection::UNCONNECTED;
    }

    const auto fo = ntk.fanout(n);

    if (fo.size() == 1 && num_fanins(ntk, fo[0]) == 2 && ranked_fanins(ntk, fo[0])[1] == n)
    {
        return buffer_connection::CONNECTED;
    }

    return buffer_connection::UNCONNECTED;
}
/**
 * Direction a single-fanin node may take: fixed by its position among the consumers of a driving fanout, free
 * otherwise.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @param ntk Network.
 * @param n Single-fanin node.
 * @return Allowed orientation.
 */
template <typename Ntk>
[[nodiscard]] allowed_orientation allowed_orientation_of(const Ntk& ntk, const mockturtle::node<Ntk>& n)
{
    const auto pre = networks::fanins(ntk, n).fanin_nodes.front();

    if (!ntk.is_fanout(pre))
    {
        return allowed_orientation::FREE;
    }

    return ranked_fanouts(ntk, pre)[0] == n ? allowed_orientation::EAST : allowed_orientation::SOUTH;
}
/**
 * Free rows between the leftmost predecessor of a node and that predecessor's left neighbour in the rank below,
 * clamped to `max_gap`: the room available for placing the node.
 *
 * @tparam Ntk Ranked network type.
 * @tparam Lyt Layout type.
 * @param ntk Network.
 * @param node2pos Positions of the placed nodes.
 * @param lvl Level of `n`.
 * @param n Node.
 * @return Gap; zero on the input level and for a predecessor without left neighbour.
 */
template <typename Ntk, typename Lyt>
[[nodiscard]] std::size_t predecessor_gap(const Ntk&                                                ntk,
                                          const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                          const uint32_t lvl, const mockturtle::node<Ntk>& n)
{
    if (lvl == 0)
    {
        return 0;
    }

    const auto pre = ranked_fanins(ntk, n).front();
    const auto r   = ntk.rank_position(pre);

    if (r == 0)
    {
        return 0;
    }

    const auto neighbour = ntk.at_rank_position(lvl - 1, r - 1);

    const auto gap =
        rows_between<Lyt>(static_cast<tile<Lyt>>(node2pos[neighbour]), static_cast<tile<Lyt>>(node2pos[pre]));

    return static_cast<std::size_t>(std::min<uint64_t>(gap, max_gap));
}
/**
 * Orientation of the first node of a level: south in the second variant if the first fanout structure or gate of
 * the level is a two-input gate, east otherwise.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @param ntk Network.
 * @param lvl Level.
 * @return Orientation of the first node.
 */
template <typename Ntk>
[[nodiscard]] orientation start_orientation(const Ntk& ntk, const uint32_t lvl)
{
    if (lvl == 0)
    {
        return orientation::EAST;
    }

    std::optional<orientation> found{};

    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &found](const auto& n)
                             {
                                 if (found.has_value())
                                 {
                                     return;
                                 }

                                 const auto fanin_nodes = networks::fanins(ntk, n).fanin_nodes;

                                 if (fanin_nodes.size() == 2)
                                 {
                                     found = orientation::SOUTH_SECOND;
                                     return;
                                 }

                                 if (fanin_nodes.size() == 1 && ntk.is_fanout(fanin_nodes.front()) &&
                                     ntk.fanout_size(fanin_nodes.front()) == 2)
                                 {
                                     found = orientation::EAST;
                                     return;
                                 }

                                 if (ntk.fanout_size(n) == 2)
                                 {
                                     found = orientation::EAST;
                                 }
                             });

    return found.value_or(orientation::EAST);
}
/**
 * Orientation and routing-line demand of every node of a level.
 */
struct level_variables
{
    /**
     * Orientation per node of the level.
     */
    std::vector<orientation> orientations;
    /**
     * New routing lines per node of the level.
     */
    std::vector<uint64_t> new_lines;
};
/**
 * Computes the orientation and the new routing lines of every node of a level from the lookup tables.
 *
 * @tparam Ntk Ranked network type with a fanout interface.
 * @tparam Lyt Layout type.
 * @param ntk Network.
 * @param node2pos Positions of the placed nodes.
 * @param lvl Level.
 * @return Orientations and new lines, aligned with the rank order of the level.
 */
template <typename Ntk, typename Lyt>
[[nodiscard]] level_variables
compute_level_variables(const Ntk& ntk, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                        const uint32_t lvl)
{
    level_variables vars{std::vector<orientation>(ntk.rank_width(lvl), orientation::EAST),
                         std::vector<uint64_t>(ntk.rank_width(lvl), 0)};

    ntk.foreach_node_in_rank(
        lvl,
        [&ntk, &node2pos, lvl, &vars](const auto& n, const auto i)
        {
            const auto gap = predecessor_gap<Ntk, Lyt>(ntk, node2pos, lvl, n);

            if (num_fanins(ntk, n) == 2)
            {
                // a gate after a south-oriented node without room between their predecessors turns east
                if (i != 0 && vars.orientations[i - 1] == orientation::SOUTH && gap == 0)
                {
                    vars.orientations[i] = orientation::EAST_SECOND;
                }

                return;
            }

            if (num_fanins(ntk, n) != 1)
            {
                return;
            }

            const auto allowed = allowed_orientation_of(ntk, n);

            if (ntk.is_fanout(n))
            {
                const auto connection = fanout_connection_of(ntk, n);

                if (i != 0)
                {
                    const auto& rule =
                        fanout_rules[index_of(connection)][index_of(allowed)][gap][index_of(vars.orientations[i - 1])];
                    vars.orientations[i] = rule.direction;
                    vars.new_lines[i]    = rule.new_lines;
                }
                else if (connection == fanout_connection::BOTH_SINGLE || connection == fanout_connection::SECOND_SINGLE)
                {
                    vars.orientations[i] = orientation::EAST_SECOND;
                }

                return;
            }

            if (i != 0)
            {
                const auto& rule     = buffer_rules[index_of(buffer_connection_of(ntk, n))][index_of(allowed)][gap]
                                                   [index_of(vars.orientations[i - 1])];
                vars.orientations[i] = rule.direction;
                vars.new_lines[i]    = rule.new_lines;
            }
            else
            {
                vars.orientations[i] = start_orientation(ntk, lvl);
            }
        });

    return vars;
}
/**
 * Rank positions of the two-input gates of a level.
 *
 * @tparam Ntk Ranked network type.
 * @param ntk Network.
 * @param lvl Level.
 * @return Positions, ascending.
 */
template <typename Ntk>
[[nodiscard]] std::vector<std::size_t> two_input_indices(const Ntk& ntk, const uint32_t lvl)
{
    std::vector<std::size_t> indices{};
    indices.reserve(ntk.rank_width(lvl));

    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &indices](const auto& n, const auto i)
                             {
                                 if (num_fanins(ntk, n) == 2)
                                 {
                                     indices.push_back(i);
                                 }
                             });

    return indices;
}
/**
 * Rows between the two predecessors of every two-input gate of a level: the diagonal advance each gate brings on
 * its own.
 *
 * @tparam Ntk Ranked network type.
 * @tparam Lyt Layout type.
 * @param ntk Network.
 * @param node2pos Positions of the placed nodes.
 * @param lvl Level.
 * @return Rows per gate, in the order of `two_input_indices`.
 */
template <typename Ntk, typename Lyt>
[[nodiscard]] std::vector<uint64_t>
two_input_new_lines(const Ntk& ntk, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                    const uint32_t lvl)
{
    std::vector<uint64_t> lines{};
    lines.reserve(ntk.rank_width(lvl));

    ntk.foreach_node_in_rank(lvl,
                             [&ntk, &node2pos, &lines](const auto& n)
                             {
                                 const auto pre = ranked_fanins(ntk, n);

                                 if (pre.size() != 2)
                                 {
                                     return;
                                 }

                                 lines.push_back(rows_between<Lyt>(static_cast<tile<Lyt>>(node2pos[pre[0]]),
                                                                   static_cast<tile<Lyt>>(node2pos[pre[1]])));
                             });

    return lines;
}
/**
 * Wire lengths of every node of a level: how far its predecessor's signal runs east and south before the node is
 * placed.
 */
struct level_wiring
{
    /**
     * East run per node.
     */
    std::vector<uint64_t> x;
    /**
     * South run per node.
     */
    std::vector<uint64_t> y;
};
/**
 * Lifts every node of a level onto the diagonal with the largest demand. The demand of a node is `x + y`, that of a
 * two-input gate additionally the rows between its predecessors, which the gate advances on its own. Nodes left of
 * the node with the largest demand get the difference as south run, nodes right of it as east run.
 *
 * @param wiring Wire lengths; adjusted.
 * @param gates Rank positions of the two-input gates.
 * @param gate_lines Rows between the predecessors of every gate, in the order of `gates`.
 */
inline void lift_to_common_diagonal(level_wiring& wiring, const std::vector<std::size_t>& gates,
                                    const std::vector<uint64_t>& gate_lines)
{
    auto& x = wiring.x;
    auto& y = wiring.y;

    const auto max_gate_it  = std::max_element(gate_lines.cbegin(), gate_lines.cend());
    const auto max_gate     = max_gate_it != gate_lines.cend() ? *max_gate_it : uint64_t{0};
    const auto max_gate_idx = max_gate_it != gate_lines.cend() ?
                                  static_cast<std::size_t>(std::distance(gate_lines.cbegin(), max_gate_it)) :
                                  std::size_t{0};

    std::size_t max_xy_idx = 0;
    uint64_t    max_xy     = 0;
    for (std::size_t i = 0; i < x.size(); ++i)
    {
        if (const auto sum = x[i] + y[i]; i == 0 || sum > max_xy)
        {
            max_xy     = sum;
            max_xy_idx = i;
        }
    }

    std::size_t center = 0;
    uint64_t    max    = 0;
    if (max_xy > max_gate)
    {
        center = max_xy_idx;
        max    = max_xy;
    }
    else
    {
        center = gates.empty() ? 0 : gates[max_gate_idx];
        max    = max_gate;
    }

    std::unordered_map<std::size_t, std::size_t> gate_of_position{};
    for (std::size_t i = 0; i < gates.size(); ++i)
    {
        gate_of_position[gates[i]] = i;
    }

    for (std::size_t i = 0; i < x.size(); ++i)
    {
        const auto it   = gate_of_position.find(i);
        const auto own  = it != gate_of_position.cend() ? gate_lines[it->second] : uint64_t{0};
        const auto diff = max - x[i] - y[i] - own;

        if (i < center)
        {
            y[i] += diff;
        }
        else if (i > center)
        {
            x[i] += diff;
        }
    }
}
/**
 * Computes the wire lengths of every node of a level.
 *
 * Two-input gates split the level into clusters. Inside a cluster a node runs east by the new lines of the nodes
 * right of it and south by the new lines of the nodes left of it, so the cluster sits on one diagonal. A gate advances
 * the diagonal by the rows between its predecessors; the surplus of a cluster's lines over that advance is propagated
 * to the clusters right of it as south runs and to the clusters left of it as east runs. Finally all nodes are lifted
 * onto the largest diagonal.
 *
 * @tparam Ntk Ranked network type.
 * @tparam Lyt Layout type.
 * @param ntk Network.
 * @param node2pos Positions of the placed nodes.
 * @param new_lines New lines per node.
 * @param lvl Level.
 * @return East and south runs per node.
 */
template <typename Ntk, typename Lyt>
[[nodiscard]] level_wiring compute_wiring(const Ntk&                                                ntk,
                                          const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos,
                                          const std::vector<uint64_t>& new_lines, const uint32_t lvl)
{
    const auto gates      = two_input_indices(ntk, lvl);
    const auto gate_lines = two_input_new_lines<Ntk, Lyt>(ntk, node2pos, lvl);

    const auto width = new_lines.size();

    level_wiring wiring{std::vector<uint64_t>(width, 0), std::vector<uint64_t>(width, 0)};
    auto&        x = wiring.x;
    auto&        y = wiring.y;

    // the lines of a cluster: those right of a node go east, those left of it go south
    const auto cluster_sums =
        [&x, &y, &new_lines](const std::size_t begin, const std::size_t end, const uint64_t south_offset)
    {
        for (auto j = begin; j < end; ++j)
        {
            x[j] = std::accumulate(new_lines.cbegin() + static_cast<std::ptrdiff_t>(j + 1),
                                   new_lines.cbegin() + static_cast<std::ptrdiff_t>(end), uint64_t{0});
            y[j] = (j == 0 ? uint64_t{0} : y[j - 1]) + new_lines[j] + (j == begin ? south_offset : uint64_t{0});
        }
    };

    if (gates.empty())
    {
        cluster_sums(0, width, 0);

        return wiring;
    }

    // surplus lines of a cluster over the advance of the gate right of it, carried on as south runs
    std::vector<uint64_t> cluster_lines(gates.size() + 1, 0);
    std::size_t           begin           = 0;
    uint64_t              propagate_right = 0;

    for (std::size_t c = 0; c <= gates.size(); ++c)
    {
        const auto end = c == gates.size() ? width : gates[c];

        cluster_sums(begin, end, propagate_right);

        if (begin != 0)
        {
            y[begin - 1] += propagate_right;  // the gate left of the cluster
        }

        if (begin != end)
        {
            cluster_lines[c] = x[begin];
        }

        const auto advance = c < gate_lines.size() ? gate_lines[c] : uint64_t{0};
        propagate_right =
            advance > cluster_lines[c] + propagate_right ? 0 : propagate_right + cluster_lines[c] - advance;

        begin = end + 1;
    }

    // surplus lines of a cluster over the advance of the gate left of it, carried on as east runs
    uint64_t propagate_left = 0;

    for (auto c = gates.size(); c > 0; --c)
    {
        const auto end     = gates[c - 1];
        const auto first   = c == 1 ? std::size_t{0} : gates[c - 2] + 1;
        const auto advance = gate_lines[c - 1];

        propagate_left = advance > cluster_lines[c] + propagate_left ? 0 : propagate_left + cluster_lines[c] - advance;

        if (end != width - 1)
        {
            x[end] += propagate_left;  // the gate itself
        }

        for (auto j = first; j < end; ++j)
        {
            x[j] += propagate_left;
        }
    }

    lift_to_common_diagonal(wiring, gates, gate_lines);

    return wiring;
}
/**
 * Continues signal `a` on tile `t` with a buffer. A tile that already holds a wire driven by `a` is shared, which
 * turns the tile of `a` into a layout fanout; any other occupant is a routing conflict.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout.
 * @param a Signal to continue.
 * @param t Tile.
 * @return Signal of the buffer on `t`.
 * @throws std::logic_error If `t` holds anything but a wire driven by `a`.
 */
template <typename Lyt>
[[nodiscard]] mockturtle::signal<Lyt> reuse_or_create_buf(Lyt& lyt, const mockturtle::signal<Lyt>& a,
                                                          const tile<Lyt>& t)
{
    if (lyt.is_empty_tile(t))
    {
        return lyt.create_buf(a, t);
    }

    const auto occupant    = lyt.get_node(t);
    bool       driven_by_a = false;

    if (lyt.is_buf(occupant) && lyt.fanin_size(occupant) == 1)
    {
        lyt.foreach_fanin(occupant, [&lyt, &a, &driven_by_a](const auto& f)
                          { driven_by_a = lyt.get_node(f) == lyt.get_node(a); });
    }

    if (!driven_by_a)
    {
        throw std::logic_error(fmt::format("tile ({},{}) is occupied by another signal", static_cast<uint64_t>(t.x),
                                           static_cast<uint64_t>(t.y)));
    }

    return static_cast<mockturtle::signal<Lyt>>(t);
}
/**
 * Wires the signal on `src` in a straight line towards `dest`, east or south, up to the tile before `dest`. Shared
 * wire tiles are reused, see `reuse_or_create_buf`.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout.
 * @param src Tile that holds the signal.
 * @param dest Tile east or south of `src`.
 * @return Signal on the tile before `dest`; the signal of `src` if the tiles are adjacent.
 * @throws std::logic_error If `src` is empty or `dest` is neither east nor south of `src`.
 */
template <typename Lyt>
[[nodiscard]] mockturtle::signal<Lyt> wire_straight(Lyt& lyt, const tile<Lyt>& src, const tile<Lyt>& dest)
{
    if (lyt.is_empty_tile(src))
    {
        throw std::logic_error(
            fmt::format("wire source ({},{}) is empty", static_cast<uint64_t>(src.x), static_cast<uint64_t>(src.y)));
    }

    const bool east  = src.y == dest.y && dest.x > src.x;
    const bool south = src.x == dest.x && dest.y > src.y;

    if (!east && !south)
    {
        throw std::logic_error(fmt::format("tile ({},{}) is neither east nor south of ({},{})",
                                           static_cast<uint64_t>(dest.x), static_cast<uint64_t>(dest.y),
                                           static_cast<uint64_t>(src.x), static_cast<uint64_t>(src.y)));
    }

    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    if (east)
    {
        for (auto x = src.x + 1; x < dest.x; ++x)
        {
            a = reuse_or_create_buf(lyt, a, tile<Lyt>{x, src.y, 0});
        }
    }
    else
    {
        for (auto y = src.y + 1; y < dest.y; ++y)
        {
            a = reuse_or_create_buf(lyt, a, tile<Lyt>{src.x, y, 0});
        }
    }

    return a;
}
/**
 * Runs the signal on `from` east and then south by the given lengths and returns the tile it ends on.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout.
 * @param from Tile that holds the signal.
 * @param east Length of the east run.
 * @param south Length of the south run that follows.
 * @return Tile the signal ends on.
 */
template <typename Lyt>
[[nodiscard]] tile<Lyt> wire_east_then_south(Lyt& lyt, tile<Lyt> from, const uint64_t east, const uint64_t south)
{
    if (east != 0)
    {
        [[maybe_unused]] const auto end = wire_straight(lyt, from, {from.x + east + 1, from.y});
        from.x += east;
    }

    if (south != 0)
    {
        [[maybe_unused]] const auto end = wire_straight(lyt, from, {from.x, from.y + south + 1});
        from.y += south;
    }

    return from;
}
/**
 * Places a single-fanin node on tile `t` and wires its predecessor to it.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Network type.
 * @param lyt Layout.
 * @param t Tile east or south of `pre_t`.
 * @param ntk Network.
 * @param n Node.
 * @param pre_t Tile that holds the predecessor's signal.
 * @return Signal of the placed node.
 * @throws std::logic_error If `t` is occupied or neither east nor south of `pre_t`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] mockturtle::signal<Lyt> connect_and_place_planar(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                                               const mockturtle::node<Ntk>& n, const tile<Lyt>& pre_t)
{
    if (!lyt.is_empty_tile(t))
    {
        throw std::logic_error(fmt::format("tile ({},{}) for node {} is occupied", static_cast<uint64_t>(t.x),
                                           static_cast<uint64_t>(t.y), n));
    }

    return place(lyt, t, ntk, n, wire_straight(lyt, pre_t, t));
}
/**
 * Places a two-input gate on tile `t` and wires its predecessors to it, the northern one from above and the western
 * one from the left.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Network type.
 * @param lyt Layout.
 * @param t Tile.
 * @param ntk Network.
 * @param n Node.
 * @param pre1_t Tile that holds one predecessor's signal.
 * @param pre2_t Tile that holds the other predecessor's signal.
 * @param c Value of a constant third fanin, if any.
 * @return Signal of the placed node.
 * @throws std::logic_error If `t` is occupied.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] mockturtle::signal<Lyt>
connect_and_place_planar(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, tile<Lyt> pre1_t,
                         tile<Lyt> pre2_t, const std::optional<bool>& c = std::nullopt)
{
    if (!lyt.is_empty_tile(t))
    {
        throw std::logic_error(fmt::format("tile ({},{}) for node {} is occupied", static_cast<uint64_t>(t.x),
                                           static_cast<uint64_t>(t.y), n));
    }

    if (pre2_t < pre1_t)
    {
        std::swap(pre1_t, pre2_t);
    }

    return place(lyt, t, ntk, n, wire_straight(lyt, pre1_t, t), wire_straight(lyt, pre2_t, t), c);
}

/**
 * Implementation of PLANE.
 *
 * Every level of the ranked network is placed on one anti-diagonal of a 2DDWave-clocked layout, the ranks running
 * from the top right to the bottom left. For each level the lookup tables give every node an orientation and a demand
 * for routing lines, `compute_wiring` turns the demands into the east and south runs of the predecessors' wires, and
 * the nodes are placed in rank order at the ends of those wires. The wire of a fanout node is shared by its consumers.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Ranked, planar network type.
 */
template <typename Lyt, typename Ntk>
class plane_impl
{
  public:
    /**
     * Network type seen by the implementation: the input wrapped in a fanout view.
     */
    using fanout_ntk = mockturtle::fanout_view<Ntk>;
    /**
     * Creates the implementation.
     *
     * @param src Network.
     * @param p Parameters.
     * @param st Statistics.
     */
    plane_impl(const Ntk& src, const plane_params& p, plane_stats& st) :
            ntk{src},
            ps{p},
            pst{st},
            node2pos{ntk},
            layout{{0, 0}, layouts::clocking::twoddwave(ps.number_of_clock_phases)}
    {}
    /**
     * Places and routes the network.
     *
     * @return The layout.
     * @throws std::logic_error If the placement breaks the diagonal invariant or a wire runs into an occupied tile.
     */
    [[nodiscard]] Lyt run()
    {
        const mockturtle::stopwatch stop{pst.time_total};

        // one step per level plus one for the outputs
        utils::progress_reporter progress{ps.on_progress, "placing levels", ntk.depth() + 2};

        pi2node = reserve_input_nodes(layout, ntk);

        // the storage is sparse, so the working area can be generous: every level advances the diagonal by at most
        // its width plus one, hence inputs plus nodes bound both dimensions. Shrunk to the bounding box at the end.
        const auto bound = static_cast<uint64_t>(ntk.num_pis()) + static_cast<uint64_t>(ntk.size()) + 1;
        layout.resize({bound, bound});

        first_pos = {ntk.num_pis() - 1, 0};

        for (uint32_t lvl = 0; lvl <= ntk.depth(); ++lvl)
        {
            place_level(lvl);
            progress.advance();
        }

        const auto extra_rows = route_outputs();
        progress.advance();

        layout.resize({first_pos.x + 1, place_t.y + extra_rows, 0});

        networks::restore_names(ntk, layout, node2pos);

        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        return layout;
    }

  private:
    /**
     * Places every node of a level in rank order.
     *
     * @param lvl Level.
     */
    void place_level(const uint32_t lvl)
    {
        const auto vars   = compute_level_variables<fanout_ntk, Lyt>(ntk, node2pos, lvl);
        const auto wiring = compute_wiring<fanout_ntk, Lyt>(ntk, node2pos, vars.new_lines, lvl);

        std::optional<uint64_t> diagonal{};

        ntk.foreach_node_in_rank(lvl,
                                 [this, &vars, &wiring, &diagonal, lvl](const auto& n, const auto i)
                                 {
                                     if (ntk.is_constant(n))
                                     {
                                         return;
                                     }

                                     if (ntk.is_pi(n))
                                     {
                                         place_input(n);
                                     }
                                     else if (num_fanins(ntk, n) == 1)
                                     {
                                         place_single_fanin(n, i, vars.orientations[i], wiring);
                                     }
                                     else
                                     {
                                         place_two_fanin(n, i, wiring);
                                     }

                                     if (lvl != 0)
                                     {
                                         check_diagonal(n, lvl, diagonal);
                                     }
                                 });
    }
    /**
     * Places a primary input. The first input sits at the top right of the input diagonal; the others follow down the
     * diagonal, with inputs in the lower half of the diagonal wired in from the left border and inputs in the upper
     * half from the top border.
     *
     * @param n Input node.
     */
    void place_input(const mockturtle::node<fanout_ntk> n)
    {
        if (ntk.rank_position(n) == 0)
        {
            node2pos[n] = layout.move_node(pi2node[n], first_pos);
            place_t     = first_pos;

            return;
        }

        place_t = {place_t.x - 1, place_t.y + 1};

        if (place_t.x == 0)
        {
            node2pos[n] = layout.move_node(pi2node[n], place_t);

            return;
        }

        const tile<Lyt> border = place_t.x < first_pos.x / 2 ? tile<Lyt>{0, place_t.y} : tile<Lyt>{place_t.x, 0};

        node2pos[n] = layout.move_node(pi2node[n], border);
        node2pos[n] = layout.create_buf(wire_straight(layout, border, place_t), place_t);
    }
    /**
     * Places a single-fanin node: its predecessor's wire runs east and south as computed, and the node sits east or
     * south of the wire end.
     *
     * @param n Node.
     * @param i Rank position of `n`.
     * @param o Orientation of `n`.
     * @param wiring Wire lengths of the level.
     */
    void place_single_fanin(const mockturtle::node<fanout_ntk> n, const std::size_t i, const orientation o,
                            const level_wiring& wiring)
    {
        const auto pre = networks::fanins(ntk, n).fanin_nodes.front();
        const auto pre_t =
            wire_east_then_south(layout, static_cast<tile<Lyt>>(node2pos[pre]), wiring.x[i], wiring.y[i]);

        place_t = is_east(o) ? tile<Lyt>{pre_t.x + 1, pre_t.y} : tile<Lyt>{pre_t.x, pre_t.y + 1};

        node2pos[n] = connect_and_place_planar(layout, place_t, ntk, n, pre_t);

        if (ntk.rank_position(n) == 0)
        {
            first_pos = place_t;
        }
    }
    /**
     * Places a two-input gate at the corner below its northern predecessor and right of its western one, after both
     * wires ran east and south as computed. The western predecessor's east run is at least as long as the east run
     * of the next node in the rank, so that its south run stays clear of that node's wire.
     *
     * @param n Node.
     * @param i Rank position of `n`.
     * @param wiring Wire lengths of the level.
     */
    void place_two_fanin(const mockturtle::node<fanout_ntk> n, const std::size_t i, const level_wiring& wiring)
    {
        const auto fc = networks::fanins(ntk, n);

        auto pre1_t = static_cast<tile<Lyt>>(node2pos[fc.fanin_nodes[0]]);
        auto pre2_t = static_cast<tile<Lyt>>(node2pos[fc.fanin_nodes[1]]);

        // pre1 is the northern predecessor
        if (pre2_t.y < pre1_t.y)
        {
            std::swap(pre1_t, pre2_t);
        }

        const auto next_x = i + 1 < wiring.x.size() ? wiring.x[i + 1] : uint64_t{0};

        pre1_t = wire_east_then_south(layout, pre1_t, wiring.x[i], wiring.y[i]);
        pre2_t = wire_east_then_south(layout, pre2_t, std::max(wiring.x[i], next_x), wiring.y[i]);

        place_t = {pre1_t.x, pre2_t.y};

        node2pos[n] = connect_and_place_planar(layout, place_t, ntk, n, pre1_t, pre2_t, fc.constant_fanin);

        if (ntk.rank_position(n) == 0)
        {
            first_pos = place_t;
        }
    }
    /**
     * Checks that a placed node sits on the level's diagonal.
     *
     * @param n Node just placed.
     * @param lvl Level.
     * @param diagonal Diagonal of the level, set by its first node.
     * @throws std::logic_error If `n` is on another diagonal.
     */
    void check_diagonal(const mockturtle::node<fanout_ntk> n, const uint32_t lvl,
                        std::optional<uint64_t>& diagonal) const
    {
        const auto t   = static_cast<tile<Lyt>>(node2pos[n]);
        const auto sum = static_cast<uint64_t>(t.x) + static_cast<uint64_t>(t.y);

        if (!diagonal.has_value())
        {
            diagonal = sum;
        }
        else if (sum != *diagonal)
        {
            throw std::logic_error(
                fmt::format("node {} of level {} sits on diagonal {} instead of {}", n, lvl, sum, *diagonal));
        }
    }
    /**
     * Routes every primary output to the right border. The nodes of the output level sit on one diagonal, so each
     * node owns the row of its tile. A node with two outputs takes the row below as well, which pushes the rows of
     * every following node down by one until a gap between two nodes absorbs the shift; a pushed node first runs
     * south to its row, then east. Rows are assigned in rank order and the outputs are created in output order.
     *
     * @return `1` plus the number of rows the shifts add below the last diagonal, `0` if none were needed.
     * @throws std::logic_error If a node drives more than two outputs.
     */
    [[nodiscard]] uint64_t route_outputs()
    {
        // outputs per driver, in output order
        mockturtle::node_map<std::vector<uint32_t>, fanout_ntk> outputs_of{ntk};
        ntk.foreach_po(
            [this, &outputs_of](const auto& po, const auto i)
            {
                if (const auto drv = ntk.get_node(po); !ntk.is_constant(drv))
                {
                    outputs_of[drv].push_back(static_cast<uint32_t>(i));
                }
            });

        // row of every output, assigned in the rank order of the drivers
        std::vector<uint64_t> row_of(ntk.num_pos(), 0);
        uint64_t              next_free_row = 0;

        ntk.foreach_node_in_rank(ntk.depth(),
                                 [this, &outputs_of, &row_of, &next_free_row](const auto& n)
                                 {
                                     const auto& outputs = outputs_of[n];

                                     if (outputs.size() > 2)
                                     {
                                         throw std::logic_error(
                                             fmt::format("node {} drives more than two primary outputs", n));
                                     }

                                     auto row = static_cast<uint64_t>(static_cast<tile<Lyt>>(node2pos[n]).y);

                                     for (const auto po_index : outputs)
                                     {
                                         row              = std::max(row, next_free_row);
                                         row_of[po_index] = row;
                                         next_free_row    = row + 1;
                                     }
                                 });

        uint64_t last_row = 0;

        ntk.foreach_po(
            [this, &row_of, &last_row](const auto& po, const auto i)
            {
                const auto drv = ntk.get_node(po);

                if (ntk.is_constant(drv))
                {
                    return;
                }

                const auto po_index = static_cast<uint32_t>(i);
                auto       anchor   = static_cast<tile<Lyt>>(node2pos[drv]);

                // down to the output's row, sharing the south run with the node's other output
                if (row_of[po_index] > anchor.y)
                {
                    anchor = static_cast<tile<Lyt>>(wire_straight(layout, anchor, {anchor.x, row_of[po_index] + 1}));
                }

                const tile<Lyt> border{first_pos.x + 1, anchor.y};

                layout.create_po(wire_straight(layout, anchor, border), output_name(po_index), border);

                last_row = std::max(last_row, static_cast<uint64_t>(anchor.y));
            });

        const auto bottom = static_cast<uint64_t>(place_t.y);

        return last_row > bottom ? last_row - bottom : 0;
    }
    /**
     * Name of a primary output: the network's if it has one, `po<index>` otherwise.
     *
     * @param po_index Index of the output.
     * @return Name.
     */
    [[nodiscard]] std::string output_name(const uint32_t po_index) const
    {
        if constexpr (mockturtle::has_has_output_name_v<Ntk>)
        {
            if (ntk.has_output_name(po_index))
            {
                return ntk.get_output_name(po_index);
            }
        }

        return fmt::format("po{}", po_index);
    }

    /**
     * The network.
     */
    const fanout_ntk ntk;
    /**
     * Parameters.
     */
    const plane_params& ps;
    /**
     * Statistics.
     */
    plane_stats& pst;
    /**
     * Signal in the layout of every placed node.
     */
    mockturtle::node_map<mockturtle::signal<Lyt>, fanout_ntk> node2pos;
    /**
     * Layout under construction.
     */
    Lyt layout;
    /**
     * Reserved input nodes of the layout, before they are moved to their positions.
     */
    mockturtle::node_map<mockturtle::node<Lyt>, fanout_ntk> pi2node{ntk};
    /**
     * Tile of the first node of the last placed level: the top right end of the current diagonal.
     */
    tile<Lyt> first_pos{};
    /**
     * Tile of the last placed node.
     */
    tile<Lyt> place_t{};
};

}  // namespace detail

/**
 * Planar Layout from Network Embedding (PLANE): places and routes a planar, balanced, fanout-substituted, ranked
 * logic network as a 2DDWave-clocked gate-level layout without wire crossings. The approach was originally proposed
 * in \"A Fully Planar Approach to Field-coupled Nanocomputing: Scalable Placement and Routing Without Wire
 * Crossings\" by B. Hien, M. Walter, S. Hofmann, and R. Wille in IEEE NANO 2025.
 *
 * Every level of the network is placed on one anti-diagonal of the layout in rank order, so the planar embedding of
 * the network is preserved. Lookup tables decide for every node whether it sits east or south of its predecessor's
 * wire and how many routing lines the level needs; the wires are then run east and south so that all nodes of a
 * level share one diagonal. Primary inputs sit on the left and top borders, primary outputs on the right border.
 *
 * The input must be planar in its rank order (see `synthesis::planarization`) with at most two non-constant fanins
 * per node.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Ranked network type (see `mutable_rank_view`).
 * @param ntk Network to place and route.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return Crossing-free gate-level layout that implements `ntk`.
 * @throws networks::high_degree_fanin_exception If a node has more than two non-constant fanins.
 * @throws std::invalid_argument If the rank order of `ntk` has crossings, or if a gate has a constant fanin next to a
 * single non-constant one.
 * @throws std::logic_error If placement or routing cannot keep the layout planar; such an input is a defect.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] Lyt plane(const Ntk& ntk, const plane_params& ps = {}, plane_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");

    if (networks::has_high_degree_fanin_nodes(ntk, 2))
    {
        throw networks::high_degree_fanin_exception();
    }

    // a gate that keeps a single non-constant fanin next to a constant one has no placement; propagate constants first
    bool constant_gate = false;
    ntk.foreach_gate(
        [&ntk, &constant_gate](const auto& n)
        {
            const auto fc = networks::fanins(ntk, n);

            if (fc.constant_fanin.has_value() && fc.fanin_nodes.size() == 1)
            {
                constant_gate = true;
            }
        });

    if (constant_gate)
    {
        throw std::invalid_argument(
            "A gate has a constant fanin besides a single non-constant one; propagate constants "
            "before placement");
    }

    utils::graph::mincross_params mc_ps{};
    mc_ps.optimize = false;
    utils::graph::mincross_stats mc_st{};

    utils::graph::mincross(ntk, mc_ps, &mc_st);

    if (mc_st.num_crossings != 0)
    {
        throw std::invalid_argument(fmt::format("The rank order of the network has {} crossings", mc_st.num_crossings));
    }

    plane_stats st{};

    detail::plane_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    st.num_crossings = result.num_crossings();

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::physical_design
