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

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): enables keyword options

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

#if (FICTION_ALGLIB_ENABLED)

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/clustercomplete.hpp>

#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using clustercomplete_options = fiction::sidb::simulation::engines::clustercomplete_params;

/**
 * @brief Binds clustercomplete helpers.
 * @param m Python module.
 */
void register_clustercomplete(nb::module_& m)
{

    /**
     * Report *Ground State Space* stats.
     */
    nb::enum_<clustercomplete_options::ground_state_space_reporting>(
        m, "ground_state_space_reporting",
        DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting))
        .value("ON", clustercomplete_options::ground_state_space_reporting::ON,
               DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting_ON))
        .value("OFF", clustercomplete_options::ground_state_space_reporting::OFF,
               DOC(fiction_sidb_simulation_engines_clustercomplete_params_ground_state_space_reporting_OFF));

    m.def(
        "clustercomplete",
        [](const fiction::sidb::layout& lyt, const decltype(clustercomplete_options{}.sim_params.base) base,
           const decltype(clustercomplete_options{}.sim_params.mu_minus)                      mu_minus,
           const decltype(clustercomplete_options{}.sim_params.epsilon_r)                     epsilon_r,
           const decltype(clustercomplete_options{}.sim_params.lambda_tf)                     lambda_tf,
           const std::optional<decltype(clustercomplete_options{}.local_external_potential)>& local_external_potential,
           const decltype(clustercomplete_options{}.global_potential)                         global_potential,
           const decltype(clustercomplete_options{}
                              .num_overlapping_witnesses_limit_gss) num_overlapping_witnesses_limit_gss,
           const std::optional<decltype(clustercomplete_options{}.available_threads)> available_threads,
           const decltype(clustercomplete_options{}.report_gss_stats)                 report_gss_stats,
           const decltype(clustercomplete_options{}.on_progress)&                     on_progress,
           const decltype(clustercomplete_options{}.on_worker_progress)&              on_worker_progress,
           const decltype(clustercomplete_options{}.validity_witness_partitioning_max_cluster_size_gss)
               validity_witness_partitioning_max_cluster_size_gss)
        {
            clustercomplete_options params{};
            params.validity_witness_partitioning_max_cluster_size_gss =
                validity_witness_partitioning_max_cluster_size_gss;
            params.sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            if (local_external_potential)
            {
                params.local_external_potential = *local_external_potential;
            }
            params.global_potential                    = global_potential;
            params.num_overlapping_witnesses_limit_gss = num_overlapping_witnesses_limit_gss;
            if (available_threads)
            {
                params.available_threads = *available_threads;
            }
            params.report_gss_stats   = report_gss_stats;
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::engines::clustercomplete(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = clustercomplete_options{}.sim_params.base,
        nb::arg("mu_minus")                            = clustercomplete_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")                           = clustercomplete_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")                           = clustercomplete_options{}.sim_params.lambda_tf,
        nb::arg("local_external_potential")            = nb::none(),
        nb::arg("global_potential")                    = clustercomplete_options{}.global_potential,
        nb::arg("num_overlapping_witnesses_limit_gss") = clustercomplete_options{}.num_overlapping_witnesses_limit_gss,
        nb::arg("available_threads")                   = nb::none(),
        nb::arg("report_gss_stats")                    = clustercomplete_options{}.report_gss_stats,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("validity_witness_partitioning_max_cluster_size_gss") =
            clustercomplete_options{}.validity_witness_partitioning_max_cluster_size_gss,
        R"doc(Simulate the SiDB layout with ClusterComplete.
The keyword options select physical values, clustering, witness searches, and available threads.
Raises ValueError when base is neither 2 nor 3.
)doc");
}

}  // namespace pyfiction

#else  // FICTION_ALGLIB_ENABLED

namespace pyfiction
{

/**
 * @brief Disable ClusterComplete.
 * @param m Python module.
 */
void register_clustercomplete([[maybe_unused]] nb::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_ALGLIB_ENABLED
