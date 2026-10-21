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
 * @brief Python bindings for `fiction/physical_design/plane.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/physical_design/plane.hpp>

#include <mockturtle/algorithms/cleanup.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using plane_options = fiction::physical_design::plane_params;

/**
 * @brief Binds PLANE helpers.
 * @param m Python module.
 */
void register_plane(nb::module_& m)
{
    using stats = fiction::physical_design::plane_stats;

    // Keep the GIL for statistics construction, field access, and bounded report formatting.
    nb::class_<stats>(m, "plane_stats", DOC(fiction_physical_design_plane_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const stats& st)
            {
                std::stringstream stream{};
                st.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &stats::time_total, DOC(fiction_physical_design_plane_stats_time_total))
        .def_ro("x_size", &stats::x_size, DOC(fiction_physical_design_plane_stats_x_size))
        .def_ro("y_size", &stats::y_size, DOC(fiction_physical_design_plane_stats_y_size))
        .def_ro("num_gates", &stats::num_gates, DOC(fiction_physical_design_plane_stats_num_gates))
        .def_ro("num_wires", &stats::num_wires, DOC(fiction_physical_design_plane_stats_num_wires))
        .def_ro("num_crossings", &stats::num_crossings, DOC(fiction_physical_design_plane_stats_num_crossings));

    m.def(
        "plane",
        [](const py_tec_network& network, stats* statistics,
           const decltype(plane_options{}.number_of_clock_phases) number_of_clock_phases,
           const decltype(plane_options{}.on_progress)&           on_progress)
        {
            plane_options parameters{};
            parameters.number_of_clock_phases = number_of_clock_phases;
            parameters.on_progress            = on_progress;

            // gates that no output reaches have no level and cannot be ranked, so they are dropped
            const auto cleaned = mockturtle::cleanup_dangling(network);

            const nb::gil_scoped_release release{};

            // the node order of a planarized technology network is its rank order
            const fiction::networks::views::mutable_rank_view ranked{cleaned};

            return fiction::physical_design::plane<fiction::layouts::cartesian_gate_layout>(ranked, parameters,
                                                                                            statistics);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("statistics") = nullptr,
        nb::arg("number_of_clock_phases") = plane_options{}.number_of_clock_phases,
        nb::arg("on_progress").none()     = nb::none(),
        nb::sig("def plane(network: mnt.pyfiction.networks.technology_network, *, statistics: plane_stats | None = "
                "None, number_of_clock_phases: num_clks = ..., on_progress: Callable[[str, int, int], None] | None = "
                "...) -> mnt.pyfiction.layouts.cartesian_gate_layout"),
        R"doc(Place and route a planar technology network without wire crossings (PLANE).
The network is the first element of the pair that `planarization` returns: its node order is the planar rank order,
which the layout preserves. Every level is placed on one anti-diagonal of a 2DDWave-clocked Cartesian gate layout.
Gates that no output reaches are dropped before placement.

Args:
    network: Planar, balanced, fanout-substituted technology network from `planarization`.
    statistics: Statistics object filled by the algorithm; None discards statistics.
    number_of_clock_phases: Number of clock phases of the 2DDWave clocking. 3 and 4 are supported.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.

Returns:
    A crossing-free Cartesian gate layout that implements `network`.

Raises:
    ValueError: If `network` has no primary inputs, if a node drives more than two fanouts, if a gate has a constant
        fanin next to a single non-constant one, if `network` is not balanced, or if its node order has crossings.
    high_degree_fanin_exception: If a node has more than two non-constant fanins.)doc");
}

}  // namespace pyfiction
