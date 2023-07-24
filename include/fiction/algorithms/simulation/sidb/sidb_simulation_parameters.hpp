//
// Created by Jan Drewniok on 24.11.22.
//

#ifndef FICTION_SIDB_SIMULATION_PARAMETERS_HPP
#define FICTION_SIDB_SIMULATION_PARAMETERS_HPP

#include "fiction/utils/units_utils.hpp"

#include <units.h>

#include <cassert>
#include <cstdint>

namespace fiction
{

/**
 * This struct collects all physical parameters for physical SiDB simulations. It can be useful to adjust them,
 * especially when experiments create new insights. However, the default values are commonly used.
 */
struct sidb_simulation_parameters
{
    /**
     * Default constructor.
     *
     * @param base_number simulation can be conducted with 2 and 3 charge states. 2 = (Negative, Neutral), 3 =
     * (Negative, Neutral, Positive).
     * @param mu_minus it is the energy transition level (0/-).
     * @param relative_permittivity it describes the electric field reduction due to polarization.
     * @param screening_distance also known as "Thomas-Fermi screening" and it describes the electric field screening
     * due to free charges in nm.
     * @param a lattice constant in Å (Ångström).
     * @param b lattice constant in Å.
     * @param c lattice constant in Å.
     */
    constexpr explicit sidb_simulation_parameters(const uint8_t                         base_number = 3,
                                                  const units::energy::electron_volt_t& mu_minus    = -0.32_eV,
                                                  const double                          relative_permittivity = 5.6,
                                                  const units::length::nanometer_t&     screening_distance    = 5.0_nm,
                                                  const units::length::angstrom_t&      a = 3.84_angstrom,
                                                  const units::length::angstrom_t&      b = 7.68_angstrom,
                                                  const units::length::angstrom_t&      c = 2.25_angstrom) :
            lat_a{a},
            lat_b{b},
            lat_c{c},
            epsilon_r{relative_permittivity},
            k{8.988 * 1E9 / epsilon_r},
            lambda_tf{screening_distance},
            mu{mu_minus},
            mu_p{mu - units::energy::electron_volt_t(0.59)},
            base{base_number}

    {
        assert((base == 2 || base == 3) && "base must be 2 or 3");
    }

    /**
     * lat_a is the lattice vector in x-direction (unit: Å).
     */
    units::length::angstrom_t lat_a;
    /**
     * lat_b is the lattice vector in y-direction (unit: Å).
     */
    units::length::angstrom_t lat_b;
    /**
     * lat_c is the dimer pair separation (unit: Å).
     */
    units::length::angstrom_t lat_c;
    /**
     * epsilon_r is the electric permittivity. It is a material specific number (unit-less).
     */
    double epsilon_r;
    /**
     * k is the Coulomb constant and is inversely proportional to the electric permittivity (unit: SI).
     */
    coulomb_constant_unit k;
    /**
     * lambda_tf is the Thomas-Fermi screening distance (unit: nm).
     */
    units::length::nanometer_t lambda_tf;
    /**
     * µ- is the energy transition level (0/-) (unit: eV).
     */
    units::energy::electron_volt_t mu;
    /**
     * µ+ is the energy transition level (+/0) (unit: eV).
     */
    units::energy::electron_volt_t mu_p;
    /**
     * base can be either 2 or 3 and describes the assumed number of charge states of one SiDB.
     * It often makes sense to assume only negatively and neutrally charged SiDBs.
     */
    uint8_t base;
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_PARAMETERS_HPP
