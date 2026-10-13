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
#include "pyfiction/types.hpp"

#include <fiction/physical_design/color_routing.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/utils/graph/graph_coloring.hpp>

#include <cstdint>
#include <optional>
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
            color_routing_options params{};
            params.conduct_partial_routing = conduct_partial_routing;
            params.crossings               = crossings;
            params.path_limit              = path_limit;
            params.engine                  = engine;
            params.partial_sat             = partial_sat;

            std::vector<fiction::physical_design::routing_objective> objs{};
            objs.reserve(objectives.size());

            for (const auto& obj : objectives)
            {
                objs.push_back({std::get<0>(obj), std::get<1>(obj), std::get<2>(obj)});
            }

            return fiction::physical_design::color_routing(lyt, objs, params);
        },
        nb::arg("layout"), nb::arg("objectives"), nb::kw_only(),
        nb::arg("conduct_partial_routing") = color_routing_options{}.conduct_partial_routing,
        nb::arg("crossings")               = color_routing_options{}.crossings,
        nb::arg("path_limit") = color_routing_options{}.path_limit, nb::arg("engine") = color_routing_options{}.engine,
        nb::arg("partial_sat") = color_routing_options{}.partial_sat,
        R"doc(Route the supplied connections by coloring their path intersections.
Returns the paths found with the keyword routing options.
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
                                                            "Graph-coloring engines used for routing.")
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

    detail::register_color_routing_impl<py_cartesian_gate_layout>(m);
    detail::register_color_routing_impl<py_shifted_cartesian_gate_layout>(m);
    detail::register_color_routing_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
