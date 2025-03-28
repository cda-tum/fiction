//
// Created by marcel on 11.08.23.
//

#ifndef FICTION_SIDB_SIMULATION_ENGINE_HPP
#define FICTION_SIDB_SIMULATION_ENGINE_HPP

#include <phmap.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace fiction
{

/**
 * Selector for the available SiDB simulation engines.
 */
enum class sidb_simulation_engine : uint8_t
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
enum class exact_sidb_simulation_engine : uint8_t
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
enum class heuristic_sidb_simulation_engine : uint8_t
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
[[nodiscard]] std::string_view sidb_simulation_engine_name(const EngineType& engine) noexcept
{
    static_assert(std::is_same_v<EngineType, sidb_simulation_engine> ||
                      std::is_same_v<EngineType, exact_sidb_simulation_engine> ||
                      std::is_same_v<EngineType, heuristic_sidb_simulation_engine>,
                  "EngineType must be a SiDB simulation engine type");

    // generic SiDB simulation engines
    if constexpr (std::is_same_v<EngineType, sidb_simulation_engine>)
    {
        switch (engine)
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
    if constexpr (std::is_same_v<EngineType, exact_sidb_simulation_engine>)
    {
        switch (engine)
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
    if constexpr (std::is_same_v<EngineType, heuristic_sidb_simulation_engine>)
    {
        switch (engine)
        {
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
}
/**
 * Returns a simulation engine by name.
 *
 * @param name Name of the desired SiDB simulation engine.
 * @return SiDB simulation engine that matches the given `name`, or `std::nullopt` if no simulation engine by the given
 * `name` exists.
 */
[[nodiscard]] inline std::optional<sidb_simulation_engine>
get_sidb_simulation_engine(const std::string_view& name) noexcept
{
#if (FICTION_ALGLIB_ENABLED)
    static const phmap::flat_hash_map<std::string, sidb_simulation_engine> engine_lookup{
        {"EXGS", sidb_simulation_engine::EXGS},
        {"QUICKEXACT", sidb_simulation_engine::QUICKEXACT},
        {"CLUSTERCOMPLETE", sidb_simulation_engine::CLUSTERCOMPLETE},
        {"QUICKSIM", sidb_simulation_engine::QUICKSIM}};
#else
    static const phmap::flat_hash_map<std::string, sidb_simulation_engine> engine_lookup{
        {"EXGS", sidb_simulation_engine::EXGS},
        {"QUICKEXACT", sidb_simulation_engine::QUICKEXACT},
        {"QUICKSIM", sidb_simulation_engine::QUICKSIM}};

    std::string upper_name = name.data();
    std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

    if (const auto it = engine_lookup.find(upper_name); it != engine_lookup.cend())
    {
        return it->second;
    }

    return std::nullopt;
}

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_ENGINE_HPP
