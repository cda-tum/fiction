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
 * @brief Python bindings for `fiction/physical_design/path_finding/a_star.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstructions.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using a_star_options = fiction::physical_design::path_finding::a_star_params;

namespace detail
{

/**
 * @brief Binds a star helpers.
 * @param m Python module.
 * @tparam Lyt Layout type.
 */
template <typename Lyt>
void register_a_star_impl(nb::module_& m)
{
    m.def(
        "a_star",
        [](const Lyt& lyt, const fiction::layouts::layout_base::coordinate& source,
           const fiction::layouts::layout_base::coordinate&     target,
           const decltype(a_star_options{}.crossings)           crossings,
           const std::optional<fiction::layouts::obstructions>& supplied_obstructions)
        {
            a_star_options params{};
            const auto     obstructions = supplied_obstructions.value_or(fiction::layouts::obstructions{});
            params.crossings            = crossings;

            return static_cast<std::vector<fiction::layouts::layout_base::coordinate>>(
                fiction::physical_design::path_finding::a_star<fiction::physical_design::layout_coordinate_path, Lyt>(
                    lyt, {source, target}, fiction::physical_design::path_finding::manhattan_distance_functor(),
                    fiction::physical_design::path_finding::unit_cost_functor<>(), params, obstructions));
        },
        nb::arg("layout"), nb::arg("source"), nb::arg("target"), nb::kw_only(),
        nb::arg("crossings") = a_star_options{}.crossings, nb::arg("obstructions") = nb::none(),
        R"doc(The A* path finding algorithm for shortest loop-less paths between a
given source and target coordinate in a layout. This function
automatically detects whether the given layout implements a clocking
interface (see `gate_level_layout`) and respects the underlying
information flow imposed by `layout`'s clocking scheme.

A* is an extension of Dijkstra's algorithm for shortest paths but
offers better average complexity. It uses a heuristic distance
function that estimates the remaining costs towards the target in
every step. Thus, this heuristic function should neither be complex to
calculate nor overestimating the remaining costs. Common heuristics to
be used are the Manhattan and the Euclidean distance functions. See
`distance_functor` for implementations.

Paths do not pass obstructed coordinates or connections, except that
the target is never obstructed. A coordinate or connection is
obstructed if the `obstructions` argument marks it or if the layout's
`is_obstructed_coordinate` or `is_obstructed_connection` reports it.
Gate-level layouts report their occupied tiles and existing signal
connections, so paths in gate-level layouts avoid all placed gates and
wires.

If crossings are enabled in the parameters, paths in gate-level
layouts may cross other wires on the crossing layer. Wire crossings
are only allowed over other wires and only if the crossing layer is
not obstructed. Furthermore, it is ensured that crossings do not run
along another wire but cross only in a single point (orthogonal
crossings + knock-knees/double wires).

In certain cases it might be desirable to determine regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:
```
{.cpp}
using clk_lyt = gate_level_layout<cartesian_layout>;
using path = layout_coordinate_path;
clk_lyt layout = ...;
auto shortest_path = a_star<path>(static_cast<cartesian_layout>(layout), {source, target});
```




A* was introduced in \"A Formal Basis for the Heuristic Determination
of Minimum Cost Paths\" by Peter E. Hart, Nils J. Nilsson, and Bertram
Raphael in IEEE Transactions on Systems Science and Cybernetics 1968,
Volume 4, Issue 2.

This implementation is based on the pseudocode from
https://en.wikipedia.org/wiki/A_star_search_algorithm.

Args:
    layout: The layout in which the shortest path between a source and
            target coordinate is to be found.
    objective: Source-target coordinate pair.
    dist_fn: A distance functor that implements the desired heuristic
             estimation function.
    cost_fn: A cost functor that implements the desired cost function.
    obstructions: Additional coordinate and connection constraints;
                  the search does not modify them.

Returns:
    The shortest loop-less path in `layout` from `objective.source` to
    `objective.target`.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");

    m.def("a_star_distance", &fiction::physical_design::path_finding::a_star_distance<Lyt>, nb::arg("layout"),
          nb::arg("source"), nb::arg("target"), nb::arg("obstructions") = fiction::layouts::obstructions{},
          DOC(fiction_physical_design_path_finding_a_star_distance));
}

}  // namespace detail

/**
 * @brief Binds a star helpers.
 * @param m Python module.
 */
void register_a_star(nb::module_& m)
{

    detail::register_a_star_impl<py_cartesian_gate_layout>(m);
    detail::register_a_star_impl<py_cartesian_layout>(m);
    detail::register_a_star_impl<py_shifted_cartesian_gate_layout>(m);
    detail::register_a_star_impl<py_shifted_cartesian_layout>(m);
    detail::register_a_star_impl<py_hexagonal_gate_layout>(m);
    detail::register_a_star_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
