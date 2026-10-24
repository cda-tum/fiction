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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/operational_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp>

#include <kitty/dynamic_truth_table.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner): converts progress callbacks
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using operational_domain_ratio_options = fiction::sidb::simulation::logic::operational_domain_ratio_params;

/**
 * @brief Binds operational domain ratio helpers.
 * @param m Python module.
 */
void register_operational_domain_ratio(nb::module_& m)
{
    using fiction::sidb::layout;

    using fiction::sidb::simulation::logic::parameter_point;

    m.def(
        "operational_domain_ratio",
        [](const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const parameter_point& pp,
           const decltype(operational_options{}.timeout)              timeout,
           const decltype(operational_options{}.sim_params.base)      base,
           const decltype(operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(operational_options{}.sim_engine)           sim_engine,
           const decltype(operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance)
                                                                                            threshold_bdl_interdistance,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                            minimum_distance,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                            maximum_distance,
           const decltype(operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(operational_options{}.op_condition)                               op_condition,
           const decltype(operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_ratio_options{}.op_domain_params.sweep_dimensions)>&
               sweep_dimensions,
           const std::optional<decltype(operational_domain_ratio_options{}.op_domain_params.number_of_threads)>
                                                                                                   number_of_threads,
           const decltype(operational_domain_ratio_options{}.op_domain_params.on_progress)&        on_progress,
           const decltype(operational_domain_ratio_options{}.op_domain_params.on_worker_progress)& on_worker_progress)
        {
            operational_domain_ratio_options params{};
            params.op_domain_params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);
            if (sweep_dimensions)
            {
                params.op_domain_params.sweep_dimensions = *sweep_dimensions;
            }
            if (number_of_threads)
            {
                params.op_domain_params.number_of_threads = *number_of_threads;
            }
            params.op_domain_params.on_progress        = on_progress;
            params.op_domain_params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::operational_domain_ratio(lyt, spec, pp, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("pp"), nb::kw_only(),
        nb::arg("timeout") = operational_domain_ratio_options{}.op_domain_params.operational_params.timeout,
        nb::arg("base")    = operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.base,
        nb::arg("mu_minus") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = operational_domain_ratio_options{}.op_domain_params.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") = operational_domain_ratio_options{}
                                                     .op_domain_params.operational_params.input_bdl_iterator_params
                                                     .bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") = operational_domain_ratio_options{}
                                          .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                                          .bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") = operational_domain_ratio_options{}
                                          .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                                          .bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            operational_domain_ratio_options{}
                .op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = operational_domain_ratio_options{}.op_domain_params.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            operational_domain_ratio_options{}
                .op_domain_params.operational_params.strategy_to_analyze_operational_status,
        nb::arg("sweep_dimensions") = nb::none(), nb::arg("number_of_threads") = nb::none(),
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        R"doc(Compute the fraction of operational parameter points surrounding the given point.
The keyword options select the sweep, sampling, physical values, operational analysis, and progress.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
}

}  // namespace pyfiction
