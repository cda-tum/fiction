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
 * @brief Python bindings for `fiction/technology/sidb/lattice.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/lattice.hpp>

#include <array>
#include <cstdint>
#include <functional>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/array.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void lattice(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::sidb::lattice_site>(m, "lattice_site", DOC(fiction_sidb_lattice_site))
        .def(py::init<>(), DOC(fiction_sidb_lattice_site_lattice_site))
        .def(py::init<int32_t, int32_t, uint8_t>(), py::arg("x"), py::arg("y"), py::arg("z"),
             DOC(fiction_sidb_lattice_site_lattice_site_2))
        .def(py::init<int32_t, int32_t>(), py::arg("x"), py::arg("y"), DOC(fiction_sidb_lattice_site_lattice_site_3))
        .def_rw("x", &fiction::sidb::lattice_site::x, DOC(fiction_sidb_lattice_site_x))
        .def_rw("y", &fiction::sidb::lattice_site::y, DOC(fiction_sidb_lattice_site_y))
        .def_rw("z", &fiction::sidb::lattice_site::z, DOC(fiction_sidb_lattice_site_z))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, DOC(fiction_sidb_lattice_site_operator_eq))
        .def(py::self != py::self)
        .def(py::self < py::self, DOC(fiction_sidb_lattice_site_operator_le))
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self)
        .def(py::self + py::self, DOC(fiction_sidb_lattice_site_operator_add))
        .def(py::self - py::self, DOC(fiction_sidb_lattice_site_operator_sub))
        // NOLINTEND(misc-redundant-expression)
        .def("__hash__",
             [](const fiction::sidb::lattice_site& s) { return std::hash<fiction::sidb::lattice_site>{}(s); })
        .def("__repr__", &fiction::sidb::lattice_site::str, DOC(fiction_sidb_lattice_site_str))

        ;

    m.def("row_of", &fiction::sidb::row_of, py::arg("site"), DOC(fiction_sidb_row_of));
    m.def("site_at_row", &fiction::sidb::site_at_row, py::arg("x"), py::arg("row"), DOC(fiction_sidb_site_at_row));
    m.def("sites_in_area", &fiction::sidb::sites_in_area, py::arg("first_corner"), py::arg("second_corner"),
          DOC(fiction_sidb_sites_in_area));

    py::class_<fiction::sidb::lattice>(m, "lattice", DOC(fiction_sidb_lattice))
        .def(py::init<>(), "Default constructor.")
        .def(py::init<const std::string&, const fiction::sidb::lattice::vector&, const fiction::sidb::lattice::vector&,
                      const std::array<fiction::sidb::lattice::vector, 2>&>(),
             py::arg("name"), py::arg("a1"), py::arg("a2"), py::arg("basis"), DOC(fiction_sidb_lattice))
        .def_static("si_100_2x1", &fiction::sidb::lattice::si_100_2x1, DOC(fiction_sidb_lattice_si_100_2x1))
        .def_static("si_111_1x1", &fiction::sidb::lattice::si_111_1x1, DOC(fiction_sidb_lattice_si_111_1x1))
        .def_rw("name", &fiction::sidb::lattice::name, DOC(fiction_sidb_lattice_name))
        .def_rw("a1", &fiction::sidb::lattice::a1, DOC(fiction_sidb_lattice_a1))
        .def_rw("a2", &fiction::sidb::lattice::a2, DOC(fiction_sidb_lattice_a2))
        .def_rw("basis", &fiction::sidb::lattice::basis, DOC(fiction_sidb_lattice_basis))
        .def("nm_position", &fiction::sidb::lattice::nm_position, py::arg("site"),
             DOC(fiction_sidb_lattice_nm_position))
        .def("nm_distance", &fiction::sidb::lattice::nm_distance, py::arg("source"), py::arg("target"),
             DOC(fiction_sidb_lattice_nm_distance))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, DOC(fiction_sidb_lattice_operator_eq))
        .def(py::self != py::self)
        // NOLINTEND(misc-redundant-expression)
        .def("__repr__", [](const fiction::sidb::lattice& lat) { return lat.name; })

        ;
}

}  // namespace pyfiction
