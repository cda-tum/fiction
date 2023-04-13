//
// Created by benjamin on 13.04.23.
//

#ifndef FICTION_INVERTER_BALANCING_HPP
#define FICTION_INVERTER_BALANCING_HPP
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"
#include "mockturtle/views/fanout_view.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <type_traits>

#include <signal.h>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

template<typename NtkSrc>
NtkSrc recursive(const NtkSrc& ntk);

template <typename NtkSrc>
class inverter_balancing_impl
{
  public:
    explicit inverter_balancing_impl(const NtkSrc& ntk_src) : ntk{ntk_src} {}

    NtkSrc run()
    {
        auto  init     = mockturtle::initialize_copy_network<NtkSrc>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename NtkSrc::signal> children{};

            if (fo_ntk.is_fanout(n))
            {
                auto fanout_inv = fanouts(fo_ntk, n);
                auto balance = std::all_of(fanout_inv.begin(), fanout_inv.end(),[this](const auto& fo_node)
                                              {return fo_ntk.is_inv(fo_node);});
                if(balance && fanout_inv.size()>1){
                    /*del_inv = fanout_inv[1];
                    blc_fos=n;*/
                    del_inv.emplace_back(fanout_inv[1]);
                    blc_fos.emplace_back(n);
                    new_inv.emplace_back(fanout_inv[0]);
                }
            }

            ntk.foreach_fanin(n,
                              [this, &old2new, &children, &n](const auto& f)
                              {
                                  auto fn         = ntk.get_node(f);

                                  /*New children for balanced fan-out nodes*/
                                  /*if(blc_fos==n){
                                      const auto fos = fanouts(fo_ntk, n);
                                      //fn = fos[0];
                                      new_inv=fos[0];
                                  }*/

                                  /*New children for new inverter node*/
                                  /*bool inv_ch = false;
                                  for(auto i = new_inv.begin(); i < new_inv.end(); ++i){
                                      if(*i == n){
                                          inv_ch=true;
                                      }
                                  }
                                  if(inv_ch){
                                      const auto fis_inv = fanins(fo_ntk, fn);
                                      fn = fis_inv.fanin_nodes[0]; //fan-in of the inverter is the fan-out node
                                      const auto fis_fo = fanins(fo_ntk, fn);
                                      fn = fis_fo.fanin_nodes[0]; //fan-in of the fan-out node is the PI node
                                  }*/

                                  /*New children for nodes with deleted Inverters as fan-in*/
                                  for(int i = 0; i<blc_fos.size(); ++i)
                                  {
                                      if(del_inv[i] == fn)
                                      {
                                          const auto fis = fanins(fo_ntk, fn);
                                          fn = new_inv[i];
                                      }
                                  }

                                  /*if(del_inv == fn){
                                      const auto fis = fanins(fo_ntk, fn);
                                      fn = new_inv;
                                  }*/

                                  const auto tgt_signal = old2new[fn];

                                  /*children.emplace_back(ntk.is_complemented(f) ? ntk_dest.create_not(tgt_signal) :
                                                                                 tgt_signal);*/

                                  children.emplace_back(tgt_signal);
                              });

            return children;
        };


#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] network conversion: |{0}|"};
#endif

