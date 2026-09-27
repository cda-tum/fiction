# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical analyses of SiDB layouts built on simulation."""

from __future__ import annotations

from mnt.pyfiction._native.sidb.simulation.analysis import (
    calculate_energy_and_state_type_with_kinks_accepted,
    calculate_energy_and_state_type_with_kinks_rejected,
    calculate_energy_distribution,
    can_positive_charges_occur,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_params,
    critical_temperature_stats,
    energy_distribution,
    energy_state,
    minimum_energy,
    occupation_probability_gate_based,
    occupation_probability_non_gate_based,
    physical_population_stability,
    physical_population_stability_params,
    physically_valid_parameters,
    physically_valid_parameters_domain,
    population_stability_information,
    state_type,
    time_to_solution,
    time_to_solution_for_given_simulation_results,
    time_to_solution_params,
    time_to_solution_stats,
    transition_type,
)

__all__ = [
    "calculate_energy_and_state_type_with_kinks_accepted",
    "calculate_energy_and_state_type_with_kinks_rejected",
    "calculate_energy_distribution",
    "can_positive_charges_occur",
    "critical_temperature_gate_based",
    "critical_temperature_non_gate_based",
    "critical_temperature_params",
    "critical_temperature_stats",
    "energy_distribution",
    "energy_state",
    "minimum_energy",
    "occupation_probability_gate_based",
    "occupation_probability_non_gate_based",
    "physical_population_stability",
    "physical_population_stability_params",
    "physically_valid_parameters",
    "physically_valid_parameters_domain",
    "population_stability_information",
    "state_type",
    "time_to_solution",
    "time_to_solution_for_given_simulation_results",
    "time_to_solution_params",
    "time_to_solution_stats",
    "transition_type",
]
