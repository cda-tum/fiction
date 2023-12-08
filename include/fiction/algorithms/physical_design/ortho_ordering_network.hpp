//
// Created by benjamin on 11.04.23.
//

#ifndef FICTION_ORTHO_ORDERING_NETWORK_HPP
#define FICTION_ORTHO_ORDERING_NETWORK_HPP

#include "fiction/algorithms/network_transformation/inverter_substitution.hpp"
#include "fiction/algorithms/physical_design/orthogonal.hpp"
#include "fiction/networks/views/input_ordering_view.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace fiction
{

namespace detail
{
/**
 * Paints a node and all incoming edges
 *
 * @tparam Ntk The logic network type.
 * @param ctn Container holding the colored network.
 * @param n Node to be painted.
 * @param color Color to paint ('south', 'east', 'null').
 */
template <typename Ntk>
void paint_node_and_edges(const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& n, const uint32_t color)
{
    const auto finc = fanin_edges(ctn.color_ntk, n);
    std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                  [&ctn, &color](const auto& fe) { ctn.color_ntk.paint_edge(fe, color); });
    ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, color);
}

/**
 * Paints nodes and edges of all nodes affected by: PIs related to two PIs over a fan-out node
 *
 * @tparam Ntk The logic network type.
 * @param ntk Network structure to be painted.
 * @param ctn Container holding the colored network.
 * @param current_node Currently visited node.
 *
 */
template <typename Ntk>
void paint_fo_two(const Ntk& ntk, const coloring_container<Ntk>& ctn, mockturtle::node<Ntk>& current_node)
{
    auto swap_color = ctn.color_south;
    ntk.foreach_fanout(current_node,
                       [&ctn, &current_node, &ntk, &swap_color](const auto& cur_fon)
                       {
                           // Jump to next node
                           current_node = cur_fon;

                           if (ntk.is_inv(current_node))
                           {
                               // Color Inverter east
                               paint_node_and_edges(ctn, current_node, ctn.color_east);
                               ntk.foreach_fanout(current_node,
                                                  [&](const auto& fon_inv)
                                                  {
                                                      // Jump to next node
                                                      current_node = fon_inv;
                                                  });
                               paint_node_and_edges(ctn, current_node, ctn.color_south);
                           }
                           else
                           {
                               paint_node_and_edges(ctn, current_node, swap_color);
                               swap_color = ctn.color_east;
                           }
                       });
}

/**
 * Paints nodes and edges of all nodes affected by: PIs related to one PI over a fan-out node
 *
 * @tparam Ntk The logic network type.
 * @param ntk Network structure to be painted.
 * @param ctn Container holding the colored network.
 * @param current_node Currently visited node.
 */
template <typename Ntk>
void paint_fo_one(const Ntk& ntk, const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& current_node)
{
    const auto fc = fanins(ctn.color_ntk, current_node);
    if (ntk.is_inv(fc.fanin_nodes[0]))
    {
        const auto fc_fi = fanins(ctn.color_ntk, fc.fanin_nodes[0]);
        if (ntk.is_fanout(fc_fi.fanin_nodes[0]) && ntk.fanout_size(fc_fi.fanin_nodes[0]) >= 2)
        {
            if (ctn.color_ntk.color(fc.fanin_nodes[0]) == ctn.color_south)
            {
                paint_node_and_edges(ctn, current_node, ctn.color_east);
            }
            else
            {
                paint_node_and_edges(ctn, current_node, ctn.color_south);
            }
        }
    }
    else if (ntk.is_inv(fc.fanin_nodes[1]))
    {
        const auto fc_fi = fanins(ctn.color_ntk, fc.fanin_nodes[1]);
        if (ntk.is_fanout(fc_fi.fanin_nodes[0]) && ntk.fanout_size(fc_fi.fanin_nodes[0]) >= 2)
        {
            if (ctn.color_ntk.color(fc.fanin_nodes[1]) == ctn.color_south)
            {
                paint_node_and_edges(ctn, current_node, ctn.color_east);
            }
            else
            {
                paint_node_and_edges(ctn, current_node, ctn.color_south);
            }
        }
    }
}

