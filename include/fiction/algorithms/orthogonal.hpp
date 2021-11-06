//
// Created by marcel on 13.07.17.
//

#ifndef FICTION_ORTHOGONAL_HPP
#define FICTION_ORTHOGONAL_HPP

#include "../io/print_layout.hpp"
#include "../layouts/clocking_scheme.hpp"
#include "../traits.hpp"
#include "../utils/debug/network_writer.hpp"
#include "fanout_substitution.hpp"
#include "layout_utils.hpp"
#include "name_restoration.hpp"
#include "network_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/color_view.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <optional>
#include <set>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

struct orthogonal_physical_design_params
{
    /**
     * Number of clock phases to use. 3 and 4 are supported.
     */
    num_clks number_of_clock_phases = num_clks::FOUR;
};

struct orthogonal_physical_design_stats
{
    mockturtle::stopwatch<>::duration time_total{0};

    uint64_t x_size{0ull}, y_size{0ull};
    uint64_t num_gates{0ull}, num_wires{0ull};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] layout size = {} × {}\n", x_size, y_size);
        out << fmt::format("[i] num. gates  = {}\n", num_gates);
        out << fmt::format("[i] num. wires  = {}\n", num_wires);
    }
};

namespace detail
{

template <typename Ntk>
bool has_po_fanout(const Ntk& ntk, const mockturtle::node<Ntk> n) noexcept
{
    bool result = false;

    ntk.foreach_fanout(n,
                       [&ntk, &n, &result](const auto& fo)
                       {
                           if (ntk.is_po(fo))
                           {
                               result = true;
                           }

                           return !result;
                       });

    return result;
}

/**
 * Determine siblings of the given node. A sibling is a node that shares the same fan-in with n.
 * @param n Node to consider.
 * @return Siblings of n.
 */
template <typename Ntk>
std::vector<mockturtle::node<Ntk>> siblings(const Ntk& ntk, const mockturtle::node<Ntk> n) noexcept
{
    std::vector<mockturtle::node<Ntk>> sibs{};
    ntk.foreach_fanin(n,
                      [&ntk, &sibs, &n](const auto& fi)
                      {
                          // skip constants
                          if (const auto fin = ntk.get_node(fi); !ntk.is_constant(fin))
                          {
                              ntk.foreach_fanout(fin,
                                                 [&ntk, &sibs, &n](const auto& fo)
                                                 {
                                                     // do not consider constants or n itself
                                                     if (const auto fon = ntk.get_node(fo);
                                                         !ntk.is_constant(fon) && (fon != n))
                                                     {
                                                         sibs.push_back(fon);
                                                     }
                                                 });
                          }
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

template <typename Ntk>
bool different_colored_fanins(const coloring_container<Ntk>& ctn, const mockturtle::node<Ntk>& n) noexcept
{
    const auto fc = fanins(ctn.color_ntk, n);

    if (const auto fanin_size = fc.fanin_nodes.size(); fanin_size == 0 || fanin_size == 1)
    {
        return false;
    }
    else if (fanin_size == 2)
    {
        const auto fnc1 = ctn.color_ntk.color(fc.fanin_nodes[0]), fnc2 = ctn.color_ntk.color(fc.fanin_nodes[1]);

        return (fnc1 != ctn.color_null && fnc2 != ctn.color_null) && fnc1 != fnc2;
    }

    return false;
}

/**
 * Nodes colored east have signals incoming from eastern direction. Nodes colored south have signals incoming from
 * southern direction.
 * @tparam Ntk
 * @param ntk
 * @return
 */
template <typename Ntk>
coloring_container<Ntk> east_south_coloring(const Ntk& ntk) noexcept
{
    coloring_container<Ntk> ctn{ntk};

#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.num_gates()),
                                 "[i] determining relative positions: |{0}|"};
#endif

    ctn.color_ntk.foreach_gate(
        [&](const auto& n, [[maybe_unused]] const auto i)
        {
            // skip already colored nodes
            if (ctn.color_ntk.color(n) == ctn.color_null)
            {
                const auto s = siblings(ctn.color_ntk, n);

                const auto clr =
                    // if node has fanins of different color, pick color null
                    different_colored_fanins(ctn, n) ?
                        ctn.color_null :
                        // if all siblings are either colored south or null, pick color east
                        std::all_of(s.cbegin(), s.cend(),
                                    [&ctn](const auto& sn)
                                    {
                                        const auto snc = ctn.color_ntk.color(sn);
                                        return snc == ctn.color_south || snc == ctn.color_null;
                                    }) ?
                        // unless a node has an additional PO fanout,then pick color south instead
                        ctn.color_ntk.is_po(ctn.color_ntk.make_signal(n)) && ctn.color_ntk.fanout_size(n) > 1 ?
                        ctn.color_south :
                        ctn.color_east :
                        // if all siblings are either colored east or null, pick color south
                        std::all_of(s.cbegin(), s.cend(),
                                    [&ctn](const auto& sn)
                                    {
                                        const auto snclr = ctn.color_ntk.color(sn);
                                        return snclr == ctn.color_east || snclr == ctn.color_null;
                                    }) ?
                        ctn.color_south :
                            // else, pick color null
                            ctn.color_null;

                // paint n with color c
                ctn.color_ntk.paint(n, clr);
            }

#if (PROGRESS_BARS)
            // update progress
            bar(i);
#endif
        });

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
                uint32_t clr = 0ul;
                ntk.foreach_fanout(n,
                                   [&ntk, &is_properly_colored, &clr](const auto& f, const auto i)
                                   {
                                       // store color of first fanout
                                       if (i == 0)
                                       {
                                           clr = ntk.color(ntk.get_node(f));
                                       }
                                       // compare color of first fanout against all others
                                       else if (clr == ntk.color(ntk.get_node(f)))
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

template <typename Ntk>
uint32_t is_eastern_po_orientation_available(const coloring_container<Ntk>& ctn,
                                             const mockturtle::node<Ntk>&   n) noexcept
{
    bool eastern_side_available = true;

    // PO nodes can have a maximum of one other fanout
    ctn.color_ntk.foreach_fanout(n,
                                 [&ctn, &eastern_side_available](const auto& fo)
                                 {
                                     // if that fanout is colored east, no PO can be placed there
                                     if (ctn.color_ntk.color(ctn.color_ntk.get_node(fo)) == ctn.color_east)
                                     {
                                         eastern_side_available = false;
                                     }
                                     return false;
                                 });

    return eastern_side_available;
}

template <typename Lyt, typename Ntk>
aspect_ratio<Lyt> determine_layout_size(const coloring_container<Ntk>& ctn) noexcept
{
#if (PROGRESS_BARS)
    // initialize a progress bar
    mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] determining layout size: |{0}|"};
#endif

    uint64_t x = 0ull, y = ctn.color_ntk.num_pis() - 1;
    ctn.color_ntk.foreach_node(
        [&](const auto& n, [[maybe_unused]] const auto i)
        {
            if (!ctn.color_ntk.is_constant(n))
            {
                if (ctn.color_ntk.is_pi(n))
                {
                    ctn.color_ntk.foreach_fanout(n,
                                                 [&ctn, &x](const auto& fo)
                                                 {
                                                     if (ctn.color_ntk.color(ctn.color_ntk.get_node(fo)) ==
                                                         ctn.color_south)
                                                         ++x;
                                                 });
                }
                else if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                    ++x;
                else if (clr == ctn.color_south)
                    ++y;
                else if (clr == ctn.color_null)
                {
                    ++x;
                    ++y;
                }

                if (ctn.color_ntk.is_po(n))
                {
                    if (is_eastern_po_orientation_available(ctn, n))
                        ++x;
                    else
                        ++y;
                }
            }

#if (PROGRESS_BARS)
            // update progress
            bar(i);
#endif
        });

    return {x, y, 1};
}

template <typename Lyt>
mockturtle::signal<Lyt> wire_east(Lyt& lyt, const tile<Lyt>& src, const tile<Lyt>& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto x = src.x + 1; x < dest.x; ++x)
    {
        auto t = tile<Lyt>{x, src.y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt>
mockturtle::signal<Lyt> wire_south(Lyt& lyt, const tile<Lyt>& src, const tile<Lyt>& dest)
{
    auto a = static_cast<mockturtle::signal<Lyt>>(src);

    for (auto y = src.y + 1; y < dest.y; ++y)
    {
        auto t = tile<Lyt>{src.x, y, 0};
        if (!lyt.is_empty_tile(t))  // crossing case
        {
            t = lyt.above(t);
        }

        a = lyt.create_buf(a, t);
    }

    return a;
}

template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> connect_and_place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                                          tile<Lyt> pre1_t, tile<Lyt> pre2_t,
                                          const std::optional<bool>& c = std::nullopt)
{
    // make sure pre1_t is the northwards tile and pre2_t is the westwards one
    if (pre2_t < pre1_t)
        std::swap(pre1_t, pre2_t);

    return place(lyt, t, ntk, n, wire_south(lyt, pre1_t, t), wire_east(lyt, pre2_t, t), c);
}

template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> connect_and_place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                                          const tile<Lyt>& pre_t)
{
    if (lyt.is_westwards_of(t, pre_t))
    {
        return place(lyt, t, ntk, n, wire_east(lyt, pre_t, t));
    }
    else if (lyt.is_northwards_of(t, pre_t))
    {
        return place(lyt, t, ntk, n, wire_south(lyt, pre_t, t));
    }

    assert(false);  // gates cannot be placed elsewhere
    return {};      // fix -Wreturn-type warning
}

template <typename Lyt, typename Ntk>
class orthogonal_impl
{
  public:
    orthogonal_impl(const Ntk& src, const orthogonal_physical_design_params& p, orthogonal_physical_design_stats& st) :
            ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<topology_network>>(src)}},
            ps{p},
            pst{st}
    {}

    Lyt run()
    {
        // measure run time
        mockturtle::stopwatch stop{pst.time_total};
        // compute a coloring
        const auto ctn = east_south_coloring(ntk);

        mockturtle::node_map<mockturtle::signal<Lyt>, decltype(ctn.color_ntk)> node2pos{ctn.color_ntk};

        // instantiate the layout
        Lyt layout{determine_layout_size<Lyt>(ctn), twoddwave_clocking<Lyt>(ps.number_of_clock_phases)};

        // reserve PI nodes without positions
        ctn.color_ntk.foreach_pi(
            [this, &layout, &ctn](const auto& pi)
            {
                const auto s = ctn.color_ntk.make_signal(pi);
                layout.create_pi(ctn.color_ntk.has_name(s) ? ctn.color_ntk.get_name(s) : "");
            });

        // first x-pos to use for gates is 1 because PIs take up the 0th column
        tile<Lyt> latest_pos{1, 0};

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ctn.color_ntk.size()), "[i] arranging layout: |{0}|"};
#endif

        ctn.color_ntk.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] const auto i)
            {
                // do not place constants
                if (!ctn.color_ntk.is_constant(n))
                {
                    // if node is a PI, move it to its correct position
                    if (ctn.color_ntk.is_pi(n))
                    {
                        node2pos[n] = layout.move_node(
                            static_cast<mockturtle::node<Lyt>>(n),
                            {0, latest_pos.y});  // this casts a network node to a layout node. This only works because
                                                 // topology_network and gate_level_layout use the same number of
                                                 // constants followed by PIs

                        // resolve conflicting PIs
                        ctn.color_ntk.foreach_fanout(
                            n,
                            [this, &ctn, &n, &layout, &node2pos, &latest_pos](const auto& fo)
                            {
                                if (ctn.color_ntk.color(ctn.color_ntk.get_node(fo)) == ctn.color_south)
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
                    else if (const auto fc = fanins(ctn.color_ntk, n); fc.fanin_nodes.size() == 1)
                    {
                        const auto& pre = fc.fanin_nodes[0];

                        const auto pre_t = static_cast<tile<Lyt>>(node2pos[pre]);

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            const tile<Lyt> t{latest_pos.x, pre_t.y};
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            const tile<Lyt> t{pre_t.x, latest_pos.y};
                            node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre_t);
                            ++latest_pos.y;
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

                        // n is colored east
                        if (const auto clr = ctn.color_ntk.color(n); clr == ctn.color_east)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.y < pre1_t.y)
                                std::swap(pre1_t, pre2_t);

                            // use larger y position of predecessors
                            t = {latest_pos.x, pre2_t.y};

                            // each 2-input gate has one incoming bent wire
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));

                            ++latest_pos.x;
                        }
                        // n is colored south
                        else if (clr == ctn.color_south)
                        {
                            // make sure pre1_t is the northwards tile and pre2_t is the westwards one
                            if (pre2_t.x > pre1_t.x)
                                std::swap(pre1_t, pre2_t);

                            // use larger x position of predecessors
                            t = {pre1_t.x, latest_pos.y};

                            // each 2-input gate has one incoming bent wire
                            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.y;
                        }
                        // n is colored null; corner case
                        else
                        {
                            // TODO if both directions are free, consider the siblings

                            // make sure pre1_t has an empty tile to its east and pre2_t to its south
                            if (!layout.is_empty_tile(layout.east(pre1_t)) ||
                                !layout.is_empty_tile(layout.south(pre2_t)))
                                std::swap(pre1_t, pre2_t);

                            t = latest_pos;

                            // both wires have one bent
                            pre1_t = static_cast<tile<Lyt>>(wire_east(layout, pre1_t, {t.x + 1, pre1_t.y}));
                            pre2_t = static_cast<tile<Lyt>>(wire_south(layout, pre2_t, {pre2_t.x, t.y + 1}));

                            ++latest_pos.x;
                            ++latest_pos.y;
                        }

                        node2pos[n] = connect_and_place(layout, t, ctn.color_ntk, n, pre1_t, pre2_t, fc.constant_fanin);
                    }

