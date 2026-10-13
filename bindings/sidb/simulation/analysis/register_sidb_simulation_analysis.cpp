/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.analysis` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `calculate_energy_and_state_type`.
 * @param m Python module.
 */
void bind_calculate_energy_and_state_type(nb::module_& m);
/**
 * @brief Binds `can_positive_charges_occur`.
 * @param m Python module.
 */
void bind_can_positive_charges_occur(nb::module_& m);
/**
 * @brief Binds `critical_temperature`.
 * @param m Python module.
 */
void bind_critical_temperature(nb::module_& m);
/**
 * @brief Binds `energy_distribution`.
 * @param m Python module.
 */
void bind_energy_distribution(nb::module_& m);
/**
 * @brief Binds `minimum_energy`.
 * @param m Python module.
 */
void bind_minimum_energy(nb::module_& m);
/**
 * @brief Binds `occupation_probability_of_excited_states`.
 * @param m Python module.
 */
void bind_occupation_probability_of_excited_states(nb::module_& m);
/**
 * @brief Binds `physical_population_stability`.
 * @param m Python module.
 */
void bind_physical_population_stability(nb::module_& m);
/**
 * @brief Binds `physically_valid_parameters`.
 * @param m Python module.
 */
void bind_physically_valid_parameters(nb::module_& m);
/**
 * @brief Binds `time_to_solution`.
 * @param m Python module.
 */
void bind_time_to_solution(nb::module_& m);

void register_sidb_simulation_analysis(nb::module_& m)
{
    bind_calculate_energy_and_state_type(m);
    bind_can_positive_charges_occur(m);
    bind_critical_temperature(m);
    bind_energy_distribution(m);
    bind_minimum_energy(m);
    bind_occupation_probability_of_excited_states(m);
    bind_physical_population_stability(m);
    bind_physically_valid_parameters(m);
    bind_time_to_solution(m);
}

}  // namespace pyfiction
