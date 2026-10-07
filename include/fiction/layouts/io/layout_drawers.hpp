/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief DOT drawers for gate-level layouts, specialized per grid topology.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/progress.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <kitty/bit_operations.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fiction::layouts::io
{

/**
 * Base class for a simple gate-level layout DOT drawer.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class simple_gate_layout_tile_drawer
{
  public:
    /** @brief Creates a stateless drawer. */
    simple_gate_layout_tile_drawer() = default;
    /** @brief Copies the stateless drawer. */
    simple_gate_layout_tile_drawer(const simple_gate_layout_tile_drawer&) = default;
    /** @brief Moves the stateless drawer. */
    simple_gate_layout_tile_drawer(simple_gate_layout_tile_drawer&&) noexcept = default;
    /** @brief Copies the stateless drawer. @return This drawer. */
    simple_gate_layout_tile_drawer& operator=(const simple_gate_layout_tile_drawer&) = default;
    /** @brief Moves the stateless drawer. @return This drawer. */
    simple_gate_layout_tile_drawer& operator=(simple_gate_layout_tile_drawer&&) noexcept = default;
    /** @brief Destroy the drawer. */
    virtual ~simple_gate_layout_tile_drawer() = default;
    /** @brief Return graph attributes. */
    [[nodiscard]] virtual std::vector<std::string> additional_graph_attributes(const Lyt& /*lyt*/) const
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

    /**
     * @brief Return a DOT identifier for the planar tile position.
     * @param t Tile coordinate.
     * @return Identifier with signed axes encoded as letters and digits.
     */
    [[nodiscard]] virtual std::string tile_id(const tile<Lyt>& t) const
    {
        auto id = fmt::format("x{}y{}", t.x, t.y);
        std::replace(id.begin(), id.end(), '-', 'n');
        return id;
    }

    /** @brief Return node attributes. */
    [[nodiscard]] virtual std::vector<std::string> additional_node_attributes(const Lyt& /*lyt*/) const
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

    /** @brief Return the gate label. */
    [[nodiscard]] virtual std::string tile_label(const Lyt& lyt, const tile<Lyt>& t) const
    {
        if (lyt.is_empty_tile(t))
        {
            return "";
        }

        const auto id = lyt.find_object(t).value();
        if (lyt.is_pi(id) || lyt.is_po(id))
        {
            if (lyt.has_name(id))
            {
                return lyt.get_name(id);
            }
            return lyt.is_pi(id) ? "PI" : "PO";
        }
        if (const auto above = lyt.above(t); above && lyt.is_wire(id) && lyt.is_wire_tile(*above))
        {
            return "+";
        }
        const auto label = gate_description(lyt, id).first;
        if constexpr (DrawIndexes)
        {
            return fmt::format("{}:{}: {}", id.index, id.generation, label);
        }
        return std::string{label};
    }

    /** @brief Return the tile color. */
    [[nodiscard]] virtual std::string tile_fillcolor(const Lyt& lyt, const tile<Lyt>& t) const
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

            return std::string{gate_description(lyt, lyt.find_object(t).value()).second};
        }
    }

  private:
    /**
     * @brief Return the label and fill color for a placed object.
     * @param lyt Layout containing the object.
     * @param id Object to describe.
     * @return Gate label and fill color.
     */
    [[nodiscard]] static std::pair<std::string_view, std::string_view>
    gate_description(const Lyt& lyt, const typename Lyt::object_id id)
    {
        if (lyt.is_fanout(id))
        {
            return {"F", "navajowhite2"};
        }
        if (lyt.is_buf(id))
        {
            return {"BUF", "palegoldenrod"};
        }
        if (lyt.is_inv(id))
        {
            return {"INV", "paleturquoise"};
        }
        if (lyt.is_and(id))
        {
            return {"AND", "lightcoral"};
        }
        if (lyt.is_nand(id))
        {
            return {"NAND", "lightcoral"};
        }
        if (lyt.is_or(id))
        {
            return {"OR", "palegreen2"};
        }
        if (lyt.is_nor(id))
        {
            return {"NOR", "palegreen2"};
        }
        if (lyt.is_xor(id))
        {
            return {"XOR", "lightskyblue"};
        }
        if (lyt.is_xnor(id))
        {
            return {"XNOR", "lightskyblue"};
        }
        if (lyt.is_lt(id))
        {
            return {"LT", "seagreen1"};
        }
        if (lyt.is_le(id))
        {
            return {"LE", "seagreen4"};
        }
        if (lyt.is_gt(id))
        {
            return {"GT", "firebrick1"};
        }
        if (lyt.is_ge(id))
        {
            return {"GE", "firebrick4"};
        }
        if (lyt.is_maj(id))
        {
            return {"MAJ", "lightsalmon"};
        }
        if (const auto function = lyt.node_function(id); function.num_vars() == 0)
        {
            return {kitty::get_bit(function, 0) ? "1" : "0", "white"};
        }
        return {"?", "white"};
    }

  protected:
    /** @brief List tile labels by row. */
    [[nodiscard]] std::vector<std::vector<std::string>> rows(const Lyt& lyt) const
    {
        std::vector<std::vector<std::string>> rows{};
        rows.reserve(lyt.height());

        for (int64_t y = 0; y < lyt.height(); ++y)
        {
            std::vector<std::string> row{};
            row.reserve(lyt.width());

            for (int64_t x = 0; x < lyt.width(); ++x)
            {
                row.emplace_back(tile_id({x, y}));
            }

            rows.push_back(row);
        }

        return rows;
    }

    /** @brief List tile labels by column. */
    [[nodiscard]] std::vector<std::vector<std::string>> columns(const Lyt& lyt) const
    {
        std::vector<std::vector<std::string>> columns{};
        columns.reserve(lyt.width());

        for (int64_t x = 0; x < lyt.width(); ++x)
        {
            std::vector<std::string> col{};
            col.reserve(lyt.height());

            for (int64_t y = 0; y < lyt.height(); ++y)
            {
                col.emplace_back(tile_id({x, y}));
            }

            columns.push_back(col);
        }

        return columns;
    }

    /** @brief Format a rank constraint. */
    [[nodiscard]] static std::string same_rank(const std::vector<std::string>& rank)
    {
        return fmt::format("rank = same {{ {} }};\n", fmt::join(rank, " -> "));
    }

    /** @brief Format an edge. */
    [[nodiscard]] static std::string edge(const std::string_view& src, const std::string_view& tgt)
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
    /** @brief Return graph attributes. */
    [[nodiscard]] std::vector<std::string> additional_graph_attributes(const Lyt& lyt) const override
    {
        auto graph_attributes = base_drawer::additional_graph_attributes(lyt);

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

    /** @brief Return node attributes. */
    [[nodiscard]] std::vector<std::string> additional_node_attributes(const Lyt& lyt) const override
    {
        auto node_attributes = base_drawer::additional_node_attributes(lyt);

        node_attributes.emplace_back("shape=square");

        return node_attributes;
    }

    /** @brief Format the grid topology. */
    [[nodiscard]] std::string enforce_topology(const Lyt& lyt) const
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
    /** @brief Define the drawer configuration. */
    using base_drawer = simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>;
};
namespace detail
{

/**
 * Base class of the gate-level layout DOT drawers for layouts with shifted rows or columns. It draws each row or
 * column in one rank and shifts every other one by an invisible node. The derived class chooses the rank separation and
 * the node shape.
 *
 * @tparam Lyt Gate-level layout type with shifted rows or columns.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors, bool DrawIndexes>
class gate_layout_shifted_tile_drawer : public simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    /** @brief Return graph attributes. */
    [[nodiscard]] std::vector<std::string> additional_graph_attributes(const Lyt& lyt) const override
    {
        auto graph_attributes = base_drawer::additional_graph_attributes(lyt);

        graph_attributes.emplace_back(fmt::format("ranksep={}", rank_separation()));
        // shifted rows are modeled as top-down graphs, shifted columns as left-right graphs
        graph_attributes.emplace_back(is_row_arrangement(lyt.get_arrangement()) ? "rankdir=TB" : "rankdir=LR");

        return graph_attributes;
    }

