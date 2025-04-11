//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_PLACEMENT_UTILS_HPP
#define FICTION_PLACEMENT_UTILS_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/array_utils.hpp"
#include "fiction/utils/network_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

namespace fiction
{

/**
 * Reserve primary input nodes in a layout in the same order as they appear in a network. This is a useful function to
 * call first when a layout is to be created from a network. The primary input nodes then exist in the layout, but are
 * not placed anywhere and also do not have names. They are just registered to preserve their order.
 *
 * This function can be seen as an equivalent to `mockturtle::initialize_copy_network`, but for layouts.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout where primary input nodes are to be reserved.
 * @param ntk Network whose primary inputs are to be reserved in `lyt`.
 * @return A `mockturtle::node_map` that maps from network nodes to layout nodes to be able to address the created
 * nodes.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] mockturtle::node_map<mockturtle::node<Lyt>, Ntk> reserve_input_nodes(Lyt& lyt, const Ntk& ntk) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function");
    static_assert(mockturtle::has_make_signal_v<Ntk>, "Ntk does not implement the make_signal function");
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::has_create_pi_v<Lyt>, "Lyt does not implement the create_pi function");

    mockturtle::node_map<mockturtle::node<Lyt>, Ntk> pi_map{ntk};

    ntk.foreach_pi(
        [&lyt, &ntk, &pi_map](const auto& pi)
        {
            std::string pi_name{};

            if constexpr (mockturtle::has_has_name_v<Ntk> && mockturtle::has_get_name_v<Ntk>)
            {
                if (const auto pi_signal = ntk.make_signal(pi); ntk.has_name(pi_signal))
                {
                    pi_name = ntk.get_name(pi_signal);
                }
            }

            pi_map[pi] = lyt.get_node(lyt.create_pi(pi_name, {0, 0}));
        });
    // little hacky: move last created node to a dead tile to remove it from the layout again but preserve its existence
    lyt.move_node(lyt.get_node({0, 0}), {});

    return pi_map;
}
/**
 * Place 0-input gates.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place a 0-input gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @return Signal pointing to the placed gate in `lyt`.
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
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place a 1-input gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @param a Incoming signal to the newly placed gate in `lyt`.
 * @return Signal pointing to the placed gate in `lyt`.
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
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place a 2-input gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @param a First incoming signal to the newly placed gate in `lyt`.
 * @param b Second incoming signal to the newly placed gate in `lyt`.
 * @param c Third optional incoming constant value signal to the newly placed gate in `lyt`. Might change the gate
 * function when set, e.g., from a MAJ to an AND if `c == false`.
 * @return Signal pointing to the placed gate in `lyt`.
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
    if constexpr (fiction::has_is_lt_v<Ntk>)
    {
        if (ntk.is_lt(n))
        {
            return lyt.create_lt(a, b, t);
        }
    }
    if constexpr (fiction::has_is_le_v<Ntk>)
    {
        if (ntk.is_le(n))
        {
            return lyt.create_le(a, b, t);
        }
    }
    if constexpr (fiction::has_is_gt_v<Ntk>)
    {
        if (ntk.is_gt(n))
        {
            return lyt.create_gt(a, b, t);
        }
    }
    if constexpr (fiction::has_is_ge_v<Ntk>)
    {
        if (ntk.is_ge(n))
        {
            return lyt.create_ge(a, b, t);
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
            // constant signal c points to 0
            return lyt.create_and(a, b, t);
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

            return lyt.create_node({a, b}, ntk.node_function(n), t);
        }
    }

    assert(false);  // n must be of some supported type
    return {};      // fix -Wreturn-type warning
}
/**
 * Place 3-input gates.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place a 3-input gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @param a First incoming signal to the newly placed gate in `lyt`.
 * @param b Second incoming signal to the newly placed gate in `lyt`.
 * @param c Third incoming signal to the newly placed gate in `lyt`.
 * @return Signal pointing to the placed gate in `lyt`.
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
 * Place any gate from a network. This function automatically identifies the arity of the passed node and fetches its
 * incoming signals from the given network and a provided `mockturtle::node_map`. This function does not update the
 * `mockturtle::node_map`.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place any gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 * @return Signal to the newly placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] mockturtle::signal<Lyt> place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                            const mockturtle::node<Ntk>&                              n,
                                            const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto fc = fanins(ntk, n);

    // NOLINTBEGIN(*-else-after-return)

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

    // NOLINTEND(*-else-after-return)

    assert(false);  // unsupported number of fanins
    return {};      // fix -Wreturn-type warning
}
/**
 * A container class to help identify layout locations of branching nodes like fanouts. When a node from a network is to
 * placed in a layout, fetching the node's fanins and looking for their locations in the layout does not work properly
 * when branching nodes like fanouts are involved that got extended by wire nodes. This container solves that issue.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @tparam fanout_size Maximum fanout size possible in the layout and/or the network.
 */
template <typename Lyt, typename Ntk, uint16_t fanout_size = 2>
struct branching_signal_container
{
    /**
     * Branch type.
     */
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
    /**
     * Accesses the branching container to find the location of a given node `n`. Returns the signal to that location if
     * it was already stored or the default signal, otherwise.
     *
     * @param n Node whose branching position is desired.
     * @return Signal to `n`'s layout location or the default signal if it wasn't found.
     */
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

        return {};
    }
    /**
     * Updates the given node's branch by another layout signal, thereby, creating a new branch or updating the position
     * of an existing one, e.g., if further wire segments were moving the head of the branch.
     *
     * @param ntk_node Node whose branch is to be updated.
     * @param lyt_signal New signal pointing to the end of the branch.
     */
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

  private:
    /**
     * Storage for all branches.
     */
    std::array<std::shared_ptr<branching_signal>, fanout_size> branches =
        create_array<fanout_size, std::shared_ptr<branching_signal>>(nullptr);
};
/**
 * Place any gate from a network. This function automatically identifies the arity of the passed node and fetches its
 * incoming signals from the given network and a provided branching_signal_container `mockturtle::node_map`. This
 * function does not update the `mockturtle::node_map`.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place any gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @param node2pos Mapping from network nodes to layout signals, i.e., a pointer to their position in the layout via
 * branches. The map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this
 * function.
 * @return Signal to the newly placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk, uint16_t fanout_size = 2>
[[nodiscard]] mockturtle::signal<Lyt>
place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
      const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto fc = fanins(ntk, n);

    // NOLINTBEGIN(*-else-after-return)

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

    // NOLINTEND(*-else-after-return)

    assert(false);  // unsupported number of fanins
    return {};      // fix -Wreturn-type warning
}

}  // namespace fiction

#endif  // FICTION_PLACEMENT_UTILS_HPP
