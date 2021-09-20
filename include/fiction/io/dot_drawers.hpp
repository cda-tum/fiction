//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_DOT_DRAWERS_HPP
#define FICTION_DOT_DRAWERS_HPP

#include "../traits.hpp"
#include "utils/version_info.hpp"

#include <fiction/layouts/hexagonal_layout.hpp>

#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <array>

namespace fiction
{

template <typename Ntk, bool DrawIndexes = false, bool DrawHexTT = false>
class technology_dot_drawer : public mockturtle::gate_dot_drawer<Ntk>
{
  public:
    [[nodiscard]] std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if constexpr (DrawIndexes)
        {
            if (!ntk.is_constant(n) && !ntk.is_pi(n))
                return fmt::format("{}: {}", ntk.node_to_index(n), node_label_callback(ntk, n));
        }

        return node_label_callback(ntk, n);
    }

    [[nodiscard]] std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
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
    [[nodiscard]] std::string node_label_callback(const Ntk& ntk, const mockturtle::node<Ntk>& n) const
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
        if (is_node_number(label) && !ntk.is_pi(n) && !ntk.is_constant(n))
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
    [[nodiscard]] std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
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

    [[nodiscard]] std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        auto c = ntk.color(n);
        return c >= colors.size() ? "black" : colors[ntk.color(n)];
    }

  private:
    const std::array<std::string, 8> colors{{"ghostwhite", "deepskyblue1", "darkseagreen2", "crimson", "goldenrod1",
                                             "darkorchid2", "chocolate1", "gray28"}};
};

template <typename Lyt, bool DrawIndexes = false>
class simple_gate_layout_tile_drawer : public technology_dot_drawer<Lyt, DrawIndexes>
{
  public:
    [[nodiscard]] virtual std::vector<std::string> additional_graph_attributes() const noexcept
    {
        // 'concentrate' merges edges, so it would be great to have since the layout topology in dot relies on invisible
        // edges, which, however, still consume area as other edges are routed around them to avoid collisions. In
        // theory, 'concentrate' would prevent that from happening. Unfortunately, when merging edges, it is possible
        // for the invisible edge to become dominant and to shadow the visible edge...
        if constexpr (DrawIndexes)
        {
            return {{{"splines=ortho"}, {"nodesep=0.5"} /*, {concentrate=true} */}};
        }
        else
        {
            return {{{"splines=ortho"}, {"nodesep=0.25"} /*, {concentrate=true} */}};
        }
    }

    [[nodiscard]] virtual std::string tile_id(const tile<Lyt>& t) const noexcept
    {
        return fmt::format("x{}y{}", t.x, t.y);
    }

    [[nodiscard]] virtual std::vector<std::string> additional_node_attributes() const noexcept
    {
        // all nodes are of fixed size and are pinned to precalculated positions (neato engine only)
        if constexpr (DrawIndexes)
        {
            return {{{"fixedsize=true"}, {"pin=true"}, {"width=1"}, {"height=1"}}};
        }
        else
        {
            return {{{"fixedsize=true"}, {"pin=true"}, {"width=0.5"}, {"height=0.5"}}};
        }
    }

    [[nodiscard]] virtual std::string tile_label(const Lyt& lyt, const tile<Lyt>& t) const noexcept
    {
        if (lyt.is_empty_tile(t))
            return "";

        if (lyt.is_pi_tile(t))
            return "PI";

        if (lyt.is_po_tile(t))
            return "PO";

        if constexpr (has_is_buf_v<Lyt>)
        {
            // crossing case
            if (lyt.is_buf(lyt.get_node(t)) && lyt.is_buf(lyt.get_node(lyt.above(t))))
            {
                return "+";
            }
        }

        return technology_dot_drawer<Lyt, DrawIndexes>::node_label(lyt, lyt.get_node(t));
    }

