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
 * @brief Python bindings for `fiction/physical_design/path_finding/enumerate_all_paths.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstructions.hpp>
#include <fiction/physical_design/path_finding/enumerate_all_paths.hpp>
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
using enumerate_all_paths_options = fiction::physical_design::path_finding::enumerate_all_paths_params;

namespace detail
{

/**
 * @brief Binds enumerate all paths helpers.
 * @param m Python module.
 * @tparam Lyt Layout type.
 */
template <typename Lyt>
void register_enumerate_all_paths_impl(nb::module_& m)
{
    m.def(
        "enumerate_all_paths",
        [](const Lyt& lyt, const fiction::layouts::layout_base::coordinate& source,
           const fiction::layouts::layout_base::coordinate&        target,
           const decltype(enumerate_all_paths_options{}.crossings) crossings,
           const std::optional<fiction::layouts::obstructions>&    supplied_obstructions)
        {
            enumerate_all_paths_options params{};
            const auto                  obstructions = supplied_obstructions.value_or(fiction::layouts::obstructions{});
            params.crossings                         = crossings;

            const auto all_paths = fiction::physical_design::path_finding::enumerate_all_paths<
                fiction::physical_design::layout_coordinate_path, Lyt>(lyt, {source, target}, params, obstructions);

            std::vector<std::vector<fiction::layouts::layout_base::coordinate>> paths{};
            paths.reserve(all_paths.size());

            for (const auto& path : all_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        nb::arg("layout"), nb::arg("source"), nb::arg("target"), nb::kw_only(),
        nb::arg("crossings") = enumerate_all_paths_options{}.crossings, nb::arg("obstructions") = nb::none(),
        R"doc(Enumerates all possible paths in a layout that start at a given source
coordinate and lead to given target coordinate. This function
automatically detects whether the given layout implements a clocking
interface (see `gate_level_layout`) and respects the underlying
information flow imposed by `layout`'s clocking scheme. This algorithm
does neither generate duplicate nor looping paths, even in a cyclic
clocking scheme. That is, along each path, each coordinate can occur
at maximum once.

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
auto all_paths = enumerate_all_paths<path>(static_cast<cartesian_layout>(layout), {source, target});
```



Args:
    layout: The layout whose paths are to be enumerated.
    objective: Source-target coordinate pair.
    obstructions: Additional coordinate and connection constraints;
                  the search does not modify them.

Returns:
    A collection of all unique paths in `layout` from
    `objective.source` to `objective.target`.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds enumerate all paths helpers.
 * @param m Python module.
 */
void register_enumerate_all_paths(nb::module_& m)
{

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_enumerate_all_paths_impl<py_cartesian_gate_layout>(m);
    detail::register_enumerate_all_paths_impl<py_cartesian_layout>(m);
    detail::register_enumerate_all_paths_impl<py_shifted_cartesian_gate_layout>(m);
    detail::register_enumerate_all_paths_impl<py_shifted_cartesian_layout>(m);
    detail::register_enumerate_all_paths_impl<py_hexagonal_gate_layout>(m);
    detail::register_enumerate_all_paths_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
