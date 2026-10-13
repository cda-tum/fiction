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
 * @brief Python bindings for `fiction/physical_design/wiring_reduction.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"  // NOLINT(misc-include-cleaner): defines the nanobind coordinate type casters

#include <fiction/physical_design/wiring_reduction.hpp>

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
using wiring_reduction_options = fiction::physical_design::wiring_reduction_params;

/** @brief Registers mutable wiring_reduction operations. @param m Python module. */
void register_wiring_reduction(nb::module_& m)
{

    nb::class_<fiction::physical_design::wiring_reduction_stats>(m, "wiring_reduction_stats",
                                                                 DOC(fiction_physical_design_wiring_reduction_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::wiring_reduction_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::wiring_reduction_stats::time_total,
                DOC(fiction_physical_design_wiring_reduction_stats_time_total))
        .def_ro("x_size_before", &fiction::physical_design::wiring_reduction_stats::x_size_before,
                DOC(fiction_physical_design_wiring_reduction_stats_x_size_before))
        .def_ro("y_size_before", &fiction::physical_design::wiring_reduction_stats::y_size_before,
                DOC(fiction_physical_design_wiring_reduction_stats_y_size_before))
        .def_ro("x_size_after", &fiction::physical_design::wiring_reduction_stats::x_size_after,
                DOC(fiction_physical_design_wiring_reduction_stats_x_size_after))
        .def_ro("y_size_after", &fiction::physical_design::wiring_reduction_stats::y_size_after,
                DOC(fiction_physical_design_wiring_reduction_stats_y_size_after))
        .def_ro("num_wires_before", &fiction::physical_design::wiring_reduction_stats::num_wires_before,
                DOC(fiction_physical_design_wiring_reduction_stats_num_wires_before))
        .def_ro("num_wires_after", &fiction::physical_design::wiring_reduction_stats::num_wires_after,
                DOC(fiction_physical_design_wiring_reduction_stats_num_wires_after))
        .def_ro("wiring_improvement", &fiction::physical_design::wiring_reduction_stats::wiring_improvement,
                DOC(fiction_physical_design_wiring_reduction_stats_wiring_improvement))
        .def_ro("area_improvement", &fiction::physical_design::wiring_reduction_stats::area_improvement,
                DOC(fiction_physical_design_wiring_reduction_stats_area_improvement))

        ;

    m.def(
        "wiring_reduction",
        [](py_cartesian_gate_layout& layout, const decltype(wiring_reduction_options{}.timeout) timeout,
           const decltype(wiring_reduction_options{}.on_progress)& on_progress,
           fiction::physical_design::wiring_reduction_stats*       statistics)
        {
            wiring_reduction_options params{};
            params.timeout     = timeout;
            params.on_progress = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::physical_design::wiring_reduction(layout, params, statistics);
        },
        nb::arg("layout"), nb::kw_only(), nb::arg("timeout") = wiring_reduction_options{}.timeout,
        nb::arg("on_progress").none() = nb::none(), nb::arg("statistics") = nullptr,
        R"doc(Reduce wires in a 2DDWave-clocked gate layout in place with A* routing.
The keyword options bound runtime and report progress.
Raises ValueError for unsupported clocking or invalid occupied geometry.
Raises OverflowError when the extent exceeds signed routing coordinates.
)doc");
}

}  // namespace pyfiction
