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
 * @brief Python bindings for `fiction/physical_design/color_routing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"  // NOLINT(misc-include-cleaner): preserves coordinate tuple input signatures.

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/physical_design/color_routing.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/utils/graph/graph_coloring.hpp>

#include <cstdint>
#include <tuple>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): Converts an unset path limit to None.
#include <nanobind/stl/tuple.h>     // NOLINT(misc-include-cleaner): Converts objective triples from Python tuples.
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner): Converts objective collections from Python lists.

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using color_routing_options = fiction::physical_design::color_routing_params;

namespace detail
{

/** @brief Bind routing objectives with explicit destination input indices. @tparam Lyt Gate layout. @param m Python
 * module. */
template <typename Lyt>
void register_color_routing_impl(nb::module_& m)
{
    m.def(
        "color_routing",
        [](Lyt&                                                                                lyt,
           const std::vector<std::tuple<fiction::layouts::layout_base::coordinate,
                                        fiction::layouts::layout_base::coordinate, uint32_t>>& objectives,
           const decltype(color_routing_options{}.conduct_partial_routing)                     conduct_partial_routing,
           const decltype(color_routing_options{}.crossings)                                   crossings,
           const decltype(color_routing_options{}.path_limit)                                  path_limit,
           const decltype(color_routing_options{}.engine)                                      engine,
           const decltype(color_routing_options{}.partial_sat)                                 partial_sat)
        {
            const color_routing_options params{.conduct_partial_routing = conduct_partial_routing,
                                               .crossings               = crossings,
                                               .path_limit              = path_limit,
                                               .engine                  = engine,
                                               .partial_sat             = partial_sat};

            std::vector<fiction::physical_design::routing_objective> objs{};
            objs.reserve(objectives.size());

            for (const auto& obj : objectives)
            {
                objs.push_back(
                    {.source = std::get<0>(obj), .target = std::get<1>(obj), .input_index = std::get<2>(obj)});
            }

            return fiction::physical_design::color_routing(lyt, objs, params);
        },
        nb::arg("layout"), nb::arg("objectives"), nb::kw_only(),
        nb::arg("conduct_partial_routing") = color_routing_options{}.conduct_partial_routing,
        nb::arg("crossings")               = color_routing_options{}.crossings,
        nb::arg("path_limit") = color_routing_options{}.path_limit, nb::arg("engine") = color_routing_options{}.engine,
        nb::arg("partial_sat") = color_routing_options{}.partial_sat, nb::call_guard<nb::gil_scoped_release>(),
        R"doc(Route the supplied connections in place by coloring their path intersections.
Returns True when all objectives are satisfied or conduct_partial_routing is enabled.
With path_limit=0, no candidate paths are generated and endpoint bounds are not checked.
Raises ValueError for an out-of-extent objective endpoint when path_limit is None or nonzero.
Raises ValueError for TABUCOL, which requires a color bound that this API does not accept.

Args:
    conduct_partial_routing: Do not abort if some objectives cannot be fulfilled, but partially route the layout as much as possible.
    crossings: Allow wires to cross on separate layers.
    path_limit: Maximum paths considered per objective; None enumerates all paths.
    engine: MCS, DSATUR, LMXRLF, or SAT.
    partial_sat: Allow partial solutions when the SAT coloring engine is selected.
)doc");
}

}  // namespace detail

/** @brief Register color routing. @param m Python module. */
void register_color_routing(nb::module_& m)
{
    /**
     * Graph coloring engine selector type.
     */
    nb::enum_<fiction::utils::graph::graph_coloring_engine>(m, "graph_coloring_engine",
                                                            DOC(fiction_utils_graph_graph_coloring_engine))
        .value("MCS", fiction::utils::graph::graph_coloring_engine::MCS,
               DOC(fiction_utils_graph_graph_coloring_engine_MCS))
        .value("DSATUR", fiction::utils::graph::graph_coloring_engine::DSATUR,
               DOC(fiction_utils_graph_graph_coloring_engine_DSATUR))
        .value("LMXRLF", fiction::utils::graph::graph_coloring_engine::LMXRLF,
               DOC(fiction_utils_graph_graph_coloring_engine_LMXRLF))
        .value("TABUCOL", fiction::utils::graph::graph_coloring_engine::TABUCOL,
               DOC(fiction_utils_graph_graph_coloring_engine_TABUCOL))
        .value("SAT", fiction::utils::graph::graph_coloring_engine::SAT,
               DOC(fiction_utils_graph_graph_coloring_engine_SAT));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_color_routing_impl<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_color_routing_impl<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_color_routing_impl<fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