    /** @brief Format the grid topology. */
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

        const auto    a     = lyt.get_arrangement();
        const auto    rows  = is_row_arrangement(a);
        const int64_t first = is_odd_arrangement(a) ? 1 : 0;

        for (const auto& line : rows ? base_drawer::rows(lyt) : base_drawer::columns(lyt))
        {
            topology << base_drawer::same_rank(line);
        }

        // shift every other row or column
        for (auto i = first; i < (rows ? lyt.height() : lyt.width()); i += 2)
        {
            shift_line(lyt, i, rows, topology);
        }

        // enforce connections other than those in direct row/column via edges
        lyt.foreach_ground_tile(
            [this, &lyt, &topology, rows](const auto& t)
            {
                lyt.foreach_adjacent_tile(t,
                                          [this, &topology, &t, rows](const auto& at)
                                          {
                                              // skip adjacent tiles in one direction to prevent double edges
                                              if (t >= at)
                                              {
                                                  return true;
                                              }

                                              // skip adjacent tiles in the same row or column to prevent double edges
                                              if (rows ? t.y == at.y : t.x == at.x)
                                              {
                                                  return true;
                                              }

                                              topology << base_drawer::edge(base_drawer::tile_id(t),
                                                                            base_drawer::tile_id(at));

                                              return true;
                                          });
            });

