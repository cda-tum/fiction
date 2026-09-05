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
 * @brief Black-lists the gate tiles a defective SiDB surface cannot host.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/layouts/layout_utils.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/hash.hpp>

#include <concepts>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::physical_design
{

/**
 * A black list of gates per tile and function: for each tile, the functions that cannot be implemented by the gates
 * with the listed ports. `surface_analysis` produces one from a defective SiDB surface, and `exact_with_blacklist`
 * consumes it.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam PortType Port type of the gate library.
 */
template <typename Lyt, typename PortType>
using surface_black_list =
    std::unordered_map<tile<Lyt>, std::unordered_map<kitty::dynamic_truth_table, std::vector<fcn::port_list<PortType>>,
                                                     kitty::hash<kitty::dynamic_truth_table>>>;

/**
 * Analyzes a defective SiDB surface for a gate-level layout: for every tile and every gate implementation of the
 * library, the gate's SiDBs are placed at the tile's position, and if any of them is affected by a defect of the
 * surface, the gate's ports are blacklisted for that tile and function. Placement can then avoid those gates.
 *
 * @tparam GateLibrary SiDB gate library type.
 * @tparam GateLyt Gate-level layout type.
 * @tparam CellLyt SiDB cell-level layout type that positions the gates; it has to use cube coordinates.
 * @param gate_lyt The gate-level layout.
 * @param surface The defective surface.
 * @param charged_defect_spacing_overwrite Overrides the spacing charged defects keep SiDBs at.
 * @param neutral_defect_spacing_overwrite Overrides the spacing neutral defects keep SiDBs at.
 * @return The black list.
 */
template <typename GateLibrary, typename GateLyt, typename CellLyt>
    requires std::same_as<fiction::technology<CellLyt>, sidb::sidb_technology> &&
             std::same_as<fiction::technology<CellLyt>, fiction::technology<GateLibrary>>
[[nodiscard]] auto surface_analysis(
    const GateLyt& gate_lyt, const sidb::layout& surface,
    const std::optional<std::pair<uint16_t, uint16_t>>& charged_defect_spacing_overwrite = std::nullopt,
    const std::optional<std::pair<uint16_t, uint16_t>>& neutral_defect_spacing_overwrite = std::nullopt) noexcept
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");
    static_assert(has_get_functional_implementations_v<GateLibrary>,
                  "GateLibrary does not implement the get_functional_implementations function");
    static_assert(has_get_gate_ports_v<GateLibrary>, "GateLibrary does not implement the get_gate_ports function");

    using port_type = typename decltype(GateLibrary::get_gate_ports())::mapped_type::value_type::port_type;

    surface_black_list<GateLyt, port_type> black_list{};

    const auto sidbs_affected_by_defects =
        surface.all_affected_sidbs(charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite);

    const auto gate_implementations = GateLibrary::get_functional_implementations();
    const auto gate_ports           = GateLibrary::get_gate_ports();

    const auto analyze_gate = [&](const auto& it, const auto& t) noexcept
    {
        const auto& [fun, impls] = it;

        for (const auto& gate : impls)
        {
            auto continue_with_next_gate = false;

            for (uint16_t y = 0u; y < GateLibrary::gate_y_size(); ++y)
            {
                for (uint16_t x = 0u; x < GateLibrary::gate_x_size(); ++x)
                {
                    if (const auto cell_type = gate[y][x]; cell_type != fiction::technology<CellLyt>::cell_type::EMPTY)
                    {
                        const cell<CellLyt> relative_cell_pos{x, y, t.z};

                        const auto sidb_pos = sidb::to_lattice_site(
                            layouts::relative_to_absolute_cell_position<GateLibrary::gate_x_size(),
                                                                        GateLibrary::gate_y_size(), GateLyt, CellLyt>(
                                gate_lyt, t, relative_cell_pos));

                        if (sidbs_affected_by_defects.contains(sidb_pos))
                        {
                            for (const auto& port : gate_ports.at(gate))
                            {
                                black_list[t][fun].push_back(port);
                            }

                            continue_with_next_gate = true;
                            break;
                        }
                    }
                }

                if (continue_with_next_gate)
                {
                    break;
                }
            }
        }
    };

    gate_lyt.foreach_tile(
        [&](const auto& t) noexcept
        {
            for (const auto& impl : gate_implementations)
            {
                analyze_gate(impl, t);
            }
        });

    return black_list;
}

}  // namespace fiction::physical_design
