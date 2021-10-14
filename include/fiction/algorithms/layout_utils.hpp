//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "../traits.hpp"
#include "network_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

namespace fiction
{
/**
 * Place 0-input gates.
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param t
 * @param ntk
 * @param n
 * @return
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    if constexpr (mockturtle::has_is_pi_v<Ntk>)
    {
        if (ntk.is_pi(n))
        {
            if constexpr (mockturtle::has_has_name_v<Ntk> && mockturtle::has_get_name_v<Ntk>)
            {
                return lyt.create_pi(ntk.has_name(n) ? ntk.get_name(n) : "", t);
            }
            else
            {
                return lyt.create_pi("", t);
            }
        }
    }
    // more gate types go here

    assert(false);  // n must be of some supported type
    return {};      // fix -Wreturn-type warning
}
/**
 * Place 1-input gates.
 * @tparam Lyt
 * @param t
 * @tparam Ntk
 * @param lyt
 * @param ntk
 * @param n
 * @param a
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                              const mockturtle::signal<Lyt>& a) noexcept
{
    if constexpr (has_is_inv_v<Ntk>)
    {
        if (ntk.is_inv(n))
        {
            return lyt.create_not(a, t);
        }
    }
    if constexpr (has_is_buf_v<Ntk>)
    {
        if (ntk.is_buf(n))
        {
            return lyt.create_buf(a, t);
        }
    }
    // more gate types go here

    assert(false);  // n must be of some supported type
    return {};      // fix -Wreturn-type warning
}

/**
 * Place 2-input gates.
 * @tparam Lyt
 * @param t
 * @tparam Ntk
 * @param lyt
 * @param ntk
 * @param n
 * @param a
 * @param b
 * @param c
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                              const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b,
                              const std::optional<bool>& c = std::nullopt) noexcept
{
    if constexpr (mockturtle::has_is_and_v<Ntk>)
    {
        if (ntk.is_and(n))
        {
            return lyt.create_and(a, b, t);
        }
    }
    if constexpr (mockturtle::has_is_or_v<Ntk>)
    {
        if (ntk.is_or(n))
        {
            return lyt.create_or(a, b, t);
        }
    }
    if constexpr (mockturtle::has_is_xor_v<Ntk>)
    {
        if (ntk.is_xor(n))
        {
            return lyt.create_xor(a, b, t);
        }
    }
    if constexpr (fiction::has_is_nand_v<Ntk>)
    {
        if (ntk.is_nand(n))
        {
            return lyt.create_nand(a, b, t);
        }
    }
    if constexpr (fiction::has_is_nor_v<Ntk>)
    {
        if (ntk.is_nor(n))
        {
            return lyt.create_nor(a, b, t);
        }
    }
    if constexpr (mockturtle::has_is_maj_v<Ntk>)
    {
        if (ntk.is_maj(n))
        {
            assert(c.has_value());

            if (*c)  // constant signal c points to 1
            {
                return lyt.create_or(a, b, t);
            }
            else  // constant signal c points to 0
            {
                return lyt.create_and(a, b, t);
            }
        }
    }
    // more gate types go here
    if constexpr (mockturtle::has_is_function_v<Ntk>)
    {
        if (ntk.is_function(n))
        {
            if (c.has_value())
            {
                return lyt.create_node({a, b, *c}, ntk.node_function(n), t);
            }
            else
            {
                return lyt.create_node({a, b}, ntk.node_function(n), t);
            }
        }
    }

    assert(false);  // n must be of some supported type
    return {};      // fix -Wreturn-type warning
}
/**
 * Place 3-input gates.
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param t
 * @param ntk
 * @param n
 * @param a
 * @param b
 * @param c
 * @return
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                              const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b,
                              const mockturtle::signal<Lyt>& c) noexcept
{
    if constexpr (mockturtle::has_is_maj_v<Ntk>)
    {
        if (ntk.is_maj(n))
        {
            return lyt.create_maj(a, b, c, t);
        }
    }
    // more gate types go here

    assert(false);  // n must be of some supported type
    return {};      // fix -Wreturn-type warning
}
/**
 * Place gates from a network.
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param t
 * @param ntk
 * @param n
 * @param node2pos
 * @return
 */
template <typename Lyt, typename Ntk>
mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
                              const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept
{
    const auto fc = fanins(ntk, n);

    if (const auto num_fanins = fc.fanin_nodes.size(); num_fanins == 0)
    {
        return place(lyt, t, ntk, n);
    }
    else if (num_fanins == 1)
    {
        const auto fanin_signal = ntk.make_signal(fc.fanin_nodes[0]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal]);
    }
    else if (num_fanins == 2)
    {
        const auto fanin_signal_a = ntk.make_signal(fc.fanin_nodes[0]);
        const auto fanin_signal_b = ntk.make_signal(fc.fanin_nodes[1]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal_a], node2pos[fanin_signal_b], fc.constant_fanin);
    }
    else if (num_fanins == 3)
    {
        const auto fanin_signal_a = ntk.make_signal(fc.fanin_nodes[0]);
        const auto fanin_signal_b = ntk.make_signal(fc.fanin_nodes[1]);
        const auto fanin_signal_c = ntk.make_signal(fc.fanin_nodes[2]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal_a], node2pos[fanin_signal_b], node2pos[fanin_signal_c]);
    }
    // more fanin sizes go here

    assert(false);  // unsupported number of fanins
    return {};      // fix -Wreturn-type warning
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
