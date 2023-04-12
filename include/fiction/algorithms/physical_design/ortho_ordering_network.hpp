//
// Created by benjamin on 11.04.23.
//

#ifndef FICTION_ORTHO_ORDERING_NETWORK_HPP
#define FICTION_ORTHO_ORDERING_NETWORK_HPP

#include "fiction/networks/views/input_ordering_view.hpp"
#include "fiction/networks/views/reverse_view.hpp"
#include "orthogonal.hpp"

namespace fiction
{
namespace detail
{

/*This is needed for the conditional coloring*/
template <typename Ntk>
void paint_edge_if(const coloring_container<Ntk>& ctn,const mockturtle::edge<out_of_place_edge_color_view<Ntk>>& e, uint32_t clr)
{
    if (ctn.color_ntk.edge_color(e) != ctn.color_null)
    {
        return;
    }
    ctn.color_ntk.paint_edge(e, clr);
}
template <typename Ntk>
void paint_edge(const coloring_container<Ntk>& ctn,const mockturtle::edge<out_of_place_edge_color_view<Ntk>>& e, uint32_t clr)
{
    ctn.color_ntk.paint_edge(e, clr);
}

/*This is needed for the conditional coloring*/
template <typename Ntk>
void paint_if( const coloring_container<Ntk>& ctn, mockturtle::node<Ntk> const& n, uint32_t color )
{
    if (ctn.color_ntk.color(n) != ctn.color_null)
    {
        return;
    }
    ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, color);
}

template <typename Ntk>
void paint_node_and_edges(const coloring_container<Ntk>& ctn, mockturtle::node<Ntk> const& n, uint32_t color)
{
    const auto finc = fanin_edges(ctn.color_ntk, n);
    std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
    paint_if(ctn, n, color);
}

template <typename Ntk>
int getIndex(std::vector<Ntk> v, Ntk n)
{
    auto it = find(v.begin(), v.end(), n);

// If element was found
if (it != v.end())
{

    // calculating the index
    // of K
    int index = it - v.begin();
    return index;
}
else {
    // If the element is not
    // present in the vector
    return -1;
}
}

/*
 * Conditional coloring:
 * 1. All 1-fanin nodes (inverters and Fanouts) after PIs have to be colored east
 * 2. Pi connected to Fanout node: one east coloring and one south coloring
     * - Case: 2-inverters -> get balanced to the beginning of the fanout node (isFo_inv_flag has to be respected here for instance)
     * - Case: one inverter (color inverter east and other outgoing edges south)
         * - PI connected to both inverted and non-inverted node (color inverter east and both other edges south)
         * - PI connected to non-inverted node (color outgoing edge south)
         * - PI connected to inverted node (color inverter east and both outgoing edges south)
     * - Case: no inverter
         * - 2 PIs connected -> color one east and one south
         * - 1 PI connected -> color PI south and other node east
 * 3. Case PI connected to node connected to PI: color east
 * */
template <typename Ntk>
coloring_container<Ntk> conditional_coloring(const Ntk& ntk) noexcept
{
    coloring_container<Ntk> ctn{ntk};
    reverse_view            rtv{ntk};  // reverse order of nodes

    /*currently viewed node*/
    mockturtle::node<Ntk> current_node;

    /*The vectors of PIs viewed in the ordering network*/
    auto fo_two = ctn.color_ntk.get_fo_two();
    auto fo_one = ctn.color_ntk.get_fo_one();
    auto pi_pi = ctn.color_ntk.get_pi_to_pi();

    ntk.foreach_pi(
        [&](const auto& nd)
        {
            ntk.foreach_fanout(
                nd, [&](const auto& fon)
                {
                    /*Always track the current_node*/
                    current_node = fon;

                    /*Skip Inverters and color them east*/
                    if (ntk.is_inv(current_node))
                    {
                        /*Color Inverter east*/
                        paint_node_and_edges(ctn, current_node, ctn.color_east);
                        ntk.foreach_fanout(fon,
                                           [&](const auto& fon_inv)
                                           {
                                               /*Skip Inverter*/
                                               current_node = fon_inv;
                                           });
                    }
                    if (ntk.is_fanout(current_node) && ntk.fanout_size(current_node) >= 2)
                    {
                        paint_node_and_edges(ctn, current_node, ctn.color_east);

                        bool inv_flag = false;

                        ntk.foreach_fanout(current_node,
                                           [&](const auto& fon_f)
                                           {
                                               current_node = fon_f;
                                               if(ntk.is_inv(current_node))
                                               {
                                                   paint_node_and_edges(ctn, current_node, ctn.color_east);
                                                   inv_flag = true;
                                               }
                                           });

                        if (getIndex(fo_two, nd) % 3 == 0)
                        {
                            auto swap_color = ctn.color_south;
                            std::cout<<"PI2 "<<nd<<std::endl;
                            ntk.foreach_fanout(current_node,
                                               [&](const auto& fon_f)
                                               {
                                                   /*Jump to next node*/
                                                   current_node = fon_f;

                                                   if(ntk.is_inv(current_node))
                                                   {
                                                       paint_node_and_edges(ctn, current_node, ctn.color_east);
                                                       ntk.foreach_fanout(fon,
                                                                          [&](const auto& fon_inv)
                                                                          {
                                                                              /*Skip Inverter*/
                                                                              current_node = fon_inv;
                                                                          });
                                                       paint_node_and_edges(ctn, current_node, ctn.color_south);
                                                   }
                                                   else if(inv_flag)
                                                   {
                                                       paint_node_and_edges(ctn, current_node, ctn.color_south);
                                                   }
                                                   else
                                                   {
                                                       paint_node_and_edges(ctn, current_node, swap_color);
                                                       swap_color = ctn.color_east;
                                                   }

                                               });
                        }
                    }

                    else if(getIndex(pi_pi, nd) % 2 == 0)
                    {
                        std::cout<<"PI_PI "<<nd<<std::endl;
                        paint_node_and_edges(ctn, current_node, ctn.color_east);
                    }

                });
        });

#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.num_gates()),
                                 "[i] determining relative positions: |{0}|"};
#endif

