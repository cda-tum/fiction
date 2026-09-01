//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_NETWORKS_IO_DOT_DRAWERS_HPP
#define FICTION_NETWORKS_IO_DOT_DRAWERS_HPP

#include "fiction/networks/network_utils.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <kitty/print.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/traits.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace fiction::networks::io
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

    /**
     * Checks whether a string represents a non-negative integer, i.e., consists exclusively of digits.
     *
     * @param s String to check.
     * @return `true` iff `s` is non-empty and consists exclusively of digits.
     */
    [[nodiscard]] bool is_node_number(const std::string_view& s) const noexcept
    {
        return !s.empty() && std::ranges::all_of(s, [](const unsigned char c) { return std::isdigit(c) != 0; });
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
 * Writes a logic network in DOT format into an output stream, using one of the drawers above.
 *
 * @tparam Ntk Logic network type.
 * @tparam Drawer DOT drawer type.
 * @param ntk Network to write.
 * @param os Output stream.
 * @param drawer Drawer that decides the node labels, colors, and styles.
 */
template <typename Ntk, typename Drawer = technology_dot_drawer<Ntk, true>>
void write_dot_network(const Ntk& ntk, std::ostream& os, const Drawer& drawer = {})
{
    mockturtle::write_dot(ntk, os, drawer);
}
/**
 * Writes a logic network in DOT format into a file, using one of the drawers above.
 *
 * @tparam Ntk Logic network type.
 * @tparam Drawer DOT drawer type.
 * @param ntk Network to write.
 * @param filename Path of the file to write.
 * @param drawer Drawer that decides the node labels, colors, and styles.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
template <typename Ntk, typename Drawer = technology_dot_drawer<Ntk, true>>
void write_dot_network(const Ntk& ntk, const std::string_view& filename, const Drawer& drawer = {})
{
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_dot_network(ntk, os, drawer);
    os.close();
}

}  // namespace fiction::networks::io

#endif  // FICTION_NETWORKS_IO_DOT_DRAWERS_HPP
