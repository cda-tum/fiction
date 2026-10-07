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
 * @brief Helpers that transfer names between networks and layouts.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/physical_design/placement_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <string>
#include <string_view>

namespace fiction::networks
{

/**
 * Helper function to conveniently fetch the name from a layout or network as they use different function names for the
 * same purpose.
 *
 * @tparam NtkOrLyt Network or layout type.
 * @param ntk_or_lyt Network or layout object.
 * @return Name of given network or layout.
 */
template <typename NtkOrLyt>
std::string get_name(const NtkOrLyt& ntk_or_lyt)
{
    if constexpr (mockturtle::has_get_network_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.get_network_name();
    }
    else if constexpr (fiction::has_get_layout_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.get_layout_name();
    }

    return {};
}
/**
 * Helper function to conveniently assign a name to a layout or network as they use different function names for the
 * same purpose.
 *
 * @tparam NtkOrLyt Network or layout type.
 * @param ntk_or_lyt Network or layout object.
 * @param name Name to assign to given network or layout.
 */
template <typename NtkOrLyt>
void set_name(NtkOrLyt& ntk_or_lyt, const std::string_view& name)
{
    if constexpr (mockturtle::has_set_network_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.set_network_name(std::string{name});
    }
    else if constexpr (fiction::has_set_layout_name_v<NtkOrLyt>)
    {
        return ntk_or_lyt.set_layout_name(std::string{name});
    }
}
/**
 * Helper function to conveniently assign the name of a source network or layout to a target network or layout as they
 * use different function names for the same purpose. This function comes in handy when networks are translated or
 * layouts are being created from networks that are supposed to have the same name.
 *
 * @tparam NtkOrLytSrc Source network or layout type.
 * @tparam NtkOrLytDest Target network or layout type.
 * @param ntk_or_lyt_src Source network or layout whose name is to be assigned to `ntk_or_lyt_dest`.
 * @param ntk_or_lyt_dest Target network or layout that is to be assigned `ntk_or_lyt_src`'s name.
 */
template <typename NtkOrLytSrc, typename NtkOrLytDest>
void restore_network_name(const NtkOrLytSrc& ntk_or_lyt_src, NtkOrLytDest& ntk_or_lyt_dest)
{
    static_assert(mockturtle::is_network_type_v<NtkOrLytSrc> || is_gate_level_layout_v<NtkOrLytSrc>);
    static_assert(mockturtle::is_network_type_v<NtkOrLytDest> || is_gate_level_layout_v<NtkOrLytDest>);

    std::string network_name{};

    if constexpr (mockturtle::has_get_network_name_v<NtkOrLytSrc>)
    {
        network_name = ntk_or_lyt_src.get_network_name();
    }
    else if constexpr (fiction::has_get_layout_name_v<NtkOrLytSrc>)
    {
        network_name = ntk_or_lyt_src.get_layout_name();
    }

    if constexpr (mockturtle::has_set_network_name_v<NtkOrLytDest>)
    {
        ntk_or_lyt_dest.set_network_name(network_name);
    }
    else if constexpr (fiction::has_set_layout_name_v<NtkOrLytDest>)
    {
        ntk_or_lyt_dest.set_layout_name(network_name);
    }
}
/**
 * Assigns input names from one network to another. Matching inputs are identified by their index.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @param ntk_src Source logic network whose input names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose inputs are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_input_names(const NtkSrc& ntk_src, NtkDest& ntk_dest)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc> || is_gate_level_layout_v<NtkSrc>);
    static_assert(mockturtle::is_network_type_v<NtkDest> || is_gate_level_layout_v<NtkDest>);

    if constexpr ((is_gate_level_layout_v<NtkSrc> ||
                   (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc>)) &&
                  (is_gate_level_layout_v<NtkDest> || mockturtle::has_set_name_v<NtkDest>))
    {
        ntk_src.foreach_pi(
            [&](const auto& pi, const auto i)
            {
                const auto input = [&]
                {
                    if constexpr (is_gate_level_layout_v<NtkSrc>)
                    {
                        return ntk_src.output(pi);
                    }
                    else
                    {
                        return ntk_src.make_signal(pi);
                    }
                }();
                if (ntk_src.has_name(input))
                {
                    if constexpr (is_gate_level_layout_v<NtkDest>)
                    {
                        ntk_dest.set_name(ntk_dest.pi_at(i), ntk_src.get_name(input));
                    }
                    else
                    {
                        ntk_dest.set_name(ntk_dest.make_signal(ntk_dest.pi_at(i)), ntk_src.get_name(input));
                    }
                }
            });
    }
}
/**
 * Assigns output names from one network to another. Matching outputs are identified by their order.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @param ntk_src Source logic network whose output names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose outputs are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_output_names(const NtkSrc& ntk_src, NtkDest& ntk_dest)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc> || is_gate_level_layout_v<NtkSrc>);
    static_assert(mockturtle::is_network_type_v<NtkDest> || is_gate_level_layout_v<NtkDest>);

    if constexpr (mockturtle::has_has_output_name_v<NtkSrc> && mockturtle::has_get_output_name_v<NtkSrc> &&
                  (is_gate_level_layout_v<NtkDest> || mockturtle::has_set_output_name_v<NtkDest>))
    {

        ntk_src.foreach_po(
            [&ntk_src, &ntk_dest]([[maybe_unused]] const auto& po, const auto i)
            {
                if (ntk_src.has_output_name(i))
                {
                    auto name = ntk_src.get_output_name(i);

                    ntk_dest.set_output_name(i, name);
                }
            });
    }
}
/**
 * Transfers signal names from a logic network to a network or placed layout using a `mockturtle::node_map`.
 * Skips absent native target endpoints. Complemented signal names transfer when used by gates or POs.
 *
 * @tparam NtkSrc Source logic network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @tparam Signal Target signal or layout output port type.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest`.
 */
template <typename NtkSrc, typename NtkDest, typename Signal>
void restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<Signal, NtkSrc>& old2new)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a logic network");
    static_assert(mockturtle::is_network_type_v<NtkDest> || is_gate_level_layout_v<NtkDest>);

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  (is_gate_level_layout_v<NtkDest> || mockturtle::has_set_name_v<NtkDest>))
    {
        static_assert(mockturtle::has_foreach_node_v<NtkSrc>, "NtkSrc does not implement the foreach_node function");
        static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");

        /** Restore one mapped source signal, leaving absent native endpoints untouched. */
        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                /** Target endpoint corresponding to the source node. */
                const auto target = old2new[ntk_src.get_node(f)];
                if constexpr (is_gate_level_layout_v<NtkDest>)
                {
                    if (target == typename NtkDest::output_port{})
                    {
                        return;
                    }
                }
                ntk_dest.set_name(target, ntk_src.get_name(f));
            }
        };

        ntk_src.foreach_node(
            [&ntk_src, &restore_signal_name](const auto& n)
            {
                restore_signal_name(ntk_src.make_signal(n));
                // names_view stores complemented signal names separately from node-output names.
                ntk_src.foreach_fanin(n,
                                      [&ntk_src, &restore_signal_name](const auto& f)
                                      {
                                          if (ntk_src.is_complemented(f))
                                          {
                                              restore_signal_name(f);
                                          }
                                      });
            });
        ntk_src.foreach_po(
            [&ntk_src, &restore_signal_name](const auto& f)
            {
                if (ntk_src.is_complemented(f))
                {
                    restore_signal_name(f);
                }
            });
    }
}
/**
 * Same as the other restore_signal_names function but this overload uses a `mockturtle::node_map` with a
 * branching_signal_container that is specifically used for networks or layouts that allow branches to be distinct,
 * e.g., by their position on the layout.
 *
 * @tparam NtkSrc Source logic network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @tparam fanout_size Maximum fanout size in the network.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a branching_signal_container.
 */
template <typename NtkSrc, typename NtkDest, uint16_t fanout_size = 2>
void restore_signal_names(
    const NtkSrc& ntk_src, NtkDest& ntk_dest,
    const mockturtle::node_map<physical_design::branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>&
        old2new)
{
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a logic network");
    static_assert(mockturtle::is_network_type_v<NtkDest> || is_gate_level_layout_v<NtkDest>);

    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> &&
                  (is_gate_level_layout_v<NtkDest> || mockturtle::has_set_name_v<NtkDest>))
    {
        static_assert(mockturtle::has_foreach_node_v<NtkSrc>, "NtkSrc does not implement the foreach_node function");
        static_assert(mockturtle::has_foreach_fanin_v<NtkSrc>, "NtkSrc does not implement the foreach_fanin function");
        static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node function");

        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& n, const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                const auto name = ntk_src.get_name(f);

                ntk_dest.set_name((old2new[ntk_src.get_node(f)][n]), name);
            }
        };

        ntk_src.foreach_node(
            [&ntk_src, &restore_signal_name](const auto& n)
            { ntk_src.foreach_fanin(n, [&restore_signal_name, &n](const auto& f) { restore_signal_name(n, f); }); });
    }
}
/**
 * Transfers all input and output names as well as the network/layout name from one network to another. This function
 * calls `restore_network_name`, `restore_input_names`, and `restore_output_names`.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @param ntk_src Source logic network whose I/O names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose I/O names are to be assigned `ntk_src`'s names.
 */
template <typename NtkSrc, typename NtkDest>
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest)
{
    restore_network_name(ntk_src, ntk_dest);
    restore_input_names(ntk_src, ntk_dest);
    restore_output_names(ntk_src, ntk_dest);
}
/**
 * Transfers all signal and output names as well as the network/layout name from one network to another. This function
 * calls `restore_network_name`, `restore_signal_names`, and `restore_output_names`.
 *
 * @tparam NtkSrc Source network type.
 * @tparam NtkDest Target network or gate-level layout type.
 * @tparam T Mapping type to identify signals by. Currently, `mockturtle::signal<NtkDest>` and
 * `branching_signal_container<NtkDest, NtkSrc, fanout_size>` are supported.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a signal identifier.
 */
template <typename NtkSrc, typename NtkDest, typename T>
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new)
{
    restore_network_name(ntk_src, ntk_dest);
    restore_input_names(ntk_src, ntk_dest);
    restore_signal_names(ntk_src, ntk_dest, old2new);
    restore_output_names(ntk_src, ntk_dest);
}

}  // namespace fiction::networks