/**
 * Paints nodes and edges of all nodes affected by: PIs related to one PI without fan-out node
 *
 * @tparam Ntk The logic network type.
 * @param ntk Network structure to be painted.
 * @param ctn Container holding the colored network.
 * @param current_node Currently visited node.
 */
template <typename Ntk>
void paint_pi_to_pi(const Ntk& ntk, const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& current_node)
{
    if (const auto fc = fanins(ctn.color_ntk, current_node);
        (ntk.is_inv(fc.fanin_nodes[0]) && !ntk.is_inv(fc.fanin_nodes[1])) ||
        (!ntk.is_inv(fc.fanin_nodes[0]) && ntk.is_inv(fc.fanin_nodes[1])))
    {
        paint_node_and_edges(ctn, current_node, ctn.color_south);
    }
    else
    {
        paint_node_and_edges(ctn, current_node, ctn.color_east);
    }
}

/**
 * Conditional coloring computes a valid coloring for all the nodes, which are affected by the input ordering network.
 *
 * Two PIs are related to each other, when they are connected to the same two fan-in gate and between the PI and this
 * gate are only fan-out nodes or inverters.
 *
 * Conditional coloring rules:
 * 0. All 1-fan-in nodes (inverters and fan-outs) after PIs get colored east
 * 1. PI connected to a fan-out and related to two PIs:
 * - Case: 2-inverters -> get balanced to the beginning of the fan-out node (nc_inv has to be respected here for
 * instance)
 * - Case: one inverter (color inverter east and other outgoing edges south)
 * - Case: no inverter (color one outgoing edge east and one south)
 * 2. PI node connected to fan-out node and related to one PI (coloring is dependent on the coloring of the network)
 * 3. PI related to PI: color connecting node east
 *
 * @tparam Ntk The logic network type.
 * @param ntk network to be colored based on the rules of the ordering network stated above
 * @return the conditional coloring
 */
template <typename Ntk>
coloring_container<Ntk> conditional_coloring(const Ntk& ntk) noexcept
{
    coloring_container<Ntk> ctn{ntk};

    // currently viewed node
    mockturtle::node<Ntk> current_node;

#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.num_gates()),
                                 "[i] determining relative positions: |{0}|"};
#endif

    // Find a coloring
    ntk.foreach_gate_reverse(
        [&](const auto& n, [[maybe_unused]] const auto i)
        {
            const auto finc = fanin_edges(ctn.color_ntk, n);

            // if any incoming edge is colored east, color them all east, and south otherwise
            const auto color =
                std::any_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                            [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }) ?
                    ctn.color_east :
                    ctn.color_south;

            std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                          [&ctn, &color](const auto& fe) { recursively_paint_edges(ctn, fe, color); });

            // if all incoming edges are colored east, paint the node east as well
            if (std::all_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                            [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_east; }))
            {
                ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_east);
            }
            // else, if all incoming edges are colored south, paint the node south as well
            else if (std::all_of(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                 [&ctn](const auto& fe) { return ctn.color_ntk.edge_color(fe) == ctn.color_south; }))
            {
                ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_south);
            }

#if (PROGRESS_BARS)
            // update progress
            bar(i);
