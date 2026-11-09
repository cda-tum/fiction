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

#include "fiction/layouts/gate_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/layout_utils.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/hash.hpp>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
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
 * @tparam PortType Port type of the gate library.
 */
template <typename PortType>
using surface_black_list =
    std::unordered_map<layouts::layout_base::coordinate,
                       std::unordered_map<kitty::dynamic_truth_table, std::vector<fcn::port_list<PortType>>,
                                          kitty::hash<kitty::dynamic_truth_table>>>;

/**
 * @brief Analyzes a defective SiDB surface for a gate-level layout: for every tile and every gate implementation of the
 * library, the gate's SiDBs are placed at the tile's position, and if any of them is affected by a defect of the
 * surface, the gate's ports are blacklisted for that tile and function. Placement can then avoid those gates.
 *
 * @tparam GateLibrary SiDB gate library with static implementation and position or direction port lookup.
 * @tparam Geometry Coordinate geometry of the gate-level layout.
 * @param gate_lyt The gate-level layout.
 * @param surface The defective surface.
 * @param charged_defect_spacing_overwrite Overrides the spacing charged defects keep SiDBs at.
 * @param neutral_defect_spacing_overwrite Overrides the spacing neutral defects keep SiDBs at.
 * @return The black list.
 * @throws std::out_of_range If a defect has an invalid basis or an affected gate has no port entry.
 * @throws std::overflow_error If an absolute gate cell exceeds the coordinate range.
 * @throws std::bad_alloc If allocation fails while preparing or building the black list.
 */
template <typename GateLibrary, typename Geometry>
    requires std::same_as<typename GateLibrary::layout, sidb::layout> &&
             requires {
                 { GateLibrary::get_functional_implementations() } -> std::same_as<typename GateLibrary::gate_functions>;
             } && (requires {
                 { GateLibrary::get_gate_ports() } ->
                     std::same_as<typename GateLibrary::template gate_ports<fcn::port_position>>;
             } || requires {
                 { GateLibrary::get_gate_ports() } ->
                     std::same_as<typename GateLibrary::template gate_ports<fcn::port_direction>>;
             })
