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
#include <utility>

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
    using coordinate      = fiction::coordinate<SidbLayout>;

    constexpr auto supports_explicit_minimum = requires(py_sidb_lattice& lyt, const coordinate& minimum,
                                                        const coordinate& maximum) { lyt.resize(minimum, maximum); };

    /**
     * SiDB lattice.
     */
    auto layout =
        py::class_<py_sidb_lattice, SidbLayout>(m, class_name.c_str(), DOC(fiction_layouts_cell_level_layout));

    layout.def(py::init<>(), "Default constructor.");

    if constexpr (supports_explicit_minimum)
    {
        layout.def(
            "__init__",
            [](py::pointer_and_handle<py_sidb_lattice> self, const std::pair<coordinate, coordinate>& dimension,
               const std::string& name) { construct_layout_from_bounds(self, dimension, name); },
            py::arg("dimension"), py::arg("name") = "",
            "Constructs a named SiDB lattice from inclusive minimum and maximum coordinates.");
    }

    layout
        .def(
            "__init__",
            [](py::pointer_and_handle<py_sidb_lattice> self, const fiction::aspect_ratio<SidbLayout>& dimension,
               const std::string& name) { new (self.p) py_sidb_lattice{validate_layout_maximum(dimension), name}; },
            py::arg("dimension"), py::arg("name") = "", DOC(fiction_sidb_surfaces_lattice))
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