#endif
        });

    // Adjust only the nodes viewed in the ordering network
    ntk.foreach_pi(
        [&](const auto& pi)
        {
            ntk.foreach_fanout(
                pi,
                [&](const auto& fon)
                {
                    // always track the current_node
                    current_node = fon;

                    // skip Inverters and paint them east
                    if (ntk.is_inv(current_node))
                    {
                        // paint inverter east
                        paint_node_and_edges(ctn, current_node, ctn.color_east);
                        auto cur_fon = fanouts(ctn.color_ntk, current_node);
                        // jump to next node
                        current_node = cur_fon[0];
                    }

                    const auto fo_one = ctn.color_ntk.get_fo_one();
                    const auto pi_pi  = ctn.color_ntk.get_pi_to_pi();

                    if (ntk.is_fanout(current_node) && ntk.fanout_size(current_node) >= 2)
                    {
                        const auto fo_two = ctn.color_ntk.get_fo_two();
                        // paint the fan-out node east
                        paint_node_and_edges(ctn, current_node, ctn.color_east);
                        // 1. fan-out node related to two PIs
                        if (const auto it = find(fo_two.cbegin(), fo_two.cend(), pi); it != fo_two.cend())
                        {
                            const auto i = std::distance(fo_two.cbegin(), it);
                            if (i % 3 == 0)
                            {
                                // paint the related nodes
                                paint_fo_two(ntk, ctn, current_node);
                            }
                        }
                    }
                    // 2. fan-out node related to one PI
                    else if (const auto it = find(fo_one.cbegin(), fo_one.cend(), pi); it != fo_one.cend())
                    {
                        const auto i = std::distance(fo_one.cbegin(), it);
                        if (i % 2 == 1)
                        {
                            // paint the related nodes
                            paint_fo_one(ntk, ctn, current_node);
                        }
                    }
                    // 3. PI connected to node connected to PI
                    else if (const auto it_pi = find(pi_pi.cbegin(), pi_pi.cend(), pi); it_pi != pi_pi.cend())
                    {
                        const auto i = std::distance(pi_pi.cbegin(), it_pi);
                        if (i % 2 == 0)
                        {
                            // paint the related nodes
                            paint_pi_to_pi(ntk, ctn, current_node);
                        }
                    }
                });
        });

    return ctn;
}

/**
 * Wires unordered PIs to the east based on the orthogonal algorithm.
 *
 * @tparam Ntk The logic network type.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param pre Preceding node.
 * @param pre_t Tile of the Preceding node.
 * @param latest_pos Coordinates of next gate location.
 */
template <typename Ntk, typename Lyt>
void resolve_unaffected_pi(Lyt& layout, const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& pre,
                           tile<Lyt>& pre_t, tile<Lyt>& latest_pos)
{
    const auto fo_two = ctn.color_ntk.get_fo_two();
    const auto fo_one = ctn.color_ntk.get_fo_one();
    const auto pi_pi  = ctn.color_ntk.get_pi_to_pi();
    if (ctn.color_ntk.is_pi(pre) && std::find(fo_two.cbegin(), fo_two.cend(), pre) == fo_two.cend() &&
        std::find(fo_one.cbegin(), fo_one.cend(), pre) == fo_one.cend() &&
        std::find(pi_pi.cbegin(), pi_pi.cend(), pre) == pi_pi.cend())
    {
        pre_t = static_cast<tile<Lyt>>(wire_east(layout, pre_t, {latest_pos.x + 1, pre_t.y}));
        ++latest_pos.x;
    }
}

/**
 * Placement and routing of a one fan-in gate colored east
 *
 * @tparam Ntk The logic network type.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param n Viewed Node.
 * @param pre Preceding node.
 * @param pre_t Tile of the Preceding node.
 * @param latest_pos Coordinates of next gate location.
 * @param insert_position_inv X-Coordinates where inverters succeeding PIs are placed
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 */
template <typename Ntk, typename Lyt>
void place_one_fanin_gate_east(const Ntk& ntk, Lyt& layout, const coloring_container<Ntk>& ctn,
                               const mockturtle::node<Ntk>& n, const mockturtle::node<Ntk>& pre, tile<Lyt>& latest_pos,
                               const tile<Lyt>& pre_t, uint64_t& insert_position_inv,
                               mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)>& node2pos)
{
    // new column for inverters
    auto insert_position = latest_pos.x;
    if (ntk.pi_inv_flag() && ntk.is_inv(n) && ntk.is_pi(pre))
    {
        insert_position = insert_position_inv;
        ++insert_position_inv;
        --latest_pos.x;
    }

    if (ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre))
    {
        ++latest_pos.y;
    }
    const tile<Lyt> t{insert_position, pre_t.y};

    node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
    ++latest_pos.x;
}

