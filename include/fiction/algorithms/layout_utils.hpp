//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "../traits.hpp"
#include "../utils/array.hpp"
#include "network_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>

namespace fiction
{
/**
 * Returns the number of adjacent coordinates of a given one. This is not a constant value because c could be located at
 * a layout border.
 *
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param c Coordinate whose number of adjacencies are required.
 * @return Number of c's adjacent coordinates.
 */
template <typename Lyt>
[[nodiscard]] uint8_t num_adjacent_coordinates(const Lyt& lyt, const coordinate<Lyt>& c) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return static_cast<uint8_t>(lyt.template adjacent_tiles<std::set<coordinate<Lyt>>>(c).size());
}
/**
 * Reserve primary input nodes in a layout in the same order as they appear in a network.
 *
 * @tparam Lyt
 * @tparam Ntk
 * @param lyt
 * @param ntk
 * @return
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] mockturtle::node_map<mockturtle::node<Lyt>, Ntk> reserve_input_nodes(Lyt& lyt, const Ntk& ntk) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::has_create_pi_v<Lyt>, "Lyt does not implement the create_pi function");

    mockturtle::node_map<mockturtle::node<Lyt>, Ntk> pi_map{ntk};

    ntk.foreach_pi([&lyt, &pi_map](const auto& pi) { pi_map[pi] = lyt.get_node(lyt.create_pi("", {0, 0})); });
    // little hacky: move last created node to a dead tile to remove it from the layout again but preserve its existence
    lyt.move_node(lyt.get_node({0, 0}), {});

    return pi_map;
}
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
[[nodiscard]] mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                            const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

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
[[nodiscard]] mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                            const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

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
[[nodiscard]] mockturtle::signal<Lyt>
place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a,
      const mockturtle::signal<Lyt>& b, const std::optional<bool>& c = std::nullopt) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

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
[[nodiscard]] mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                            const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a,
                                            const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    if constexpr (mockturtle::has_is_maj_v<Ntk>)
    {
        if (ntk.is_maj(n))
        {
            return lyt.create_maj(a, b, c, t);
        }
    }
    // more gate types go here
    if constexpr (mockturtle::has_is_function_v<Ntk>)
    {
        if (ntk.is_function(n))
        {
            return lyt.create_node({a, b, c}, ntk.node_function(n), t);
        }
    }

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
[[nodiscard]] mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                            const mockturtle::node<Ntk>&                              n,
                                            const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

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

template <typename Lyt, typename Ntk, uint16_t fanout_size = 2>
struct branching_signal_container
{
    struct branching_signal
    {
        const mockturtle::node<Ntk> ntk_node;
        mockturtle::signal<Lyt>     lyt_signal;

        branching_signal(const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& s) : ntk_node{n}, lyt_signal{s}
        {
            static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
            static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
        }
    };

    std::array<std::shared_ptr<branching_signal>, fanout_size> branches =
        create_array<fanout_size, std::shared_ptr<branching_signal>>(nullptr);

    [[nodiscard]] mockturtle::signal<Lyt> operator[](const mockturtle::node<Ntk>& n) const
    {
        if (const auto branch = std::find_if(branches.cbegin(), branches.cend(),
                                             [&n](const auto& b)
                                             {
                                                 if (b != nullptr)
                                                 {
                                                     if (b->ntk_node == n)
                                                     {
                                                         return true;
                                                     }
                                                 }

                                                 return false;
                                             });
            branch != branches.cend())
        {
            return (*branch)->lyt_signal;
        }
        else
        {
            return {};
        }
    }

    void update_branch(const mockturtle::node<Ntk>& ntk_node, const mockturtle::signal<Lyt>& lyt_signal)
    {
        for (auto i = 0u; i < branches.size(); ++i)
        {
            if (const auto b = branches[i]; b != nullptr)
            {
                if (b->ntk_node == ntk_node)
                {
                    b->lyt_signal = lyt_signal;

                    return;
                }
            }
            else
            {
                branches[i] = std::make_shared<branching_signal>(ntk_node, lyt_signal);

                return;
            }
        }
    }
};

template <typename Lyt, typename Ntk, uint16_t fanout_size = 2>
[[nodiscard]] mockturtle::signal<Lyt>
place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
      const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto fc = fanins(ntk, n);

    if (const auto num_fanins = fc.fanin_nodes.size(); num_fanins == 0)
    {
        return place(lyt, t, ntk, n);
    }
    else if (num_fanins == 1)
    {
        const auto fanin_signal = ntk.make_signal(fc.fanin_nodes[0]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal][n]);
    }
    else if (num_fanins == 2)
    {
        const auto fanin_signal_a = ntk.make_signal(fc.fanin_nodes[0]);
        const auto fanin_signal_b = ntk.make_signal(fc.fanin_nodes[1]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal_a][n], node2pos[fanin_signal_b][n], fc.constant_fanin);
    }
    else if (num_fanins == 3)
    {
        const auto fanin_signal_a = ntk.make_signal(fc.fanin_nodes[0]);
        const auto fanin_signal_b = ntk.make_signal(fc.fanin_nodes[1]);
        const auto fanin_signal_c = ntk.make_signal(fc.fanin_nodes[2]);

        return place(lyt, t, ntk, n, node2pos[fanin_signal_a][n], node2pos[fanin_signal_b][n],
                     node2pos[fanin_signal_c][n]);
    }
    // more fanin sizes go here

    assert(false);  // unsupported number of fanins
    return {};      // fix -Wreturn-type warning
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
