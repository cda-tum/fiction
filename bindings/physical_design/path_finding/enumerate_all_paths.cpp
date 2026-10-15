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

            const auto all_paths = fiction::physical_design::path_finding::enumerate_all_paths<Lyt>(
                lyt, {source, target}, params, obstructions);

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
        nb::call_guard<nb::gil_scoped_release>(),
        R"doc(Enumerate all loop-free paths from source to target in the layout.
The keyword options select obstructions and crossing constraints.

Args:
    crossings: Allow wires to cross on separate layers.
    obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
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
