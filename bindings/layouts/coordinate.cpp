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
 * @brief Python bindings for `fiction/layouts/layout_base.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/layout_base.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Registers coordinates.
 * @param m Python layouts module.
 */
void coordinate(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<py_coordinate>(m, "coordinate", DOC(fiction_layouts_layout_base_coordinate))
        .def(py::init<>(), DOC(fiction_layouts_layout_base_coordinate_coordinate))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_coordinate> self, const int64_t x, const int64_t y, const int64_t z)
            { std::construct_at(self.p, coordinate_axis(x), coordinate_axis(y), coordinate_axis(z)); }, py::arg("x"),
            py::arg("y"), py::arg("z") = 0, DOC(fiction_layouts_layout_base_coordinate_coordinate_2))
        .def(py::init<const py_coordinate>(), py::arg("c"),
             py::sig("def __init__(self, c: mnt.pyfiction.layouts.coordinate) -> None"))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_coordinate> self, const py::tuple& t)
            {
                const auto size = t.size();

                if (size == 2)
                {
                    std::construct_at(self.p, coordinate_axis(py::cast<int64_t>(py::int_(py::handle(t[0])))),
                                      coordinate_axis(py::cast<int64_t>(py::int_(py::handle(t[1])))));
                    return;
                }
                if (size == 3)
                {
                    std::construct_at(self.p, coordinate_axis(py::cast<int64_t>(py::int_(py::handle(t[0])))),
                                      coordinate_axis(py::cast<int64_t>(py::int_(py::handle(t[1])))),
                                      coordinate_axis(py::cast<int64_t>(py::int_(py::handle(t[2])))));
                    return;
                }

                throw std::runtime_error("Wrong number of dimensions provided for coordinate");
            },
            py::arg("tuple_repr"),
            py::sig("def __init__(self, tuple_repr: tuple[int, int] | tuple[int, int, int]) -> None"))

        .def_prop_rw(
            "x", [](const py_coordinate& self) -> int32_t { return self.x; },
            [](py_coordinate& self, const int64_t value) { self.x = coordinate_axis(value); },
            DOC(fiction_layouts_layout_base_coordinate_x))
        .def_prop_rw(
            "y", [](const py_coordinate& self) -> int32_t { return self.y; },
            [](py_coordinate& self, const int64_t value) { self.y = coordinate_axis(value); },
            DOC(fiction_layouts_layout_base_coordinate_y))
        .def_prop_rw(
            "z", [](const py_coordinate& self) -> int32_t { return self.z; },
            [](py_coordinate& self, const int64_t value) { self.z = coordinate_axis(value); },
            DOC(fiction_layouts_layout_base_coordinate_z))

        .def("is_valid", &py_coordinate::is_valid, DOC(fiction_layouts_layout_base_coordinate_is_valid))

        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_ge))
        // NOLINTEND(misc-redundant-expression)

        .def("__repr__", &py_coordinate::str, DOC(fiction_layouts_layout_base_coordinate_str))
        .def(
            "__hash__", [](const py_coordinate& self) { return std::hash<py_coordinate>{}(self); },
            "Returns a hash value of the coordinate.")

        ;

    py::implicitly_convertible<py::tuple, py_coordinate>();
}

/**
 * @brief Registers coordinate area and volume functions.
 * @param m Python layouts module.
 */
void coordinate_utility(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def("area", &fiction::layouts::area_of<py_coordinate>, py::arg("coord"), DOC(fiction_layouts_area_of));
    m.def("volume", &fiction::layouts::volume_of<py_coordinate>, py::arg("coord"), DOC(fiction_layouts_volume_of));
}

}  // namespace pyfiction
