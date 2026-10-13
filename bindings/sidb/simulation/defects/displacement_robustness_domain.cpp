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

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using displacement_robustness_domain_options =
    fiction::sidb::simulation::defects::displacement_robustness_domain_params;

/**
 * @brief Binds displacement robustness domain helpers.
 * @param m Python module.
 */
void register_displacement_robustness_domain(nb::module_& m)
{
    using fiction::sidb::layout;
    using fiction::sidb::simulation::defects::displacement_robustness_domain;

    using fiction::sidb::simulation::defects::displacement_robustness_domain_stats;

    nb::enum_<displacement_robustness_domain_options::dimer_displacement_policy>(
        m, "dimer_displacement_policy",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy))
        .value(
            "STAY_ON_ORIGINAL_DIMER",
            displacement_robustness_domain_options::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy_STAY_ON_ORIGINAL_DIMER))
        .value(
            "ALLOW_OTHER_DIMER", displacement_robustness_domain_options::dimer_displacement_policy::ALLOW_OTHER_DIMER,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_dimer_displacement_policy_ALLOW_OTHER_DIMER));

    nb::enum_<displacement_robustness_domain_options::displacement_analysis_mode>(
        m, "displacement_analysis_mode",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode))
        .value(
            "EXHAUSTIVE", displacement_robustness_domain_options::displacement_analysis_mode::EXHAUSTIVE,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode_EXHAUSTIVE))
        .value(
            "RANDOM", displacement_robustness_domain_options::displacement_analysis_mode::RANDOM,
            DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_params_displacement_analysis_mode_RANDOM));

    nb::class_<displacement_robustness_domain_stats>(
        m, "displacement_robustness_domain_stats",
        DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_stats))
        .def(nb::init<>(), "Default constructor.")
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

    nb::class_<displacement_robustness_domain>(m, "displacement_robustness_domain",
                                               DOC(fiction_sidb_simulation_defects_displacement_robustness_domain))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("influence_information", &displacement_robustness_domain::operational_values,
                DOC(fiction_sidb_simulation_defects_displacement_robustness_domain_operational_values));

    m.def(
        "determine_displacement_robustness_domain",
        [](const layout& lyt, const std::vector<py_tt>& spec,
           const decltype(displacement_robustness_domain_options{}.analysis_mode)              analysis_mode,
           const decltype(displacement_robustness_domain_options{}.displacement_variations)    displacement_variations,
           const decltype(displacement_robustness_domain_options{}.operational_params.timeout) timeout,
           const decltype(displacement_robustness_domain_options{}.operational_params.sim_params.base)      base,
           const decltype(displacement_robustness_domain_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(displacement_robustness_domain_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(displacement_robustness_domain_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(displacement_robustness_domain_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(displacement_robustness_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance) threshold_bdl_interdistance,
           const decltype(displacement_robustness_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)            minimum_distance,
           const decltype(displacement_robustness_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)            maximum_distance,
           const decltype(displacement_robustness_domain_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config)       input_bdl_config,
           const decltype(displacement_robustness_domain_options{}.operational_params.op_condition) op_condition,
           const decltype(displacement_robustness_domain_options{}
                              .operational_params
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(displacement_robustness_domain_options{}.fixed_sidbs)>       fixed_sidbs,
           const decltype(displacement_robustness_domain_options{}.dimer_policy)                     dimer_policy,
           const std::optional<decltype(displacement_robustness_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(displacement_robustness_domain_options{}.on_progress)                      on_progress,
           displacement_robustness_domain_stats*                                                     stats,
           const decltype(displacement_robustness_domain_options{}
                              .percentage_of_analyzed_displaced_layouts) percentage_of_analyzed_displaced_layouts)
        {
            displacement_robustness_domain_options params{};
            params.percentage_of_analyzed_displaced_layouts = percentage_of_analyzed_displaced_layouts;
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.operational_params.sim_params =
                fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
            params.analysis_mode                 = analysis_mode;
            params.displacement_variations       = displacement_variations;
            params.operational_params.timeout    = timeout;
            params.operational_params.sim_engine = sim_engine;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance =
                threshold_bdl_interdistance;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance =
                minimum_distance;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance =
                maximum_distance;
            params.operational_params.input_bdl_iterator_params.input_bdl_config = input_bdl_config;
            params.operational_params.op_condition                               = op_condition;
            params.operational_params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            if (fixed_sidbs)
            {
                params.fixed_sidbs = *fixed_sidbs;
            }
            params.dimer_policy = dimer_policy;
            if (number_of_threads)
            {
                params.number_of_threads = *number_of_threads;
            }
            params.on_progress = on_progress;
            const nb::gil_scoped_release release{};

            return fiction::sidb::simulation::defects::determine_displacement_robustness_domain(lyt, spec, params,
                                                                                                stats);
        },
        nb::arg("layout"), nb::arg("spec"), nb::kw_only(),
        nb::arg("analysis_mode")           = displacement_robustness_domain_options{}.analysis_mode,
        nb::arg("displacement_variations") = displacement_robustness_domain_options{}.displacement_variations,
        nb::arg("timeout")                 = displacement_robustness_domain_options{}.operational_params.timeout,
        nb::arg("base")       = displacement_robustness_domain_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = displacement_robustness_domain_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = displacement_robustness_domain_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = displacement_robustness_domain_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = displacement_robustness_domain_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            displacement_robustness_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            displacement_robustness_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            displacement_robustness_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            displacement_robustness_domain_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = displacement_robustness_domain_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            displacement_robustness_domain_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("fixed_sidbs")       = nb::none(),
        nb::arg("dimer_policy")      = displacement_robustness_domain_options{}.dimer_policy,
        nb::arg("number_of_threads") = nb::none(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("stats") = nullptr, R"doc(During fabrication, SiDBs may not align precisely with their intended
atomic positions, resulting in displacement. This means that an SiDB
is fabricated close to the desired one, typically one or a few H-Si
positions away. Consequently, depending on the fabrication speed, a
certain number of SiDBs may experience displacement.

This function determines the operational status of all possible
displacements of the SiDBs of the given SiDB layout, based on the
provided truth table specification and displacement robustness
computation parameters. The number of displacements grows
exponentially with the number of SiDBs. For small layouts, all
displacements can be analyzed. For larger layouts, random sampling can
be applied, controllable by the `analysis_mode` and
`percentage_of_analyzed_displaced_layouts` in the keyword options.

Displaced layouts retain the layout name and the defects at their
original positions.

Args:
    lyt: The SiDB layout which is analyzed.
    spec: Vector of truth table specifications.
    stats: Statistics related to the displacement robustness
           computation.

Returns:
    The displacement robustness domain of the SiDB layout.

Raises:
    std::out_of_range: if a displacement exceeds the lattice-site
                       range.

Algorithm options are keyword-only; the signature lists their defaults.
)doc",
        nb::arg("percentage_of_analyzed_displaced_layouts") =
            displacement_robustness_domain_options{}.percentage_of_analyzed_displaced_layouts);
}

}  // namespace pyfiction
