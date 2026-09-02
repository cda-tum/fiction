/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 11.08.23.
//

#pragma once

#include <phmap.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace fiction::sidb::simulation
{

/**
 * Selector for the available SiDB simulation engines.
 */
enum class engine : uint8_t
{
    /**
     * *Exhaustive Ground State Search (EXGS)* is an exact simulation engine that always has exponential runtime.
     */
    EXGS,
    /**
     * *QuickSim* is a heuristic simulation engine that only requires polynomial runtime.
     */
    QUICKSIM,
    /**
     * *QuickExact* is also an exact simulation engine that requires exponential runtime, but it scales a lot better
     * than *ExGS* due to its effective search-space pruning.
     */
    QUICKEXACT,
#if (FICTION_ALGLIB_ENABLED)
    /**
     * *ClusterComplete* is a novel exact simulation engine that requires exponential runtime, though, depending on the
     * simulation problem, it effectively reduces the base number by a real number, thus allowing problem sizes that
     * were previously considered astronomical in size. Inherent to the simulation methodology that does not depend on
     * the simulation base, it simulates very effectively for either base number (2 or 3).
     */
    CLUSTERCOMPLETE
#endif  // FICTION_ALGLIB_ENABLED
};
/**
 * Selector exclusively for exact SiDB simulation engines.
 */
enum class exact_engine : uint8_t
{
    /**
     * *Exhaustive Ground State Search* (EXGS) is an exact simulation engine that always has exponential runtime.
     */
    EXGS,
    /**
     * *QuickExact* is also an exact simulation engine that requires exponential runtime, but it scales a lot better
     * than ExGS due to its effective search-space pruning.
     */
    QUICKEXACT,
#if (FICTION_ALGLIB_ENABLED)
    /**
     * *ClusterComplete* is a novel exact simulation engine that requires exponential runtime, though, depending on the
     * simulation problem, it effectively reduces the base number by a real number, thus allowing problem sizes that
     * were previously considered astronomical. Inherent to the simulation methodology that does not depend on
     * the simulation base, it simulates very effectively for either base number (2 or 3).
     */
    CLUSTERCOMPLETE
#endif  // FICTION_ALGLIB_ENABLED
};
/**
 * Selector exclusively for heuristic SiDB simulation engines.
 */
enum class heuristic_engine : uint8_t
{
    /**
     * *QuickSim* is a heuristic simulation engine that only requires polynomial runtime.
     */
    QUICKSIM
};
/**
 * Returns the name of the given simulation engine.
 *
 * @tparam EngineType The type of the SiDB simulation engine (exhaustive/heuristic/generic).
 * @param engine An SiDB simulation engine.
 * @return The name of the simulation engine.
 */
template <typename EngineType>
[[nodiscard]] std::string_view engine_name(const EngineType& eng) noexcept
{
    static_assert(std::is_same_v<EngineType, engine> || std::is_same_v<EngineType, exact_engine> ||
                      std::is_same_v<EngineType, heuristic_engine>,
                  "EngineType must be a SiDB simulation engine type");

    // generic SiDB simulation engines
    if constexpr (std::is_same_v<EngineType, engine>)
    {
        switch (eng)
        {
            case EngineType::EXGS:
            {
                return "ExGS";
            }
            case EngineType::QUICKEXACT:
            {
                return "QuickExact";
            }
#if (FICTION_ALGLIB_ENABLED)
            case EngineType::CLUSTERCOMPLETE:
            {
                return "ClusterComplete";
            }
#endif  // FICTION_ALGLIB_ENABLED
            case EngineType::QUICKSIM:
            {
                return "QuickSim";
            }
            default:
            {
                return "unsupported simulation engine";
            }
        }
    }

    // exact SiDB simulation engines
    if constexpr (std::is_same_v<EngineType, exact_engine>)
    {
        switch (eng)
        {
            case EngineType::EXGS:
            {
                return "ExGS";
            }
            case EngineType::QUICKEXACT:
            {
                return "QuickExact";
            }
#if (FICTION_ALGLIB_ENABLED)
            case EngineType::CLUSTERCOMPLETE:
            {
                return "ClusterComplete";
            }
#endif  // FICTION_ALGLIB_ENABLED
            default:
            {
                return "unsupported simulation engine";
            }
        }
    }

    // heuristic SiDB simulation engines
    if constexpr (std::is_same_v<EngineType, heuristic_engine>)
    {
        if (eng == EngineType::QUICKSIM)
        {
            return "QuickSim";
        }

        return "unsupported simulation engine";
    }
}
/**
 * Returns a simulation engine by name.
 *
 * @param name Name of the desired SiDB simulation engine.
 * @return SiDB simulation engine that matches the given `name`, or `std::nullopt` if no simulation engine by the given
 * `name` exists.
 */
[[nodiscard]] inline std::optional<engine> get_engine(const std::string_view& name) noexcept
{
    static const phmap::flat_hash_map<std::string, engine> engine_lookup{{"EXGS", engine::EXGS},
                                                                         {"QUICKEXACT", engine::QUICKEXACT},
#if (FICTION_ALGLIB_ENABLED)
                                                                         {"CLUSTERCOMPLETE", engine::CLUSTERCOMPLETE},
#endif  // FICTION_ALGLIB_ENABLED
                                                                         {"QUICKSIM", engine::QUICKSIM}};

    std::string upper_name{name};
    std::ranges::transform(upper_name, upper_name.begin(), ::toupper);

    if (const auto it = engine_lookup.find(upper_name); it != engine_lookup.cend())
    {
        return it->second;
    }

    return std::nullopt;
}

}  // namespace fiction::sidb::simulation
