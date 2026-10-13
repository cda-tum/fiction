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
 * @brief Binds calculate energy and state type helpers.
 * @param m Python module.
 */
void register_calculate_energy_and_state_type(nb::module_& m);
/**
 * @brief Binds can positive charges occur helpers.
 * @param m Python module.
 */
void register_can_positive_charges_occur(nb::module_& m);
/**
 * @brief Binds critical temperature helpers.
 * @param m Python module.
 */
void register_critical_temperature(nb::module_& m);
/**
 * @brief Binds energy distribution helpers.
 * @param m Python module.
 */
void register_energy_distribution(nb::module_& m);
/**
 * @brief Binds minimum energy helpers.
 * @param m Python module.
 */
void register_minimum_energy(nb::module_& m);
/**
 * @brief Binds occupation probability of excited states helpers.
 * @param m Python module.
 */
void register_occupation_probability_of_excited_states(nb::module_& m);
/**
 * @brief Binds physical population stability helpers.
 * @param m Python module.
 */
void register_physical_population_stability(nb::module_& m);
/**
 * @brief Binds physically valid parameters helpers.
 * @param m Python module.
 */
void register_physically_valid_parameters(nb::module_& m);
/**
 * @brief Binds time to solution helpers.
 * @param m Python module.
 */
void register_time_to_solution(nb::module_& m);

void register_sidb_simulation_analysis(nb::module_& m)
{
    register_calculate_energy_and_state_type(m);
    register_can_positive_charges_occur(m);
    register_critical_temperature(m);
    register_energy_distribution(m);
    register_minimum_energy(m);
    register_occupation_probability_of_excited_states(m);
    register_physical_population_stability(m);
    register_physically_valid_parameters(m);
    register_time_to_solution(m);
}

}  // namespace pyfiction
