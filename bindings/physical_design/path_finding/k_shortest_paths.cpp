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
 * @brief Python bindings for `fiction/physical_design/path_finding/k_shortest_paths.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstructions.hpp>
#include <fiction/physical_design/path_finding/k_shortest_paths.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <cstdint>
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
using yen_k_shortest_paths_options = fiction::physical_design::path_finding::yen_k_shortest_paths_params;

namespace detail
{

/**
 * @brief Binds yen k shortest paths helpers.
 * @param m Python module.
 * @tparam Lyt Layout type.
 */
template <typename Lyt>
void register_yen_k_shortest_paths_impl(nb::module_& m)
{
    m.def(
        "yen_k_shortest_paths",
        [](const Lyt& lyt, const fiction::layouts::layout_base::coordinate& source,
           const fiction::layouts::layout_base::coordinate& target, const uint32_t k,
           const decltype(yen_k_shortest_paths_options{}.astar_params.crossings) crossings,
           const std::optional<fiction::layouts::obstructions>&                  supplied_obstructions)
        {
            yen_k_shortest_paths_options params{};
            const auto obstructions       = supplied_obstructions.value_or(fiction::layouts::obstructions{});
            params.astar_params.crossings = crossings;

            const auto k_paths = fiction::physical_design::path_finding::yen_k_shortest_paths<
                fiction::physical_design::layout_coordinate_path, Lyt>(lyt, {source, target}, k, params, obstructions);

            std::vector<std::vector<fiction::layouts::layout_base::coordinate>> paths{};
            paths.reserve(k_paths.size());

            for (const auto& path : k_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        nb::arg("layout"), nb::arg("source"), nb::arg("target"), nb::arg("k"), nb::kw_only(),
        nb::arg("crossings")    = yen_k_shortest_paths_options{}.astar_params.crossings,
        nb::arg("obstructions") = nb::none(), R"doc(Yen's algorithm for finding up to :math:`k` shortest paths without
loops from a source to a target coordinate. If :math:`k` is larger
than the number of possible paths from source to target, the size of
the returned path collection will be smaller than :math:`k`.

This implementation uses the A* algorithm with the Manhattan distance
function internally.

This function automatically detects whether the given layout
implements a clocking interface (see `gate_level_layout`) and respects
the underlying information flow imposed by `layout`'s clocking scheme.
This algorithm does neither generate duplicate nor looping paths, even
in a cyclic clocking scheme. That is, along each path, each coordinate
can occur at maximum once.

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

In certain cases it might be desirable to enumerate regular coordinate
paths even if the layout implements a clocking interface. This can be
achieved by static-casting the layout to a coordinate layout when
calling this function:
```
{.cpp}
using clk_lyt = gate_level_layout<cartesian_layout>;
using path = layout_coordinate_path;
clk_lyt layout = ...;
auto k_paths = yen_k_shortest_paths<path>(static_cast<cartesian_layout>(layout), {source, target}, k);
```




The algorithm was originally described in \"An algorithm for finding
shortest routes from all source nodes to a given destination in
general networks\" by Jin Y. Yen in Quarterly of Applied Mathematics,
1970.

Args:
    layout: The layout in which the :math:`k` shortest paths are to be
            found.
    objective: Source-target coordinate pair.
    k: Maximum number of shortest paths to find.
    obstructions: Additional constraints; caller and layout
                  obstructions remain unchanged.

Returns:
    A collection of up to :math:`k` shortest loop-less paths in
    `layout` from `objective.source` to `objective.target`.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds k shortest paths helpers.
 * @param m Python module.
 */
void register_k_shortest_paths(nb::module_& m)
{

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_yen_k_shortest_paths_impl<py_cartesian_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<py_cartesian_layout>(m);
    detail::register_yen_k_shortest_paths_impl<py_shifted_cartesian_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<py_shifted_cartesian_layout>(m);
    detail::register_yen_k_shortest_paths_impl<py_hexagonal_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
