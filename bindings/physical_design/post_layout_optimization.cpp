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
 * @brief Python bindings for `fiction/physical_design/post_layout_optimization.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"  // NOLINT(misc-include-cleaner): defines the nanobind coordinate type casters

#include <fiction/physical_design/post_layout_optimization.hpp>

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
using post_layout_optimization_options = fiction::physical_design::post_layout_optimization_params;

/** @brief Registers mutable post_layout_optimization operations. @param m Python module. */
void register_post_layout_optimization(nb::module_& m)
{

    nb::class_<fiction::physical_design::post_layout_optimization_stats>(
        m, "post_layout_optimization_stats", DOC(fiction_physical_design_post_layout_optimization_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::post_layout_optimization_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::post_layout_optimization_stats::time_total,
                DOC(fiction_physical_design_post_layout_optimization_stats_time_total))
        .def_ro("x_size_before", &fiction::physical_design::post_layout_optimization_stats::x_size_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_x_size_before))
        .def_ro("y_size_before", &fiction::physical_design::post_layout_optimization_stats::y_size_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_y_size_before))
        .def_ro("x_size_after", &fiction::physical_design::post_layout_optimization_stats::x_size_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_x_size_after))
        .def_ro("y_size_after", &fiction::physical_design::post_layout_optimization_stats::y_size_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_y_size_after))
        .def_ro("area_improvement", &fiction::physical_design::post_layout_optimization_stats::area_improvement,
                DOC(fiction_physical_design_post_layout_optimization_stats_area_improvement))
        .def_ro("num_wires_before", &fiction::physical_design::post_layout_optimization_stats::num_wires_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_wires_before))
        .def_ro("num_wires_after", &fiction::physical_design::post_layout_optimization_stats::num_wires_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_wires_after))
        .def_ro("num_crossings_before", &fiction::physical_design::post_layout_optimization_stats::num_crossings_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_crossings_before))
        .def_ro("num_crossings_after", &fiction::physical_design::post_layout_optimization_stats::num_crossings_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_crossings_after));

    m.def(
        "post_layout_optimization",
        [](py_cartesian_gate_layout&                                               layout,
           const decltype(post_layout_optimization_options{}.max_gate_relocations) max_gate_relocations,
           const decltype(post_layout_optimization_options{}.optimize_pos_only)    optimize_pos_only,
           const decltype(post_layout_optimization_options{}.planar_optimization)  planar_optimization,
           const decltype(post_layout_optimization_options{}.timeout)              timeout,
           const decltype(post_layout_optimization_options{}.on_progress)          on_progress,
           fiction::physical_design::post_layout_optimization_stats*               statistics)
        {
            post_layout_optimization_options params{};
            params.max_gate_relocations = max_gate_relocations;
            params.optimize_pos_only    = optimize_pos_only;
            params.planar_optimization  = planar_optimization;
            params.timeout              = timeout;
            params.on_progress          = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::physical_design::post_layout_optimization(layout, params, statistics);
        },
        nb::arg("layout"), nb::kw_only(),
        nb::arg("max_gate_relocations") = post_layout_optimization_options{}.max_gate_relocations,
        nb::arg("optimize_pos_only")    = post_layout_optimization_options{}.optimize_pos_only,
        nb::arg("planar_optimization")  = post_layout_optimization_options{}.planar_optimization,
        nb::arg("timeout") = post_layout_optimization_options{}.timeout, nb::arg("on_progress").none() = nb::none(),
        nb::arg("statistics") = nullptr,
        R"doc(A post-layout optimization algorithm as originally proposed in \"Post-
Layout Optimization for Field-coupled Nanotechnologies\" by S.
Hofmann, M. Walter, and R. Wille in NANOARCH 2023
(https://dl.acm.org/doi/10.1145/3611315.3633247) and extended in
\"Efficient and Scalable Post-Layout Optimization for Field-coupled
Nanotechnologies\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
(https://ieeexplore.ieee.org/document/10916761). It can be used to
reduce the area of a given sub-optimal Cartesian gate-level layout
created by heuristics or machine learning. This optimization utilizes
the distinct characteristics of the 2DDWave clocking scheme, which
only allows information flow from top to bottom and left to right,
therefore only aforementioned clocking scheme is supported.

To reduce the layout area, first, gates are moved up and to the left
as far as possible, including rerouting. This creates more compact
layouts by freeing up space to the right and bottom, as all gates were
moved to the top left corner.

After moving all gates, this algorithm also checks if excess wiring
exists on the layout using the `wiring_reduction` algorithm (cf.
`wiring_reduction.hpp`)

As outputs have to lay on the border of a layout for better
accessibility, they are also moved to new borders determined based on
the location of all other gates.

Args:
    lyt: 2DDWave-clocked Cartesian gate-level layout to optimize.
    pst: Statistics.

Raises:
    std::invalid_argument: If clocking or occupied geometry is
                           invalid.
    std::overflow_error: If the extent leaves no room for signed
                         routing coordinates.

Note:
    This function requires the gate-level layout to be 2DDWave-
    clocked!

Note:
    Exceptions may leave a partially edited layout. Object identities,
    occupancy, and connection storage remain structurally valid;
    logical equivalence and physical validity are not guaranteed after
    failure.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
