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
#include <stdexcept>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/array.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers SiDB lattice sites, lattice geometry, and row conversions.
 *
 * @param m Python module.
 */
void register_lattice(nb::module_& m)
{
    nb::class_<fiction::sidb::lattice_site>(m, "lattice_site", DOC(fiction_sidb_lattice_site))
        .def(nb::init<>(), DOC(fiction_sidb_lattice_site_lattice_site))
        .def(nb::init<int32_t, int32_t, int8_t>(), nb::arg("x"), nb::arg("y"), nb::arg("z"),
             DOC(fiction_sidb_lattice_site_lattice_site_2))
        .def(nb::init<int32_t, int32_t>(), nb::arg("x"), nb::arg("y"), DOC(fiction_sidb_lattice_site_lattice_site_3))
        .def_rw("x", &fiction::sidb::lattice_site::x, DOC(fiction_sidb_lattice_site_x))
        .def_rw("y", &fiction::sidb::lattice_site::y, DOC(fiction_sidb_lattice_site_y))
        .def_prop_rw(
            "z", [](const fiction::sidb::lattice_site& s) { return s.z; },
            [](fiction::sidb::lattice_site& s, const int64_t z)
            {
                if (z != 0 && z != 1)
                {
                    throw std::out_of_range("Invalid lattice basis index");
                }
                s.z = static_cast<int8_t>(z);
            },
            DOC(fiction_sidb_lattice_site_z))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(nb::self == nb::self, DOC(fiction_sidb_lattice_site_operator_eq))
        .def(nb::self != nb::self)
        .def(nb::self < nb::self, DOC(fiction_sidb_lattice_site_operator_le))
        .def(nb::self <= nb::self)
        .def(nb::self > nb::self)
        .def(nb::self >= nb::self)
        .def(nb::self + nb::self, DOC(fiction_sidb_lattice_site_operator_add))
        .def(nb::self - nb::self, DOC(fiction_sidb_lattice_site_operator_sub))
        // NOLINTEND(misc-redundant-expression)
        .def("__hash__",
             [](const fiction::sidb::lattice_site& s) { return std::hash<fiction::sidb::lattice_site>{}(s); })
        .def("__repr__", &fiction::sidb::lattice_site::str, DOC(fiction_sidb_lattice_site_str))

        ;

    m.def("row_of", &fiction::sidb::row_of, nb::arg("site"), DOC(fiction_sidb_row_of));
    m.def("site_at_row", &fiction::sidb::site_at_row, nb::arg("x"), nb::arg("row"), DOC(fiction_sidb_site_at_row));
    m.def("sites_in_area", &fiction::sidb::sites_in_area, nb::arg("first_corner"), nb::arg("second_corner"),
          DOC(fiction_sidb_sites_in_area));

    nb::class_<fiction::sidb::lattice>(m, "lattice", DOC(fiction_sidb_lattice))
        .def(nb::init<>(), "Default constructor.")
        .def(nb::init<const std::string&, const fiction::sidb::lattice::vector&, const fiction::sidb::lattice::vector&,
                      const std::array<fiction::sidb::lattice::vector, 2>&>(),
             nb::arg("name"), nb::arg("a1"), nb::arg("a2"), nb::arg("basis"), DOC(fiction_sidb_lattice))
        .def_static("si_100_2x1", &fiction::sidb::lattice::si_100_2x1, DOC(fiction_sidb_lattice_si_100_2x1))
        .def_static("si_111_1x1", &fiction::sidb::lattice::si_111_1x1, DOC(fiction_sidb_lattice_si_111_1x1))
        .def_rw("name", &fiction::sidb::lattice::name, DOC(fiction_sidb_lattice_name))
        .def_rw("a1", &fiction::sidb::lattice::a1, DOC(fiction_sidb_lattice_a1))
        .def_rw("a2", &fiction::sidb::lattice::a2, DOC(fiction_sidb_lattice_a2))
        .def_rw("basis", &fiction::sidb::lattice::basis, DOC(fiction_sidb_lattice_basis))
        .def("nm_position", &fiction::sidb::lattice::nm_position, nb::arg("site"),
             DOC(fiction_sidb_lattice_nm_position))
        .def("nm_distance", &fiction::sidb::lattice::nm_distance, nb::arg("source"), nb::arg("target"),
             DOC(fiction_sidb_lattice_nm_distance))
        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(nb::self == nb::self, DOC(fiction_sidb_lattice_operator_eq))
        .def(nb::self != nb::self)
        // NOLINTEND(misc-redundant-expression)
        .def("__repr__", [](const fiction::sidb::lattice& lat) { return lat.name; })

        ;
}

}  // namespace pyfiction
