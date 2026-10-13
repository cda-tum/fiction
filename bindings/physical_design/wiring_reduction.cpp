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
           const decltype(wiring_reduction_options{}.on_progress) on_progress,
           fiction::physical_design::wiring_reduction_stats*      statistics)
        {
            wiring_reduction_options params{};
            params.timeout     = timeout;
            params.on_progress = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::physical_design::wiring_reduction(layout, params, statistics);
        },
        nb::arg("layout"), nb::kw_only(), nb::arg("timeout") = wiring_reduction_options{}.timeout,
        nb::arg("on_progress").none() = nb::none(), nb::arg("statistics") = nullptr,
        R"doc(A scalable wiring reduction algorithm for 2DDWave-clocked layouts
based on A* path finding as originally proposed in \"Late Breaking
Results: Wiring Reduction for Field-coupled Nanotechnologies\" by S.
Hofmann, M. Walter, and R. Wille in DAC 2024
(https://dl.acm.org/doi/10.1145/3649329.3663491) and extended in
\"Efficient and Scalable Post-Layout Optimization for Field-coupled
Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
(https://ieeexplore.ieee.org/document/10916761).

The core concept revolves around the selective removal of excess
wiring by cutting them from a layout, contingent upon the ability to
restore functional correctness by realigning the remaining layout
fragments. Given the complexity of identifying these cuts,
obstructions are strategically inserted into the layout to safeguard
against the inadvertent deletion of standard gates or wire segments
essential for the layout's integrity. Leveraging the obstructed layout
as a basis, A* Search is employed to systematically identify feasible
cuts either from left to right or top to bottom. Subsequently, these
identified cuts are removed from the layout to minimize not only the
number of wire segments, but also the area and critical path length.

Args:
    lyt: The 2DDWave-clocked layout whose wiring is to be reduced.
    pst: Statistics.

Raises:
    std::invalid_argument: If clocking or occupied geometry is
                           invalid.
    std::overflow_error: If the extent leaves no room for signed
                         routing coordinates.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
