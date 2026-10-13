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
 * @brief Python bindings for `fiction/technology/sidb/generators/design_gates.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/operational_parameters.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/generators/design_gates.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner): Converts the statistics representation.
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using design_gates_options = fiction::sidb::generators::design_gates_params;

/**
 * @brief Registers SiDB gate design, and statistics.
 *
 * @param m Python module.
 */
void register_design_gates(nb::module_& m)
{
    using fiction::sidb::layout;

    using fiction::sidb::generators::design_gates_stats;

    nb::class_<design_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_sidb_generators_design_gates_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const design_gates_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.");

    /**
     * Design approach selector type.
     */
    nb::enum_<design_gates_options::design_gates_mode>(
        m, "design_sidb_gates_mode", DOC(fiction_sidb_generators_design_gates_params_design_gates_mode))
        .value("QUICKCELL", design_gates_options::design_gates_mode::QUICKCELL,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_QUICKCELL))
        .value("AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER",
               design_gates_options::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER))
        .value("RANDOM", design_gates_options::design_gates_mode::RANDOM,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_RANDOM))
        .value("PRUNING_ONLY", design_gates_options::design_gates_mode::PRUNING_ONLY,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_PRUNING_ONLY));

    /**
     * Termination condition selector type.
     */
    nb::enum_<design_gates_options::termination_condition>(m, "termination_condition")
        .value("AFTER_FIRST_SOLUTION", design_gates_options::termination_condition::AFTER_FIRST_SOLUTION)
        .value("ALL_COMBINATIONS_ENUMERATED", design_gates_options::termination_condition::ALL_COMBINATIONS_ENUMERATED);
    m.def(
        "design_sidb_gates",
        // NOLINTNEXTLINE(performance-unnecessary-value-param): Copy Python inputs before releasing the GIL.
        [](const layout skeleton, const std::vector<py_tt> spec,
           const decltype(design_gates_options{}.operational_params.timeout)              timeout,
           const decltype(design_gates_options{}.operational_params.sim_params.base)      base,
           const decltype(design_gates_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(design_gates_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(design_gates_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(design_gates_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(design_gates_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)                               threshold_bdl_interdistance,
           const decltype(design_gates_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                          minimum_distance,
           const decltype(design_gates_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                          maximum_distance,
           const decltype(design_gates_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(design_gates_options{}.operational_params.op_condition)             op_condition,
           const decltype(design_gates_options{}.operational_params.strategy_to_analyze_operational_status)
                                                                                 strategy_to_analyze_operational_status,
           const decltype(design_gates_options{}.design_mode)                    design_mode,
           const decltype(design_gates_options{}.canvas)                         canvas,
           const decltype(design_gates_options{}.number_of_canvas_sidbs)         number_of_canvas_sidbs,
           const decltype(design_gates_options{}.maximal_random_design_attempts) maximal_random_design_attempts,
           const decltype(design_gates_options{}.termination_cond)               termination_cond,
           const decltype(design_gates_options{}.on_progress)& on_progress, design_gates_stats* stats)
        {
            design_gates_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            params.design_mode                    = design_mode;
            params.canvas                         = canvas;
            params.number_of_canvas_sidbs         = number_of_canvas_sidbs;
            params.maximal_random_design_attempts = maximal_random_design_attempts;
            params.termination_cond               = termination_cond;
            params.on_progress                    = on_progress;

            // Copy Python-owned inputs before releasing the GIL; publish statistics after reacquiring it.
            design_gates_stats  local_stats{};
            std::vector<layout> result{};
            {
                const nb::gil_scoped_release release{};
                result = fiction::sidb::generators::design_gates(skeleton, spec, params, &local_stats);
            }
            if (stats != nullptr)
            {
                *stats = local_stats;
            }
            return result;
        },
        nb::arg("skeleton"), nb::arg("spec"), nb::kw_only(),
        nb::arg("timeout")    = design_gates_options{}.operational_params.timeout,
        nb::arg("base")       = design_gates_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = design_gates_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = design_gates_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = design_gates_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = design_gates_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            design_gates_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            design_gates_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            design_gates_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            design_gates_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = design_gates_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            design_gates_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("design_mode") = design_gates_options{}.design_mode, nb::arg("canvas") = design_gates_options{}.canvas,
        nb::arg("number_of_canvas_sidbs")         = design_gates_options{}.number_of_canvas_sidbs,
        nb::arg("maximal_random_design_attempts") = design_gates_options{}.maximal_random_design_attempts,
        nb::arg("termination_cond")               = design_gates_options{}.termination_cond,
        nb::arg("on_progress").none() = nb::none(), nb::arg("stats") = nullptr,
        R"doc(Design SiDB gates implementing the supplied truth tables within the canvas.
The keyword options select physical values, BDL detection, the design mode, and termination.
Expiration raises TimeoutError; on_progress reports completed search work.
)doc");
}

}  // namespace pyfiction
