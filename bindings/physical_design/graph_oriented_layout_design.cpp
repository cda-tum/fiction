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
 * @brief Python bindings for `fiction/physical_design/graph_oriented_layout_design.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/graph_oriented_layout_design.hpp>

#include <optional>
#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using graph_oriented_layout_design_options = fiction::physical_design::graph_oriented_layout_design_params;

/** @brief Registers graph-oriented layout design. @param m Python module. */
void register_graph_oriented_layout_design(nb::module_& m)
{
    nb::enum_<graph_oriented_layout_design_options::effort_mode>(
        m, "gold_effort_mode", DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode))
        .value("HIGH_EFFICIENCY", graph_oriented_layout_design_options::effort_mode::HIGH_EFFICIENCY,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGH_EFFICIENCY))
        .value("HIGH_EFFORT", graph_oriented_layout_design_options::effort_mode::HIGH_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGH_EFFORT))
        .value("HIGHEST_EFFORT", graph_oriented_layout_design_options::effort_mode::HIGHEST_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGHEST_EFFORT))
        .value("MAXIMUM_EFFORT", graph_oriented_layout_design_options::effort_mode::MAXIMUM_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_MAXIMUM_EFFORT));

    nb::enum_<graph_oriented_layout_design_options::cost_objective>(
        m, "gold_cost_objective", DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective))
        .value("AREA", graph_oriented_layout_design_options::cost_objective::AREA,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_AREA))
        .value("WIRES", graph_oriented_layout_design_options::cost_objective::WIRES,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_WIRES))
        .value("CROSSINGS", graph_oriented_layout_design_options::cost_objective::CROSSINGS,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_CROSSINGS))
        .value("ACP", graph_oriented_layout_design_options::cost_objective::ACP,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_ACP))
        .value("CUSTOM", graph_oriented_layout_design_options::cost_objective::CUSTOM,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_CUSTOM));

    nb::class_<fiction::physical_design::graph_oriented_layout_design_stats>(
        m, "graph_oriented_layout_design_stats", DOC(fiction_physical_design_graph_oriented_layout_design_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::graph_oriented_layout_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::graph_oriented_layout_design_stats::time_total,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::graph_oriented_layout_design_stats::x_size,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::graph_oriented_layout_design_stats::y_size,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::graph_oriented_layout_design_stats::num_gates,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::graph_oriented_layout_design_stats::num_wires,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::graph_oriented_layout_design_stats::num_crossings,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_crossings));

    m.def(
        "graph_oriented_layout_design",
        [](py_tec_network& network, const decltype(graph_oriented_layout_design_options{}.timeout) timeout,
           const decltype(graph_oriented_layout_design_options{}.num_vertex_expansions)     num_vertex_expansions,
           const decltype(graph_oriented_layout_design_options{}.verbose)                   verbose,
           const decltype(graph_oriented_layout_design_options{}.mode)                      mode,
           const decltype(graph_oriented_layout_design_options{}.cost)                      cost,
           const decltype(graph_oriented_layout_design_options{}.return_first)              return_first,
           const decltype(graph_oriented_layout_design_options{}.planar)                    planar,
           const decltype(graph_oriented_layout_design_options{}.enable_multithreading)     enable_multithreading,
           const decltype(graph_oriented_layout_design_options{}.seed)                      seed,
           const decltype(graph_oriented_layout_design_options{}.straight_inverters)        straight_inverters,
           const decltype(graph_oriented_layout_design_options{}.tiles_to_skip_between_pis) tiles_to_skip_between_pis,
           const decltype(graph_oriented_layout_design_options{}
                              .randomize_tiles_to_skip_between_pis)           randomize_tiles_to_skip_between_pis,
           const decltype(graph_oriented_layout_design_options{}.on_progress) on_progress,
           const decltype(graph_oriented_layout_design_options{}.on_worker_progress) on_worker_progress,
           fiction::physical_design::graph_oriented_layout_design_stats*             statistics,
           std::function<uint64_t(const py_cartesian_gate_layout&)>                  custom_cost_objective)
        {
            graph_oriented_layout_design_options params{};
            params.timeout                             = timeout;
            params.num_vertex_expansions               = num_vertex_expansions;
            params.verbose                             = verbose;
            params.mode                                = mode;
            params.cost                                = cost;
            params.return_first                        = return_first;
            params.planar                              = planar;
            params.enable_multithreading               = enable_multithreading;
            params.seed                                = seed;
            params.straight_inverters                  = straight_inverters;
            params.tiles_to_skip_between_pis           = tiles_to_skip_between_pis;
            params.randomize_tiles_to_skip_between_pis = randomize_tiles_to_skip_between_pis;
            params.on_progress                         = on_progress;
            params.on_worker_progress                  = on_worker_progress;
            const nb::gil_scoped_release release{};
            return (&fiction::physical_design::graph_oriented_layout_design<py_cartesian_gate_layout,
                                                                            py_tec_network>)(network, params,
                                                                                             statistics,
                                                                                             custom_cost_objective);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("timeout") = graph_oriented_layout_design_options{}.timeout,
        nb::arg("num_vertex_expansions")     = graph_oriented_layout_design_options{}.num_vertex_expansions,
        nb::arg("verbose")                   = graph_oriented_layout_design_options{}.verbose,
        nb::arg("mode")                      = graph_oriented_layout_design_options{}.mode,
        nb::arg("cost")                      = graph_oriented_layout_design_options{}.cost,
        nb::arg("return_first")              = graph_oriented_layout_design_options{}.return_first,
        nb::arg("planar")                    = graph_oriented_layout_design_options{}.planar,
        nb::arg("enable_multithreading")     = graph_oriented_layout_design_options{}.enable_multithreading,
        nb::arg("seed")                      = graph_oriented_layout_design_options{}.seed,
        nb::arg("straight_inverters")        = graph_oriented_layout_design_options{}.straight_inverters,
        nb::arg("tiles_to_skip_between_pis") = graph_oriented_layout_design_options{}.tiles_to_skip_between_pis,
        nb::arg("randomize_tiles_to_skip_between_pis") =
            graph_oriented_layout_design_options{}.randomize_tiles_to_skip_between_pis,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("statistics") = nullptr, nb::arg("custom_cost_objective") = nullptr,
        R"doc(A scalable and efficient placement & routing approach based on
spanning a search space graph of partial layouts and finding a path to
one of its leaves, i.e., a complete layout.

The search space graph starts with an empty layout and then expands it
based on where the first node in a topological sort of the logic
network can be placed. Based on the position of this first node, a
cost is assigned to each expansion based on the position of the placed
node. The vertex with the lowest cost, which is the smallest layout
w.r.t. the cost objective (e.g. area), is then chosen for the next
expansion. This iterative process continues until a leaf node is
found, which is a layout with all nodes placed. The algorithm then
continues to backtrack through the search space graph to find other
complete layouts with lower cost.

Exclusively generates 2DDWave-clocked layouts.

This algorithm was proposed in \"A* is Born: Efficient and Scalable
Physical Design for Field-coupled Nanocomputing\" by S. Hofmann, M.
Walter, and R. Wille in IEEE NANO 2024
(https://ieeexplore.ieee.org/document/10628808) and extended in
\"Physical Design for Field-coupled Nanocomputing with Discretionary
Cost Objectives\" by S. Hofmann, M. Walter, and R. Wille in LASCAS
2025 (https://ieeexplore.ieee.org/document/10966234).

Args:
    ntk: The network to be placed and routed.
    pst: A pointer to a statistics object to record execution details.
         Defaults to nullptr.
    custom_cost_objective: A custom cost objective that is evaluated
                           at every expansion of the search space
                           graph. Should be a function that can be
                           calculated based on the current partial
                           layout and returns an uint64_t that should
                           be minimized.

Returns:
    The smallest layout yielded by the graph-oriented layout design
    algorithm under the given parameters.

Raises:
    networks::high_degree_fanin_exception: If `ntk` has a node with
                                           more than two fanins.
    std::invalid_argument: If the cost objective is `CUSTOM` and no
                           custom cost objective is provided, or if
                           `tiles_to_skip_between_pis` does not lie
                           in :math:`[0, 2^{20}]`.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
