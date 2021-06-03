//
// Created by marcel on 13.07.17.
//

#ifndef FICTION_ORTHOGONAL_HPP
#define FICTION_ORTHOGONAL_HPP

#include "../algorithms/name_restoration.hpp"
#include "../layouts/clocking_scheme.hpp"
#include "../utils/debug/network_writer.hpp"
#include "fanout_substitution.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <vector>

namespace fiction
{

struct orthogonal_physical_design_params
{
    uint8_t number_of_clock_phases = 4u;

    bool utilize_io_ports = false;

    bool route_ios_to_layout_borders = false;
};

namespace detail
{
/**
 * Determine siblings of the given node. A sibling is a node that shares the same fan-in with n.
 * @param n Node to consider.
 * @return Siblings of n.
 */
template <typename Ntk>
std::vector<mockturtle::node<Ntk>> siblings(const Ntk& ntk, const mockturtle::node<Ntk> n) noexcept
{
    std::vector<mockturtle::node<topology_network>> sibs{};
    ntk.foreach_fanin(n,
                      [&ntk, &sibs, &n](const auto& fi)
                      {
                          ntk.foreach_fanout(fi,
                                             [&ntk, &sibs, &n](const auto& fo)
                                             {
                                                 // do not consider constants or n itself
                                                 if (const auto fon = ntk.get_node(fo);
                                                     !ntk.is_constant(fon) && fon != n)
                                                 {
                                                     sibs.push_back(fon);
                                                 }
                                             });
                      });

    return sibs;
}

template <typename Ntk>
struct coloring_container
{
    explicit coloring_container(const Ntk& ntk) :
            color_ntk{ntk},
            color_east{color_ntk.new_color()},
            color_south{color_ntk.new_color()}
    {}

    mockturtle::out_of_place_color_view<Ntk> color_ntk;

