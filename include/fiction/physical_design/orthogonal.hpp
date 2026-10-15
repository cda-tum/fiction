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
 * @brief Scalable placement and routing based on orthogonal graph drawing.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/gate_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/layout_utils.hpp"
#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/networks/views/edge_color_view.hpp"
#include "fiction/physical_design/placement_utils.hpp"
#include "fiction/synthesis/fanout_substitution.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <kitty/bit_operations.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ostream>
#include <utility>
#include <vector>

namespace fiction::physical_design
{

/**
 * Parameters for the orthogonal physical design algorithm.
 */
struct orthogonal_physical_design_params
{
    /**
     * Number of clock phases to use. 3 and 4 are supported.
     */
    layouts::clocking::num_clks number_of_clock_phases = layouts::clocking::num_clks::FOUR;
    /**
     * Arrangement of the shifted rows or columns of the created layout. Shifted Cartesian and hexagonal layouts require
     * it, Cartesian layouts ignore it.
     */
    std::optional<layouts::arrangement> layout_arrangement = std::nullopt;
    /**
     * Callback that receives the progress of the gate placement.
     */
    utils::progress_callback on_progress{};
};

struct orthogonal_physical_design_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    /** @brief Number of logic gates, excluding primary inputs and outputs. */
    uint64_t num_gates{0ull};
    /** @brief Number of physical wires, excluding primary inputs and outputs. */
    uint64_t num_wires{0ull};
    /** @brief Number of crossing-layer wires above occupied ground-layer tiles. */
    uint64_t num_crossings{0ull};

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

/** @brief Owns the colored traversal and its directional colors. */
struct coloring_container
{
    /** @brief Colors the named technology-network traversal. */
    explicit coloring_container(const mockturtle::topo_view<mockturtle::fanout_view<tec_nt>>& ntk) :
            color_ntk{ntk},
            color_east{color_ntk.new_color()},
            color_south{color_ntk.new_color()}
    {}

    /** @brief Named traversal with separate node and edge colors. */
    networks::views::out_of_place_edge_color_view<mockturtle::topo_view<mockturtle::fanout_view<tec_nt>>> color_ntk;

    /** @brief Identifier for uncolored edges. */
    uint32_t color_null = 0ul;
    /** @brief Identifier for eastward edges. */
    uint32_t color_east;
    /** @brief Identifier for southward edges. */
    uint32_t color_south;