                    // create PO at applicable position
                    if (ctn.color_ntk.is_po(n))
                    {
                        const auto n_s = node2pos[n];

                        tile<Lyt> po_tile{};

                        // determine PO orientation
                        if (is_eastern_po_orientation_available(ctn, n))
                        {
                            po_tile = layout.east(static_cast<tile<Lyt>>(n_s));
                            ++latest_pos.x;
                        }
                        else
                        {
                            po_tile = layout.south(static_cast<tile<Lyt>>(n_s));
                            ++latest_pos.y;
                        }

                        // check if PO position is located at the border
                        if (layout.is_eastern_border(po_tile))
                        {
                            layout.create_po(n_s,
                                             ctn.color_ntk.has_output_name(po_counter) ?
                                                 ctn.color_ntk.get_output_name(po_counter++) :
                                                 fmt::format("po{}", po_counter++),
                                             po_tile);
                        }
                        // place PO at the border and connect it by wire segments
                        else
                        {
                            const auto anker = layout.create_buf(n_s, po_tile);

                            po_tile = layout.eastern_border_of(po_tile);

                            layout.create_po(wire_east(layout, static_cast<tile<Lyt>>(anker), po_tile),
                                             ctn.color_ntk.has_output_name(po_counter) ?
                                                 ctn.color_ntk.get_output_name(po_counter++) :
                                                 fmt::format("po{}", po_counter++),
                                             po_tile);
                        }
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
            });

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
    mockturtle::topo_view<mockturtle::fanout_view<mockturtle::names_view<topology_network>>> ntk;

    orthogonal_physical_design_params ps;
    orthogonal_physical_design_stats& pst;

    uint32_t po_counter{0};
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
 *
 * May throw an 'high_degree_fanin_exception'.
 */
template <typename Lyt, typename Ntk>
Lyt orthogonal(const Ntk& ntk, orthogonal_physical_design_params ps = {},
               orthogonal_physical_design_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::is_network_type_v<Ntk>,
                  "Ntk is not a network type");  // Ntk is being converted to a topology_network anyways, therefore,
                                                 // this is the only relevant check here

    // check for input degree
    if (has_high_degree_fanin_nodes(ntk, 2))
    {
        throw high_degree_fanin_exception();
    }

    orthogonal_physical_design_stats  st{};
    detail::orthogonal_impl<Lyt, Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_ORTHOGONAL_HPP
