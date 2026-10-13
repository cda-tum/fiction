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

void calculate_energy_and_state_type(nb::module_& m);
void can_positive_charges_occur(nb::module_& m);
void critical_temperature(nb::module_& m);
void energy_distribution(nb::module_& m);
void minimum_energy(nb::module_& m);
void occupation_probability_of_excited_states(nb::module_& m);
void physical_population_stability(nb::module_& m);
void physically_valid_parameters(nb::module_& m);
void time_to_solution(nb::module_& m);

void register_sidb_simulation_analysis(nb::module_& m)
{
    calculate_energy_and_state_type(m);
    can_positive_charges_occur(m);
    critical_temperature(m);
    energy_distribution(m);
    minimum_energy(m);
    occupation_probability_of_excited_states(m);
    physical_population_stability(m);
    physically_valid_parameters(m);
    time_to_solution(m);
}

}  // namespace pyfiction
