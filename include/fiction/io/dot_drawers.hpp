//
// Created by marcel on 26.05.21.
//

#ifndef FICTION_DOT_DRAWERS_HPP
#define FICTION_DOT_DRAWERS_HPP

#include "../traits.hpp"

#include <fmt/format.h>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/traits.hpp>

#include <array>

namespace fiction
{

template <typename Ntk, bool DrawIndexes = false>
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
        if constexpr (has_is_wire_v<Ntk>)
        {
            if (ntk.is_wire(n))
            {
                return "palegoldenrod";
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
        if constexpr (has_is_wire_v<Ntk>)
        {
            if (ntk.is_wire(n))
            {
                return "WIRE";
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

        return mockturtle::gate_dot_drawer<Ntk>::node_label(ntk, n);
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