    /*Color the rest of the network*/
    rtv.foreach_gate(
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

    return ctn;
}


template <typename Lyt, typename Ntk>
class orthogonal_ordering_network_impl
{
  public:
    orthogonal_ordering_network_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{input_ordering_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(src)}}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        //fiction::debug::write_dot_network(ntk, "ortho_inv_blc_o");
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = conditional_coloring(ntk);

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        //Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};
        Lyt layout{{1, 1, 1}, twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        auto pi2node = reserve_input_nodes(layout, ctn.color_ntk);

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};

        /*CHANGE so all inverters are placed into the first column*/
        if(ctn.color_ntk.isFo_inv_flag())
        {
            latest_pos.x = latest_pos.x + ctn.color_ntk.isFo_inv_flag_num();
        }
        tile<Lyt> latest_pos_inputs{0, 0};

        latest_pos.x = latest_pos.x;

        latest_pos_inputs.x = latest_pos_inputs.x;

        //std::cout<<"Inverter_Flag"<<ctn.color_ntk.isFo_inv_flag()<<std::endl;

        /*std::cout<<"Numpis: "<<layout.num_pis()<<std::endl;

        std::cout<<"Numros: "<<layout.num_ros()<<std::endl;

        std::cout<<"Numcis: "<<layout.num_cis()<<std::endl;*/

        std::vector<mockturtle::node<Ntk>> recolored_fanouts;

        int insert_position_inv = 1;

