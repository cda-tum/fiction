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
    QUICKEXACT,
    /**
     * *ClusterComplete* is a novel exact simulation engine that requires sub-exponential runtime, for
     * simulation problems in practice like SiDB logic. It handles base 3 simulation very effectively, since the engine
     * makes no distinction in the simulation methodology for a different base number.
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
     * *ClusterComplete* is a novel exact simulation engine that requires sub-exponential exponential runtime, for
     * simulation problems in practice like SiDB logic. It handles base 3 simulation very effectively, since the engine
     * makes no distinction in the simulation methodology for a different base number.
     */
    CLUSTERCOMPLETE
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_ENGINE_HPP
