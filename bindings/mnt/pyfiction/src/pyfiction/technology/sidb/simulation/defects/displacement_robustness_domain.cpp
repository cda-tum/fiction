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
 * @brief Python bindings for `fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void displacement_robustness_domain(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::simulation::defects::displacement_robustness_domain;
    using fiction::sidb::simulation::defects::displacement_robustness_domain_params;
    using fiction::sidb::simulation::defects::displacement_robustness_domain_stats;

    py::enum_<displacement_robustness_domain_params::dimer_displacement_policy>(
        m, "dimer_displacement_policy",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy))
        .value(
            "STAY_ON_ORIGINAL_DIMER",
            displacement_robustness_domain_params::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy_STAY_ON_ORIGINAL_DIMER))
        .value(
            "ALLOW_OTHER_DIMER", displacement_robustness_domain_params::dimer_displacement_policy::ALLOW_OTHER_DIMER,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy_ALLOW_OTHER_DIMER));

    py::enum_<displacement_robustness_domain_params::displacement_analysis_mode>(
        m, "displacement_analysis_mode",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode))
        .value(
            "EXHAUSTIVE", displacement_robustness_domain_params::displacement_analysis_mode::EXHAUSTIVE,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode_EXHAUSTIVE))
        .value(
            "RANDOM", displacement_robustness_domain_params::displacement_analysis_mode::RANDOM,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode_RANDOM));

    py::class_<displacement_robustness_domain_params>(
        m, "displacement_robustness_domain_params",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("analysis_mode", &displacement_robustness_domain_params::analysis_mode,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_analysis_mode))
        .def_rw(
            "percentage_of_analyzed_displaced_layouts",
            &displacement_robustness_domain_params::percentage_of_analyzed_displaced_layouts,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_percentage_of_analyzed_displaced_layouts))
        .def_rw("displacement_variations", &displacement_robustness_domain_params::displacement_variations,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_variations))
        .def_rw("operational_params", &displacement_robustness_domain_params::operational_params,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_operational_params))
        .def_rw("fixed_sidbs", &displacement_robustness_domain_params::fixed_sidbs,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_fixed_sidbs))
        .def_rw("dimer_policy", &displacement_robustness_domain_params::dimer_policy,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_policy))
        .def_rw("number_of_threads", &displacement_robustness_domain_params::number_of_threads,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_number_of_threads));

    py::class_<displacement_robustness_domain_stats>(
        m, "displacement_robustness_domain_stats",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_stats))
        .def(py::init<>(), "Default constructor.")
        .def_rw("time_total", &displacement_robustness_domain_stats::time_total,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_stats_time_total))
        .def_rw(
            "num_operational_sidb_displacements",
            &displacement_robustness_domain_stats::num_operational_sidb_displacements,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_stats_num_operational_sidb_displacements))
        .def_rw(
            "num_non_operational_sidb_displacements",
            &displacement_robustness_domain_stats::num_non_operational_sidb_displacements,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_stats_num_non_operational_sidb_displacements));

    py::class_<displacement_robustness_domain>(m, "displacement_robustness_domain",
                                               DOC(fiction_sidb_simulation_defects_displacement_robustness_domain))
        .def(py::init<>(), "Default constructor.")
        .def_rw("influence_information", &displacement_robustness_domain::operational_values,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_operational_values));

    m.def(
        "determine_displacement_robustness_domain",
        [](const layout& lyt, const std::vector<py_tt>& spec, const displacement_robustness_domain_params& params,
           displacement_robustness_domain_stats* stats)
        {
            return fiction::sidb::simulation::defects::determine_displacement_robustness_domain(lyt, spec, params,
                                                                                                stats);
        },
        py::arg("layout"), py::arg("spec"), py::arg("params") = displacement_robustness_domain_params{},
        py::arg("stats") = nullptr, DOC(fiction_sidb_simulation_defects_determine_displacement_robustness_domain));
}

}  // namespace pyfiction