/**
 * Placement and routing of a one fan-in gate colored south
 *
 * @tparam Ntk The logic network type.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param n Viewed Node.
 * @param latest_pos Coordinates of next gate location.
 * @param latest_pos_inputs Coordinates for PI placement
 * @param pre_t Tile of the Preceding node.
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 */
template <typename Ntk, typename Lyt>
void place_one_fanin_gate_south(Lyt& layout, const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& n,
                                tile<Lyt>& latest_pos, tile<Lyt>& latest_pos_inputs, const tile<Lyt>& pre_t,
                                mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)>& node2pos)
{
    if ((ctn.color_ntk.is_inv(n) || ctn.color_ntk.is_fanout(n)) && latest_pos.y < latest_pos_inputs.y)
    {
        const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};

        // place and route from 'pre_t' to 't'
        node2pos[n]  = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
        latest_pos.y = t.y + 1;
    }
    else
    {
        const tile<Lyt> t{pre_t.x, latest_pos.y};

        // place and route from 'pre_t' to 't'
        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
        ++latest_pos.y;
    }
}

/**
 * Placement and routing of a two fan-in gate colored east
 *
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param latest_pos Coordinates of next gate location.
 * @param latest_pos_inputs Coordinates for PI placement
 * @param t Viewed tile.
 * @param pre1_t Tile of the first preceding node.
 * @param pre2_t Tile of the second preceding node.
 */
template <typename Lyt>
void place_two_fanin_gate_east(Lyt& layout, tile<Lyt>& latest_pos, tile<Lyt>& latest_pos_inputs, tile<Lyt>& t,
                               tile<Lyt>& pre1_t, tile<Lyt>& pre2_t)
{
    // make sure pre1_t is the northward tile
    if (pre2_t.y < pre1_t.y)
    {
        std::swap(pre1_t, pre2_t);
    }

    // use larger y position of predecessors
    t = {latest_pos.x, pre2_t.y};

    // each 2-input gate has one incoming bent wire
    pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

    ++latest_pos.x;
    if (latest_pos.y < latest_pos_inputs.y)
    {
        latest_pos.y = t.y + 1;
    }
}

/**
 * This function handles the placement and routing of a two fan-in gate slated for a southern position. A new rule from
 * the Ordering Distribution Network is introduced in this function which enables southern placement without the need to
 * add a new row.
 *
 * @tparam Ntk The logic network type.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param latest_pos Coordinates of next gate location.
 * @param latest_pos_inputs Coordinates for PI placement
 * @param pre1 First preceding Node
 * @param pre2 Second preceding node
 * @param t Viewed tile.
 * @param pre1_t Tile of the first preceding node.
 * @param pre2_t Tile of the second preceding node.
 *
 */
template <typename Ntk, typename Lyt>
void place_two_fanin_gate_south(Lyt& layout, const coloring_container<Ntk>& ctn, tile<Lyt>& latest_pos,
                                const tile<Lyt>& latest_pos_inputs, const mockturtle::node<Ntk>& pre1,
                                const mockturtle::node<Ntk>& pre2, tile<Lyt>& t, tile<Lyt>& pre1_t, tile<Lyt>& pre2_t)
{
    // resolve conflicts for PIs not affected by the ordering
    resolve_unaffected_pi(layout, ctn, pre2, pre2_t, latest_pos);
    resolve_unaffected_pi(layout, ctn, pre1, pre1_t, latest_pos);

    auto pre_fo = pre2;
    if (pre2_t.x > pre1_t.x)
    {
        std::swap(pre1_t, pre2_t);
        pre_fo = pre1;
    }
    // Area saving south rule
    // check if pre1_t is now also the northward tile
    if (pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo))
    {
        if (pre2_t.x == pre1_t.x)
        {
            // use larger x position of predecessors
            t = {latest_pos.x, pre2_t.y};
            ++latest_pos.x;
        }
        else
        {
            // use larger x position of predecessors
            t = {pre1_t.x, pre2_t.y};
        }
        if (pre2_t.y + 1 > latest_pos.y)
        {
            latest_pos.y = pre2_t.y + 1;
        }
    }
    else
    {
        if (latest_pos.y < latest_pos_inputs.y)
        {
            // use larger x position of predecessors
            t = {pre1_t.x, latest_pos_inputs.y};
            // each 2-input gate has one incoming bent wire
            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

            latest_pos.y = t.y + 1;
        }
        else
        {
            // use larger x position of predecessors
            t = {pre1_t.x, latest_pos.y};

            // each 2-input gate has one incoming bent wire
            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

            ++latest_pos.y;
        }
    }
}