    [[nodiscard]] virtual std::string tile_fillcolor(const Lyt& lyt, const tile<Lyt>& t) const noexcept
    {
        if (lyt.is_empty_tile(t))
            return "white";

        if (lyt.is_pi_tile(t) || lyt.is_po_tile(t))
            return "snow2";

        return technology_dot_drawer<Lyt, DrawIndexes>::node_fillcolor(lyt, lyt.get_node(t));
    }

  protected:
    [[nodiscard]] std::vector<std::vector<std::string>> rows(const Lyt& lyt) const noexcept
    {
        std::vector<std::vector<std::string>> rows{};

        for (auto y = 0ul; y <= lyt.y(); ++y)
        {
            std::vector<std::string> row{};
            for (auto x = 0ul; x <= lyt.x(); ++x) { row.emplace_back(tile_id({x, y})); }
            rows.push_back(row);
        }

        return rows;
    }

    [[nodiscard]] std::vector<std::vector<std::string>> columns(const Lyt& lyt) const noexcept
    {
        std::vector<std::vector<std::string>> columns{};

        for (auto x = 0ul; x <= lyt.x(); ++x)
        {
            std::vector<std::string> col{};
            for (auto y = 0ul; y <= lyt.y(); ++y) { col.emplace_back(tile_id({x, y})); }
            columns.push_back(col);
        }

        return columns;
    }

    [[nodiscard]] std::string same_rank(const std::vector<std::string>& rank) const noexcept
    {
        return fmt::format("rank = same {{ {} }};\n", fmt::join(rank, " -> "));
    }

    [[nodiscard]] std::string edge(const std::string& src, const std::string& tgt) const noexcept
    {
        return fmt::format("{} -> {};\n", src, tgt);
    }
};

template <typename Lyt, bool DrawIndexes = false>
class gate_layout_cartesian_drawer : public simple_gate_layout_tile_drawer<Lyt, DrawIndexes>
{
  public:
    [[nodiscard]] virtual std::vector<std::string> additional_graph_attributes() const noexcept
    {
        auto graph_attributes = simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::additional_graph_attributes();

        if constexpr (DrawIndexes)
        {
            graph_attributes.emplace_back("ranksep=0.5");
        }
        else
        {
            graph_attributes.emplace_back("ranksep=0.25");
        }

        graph_attributes.emplace_back("rankdir=TB");

        return graph_attributes;
    }

    [[nodiscard]] virtual std::vector<std::string> additional_node_attributes() const noexcept
    {
        auto node_attributes = simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::additional_node_attributes();

        node_attributes.emplace_back("shape=square");

        return node_attributes;
    }

    //    std::string tile_position(const tile<Lyt>& t) const noexcept
    //    {
    //        return fmt::format("({},{})", t.x, t.y);
    //    }

    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const noexcept
    {
        std::stringstream topology{};

        topology << "edge [constraint=true, style=invis];\n";

        const auto enforce_same_cardinal_column = [this, &lyt, &topology]()
        {
            for (const auto& col : simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::columns(lyt))
            {
                topology << fmt::format("{};\n", fmt::join(col, " -> "));
            }
        };

        const auto enforce_same_cardinal_row = [this, &lyt, &topology]()
        {
            for (const auto& row : simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::rows(lyt))
            {
                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::same_rank(row);
            }
        };

        enforce_same_cardinal_column();
        enforce_same_cardinal_row();

        return topology.str();
    }
};

template <typename Lyt, bool DrawIndexes = false>
class gate_layout_hexagonal_drawer : public simple_gate_layout_tile_drawer<Lyt, DrawIndexes>
{
  public:
    [[nodiscard]] virtual std::vector<std::string> additional_graph_attributes() const noexcept
    {
        auto graph_attributes = simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::additional_graph_attributes();

        // hexagon visuals benefit from halved rank separation because they are interlaced
        if constexpr (DrawIndexes)
        {
            graph_attributes.emplace_back("ranksep=0.25");
        }
        else
        {
            graph_attributes.emplace_back("ranksep=0.125");
        }

        // pointy top hexagons are modeled as top-down graphs
        if constexpr (std::is_same_v<typename Lyt::hex_arrangement::orientation, pointy_top>)
        {
            graph_attributes.emplace_back("rankdir=TB");
        }
        // while flat top hexagons are modeled as left-right graphs
        else
        {
            graph_attributes.emplace_back("rankdir=LR");
        }

        return graph_attributes;
    }

