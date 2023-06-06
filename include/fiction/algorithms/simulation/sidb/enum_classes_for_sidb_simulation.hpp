//
// Created by Jan Drewniok on 06.06.23.
//

#ifndef FICTION_ENUM_CLASSES_FOR_SIDB_SIMULATION_HPP
#define FICTION_ENUM_CLASSES_FOR_SIDB_SIMULATION_HPP

namespace fiction
{
/**
 * An enumeration of modes to use for the QuickExact algorithm.
 */
enum class automatic_base_number_detection
{
    /**
     * It automatically detects if a 2-state simulation is sufficient or if a 3-state simulation is required. It checks
     * if a positive charge could occur due to maximum band bending. If this mode is active, 3-state simulation could be
     * run even if base_num = 2 was set in the physical parameters.
     */
    ON,
    /**
     * The base number from the physical parameter is used for the simulation.
     */
    OFF
};

/**
 * An enumeration of exact algorithms for the tts-simulation.
 */
enum class exhaustive_algorithm
{
    /**
     * ExGS
     */
    EXGS,
    /**
     * QuickExact
     */
    QUICKEXACT
};

enum class dependent_cell_mode
{
    FIXED,
    VARIABLE
};

enum class energy_calculation
{
    KEEP_OLD_ENERGY_VALUE,
    UPDATE_ENERGY
};

enum class history
{
    CONSIDER,
    NEGLECT
};

}  // namespace fiction

#endif  // FICTION_ENUM_CLASSES_FOR_SIDB_SIMULATION_HPP
