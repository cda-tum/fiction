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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace
{

/**
 * @brief Sets one axis of a coordinate.
 *
 * @param self Coordinate to change.
 * @param axis Pointer to the member that holds the axis.
 * @param value Python integer.
 * @throws std::overflow_error If `value` does not fit a coordinate axis.
 */
void set_axis(py_coordinate& self, int32_t py_coordinate::* axis, const int64_t value)
{
    self.*axis = coordinate_axis(value);
}

}  // namespace

/**
 * @brief Registers signed coordinates and checked size extents.
 * @param m Python layouts module.
 */
void register_coordinate(nb::module_& m)
{
    // Keep the GIL: tuple constructors access Python objects; the remaining value operations are too short to release.
    nb::class_<py_coordinate>(m, "coordinate", DOC(fiction_layouts_layout_base_coordinate))
        .def(nb::init<>(), DOC(fiction_layouts_layout_base_coordinate_coordinate))
        .def(
            "__init__",
            [](nb::pointer_and_handle<py_coordinate> self, const int64_t x, const int64_t y, const int64_t z)
            { std::construct_at(self.p, coordinate_axis(x), coordinate_axis(y), coordinate_axis(z)); }, nb::arg("x"),
            nb::arg("y"), nb::arg("z") = 0,
            parameter_doc(DOC(fiction_layouts_layout_base_coordinate_coordinate_2),
                          {{"coordinate_x", "x"}, {"coordinate_y", "y"}, {"coordinate_z", "z"}})
                .c_str())
        .def(nb::init<const py_coordinate>(), nb::arg("c"),
             nb::sig("def __init__(self, c: mnt.pyfiction.layouts.coordinate) -> None"))
        .def(
            "__init__",
            [](nb::pointer_and_handle<py_coordinate> self, const nb::tuple& t)
            {
                const auto size = t.size();

                if (size == 2)
                {
                    std::construct_at(self.p, coordinate_axis(nb::cast<int64_t>(nb::int_(nb::handle(t[0])))),
                                      coordinate_axis(nb::cast<int64_t>(nb::int_(nb::handle(t[1])))));
                    return;
                }
                if (size == 3)
                {
                    std::construct_at(self.p, coordinate_axis(nb::cast<int64_t>(nb::int_(nb::handle(t[0])))),
                                      coordinate_axis(nb::cast<int64_t>(nb::int_(nb::handle(t[1])))),
                                      coordinate_axis(nb::cast<int64_t>(nb::int_(nb::handle(t[2])))));
                    return;
                }

                throw std::runtime_error("Wrong number of axes provided for coordinate");
            },
            nb::arg("tuple_repr"),
            nb::sig("def __init__(self, tuple_repr: tuple[int, int] | tuple[int, int, int]) -> None"))

        .def_prop_rw(
            "x", [](const py_coordinate& self) -> int32_t { return self.x; },
            [](py_coordinate& self, const int64_t value) { set_axis(self, &py_coordinate::x, value); },
            DOC(fiction_layouts_layout_base_coordinate_x))
        .def_prop_rw(
            "y", [](const py_coordinate& self) -> int32_t { return self.y; },
            [](py_coordinate& self, const int64_t value) { set_axis(self, &py_coordinate::y, value); },
            DOC(fiction_layouts_layout_base_coordinate_y))
        .def_prop_rw(
            "z", [](const py_coordinate& self) -> int32_t { return self.z; },
            [](py_coordinate& self, const int64_t value) { set_axis(self, &py_coordinate::z, value); },
            DOC(fiction_layouts_layout_base_coordinate_z))

        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(nb::self == nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_eq))
        .def(nb::self != nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_ne))
        .def(nb::self < nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_lt))
        .def(nb::self > nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_gt))
        .def(nb::self <= nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_le))
        .def(nb::self >= nb::self, nb::arg("other"), DOC(fiction_layouts_layout_base_coordinate_operator_ge))
        // NOLINTEND(misc-redundant-expression)

        .def("__repr__", &py_coordinate::str, DOC(fiction_layouts_layout_base_coordinate_str))
        .def(
            "__hash__", [](const py_coordinate& self) { return std::hash<py_coordinate>{}(self); },
            "Returns a hash value of the coordinate.")

        ;

    nb::implicitly_convertible<nb::tuple, py_coordinate>();

    nb::class_<py_extent>(m, "extent", "Nonnegative width, height, and layer counts of a zero-origin layout.")
        .def(nb::init<>(), "Creates an empty extent.")
        .def(
            "__init__",
            [](nb::pointer_and_handle<py_extent> self, const int64_t width, const int64_t height, const int64_t layers)
            { std::construct_at(self.p, width, height, layers); }, nb::arg("width"), nb::arg("height"),
            nb::arg("layers") = 1,
            "Creates checked sizes. Two axes describe one layer. Each size lies between zero and 2147483648.")
        .def(nb::init<const py_extent&>(), nb::arg("extent"))
        .def(
            "__init__",
            [](nb::pointer_and_handle<py_extent> self, const nb::tuple& extent)
            {
                if (extent.size() != 2 && extent.size() != 3)
                {
                    throw std::invalid_argument("An extent requires two or three axis sizes");
                }
                std::construct_at(self.p, nb::cast<int64_t>(extent[0]), nb::cast<int64_t>(extent[1]),
                                  extent.size() == 3 ? nb::cast<int64_t>(extent[2]) : 1);
            },
            nb::arg("extent"), nb::sig("def __init__(self, extent: tuple[int, int] | tuple[int, int, int]) -> None"))
        .def_prop_rw(
            "width", [](const py_extent& self) { return self.width; }, [](py_extent& self, const int64_t value)
            { self.width = py_extent{value, 0}.width; }, "Checked width in coordinates.")
        .def_prop_rw(
            "height", [](const py_extent& self) { return self.height; }, [](py_extent& self, const int64_t value)
            { self.height = py_extent{0, value}.height; }, "Checked height in coordinates.")
        .def_prop_rw(
            "layers", [](const py_extent& self) { return self.layers; }, [](py_extent& self, const int64_t value)
            { self.layers = py_extent{0, 0, value}.layers; }, "Checked number of layers.")
        .def("__eq__", &py_extent::operator==, nb::arg("other"), nb::is_operator())
        .attr("__hash__") = nb::none();

    nb::implicitly_convertible<nb::tuple, py_extent>();
}

/**
 * @brief Registers the area function for size extents.
 * @param m Python layouts module.
 */
void register_coordinate_utility(nb::module_& m)
{
    // Keep the GIL for the constant-time extent calculation.
    m.def("area", &fiction::layouts::area_of, nb::arg("extent"),
          parameter_doc(DOC(fiction_layouts_area_of), {{"size", "extent"}}).c_str());
}

}  // namespace pyfiction
