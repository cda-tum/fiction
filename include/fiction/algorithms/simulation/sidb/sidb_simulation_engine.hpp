//
// Created by marcel on 11.08.23.
//

#ifndef FICTION_SIDB_SIMULATION_ENGINE_HPP
#define FICTION_SIDB_SIMULATION_ENGINE_HPP

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
    QUICKEXACT
};

/**
 * Selector exclusively for exact SiDB simulation engines.
 */
enum class exact_sidb_simulation_engine
{
    /**
     * *Exhaustive Ground State Search* (EXGS) is an exact simulation engine that always has exponential runtime.
     */
    EXGS,
    /**
     * *QuickExact* is also an exact simulation engine that requires exponential runtime, but it scales a lot better
     * than ExGS due to its effective search-space pruning.
     */
    QUICKEXACT
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_ENGINE_HPP
