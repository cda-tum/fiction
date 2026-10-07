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
 * @brief Validates finished layouts for FGL version 2.
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/verification/design_rule_violations.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace fiction::layouts::io::detail::fgl
{

/**
 * @brief Reject XML 1.0 control characters in a name.
 * @param value Layout or object name.
 * @throws std::invalid_argument If the name contains an illegal control character.
 */
inline void validate_xml_text(const std::string& value)
{
    for (const auto character : value)
    {
        if (static_cast<unsigned char>(character) < 0x20 && character != '\t' && character != '\n' && character != '\r')
        {
            throw std::invalid_argument("FGL names require XML 1.0 text characters");
        }
    }
}

/**
 * @brief Return the FGL scheme name, including a three-phase suffix where needed.
 * @param scheme Clocking scheme.
 * @return Scheme name.
 */
[[nodiscard]] inline std::string clocking_name(const clocking::scheme& scheme)
{
    return scheme.name() + (scheme.num_clocks() == 3u && scheme.name() != layouts::clocking::BANCS_NAME ? "3" : "");
}

/**
 * @brief Validate every object and metadata entry for finished FGL version 2.
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to validate.
 * @throws std::invalid_argument If the layout is incomplete, cyclic, physically invalid, uses an unsupported
 * scheme, or contains illegal XML text controls.
 */
template <typename Lyt>
void validate_layout(const Lyt& lyt)
{
    validate_xml_text(lyt.get_layout_name());
    std::unordered_map<typename Lyt::object_id, uint32_t> remaining{};
    std::vector<typename Lyt::object_id>                  ready{};
    lyt.foreach_node(
        [&](const auto id)
        {
            validate_xml_text(lyt.get_name(id));
            remaining.emplace(id, lyt.input_count(id));
            for (uint32_t input = 0; input < lyt.input_count(id); ++input)
            {
                if (!lyt.source({id, input}))
                {
                    throw std::invalid_argument("FGL requires every declared input to be connected");
                }
            }
            if (lyt.input_count(id) == 0)
            {
                ready.push_back(id);
            }
        });
    for (std::size_t i = 0; i < ready.size(); ++i)
    {
        lyt.foreach_sink(lyt.output(ready[i]),
                         [&](const auto port)
                         {
                             if (--remaining.at(port.object) == 0)
                             {
                                 ready.push_back(port.object);
                             }
                         });
    }
    if (ready.size() != remaining.size())
    {
        throw std::invalid_argument("FGL requires an acyclic layout");
    }
    std::optional<layouts::arrangement> arrangement{};
    if constexpr (is_hexagonal_layout_v<Lyt>)
    {
        arrangement = lyt.get_arrangement();
    }
    /** @brief Borrowed clocking scheme used to validate the serialized base scheme. */
    const auto& source_scheme = lyt.get_clocking_scheme();
    auto        scheme        = layouts::clocking::get_scheme(clocking_name(source_scheme), arrangement);
    if (!scheme)
    {
        throw std::invalid_argument("FGL requires a supported named clocking scheme");
    }
    source_scheme.foreach_override([&](const auto x, const auto y, const auto number)
                                   { scheme->override_clock_number(x, y, number); });
    if (*scheme != source_scheme)
    {
        throw std::invalid_argument("FGL requires a supported named clocking base scheme");
    }
    source_scheme.foreach_override(
        [&lyt](const auto x, const auto y, const auto)
        {
            if (x < 0 || y < 0 || static_cast<uint64_t>(x) >= lyt.width() || static_cast<uint64_t>(y) >= lyt.height() ||
                lyt.layers() == 0)
            {
                throw std::invalid_argument("FGL requires clock overrides inside the extent");
            }
        });
    lyt.foreach_synchronization_element(
        [&lyt](const auto& coordinate, const auto)
        {
            if (!lyt.contains_coordinate(coordinate))
            {
                throw std::invalid_argument("FGL requires synchronization elements inside the extent");
            }
        });
    verification::gate_level_drv_params params{};
    params.missing_connections = false;
    params.has_io              = false;
    std::ostringstream report{};
    params.out = &report;
    verification::gate_level_drv_stats stats{};
    verification::gate_level_drvs(lyt, params, &stats);
    if (stats.drvs != 0)
    {
        throw std::invalid_argument("FGL requires a physically valid layout");
    }
}

}  // namespace fiction::layouts::io::detail::fgl