/**
 * Placement and routing of a two fan-in gate colored null
 *
 * @tparam Ntk The logic network type.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param ctn Contains the colored network.
 * @param latest_pos Coordinates of next gate location.
 * @param t Viewed tile.
 * @param pre1_t Tile of the first preceding node.
 * @param pre2_t Tile of the second preceding node.
 * @param pre1 First preceding Node
 * @param pre2 Second preceding node
 */
template <typename Ntk, typename Lyt>
void place_two_fanin_gate_null(Lyt& layout, const coloring_container<Ntk>& ctn, tile<Lyt>& latest_pos, tile<Lyt>& t,
                               tile<Lyt>& pre1_t, tile<Lyt>& pre2_t, const mockturtle::node<Ntk>& pre1,
                               const mockturtle::node<Ntk>& pre2)
{
    // check which output direction is already used
    auto fos_pre1   = fanouts(ctn.color_ntk, pre1);
    auto color_pre1 = std::any_of(fos_pre1.begin(), fos_pre1.end(),
                                  [&ctn](const auto& fe) { return ctn.color_ntk.color(fe) == ctn.color_east; }) ?
                          ctn.color_south :
                          ctn.color_east;

    auto fos_pre2   = fanouts(ctn.color_ntk, pre2);
    auto color_pre2 = std::any_of(fos_pre2.begin(), fos_pre2.end(),
                                  [&ctn](const auto& fe) { return ctn.color_ntk.color(fe) == ctn.color_east; }) ?
                          ctn.color_south :
                          ctn.color_east;

    t = latest_pos;

    if (color_pre1 == ctn.color_east && color_pre2 == ctn.color_south)
    {
        // both wires have one bent
        pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
        pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));
    }
    else
    {
        // both wires have one bent
        pre1_t = static_cast<tile<Lyt>>(wire_south(layout, pre1_t, {pre1_t.x, t.y + 1}));
        pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {t.x + 1, pre2_t.y}));
    }

    ++latest_pos.x;
    ++latest_pos.y;
}

/**
 * Places the POs in the layout.
 *
 *@tparam Ntk The logic network type.
 * @tparam Ntk The logic network type of the input network.
 * @tparam Lyt Gate-level layout type.
 * @param layout Current gate-level layout.
 * @param inp_ntk Input network, so the order of the POs is preserved for equivalence checking.
 * @param ctn Contains the colored network.
 * @param latest_pos Coordinates of next gate location.
 * @param po_counter Counter for POs.
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 */
template <typename Ntk, typename NtkI, typename Lyt>
void place_outputs(Lyt& layout, NtkI inp_ntk, const coloring_container<Ntk>& ctn, tile<Lyt>& latest_pos,
                   uint32_t                                                                po_counter,
                   mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)>& node2pos)
{
    bool                               multi_out_node = false;
    std::vector<mockturtle::node<Ntk>> out_nodes;
    uint32_t                           inputs_border = ctn.color_ntk.num_pis();
    tile<Lyt>                          po_tile{};
    inp_ntk.foreach_po(
        [&po_counter, &out_nodes, &multi_out_node, &node2pos, &ctn, &layout, &latest_pos, &inputs_border,
         &po_tile](const auto& po)
        {
            if (!ctn.color_ntk.is_constant(po))
            {
                const auto n_s = node2pos[po];

                if (std::find(out_nodes.begin(), out_nodes.end(), po) != out_nodes.end())
                {
                    multi_out_node = true;
                }

                // determine PO orientation
                if (is_eastern_po_orientation_available(ctn, po) && !multi_out_node)
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    layout.resize({latest_pos.x, latest_pos.y - 1, 1});
                }
                else
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    po_tile = static_cast<tile<Lyt>>(wire_south(layout, po_tile, {po_tile.x, po_tile.y + 2}));
                    if (po_tile.y < inputs_border)
                    {
                        po_tile = static_cast<tile<Lyt>>(wire_south(layout, po_tile, {po_tile.x, latest_pos.y + 1}));
                        ++latest_pos.y;
                    }
                    layout.resize({latest_pos.x, latest_pos.y - 1, 1});
                }
                // check if PO position is located at the border
                if (layout.is_at_eastern_border({po_tile.x + 1, po_tile.y}) && !multi_out_node)
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
                    const tile<Lyt> anker{po_tile};

                    po_tile = layout.eastern_border_of(po_tile);

                    layout.create_po(wire_east(layout, anker, po_tile),
                                     ctn.color_ntk.has_output_name(po_counter) ?
                                         ctn.color_ntk.get_output_name(po_counter++) :
                                         fmt::format("po{}", po_counter++),
                                     po_tile);
                    multi_out_node = false;
                }

                out_nodes.push_back(po);
            }
        });
}

