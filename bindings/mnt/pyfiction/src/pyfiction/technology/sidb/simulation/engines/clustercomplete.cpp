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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/clustercomplete.hpp`.
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#if (FICTION_ALGLIB_ENABLED)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/clustercomplete.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void clustercomplete(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::simulation::engines::clustercomplete_params;

    /**
     * Report *Ground State Space* stats.
     */
    py::enum_<clustercomplete_params::ground_state_space_reporting>(
        m, "ground_state_space_reporting",
        DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting))
        .value("ON", clustercomplete_params::ground_state_space_reporting::ON,
               DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting_ON))
        .value("OFF", clustercomplete_params::ground_state_space_reporting::OFF,
               DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting_OFF));

    /**
     * ClusterComplete parameters.
     */
    py::class_<clustercomplete_params>(m, "clustercomplete_params",
                                       DOC(fiction_sidb_simulation_engines_clustercomplete_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &clustercomplete_params::sim_params,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_sim_params))
        .def_rw("local_external_potential", &clustercomplete_params::local_external_potential,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_local_external_potential))
        .def_rw("global_potential", &clustercomplete_params::global_potential,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_global_potential))
        .def_rw(
            "validity_witness_partitioning_max_cluster_size_gss",
            &clustercomplete_params::validity_witness_partitioning_max_cluster_size_gss,
            DOC(fiction_sidb_simulation_engines_clustercomplete_params_validity_witness_partitioning_max_cluster_size_gss))
        .def_rw("num_overlapping_witnesses_limit_gss", &clustercomplete_params::num_overlapping_witnesses_limit_gss,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_num_overlapping_witnesses_limit_gss))
        .def_rw("available_threads", &clustercomplete_params::available_threads,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_available_threads))
        .def_rw("report_gss_stats", &clustercomplete_params::report_gss_stats,
                DOC(fiction_sidb_simulation_engines_clustercomplete_params_report_gss_stats));

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::simulation::result (*const clustercomplete_pointer)(const fiction::sidb::layout&,
                                                                       const clustercomplete_params&) =
        &fiction::sidb::simulation::engines::clustercomplete;

    m.def("clustercomplete", clustercomplete_pointer, py::arg("lyt"), py::arg("params") = clustercomplete_params{},
          DOC(fiction_sidb_simulation_engines_clustercomplete));
}

}  // namespace pyfiction

#else  // FICTION_ALGLIB_ENABLED

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * Disable ClusterComplete.
 */
void clustercomplete([[maybe_unused]] nanobind::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_ALGLIB_ENABLED
