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
 * @brief Converts a logic network into an equivalent one of another network type.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/networks/name_utils.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/node_resynthesis/shannon.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace fiction::synthesis
{

namespace detail
{

template <typename NtkDest, typename NtkSrc, bool is_same_ntk_type = std::is_same_v<NtkDest, NtkSrc>>
class convert_network_impl
{};

/** @brief Copies a source network into the destination network type. */
template <typename NtkDest, typename NtkSrc>
class convert_network_impl<NtkDest, NtkSrc, true>
{
  public:
    /** @brief Stores the source network for conversion. */
    explicit convert_network_impl(const NtkSrc& ntk_src) : ntk{ntk_src} {}

    /** @brief Returns a copy of the source network in the destination type. */
    NtkDest run()
    {
        return mockturtle::cleanup_dangling<NtkSrc, NtkDest>(ntk, false, false);
    }

  private:
    NtkDest ntk;
};

/** @brief Copies a source network into the destination network type. */
template <typename NtkDest, typename NtkSrc>
class convert_network_impl<NtkDest, NtkSrc, false>
{
  public:
    /** @brief Stores the source network for conversion. */
    explicit convert_network_impl(const NtkSrc& ntk_src) : ntk{ntk_src} {}

    /** @brief Returns a copy of the source network in the destination type. */
    NtkDest run()
    {
        auto  init     = mockturtle::initialize_copy_network<NtkDest>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename NtkDest::signal> children{};

            ntk.foreach_fanin(n,
                              [this, &ntk_dest, &old2new, &children](const auto& f)
                              {
                                  const auto fn         = ntk.get_node(f);
                                  const auto tgt_signal = old2new[fn];

                                  children.emplace_back(ntk.is_complemented(f) ? ntk_dest.create_not(tgt_signal) :
                                                                                 tgt_signal);
                              });

            return children;
        };

        ntk.foreach_gate(
            [&, this](const auto& g)
            {
                auto children = gather_fanin_signals(g);

                if constexpr (mockturtle::has_is_and_v<TopoNtkSrc> && mockturtle::has_create_and_v<NtkDest>)
                {
                    if (ntk.is_and(g))
                    {
                        old2new[g] = ntk_dest.create_and(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_or_v<TopoNtkSrc> && mockturtle::has_create_or_v<NtkDest>)
                {
                    if (ntk.is_or(g))
                    {
                        old2new[g] = ntk_dest.create_or(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_xor_v<TopoNtkSrc> && mockturtle::has_create_xor_v<NtkDest>)
                {
                    if (ntk.is_xor(g))
                    {
                        old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_maj_v<TopoNtkSrc> && mockturtle::has_create_maj_v<NtkDest>)
                {
                    if (ntk.is_maj(g))
                    {
                        old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_and_v<TopoNtkSrc> && mockturtle::has_create_nary_and_v<NtkDest>)
                {
                    if (ntk.is_nary_and(g))
                    {
                        old2new[g] = ntk_dest.create_nary_and(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_or_v<TopoNtkSrc> && mockturtle::has_create_nary_or_v<NtkDest>)
                {
                    if (ntk.is_nary_or(g))
                    {
                        old2new[g] = ntk_dest.create_nary_or(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_xor_v<TopoNtkSrc> && mockturtle::has_create_nary_xor_v<NtkDest>)
                {
                    if (ntk.is_nary_xor(g))
                    {
                        old2new[g] = ntk_dest.create_nary_xor(children);
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_buf_v<NtkDest>)
                {
                    if (ntk.is_buf(g))
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                }
                // a technology network keeps inverters as nodes; a target without `create_node` takes them as
                // complemented signals
                if constexpr (fiction::has_is_inv_v<TopoNtkSrc> && mockturtle::has_create_not_v<NtkDest>)
                {
                    if (ntk.is_inv(g))
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<NtkDest>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;
                }

                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc>)
                {
                    mockturtle::shannon_resynthesis<NtkDest>{}(ntk_dest, ntk.node_function(g), children.begin(),
                                                               children.end(),
                                                               [&](const auto& signal) { old2new[g] = signal; });
                    return true;
                }

                throw std::invalid_argument("network conversion requires a supported gate or its truth table");
            });

        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                const auto tgt_signal = old2new[ntk.get_node(po)];
                const auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;

                ntk_dest.create_po(tgt_po);
            });

        // restore signal names if applicable
        networks::restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<NtkSrc>;
    TopoNtkSrc ntk;
};
}  // namespace detail

/**
 * Converts a logic network into an equivalent one of another type. Thereby, this function is very similar to
 * `mockturtle::cleanup_dangling`. However, it supports real buffer nodes used for fanouts and path balancing in
 * fiction.
 *
 * @note If `NtkDest` and `NtkSrc` are of the same type, this function returns `ntk` cleaned using
 * `mockturtle::cleanup_dangling`.
 *
 * @tparam NtkDest Logic network with constant, input, output, and Boolean gate creation.
 * @tparam NtkSrc Logic network with node, interface, and fanin traversal.
 * @param ntk The input logic network.
 * @return A logic network of type `NtkDest` that is logically equivalent to `ntk`.
 */
template <typename NtkDest, typename NtkSrc>
    requires mockturtle::is_network_type_v<NtkSrc> && mockturtle::is_network_type_v<NtkDest> &&
             mockturtle::has_get_node_v<NtkSrc> && mockturtle::has_is_complemented_v<NtkSrc> &&
             mockturtle::has_foreach_pi_v<NtkSrc> && mockturtle::has_foreach_gate_v<NtkSrc> &&
             mockturtle::has_foreach_po_v<NtkSrc> && mockturtle::has_foreach_fanin_v<NtkSrc> &&
             mockturtle::has_get_constant_v<NtkSrc> && mockturtle::has_get_constant_v<NtkDest> &&
             mockturtle::has_create_pi_v<NtkDest> && mockturtle::has_create_po_v<NtkDest> &&
             mockturtle::has_create_not_v<NtkDest> && mockturtle::has_create_and_v<NtkDest> &&
             mockturtle::has_create_or_v<NtkDest> && mockturtle::has_create_xor_v<NtkDest> &&
             mockturtle::has_create_maj_v<NtkDest>
NtkDest convert_network(const NtkSrc& ntk)
{
    // TODO handle ci/ro/etc...

    assert(ntk.is_combinational() && "Network has to be combinational");

    detail::convert_network_impl<NtkDest, NtkSrc> p{ntk};

    auto result = p.run();

    return result;
}

}  // namespace fiction::synthesis
