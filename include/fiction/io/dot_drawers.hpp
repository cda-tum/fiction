//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_DOT_DRAWERS_HPP
#define FICTION_DOT_DRAWERS_HPP

#include "../traits.hpp"

#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <array>

namespace fiction
{

template <typename Ntk, bool DrawIndexes = false, bool DrawHexTT = false>
class topology_dot_drawer : public mockturtle::gate_dot_drawer<Ntk>
{
  public:
    std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if constexpr (DrawIndexes)
        {
            if (!ntk.is_constant(n) && !ntk.is_pi(n))
                return fmt::format("{}: {}", ntk.node_to_index(n), node_label_callback(ntk, n));
        }

        return node_label_callback(ntk, n);
    }

    std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if (ntk.is_pi(n))
        {
            return "snow2";
        }
        if constexpr (has_is_fanout_v<Ntk>)
        {
            if (ntk.is_fanout(n))
            {
                return "navajowhite2";
            }
        }
        if constexpr (has_is_buf_v<Ntk>)
        {
            if (ntk.is_buf(n))
            {
                return "palegoldenrod";
            }
        }
        if constexpr (has_is_inv_v<Ntk>)
        {
            if (ntk.is_inv(n))
            {
                return "paleturquoise";
            }
        }
        if constexpr (has_is_nand_v<Ntk>)
        {
            if (ntk.is_nand(n))
            {
                return "lightcoral";
            }
        }
        if constexpr (has_is_nor_v<Ntk>)
        {
            if (ntk.is_nor(n))
            {
                return "palegreen2";
            }
        }
        if constexpr (has_is_dot_v<Ntk>)
        {
            if (ntk.is_dot(n))
            {
                return "thistle";
            }
        }

        return mockturtle::gate_dot_drawer<Ntk>::node_fillcolor(ntk, n);
    }

  private:
    std::string node_label_callback(const Ntk& ntk, const mockturtle::node<Ntk>& n) const
    {
        if constexpr (has_is_fanout_v<Ntk>)
        {
            if (ntk.is_fanout(n))
            {
                return "F";
            }
        }
        if constexpr (has_is_buf_v<Ntk>)
        {
            if (ntk.is_buf(n))
            {
                return "BUF";
            }
        }
        if constexpr (has_is_inv_v<Ntk>)
        {
            if (ntk.is_inv(n))
            {
                return "INV";
            }
        }
        if constexpr (has_is_nand_v<Ntk>)
        {
            if (ntk.is_nand(n))
            {
                return "NAND";
            }
        }
        if constexpr (has_is_nor_v<Ntk>)
        {
            if (ntk.is_nor(n))
            {
                return "NOR";
            }
        }
        if constexpr (has_is_dot_v<Ntk>)
        {
            if (ntk.is_dot(n))
            {
                return "DOT";
            }
        }

        const auto label = mockturtle::gate_dot_drawer<Ntk>::node_label(ntk, n);

        // check if base drawer could not identify the gate either
        if (is_node_number(label))
        {
            // try to fetch the node's truth table
            if constexpr (mockturtle::has_node_function_v<Ntk>)
            {
                const auto node_tt = ntk.node_function(n);

                // print truth tables with up to 4 variables exclusively
                if (node_tt.num_vars() <= 4)
                {
                    if constexpr (DrawHexTT)
                    {
                        return kitty::to_hex(node_tt);
                    }
                    else
                    {
                        return kitty::to_binary(node_tt);
                    }
                }
            }
        }

        return label;
    }

    [[nodiscard]] bool is_node_number(const std::string& s) const noexcept
    {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }
};

template <typename Ntk, bool DrawIndexes = false>
class color_view_drawer : public mockturtle::default_dot_drawer<Ntk>
{
  public:
    std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if constexpr (DrawIndexes)
        {
            return fmt::format("n: {}, c: {}", ntk.node_to_index(n), ntk.color(n));
        }
        else
        {
            return fmt::format("c: {}", ntk.color(n));
        }
    }

    std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        auto c = ntk.color(n);
        return c >= colors.size() ? "black" : colors[ntk.color(n)];
    }

  private:
    const std::array<std::string, 8> colors{{"ghostwhite", "deepskyblue1", "darkseagreen2", "crimson", "goldenrod1",
                                             "darkorchid2", "chocolate1", "gray28"}};
};

/*! \brief Writes layout in DOT format into output stream
 *
 * An overloaded variant exists that writes the layout into a file.
 *
 * **Required network functions:**
 * - is_constant
 * - is_pi
 * - foreach_node
 * - foreach_fanin
 * - foreach_po
 *
 * \param lyt Layout
 * \param os Output stream
 */
