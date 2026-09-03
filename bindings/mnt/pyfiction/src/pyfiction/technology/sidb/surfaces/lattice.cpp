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
 * @brief Python bindings for `fiction/technology/sidb/surfaces/lattice.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/layout_dimension.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename LatticeOrientation, typename SidbLayout = py_sidb_layout>
void sidb_lattice_cell_level_layout(nanobind::module_& m, const std::string& class_name)
{
    namespace py = nanobind;

    using py_sidb_lattice = py_sidb_lattice<LatticeOrientation, SidbLayout>;

    /**
     * SiDB lattice.
     */
    py::class_<py_sidb_lattice, SidbLayout>(m, class_name.c_str(), DOC(fiction_layouts_cell_level_layout))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__init__",
            [](py::pointer_and_handle<py_sidb_lattice> self, const fiction::aspect_ratio<SidbLayout>& dimension,
               const std::string& name) { new (self.p) py_sidb_lattice{validate_layout_maximum(dimension), name}; },
            py::arg("dimension"), py::arg("name") = "", DOC(fiction_sidb_surfaces_lattice))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_sidb_lattice> self, const py::tuple& dimension, const std::string& name)
            {
                using coordinate = fiction::coordinate<SidbLayout>;

                const auto parsed_dimension = parse_layout_dimension<coordinate>(dimension);
                if (parsed_dimension.minimum.has_value())
                {
                    new (self.p) py_sidb_lattice{coordinate{0, 0, 0}, name};
                    self.p->resize(*parsed_dimension.minimum, parsed_dimension.maximum);
                    return;
                }

                new (self.p) py_sidb_lattice{parsed_dimension.maximum, name};
            },
            py::arg("dimension"), py::arg("name") = "",
            "Constructs a named SiDB lattice from a maximum coordinate or an inclusive (minimum, maximum) coordinate "
            "pair.")
        .def("clone", &py_sidb_lattice::clone)

        ;
}

}  // namespace detail

void lattice(nanobind::module_& m)
{
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_100>(m, "sidb_100_lattice");
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_111>(m, "sidb_111_lattice");
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_100, py_sidb_layout_cube>(
        m, "sidb_100_lattice_cube");
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_111, py_sidb_layout_cube>(
        m, "sidb_111_lattice_cube");
}

}  // namespace pyfiction
