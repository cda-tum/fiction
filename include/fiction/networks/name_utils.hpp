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

#include "fiction/layouts/gate_layout.hpp"
#include "fiction/physical_design/placement_utils.hpp"
#include "fiction/types.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <concepts>
#include <cstdint>
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
    else if constexpr (requires { ntk_or_lyt.get_layout_name(); })
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
    else if constexpr (requires { ntk_or_lyt.set_layout_name(std::string{name}); })
    {
        return ntk_or_lyt.set_layout_name(std::string{name});
    }
}
/**
 * Transfers the source network or gate-layout name. An unnamed source clears a supported destination name.
 * A destination without naming support is unchanged.
 *
 * @tparam NtkOrLytSrc Source network or layout type.
 * @tparam NtkOrLytDest Target network or layout type.
 * @param ntk_or_lyt_src Source network or layout whose name is to be assigned to `ntk_or_lyt_dest`.
 * @param ntk_or_lyt_dest Target network or layout that is to be assigned `ntk_or_lyt_src`'s name.
 */
template <typename NtkOrLytSrc, typename NtkOrLytDest>
    requires(mockturtle::is_network_type_v<NtkOrLytSrc> ||
             requires { requires std::same_as<typename NtkOrLytSrc::object_id, layouts::layout_object_id>; }) &&
            (mockturtle::is_network_type_v<NtkOrLytDest> ||
             requires { requires std::same_as<typename NtkOrLytDest::object_id, layouts::layout_object_id>; })
void restore_network_name(const NtkOrLytSrc& ntk_or_lyt_src, NtkOrLytDest& ntk_or_lyt_dest)
{
    set_name(ntk_or_lyt_dest, get_name(ntk_or_lyt_src));
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
    requires(mockturtle::is_network_type_v<NtkSrc> ||
             requires { requires std::same_as<typename NtkSrc::object_id, layouts::layout_object_id>; }) &&
            (mockturtle::is_network_type_v<NtkDest> ||
             requires { requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>; }) &&
            (!((requires {
                   requires std::same_as<typename NtkSrc::object_id, layouts::layout_object_id>;
               } || (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc>)) && (requires {
                   requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>;
               } || mockturtle::has_set_name_v<NtkDest>)) ||
             (requires(const NtkSrc& src, NtkDest& dest) {
                 src.foreach_pi([](const auto&, const auto) {});
                 dest.pi_at(uint32_t{});
             } && (requires {
                  requires std::same_as<typename NtkSrc::object_id, layouts::layout_object_id>;
              } || mockturtle::has_make_signal_v<NtkSrc>) && (requires {
                  requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>;
              } || mockturtle::has_make_signal_v<NtkDest>)))
void restore_input_names(const NtkSrc& ntk_src, NtkDest& ntk_dest)
{
    if constexpr ((requires {
                      requires std::same_as<typename NtkSrc::object_id, fiction::layouts::layout_object_id>;
                  } || (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc>)) && (requires {
                      requires std::same_as<typename NtkDest::object_id, fiction::layouts::layout_object_id>;
                  } || mockturtle::has_set_name_v<NtkDest>))
    {
        ntk_src.foreach_pi(
            [&](const auto& pi, const auto i)
            {
                const auto input = [&]
                {
                    if constexpr (requires {
                                      requires std::same_as<typename NtkSrc::object_id,
                                                            fiction::layouts::layout_object_id>;
                                  })
                    {
                        return pi;
                    }
                    else
                    {
                        return ntk_src.make_signal(pi);
                    }
                }();
                if (ntk_src.has_name(input))
                {
                    if constexpr (requires {
                                      requires std::same_as<typename NtkDest::object_id,
                                                            fiction::layouts::layout_object_id>;
                                  })
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
    requires(mockturtle::is_network_type_v<NtkSrc> ||
             requires { requires std::same_as<typename NtkSrc::object_id, layouts::layout_object_id>; }) &&
            (mockturtle::is_network_type_v<NtkDest> ||
             requires { requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>; }) &&
            (!(mockturtle::has_has_output_name_v<NtkSrc> && mockturtle::has_get_output_name_v<NtkSrc> && (requires {
                   requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>;
               } || mockturtle::has_set_output_name_v<NtkDest>)) ||
             requires(const NtkSrc& src) { src.foreach_po([](const auto&, const auto) {}); })
void restore_output_names(const NtkSrc& ntk_src, NtkDest& ntk_dest)
{
    if constexpr (mockturtle::has_has_output_name_v<NtkSrc> && mockturtle::has_get_output_name_v<NtkSrc> && (requires {
                      requires std::same_as<typename NtkDest::object_id, fiction::layouts::layout_object_id>;
                  } || mockturtle::has_set_output_name_v<NtkDest>))
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
    requires mockturtle::is_network_type_v<NtkSrc> &&
             (mockturtle::is_network_type_v<NtkDest> ||
              requires { requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>; }) &&
             (!(mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> && (requires {
                    requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>;
                } || mockturtle::has_set_name_v<NtkDest>)) ||
              (mockturtle::has_foreach_node_v<NtkSrc> && mockturtle::has_foreach_fanin_v<NtkSrc> &&
               mockturtle::has_get_node_v<NtkSrc> && mockturtle::has_foreach_po_v<NtkSrc> &&
               mockturtle::has_make_signal_v<NtkSrc> && mockturtle::has_is_complemented_v<NtkSrc>))
void restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<Signal, NtkSrc>& old2new)
{
    if constexpr (mockturtle::has_has_name_v<NtkSrc> && mockturtle::has_get_name_v<NtkSrc> && (requires {
                      requires std::same_as<typename NtkDest::object_id, fiction::layouts::layout_object_id>;
                  } || mockturtle::has_set_name_v<NtkDest>))
    {
        /** Restore one mapped source signal, leaving absent native endpoints untouched. */
        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                /** Target endpoint corresponding to the source node. */
                const auto target = old2new[ntk_src.get_node(f)];
                if constexpr (requires {
                                  requires std::same_as<typename NtkDest::object_id,
                                                        fiction::layouts::layout_object_id>;
                              })
                {
                    if (target == typename NtkDest::object_id{})
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
 * e.g., by their position on the layout. Skips branches with no mapped layout object.
 *
 * @tparam NtkDest Target network or gate-level layout type.
 * @tparam fanout_size Maximum fanout size in the network.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a branching_signal_container.
 */
template <typename NtkDest, uint16_t fanout_size = 2>
    requires(mockturtle::is_network_type_v<NtkDest> ||
             requires { requires std::same_as<typename NtkDest::object_id, layouts::layout_object_id>; })
void restore_signal_names(
    const tec_nt& ntk_src, NtkDest& ntk_dest,
    const mockturtle::node_map<physical_design::branching_signal_container<fanout_size>, tec_nt>& old2new)
{
    if constexpr (requires {
                      requires std::same_as<typename NtkDest::object_id, fiction::layouts::layout_object_id>;
                  } || mockturtle::has_set_name_v<NtkDest>)
    {
        const auto restore_signal_name = [&ntk_src, &ntk_dest, &old2new](const auto& n, const auto& f)
        {
            if (ntk_src.has_name(f))
            {
                const auto target = old2new[ntk_src.get_node(f)][n];
                if (target == layouts::layout_object_id{})
                {
                    return;
                }
                ntk_dest.set_name(target, ntk_src.get_name(f));
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
    requires requires(const NtkSrc& src, NtkDest& dest) {
        restore_network_name(src, dest);
        restore_input_names(src, dest);
        restore_output_names(src, dest);
    }
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
 * `branching_signal_container<fanout_size>` are supported.
 * @param ntk_src Source logic network whose signal names are to be transferred to `ntk_dest`.
 * @param ntk_dest Target logic network whose signal names are to be assigned `ntk_src`'s names.
 * @param old2new Mapping of signals from `ntk_src` to `ntk_dest` using a signal identifier.
 */
template <typename NtkSrc, typename NtkDest, typename T>
    requires requires(const NtkSrc& src, NtkDest& dest, mockturtle::node_map<T, NtkSrc>& map) {
        restore_network_name(src, dest);
        restore_input_names(src, dest);
        restore_signal_names(src, dest, map);
        restore_output_names(src, dest);
    }
void restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new)
{
    restore_network_name(ntk_src, ntk_dest);
    restore_input_names(ntk_src, ntk_dest);
    restore_signal_names(ntk_src, ntk_dest, old2new);
    restore_output_names(ntk_src, ntk_dest);
}

}  // namespace fiction::networks
