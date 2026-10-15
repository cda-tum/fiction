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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using physical_population_stability_options = fiction::sidb::simulation::analysis::physical_population_stability_params;

/**
 * @brief Registers the population stability information and the analysis on `sidb_layout`.
 *
 * @param m The module.
 */
void register_physical_population_stability(nb::module_& m)
{
    // Keep the GIL for population-stability properties; the native analysis releases it.
    nb::enum_<fiction::sidb::simulation::analysis::transition_type>(
        m, "transition_type", DOC(fiction_sidb_simulation_analysis_transition_type))
        .value("NEUTRAL_TO_NEGATIVE", fiction::sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE,
               DOC(fiction_sidb_simulation_analysis_transition_type_NEUTRAL_TO_NEGATIVE))
        .value("NEGATIVE_TO_NEUTRAL", fiction::sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL,
               DOC(fiction_sidb_simulation_analysis_transition_type_NEGATIVE_TO_NEUTRAL))
        .value("NEUTRAL_TO_POSITIVE", fiction::sidb::simulation::analysis::transition_type::NEUTRAL_TO_POSITIVE,
               DOC(fiction_sidb_simulation_analysis_transition_type_NEUTRAL_TO_POSITIVE))
        .value("POSITIVE_TO_NEUTRAL", fiction::sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL,
               DOC(fiction_sidb_simulation_analysis_transition_type_POSITIVE_TO_NEUTRAL));

    using fiction::sidb::simulation::analysis::population_stability_information;

    nb::class_<population_stability_information>(m, "population_stability_information",
                                                 DOC(fiction_sidb_simulation_analysis_population_stability_information))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("critical_dot", &population_stability_information::critical_dot,
                DOC(fiction_sidb_simulation_analysis_population_stability_information_critical_dot))
        .def_rw("transition_potentials", &population_stability_information::transition_potentials,
                DOC(fiction_sidb_simulation_analysis_population_stability_information_transition_potentials))
        .def_rw(
            "distance_corresponding_to_potential",
            &population_stability_information::distance_corresponding_to_potential,
            DOC(fiction_sidb_simulation_analysis_population_stability_information_distance_corresponding_to_potential))
        .def_rw("system_energy", &population_stability_information::system_energy,
                DOC(fiction_sidb_simulation_analysis_population_stability_information_system_energy));

    m.def(
        "physical_population_stability",
        [](const fiction::sidb::layout& lyt, const decltype(physical_population_stability_options{}.timeout) timeout,
           const decltype(physical_population_stability_options{}.sim_params.base)      base,
           const decltype(physical_population_stability_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(physical_population_stability_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(physical_population_stability_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(physical_population_stability_options{}.precision_for_distance_corresponding_to_potential)
               precision_for_distance_corresponding_to_potential)
        {
            physical_population_stability_options params{};
            params.precision_for_distance_corresponding_to_potential =
                precision_for_distance_corresponding_to_potential;
            params.sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.timeout    = timeout;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::physical_population_stability(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("timeout") = physical_population_stability_options{}.timeout,
        nb::arg("base")      = physical_population_stability_options{}.sim_params.base,
        nb::arg("mu_minus")  = physical_population_stability_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r") = physical_population_stability_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf") = physical_population_stability_options{}.sim_params.lambda_tf,
        nb::arg("precision_for_distance_corresponding_to_potential") =
            physical_population_stability_options{}.precision_for_distance_corresponding_to_potential,
        R"doc(Determine the stability of the SiDB population under the physical model.
The keyword options select simulation, potential precision, and the execution budget.
Distance conversion bisects the distance bracket up to floating-point resolution.

Raises:
    ValueError: Invalid physical parameters or precision.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    precision_for_distance_corresponding_to_potential: Decimal places for distance conversion, from 0 to 15.
)doc");
}

}  // namespace pyfiction
