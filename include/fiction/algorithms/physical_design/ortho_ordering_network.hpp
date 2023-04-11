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

/*Conditional coloring*/
template <typename Ntk>
coloring_container<Ntk> conditional_coloring(const Ntk& ntk) noexcept
{
    coloring_container<Ntk> ctn{ntk};
    reverse_view            rtv{ntk};  // reverse order of nodes

    /*Coloring for Input Network*/
    bool fo_node = false;
    bool node_pi = false;
    bool node_fo = false;
    bool inv_flag = false;

    bool po_flag = false;

    std::vector<typename Ntk::node> new_output_node;
    mockturtle::node<Ntk> safe_node;

    ntk.foreach_ci(
        [&](const auto& nd){
            ntk.foreach_fanout(
                nd,
                [&](const auto& fon)
                {
                    fo_node = false;
                    node_pi = false;
                    node_fo = false;
                    inv_flag = false;
                    po_flag = false;

                    /*
                     * 1. All 1-fanin nodes (inverters and Fanouts) after PIs have to be colored east
                     * 2. Pi connected to Fanout node: one east coloring and one south coloring
                     * - Case: 2-inverters -> get balanced to the beginning of the fanout node (isFo_inv_flag has to be respected here for instance)
                     * - Case: one inverter (color inverter east and both outgoing edges south)
                     * - PI connected to both inverted and non-inverted node (color inverter east and both outgoing edges south)
                     * - PI connected to non-inverted node
                     * - PI connected to inverted node
                     * - Case: no inverter
                     * - 2 PIs connected -> color one east and one south
                     * - 1 PI connected -> color PI south and other node east
                     * 3. Case PI connected to node connected to PI: color east
                     * */

                    /*Ignore Inverters*/
                    new_output_node.clear();
                    new_output_node.push_back(fon);
                    if (ntk.is_inv(fon))
                    {
                        ntk.foreach_fanout(fon,
                                           [&](const auto& fon_inv) {
                                               new_output_node[0] = fon_inv;

                                               /*COLOR INVERTER EAST*/
                                               auto color = ctn.color_east;
                                               const auto finc = fanin_edges(ctn.color_ntk, fon);
                                               std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                                             [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                           });
                    }
                    if (ntk.is_fanout(new_output_node[0]) && ntk.fanout_size(new_output_node[0]) >= 2){
                        safe_node = new_output_node[0];
                        fo_node = true;

                        /*COLOR FAN-OUT EAST*/
                        auto color = ctn.color_east;
                        const auto finc = fanin_edges(ctn.color_ntk, new_output_node[0]);
                        std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });

                        new_output_node.clear();
                        ntk.foreach_fanout(safe_node,
                                           [&](const auto& fon) {
                                               if (ntk.is_inv(fon))
                                               {
                                                   inv_flag = true;

                                                   if(ntk.is_po(fon))
                                                   {
                                                       po_flag = true;
                                                   }

                                                   /*Inverter Flag*/
                                                   ntk.foreach_fanout(fon,
                                                                      [&](const auto& fon_inv) {new_output_node.insert(new_output_node.begin(), fon_inv);});
                                               }
                                               else{
                                                   if(ntk.is_po(fon))
                                                   {
                                                       po_flag = true;
                                                   }
                                                   /*COLOR NODE SOUTH AND COLOR THE INCOMING EDGES OF THIS NODE ALSO SOUTH*/
                                                   new_output_node.push_back(fon);
                                               }

                                               /*COLOR THE OTHER OUTGOING EDGE OF THE FAN-OUT EAST*/
                                               /*IF the node is an inverter color next edge also east*/
                                           });
                    }

                    /*The Fan-out has to be connected to 1.Fan-out nodes of Inputs or 2.Inputs*/

                    /*Instead again use trav_id()*/
                    bool already_painted = false;
                    bool all_inputs = false;

                    for(int i = 0; i <new_output_node.size(); ++i)
                    {
                        ntk.foreach_fanin(
                            new_output_node[i],
                            [&](const auto& fi)
                            {
                                auto fin_inp = ntk.get_node(fi);
                                /*Ignore Inverters*/
                                if (ntk.is_inv(fin_inp))
                                {
                                    auto color = ctn.color_east;
                                    const auto fin_inv = fanin_edges(ctn.color_ntk, fin_inp);
                                    std::for_each(fin_inv.fanin_edges.cbegin(), fin_inv.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                    paint_if(ctn, fin_inp, color);
                                    const auto fis_inv = fanins(ntk, fin_inp);
                                    fin_inp            = fis_inv.fanin_nodes[0];
                                }
                                /*1*/
                                if (ntk.fanout_size(fin_inp) >= 2 && ntk.is_fanout(fin_inp) && !po_flag)
                                {
                                    //For coloring fanouts south
                                    /*auto color = ctn.color_south;
                                    const auto finc = fanin_edges(ctn.color_ntk, safe_node);
                                    std::for_each(finc.fanin_edges.cbegin(), finc.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge(ctn, fe, color); });*/
                                    ntk.foreach_fanin(
                                        fin_inp,
                                        [&](const auto& fin)
                                        {
                                            auto fin_inp_sec = ntk.get_node(fin);
                                            /*Ignore Inverters*/
                                            if (ntk.is_inv(fin_inp_sec))
                                            {
                                                const auto fis_inv = fanins(ntk, fin_inp_sec);
                                                fin_inp_sec        = fis_inv.fanin_nodes[0];
                                            }
                                            if (ntk.is_ci(fin_inp_sec) == true &&
                                                fin_inp_sec != nd)
                                            {
                                                if(fo_node && !already_painted){
                                                    if (inv_flag)
                                                    {
                                                        auto color = ctn.color_south;
                                                        const auto finc_fo = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        i = 1 - i;

                                                        /*Color other fan-out south*/
                                                        color = ctn.color_south;
                                                        const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        already_painted = true;
                                                    }
                                                    else
                                                    {
                                                        /*Color first fan-out node east*/
                                                        auto color = ctn.color_east;
                                                        const auto finc_fo = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_fo.fanin_edges.cbegin(), finc_fo.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        i = 1 - i;

                                                        /*Color other fan-out south*/
                                                        color = ctn.color_south;
                                                        const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                        std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                                        paint_if(ctn, new_output_node[i], color);

                                                        already_painted = true;
                                                    }

                                                }
                                                node_fo = true;
                                                all_inputs = true;
                                            }
                                        });
                                }
                                /*else if (po_flag)
                                {
                                    auto color = ctn.color_south;
                                    const auto finc_other = fanin_edges(ctn.color_ntk, new_output_node[0]);
                                    std::for_each(finc_other.fanin_edges.cbegin(), finc_other.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                    paint_if(ctn, new_output_node[0], color);

                                    already_painted = true;
                                }*/
                                /*2*/
                                else if (ntk.is_ci(fin_inp) == true && fin_inp != nd)
                                {
                                    if(fo_node)
                                    {
                                        if (!already_painted)
                                        {
                                            if (inv_flag)
                                            {
                                                /*Color first fan-out node south*/
                                                auto       color    = ctn.color_south;
                                                const auto finc_inv = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_inv.fanin_edges.cbegin(),
                                                              finc_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);

                                                /*Color other fan-out south*/
                                                color                  = ctn.color_south;
                                                const auto finc_no_inv = fanin_edges(ctn.color_ntk, new_output_node[i + 1]);
                                                std::for_each(finc_no_inv.fanin_edges.cbegin(),
                                                              finc_no_inv.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i+1], color);
                                            }
                                            else
                                            {
                                                /*Color first fan-out node south*/
                                                auto       color           = ctn.color_south;
                                                const auto finc_no_inv_one = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_one.fanin_edges.cbegin(),
                                                              finc_no_inv_one.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);

                                                i = 1 - i;

                                                /*Color other fan-out east*/
                                                color = ctn.color_east;
                                                const auto finc_no_inv_two =
                                                    fanin_edges(ctn.color_ntk, new_output_node[i]);
                                                std::for_each(finc_no_inv_two.fanin_edges.cbegin(),
                                                              finc_no_inv_two.fanin_edges.cend(),
                                                              [&ctn, &color](const auto& fe)
                                                              { paint_edge_if(ctn, fe, color); });
                                                paint_if(ctn, new_output_node[i], color);
                                            }
                                            already_painted = true;
                                        }
                                    }
                                    else
                                    {
                                        auto color = ctn.color_east;
                                        const auto finc_only_pis = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                        std::for_each(finc_only_pis.fanin_edges.cbegin(), finc_only_pis.fanin_edges.cend(),
                                                      [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                        paint_if(ctn, new_output_node[i], color);
                                    }
                                    node_pi = true;
                                    all_inputs = true;
                                }
                                else if (ntk.is_maj(fin_inp))
                                {
                                    auto color = ctn.color_east;
                                    const auto finc_only_pis = fanin_edges(ctn.color_ntk, new_output_node[i]);
                                    std::for_each(finc_only_pis.fanin_edges.cbegin(), finc_only_pis.fanin_edges.cend(),
                                                  [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                    paint_if(ctn, new_output_node[i], color);
                                }


                            });
                    }
                });
        });

    ntk.foreach_ci(
        [&](const auto& nd) {
            ntk.foreach_fanout(nd,
                               [&](const auto& fon) {
                                   //This was to color gates east even after inverters have been placed:
                                   // resolved by placing inverters in the ordering network in seperate columns
                                   /*if (ntk.is_inv(fon))
                                   {
                                       auto fos = fanouts(ctn.color_ntk, fon);
                                       auto fo = fos[0];
                                       auto color = ctn.color_east;
                                       const auto fo_edges = fanin_edges(ctn.color_ntk, fo);
                                       std::for_each(fo_edges.fanin_edges.cbegin(), fo_edges.fanin_edges.cend(),
                                                     [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                       paint_if(ctn, fo, color);
                                   }*/
                                   auto color = ctn.color_east;
                                   const auto fo_edges = fanin_edges(ctn.color_ntk, fon);
                                   std::for_each(fo_edges.fanin_edges.cbegin(), fo_edges.fanin_edges.cend(),
                                                 [&ctn, &color](const auto& fe) { paint_edge_if(ctn, fe, color); });
                                   paint_if(ctn, fon, color);
                               });
        });

#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.num_gates()),
                                 "[i] determining relative positions: |{0}|"};
#endif


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

/*
 * Check edges, which have to be buffered according to the delays resulting from majority gates
 * */

template <typename Ntk>
std::vector<int> majority_buffer(const Ntk& ntk, mockturtle::node<Ntk> n) noexcept
{
    std::vector<mockturtle::node<Ntk>> delay_nodes;

    int size{0};
    int iterator{0};
    const auto fc = fanins(ntk, n);
    size = fc.fanin_nodes.size();

    std::vector<int>                         delays(size);
    //std::vector<mockturtle::node<Ntk>> incoming_path_node(size);
    if(size != 0){

        foreach_incoming_edge(ntk, n,
                              [&](const auto& e)
                              {
                                  if(!ntk.is_constant(e.source))
                                  {
                                      auto node_paths = all_incoming_edge_paths(ntk, e.source);
                                      // incoming_path_node[iterator] = e.source;

                                      for (int i = 0; i < node_paths.size(); ++i)
                                      {
                                          int path_delay = 0;
                                          for (int j = 0; j < node_paths[i].size(); ++j)
                                          {
                                              if (const auto ft = fanins(ntk, node_paths[i][j].target); ntk.is_maj(node_paths[i][j].target) && ft.fanin_nodes.size()>2)
                                              {
                                                  ++path_delay;
                                              }
                                          }
                                          if (delays[iterator] < path_delay)
                                          {
                                              delays[iterator] = path_delay;
                                          }
                                      }
                                      ++iterator;
                                  }
                              });

        int max = *std::max_element(delays.begin(), delays.end());
        for(int k=0; k<delays.size(); ++k)
        {
            delays[k] = max - delays[k];
        }
    }

    return delays;
}

template <typename Lyt, typename Ntk>
class orthogonal_ordering_network_impl
{
  public:
    orthogonal_ordering_network_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{input_ordering_view{mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(src))}}},
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
        if(ctn.color_ntk.isFo_inv_flag()){
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


        //This is only for additional coloring, if the conditional coloring isnt correct
        ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                if(auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() ==2)
                {
                    const auto &pre1 = fc.fanin_nodes[0], pre2 = fc.fanin_nodes[1];

                    auto pre1_t = static_cast<tile<Lyt>>(node2pos[pre1]),
                         pre2_t = static_cast<tile<Lyt>>(node2pos[pre2]);

                    tile<Lyt> t{};
                    if(auto fos = fanouts(ctn.color_ntk, pre1); ctn.color_ntk.is_fanout(pre1) && fos.size()>1)
                    {

                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            if(!(std::find(recolored_fanouts.begin(), recolored_fanouts.end(), fos[1]) != recolored_fanouts.end()))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_east);
                                recolored_fanouts.push_back(fos[1]);
                            }else
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                            }

                        }
                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; }))
                        {
                            auto pre_fo = pre2;
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                pre_fo = pre1;
                            }
                            bool new_south = pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo);

                            if(!(std::find(recolored_fanouts.begin(), recolored_fanouts.end(), fos[1]) != recolored_fanouts.end()) && !(ctn.color_ntk.is_ci(pre1)/*&&!new_south*/))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_south);
                                recolored_fanouts.push_back(fos[1]);
                            }else
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                    }
                    if(auto fos = fanouts(ctn.color_ntk, pre2); ctn.color_ntk.is_fanout(pre2) && fos.size()>1)
                    {

                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            if(!(std::find(recolored_fanouts.begin(), recolored_fanouts.end(), fos[1]) != recolored_fanouts.end()))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_east);
                                recolored_fanouts.push_back(fos[1]);
                            }else
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; }))
                        {
                            auto pre_fo = pre2;
                            // pre2_t is the westwards tile
                            if (pre2_t.x > pre1_t.x){
                                std::swap(pre1_t, pre2_t);
                                pre_fo = pre1;
                            }
                            bool new_south = pre1_t.y < pre2_t.y && !ctn.color_ntk.is_fanout(pre_fo);
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            if(!(std::find(recolored_fanouts.begin(), recolored_fanouts.end(), fos[1]) != recolored_fanouts.end())  && !(ctn.color_ntk.is_ci(pre1)/*&&!new_south*/))
                            {
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_south);
                                recolored_fanouts.push_back(fos[1]);
                            }else
                                ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                    }
                    //is still not resolved we need color_null
                    if(auto fos = fanouts(ctn.color_ntk, pre1); ctn.color_ntk.is_fanout(pre1) && fos.size()>1)
                    {

                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                    }
                    if(auto fos = fanouts(ctn.color_ntk, pre2); ctn.color_ntk.is_fanout(pre2) && fos.size()>1)
                    {

                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_south; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                        if(std::all_of(fos.cbegin(), fos.cend(),
                                        [&](const auto& fo) { return ctn.color_ntk.color(fo) == ctn.color_east; }))
                        {
                            //evtl Spezialfall color = color_null, dann wird aber majority_buffer bei wiring von color_null benötigt
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                        }
                    }}});

        //This is only for additional coloring, if the conditional coloring isn't correct
        ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                if(const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                {
                    if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_null)
                    {
                        auto fanout_node = fc.fanin_nodes[0];
                        auto fos = fanouts(ctn.color_ntk, fanout_node);
                        const auto clr_fo_0 = ctn.color_ntk.color(fos[0]);
                        const auto clr_fo_1 = ctn.color_ntk.color(fos[1]);
                        if(clr_fo_0 == ctn.color_null)
                        {
                            //switch the colors
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, ctn.color_null);
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[0]}, clr_fo_1);
                        }
                        if(clr_fo_1 == ctn.color_null)
                        {
                            //switch the colors
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[0]}, ctn.color_null);
                            ctn.color_ntk.paint(mockturtle::node<Ntk>{fos[1]}, clr_fo_0);
                        }
                    }
                }
            });

        //Start of the algorithm
        ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
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
                        //std::cout<<n<<"Pi plaziert auf "<<"X:"<<latest_pos_inputs.x<<"Y:"<<latest_pos_inputs.y<<std::endl;

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

                            /*if(ctn.color_ntk.is_fanout(n)){
                                std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }else{
                                std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            }

                            std::cout<<n<<"Pre"<<pre<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;*/
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            if((ctn.color_ntk.is_inv(n) ||ctn.color_ntk.is_fanout(n)) && latest_pos.y<latest_pos_inputs.y){
                                const tile<Lyt> t{pre_t.x, latest_pos_inputs.y};
                                /*if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre"<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;*/
                                node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                                latest_pos.y = t.y+1;
                            }
                            else
                            {
                                const tile<Lyt> t{pre_t.x, latest_pos.y};
                                /*if(ctn.color_ntk.is_fanout(n)){
                                    std::cout<<n<<"FO plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }else{
                                    std::cout<<n<<"Inv plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                }
                                std::cout<<n<<"Pre"<<pre<<std::endl;
                                std::cout<<n<<"color: "<<"south"<<std::endl;*/
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

                            /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"east"<<std::endl;*/

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
                                    /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;*/
                                    ++latest_pos.x;
                                }
                                else
                                {
                                    // node can be placed on y position of pre2_t
                                    //std::cout<<"Neue south wires für: "<<n<<std::endl;

                                    // use larger x position of predecessors
                                    t = {pre1_t.x, pre2_t.y};
                                    /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                                    std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                                    std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                                    std::cout<<n<<"color: "<<"south"<<std::endl;*/
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

                            /*std::cout<<n<<"And plaziert auf"<<"X:"<<t.x<<"Y:"<<t.y<<std::endl;
                            std::cout<<n<<"Pre1: "<<pre1<<std::endl;
                            std::cout<<n<<"Pre2: "<<pre2<<std::endl;
                            std::cout<<n<<"color: "<<"southeast"<<std::endl;*/

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
            [&](const auto& po){
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
                //std::cout<<po<<"PO plaziert auf"<<"X:"<<po_tile.x<<"Y:"<<po_tile.y<<std::endl;
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