template <typename Lyt, typename Ntk>
class orthogonal_ordering_network_impl
{
  public:
    orthogonal_ordering_network_impl(const Ntk& src, const orthogonal_physical_design_params& p,
                                     orthogonal_physical_design_stats& st) :
            ntk{input_ordering_view{mockturtle::fanout_view{
                inverter_substitution(fanout_substitution<mockturtle::names_view<technology_network>>(src))}}},
            inp_ntk{mockturtle::fanout_view{
                inverter_substitution(fanout_substitution<mockturtle::names_view<technology_network>>(src))}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute coloring
        const auto ctn = conditional_coloring(ntk);

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        Lyt layout{{1, 1, 1}, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, inp_ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};
        // ordering network tracker
        tile<Lyt> latest_pos_inputs{0, 0};

        // Reserve new columns for inverters in the ordering network
        if (ctn.color_ntk.pi_inv_flag())
        {
            latest_pos.x = latest_pos.x + static_cast<uint64_t>(ctn.color_ntk.nc_inv_num()) + 1u;
        }
        // new column inverter tracker
        uint64_t insert_position_inv{1};

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] arranging layout: |{0}|"};
#endif
        // Find multi_output_nodes
        std::vector<mockturtle::node<Ntk>> output_nodes;
        std::vector<mockturtle::node<Ntk>> multi_out_nodes;
        ctn.color_ntk.foreach_po(
            [&](const auto& po)
            {
                if (std::find(output_nodes.begin(), output_nodes.end(), po) != output_nodes.end())
                {
                    multi_out_nodes.push_back(po);
                }
                output_nodes.push_back(po);
            });

        // Start of the algorithm
        ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                // do not place constants
                if (!ctn.color_ntk.is_constant(n))
                {
                    // if node is a PI, move it to its correct position
                    if (ctn.color_ntk.is_pi(n))
                    {
                        node2pos[n] = layout.move_node(pi2node[n], {latest_pos_inputs});
                        ++latest_pos_inputs.y;
                    }
                    // if n has only one fan-in
                    else if (const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            place_one_fanin_gate_east(ntk, layout, ctn, n, pre, latest_pos, pre_t, insert_position_inv,
                                                      node2pos);
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            place_one_fanin_gate_south(layout, ctn, n, latest_pos, latest_pos_inputs, pre_t, node2pos);
                        }
                        else
                        {
                            // single fan-in nodes should not be colored null
                            assert(false);
                        }
                    }
                    else  // if node has two fan-ins (or three fan-ins with one of them being constant)
                    {
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                             pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                        // the tile, where the gate is placed on
                        tile<Lyt> t{};

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            place_two_fanin_gate_east(layout, latest_pos, latest_pos_inputs, t, pre1_t, pre2_t);
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            place_two_fanin_gate_south(layout, ctn, latest_pos, latest_pos_inputs, pre1, pre2, t,
                                                       pre1_t, pre2_t);
                        }
                        // n is colored null; corner case
                        else
                        {
                            place_two_fanin_gate_null(layout, ctn, latest_pos, t, pre1_t, pre2_t, pre1, pre2);
                        }

