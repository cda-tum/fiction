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
 * @brief Python bindings for `fiction/verification/design_rule_violations.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/verification/design_rule_violations.hpp>

#include <cstddef>
#include <sstream>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner): enables return-value conversion
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner): enables return-value conversion
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names

namespace pyfiction
{

namespace detail
{

/** @brief Binds object layout design rule checks. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void gate_level_drvs_impl(nanobind::module_& m)
{
    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt, fiction::verification::gate_level_drv_params params = {}, const bool print_report = false,
           fiction::verification::gate_level_drv_stats* statistics = nullptr) -> std::pair<std::size_t, std::size_t>
        {
            std::ostringstream report_stream{};
            params.out = &report_stream;

            fiction::verification::gate_level_drv_stats stats{};

            {
                const nanobind::gil_scoped_release release{};
                fiction::verification::gate_level_drvs(lyt, params, &stats);
            }

            if (print_report)
            {
                nanobind::print(report_stream.str().c_str());
            }

            if (statistics != nullptr)
            {
                *statistics = stats;
            }

            return {stats.warnings, stats.drvs};
        },
        nanobind::arg("layout"), nanobind::arg("params") = fiction::verification::gate_level_drv_params{},
        nanobind::arg("print_report") = false, nanobind::arg("statistics") = nullptr,
        DOC(fiction_verification_gate_level_drvs));
}

}  // namespace detail

/** @brief Registers design rule checks and reports. @param m Python module. */
void design_rule_violations(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::verification::gate_level_drv_params>(
        m, "gate_level_drv_params", pyfiction::progress_type_slots<fiction::verification::gate_level_drv_params>(),
        DOC(fiction_verification_gate_level_drv_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("on_progress", &fiction::verification::gate_level_drv_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                "Receives completed work and the phase total.")

        .def_rw("outside_extent", &fiction::verification::gate_level_drv_params::outside_extent,
                "Checks that every live object lies within the layout extent.")
        .def_rw("non_adjacent_connections", &fiction::verification::gate_level_drv_params::non_adjacent_connections,
                DOC(fiction_verification_gate_level_drv_params_non_adjacent_connections))
        .def_rw("missing_connections", &fiction::verification::gate_level_drv_params::missing_connections,
                DOC(fiction_verification_gate_level_drv_params_missing_connections))
        .def_rw("crossing_gates", &fiction::verification::gate_level_drv_params::crossing_gates,
                DOC(fiction_verification_gate_level_drv_params_crossing_gates))
        .def_rw("clocked_data_flow", &fiction::verification::gate_level_drv_params::clocked_data_flow,
                DOC(fiction_verification_gate_level_drv_params_clocked_data_flow))
        .def_rw("has_io", &fiction::verification::gate_level_drv_params::has_io,
                DOC(fiction_verification_gate_level_drv_params_has_io))

        .def_rw("border_io", &fiction::verification::gate_level_drv_params::border_io,
                DOC(fiction_verification_gate_level_drv_params_border_io))

        ;

    py::class_<fiction::verification::gate_level_drv_stats>(m, "gate_level_drv_stats",
                                                            DOC(fiction_verification_gate_level_drv_stats))
        .def(py::init<>(), "Default constructor.")
        .def_ro("drvs", &fiction::verification::gate_level_drv_stats::drvs,
                DOC(fiction_verification_gate_level_drv_stats_drvs))
        .def_ro("warnings", &fiction::verification::gate_level_drv_stats::warnings,
                DOC(fiction_verification_gate_level_drv_stats_warnings))
        .def_prop_ro(
            "report", [](const fiction::verification::gate_level_drv_stats& stats) { return stats.report.dump(); },
            "The full design rule check report as a JSON string.")

        ;

    detail::gate_level_drvs_impl<py_cartesian_gate_layout>(m);
    detail::gate_level_drvs_impl<py_shifted_cartesian_gate_layout>(m);
    detail::gate_level_drvs_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
