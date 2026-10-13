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

namespace detail
{

/**
 * @brief Registers the population stability information and the analysis on `sidb_layout`.
 *
 * @param m The module.
 */
inline void bind_physical_population_stability_functions(nb::module_& m)
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

    m.def("physical_population_stability", &fiction::sidb::simulation::analysis::physical_population_stability,
          nb::arg("lyt"),
          nb::arg("params") = fiction::sidb::simulation::analysis::physical_population_stability_params{},
          DOC(fiction_sidb_simulation_analysis_physical_population_stability));
}

}  // namespace detail

/**
 * @brief Binds `physical_population_stability`.
 * @param m Python module.
 */
void bind_physical_population_stability(nb::module_& m)
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

    /**
     * Parameters.
     */
    nb::class_<fiction::sidb::simulation::analysis::physical_population_stability_params>(
        m, "physical_population_stability_params",
        DOC(fiction_sidb_simulation_analysis_physical_population_stability_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("timeout", &fiction::sidb::simulation::analysis::physical_population_stability_params::timeout,
                DOC(fiction_sidb_simulation_analysis_physical_population_stability_params_timeout))
        .def_rw("simulation_parameters",
                &fiction::sidb::simulation::analysis::physical_population_stability_params::sim_params,
                DOC(fiction_sidb_simulation_analysis_physical_population_stability_params))
        .def_rw(
            "precision_for_distance_corresponding_to_potential",
            &fiction::sidb::simulation::analysis::physical_population_stability_params::
                precision_for_distance_corresponding_to_potential,
            DOC(fiction_sidb_simulation_analysis_physical_population_stability_params_precision_for_distance_corresponding_to_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::bind_physical_population_stability_functions(m);
}

}  // namespace pyfiction