                        // place and route from 'pre1_t' and 'pre2_t' to 't'
                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre1_t, pre2_t, fc.constant_fanin);
                    }
                    if (ctn.color_ntk.is_po(n))
                    {
                        if (!is_eastern_po_orientation_available(ctn, n) ||
                            std::find(multi_out_nodes.begin(), multi_out_nodes.end(), n) != multi_out_nodes.end())
                        {
                            ++latest_pos.y;
                            ++latest_pos_inputs.y;
                        }
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

        // Since the layout size is only known after placing all gates, the POs are placed after the main algorithm
        place_outputs(layout, inp_ntk, ctn, latest_pos, po_counter, node2pos);

        // restore possibly set signal names
        restore_names(ctn.color_ntk, layout, node2pos);

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        return layout;
    }

  private:
    /**
     * Network with ordered PIs
     */
    input_ordering_view<mockturtle::fanout_view<mockturtle::names_view<technology_network>>> ntk;
    /**
     * Network without ordered PIs
     */
    mockturtle::fanout_view<mockturtle::names_view<technology_network>> inp_ntk;
    /**
     * Design parameters for orthogonal P&R
     */
    orthogonal_physical_design_params ps;
    /**
     * Design stats for orthogonal P&R
     */
    orthogonal_physical_design_stats& pst;  // NOLINT
    // This line will suppress the warning cppcoreguidelines-avoid-const-or-ref-data-members
    /**
     * Counter for POs
     */
    uint32_t po_counter{0};
};

}  // namespace detail

/**
 * A modification of the `orthogonal` physical design algorithm as originally proposed in \"Versatile Signal
 * Distribution Networks for Scalable Placement and Routing of Field-coupled Nanocomputing Technologies\" by M. Walter,
 * B. Hien, R. Wille in ISVLSI 2023. The modification addresses certain limitations of the original and aim to save
 * area, reduce wiring and wire crossings, and optimize the placement of nodes.
 *
 * The first modification concerns the arrangement of PIs, which are linked as fan-ins to the same gate, and situating
 * them adjacently. This narrowing of distance between such "linked" nodes minimizes wiring effort and thereby saves
 * area.
 *
 * The second drawback of the original algorithm relates to conflicts that arise from the coloring of outgoing edges of
 * PIs, as it would lead to forbidden routing over other PIs. The modified algorithm uses a combination of PI ordering,
 * conditional coloring, and a new placement rule to resolve conflicts while still adhering to coloring constraints.
 * This enables the algorithm to utilize PI rows in the layout more effectively, hence, minimizing wire crossings.
 *
 * Finally, this algorithm addresses the challenges posed by inverters. The function `inverter_substitution`
 * substitutes inverters at the outputs of each fan-out node with a single inverter at its input.
 * This substitution significantly reduces area, wiring, and wire crossings, because fewer gates need to be placed.
 *
 * May throw a `high_degree_fanin_exception` if `ntk` contains any node with a fan-in larger than `2`.
 *
 * @tparam Lyt Desired gate-level layout type.
 * @tparam Ntk Network type that acts as specification.
 * @param ntk The network that is to be placed and routed.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return A gate-level layout of type `Lyt` that implements `ntk` as an FCN circuit.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] Lyt orthogonal_ordering_network(const Ntk& ntk, orthogonal_physical_design_params ps = {},
                                              orthogonal_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyway, therefore,
                                                 // this is the only relevant check here

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, 2))
    {
        throw high_degree_fanin_exception();
    }

    orthogonal_physical_design_stats                   st{};
    detail::orthogonal_ordering_network_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_ORTHO_ORDERING_NETWORK_HPP
