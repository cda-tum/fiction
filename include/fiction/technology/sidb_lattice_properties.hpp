//
// Created by Jan Drewniok on 13.02.24.
//

#ifndef FICTION_SIDB_LATTICE_PROPERTIES_HPP
#define FICTION_SIDB_LATTICE_PROPERTIES_HPP

#include <utility>

namespace fiction
{

/**
 * Selector for the available H-Si lattice orientations
 */
enum class si_lattice_orientations
{
    /**
     * H-Si in 100 direction.
     */
    SI_100,
    /**
     * H-Si in 111 direction.
     */
    SI_111,
};

struct si_lattice_constants
{
    /**
     * lat_a_100 is the lattice vector length in x-direction of Si-100 (unit: Å).
     */
    double lat_a_100 = 3.84;
    /**
     * lat_b_100 is the lattice vector length in y-direction of Si-100 (unit: Å).
     */
    double lat_b_100 = 7.68;
    /**
     * lat_c_100 is the base vector of Si-100 (unit: Å).
     */
    std::pair<double, double> lat_c_100 = {0, 2.25};
    /**
     * lat_a_111 is the lattice vector length in x-direction of Si-111 (unit: Å).
     */
    double lat_a_111 = 6.65;
    /**
     * lat_b_111 is the lattice vector length in y-direction of Si-111 (unit: Å).
     */
    double lat_b_111 = 3.84;
    /**
     * lat_c_111 is the base vector of Si-100 (unit: Å).
     */
    std::pair<double, double> lat_c_111 = {3.3255, 1.92};
};

}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_PROPERTIES_HPP
