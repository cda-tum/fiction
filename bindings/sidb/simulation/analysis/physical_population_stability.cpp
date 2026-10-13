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

#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>

#include <optional>

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

namespace detail
{

/**
 * @brief Registers the population stability information and the analysis on `sidb_layout`.
 *
 * @param m The module.
 */
inline void register_physical_population_stability_functions(nb::module_& m)
{
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
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            return fiction::sidb::simulation::analysis::physical_population_stability(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("timeout") = physical_population_stability_options{}.timeout,
        nb::arg("base")      = physical_population_stability_options{}.sim_params.base,
        nb::arg("mu_minus")  = physical_population_stability_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r") = physical_population_stability_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf") = physical_population_stability_options{}.sim_params.lambda_tf,
        R"doc(Assesses the population stability of an SiDB layout: for every
physically valid charge distribution (found with *QuickExact*), it
determines the SiDB closest to each charge transition and the
potential change required for it, in ascending order of the
distributions' energies. The distance an SiDB would have to be placed
at to cause that potential change is reported as well.

Args:
    lyt: The layout to analyze.

Returns:
    The population stability information, one entry per distinct
    charge distribution.

Algorithm options are keyword-only; the signature lists their defaults.
)doc",
        nb::arg("precision_for_distance_corresponding_to_potential") =
            physical_population_stability_options{}.precision_for_distance_corresponding_to_potential);
}

}  // namespace detail

/**
 * @brief Binds physical population stability helpers.
 * @param m Python module.
 */
void register_physical_population_stability(nb::module_& m)
{
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
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::register_physical_population_stability_functions(m);
}

}  // namespace pyfiction
