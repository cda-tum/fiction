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

namespace detail
{

/**
 * @brief Binds a star helpers.
 * @param m Python module.
 * @tparam Lyt Layout type.
 */
template <typename Lyt>
void bind_a_star_impl(nb::module_& m)
{
    m.def(
        "a_star",
        [](const Lyt& lyt, const fiction::layouts::layout_base::coordinate& source,
           const fiction::layouts::layout_base::coordinate&             target,
           const fiction::physical_design::path_finding::a_star_params& params,
           const fiction::layouts::obstructions&                        obstructions)
        {
            return static_cast<std::vector<fiction::layouts::layout_base::coordinate>>(
                fiction::physical_design::path_finding::a_star<fiction::physical_design::layout_coordinate_path, Lyt>(
                    lyt, {source, target}, fiction::physical_design::path_finding::manhattan_distance_functor(),
                    fiction::physical_design::path_finding::unit_cost_functor<>(), params, obstructions));
        },
        nb::arg("layout"), nb::arg("source"), nb::arg("target"),
        nb::arg("params")       = fiction::physical_design::path_finding::a_star_params{},
        nb::arg("obstructions") = fiction::layouts::obstructions{}, DOC(fiction_physical_design_path_finding_a_star));

    m.def("a_star_distance", &fiction::physical_design::path_finding::a_star_distance<Lyt>, nb::arg("layout"),
          nb::arg("source"), nb::arg("target"), nb::arg("obstructions") = fiction::layouts::obstructions{},
          DOC(fiction_physical_design_path_finding_a_star_distance));
}

}  // namespace detail

/**
 * @brief Binds a star helpers.
 * @param m Python module.
 */
void bind_a_star(nb::module_& m)
{
    nb::class_<fiction::physical_design::path_finding::a_star_params>(
        m, "a_star_params", DOC(fiction_physical_design_path_finding_a_star_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("crossings", &fiction::physical_design::path_finding::a_star_params::crossings,
                DOC(fiction_physical_design_path_finding_a_star_params_crossings));

    detail::bind_a_star_impl<py_cartesian_gate_layout>(m);
    detail::bind_a_star_impl<py_cartesian_layout>(m);
    detail::bind_a_star_impl<py_shifted_cartesian_gate_layout>(m);
    detail::bind_a_star_impl<py_shifted_cartesian_layout>(m);
    detail::bind_a_star_impl<py_hexagonal_gate_layout>(m);
    detail::bind_a_star_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
