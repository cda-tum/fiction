#ifndef FICTION_SIDB_LATTICE_TYPES_HPP
#define FICTION_SIDB_LATTICE_TYPES_HPP

#include <utility>

namespace fiction
{
/**
 * H-Si 100 surface.
 */
struct sidb_100_lattice
{
    /**
     * lat_a_100 is the lattice vector length in x-direction of Si-100 (unit: Å).
     */
    static constexpr double LAT_A = 3.84;
    /**
     * lat_b_100 is the lattice vector length in y-direction of Si-100 (unit: Å).
     */
    static constexpr double LAT_B = 7.68;
    /**
     * lat_c_100 is the base vector of Si-100 (unit: Å).
     */
    static constexpr std::pair<double, double> LAT_C = {0, 2.25};
};

struct sidb_111_lattice
{
    /**
     * lat_a_111 is the lattice vector length in x-direction of Si-111 (unit: Å).
     */
    static constexpr double LAT_A = 6.65;
    /**
     * lat_b_111 is the lattice vector length in y-direction of Si-111 (unit: Å).
     */
    static constexpr double LAT_B = 3.84;
    /**
     * lat_c_111 is the base vector of Si-100 (unit: Å).
     */
    static constexpr std::pair<double, double> LAT_C = {3.3255, 1.92};
};

}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_TYPES_HPP
