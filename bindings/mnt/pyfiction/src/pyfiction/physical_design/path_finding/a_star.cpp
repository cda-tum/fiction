/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/traits.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void a_star_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "a_star",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const fiction::physical_design::path_finding::a_star_params& params)
        {
            return static_cast<std::vector<fiction::coordinate<Lyt>>>(
                fiction::physical_design::path_finding::a_star<fiction::physical_design::layout_coordinate_path<Lyt>,
                                                               Lyt>(
                    lyt, {source, target}, fiction::physical_design::path_finding::manhattan_distance_functor<Lyt>(),
                    fiction::physical_design::path_finding::unit_cost_functor<Lyt>(), params));
        },
        py::arg("layout"), py::arg("source"), py::arg("target"),
        py::arg("params") = fiction::physical_design::path_finding::a_star_params{},
        DOC(fiction_physical_design_path_finding_a_star));

    m.def("a_star_distance", &fiction::physical_design::path_finding::a_star_distance<Lyt, double>, py::arg("layout"),
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_a_star_distance));
}

}  // namespace detail

void a_star(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::path_finding::a_star_params>(
        m, "a_star_params", DOC(fiction_physical_design_path_finding_a_star_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("crossings", &fiction::physical_design::path_finding::a_star_params::crossings,
                DOC(fiction_physical_design_path_finding_a_star_params_crossings));

    detail::a_star_impl<py_cartesian_obstruction_layout>(m);
    detail::a_star_impl<py_cartesian_gate_layout>(m);
    detail::a_star_impl<py_cartesian_clocked_layout>(m);
    detail::a_star_impl<py_cartesian_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_obstruction_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_gate_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_clocked_layout>(m);
    detail::a_star_impl<py_shifted_cartesian_layout>(m);
    detail::a_star_impl<py_hexagonal_obstruction_layout>(m);
    detail::a_star_impl<py_hexagonal_gate_layout>(m);
    detail::a_star_impl<py_hexagonal_clocked_layout>(m);
    detail::a_star_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
