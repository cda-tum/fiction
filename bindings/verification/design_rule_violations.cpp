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

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <cstddef>
#include <sstream>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner): enables return-value conversion
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner): enables return-value conversion
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using gate_level_drv_options = fiction::verification::gate_level_drv_params;

namespace detail
{

/** @brief Binds object layout design rule checks. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void register_gate_level_drvs_impl(nb::module_& m)
{
    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt, const decltype(gate_level_drv_options{}.on_progress)& on_progress,
           const decltype(gate_level_drv_options{}.outside_extent)           outside_extent,
           const decltype(gate_level_drv_options{}.non_adjacent_connections) non_adjacent_connections,
           const decltype(gate_level_drv_options{}.missing_connections)      missing_connections,
           const decltype(gate_level_drv_options{}.crossing_gates)           crossing_gates,
           const decltype(gate_level_drv_options{}.clocked_data_flow)        clocked_data_flow,
           const decltype(gate_level_drv_options{}.has_io)                   has_io,
           const decltype(gate_level_drv_options{}.border_io) border_io, const bool print_report,
           fiction::verification::gate_level_drv_stats* statistics) -> std::pair<std::size_t, std::size_t>
        {
            gate_level_drv_options params{};
            params.on_progress              = on_progress;
            params.outside_extent           = outside_extent;
            params.non_adjacent_connections = non_adjacent_connections;
            params.missing_connections      = missing_connections;
            params.crossing_gates           = crossing_gates;
            params.clocked_data_flow        = clocked_data_flow;
            params.has_io                   = has_io;
            params.border_io                = border_io;

            std::ostringstream report_stream{};
            params.out = &report_stream;

            fiction::verification::gate_level_drv_stats stats{};

            {
                const nb::gil_scoped_release release{};
                fiction::verification::gate_level_drvs(lyt, params, &stats);
            }

            if (print_report)
            {
                // Python stdout access requires the GIL after the native check.
                nb::print(report_stream.str().c_str());
            }

            if (statistics != nullptr)
            {
                *statistics = stats;
            }

            return {stats.warnings, stats.drvs};
        },
        nb::arg("layout"), nb::kw_only(), nb::arg("on_progress").none() = nb::none(),
        nb::arg("outside_extent")           = gate_level_drv_options{}.outside_extent,
        nb::arg("non_adjacent_connections") = gate_level_drv_options{}.non_adjacent_connections,
        nb::arg("missing_connections")      = gate_level_drv_options{}.missing_connections,
        nb::arg("crossing_gates")           = gate_level_drv_options{}.crossing_gates,
        nb::arg("clocked_data_flow")        = gate_level_drv_options{}.clocked_data_flow,
        nb::arg("has_io") = gate_level_drv_options{}.has_io, nb::arg("border_io") = gate_level_drv_options{}.border_io,
        nb::arg("print_report") = false, nb::arg("statistics") = nullptr,
        R"doc(Count design-rule violations in the gate-level layout.
Returns (warnings, drvs); statistics receives their counts.
The keyword options select which checks run.

Args:
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    non_adjacent_connections: Check for nodes that are connected to non-adjacent ones.
    missing_connections: Check for nodes without connections.
    crossing_gates: Check for wires that are crossing gates.
    clocked_data_flow: Check if all node connections obey the clocking scheme data flow.
    has_io: Check if the layout has I/Os.
    border_io: Check whether primary inputs and outputs lie on the layout border.
    print_report: Print the design-rule report to Python stdout.
    statistics: Statistics object filled by the algorithm; None discards statistics.
    outside_extent: Check that every live object lies within the layout extent.
)doc");
}

}  // namespace detail

/** @brief Registers design rule checks and reports. @param m Python module. */
void register_design_rule_violations(nb::module_& m)
{

    // Keep the GIL for statistics construction and field access; conversions can create Python objects.
    nb::class_<fiction::verification::gate_level_drv_stats>(m, "gate_level_drv_stats",
                                                            DOC(fiction_verification_gate_level_drv_stats))
        .def(nb::init<>(), "Default constructor.")
        .def_ro("drvs", &fiction::verification::gate_level_drv_stats::drvs,
                DOC(fiction_verification_gate_level_drv_stats_drvs))
        .def_ro("warnings", &fiction::verification::gate_level_drv_stats::warnings,
                DOC(fiction_verification_gate_level_drv_stats_warnings))
        .def_prop_ro(
            "report", [](const fiction::verification::gate_level_drv_stats& stats) { return stats.report.dump(); },
            nb::for_getter(nb::call_guard<nb::gil_scoped_release>()),
            "The full design rule check report as a JSON string.")

        ;

    detail::register_gate_level_drvs_impl<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_gate_level_drvs_impl<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_gate_level_drvs_impl<fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