    [[nodiscard]] virtual std::vector<std::string> additional_node_attributes() const noexcept
    {
        auto node_attributes = simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::additional_node_attributes();

        node_attributes.emplace_back("shape=hexagon");

        if constexpr (std::is_same_v<typename Lyt::hex_arrangement::orientation, pointy_top>)
        {
            // pointy top hexagons are rotated by 30°
            node_attributes.emplace_back("orientation=30");
        }

        return node_attributes;
    }

    //    std::string tile_position(const tile<Lyt>& t) const noexcept
    //    {
    //        return fmt::format("({},{})", t.x, t.y);
    //    }

    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const
    {
        std::stringstream topology{};

        topology << "edge [constraint=true, style=invis];\n";

        const auto enforce_same_hexagonal_column = [this, &lyt, &topology]()
        {
            for (const auto& col : simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::columns(lyt))
            {
                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::same_rank(col);
            }
        };

        const auto enforce_same_hexagonal_row = [this, &lyt, &topology]()
        {
            for (const auto& row : simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::rows(lyt))
            {
                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::same_rank(row);
            }
        };

        const auto r = [](const auto i) -> std::string { return fmt::format("r{}", i); };

        const auto shift_column = [this, &lyt, &topology, &r](const auto i)
        {
            topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::same_rank(
                {{r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id({i, 0})}});

            // previous column only exist if i != 0
            if (i != 0)
            {
                const tile<Lyt> previous_column{i - 1, 0};

                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::edge(
                    r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(previous_column));
            }

            // next column could be out of bounds and need to be checked for
            if (const tile<Lyt> next_column{i + 1, 0}; lyt.x() >= next_column.x)
            {
                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::edge(
                    r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(next_column));
            }
        };

        const auto shift_row = [this, &lyt, &topology, &r](const auto i)
        {
            topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::same_rank(
                {{r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id({0, i})}});

            // previous row only exist if i != 0
            if (i != 0)
            {
                const tile<Lyt> previous_row{0, i - 1};

                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::edge(
                    r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(previous_row));
            }

            // next row could be out of bounds and need to be checked for
            if (const tile<Lyt> next_row{0, i + 1}; lyt.y() >= next_row.y)
            {
                topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::edge(
                    r(i), simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(next_row));
            }
        };

        // add invisible nodes to shift rows/columns
        if constexpr (DrawIndexes)
        {
            topology << "node [label=\"\", width=1, height=1, style=invis];\n";
        }
        else
        {
            topology << "node [label=\"\", width=0.5, height=0.5, style=invis];\n";
        }

        if constexpr (std::is_same_v<typename Lyt::hex_arrangement, odd_row>)
        {
            enforce_same_hexagonal_row();

            // shift odd rows
            for (auto i = 1ul; i <= lyt.y(); i += 2) { shift_row(i); }
        }
        else if constexpr (std::is_same_v<typename Lyt::hex_arrangement, even_row>)
        {
            enforce_same_hexagonal_row();

            // shift even rows
            for (auto i = 0ul; i <= lyt.y(); i += 2) { shift_row(i); }
        }
        else if constexpr (std::is_same_v<typename Lyt::hex_arrangement, odd_column>)
        {
            enforce_same_hexagonal_column();

            // shift odd columns
            for (auto i = 1ul; i <= lyt.x(); i += 2) { shift_column(i); }
        }
        else if constexpr (std::is_same_v<typename Lyt::hex_arrangement, even_column>)
        {
            enforce_same_hexagonal_column();

            // shift even columns
            for (auto i = 0ul; i <= lyt.x(); i += 2) { shift_column(i); }
        }

        // enforce connections other than those in direct row/column via edges
        lyt.foreach_ground_tile(
            [this, &lyt, &topology](const auto& t)
            {
                lyt.foreach_adjacent_tile(
                    t,
                    [this, &topology, &t](const auto& at)
                    {
                        // skip adjacent tiles in one direction to prevent double edges
                        if (t >= at)
                        {
                            return true;
                        }

                        if constexpr (std::is_same_v<typename Lyt::hex_arrangement::orientation, pointy_top>)
                        {
                            // skip adjacent tiles in same row to prevent double edges
                            if (t.y == at.y)
                            {
                                return true;
                            }
                        }
                        else  // flat_top
                        {
                            // skip adjacent tiles in same column to prevent double edges
                            if (t.x == at.x)
                            {
                                return true;
                            }
                        }

                        topology << simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::edge(
                            simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(t),
                            simple_gate_layout_tile_drawer<Lyt, DrawIndexes>::tile_id(at));

                        return true;
                    });
            });

        return topology.str();
    }
};