template <class Lyt, class Drawer = mockturtle::default_dot_drawer<Lyt>>
void write_grid_dot(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {})
{
    static_assert(mockturtle::is_network_type_v<Lyt>, "Ntk is not a network type");
    static_assert(mockturtle::has_is_constant_v<Lyt>, "Ntk does not implement the is_constant method");
    static_assert(mockturtle::has_is_pi_v<Lyt>, "Ntk does not implement the is_pi method");
    static_assert(mockturtle::has_foreach_node_v<Lyt>, "Ntk does not implement the foreach_node method");
    static_assert(mockturtle::has_foreach_fanin_v<Lyt>, "Ntk does not implement the foreach_fanin method");
    static_assert(mockturtle::has_foreach_po_v<Lyt>, "Ntk does not implement the foreach_po method");

    std::stringstream nodes{}, edges{}, levels{}, grid{};

//    std::vector<std::vector<uint32_t>> level_to_node_indexes;

//    lyt.foreach_node(
//        [&](auto const& n)
//        {
//            nodes << fmt::format("{} [label=\"{}\",shape={},style=filled,fillcolor={}]\n", lyt.node_to_index(n),
//                                 drawer.node_label(lyt, n), drawer.node_shape(lyt, n), drawer.node_fillcolor(lyt, n));
//            if (!lyt.is_constant(n) && !lyt.is_pi(n))
//            {
//                lyt.foreach_fanin(n,
//                                  [&](auto const& f)
//                                  {
//                                      if (!drawer.draw_signal(lyt, n, f))
//                                          return true;
//                                      edges << fmt::format("{} -> {} [style={}]\n", lyt.node_to_index(lyt.get_node(f)),
//                                                           lyt.node_to_index(n), drawer.signal_style(lyt, f));
//                                      return true;
//                                  });
//            }
//
//            const auto lvl = drawer.node_level(lyt, n);
//            if (level_to_node_indexes.size() <= lvl)
//            {
//                level_to_node_indexes.resize(lvl + 1);
//            }
//            level_to_node_indexes[lvl].push_back(lyt.node_to_index(n));
//        });
//
//    for (auto const& indexes : level_to_node_indexes)
//    {
//        levels << "{rank = same; ";
//        std::copy(indexes.begin(), indexes.end(), std::ostream_iterator<uint32_t>(levels, "; "));
//        levels << "}\n";
//    }
//
//    levels << "{rank = same; ";
//    lyt.foreach_po(
//        [&](auto const& f, auto i)
//        {
//            nodes << fmt::format("po{} [shape={},style=filled,fillcolor={}]\n", i, drawer.po_shape(lyt, i),
//                                 drawer.po_fillcolor(lyt, i));
//            edges << fmt::format("{} -> po{} [style={}]\n", lyt.node_to_index(lyt.get_node(f)), i,
//                                 drawer.signal_style(lyt, f));
//            levels << fmt::format("po{}; ", i);
//        });
//    levels << "}\n";

    // enforce grid structure
    grid << "edge [weight=1000, style=invis];\n";
    for (auto x = 0ul; x < lyt.x(); ++x)
    {
        std::vector<std::string> column{};
        for (auto y = 0ul; y < lyt.y(); ++y)
        {
            column.emplace_back(fmt::format("{}{}", x, y));
        }

        grid << fmt::format("{};\n", fmt::join(column, " -> "));
    }

    for (auto y = 0ul; y < lyt.y(); ++y)
    {
        std::vector<std::string> row{};
        for (auto x = 0ul; x < lyt.x(); ++x) { row.emplace_back(fmt::format("{}{}", x, y)); }

        grid << fmt::format("rank = same {{ {} }};\n", fmt::join(row, " -> "));
    }

    os << "digraph {\n"
       << "rankdir=TB;\n"
       << nodes.str() << edges.str() << levels.str() << grid.str() << "}\n";
}

/*! \brief Writes layout in DOT format into a file
 *
 * **Required network functions:**
 * - is_constant
 * - is_pi
 * - foreach_node
 * - foreach_fanin
 * - foreach_po
 *
 * \param lyt Layout
 * \param filename Filename
 */
template <class Lyt, class Drawer = mockturtle::default_dot_drawer<Lyt>>
void write_grid_dot(const Lyt& lyt, const std::string& filename, const Drawer& drawer = {})
{
    std::ofstream os{filename.c_str(), std::ofstream::out};
    write_dot(lyt, os, drawer);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_DOT_DRAWERS_HPP
