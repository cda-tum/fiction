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
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/physical_design/plane.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/chrono.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Binds PLANE helpers.
 * @param m Python module.
 */
void register_plane(nanobind::module_& m)
{
    namespace py = nanobind;

    using params = fiction::physical_design::plane_params;
    using stats  = fiction::physical_design::plane_stats;

    py::class_<params>(m, "plane_params", DOC(fiction_physical_design_plane_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("number_of_clock_phases", &params::number_of_clock_phases,
                DOC(fiction_physical_design_plane_params_number_of_clock_phases))
        .def_rw("on_progress", &params::on_progress, pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                DOC(fiction_physical_design_plane_params_on_progress));

    py::class_<stats>(m, "plane_stats", DOC(fiction_physical_design_plane_stats))
        .def(py::init<>(), "Default constructor.")
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
        [](const py_tec_network& network, const params& ps, stats* pst)
        {
            // the node order of a planarized technology network is its rank order
            const fiction::networks::views::mutable_rank_view ranked{network};

            return fiction::physical_design::plane<fiction::layouts::cartesian_gate_layout>(ranked, ps, pst);
        },
        py::arg("network"), py::arg("parameters") = params{}, py::arg("statistics") = nullptr,
        py::call_guard<py::gil_scoped_release>(),
        R"doc(Planar Layout from Network Embedding (PLANE): places and routes a planar network as a 2DDWave-clocked
Cartesian gate-level layout without wire crossings. The network is the first element of the pair that `planarization`
returns: its node order is the planar rank order, which the layout preserves.

Args:
    network: Planar, balanced, fanout-substituted technology network from `planarization`.
    parameters: Parameters.
    statistics: Optional statistics object that receives the runtime and the layout sizes.

Returns:
    A crossing-free Cartesian gate-level layout that implements `network`.

Raises:
    ValueError: If the rank order of `network` has crossings, or if a gate has a constant fanin next to a single
        non-constant one.
    high_degree_fanin_exception: If a node has more than two non-constant fanins.)doc");
}

}  // namespace pyfiction