    /** @brief Returns the other direction color. */
    [[nodiscard]] uint32_t opposite_color(const uint32_t c) const noexcept
    {
        return c == color_east ? color_south : color_east;
    }
};

/** @brief Paints adjacent edges with alternating direction colors. */
inline void recursively_paint_edges(
    const coloring_container& ctn,
    const mockturtle::edge<
        networks::views::out_of_place_edge_color_view<mockturtle::topo_view<mockturtle::fanout_view<tec_nt>>>>& e,
    const uint32_t c) noexcept
{
    // exit condition: edge is already painted
    if (ctn.color_ntk.edge_color(e) != ctn.color_null)
    {
        return;
    }

    // paint edge with given color
    ctn.color_ntk.paint_edge(e, c);

    // paint children edges
    networks::foreach_outgoing_edge(ctn.color_ntk, e.source,
                                    [&ctn, &e, &c](const auto& oe)
                                    {
                                        if (oe != e)
                                        {
                                            recursively_paint_edges(ctn, oe, ctn.opposite_color(c));
                                        }
                                    });

    // paint spouse edges
    networks::foreach_incoming_edge(ctn.color_ntk, e.target,
                                    [&ctn, &e, &c](const auto& ie)
                                    {
                                        if (ie != e)
                                        {
                                            recursively_paint_edges(ctn, ie, c);
                                        }
                                    });
}

/**
 * @brief Colors the named traversal for orthogonal placement.
 * @param ntk Topologically ordered named technology network to read.
 * @return Traversal with its own node and edge color storage.
 */
inline coloring_container
east_south_edge_coloring(const mockturtle::topo_view<mockturtle::fanout_view<tec_nt>>& ntk) noexcept
{
    coloring_container ctn{ntk};

    ntk.foreach_gate_reverse(
        [&](const auto& n)
        {
            const auto finc = networks::fanin_edges(ctn.color_ntk, n);

            // if any incoming edge is colored east, color them all east, and south otherwise
            const auto color = std::ranges::any_of(finc.fanin_edges, [&ctn](const auto& fe)
                                                   { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }) ?
                                   ctn.color_east :
                                   ctn.color_south;

            std::ranges::for_each(finc.fanin_edges,
                                  [&ctn, &color](const auto& fe) { recursively_paint_edges(ctn, fe, color); });

            // if all incoming edges are colored east, paint the node east as well
            if (std::ranges::all_of(finc.fanin_edges,
                                    [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }))
            {
                ctn.color_ntk.paint(tec_nt::node{n}, ctn.color_east);
            }
            // else, if all incoming edges are colored south, paint the node south as well
            else if (std::ranges::all_of(finc.fanin_edges, [&ctn](const auto& fe)
                                         { return ctn.color_ntk.edge_color(fe) == ctn.color_south; }))
            {
                ctn.color_ntk.paint(tec_nt::node{n}, ctn.color_south);
            }
        });

    return ctn;
}

/** @brief Checks that fanouts use distinct direction colors. */
inline bool is_east_south_colored(
    const networks::views::out_of_place_edge_color_view<mockturtle::topo_view<mockturtle::fanout_view<tec_nt>>>&
        ntk) noexcept
{
    bool is_properly_colored = true;
    ntk.foreach_node(
        [&ntk, &is_properly_colored](const auto& n)
        {
            if (!ntk.is_constant(n))
            {
                uint32_t clr = 0ul;
                ntk.foreach_fanout(n,
                                   [&ntk, &is_properly_colored, &clr](const auto& fon, const auto i)
                                   {
                                       // store color of first fanout
                                       if (i == 0)
                                       {
                                           clr = ntk.color(fon);
                                       }
                                       // compare color of first fanout against all others
                                       else if (clr == ntk.color(fon))
                                       {
                                           // since we are working with 3-graphs, this can only be executed if i == 1
                                           is_properly_colored = false;
                                       }

                                       return is_properly_colored;
                                   });
            }
            return is_properly_colored;
        });

    return is_properly_colored;
}

/** @brief Checks whether the eastern side can host a primary output. */
inline uint32_t is_eastern_po_orientation_available(const coloring_container& ctn, const tec_nt::node& n) noexcept
{
    bool eastern_side_available = true;

    // PO nodes can have a maximum of one other fanout
    ctn.color_ntk.foreach_fanout(n,
                                 [&ctn, &eastern_side_available](const auto& fon)
                                 {
                                     // if that fanout is colored east, no PO can be placed there
                                     if (ctn.color_ntk.color(fon) == ctn.color_east)
                                     {
                                         eastern_side_available = false;
                                     }
                                     return false;
                                 });

    return static_cast<uint32_t>(eastern_side_available);
}

/** @brief Computes the extent required by colored gates and primary outputs. */
inline fiction::layouts::layout_base::extent determine_layout_size(const coloring_container& ctn,
                                                                   const uint32_t            num_multi_output_nodes)
{
    uint64_t x = 0ull, y = ctn.color_ntk.num_pis() == 0 ? 0 : ctn.color_ntk.num_pis() - 1;
    ctn.color_ntk.foreach_node(
        [&](const auto& n)
        {
            if (!ctn.color_ntk.is_constant(n))
            {
                if (ctn.color_ntk.is_pi(n))
                {
                    ctn.color_ntk.foreach_fanout(n,
                                                 [&ctn, &x](const auto& fon)
                                                 {
                                                     if (ctn.color_ntk.color(fon) == ctn.color_south)
                                                     {
                                                         ++x;
                                                     }
                                                 });
                }
                else if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                {
                    ++x;
                }
                else if (clr == ctn.color_south)
                {
                    ++y;
                }
                else if (clr == ctn.color_null)
                {
                    ++x;
                    ++y;
                }

                if (ctn.color_ntk.is_po(n))
                {
                    if (is_eastern_po_orientation_available(ctn, n))
                    {
                        ++x;
                    }
                    else
                    {
                        ++y;
                    }
                }
            }
        });

    // for multi-output nodes, add another row
    y += num_multi_output_nodes;

    return {x + 1, y + 1, 2};
}

template <typename Lyt>
typename Lyt::object_id wire_east(Lyt& lyt, const fiction::layouts::layout_base::coordinate& src,
                                  const fiction::layouts::layout_base::coordinate& dest)
{
    auto a = lyt.find_object(src).value();

    for (auto x = src.x + 1; x < dest.x; ++x)
    {
        auto t = fiction::layouts::layout_base::coordinate{x, src.y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t).value();
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt>
typename Lyt::object_id wire_south(Lyt& lyt, const fiction::layouts::layout_base::coordinate& src,
                                   const fiction::layouts::layout_base::coordinate& dest)
{
    auto a = lyt.find_object(src).value();

    for (auto y = src.y + 1; y < dest.y; ++y)
    {
        auto t = fiction::layouts::layout_base::coordinate{src.x, y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t).value();
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

/** @brief Routes ordered inputs to a placed two-input gate. @tparam Lyt Output layout type. */
template <typename Lyt>
typename Lyt::object_id
connect_and_place(Lyt& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk,
                  const tec_nt::node& n, fiction::layouts::layout_base::coordinate pre1_t,
                  fiction::layouts::layout_base::coordinate pre2_t, const std::optional<bool>& c = std::nullopt)
{
    const bool reversed = pre2_t < pre1_t;
    if (reversed)
    {
        std::swap(pre1_t, pre2_t);
    }
    const auto north = wire_south(lyt, pre1_t, t);
    const auto west  = wire_east(lyt, pre2_t, t);
    return reversed ? place(lyt, t, ntk, n, west, north, c) : place(lyt, t, ntk, n, north, west, c);
}

/** @brief Routes one input to a placed unary gate. @tparam Lyt Output layout type. */
template <typename Lyt>
typename Lyt::object_id connect_and_place(Lyt& lyt, const fiction::layouts::layout_base::coordinate& t,
                                          const tec_nt& ntk, const tec_nt::node& n,
                                          const fiction::layouts::layout_base::coordinate& pre_t)
{
    if (lyt.is_westwards_of(t, pre_t))
    {
        return place(lyt, t, ntk, n, wire_east(lyt, pre_t, t));
    }
    if (lyt.is_northwards_of(t, pre_t))
    {
        return place(lyt, t, ntk, n, wire_south(lyt, pre_t, t));
    }

    assert(false);  // gates cannot be placed elsewhere
    return {};      // fix -Wreturn-type warning
}

/**
 * Places the primary outputs (POs) in the layout.
 *
 * This function positions the POs within the provided layout. If a PO is a multi-output node (a fanout with two POs
 * connected to it), the second PO is automatically placed and connected below the first output.
 *
 * The positioning of multi-output nodes will follow this structure:
 *
 * F→=→...→O
 * ↓
 * =→=→...→O
 *
 * @tparam Lyt Desired gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param po_counter Counter for POs.
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 */
template <typename Lyt>
void place_outputs(Lyt& layout, const coloring_container& ctn, uint32_t po_counter,
                   const mockturtle::node_map<typename Lyt::object_id, tec_nt>& node2pos)
{
    std::vector<tec_nt::node> output_nodes{};

    ctn.color_ntk.foreach_po(
        [&po_counter, &output_nodes, &node2pos, &ctn, &layout](const auto& po)
        {
            if (!ctn.color_ntk.is_constant(po))
            {
                auto n_s     = node2pos[po];
                auto po_tile = layout.get_tile(n_s);

                const auto multi_output_node = std::ranges::find(output_nodes, po) != output_nodes.cend();

                // determine PO orientation
                if (!is_eastern_po_orientation_available(ctn, po) || multi_output_node)
                {
                    n_s     = wire_south(layout, po_tile, {po_tile.x, po_tile.y + 2});
                    po_tile = layout.get_tile(n_s);
                }

                // check if PO position is located at the border
                if (layout.is_at_eastern_border({po_tile.x + 1, po_tile.y}) && !multi_output_node)
                {
                    ++po_tile.x;
                    layout.create_po(n_s,
                                     ctn.color_ntk.has_output_name(po_counter) ?
                                         ctn.color_ntk.get_output_name(po_counter++) :
                                         fmt::format("po{}", po_counter++),
                                     po_tile);
                }
                // place PO at the border and connect it by wire segments
                else
                {
                    const fiction::layouts::layout_base::coordinate anker{po_tile};

                    po_tile = layout.eastern_border_of(po_tile).value();

                    layout.create_po(wire_east(layout, anker, po_tile),
                                     ctn.color_ntk.has_output_name(po_counter) ?
                                         ctn.color_ntk.get_output_name(po_counter++) :
                                         fmt::format("po{}", po_counter++),
                                     po_tile);
                }

                output_nodes.push_back(po);
            }
            else
            {
                const auto y = layout.height();
                layout.resize({std::max(layout.width(), uint32_t{2}), static_cast<uint64_t>(y) + 1, layout.layers()});
                kitty::dynamic_truth_table function{0};
                if (ctn.color_ntk.constant_value(po))
                {
                    kitty::set_bit(function, 0);
                }
                const auto constant = layout.create_gate({}, function, {0, y});
                layout.create_po(constant,
                                 ctn.color_ntk.has_output_name(po_counter) ? ctn.color_ntk.get_output_name(po_counter) :
                                                                             fmt::format("po{}", po_counter),
                                 {1, y});
                ++po_counter;
            }
        });
}

/**
 * Implementation of the orthogonal physical design algorithm.
 *
 * The named technology network has normalized fanout. This class specializes only the output layout type.
 *
 * @tparam Lyt Gate-level layout type.
 */
template <typename Lyt>
class orthogonal_impl
{
  public:
    /**
     * Constructor for the orthogonal physical design algorithm.
     *
     * @param src The source network to be placed, already fanout-substituted into a `technology_network`.
     * @param p The parameters for the orthogonal physical design algorithm.
     * @param st The statistics object to record execution details.
     */
    orthogonal_impl(const tec_nt& src, orthogonal_physical_design_params p, orthogonal_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{src}},
            ps{std::move(p)},
            pst{st}
    {}
    /**
     * Places and routes the source network with the orthogonal algorithm.
     *
     * @return A gate-level layout that implements the source network.
     */
    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = east_south_edge_coloring(ntk);

        mockturtle::node_map<typename Lyt::object_id, tec_nt> node2pos{static_cast<const tec_nt&>(ctn.color_ntk)};

        // find multi-output nodes
        std::vector<mockturtle::node<decltype(ntk)>> output_nodes{};
        std::vector<mockturtle::node<decltype(ntk)>> multi_output_nodes{};
        uint32_t                                     num_multi_output_nodes{0};

        ctn.color_ntk.foreach_po(
            [&](const auto& po)
            {
                if (std::ranges::find(output_nodes, po) != output_nodes.cend())
                {
                    multi_output_nodes.push_back(po);
                    ++num_multi_output_nodes;
                }

                output_nodes.push_back(po);
            });

        // instantiate the layout
        auto layout = layouts::make_gate_level_layout<Lyt>(ps.layout_arrangement,
                                                           determine_layout_size(ctn, num_multi_output_nodes),
                                                           layouts::clocking::twoddwave(ps.number_of_clock_phases));

        mockturtle::node_map<typename Lyt::object_id, tec_nt> pi2node{static_cast<const tec_nt&>(ctn.color_ntk)};

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        fiction::layouts::layout_base::coordinate latest_pos{1, 0};

        utils::progress_reporter progress{ps.on_progress, "placing gates", ctn.color_ntk.size()};

        ctn.color_ntk.foreach_node(
            [&](const auto& n)
            {
                // do not place constants
                if (!ctn.color_ntk.is_constant(n))
                {
                    // Place each PI at its assigned position.
                    if (ctn.color_ntk.is_pi(n))
                    {
                        node2pos[n] = place(layout, {0, latest_pos.y}, ctn.color_ntk, n);
                        pi2node[n]  = node2pos[n];

                        // resolve conflicting PIs
                        ctn.color_ntk.foreach_fanout(
                            n,
                            [&ctn, &n, &layout, &node2pos, &latest_pos](const auto& fon)
                            {
                                if (ctn.color_ntk.color(fon) == ctn.color_south)
                                {
                                    node2pos[n] =
                                        layout.create_buf(wire_east(layout, {0, latest_pos.y}, latest_pos), latest_pos);
                                    ++latest_pos.x;
                                }

                                // PIs have only one fanout
                                return false;
                            });

                        ++latest_pos.y;
                    }
                    // if n has only one fanin
                    else if (const auto fc = networks::fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = layout.get_tile(node2pos[pre]);

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            const fiction::layouts::layout_base::coordinate t{latest_pos.x, pre_t.y};
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            const fiction::layouts::layout_base::coordinate t{pre_t.x, latest_pos.y};
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.y;
                        }
                        else
                        {
                            // single fanin nodes should not be colored null
                            assert(false);
                        }
                    }
                    else  // if node has two fanins (or three fanins with one of them
                          // being constant)
                    {
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                        auto pre1_t = layout.get_tile(node2pos[pre1]), pre2_t = layout.get_tile(node2pos[pre2]);

                        fiction::layouts::layout_base::coordinate t{};
                        bool                                      reversed = false;

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.y < pre1_t.y)
                            {
                                std::swap(pre1_t, pre2_t);
                                reversed = !reversed;
                            }

                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = layout.get_tile(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.x > pre1_t.x)
                            {
                                std::swap(pre1_t, pre2_t);
                                reversed = !reversed;
                            }

                            // use larger x position of predecessors
                            t = {pre1_t.x, latest_pos.y};

                            // each 2-input gate has one incoming bent wire
                            pre2_t = layout.get_tile(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.y;
                        }
                        // n is colored null; corner case
                        else
                        {
                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            if (!(layout.east(pre1_t) && layout.is_empty_tile(*layout.east(pre1_t))) ||
                                !(layout.south(pre2_t) && layout.is_empty_tile(*layout.south(pre2_t))))
                            {
                                std::swap(pre1_t, pre2_t);
                                reversed = !reversed;
                            }

                            t = latest_pos;

                            // both wires have one bent
                            pre1_t = layout.get_tile(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
                            pre2_t = layout.get_tile(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.x;
                            ++latest_pos.y;
                        }

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, reversed ? pre2_t : pre1_t,
                                                        reversed ? pre1_t : pre2_t, fc.constant_fanin);
                    }

                    if (ctn.color_ntk.is_po(n) &&
                        (!is_eastern_po_orientation_available(ctn, n) ||
                         std::ranges::find(multi_output_nodes, n) != multi_output_nodes.cend()))
                    {
                        ++latest_pos.y;
                    }
                }

                progress.advance();
            });

        // place outputs after the main algorithm to handle possible multi-output or unordered nodes
        place_outputs(layout, ctn, po_counter, node2pos);

        std::vector<typename Lyt::object_id> input_order{};
        input_order.reserve(ctn.color_ntk.num_pis());
        ctn.color_ntk.foreach_pi([&](const auto& pi) { input_order.push_back(pi2node[pi]); });
        layout.set_input_order(input_order);

        // restore possibly set signal names
        networks::restore_names(static_cast<const tec_nt&>(ctn.color_ntk), layout, node2pos);

        // statistical information
        pst.x_size        = layout.width();
        pst.y_size        = layout.height();
        pst.num_gates     = layout.num_gates();
        pst.num_wires     = layout.num_wires();
        pst.num_crossings = layout.num_crossings();

        return layout;
    }

  private:
    mockturtle::topo_view<mockturtle::fanout_view<tec_nt>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail

/**
 * A scalable placement & routing approach based on orthogonal graph drawing as originally proposed in \"Scalable Design
 * for Field-coupled Nanocomputing Circuits\" by M. Walter, R. Wille, F. Sill Torres, D. Große, and R. Drechsler in
 * ASP-DAC 2019. A more extensive description can be found in \"Design Automation for Field-coupled Nanotechnologies\"
 * by M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer Nature in 2022.
 *
 * Via certain restrictions to the degrees of freedom in FCN physical design, this algorithm achieves a polynomial time
 * complexity. However, these restrictions lead to an overall approximation of optimal layout quality within several
 * factors. Therefore, this algorithm produces valid layouts within a short amount of time, its results are far from
 * being optimal in terms of area.
 *
 * The imposed restrictions are that the input logic network has to be a 3-graph, i.e., cannot have any node exceeding
 * degree 3 (combined input and output), and that the resulting layout is always 2DDWave-clocked.
 *
 * This algorithm is based on a modification of \"Improved orthogonal drawings of 3-graphs\" by Therese C. Biedl in
 * Canadian Conference on Computational Geometry 1996. Biedl's original algorithm works for undirected graphs only while
 * this modification respects information flow of directed logic networks. To this end, the edge directions of the logic
 * network directly used instead of relabeling the edges according to its DFS tree, ordering the vertices using
 * topological sorting instead of DFS, and adding an extra placement rule for nodes without predecessors.
 *
 * The algorithm works in polynomial time \f$\mathcal{O}(3|N| + |L|)\f$ where \f$|N|\f$ is the number of nodes the given
 * network and \f$|L|\f$ is the resulting layout size given by \f$x \cdot y\f$, which approaches \f$(\frac{|N|}{2})^2\f$
 * asymptotically.
 *
 * May throw a high_degree_fanin_exception if `ntk` contains any node with a fan-in larger than 2.
 *
 * @tparam Lyt Desired gate-level layout type.
 * @param ntk Named technology network to place and route.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `Lyt` that implements `ntk` as an FCN circuit.
 * @throws std::invalid_argument If `Lyt` is a shifted Cartesian or hexagonal layout and `ps.layout_arrangement` is
 * empty.
 */
template <typename Lyt>
Lyt orthogonal(const tec_nt& ntk, orthogonal_physical_design_params ps = {},
               orthogonal_physical_design_stats* pst = nullptr)
{
    static_assert(
        requires { requires std::same_as<typename Lyt::object_id, fiction::layouts::layout_object_id>; },
        "Lyt is not a gate-level layout");

    layouts::require_arrangement<Lyt>(ps.layout_arrangement);

    // check for input degree
    if (networks::has_high_degree_fanin_nodes(ntk, 2))
    {
        throw networks::high_degree_fanin_exception();
    }

    orthogonal_physical_design_stats st{};
    detail::orthogonal_impl<Lyt>     p{synthesis::fanout_substitution<tec_nt>(ntk), ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::physical_design
