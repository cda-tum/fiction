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
    std::array<std::string, 8> colors{{"ghostwhite", "deepskyblue1", "darkseagreen2", "crimson", "goldenrod1",
                                       "darkorchid2", "chocolate1", "gray28"}};
};

}  // namespace fiction

#endif  // FICTION_DOT_DRAWERS_HPP
