//
// Created by marcel on 11.08.23.
//

#ifndef FICTION_SIDB_SIMULATION_ENGINE_HPP
#define FICTION_SIDB_SIMULATION_ENGINE_HPP

#include <phmap.h>

#include <algorithm>
#include <optional>
#include <string>
#include <type_traits>

namespace fiction
{
/**
 * Selector for the available SiDB simulation engines.
 */
enum class sidb_simulation_engine
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
    /**
     * *ClusterComplete* is a novel exact simulation engine that requires exponential runtime, though, depending on the
     * simulation problem, it effectively reduces the base number by a real number, thus allowing problem sizes that
     * were previously considered astronomical in size. Inherent to the simulation methodology that does not depend on
     * the simulation base, it simulates very effectively for either base number (2 or 3).
     */
    CLUSTERCOMPLETE
};
/**
 * Selector exclusively for exhaustive SiDB simulation engines.
 */
enum class exhaustive_sidb_simulation_engine
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
    /**
     * *ClusterComplete* is a novel exact simulation engine that requires exponential runtime, though, depending on the
     * simulation problem, it effectively reduces the base number by a real number, thus allowing problem sizes that
     * were previously considered astronomical in size. Inherent to the simulation methodology that does not depend on
     * the simulation base, it simulates very effectively for either base number (2 or 3).
     */
    CLUSTERCOMPLETE
};
/**
 * Selector exclusively for heuristic SiDB simulation engines.
 */
enum class heuristic_sidb_simulation_engine
{
    /**
     * *QuickSim* is a heuristic simulation engine that only requires polynomial runtime.
     */
    QUICKSIM
};

/**
 * Returns the name of the given simulation engine.
 *
 * @tparam EngineType The type of the SiDB simulation engine (exhaustive/heuristic/general).
 * @param engine An SiDB simulation engine.
 * @return The name of the simulation engine as a string.
 */
template <typename EngineType>
[[nodiscard]] inline std::string sidb_simulation_engine_name(const EngineType& engine) noexcept
{
    static_assert(std::is_same_v<EngineType, sidb_simulation_engine> ||
                      std::is_same_v<EngineType, exhaustive_sidb_simulation_engine> ||
                      std::is_same_v<EngineType, heuristic_sidb_simulation_engine>,
                  "EngineType must be a SiDB simulation engine type");

    if constexpr (std::is_same_v<EngineType, sidb_simulation_engine> ||
                  std::is_same_v<EngineType, exhaustive_sidb_simulation_engine>)
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
            case EngineType::CLUSTERCOMPLETE:
            {
                return "ClusterComplete";
            }
        }
    }

    if constexpr (std::is_same_v<EngineType, sidb_simulation_engine> ||
                  std::is_same_v<EngineType, heuristic_sidb_simulation_engine>)
    {
        switch (engine)
        {
            case EngineType::QUICKSIM:
            {
                return "QuickSim";
            }
        }
    }

    return "unsupported simulation engine";
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
    static const phmap::flat_hash_map<std::string, sidb_simulation_engine> engine_lookup{
        {"EXGS", sidb_simulation_engine::EXGS},
        {"QUICKEXACT", sidb_simulation_engine::QUICKEXACT},
        {"CLUSTERCOMPLETE", sidb_simulation_engine::CLUSTERCOMPLETE},
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
