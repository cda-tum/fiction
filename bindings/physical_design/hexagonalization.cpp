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
#include "pyfiction/types.hpp"

#include <fiction/physical_design/hexagonalization.hpp>

#include <optional>
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

/** @brief Native options constructed for each Python call. */
using hexagonalization_options = fiction::physical_design::hexagonalization_params;

/**
 * @brief Binds hexagonalization helpers.
 * @param m Python module.
 */
void register_hexagonalization(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::physical_design::hexagonalization_io_pin_routing_error>(
        m, "hexagonalization_io_pin_routing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    nb::enum_<hexagonalization_options::io_pin_extension_mode>(
        m, "hexagonalization_io_pin_extension_mode",
        DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode))
        .value("NONE", hexagonalization_options::io_pin_extension_mode::NONE,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_NONE))
        .value("EXTEND", hexagonalization_options::io_pin_extension_mode::EXTEND,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_EXTEND))
        .value("EXTEND_PLANAR", hexagonalization_options::io_pin_extension_mode::EXTEND_PLANAR,
               DOC(fiction_physical_design_hexagonalization_params_io_pin_extension_mode_EXTEND_PLANAR));

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

    m.def(
        "hexagonalization",
        [](const py_cartesian_gate_layout&                                 layout,
           const decltype(hexagonalization_options{}.input_pin_extension)  input_pin_extension,
           const decltype(hexagonalization_options{}.output_pin_extension) output_pin_extension,
           const decltype(hexagonalization_options{}.on_progress)          on_progress,
           fiction::physical_design::hexagonalization_stats*               statistics)
        {
            hexagonalization_options params{};
            params.input_pin_extension  = input_pin_extension;
            params.output_pin_extension = output_pin_extension;
            params.on_progress          = on_progress;
            const nb::gil_scoped_release release{};
            return (&fiction::physical_design::hexagonalization<py_hexagonal_gate_layout,
                                                                py_cartesian_gate_layout>)(layout, params, statistics);
        },
        nb::arg("layout"), nb::kw_only(),
        nb::arg("input_pin_extension")  = hexagonalization_options{}.input_pin_extension,
        nb::arg("output_pin_extension") = hexagonalization_options{}.output_pin_extension,
        nb::arg("on_progress").none() = nb::none(), nb::arg("statistics") = nullptr,
        R"doc(Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even
row clocked layout suitable for SiDBs by remapping all gates and wires
as originally proposed in \"Scalable Physical Design for Silicon
Dangling Bond Logic: How a 45° Turn Prevents the Reinvention of the
Wheel\" by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2023
(https://ieeexplore.ieee.org/document/10231278).

Args:
    lyt: 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
    stats: Statistics.

Returns:
    Hexagonal representation of the Cartesian layout.

Raises:
    std::invalid_argument: If clocking or placed geometry does not
                           satisfy the input contract.
    std::overflow_error: If the transformed coordinates exceed the
                         signed coordinate range.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
