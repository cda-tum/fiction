#ifndef FICTION_TECHNOLOGY_SIDB_PRIMITIVES_LATTICE_ORIENTATIONS_HPP
#define FICTION_TECHNOLOGY_SIDB_PRIMITIVES_LATTICE_ORIENTATIONS_HPP

#include <utility>

namespace fiction::sidb::primitives
{
/**
 * H-Si(100)-2x1 surface.
 */
struct lattice_100
{
    /**
     * The lattice vector length in x-direction (unit: Å).
     */
    static constexpr double LAT_A = 3.84;
    /**
     * The lattice vector length in y-direction (unit: Å).
     */
    static constexpr double LAT_B = 7.68;
    /**
     * The base vector (unit: Å).
     */
    static constexpr std::pair<double, double> LAT_C = {0, 2.25};

    // Deleted constructors to prevent instantiation
    lattice_100() = delete;
};
/**
 * H-Si(111)-1x1 surface.
 */
struct lattice_111
{
    /**
     * The lattice vector length in x-direction (unit: Å).
     */
    static constexpr double LAT_A = 6.65;
    /**
     * The lattice vector length in y-direction (unit: Å).
     */
    static constexpr double LAT_B = 3.84;
    /**
     * The base vector (unit: Å).
     */
    static constexpr std::pair<double, double> LAT_C = {3.3255, 1.92};

    // Deleted constructors to prevent instantiation
    lattice_111() = delete;
};

}  // namespace fiction::sidb::primitives
#endif  // FICTION_TECHNOLOGY_SIDB_PRIMITIVES_LATTICE_ORIENTATIONS_HPP
