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
#include "pyfiction/types.hpp"  // NOLINT(misc-include-cleaner): preserves coordinate tuple input signatures.

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/obstructions.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
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
        nb::arg("obstructions") = nb::none(), nb::call_guard<nb::gil_scoped_release>(),
        R"doc(Find up to k shortest loop-free paths from source to target.
The keyword options select obstructions and crossing constraints.
Raises ValueError when source or target lies outside the layout extent.

Args:
    obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
    crossings: Allow wires to cross on separate layers.
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

    detail::register_yen_k_shortest_paths_impl<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<fiction::layouts::cartesian_layout>(m);
    detail::register_yen_k_shortest_paths_impl<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<fiction::layouts::shifted_cartesian_layout>(m);
    detail::register_yen_k_shortest_paths_impl<fiction::layouts::hexagonal_gate_layout>(m);
    detail::register_yen_k_shortest_paths_impl<fiction::layouts::hexagonal_layout>(m);
}

}  // namespace pyfiction
