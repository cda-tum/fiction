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
 * @brief Python bindings for `fiction/physical_design/hexagonalization.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/hexagonalization.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void hexagonalization(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::physical_design::hexagonalization_io_pin_routing_error>(
        m, "hexagonalization_io_pin_routing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    nb::enum_<fiction::physical_design::hexagonalization_params::io_pin_extension_mode>(
        m, "hexagonalization_io_pin_extension_mode",
        DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode))
        .value("NONE", fiction::physical_design::hexagonalization_params::io_pin_extension_mode::NONE,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_NONE))
        .value("EXTEND", fiction::physical_design::hexagonalization_params::io_pin_extension_mode::EXTEND,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_EXTEND))
        .value("EXTEND_PLANAR", fiction::physical_design::hexagonalization_params::io_pin_extension_mode::EXTEND_PLANAR,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_EXTEND_PLANAR));

    nb::class_<fiction::physical_design::hexagonalization_params>(
        m, "hexagonalization_params",
        pyfiction::progress_type_slots<fiction::physical_design::hexagonalization_params>(),
        DOC(fiction_physical_design_hexagonalization_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("input_pin_extension", &fiction::physical_design::hexagonalization_params::input_pin_extension,
                DOC(fiction_physical_design_hexagonalization_params_input_pin_extension))
        .def_rw("output_pin_extension", &fiction::physical_design::hexagonalization_params::output_pin_extension,
                DOC(fiction_physical_design_hexagonalization_params_output_pin_extension))
        .def_rw("on_progress", &fiction::physical_design::hexagonalization_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                DOC(fiction_physical_design_hexagonalization_params_on_progress));

    nb::class_<fiction::physical_design::hexagonalization_stats>(m, "hexagonalization_stats",
                                                                 DOC(fiction_physical_design_hexagonalization_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::hexagonalization_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::hexagonalization_stats::time_total,
                DOC(fiction_physical_design_hexagonalization_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::hexagonalization_stats::x_size,
                DOC(fiction_physical_design_hexagonalization_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::hexagonalization_stats::y_size,
                DOC(fiction_physical_design_hexagonalization_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::hexagonalization_stats::num_gates,
                DOC(fiction_physical_design_hexagonalization_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::hexagonalization_stats::num_wires,
                DOC(fiction_physical_design_hexagonalization_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::hexagonalization_stats::num_crossings,
                DOC(fiction_physical_design_hexagonalization_stats_num_crossings));

    m.def("hexagonalization",
          &fiction::physical_design::hexagonalization<py_hexagonal_gate_layout, py_cartesian_gate_layout>,
          nb::arg("layout"), nb::arg("parameters") = fiction::physical_design::hexagonalization_params{},
          nb::arg("statistics") = nullptr, nb::call_guard<nb::gil_scoped_release>(),
          DOC(fiction_physical_design_hexagonalization));
}

}  // namespace pyfiction