        return topology.str();
    }

  protected:
    /**
     * The drawer of the tiles that this class extends.
     */
    using base_drawer = simple_gate_layout_tile_drawer<Lyt, ClockColors, DrawIndexes>;
    /**
     * Returns the DOT value of the `ranksep` graph attribute.
     *
     * @return Separation of the ranks.
     */
    [[nodiscard]] virtual std::string_view rank_separation() const = 0;

  private:
    /**
     * Returns the name of the invisible node that shifts a row or column.
     *
     * @param i Index of the row or column.
     * @return Node name.
     */
    [[nodiscard]] static std::string invisible_node(const int64_t i)
    {
        return fmt::format("invis{}", i);
    }
    /**
     * Shifts a row or column by placing an invisible node in its rank and connecting the node to the neighboring rows
     * or columns.
     *
     * @param lyt Layout to draw.
     * @param index Index of the row or column.
     * @param is_row Whether `index` names a row. Otherwise, it names a column.
     * @param stream Stream to write the DOT statements to.
     */
    void shift_line(const Lyt& lyt, const int64_t index, const bool is_row, std::stringstream& stream) const
    {
        const auto line_tile = [is_row](const int64_t i) { return is_row ? tile<Lyt>{0, i} : tile<Lyt>{i, 0}; };

        stream << base_drawer::same_rank(
            std::vector<std::string>{invisible_node(index), base_drawer::tile_id(line_tile(index))});

        // the previous row or column only exists if index != 0
        if (index != 0)
        {
            stream << base_drawer::edge(invisible_node(index), base_drawer::tile_id(line_tile(index - 1)));
        }

        // the next row or column could be out of bounds and needs to be checked for
        if (index + 1 < (is_row ? lyt.height() : lyt.width()))
        {
            stream << base_drawer::edge(invisible_node(index), base_drawer::tile_id(line_tile(index + 1)));
        }
    }
};

}  // namespace detail

/**
 * An extended gate-level layout DOT drawer for shifted Cartesian layouts.
 *
 * @tparam Lyt Shifted Cartesian gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class gate_layout_shifted_cartesian_drawer
        : public detail::gate_layout_shifted_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    /** @brief Return node attributes. */
    [[nodiscard]] std::vector<std::string> additional_node_attributes(const Lyt& lyt) const override
    {
        auto node_attributes = shifted_drawer::additional_node_attributes(lyt);

        node_attributes.emplace_back("shape=square");

        return node_attributes;
    }

  protected:
    /** @brief Define the drawer configuration. */
    [[nodiscard]] std::string_view rank_separation() const override
    {
        return DrawIndexes ? "0.5" : "0.25";
    }

  private:
    /** @brief Define the drawer configuration. */
    using shifted_drawer = detail::gate_layout_shifted_tile_drawer<Lyt, ClockColors, DrawIndexes>;
};
/**
 * An extended gate-level layout DOT drawer for hexagonal layouts.
 *
 * @tparam Lyt Hexagonal gate-level layout type.
 * @tparam ClockColors Flag to toggle the drawing of clock colors instead of gate type colors.
 * @tparam DrawIndexes Flag to toggle the drawing of node indices.
 */
