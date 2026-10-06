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
 * @brief Helpers that place network nodes of any arity onto layout tiles.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/networks/network_utils.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/stl/array_utils.hpp"

#include <kitty/operations.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace fiction::physical_design
{

/**
 * Place 0-input gates.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param lyt Gate-level layout in which to place a 0-input gate.
 * @param t Tile in `lyt` to place the gate onto.
 * @param ntk Network whose node is to be placed.
 * @param n Node in `ntk` to place onto `t` in `lyt`.
 * @return Output port pointing to the placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] typename Lyt::output_port place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                              const mockturtle::node<Ntk>& n)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    if constexpr (mockturtle::has_is_pi_v<Ntk>)
    {
        if (ntk.is_pi(n))
        {
            if constexpr (mockturtle::has_has_name_v<Ntk> && mockturtle::has_get_name_v<Ntk>)
            {
                return lyt.create_pi(ntk.has_name(ntk.make_signal(n)) ? ntk.get_name(ntk.make_signal(n)) : "", t);
            }
            else
            {
                return lyt.create_pi("", t);
            }
        }
    }
    throw std::invalid_argument("Unsupported gate function");
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
 * @return Output port pointing to the placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] typename Lyt::output_port place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                              const mockturtle::node<Ntk>& n, const typename Lyt::output_port& a)
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
    throw std::invalid_argument("Unsupported gate function");
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
 * @return Output port pointing to the placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] typename Lyt::output_port
place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const typename Lyt::output_port& a,
      const typename Lyt::output_port& b, const std::optional<bool>& c = std::nullopt)
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
            if (!c.has_value())
            {
                throw std::invalid_argument("A two-input majority placement requires a constant input");
            }

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
                auto function = ntk.node_function(n);
                if (function.num_vars() != 3)
                {
                    throw std::invalid_argument("Constant reduction requires a three-input function");
                }
                uint8_t constant_index{};
                ntk.foreach_fanin(n,
                                  [&](const auto& f, const auto index)
                                  {
                                      if (ntk.is_constant(ntk.get_node(f)))
                                      {
                                          constant_index = static_cast<uint8_t>(index);
                                      }
                                  });
                for (auto index = constant_index; index < 2; ++index)
                {
                    kitty::swap_inplace(function, index, static_cast<uint8_t>(index + 1));
                }
                if (*c)
                {
                    kitty::cofactor1_inplace(function, uint8_t{2});
                }
                else
                {
                    kitty::cofactor0_inplace(function, uint8_t{2});
                }
                return lyt.create_node({a, b}, kitty::shrink_to(function, 2), t);
            }

            return lyt.create_node({a, b}, ntk.node_function(n), t);
        }
    }

    throw std::invalid_argument("Unsupported gate function");
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
 * @return Output port pointing to the placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] typename Lyt::output_port place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                              const mockturtle::node<Ntk>& n, const typename Lyt::output_port& a,
                                              const typename Lyt::output_port& b, const typename Lyt::output_port& c)
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

    throw std::invalid_argument("Unsupported gate function");
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
 * @param node2pos Mapping from network nodes to layout output ports. The
 * map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this function.
 * @return Output port of the newly placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk>
[[nodiscard]] typename Lyt::output_port place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk,
                                              const mockturtle::node<Ntk>&                                n,
                                              const mockturtle::node_map<typename Lyt::output_port, Ntk>& node2pos)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto fc = networks::fanins(ntk, n);

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

    throw std::invalid_argument("Unsupported gate input count");
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
        /**
         * Destination network node.
         */
        const mockturtle::node<Ntk> ntk_node;
        /**
         * Output port at the end of the route.
         */
        typename Lyt::output_port lyt_signal;

        /**
         * Associates a network destination with a layout output port.
         */
        branching_signal(const mockturtle::node<Ntk>& n, const typename Lyt::output_port& s) :
                ntk_node{n},
                lyt_signal{s}
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
    [[nodiscard]] typename Lyt::output_port operator[](const mockturtle::node<Ntk>& n) const
    {
        if (const auto branch = std::ranges::find_if(branches,
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
    void update_branch(const mockturtle::node<Ntk>& ntk_node, const typename Lyt::output_port& lyt_signal)
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
        fiction::utils::stl::create_array<fanout_size, std::shared_ptr<branching_signal>>(nullptr);
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
 * @param node2pos Mapping from network nodes to layout output ports via
 * branches. The map is used to fetch location of the fanins. The `mockturtle::node_map` is not updated by this
 * function.
 * @return Output port of the newly placed gate in `lyt`.
 */
template <typename Lyt, typename Ntk, uint16_t fanout_size = 2>
[[nodiscard]] typename Lyt::output_port
place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n,
      const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout type");
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto fc = networks::fanins(ntk, n);

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

    throw std::invalid_argument("Unsupported gate input count");
}

}  // namespace fiction::physical_design