[[nodiscard]] auto
surface_analysis(const fiction::layouts::gate_layout<Geometry>& gate_lyt, const sidb::layout& surface,
                 const std::optional<std::pair<uint16_t, uint16_t>>& charged_defect_spacing_overwrite = std::nullopt,
                 const std::optional<std::pair<uint16_t, uint16_t>>& neutral_defect_spacing_overwrite = std::nullopt)
{
    using port_type = typename decltype(GateLibrary::get_gate_ports())::mapped_type::value_type::port_type;

    surface_black_list<port_type> black_list{};

    /** @brief Pooled influence-site budget per defect. */
    constexpr uint64_t max_cached_sites_per_defect{64};
    /** @brief Maximum number of influence sites to cache. */
    constexpr uint64_t max_cached_sites{1'000'000};
    /** @brief Maximum number of influence-row entries to cache. */
    constexpr uint64_t max_cached_rows{65'536};
    /** @brief Defects in lattice order. */
    const auto& defects = surface.defects();
    /** @brief Remaining pooled influence-site budget. */
    auto remaining_cache_budget = std::min<uint64_t>(defects.size(), max_cached_sites / max_cached_sites_per_defect) *
                                  max_cached_sites_per_defect;

    /** @brief Whether every influence rectangle fits the site budget. */
    bool bounded_rectangles{true};
    for (const auto& [site, defect] : defects)
    {
        if (site.z != 0 && site.z != 1)
        {
            throw std::out_of_range("Invalid lattice basis index");
        }
        /** @brief Effective horizontal and vertical influence extents. */
        const auto extent =
            sidb::model::defect_extent(defect, charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite);

        /** @brief Influence width in lattice sites. */
        const auto width = (uint64_t{2} * extent.first) + 1;
        /** @brief Influence height in lattice sites. */
        const auto height = (uint64_t{2} * extent.second) + 1;
        /** @brief Number of sites in the unclipped influence rectangle. */
        const auto sites = width * height;
        if (sites > remaining_cache_budget)
        {
            bounded_rectangles = false;
        }
        else
        {
            remaining_cache_budget -= sites;
        }
    }
    // Bound total cached sites while allowing a small number of wider rectangles.
    /** @brief Materialized influence sites when the site budget permits. */
    const auto affected_sites = bounded_rectangles ?
                                    std::optional{surface.all_affected_sidbs(charged_defect_spacing_overwrite,
                                                                             neutral_defect_spacing_overwrite)} :
                                    std::nullopt;

    /** @brief A defect and its lattice site. */
    using entry_type = std::pair<sidb::lattice_site, sidb::model::defect>;
    /** @brief Inclusive horizontal influence interval. */
    using interval = std::pair<int64_t, int64_t>;
    /** @brief Cached horizontal intervals keyed by lattice row and basis. */
    std::unordered_map<sidb::lattice_site, std::vector<interval>> rows{};
    /** @brief Defects whose influence is checked by scanning. */
    std::vector<entry_type> uncached{};
    /** @brief Largest vertical extent among scanned defects. */
    uint16_t uncached_max_y{};
    /** @brief Remaining pooled influence-row budget. */
    auto row_budget =
        std::min<uint64_t>(defects.size(), max_cached_rows / max_cached_sites_per_defect) * max_cached_sites_per_defect;
    /** @brief Conservative rightmost gate-cell coordinate, including shifted rows. */
    const auto max_gate_x = (int64_t{gate_lyt.width()} * GateLibrary::gate_x_size()) + (GateLibrary::gate_x_size() / 2);
    // Bound row allocations and leave tall rectangles for the scan; dense uncached bands still cost O(QD).
    if (!bounded_rectangles)
    {
        for (const auto& [site, defect] : defects)
        {
            /** @brief Effective horizontal and vertical influence extents. */
            const auto [x, y] =
                sidb::model::defect_extent(defect, charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite);
            /** @brief First representable influenced row. */
            const auto lo = std::max<int64_t>(int64_t{site.y} - y, std::numeric_limits<int32_t>::min());
            /** @brief Last representable influenced row. */
            const auto hi = std::min<int64_t>(int64_t{site.y} + y, std::numeric_limits<int32_t>::max());
            /** @brief Number of influenced rows. */
            const auto count = static_cast<uint64_t>(hi - lo + 1);
            if (count <= max_cached_sites_per_defect && count <= row_budget)
            {
                row_budget -= count;
                for (auto row = lo; row <= hi; ++row)
                {
                    rows[sidb::lattice_site{0, static_cast<int32_t>(row), site.z}].emplace_back(int64_t{site.x} - x,
                                                                                                int64_t{site.x} + x);
                }
            }
            else
            {
                if ((int64_t{site.x} + x < 0) || (int64_t{site.x} - x > max_gate_x))
                {
                    continue;
                }
                uncached.emplace_back(site, defect);
                uncached_max_y = std::max(uncached_max_y, y);
            }
        }
        // Merge overlapping or adjacent intervals so one binary search tests each row.
        for (auto& [row, spans] : rows)
        {
            std::ranges::sort(spans);
            /** @brief Number of merged intervals. */
            std::size_t n{};
            for (const auto span : spans)
            {
                if (n && span.first <= spans.at(n - 1).second + 1)
                {
                    spans.at(n - 1).second = std::max(spans.at(n - 1).second, span.second);
                }
                else
                {
                    spans.at(n++) = span;
                }
            }
            spans.resize(n);
        }
    }
    /** @brief Checks whether any defect influences the lattice site. */
    const auto affected = [&](const sidb::lattice_site& site)
    {
        if (affected_sites.has_value())
        {
            return affected_sites->contains(site);
        }
        if (const auto found = rows.find(sidb::lattice_site{0, site.y, site.z}); found != rows.end())
        {
            /** @brief Merged influence intervals for the queried row. */
            const auto& spans = found->second;
            /** @brief First interval starting after the queried column. */
            const auto it = std::upper_bound(spans.begin(), spans.end(), int64_t{site.x},
                                             [](const auto x, const auto& span) { return x < span.first; });
            if (it != spans.begin() && std::prev(it)->second >= site.x)
            {
                return true;
            }
        }
        /** @brief First defect row that can influence the site. */
        const auto lower_y = int64_t{site.y} - uncached_max_y;
        /** @brief Last defect row that can influence the site. */
        const auto upper_y = int64_t{site.y} + uncached_max_y;
        /** @brief First uncached defect in the candidate row band. */
        auto it =
            std::ranges::lower_bound(uncached, lower_y, {}, [](const auto& entry) { return int64_t{entry.first.y}; });
        for (; it != uncached.cend() && int64_t{it->first.y} <= upper_y; ++it)
        {
            const auto& [defect_site, defect] = *it;
            if (defect_site.z != site.z)
            {
                continue;
            }
            /** @brief Effective horizontal and vertical influence extents. */
            const auto [horizontal, vertical] =
                sidb::model::defect_extent(defect, charged_defect_spacing_overwrite, neutral_defect_spacing_overwrite);
            /** @brief Horizontal displacement from the defect. */
            const auto dx = int64_t{site.x} - defect_site.x;
            /** @brief Vertical displacement from the defect. */
            const auto dy = int64_t{site.y} - defect_site.y;
            if (dx >= -int64_t{horizontal} && dx <= horizontal && dy >= -int64_t{vertical} && dy <= vertical)
            {
                return true;
            }
        }
        return false;
    };

    const auto gate_implementations = GateLibrary::get_functional_implementations();
    const auto gate_ports           = GateLibrary::get_gate_ports();

    const auto analyze_gate = [&](const auto& it, const auto& t)
    {
        const auto& [fun, impls] = it;

        for (const auto& gate : impls)
        {
            auto continue_with_next_gate = false;

            for (uint16_t y = 0u; y < GateLibrary::gate_y_size(); ++y)
            {
                for (uint16_t x = 0u; x < GateLibrary::gate_x_size(); ++x)
                {
                    if (const auto cell_type = gate[y][x]; cell_type != sidb::dot_tag::EMPTY)
                    {
                        const auto sidb_pos = sidb::to_lattice_site(
                            layouts::relative_to_absolute_cell_position<GateLibrary::gate_x_size(),
                                                                        GateLibrary::gate_y_size()>(
                                gate_lyt, t, layouts::layout_base::coordinate{x, y, t.z}));

                        if (affected(sidb_pos))
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
        [&](const auto& t)
        {
            for (const auto& impl : gate_implementations)
            {
                analyze_gate(impl, t);
            }
        });

    return black_list;
}

}  // namespace fiction::physical_design
