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
 * @brief Extracts declared layout logic into a named LUT network.
 */

#pragma once

#include <mockturtle/networks/klut.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace fiction::networks
{

/**
 * @brief Extracts the logic required by the layout's primary outputs.
 *
 * Every primary input appears in declared interface order, including unused inputs. Output order, terminal names, and
 * truth-table input indices are preserved. Wires and primary output objects forward their input. Dangling objects do
 * not enter the network. Placement, adjacency, and clocking do not affect extraction. Traversal uses an explicit stack.
 * @tparam Lyt Placed object layout with ordered input ports and declared primary interfaces.
 * @param lyt Layout to extract without mutation.
 * @return Independent named LUT network.
 * @throws std::invalid_argument If an output dependency has a missing input or a cycle.
 */
template <typename Lyt>
[[nodiscard]] mockturtle::names_view<mockturtle::klut_network> extract_layout_network(const Lyt& lyt)
{
    /** @brief Stable identity of an object in the source layout. */
    using object_id = typename Lyt::object_id;
    /** @brief Signal in the extracted network. */
    using signal = mockturtle::klut_network::signal;
    /** @brief Traversal position in an object's ordered input ports. */
    struct frame
    {
        /** @brief Object under traversal. */
        object_id object{};
        /** @brief Next input index to visit. */
        uint32_t input{};
    };

    mockturtle::names_view<mockturtle::klut_network> ntk{};
    ntk.set_network_name(lyt.get_layout_name());
    // An empty optional marks an active dependency; completed dependencies hold their extracted signal.
    std::unordered_map<object_id, std::optional<signal>> signals{};
    lyt.foreach_pi([&](const auto id) { signals.emplace(id, ntk.create_pi(lyt.get_name(id))); });
    std::vector<frame>  stack{};
    std::vector<signal> children{};
    lyt.foreach_po(
        [&](const auto po)
        {
            if (!signals.contains(po))
            {
                signals.emplace(po, std::nullopt);
                stack.push_back({po, 0});
            }
            while (!stack.empty())
            {
                auto& current = stack.back();
                if (current.input < lyt.input_count(current.object))
                {
                    const auto source = lyt.source({current.object, current.input++});
                    if (!source)
                    {
                        throw std::invalid_argument("A primary output dependency has a disconnected input");
                    }
                    const auto [entry, inserted] = signals.try_emplace(*source, std::nullopt);
                    if (inserted)
                    {
                        stack.push_back({*source, 0});
                    }
                    else if (!entry->second)
                    {
                        throw std::invalid_argument("A primary output dependency contains a cycle");
                    }
                    continue;
                }
                children.clear();
                for (uint32_t input{}; input < lyt.input_count(current.object); ++input)
                {
                    children.push_back(*signals.at(*lyt.source({current.object, input})));
                }
                const auto result          = lyt.is_buf(current.object) || lyt.is_po(current.object) ?
                                                 children.at(0) :
                                                 ntk.create_node(children, lyt.object_function(current.object));
                signals.at(current.object) = result;
                stack.pop_back();
            }
            ntk.create_po(*signals.at(po), lyt.get_name(po));
        });
    return ntk;
}

}  // namespace fiction::networks
