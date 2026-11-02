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
 * @brief Catch2 diagnostics for routing-objective coordinates and input indices.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <catch2/catch_tostring.hpp>

#include <fiction/physical_design/routing_utils.hpp>

#include <sstream>
#include <string>

/** @brief Catch2 value formatting. */
namespace Catch
{
/** @brief Formats a routing objective without discarding its logical input index. */
template <>
struct StringMaker<fiction::physical_design::routing_objective>
{
    /**
     * @brief Returns the source, target, and logical input index.
     * @param objective Routing objective to format.
     * @return Readable routing objective.
     */
    static std::string convert(const fiction::physical_design::routing_objective& objective)
    {
        /** @brief Stream for the objective's diagnostic fields. */
        std::ostringstream out{};
        out << "source=" << objective.source << ", target=" << objective.target
            << ", input_index=" << objective.input_index;
        return out.str();
    }
};
}  // namespace Catch
