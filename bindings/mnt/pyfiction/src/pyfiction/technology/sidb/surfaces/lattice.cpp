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
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
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

template <typename LatticeOrientation>
void sidb_lattice_cell_level_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // fetch technology name
    auto orientation = std::string{fiction::sidb_lattice_name<LatticeOrientation>};
    std::ranges::transform(orientation, orientation.begin(), ::tolower);

    using py_sidb_lattice = py_sidb_lattice<LatticeOrientation>;

    /**
     * SiDB lattice.
     */
    py::class_<py_sidb_lattice, py_sidb_layout>(m, fmt::format("sidb_{}_lattice", orientation).c_str(),
                                                DOC(fiction_layouts_cell_level_layout))
        .def(py::init<>(), "Default constructor.")
        .def(py::init<const fiction::aspect_ratio<py_sidb_layout>&, const std::string&>(), py::arg("dimension"),
             py::arg("name") = "", DOC(fiction_sidb_surfaces_lattice))
        .def("clone", &py_sidb_lattice::clone)

        ;
}

}  // namespace detail

void lattice(nanobind::module_& m)
{
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_100>(m);
    detail::sidb_lattice_cell_level_layout<fiction::sidb::surfaces::lattice_111>(m);
}

}  // namespace pyfiction
