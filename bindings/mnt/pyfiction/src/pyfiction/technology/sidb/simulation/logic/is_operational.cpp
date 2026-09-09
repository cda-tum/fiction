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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/is_operational.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Registers the operational checks on `sidb_layout`.
 *
 * @param m The module.
 */
inline void is_operational_functions(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_wire;
    using fiction::sidb::simulation::logic::is_operational_params;
    using wires = std::vector<bdl_wire>;

    m.def(
        "is_operational", [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params)
        { return fiction::sidb::simulation::logic::is_operational(lyt, spec, params); }, py::arg("lyt"),
        py::arg("spec"), py::arg("params") = is_operational_params{},
        DOC(fiction_sidb_simulation_logic_is_operational));
    m.def(
        "is_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params, const wires& in,
           const wires& out, const std::optional<layout>& canvas_lyt)
        { return fiction::sidb::simulation::logic::is_operational(lyt, spec, params, in, out, canvas_lyt); },
        py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
        py::arg("canvas_lyt") = std::nullopt, DOC(fiction_sidb_simulation_logic_is_operational_2));
    m.def(
        "is_operational",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec,
           const is_operational_params& params, const wires& in, const wires& out,
           const std::optional<layout>& canvas_lyt)
        {
            return fiction::sidb::simulation::logic::is_operational(input_pattern_layouts, spec, params, in, out,
                                                                    canvas_lyt);
        },
        py::arg("input_pattern_layouts"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"),
        py::arg("output_bdl_wire"), py::arg("canvas_lyt") = std::nullopt,
        DOC(fiction_sidb_simulation_logic_is_operational_3));
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params)
        { return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params); }, py::arg("lyt"),
        py::arg("spec"), py::arg("params") = is_operational_params{},
        DOC(fiction_sidb_simulation_logic_operational_input_patterns));
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params, const wires& in,
           const wires& out, const std::optional<layout>& canvas_lyt)
        {
            return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params, in, out, canvas_lyt);
        },
        py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
        py::arg("canvas_lyt") = std::nullopt, DOC(fiction_sidb_simulation_logic_operational_input_patterns_2));
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params)
        { return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params); },
        py::arg("lyt"), py::arg("spec"), py::arg("params") = is_operational_params{},
        DOC(fiction_sidb_simulation_logic_kink_induced_non_operational_input_patterns));
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params, const wires& in,
           const wires& out, const std::optional<layout>& canvas_lyt)
        {
            return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params, in,
                                                                                                 out, canvas_lyt);
        },
        py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
        py::arg("canvas_lyt") = std::nullopt,
        DOC(fiction_sidb_simulation_logic_kink_induced_non_operational_input_patterns_2));
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params)
        { return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params); },
        py::arg("lyt"), py::arg("spec"), py::arg("params") = is_operational_params{},
        DOC(fiction_sidb_simulation_logic_is_kink_induced_non_operational));
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const is_operational_params& params, const wires& in,
           const wires& out, const std::optional<layout>& canvas_lyt)
        {
            return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params, in, out,
                                                                                     canvas_lyt);
        },
        py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
        py::arg("canvas_lyt") = std::nullopt, DOC(fiction_sidb_simulation_logic_is_kink_induced_non_operational_2));
}

}  // namespace detail

void is_operational(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::sidb::simulation::logic::operational_status>(
        m, "operational_status", DOC(fiction_sidb_simulation_logic_operational_status))
        .value("OPERATIONAL", fiction::sidb::simulation::logic::operational_status::OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::sidb::simulation::logic::operational_status::NON_OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_NON_OPERATIONAL));

    py::enum_<fiction::sidb::simulation::logic::is_operational_params::operational_condition>(
        m, "operational_condition", DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition))
        .value("TOLERATE_KINKS",
               fiction::sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_TOLERATE_KINKS))
        .value("REJECT_KINKS",
               fiction::sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_REJECT_KINKS));

    py::enum_<fiction::sidb::simulation::logic::is_operational_params::operational_analysis_strategy>(
        m, "operational_analysis_strategy",
        DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy))
        .value("SIMULATION_ONLY",
               fiction::sidb::simulation::logic::is_operational_params::operational_analysis_strategy::SIMULATION_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_SIMULATION_ONLY))
        .value("FILTER_ONLY",
               fiction::sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_ONLY))
        .value(
            "FILTER_THEN_SIMULATION",
            fiction::sidb::simulation::logic::is_operational_params::operational_analysis_strategy::
                FILTER_THEN_SIMULATION,
            DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_THEN_SIMULATION));

    py::class_<fiction::sidb::simulation::logic::is_operational_params>(
        m, "is_operational_params", DOC(fiction_sidb_simulation_logic_is_operational_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &fiction::sidb::simulation::logic::is_operational_params::sim_params,
                DOC(fiction_sidb_simulation_logic_is_operational_params_sim_params))
        .def_rw("sim_engine", &fiction::sidb::simulation::logic::is_operational_params::sim_engine,
                DOC(fiction_sidb_simulation_logic_is_operational_params_sim_engine))
        .def_rw("input_bdl_iterator_params",
                &fiction::sidb::simulation::logic::is_operational_params::input_bdl_iterator_params,
                DOC(fiction_sidb_simulation_logic_is_operational_params_input_bdl_iterator_params))
        .def_rw("op_condition", &fiction::sidb::simulation::logic::is_operational_params::op_condition,
                DOC(fiction_sidb_simulation_logic_is_operational_params_op_condition))
        .def_rw("strategy_to_analyze_operational_status",
                &fiction::sidb::simulation::logic::is_operational_params::strategy_to_analyze_operational_status,
                DOC(fiction_sidb_simulation_logic_is_operational_params_strategy_to_analyze_operational_status));

    detail::is_operational_functions(m);
}

}  // namespace pyfiction