template <typename Lyt, bool ClockColors = false, bool DrawIndexes = false>
class gate_layout_hexagonal_drawer : public detail::gate_layout_shifted_tile_drawer<Lyt, ClockColors, DrawIndexes>
{
  public:
    /** @brief Return node attributes. */
    [[nodiscard]] std::vector<std::string> additional_node_attributes(const Lyt& lyt) const override
    {
        auto node_attributes = shifted_drawer::additional_node_attributes(lyt);

        node_attributes.emplace_back("shape=hexagon");

        if (is_row_arrangement(lyt.get_arrangement()))
        {
            // pointy top hexagons are rotated by 30°
            node_attributes.emplace_back("orientation=30");
        }

        return node_attributes;
    }

  protected:
    // hexagon visuals benefit from halved rank separation because they are interlaced
    /** @brief Define the drawer configuration. */
    [[nodiscard]] std::string_view rank_separation() const override
    {
        return DrawIndexes ? "0.25" : "0.125";
    }

  private:
    /** @brief Define the drawer configuration. */
    using shifted_drawer = detail::gate_layout_shifted_tile_drawer<Lyt, ClockColors, DrawIndexes>;
};
/**
 * Writes a layout in DOT format into an output stream. Terminal names use quoted DOT strings.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Drawer DOT drawer type.
 * @param lyt Layout.
 * @param os Output stream.
 * @param drawer Formats the layout's tiles and topology.
 * @param on_progress Receives completed drawing work.
 */
template <class Lyt, class Drawer>
void write_dot_layout(const Lyt& lyt, std::ostream& os, const Drawer& drawer = {},
                      utils::progress_callback on_progress = {})
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    std::stringstream nodes{}, edges{}, topology{};

    auto node_attributes = drawer.additional_node_attributes(lyt);
    node_attributes.emplace_back("style=filled");

    nodes << fmt::format("node [{}];\n", fmt::join(node_attributes, ", "));

    utils::progress_reporter tiles_progress{std::move(on_progress), "drawing tiles", lyt.area()};
    // draw tiles
    lyt.foreach_ground_tile(
        [&lyt, &drawer, &nodes, &tiles_progress](const auto& t)
        {
            nodes << drawer.tile_id(t) << " [label=" << std::quoted(drawer.tile_label(lyt, t))
                  << fmt::format(", fillcolor={}];\n", drawer.tile_fillcolor(lyt, t));
            tiles_progress.advance();
        });

    edges << "edge [constraint=false];\n";

    // draw connections
    lyt.foreach_node(
        [&lyt, &drawer, &edges](const auto& n)
        {
            lyt.foreach_fanin(n,
                              [&lyt, &drawer, &edges, &n](const auto& f)
                              {
                                  edges << fmt::format("{} -> {} [style={}];\n", drawer.tile_id(lyt.get_tile(f.object)),
                                                       drawer.tile_id(lyt.get_tile(n)), "solid");
                              });
        });

    // enforce topological structure
    topology << drawer.enforce_topology(lyt);

    // draw layout
    os << fmt::format("digraph layout {{  // Generated by {} ({})\n{};\n\n", FICTION_VERSION, FICTION_REPO,
                      fmt::join(drawer.additional_graph_attributes(lyt), ";\n"))
       << nodes.rdbuf() << '\n'
       << edges.rdbuf() << '\n'
       << topology.rdbuf() << "}\n";
}
/*! \brief Writes layout in DOT format into a file
 *
 * \param lyt Layout
 * @param on_progress Receives completed drawing work.
 * \param filename Filename
 */
template <class Lyt, class Drawer>
void write_dot_layout(const Lyt& lyt, const std::string_view& filename, const Drawer& drawer = {},
                      utils::progress_callback on_progress = {})
{
    fiction::detail::atomic_write(filename, [&](std::ostream& os) { write_dot_layout(lyt, os, drawer, on_progress); });
}
}  // namespace fiction::layouts::io
