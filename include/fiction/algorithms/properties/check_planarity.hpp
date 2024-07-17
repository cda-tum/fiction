//
// Created by benjamin on 7/17/24.
//

#ifndef FICTION_CHECK_PLANARITY_HPP
#define FICTION_CHECK_PLANARITY_HPP

#include "fiction/algorithms/network_transformation/network_balancing.hpp"

#include <stdexcept>

namespace fiction
{

template <typename Ntk>
class check_planarity_impl
{
  public:
    check_planarity_impl(const Ntk& ntk) : ntk(ntk) {}

    bool run()
    {
        bool return_false = false;
        for (uint32_t r = 1; r < ntk.depth() + 1; r++)
        {
            uint32_t bound = 0;
            ntk.foreach_node_in_rank(r,
                                     [this, &bound, &return_false](const auto& n)
                                     {
                                         uint32_t new_bound = bound;
                                             ntk.foreach_fanin(n,
                                                           [this, &bound, &new_bound, &return_false](const auto& fi)
                                                           {
                                                               const auto fi_n = ntk.get_node(fi);
                                                               if (ntk.rank_position(fi_n) < bound)
                                                               {
                                                                   return_false = true;
                                                               }
                                                               new_bound = std::max(new_bound, ntk.rank_position(fi_n));
                                                           });
                                         if (return_false)
                                             return;
                                         bound = new_bound;
                                     });
            if (return_false)
                return false;
        }
        return true;
    }

  private:
    Ntk ntk;
};

template <typename Ntk>
bool check_planarity(Ntk& ntk)
{
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not have rank position trait");
    static_assert(mockturtle::has_at_rank_position_v<Ntk>, "Ntk does not have at rank position trait");
    static_assert(mockturtle::has_swap_v<Ntk>, "Ntk does not have swap trait");
    static_assert(mockturtle::has_width_v<Ntk>, "Ntk does not have width trait");
    static_assert(mockturtle::has_foreach_node_in_rank_v<Ntk>, "Ntk does not have foreach node in rank trait");
    static_assert(mockturtle::has_foreach_gate_in_rank_v<Ntk>, "Ntk does not have foreach gate in rank trait");

    if (!is_balanced(ntk))
    {
        // ToDo: Instead drag this to the implementation and insert buffers so that they divide multi-level edges
        throw std::runtime_error("Network must be balanced");
    }

    check_planarity_impl<Ntk> p{ntk};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_CHECK_PLANARITY_HPP