        bool po_resize = false;

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] arranging layout: |{0}|"};
#endif
        //Find multi_output_nodes
        std::vector<mockturtle::node<Ntk>> my_out_nodes;
        std::vector<mockturtle::node<Ntk>> multi_out_nodes;
        ctn.color_ntk.foreach_po(
            [&](const auto& po){
                const auto n_s = node2pos[po];

                tile<Lyt> po_tile{};

                if (std::find(my_out_nodes.begin(), my_out_nodes.end(), po) != my_out_nodes.end())
                {
                    multi_out_nodes.push_back(po);
                }
                my_out_nodes.push_back(po);
            });

        //Start of the algorithm
        ntk.foreach_node(
            [&ctn, this, &insert_position_inv, &multi_out_nodes, &node2pos, &layout, &pi2node, &latest_pos_inputs, &latest_pos](const auto& n, [[maybe_unused]] const auto i)
            {
                // do not place constants
                if (!ctn.color_ntk.is_constant(n))
                {
                    // if node is a PI, move it to its correct position
                    if (ctn.color_ntk.is_pi(n))
                    {
                        /**NEW CODE
                         * !!new latest_pos for inputs
                         * **/
                        node2pos[n] = layout.move_node(pi2node[n], {latest_pos_inputs});

                        /*Placing Inputs in first Column*/
                        std::cout<<n<<"Pi plaziert auf "<<"X:"<<latest_pos_inputs.x<<"Y:"<<latest_pos_inputs.y<<std::endl;

                        //++latest_pos.y;
                        ++latest_pos_inputs.y;
                    }
                    // if n has only one fanin
                    else if (const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                        /*if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre)){
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{n}, ctn.color_east);
                            ++latest_pos.y;
                        }*/

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            auto insert_position = latest_pos.x;
                            if(ntk.isFo_inv_flag() && ntk.is_inv(n) && ntk.is_pi(pre)){
                                insert_position = insert_position_inv;
                                ++insert_position_inv;
                                --latest_pos.x;
                                //std::cout<<"INV"<<ntk.isFo_inv_flag_num()<<std::endl;
                            }
                            /*oder ist inverter und nächster knoten ist pi*/
                            if(ctn.color_ntk.is_fanout(n) && ctn.color_ntk.is_pi(pre))
                            {
                                ++latest_pos.y;
                            }
                            const tile<Lyt> t{insert_position, pre_t.y};

                            if(ctn.color_ntk.is_fanout(n)){
                                std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }else{
                                std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }

                            std::cout<<n<<"Pre: "<<pre<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            if((ctn.color_ntk.is_inv(n) ||ctn.color_ntk.is_fanout(n)) && latest_pos.y<latest_pos_inputs.y){
                                const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                                if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre: "<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                latest_pos.y = t.y+1;
                            }
                            else
                            {
                                const tile<Lyt> t{pre_t.x, latest_pos.y};
                                if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre: "<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                ++latest_pos.y;
                            }
                        }
                        else
                        {
                            // single fanin nodes should not be colored null
                            assert(false);
                        }
                    }
                    else  // if node has two fanins (or three fanins with one of them being constant)
                    {
                        const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                        auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                             pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                        tile<Lyt> t{};

                        /**insert buffer for inputs colored south**/
                        /**maybe only if a fanout is connected to them**/
                        /*if(auto clr = ctn.color_ntk.color(pre1); ctn.color_ntk.is_ci(pre1) && clr != ctn.color_east)
                        {
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                            ++latest_pos.x;
                        }
                        if(auto clr = ctn.color_ntk.color(pre2); ctn.color_ntk.is_ci(pre2) && clr != ctn.color_east)
                        {
                            pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre2_t.y}));
                            ++latest_pos.x;
                        }*/


                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile
                            if (pre2_t.y < pre1_t.y){
                                std::swap(pre1_t, pre2_t);
                            }


                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                            ++latest_pos.x;
                            if(latest_pos.y < latest_pos_inputs.y){
                                latest_pos.y = t.y+1;
                            }

                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;

                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            auto pre_fo = pre2;
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                pre_fo = pre1;
                            }
                            bool new_south = pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo);
                            if(ctn.color_ntk.is_pi(pre2) && !new_south)
                            {
                                pre2_t = static_cast<tile<Lyt>>(wire_east(layout, pre2_t, {latest_pos.x + 1, pre2_t.y}));
                                ++latest_pos.x;
                            }
                            if(ctn.color_ntk.is_pi(pre1) && !new_south)
                            {
                                pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {latest_pos.x + 1, pre1_t.y}));
                                ++latest_pos.x;
                            }
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                pre_fo = pre1;
                            }
                            /**NEW CODE
                             * !!new south wire option
                             * **/
                            // check if pre1_t is now also the northwards tile
                            if (pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo))
                            {
                                if(pre2_t.x == pre1_t.x)
                                {
                                    // node can be placed on y position of pre2_t
                                    //std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {latest_pos.x, pre2_t.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                    ++latest_pos.x;
                                }
                                else
                                {
                                    // node can be placed on y position of pre2_t
                                    //std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {pre1_t.x, pre2_t.y};
                                    std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;
                                }
                                if(pre2_t.y+1>latest_pos.y)
                                {
                                    latest_pos.y =pre2_t.y+1;
                                }
                            }
                            /**!!**************************************************************************************/
                            else
                            {
                                if(latest_pos.y<latest_pos_inputs.y)
                                {
                                    // use larger x position of predecessors
                                    t = {pre1_t.x, latest_pos_inputs.y};
                                    // each 2-input gate has one incoming bent wire
                                    pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                                    latest_pos.y = t.y+1;
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
                        // n is colored null; corner case
                        else
                        {
                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            //std::swap(pre1_t, pre2_t);
                            /*if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                                !layout.is_empty_tile(layout.south(pre2_t)))
                                std::swap(pre1_t, pre2_t);*/

                            auto fos_pre1 = fanouts(ctn.color_ntk, pre1);
                            auto color_pre1 =
                                std::any_of(fos_pre1.begin(), fos_pre1.end(),
                                            [&ctn](const auto& fe) { return ctn.color_ntk.color(fe) == ctn.color_east; }) ?
                                    ctn.color_south :
                                    ctn.color_east;

                            auto fos_pre2 = fanouts(ctn.color_ntk, pre2);
                            auto color_pre2 =
                                std::any_of(fos_pre2.begin(), fos_pre2.end(),
                                            [&ctn](const auto& fe) { return ctn.color_ntk.color(fe) == ctn.color_east; }) ?
                                    ctn.color_south :
                                    ctn.color_east;

                            const auto ttt = layout.get_node(layout.east(pre1_t));

                            t = latest_pos;

                            std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"southeast"<<std::endl;

                            if(color_pre1 == ctn.color_east && color_pre2 == ctn.color_south)
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

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre1_t, pre2_t, fc.constant_fanin);
                    }
                    if (ctn.color_ntk.is_po(n))
                    {
                        if (!is_eastern_po_orientation_available(ctn, n) || std::find(multi_out_nodes.begin(), multi_out_nodes.end(), n) != multi_out_nodes.end())
                        {
                            ++latest_pos.y;
                        }
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

        /**********************************************************Begin: Place Pos***************************************************************/
        //Since the layout size is only known after placing all gates, the POs have to be placed after the main algorithm
        //
        //
        bool multi_out_node = false;
        std::vector<mockturtle::node<Ntk>> out_nodes;
        ctn.color_ntk.foreach_po(
            [this, &out_nodes, &multi_out_node, &node2pos, &ctn, &layout, &latest_pos](const auto& po){
                const auto n_s = node2pos[po];

                tile<Lyt> po_tile{};

                if(std::find(out_nodes.begin(), out_nodes.end(), po) != out_nodes.end())
                {
                    multi_out_node = true;
                }

                // determine PO orientation
                if (is_eastern_po_orientation_available(ctn, po) && !multi_out_node)
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    //++po_tile.x;
                    //++latest_pos.x;
                    layout.resize({latest_pos.x, latest_pos.y-1, 1});
                }
                else
                {
                    po_tile = static_cast<tile<Lyt>>(n_s);
                    //++latest_pos.y;
                    po_tile = static_cast<tile<Lyt>>(wire_south(layout, po_tile, {po_tile.x, po_tile.y + 2}));
                    //++po_tile.y;
                    layout.resize({latest_pos.x, latest_pos.y-1, 1});
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
                std::cout<<po<<"PO plaziert auf"<<"X:"<<po_tile.x<<"Y:"<<po_tile.y<<std::endl;
            });
        /**********************************************************End: Place Pos***************************************************************/

        // restore possibly set signal names
        restore_names(ctn.color_ntk, layout, node2pos);

        int crossing_count{0};
        layout.foreach_tile(
            [&layout, &crossing_count](const auto& t)
            {
                const auto nde = layout.get_node(t);
                if(layout.is_wire(nde))
                {
                    if(t.z == 1)
                    {
                        ++crossing_count;
                    }
                }
            });
        std::cout<<"Crossing_Num: "<<crossing_count<<std::endl;

        // statistical information
        pst.x_size    = layout.x() + 1;
        pst.y_size    = layout.y() + 1;
        pst.num_gates = layout.num_gates();
        pst.num_wires = layout.num_wires();

        std::cout<<"ntk.num_gates()"<<ntk.num_gates()<<std::endl;

        //std::cout<<"latest X: "<<latest_pos.x<<std::endl;
        //std::cout<<"latest Y: "<<latest_pos.y<<std::endl;

        return layout;
    }

  private:
    input_ordering_view<mockturtle::fanout_view<mockturtle::names_view<technology_network>>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
};

}  // namespace detail


template <typename Lyt, typename Ntk>
Lyt orthogonal_ordering_network(const Ntk& ntk, orthogonal_physical_design_params ps = {},
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

    orthogonal_physical_design_stats  st{};
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
