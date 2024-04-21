//
// Created by marcel on 11.08.23.
//

#ifndef FICTION_SIDB_SIMULATION_ENGINE_HPP
#define FICTION_SIDB_SIMULATION_ENGINE_HPP

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
     * *ClusterComplete* is a novel exact simulation engine that requires sub-exponential runtime, for
     * simulation problems in practice like SiDB logic. It handles base 3 simulation very effectively, since the engine
     * makes no distinction in the simulation methodology for different base numbers.
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
     * *ClusterComplete* is a novel exact simulation engine that requires sub-exponential runtime, for simulation
     * problems in practice like SiDB logic. It handles base 3 simulation very effectively, since the engine makes no
     * distinction in the simulation methodology for a different base number.
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
 * @tparam EngineType The type of the SiDB engine (exhaustive/heuristic/general).
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

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_ENGINE_HPP
