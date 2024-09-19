//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_DOT_DRAWERS_HPP
#define FICTION_DOT_DRAWERS_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/network_utils.hpp"
#include "utils/version_info.hpp"

#include <fiction/layouts/hexagonal_layout.hpp>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <kitty/print.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace fiction
{

/**
 * A DOT drawer that extends mockturtle's one by several additional gate types.
 *
 * @tparam Ntk Logic network type.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 * @tparam DrawHexTT Flag to toggle the representation of truth tables for unknown functions in hexadecimal notation.
 */
template <typename Ntk, bool DrawIndexes = false, bool DrawHexTT = false>
class technology_dot_drawer : public mockturtle::gate_dot_drawer<Ntk>
{
  public:
    [[nodiscard]] std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if constexpr (DrawIndexes)
        {
            if (!ntk.is_constant(n) && !ntk.is_pi(n))
            {
                return fmt::format("{}: {}", ntk.node_to_index(n), node_label_callback(ntk, n));
            }
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
        if constexpr (has_is_xnor_v<Ntk>)
        {
            if (ntk.is_xnor(n))
            {
                return "lightskyblue";
            }
        }
        if constexpr (has_is_lt_v<Ntk>)
        {
            if (ntk.is_lt(n))
            {
                return "seagreen1";
            }
        }
        if constexpr (has_is_le_v<Ntk>)
        {
            if (ntk.is_le(n))
            {
                return "seagreen4";
            }
        }
        if constexpr (has_is_gt_v<Ntk>)
        {
            if (ntk.is_gt(n))
            {
                return "firebrick1";
            }
        }
        if constexpr (has_is_ge_v<Ntk>)
        {
            if (ntk.is_ge(n))
            {
                return "firebrick4";
            }
        }
        if constexpr (has_is_dot_v<Ntk>)
        {
            if (ntk.is_dot(n))
            {
                return "thistle";
            }
        }
        if constexpr (has_is_xor_and_v<Ntk>)
        {
            if (ntk.is_xor_and(n))
            {
                return "lightpink";
            }
        }
        if constexpr (has_is_or_and_v<Ntk>)
        {
            if (ntk.is_or_and(n))
            {
                return "lightgreen";
            }
        }
        if constexpr (has_is_onehot_v<Ntk>)
        {
            if (ntk.is_onehot(n))
            {
                return "lightgoldenrod";
            }
        }
        if constexpr (has_is_gamble_v<Ntk>)
        {
            if (ntk.is_gamble(n))
            {
                return "lightsteelblue";
            }
        }
        if constexpr (mockturtle::has_is_ite_v<Ntk>)
        {
            if (ntk.is_ite(n))
            {
                return "lightcyan";
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
        if constexpr (has_is_xnor_v<Ntk>)
        {
            if (ntk.is_xnor(n))
            {
                return "XNOR";
            }
        }
        if constexpr (has_is_lt_v<Ntk>)
        {
            if (ntk.is_lt(n))
            {
                return "LT";
            }
        }
        if constexpr (has_is_le_v<Ntk>)
        {
            if (ntk.is_le(n))
            {
                return "LE";
            }
        }
        if constexpr (has_is_gt_v<Ntk>)
        {
            if (ntk.is_gt(n))
            {
                return "GT";
            }
        }
        if constexpr (has_is_ge_v<Ntk>)
        {
            if (ntk.is_ge(n))
            {
                return "GE";
            }
        }
        if constexpr (has_is_dot_v<Ntk>)
        {
            if (ntk.is_dot(n))
            {
                return "DOT";
            }
        }
        if constexpr (has_is_xor_and_v<Ntk>)
        {
            if (ntk.is_xor_and(n))
            {
                return "XOR_AND";
            }
        }
        if constexpr (has_is_or_and_v<Ntk>)
        {
            if (ntk.is_or_and(n))
            {
                return "OR_AND";
            }
        }
        if constexpr (has_is_onehot_v<Ntk>)
        {
            if (ntk.is_onehot(n))
            {
                return "ONEHOT";
            }
        }
        if constexpr (has_is_gamble_v<Ntk>)
        {
            if (ntk.is_gamble(n))
            {
                return "GAMBLE";
            }
        }
        if constexpr (mockturtle::has_is_ite_v<Ntk>)
        {
            if (ntk.is_ite(n))
            {
                return "ITE";
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

    [[nodiscard]] bool is_node_number(const std::string_view& s) const noexcept
    {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }
};
/**
 * A DOT drawer for networks with colored nodes. Node colors represent their painted color instead of their gate type.
 *
 * @tparam Ntk Logic network type.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Ntk, bool DrawIndexes = false>
class color_view_drawer : public mockturtle::default_dot_drawer<Ntk>
{
  public:
    [[nodiscard]] std::string node_label(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        if constexpr (DrawIndexes)
        {
            return fmt::format("n: {}, c: {}", ntk.node_to_index(n), ntk.edge_color(n));
        }
        else
        {
            return fmt::format("c: {}", ntk.edge_color(n));
        }
    }

    [[nodiscard]] std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        const auto c = ntk.edge_color(n);

        static constexpr const char* undef_color = "black, fontcolor=white";

        return c < COLORS.size() ? COLORS[c] : undef_color;
    }

  protected:
    static constexpr const std::array<const char*, 8> COLORS{{"ghostwhite", "deepskyblue1", "darkseagreen2", "crimson",
                                                              "goldenrod1", "darkorchid2", "chocolate1", "gray28"}};
};
/**
 * A DOT drawer for networks with colored edges. Node colors represent their painted color instead of
 * their gate type.
 *
 * @tparam Ntk Logic network type.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Ntk, bool DrawIndexes = false>
class edge_color_view_drawer : public color_view_drawer<Ntk, DrawIndexes>
{
  public:
    /**
     * Override function to store the previously accessed node such that edges can be colored when signal_style is
     * called without specific information on the target node. This is a little bit hacky and depends on the way
     * mockturtle's dot drawer works.
     */
    [[nodiscard]] std::string node_fillcolor(const Ntk& ntk, const mockturtle::node<Ntk>& n) const override
    {
        last_accessed = n;

        return base_drawer::node_fillcolor(ntk, n);
    }

    [[nodiscard]] std::string signal_style(Ntk const& ntk, mockturtle::signal<Ntk> const& f) const override
    {
        const auto c = ntk.edge_color(mockturtle::edge<Ntk>{ntk.get_node(f), last_accessed});

        static constexpr const char* undef_color = "black";

        const auto color_str = (c == 0 || c >= base_drawer::colors.size()) ? undef_color : base_drawer::colors[c];

        return fmt::format("{}, color=\"{}\"", base_drawer::signal_style(ntk, f), color_str);
    }

  private:
    mutable mockturtle::node<Ntk> last_accessed{};

    using base_drawer = color_view_drawer<Ntk, DrawIndexes>;
};
/**
 * Base class for a simple gate-level layout DOT drawer.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
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
        if constexpr (DrawIndexes)
        {
            return {{{"fixedsize=true"}, {"width=1"}, {"height=1"}}};
        }
        else
        {
            return {{{"fixedsize=true"}, {"width=0.5"}, {"height=0.5"}}};
        }
    }

    [[nodiscard]] virtual std::string tile_label(const Lyt& lyt, const tile<Lyt>& t) const noexcept
    {
        if (lyt.is_empty_tile(t))
        {
            return "";
        }

        if (lyt.is_pi_tile(t) || lyt.is_po_tile(t))
        {
            if constexpr (mockturtle::has_get_name_v<Lyt> && mockturtle::has_has_name_v<Lyt>)
            {
                if (const auto n = lyt.get_node(t); lyt.has_name(n))
                {
                    return lyt.get_name(n);
                }
            }
            else
            {
                if (lyt.is_pi_tile(t))
                {
                    return "PI";
                }

                if (lyt.is_po_tile(t))
                {
                    return "PO";
                }
            }
        }

        if constexpr (has_is_buf_v<Lyt>)
        {
            // crossing case
            if (const auto at = lyt.above(t);
                (t != at) && (lyt.is_buf(lyt.get_node(t)) && lyt.is_buf(lyt.get_node(at))))
            {
                return "+";
            }
        }

        return technology_dot_drawer<Lyt, DrawIndexes>::node_label(lyt, lyt.get_node(t));
    }

    [[nodiscard]] virtual std::string tile_fillcolor(const Lyt& lyt, const tile<Lyt>& t) const noexcept
    {
        if constexpr (ClockColors)
        {
            static constexpr const std::array<const char*, 4> clk_colors{
                {"gray94, fontcolor=black", "gray67, fontcolor=black", "gray44, fontcolor=white",
                 "gray17, fontcolor=white"}};
            static constexpr const char* undef_color = "black, fontcolor=white";

            const auto clk_number = lyt.get_clock_number(t);

            return clk_number < clk_colors.size() ? clk_colors[clk_number] : undef_color;
        }
        else
        {
            if (lyt.is_empty_tile(t))
            {
                return "white";
            }

            if (lyt.is_pi_tile(t) || lyt.is_po_tile(t))
            {
                return "snow2";
            }

            return technology_dot_drawer<Lyt, DrawIndexes>::node_fillcolor(lyt, lyt.get_node(t));
        }
    }

  protected:
    [[nodiscard]] std::vector<std::vector<std::string>> rows(const Lyt& lyt) const noexcept
    {
        std::vector<std::vector<std::string>> rows{};
        rows.reserve(lyt.y() + 1);

        for (auto y = 0ul; y <= lyt.y(); ++y)
        {
            std::vector<std::string> row{};
            row.reserve(lyt.x() + 1);

            for (auto x = 0ul; x <= lyt.x(); ++x)
            {
                row.emplace_back(tile_id({x, y}));
            }

            rows.push_back(row);
        }

        return rows;
    }

    [[nodiscard]] std::vector<std::vector<std::string>> columns(const Lyt& lyt) const noexcept
    {
        std::vector<std::vector<std::string>> columns{};
        columns.reserve(lyt.x() + 1);

        for (auto x = 0ul; x <= lyt.x(); ++x)
        {
            std::vector<std::string> col{};
            col.reserve(lyt.y() + 1);

            for (auto y = 0ul; y <= lyt.y(); ++y)
            {
                col.emplace_back(tile_id({x, y}));
            }

            columns.push_back(col);
        }

        return columns;
    }

    [[nodiscard]] std::string same_rank(const std::vector<std::string>& rank) const noexcept
    {
        return fmt::format("rank = same {{ {} }};\n", fmt::join(rank, " -> "));
    }

    [[nodiscard]] std::string edge(const std::string_view& src, const std::string_view& tgt) const noexcept
    {
        return fmt::format("{} -> {};\n", src, tgt);
    }
};
/**
 * An extended gate-level layout DOT drawer for Cartesian layouts.
 *
 * @tparam Lyt Cartesian gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class gate_layout_cartesian_drawer : public simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    [[nodiscard]] std::vector<std::string> additional_graph_attributes() const noexcept override
    {
        auto graph_attributes = base_drawer::additional_graph_attributes();

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

    [[nodiscard]] std::vector<std::string> additional_node_attributes() const noexcept override
    {
        auto node_attributes = base_drawer::additional_node_attributes();

        node_attributes.emplace_back("shape=square");

        return node_attributes;
    }

    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const noexcept
    {
        std::stringstream topology{};

        topology << "edge [constraint=true, style=invis];\n";

        const auto enforce_same_cardinal_column = [this, &lyt, &topology]()
        {
            for (const auto& col : base_drawer::columns(lyt))
            {
                topology << fmt::format("{};\n", fmt::join(col, " -> "));
            }
        };

        const auto enforce_same_cardinal_row = [this, &lyt, &topology]()
        {
            for (const auto& row : base_drawer::rows(lyt))
            {
                topology << base_drawer::same_rank(row);
            }
        };

        enforce_same_cardinal_column();
        enforce_same_cardinal_row();

        return topology.str();
    }

  private:
    using base_drawer = simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>;
};
/**
 * An extended gate-level layout DOT drawer for shifted Cartesian layouts.
 *
 * @tparam Lyt Shifted Cartesian gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class gate_layout_shifted_cartesian_drawer : public simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    [[nodiscard]] std::vector<std::string> additional_graph_attributes() const noexcept override
    {
        auto graph_attributes = base_drawer::additional_graph_attributes();

        if constexpr (DrawIndexes)
        {
            graph_attributes.emplace_back("ranksep=0.5");
        }
        else
        {
            graph_attributes.emplace_back("ranksep=0.25");
        }

        // horizontal shifts are modeled as top-down graphs
        if constexpr (has_horizontally_shifted_cartesian_orientation_v<Lyt>)
        {
            graph_attributes.emplace_back("rankdir=TB");
        }
        // while vertical shifts are modeled as left-right graphs
        else
        {
            graph_attributes.emplace_back("rankdir=LR");
        }

        return graph_attributes;
    }

    [[nodiscard]] std::vector<std::string> additional_node_attributes() const noexcept override
    {
        auto node_attributes = base_drawer::additional_node_attributes();

        node_attributes.emplace_back("shape=square");

        return node_attributes;
    }

    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const
    {
        std::stringstream topology{};

        topology << "edge [constraint=true, style=invis];\n";

        // add invisible nodes to shift rows/columns
        if constexpr (DrawIndexes)
        {
            topology << "node [label=\"\", width=1, height=1, style=invis];\n";
        }
        else
        {
            topology << "node [label=\"\", width=0.5, height=0.5, style=invis];\n";
        }

        if constexpr (has_odd_row_cartesian_arrangement_v<Lyt>)
        {
            enforce_same_shifted_row(lyt, topology);

            // shift odd rows
            for (auto i = 1ul; i <= lyt.y(); i += 2)
            {
                shift_row(lyt, i, topology);
            }
        }
        else if constexpr (has_even_row_cartesian_arrangement_v<Lyt>)
        {
            enforce_same_shifted_row(lyt, topology);

            // shift even rows
            for (auto i = 0ul; i <= lyt.y(); i += 2)
            {
                shift_row(lyt, i, topology);
            }
        }
        else if constexpr (has_odd_column_cartesian_arrangement_v<Lyt>)
        {
            enforce_same_shifted_column(lyt, topology);

            // shift odd columns
            for (auto i = 1ul; i <= lyt.x(); i += 2)
            {
                shift_column(lyt, i, topology);
            }
        }
        else if constexpr (has_even_column_cartesian_arrangement_v<Lyt>)
        {
            enforce_same_shifted_column(lyt, topology);

            // shift even columns
            for (auto i = 0ul; i <= lyt.x(); i += 2)
            {
                shift_column(lyt, i, topology);
            }
        }

        // enforce connections other than those in direct row/column via edges
        lyt.foreach_ground_tile(
            [this, &lyt, &topology](const auto& t)
            {
                lyt.foreach_adjacent_tile(t,
                                          [this, &topology, &t](const auto& at)
                                          {
                                              // skip adjacent tiles in one direction to prevent double edges
                                              if (t >= at)
                                              {
                                                  return true;
                                              }

                                              if constexpr (has_horizontally_shifted_cartesian_orientation_v<Lyt>)
                                              {
                                                  // skip adjacent tiles in same row to prevent double edges
                                                  if (t.y == at.y)
                                                  {
                                                      return true;
                                                  }
                                              }
                                              else  // vertically shifted
                                              {
                                                  // skip adjacent tiles in same column to prevent double edges
                                                  if (t.x == at.x)
                                                  {
                                                      return true;
                                                  }
                                              }

                                              topology << base_drawer::edge(base_drawer::tile_id(t),
                                                                            base_drawer::tile_id(at));

                                              return true;
                                          });
            });

        return topology.str();
    }

  protected:
    using base_drawer = simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>;

    [[nodiscard]] std::string invisible_node(const uint64_t i) const noexcept
    {
        return fmt::format("invis{}", i);
    }

    void enforce_same_shifted_column(const Lyt& lyt, std::stringstream& stream) const noexcept
    {
        for (const auto& col : base_drawer::columns(lyt))
        {
            stream << base_drawer::same_rank(col);
        }
    }

    void enforce_same_shifted_row(const Lyt& lyt, std::stringstream& stream) const noexcept
    {
        for (const auto& row : base_drawer::rows(lyt))
        {
            stream << base_drawer::same_rank(row);
        }
    }

    void shift_column(const Lyt& lyt, const uint64_t col, std::stringstream& stream) const noexcept
    {
        stream << base_drawer::same_rank({{invisible_node(col), base_drawer::tile_id({col, 0})}});

        // previous column only exist if i != 0
        if (col != 0)
        {
            const tile<Lyt> previous_column_tile{col - 1, 0};

            stream << base_drawer::edge(invisible_node(col), base_drawer::tile_id(previous_column_tile));
        }

        // next column could be out of bounds and need to be checked for
        if (const tile<Lyt> next_column_tile{col + 1, 0}; lyt.x() >= next_column_tile.x)
        {
            stream << base_drawer::edge(invisible_node(col), base_drawer::tile_id(next_column_tile));
        }
    }

    void shift_row(const Lyt& lyt, const uint64_t row, std::stringstream& stream) const noexcept
    {
        stream << base_drawer::same_rank({{invisible_node(row), base_drawer::tile_id({0, row})}});

        // previous row only exist if i != 0
        if (row != 0)
        {
            const tile<Lyt> previous_row_tile{0, row - 1};

            stream << base_drawer::edge(invisible_node(row), base_drawer::tile_id(previous_row_tile));
        }

        // next row could be out of bounds and need to be checked for
        if (const tile<Lyt> next_row_tile{0, row + 1}; lyt.y() >= next_row_tile.y)
        {
            stream << base_drawer::edge(invisible_node(row), base_drawer::tile_id(next_row_tile));
        }
    }
};
/**
 * An extended gate-level layout DOT drawer for hexagonal layouts.
 *
 * @tparam Lyt Hexagonal gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class gate_layout_hexagonal_drawer : public simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    [[nodiscard]] std::vector<std::string> additional_graph_attributes() const noexcept override
    {
        auto graph_attributes = base_drawer::additional_graph_attributes();

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
        if constexpr (has_pointy_top_hex_orientation_v<Lyt>)
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

    [[nodiscard]] std::vector<std::string> additional_node_attributes() const noexcept override
    {
        auto node_attributes = base_drawer::additional_node_attributes();

        node_attributes.emplace_back("shape=hexagon");

        if constexpr (has_pointy_top_hex_orientation_v<Lyt>)
        {
            // pointy top hexagons are rotated by 30°
            node_attributes.emplace_back("orientation=30");
        }

        return node_attributes;
    }

    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const
    {
        std::stringstream topology{};

        topology << "edge [constraint=true, style=invis];\n";

        // add invisible nodes to shift rows/columns
        if constexpr (DrawIndexes)
        {
            topology << "node [label=\"\", width=1, height=1, style=invis];\n";
        }
        else
        {
            topology << "node [label=\"\", width=0.5, height=0.5, style=invis];\n";
        }

        if constexpr (has_odd_row_hex_arrangement_v<Lyt>)
        {
            enforce_same_hexagonal_row(lyt, topology);

            // shift odd rows
            for (auto i = 1ul; i <= lyt.y(); i += 2)
            {
                shift_row(lyt, i, topology);
            }
        }
        else if constexpr (has_even_row_hex_arrangement_v<Lyt>)
        {
            enforce_same_hexagonal_row(lyt, topology);

            // shift even rows
            for (auto i = 0ul; i <= lyt.y(); i += 2)
            {
                shift_row(lyt, i, topology);
            }
        }
        else if constexpr (has_odd_column_hex_arrangement_v<Lyt>)
        {
            enforce_same_hexagonal_column(lyt, topology);

            // shift odd columns
            for (auto i = 1ul; i <= lyt.x(); i += 2)
            {
                shift_column(lyt, i, topology);
            }
        }
        else if constexpr (has_even_column_hex_arrangement_v<Lyt>)
        {
            enforce_same_hexagonal_column(lyt, topology);

            // shift even columns
            for (auto i = 0ul; i <= lyt.x(); i += 2)
            {
                shift_column(lyt, i, topology);
            }
        }

        // enforce connections other than those in direct row/column via edges
        lyt.foreach_ground_tile(
            [this, &lyt, &topology](const auto& t)
            {
                lyt.foreach_adjacent_tile(t,
                                          [this, &topology, &t](const auto& at)
                                          {
                                              // skip adjacent tiles in one direction to prevent double edges
                                              if (t >= at)
                                              {
                                                  return true;
                                              }

                                              if constexpr (has_pointy_top_hex_orientation_v<Lyt>)
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

                                              topology << base_drawer::edge(base_drawer::tile_id(t),
                                                                            base_drawer::tile_id(at));

                                              return true;
                                          });
            });

        return topology.str();
    }

  protected:
    using base_drawer = simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>;

    [[nodiscard]] std::string invisible_node(const uint64_t i) const noexcept
    {
        return fmt::format("invis{}", i);
    }

    void enforce_same_hexagonal_column(const Lyt& lyt, std::stringstream& stream) const noexcept
    {
        for (const auto& col : base_drawer::columns(lyt))
        {
            stream << base_drawer::same_rank(col);
        }
    }

    void enforce_same_hexagonal_row(const Lyt& lyt, std::stringstream& stream) const noexcept
    {
        for (const auto& row : base_drawer::rows(lyt))
        {
            stream << base_drawer::same_rank(row);
        }
    }

    void shift_column(const Lyt& lyt, const uint64_t col, std::stringstream& stream) const noexcept
    {
        stream << base_drawer::same_rank({{invisible_node(col), base_drawer::tile_id({col, 0})}});

        // previous column only exist if i != 0
        if (col != 0)
        {
            const tile<Lyt> previous_column_tile{col - 1, 0};

            stream << base_drawer::edge(invisible_node(col), base_drawer::tile_id(previous_column_tile));
        }

        // next column could be out of bounds and need to be checked for
        if (const tile<Lyt> next_column_tile{col + 1, 0}; lyt.x() >= next_column_tile.x)
        {
            stream << base_drawer::edge(invisible_node(col), base_drawer::tile_id(next_column_tile));
        }
    }

    void shift_row(const Lyt& lyt, const uint64_t row, std::stringstream& stream) const noexcept
    {
        stream << base_drawer::same_rank({{invisible_node(row), base_drawer::tile_id({0, row})}});

        // previous row only exist if i != 0
        if (row != 0)
        {
            const tile<Lyt> previous_row_tile{0, row - 1};

            stream << base_drawer::edge(invisible_node(row), base_drawer::tile_id(previous_row_tile));
        }

        // next row could be out of bounds and need to be checked for
        if (const tile<Lyt> next_row_tile{0, row + 1}; lyt.y() >= next_row_tile.y)
        {
            stream << base_drawer::edge(invisible_node(row), base_drawer::tile_id(next_row_tile));
        }
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
template <class Lyt, class Drawer>
void write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_dot_layout(lyt, os, drawer);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_DOT_DRAWERS_HPP
