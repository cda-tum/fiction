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

#include <fiction/physical_design/path_finding/enumerate_all_paths.hpp>
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
void enumerate_all_paths_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "enumerate_all_paths",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const fiction::physical_design::path_finding::enumerate_all_paths_params& params)
        {
            const auto all_paths = fiction::physical_design::path_finding::enumerate_all_paths<
                fiction::physical_design::layout_coordinate_path<Lyt>, Lyt>(lyt, {source, target}, params);

            std::vector<std::vector<fiction::coordinate<Lyt>>> paths{};
            paths.reserve(all_paths.size());

            for (const auto& path : all_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        py::arg("layout"), py::arg("source"), py::arg("target"),
        py::arg("params") = fiction::physical_design::path_finding::enumerate_all_paths_params{},
        DOC(fiction_physical_design_path_finding_enumerate_all_paths));
}

}  // namespace detail

void enumerate_all_paths(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::path_finding::enumerate_all_paths_params>(
        m, "enumerate_all_paths_params", DOC(fiction_physical_design_path_finding_enumerate_all_paths_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("crossings", &fiction::physical_design::path_finding::enumerate_all_paths_params::crossings,
                DOC(fiction_physical_design_path_finding_enumerate_all_paths_params_crossings))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::enumerate_all_paths_impl<py_cartesian_obstruction_layout>(m);
    detail::enumerate_all_paths_impl<py_cartesian_gate_layout>(m);
    detail::enumerate_all_paths_impl<py_cartesian_clocked_layout>(m);
    detail::enumerate_all_paths_impl<py_cartesian_layout>(m);
    detail::enumerate_all_paths_impl<py_shifted_cartesian_obstruction_layout>(m);
    detail::enumerate_all_paths_impl<py_shifted_cartesian_gate_layout>(m);
    detail::enumerate_all_paths_impl<py_shifted_cartesian_clocked_layout>(m);
    detail::enumerate_all_paths_impl<py_shifted_cartesian_layout>(m);
    detail::enumerate_all_paths_impl<py_hexagonal_obstruction_layout>(m);
    detail::enumerate_all_paths_impl<py_hexagonal_gate_layout>(m);
    detail::enumerate_all_paths_impl<py_hexagonal_clocked_layout>(m);
    detail::enumerate_all_paths_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction
