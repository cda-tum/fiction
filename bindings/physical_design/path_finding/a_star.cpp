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
                fiction::physical_design::path_finding::a_star<Lyt>(
                    lyt, {source, target}, fiction::physical_design::path_finding::manhattan_distance_functor(),
                    fiction::physical_design::path_finding::unit_cost_functor<>(), params, obstructions));
        },
        nb::arg("layout"), nb::arg("source"), nb::arg("target"), nb::kw_only(),
        nb::arg("crossings") = a_star_options{}.crossings, nb::arg("obstructions") = nb::none(),
        nb::call_guard<nb::gil_scoped_release>(),
        R"doc(Find a shortest loop-free path from source to target in the layout.
The keyword options select obstructions and crossing constraints.
Raises ValueError when source or target lies outside the layout extent.

Args:
    obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
    crossings: Allow wires to cross on separate layers.
)doc");

    m.def("a_star_distance", &fiction::physical_design::path_finding::a_star_distance<Lyt>, nb::arg("layout"),
          nb::arg("source"), nb::arg("target"), nb::arg("obstructions") = fiction::layouts::obstructions{},
          nb::call_guard<nb::gil_scoped_release>(), DOC(fiction_physical_design_path_finding_a_star_distance));
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