        ntk.foreach_gate(
            [this, &gather_fanin_signals, &old2new, &ntk_dest](const auto& g, [[maybe_unused]] auto i)
            {
                if constexpr (mockturtle::has_is_ro_v<NtkSrc>){
                    if(ntk.is_ro(g)){
                        return true;
                    }
                }

                auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                if constexpr (has_is_inv_v<TopoNtkSrc>)
                {
                    if (ntk.is_inv(g) && std::find(new_inv.begin(), new_inv.end(), g) != new_inv.end())
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                    auto po_it = std::find(del_inv.begin(), del_inv.end(), g);
                    if (ntk.is_inv(g) && po_it != del_inv.end())
                    {
                        if(ntk.is_po(g))
                        {
                            //Preserve Outputs
                            int index = po_it - del_inv.begin();
                            preserved_po.push_back(new_inv[index]);
                        }
                        //old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_buf_v<NtkSrc>)
                {
                    if (ntk.is_buf(g) && std::find(blc_fos.begin(), blc_fos.end(), g) != blc_fos.end())
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_and_v<TopoNtkSrc> && mockturtle::has_create_and_v<NtkSrc>)
                {
                    if (ntk.is_and(g))
                    {
                        old2new[g] = ntk_dest.create_and(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_or_v<TopoNtkSrc> && mockturtle::has_create_or_v<NtkSrc>)
                {
                    if (ntk.is_or(g))
                    {
                        old2new[g] = ntk_dest.create_or(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_xor_v<TopoNtkSrc> && mockturtle::has_create_xor_v<NtkSrc>)
                {
                    if (ntk.is_xor(g))
                    {
                        old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_maj_v<TopoNtkSrc> && mockturtle::has_create_maj_v<NtkSrc>)
                {
                    if (ntk.is_maj(g))
                    {
                        old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_and_v<TopoNtkSrc> && mockturtle::has_create_nary_and_v<NtkSrc>)
                {
                    if (ntk.is_nary_and(g))
                    {
                        old2new[g] = ntk_dest.create_nary_and(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_or_v<TopoNtkSrc> && mockturtle::has_create_nary_or_v<NtkSrc>)
                {
                    if (ntk.is_nary_or(g))
                    {
                        old2new[g] = ntk_dest.create_nary_or(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_xor_v<TopoNtkSrc> && mockturtle::has_create_nary_xor_v<NtkSrc>)
                {
                    if (ntk.is_nary_xor(g))
                    {
                        old2new[g] = ntk_dest.create_nary_xor(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<NtkSrc>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;
                }

                return true;
            });

        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                auto tgt_signal = old2new[ntk.get_node(po)];
                auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                if(tgt_po == 0)
                {
                    tgt_signal = old2new[ntk.get_node(preserved_po[0])];
                    preserved_po.erase(preserved_po.begin());
                    tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                }
                ntk_dest.create_po(tgt_po);
            });

        if constexpr (mockturtle::has_foreach_ri_v<TopoNtkSrc>){
            ntk.foreach_ri(
                [this, &ntk_dest, &old2new](const auto& po)
                {
                    const auto tgt_signal = old2new[ntk.get_node(po)];
                    const auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;

                    ntk_dest.create_ri(tgt_po);
                });
        }

        // restore signal names if applicable
        fiction::restore_names(ntk, ntk_dest, old2new);


        return ntk_dest;

    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<NtkSrc>;
    TopoNtkSrc ntk;
    mockturtle::fanout_view<TopoNtkSrc> fo_ntk{ntk};
    using node = typename NtkSrc::node;
    std::vector<node> del_inv;
    std::vector<node> new_inv;
    std::vector<node> blc_fos;
    std::vector<node> preserved_po;
};

template<typename NtkSrc>
bool inverter_balancing_recursive(const NtkSrc& ntk){
    bool return_val = false;
    const auto fo_ntk= mockturtle::fanout_view<NtkSrc>(ntk);
    //detail::inverter_balancing_impl<NtkSrc> k(ntk);
    ntk.foreach_gate(
        [&](const auto& g)
        {
            if (ntk.fanout_size(g)>=2)
            {
                auto fanout_inv = fanouts(fo_ntk, g);
                auto balance = std::all_of(fanout_inv.begin(), fanout_inv.end(),[&](const auto& fo_node)
                                              {return fo_ntk.is_inv(fo_node);});
                if(balance && fanout_inv.size()>1){
                    return_val = true;
                }
            }
        });
    return return_val;
}

}  // namespace detail

/**
 * Converts a logic network into an equivalent one of another type. Thereby, this function is very similar to
 * mockturtle::cleanup_dangling. However, it supports real buffer nodes used for fanouts and path balancing in fiction.
 *
 * If all fanouts of a node are inverted, the inverters are replaced by one inverter as fanin of this node
 * This is part of the Distribution Newtork I: Inputs
 *
 * NOTE: In contrast to mockturtle::cleanup_dangling, this function returns ntk if NtkDest and NtkSrc are of the same
 * type.
 *
 * @tparam NtkDest Type of the returned logic network.
 * @tparam NtkSrc Type of the input logic network.
 * @param ntk The input logic network.
 * @return A logic network of type NtkDest that is logically equivalent to ntk.*/


template <typename NtkSrc>
NtkSrc inverter_balancing(const NtkSrc& ntk)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");

    static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");
    static_assert(mockturtle::has_is_complemented_v<NtkSrc>, "NtkSrc does not implement the is_complemented function");
    static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi function");
    static_assert(mockturtle::has_foreach_gate_v<NtkSrc>, "NtkSrc does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_po_v<NtkSrc>, "NtkSrc does not implement the foreach_po function");
    static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");

    static_assert(mockturtle::has_get_constant_v<NtkSrc>, "NtkSrc does not implement the get_constant function");

    static_assert(mockturtle::has_create_pi_v<NtkSrc>, "NtkDest does not implement the create_pi function");
    static_assert(mockturtle::has_create_po_v<NtkSrc>, "NtkDest does not implement the create_po function");
    static_assert(mockturtle::has_create_not_v<NtkSrc>, "NtkDest does not implement the create_not function");
    static_assert(mockturtle::has_create_and_v<NtkSrc>, "NtkDest does not implement the create_and function");
    static_assert(mockturtle::has_create_or_v<NtkSrc>, "NtkDest does not implement the create_or function");
    static_assert(mockturtle::has_create_xor_v<NtkSrc>, "NtkDest does not implement the create_xor function");
    static_assert(mockturtle::has_create_maj_v<NtkSrc>, "NtkDest does not implement the create_maj function");

    static_assert(has_is_fanout_v<NtkSrc>, "NtkDest does not implement the has_is_fanout function");
    // TODO handle ci/ro/etc...

    //assert(ntk.is_combinational() && "Network has to be combinational");

    detail::inverter_balancing_impl<NtkSrc> p{ntk};

    auto result = p.run();

    bool rerun = detail::inverter_balancing_recursive(result);

    do{
        detail::inverter_balancing_impl<NtkSrc> k{result};

        result = k.run();

        rerun = detail::inverter_balancing_recursive(result);
    }
    while(rerun);

    return result;
}

}  // namespace fiction
#endif  // FICTION_INVERTER_BALANCING_HPP
