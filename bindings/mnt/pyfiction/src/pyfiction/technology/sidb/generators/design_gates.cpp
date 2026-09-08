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
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/generators/design_gates.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void design_gates(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::generators::design_gates_params;
    using fiction::sidb::generators::design_gates_stats;

    py::class_<design_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_sidb_generators_design_gates_stats))
        .def(py::init<>(), "Default constructor.")
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
    py::enum_<design_gates_params::design_gates_mode>(
        m, "design_sidb_gates_mode", DOC(fiction_sidb_generators_design_gates_params_design_gates_mode))
        .value("QUICKCELL", design_gates_params::design_gates_mode::QUICKCELL,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_QUICKCELL))
        .value("AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER",
               design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER))
        .value("RANDOM", design_gates_params::design_gates_mode::RANDOM,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_RANDOM))
        .value("PRUNING_ONLY", design_gates_params::design_gates_mode::PRUNING_ONLY,
               DOC(fiction_sidb_generators_design_gates_params_design_gates_mode_PRUNING_ONLY));

    /**
     * Termination condition selector type.
     */
    py::enum_<design_gates_params::termination_condition>(m, "termination_condition")
        .value("AFTER_FIRST_SOLUTION", design_gates_params::termination_condition::AFTER_FIRST_SOLUTION)
        .value("ALL_COMBINATIONS_ENUMERATED", design_gates_params::termination_condition::ALL_COMBINATIONS_ENUMERATED);

    /**
     * Parameters.
     */
    py::class_<design_gates_params>(m, "design_sidb_gates_params", DOC(fiction_sidb_generators_design_gates_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_params", &design_gates_params::operational_params,
                DOC(fiction_sidb_generators_design_gates_params_operational_params))
        .def_rw("design_mode", &design_gates_params::design_mode,
                DOC(fiction_sidb_generators_design_gates_params_design_mode))
        .def_rw("canvas", &design_gates_params::canvas, DOC(fiction_sidb_generators_design_gates_params_canvas))
        .def_rw("number_of_canvas_sidbs", &design_gates_params::number_of_canvas_sidbs,
                DOC(fiction_sidb_generators_design_gates_params_number_of_canvas_sidbs))
        .def_rw("maximal_random_design_attempts", &design_gates_params::maximal_random_design_attempts,
                DOC(fiction_sidb_generators_design_gates_params_maximal_random_design_attempts))
        .def_rw("termination_cond", &design_gates_params::termination_cond,
                DOC(fiction_sidb_generators_design_gates_params_termination_condition));

    m.def(
        "design_sidb_gates",
        [](const layout& skeleton, const std::vector<py_tt>& spec, const design_gates_params& params,
           design_gates_stats* stats)
        { return fiction::sidb::generators::design_gates(skeleton, spec, params, stats); },
        py::arg("skeleton"), py::arg("spec"), py::arg("params") = design_gates_params{}, py::arg("stats") = nullptr,
        DOC(fiction_sidb_generators_design_gates));
}

}  // namespace pyfiction