    const uint32_t color_null = 0ul, color_east, color_south;
};

/**
 * Nodes colored east have signals incoming from eastern direction. Nodes colored south have signals incoming from
 * southern direction.
 * @tparam Ntk
 * @param ntk
 * @return
 */
template <typename Ntk>
coloring_container<Ntk> east_south_coloring(const Ntk& ntk)
{
    coloring_container<Ntk> ctn{ntk};

    ntk.foreach_gate(
        [&ntk, &ctn](const auto& n)
        {
            // skip already colored nodes
            if (ctn.color_ntk.color(n) == ctn.color_null)
            {
                const auto s = siblings(ntk, n);
                // if all siblings are either colored south or null, pick color east
                auto c = std::all_of(s.cbegin(), s.cend(),
                                     [&ctn](const auto& sn)
                                     {
                                         const auto snc = ctn.color_ntk.color(sn);
                                         return snc == ctn.color_south || snc == ctn.color_null;
                                     }) ?
                             ctn.color_east :
                             // if all siblings are either colored east or null, pick color south
                             std::all_of(s.cbegin(), s.cend(),
                                         [&ctn](const auto& sn)
                                         {
                                             const auto snc = ctn.color_ntk.color(sn);
                                             return snc == ctn.color_east || snc == ctn.color_null;
                                         }) ?
                             ctn.color_south :
                             // else, pick color null
                             ctn.color_null;

                // paint n with color c
                ctn.color_ntk.paint(n, c);
            }
        });

        debug::write_dot_network<mockturtle::out_of_place_color_view<Ntk>,
                             color_view_drawer<mockturtle::out_of_place_color_view<Ntk>>>(ctn.color_ntk, "colored_ntk");

    return ctn;
}

template <typename Ntk>
bool is_east_south_colored(const Ntk& ntk) noexcept
{
    bool is_properly_colored = true;
    ntk.foreach_node(
        [&ntk, &is_properly_colored](const auto& n)
        {
            if (!ntk.is_constant(n))
            {
                uint32_t c = 0ul;
                ntk.foreach_fanout(n,
                                   [&ntk, &is_properly_colored, &c](const auto& f, auto i)
                                   {
                                       // store color of first fanout
                                       if (i == 0)
                                       {
                                           c = ntk.color(ntk.get_node(f));
                                       }
                                       // compare color of first fanout against all others
                                       else if (c == ntk.color(ntk.get_node(f)))
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

template <typename Lyt, typename Ntk>
typename Lyt::aspect_ratio determine_layout_size(const coloring_container<Ntk>& ctn) noexcept
{
    uint64_t x = 1ull, y = ctn.color_ntk.num_pis() - 1;
    ctn.color_ntk.foreach_gate(
        [&ctn, &x, &y](const auto& g)
        {
            if (const auto c = ctn.color_ntk.color(g); c == ctn.color_east)
                ++x;
            else if (c == ctn.color_south)
                ++y;
            else if (c == ctn.color_null)
            {
                ++x;
                ++y;
            }
        });

    return {x, y, 1};
}

template <typename Ntk>
std::vector<mockturtle::node<Ntk>> fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    std::vector<mockturtle::node<Ntk>> fs{};

    ntk.foreach_fanin(n,
                      [&ntk, &fs](const auto& f)
                      {
                          if (const auto fn = ntk.get_node(f); !ntk.is_constant(fn))
                              fs.push_back(fn);
                      });

    return fs;
}

/**
 * Place 1-input gates.
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param ntk
 * @param n
 * @param a
 * @param t
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const Ntk& ntk, const mockturtle::node<Ntk>& n, const typename Lyt::signal& a,
                              const typename Lyt::tile& t)
{
    if (ntk.is_inv(n))
    {
        return lyt.create_not(a, t);
    }
    else if constexpr (fiction::has_is_buf_v<Ntk>)
    {
        if (ntk.is_buf(n))
        {
            return lyt.create_buf(a, t);
        }
    }
    else if constexpr (fiction::has_is_wire_v<Ntk>)
    {
        if (ntk.is_wire(n))
        {
            return lyt.create_buf(a, t);
        }
    }
    // more gate types go here
}

/**
 * Place 2-input gates.
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param ntk
 * @param n
 * @param a
 * @param b
 * @param t
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const Ntk& ntk, const mockturtle::node<Ntk>& n, const typename Lyt::signal& a,
                              const typename Lyt::signal& b, const typename Lyt::tile& t)
{
    if (ntk.is_and(n))
    {
        return lyt.create_and(a, b, t);
    }
    else if (ntk.is_or(n))
    {
        return lyt.create_or(a, b, t);
    }
    else if constexpr (mockturtle::has_is_xor_v<Ntk>)
    {
        if (ntk.is_xor(n))
        {
            return lyt.create_xor(a, b, t);
        }
    }
    // more gate types go here
}

template <typename Lyt>
mockturtle::signal<Lyt> wire_east(Lyt& lyt, const typename Lyt::tile& src, const typename Lyt::tile& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto x = src.x + 1; x < dest.x; ++x)
    {
        auto t = typename Lyt::tile{x, src.y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt>
mockturtle::signal<Lyt> wire_south(Lyt& lyt, const typename Lyt::tile& src, const typename Lyt::tile& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto y = src.y + 1; y < dest.y; ++y)
    {
        auto t = typename Lyt::tile{src.x, y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> connect_and_place(Lyt& lyt, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                                          typename Lyt::tile pre1_t, typename Lyt::tile pre2_t,
                                          const typename Lyt::tile& t)
{
    // make sure pre1_t is the northwards tile and pre2_t is the westwards one
    if (pre2_t < pre1_t)
        std::swap(pre1_t, pre2_t);

    return place(lyt, ntk, n, wire_south(lyt, pre1_t, t), wire_east(lyt, pre2_t, t), t);
}

template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> connect_and_place(Lyt& lyt, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                                          const typename Lyt::tile& pre_t, const typename Lyt::tile& t)
{
    if (lyt.is_westwards_of(t, pre_t))
    {
        return place(lyt, ntk, n, wire_east(lyt, pre_t, t), t);
    }
    else if (lyt.is_northwards_of(t, pre_t))
    {
        return place(lyt, ntk, n, wire_south(lyt, pre_t, t), t);
    }
    else
    {
        // gates cannot be placed elsewhere
        assert(false);
    }
}

template <typename Lyt, typename Ntk>
class orthogonal_impl
{
  public:
    orthogonal_impl(const Ntk& src, const orthogonal_physical_design_params& p) :
            ntk{mockturtle::fanout_view{fanout_substitution<topology_network>(src)}},
            ps{p},
            node2pos{ntk}
    {}

    Lyt run()
    {
        auto ctn = east_south_coloring(ntk);

        Lyt layout{determine_layout_size<Lyt>(ctn),
                   ps.number_of_clock_phases == 3 ? twoddwave_3_clocking : twoddwave_4_clocking};

        // reserve PI nodes without positions
        ntk.foreach_pi([this, &layout]([[maybe_unused]] const auto& pi) { layout.create_pi(); });

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        typename Lyt::tile latest_pos{1, 0};

        ntk.foreach_node(
            [this, &layout, &ctn, &latest_pos](const auto& n)
            {
                // do not place constants
                if (!ntk.is_constant(n))
                {
                    // if node is a PI, move it to its correct position
                    if (ntk.is_pi(n))
                    {
                        node2pos[n] = layout.move_node(n, {0, latest_pos.y});
                        ++latest_pos.y;
                    }
                    // if n has only one fanin
                    else if (const auto fs = fanins(ntk, n); fs.size() == 1)
                    {
                        const auto& pre = fs[0];

                        const auto pre_t = static_cast<typename Lyt::tile>(node2pos[pre]);

                        // n is colored east
                        if (const auto c = ctn.color_ntk.color(n); c == ctn.color_east)
                        {
                            const typename Lyt::tile t{latest_pos.x, pre_t.y};
                            node2pos[n] = connect_and_place(layout, ntk, n, pre_t, t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (c == ctn.color_south)
                        {
                            const typename Lyt::tile t{pre_t.x, latest_pos.y};
                            node2pos[n] = connect_and_place(layout, ntk, n, pre_t, t);
                            ++latest_pos.y;
                        }
                        else
                        {
                            // single fanin nodes should not be colored null
                            assert(false);
                        }
                    }
                    // if node has two fanins
                    else if (fs.size() == 2)
                    {
                        const auto &pre1 = fs[0], pre2 = fs[1];

                        auto pre1_t = static_cast<typename Lyt::tile>(node2pos[pre1]),
                             pre2_t = static_cast<typename Lyt::tile>(node2pos[pre2]);

                        typename Lyt::tile t{};

                        // n is colored east
                        if (const auto c = ctn.color_ntk.color(n); c == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.y < pre1_t.y)
                                std::swap(pre1_t, pre2_t);

                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = static_cast<typename Lyt::tile>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (c == ctn.color_south)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.x > pre1_t.x)
                                std::swap(pre1_t, pre2_t);

                            // use larger x position of predecessors
                            t = {pre1_t.x, latest_pos.y};

                            // each 2-input gate has one incoming bent wire
                            pre2_t = static_cast<typename Lyt::tile>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.y;
                        }
                        // n is colored null; corner case
                        else
                        {
                            t = latest_pos;

                            // ordering of pre1_t and pre2_t does not matter

                            // both wires have one bent
                            pre1_t = static_cast<typename Lyt::tile>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
                            pre2_t = static_cast<typename Lyt::tile>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.x;
                            ++latest_pos.y;
                        }

                        node2pos[n] = connect_and_place(layout, ntk, n, pre1_t, pre2_t, t);

                        // if n is a PO node, reserve a tile for later PO insertion
                        if (ntk.is_po(n))
                        {
                            ++latest_pos.x;
                        }
                    }
                }
            });

        // attach primary outputs
        ntk.foreach_po(
            [this, &layout](const auto& po)
            {
                const auto l_po = node2pos[ntk.get_node(po)];
                layout.create_po(l_po, "", layout.east(static_cast<typename Lyt::tile>(l_po)));
            });

        // restore possibly set signal names
        restore_names(ntk, layout, node2pos);

        return layout;
    }

  private:
    mockturtle::topo_view<mockturtle::fanout_view<topology_network>> ntk;

    orthogonal_physical_design_params ps;

    mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ntk)> node2pos;
};

}  // namespace detail

/**
 * A heuristic physical design approach based on orthogonal graph drawing. A slight modification of
 * Therese C. Biedl's improved algorithm for drawing of 3-graphs is used because the original
 * one works for undirected graphs only. Modification includes using directions of the logic network
 * directly instead of relabeling the edges according to its DFS tree, ordering the vertices
 * using jDFS instead of DFS, and adding an extra placement rule for nodes without predecessors.
 *
 * The algorithm works in linear time O(2|V| + |E|). Produced layout has a size of x * y, where
 * x + y = |V| - |PI| + 1. This is because each vertex leads to either one extra row or column
 * except for those without predecessors which create both.
 *
 * This is a proof of concept implementation for a scalable physical design approach for FCN.
 * It is not meant to be used for arranging fabricable circuits, as area is far from being optimal.
 */
template <typename Lyt, typename Ntk>
Lyt orthogonal(const Ntk& ntk, orthogonal_physical_design_params ps = {})
{
    static_assert(mockturtle::is_network_type_v<Lyt>, "Lyt is not a network type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    detail::orthogonal_impl<Lyt, Ntk> p{ntk, ps};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_ORTHOGONAL_HPP