/*! \brief Writes layout in DOT format into output stream
 *
 * An overloaded variant exists that writes the layout into a file.
 *
 * **Required network functions:**
 * - is_pi
 * - foreach_node
 * - foreach_fanin
 *
 * \param lyt Layout
 * \param os Output stream
 */
template <class Lyt, class Drawer>
void write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {})
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::has_is_pi_v<Lyt>, "Lyt does not implement the is_pi function");
    static_assert(mockturtle::has_foreach_node_v<Lyt>, "Lyt does not implement the foreach_node function");
    static_assert(mockturtle::has_foreach_fanin_v<Lyt>, "Lyt does not implement the foreach_fanin function");

    std::stringstream nodes{}, edges{}, topology{};

    auto node_attributes = drawer.additional_node_attributes();
    node_attributes.emplace_back("style=filled");

    nodes << fmt::format("node [{}];\n", fmt::join(node_attributes, ", "));

    // draw tiles
    lyt.foreach_ground_tile(
        [&lyt, &drawer, &nodes](const auto& t)
        {
            nodes << fmt::format("{} [label=\"{}\", fillcolor={}];\n", drawer.tile_id(t), drawer.tile_label(lyt, t),
                                 drawer.tile_fillcolor(lyt, t));
        });

    edges << "edge [constraint=false];\n";

    // draw connections
    lyt.foreach_node(
        [&lyt, &drawer, &edges](const auto& n)
        {
            lyt.foreach_fanin(n,
                              [&lyt, &drawer, &edges, &n](const auto& f)
                              {
                                  edges << fmt::format("{} -> {} [style={}];\n",
                                                       drawer.tile_id(static_cast<tile<Lyt>>(f)),
                                                       drawer.tile_id(lyt.get_tile(n)), drawer.signal_style(lyt, f));
                              });
        });

    // enforce topological structure
    topology << drawer.enforce_topology(lyt);

    // draw layout
    os << fmt::format("digraph layout {{  // Generated by {} ({})\n{};\n\n", FICTION_VERSION, FICTION_REPO,
                      fmt::join(drawer.additional_graph_attributes(), ";\n"))
       << nodes.rdbuf() << '\n'
       << edges.rdbuf() << '\n'
       << topology.rdbuf() << "}\n";
}

/*! \brief Writes layout in DOT format into a file
 *
 * **Required network functions:**
 * - is_pi
 * - foreach_node
 * - foreach_fanin
 *
 * \param lyt Layout
 * \param filename Filename
 */
template <class Lyt, class Drawer = gate_layout_cartesian_drawer<Lyt>>
void write_dot_layout(const Lyt& lyt, const std::string& filename, const Drawer& drawer = {})
{
    std::ofstream os{filename.c_str(), std::ofstream::out};
    write_dot(lyt, os, drawer);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_DOT_DRAWERS_HPP
