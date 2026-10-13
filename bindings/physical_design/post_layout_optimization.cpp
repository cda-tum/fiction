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
           const decltype(post_layout_optimization_options{}.on_progress)&         on_progress,
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
        R"doc(Optimize a 2DDWave-clocked Cartesian gate layout in place.
The keyword options bound relocations and runtime; statistics receives the completed result.
Raises ValueError for unsupported clocking or invalid occupied geometry.
Raises OverflowError when the extent exceeds signed routing coordinates.
)doc");
}

}  // namespace pyfiction
