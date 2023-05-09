//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_COORDINATES_HPP
#define PYFICTION_COORDINATES_HPP

#include "pyfiction/types.hpp"

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <cstdint>
#include <exception>

namespace pyfiction
{

inline void coordinates(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;
    /**
     * Unsigned Cartesian coordinates.
     */
    py::class_<py_coordinate>(m, "coordinate")
        .def(py::init<>())
        .def(py::init<const uint64_t>(), "int_repr"_a)
        .def(py::init<const uint64_t, const uint64_t, const uint64_t>(), "x"_a, "y"_a, "z"_a = 0)
        .def(py::init<const py_coordinate>(), "c"_a)
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_coordinate{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_coordinate{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             "tuple_repr"_a)

        .def_property(
            "x", [](py_coordinate& self) -> decltype(self.x) { return self.x; },
            [](py_coordinate& self, const decltype(self.x) value) { self.x = value; })
        .def_property(
            "y", [](py_coordinate& self) -> decltype(self.y) { return self.y; },
            [](py_coordinate& self, const decltype(self.y) value) { self.y = value; })
        .def_property(
            "z", [](py_coordinate& self) -> decltype(self.z) { return self.z; },
            [](py_coordinate& self, const decltype(self.z) value) { self.z = value; })

        .def(py::self == py::self, "other"_a)
        .def(py::self != py::self, "other"_a)
        .def(py::self < py::self, "other"_a)
        .def(py::self > py::self, "other"_a)
        .def(py::self <= py::self, "other"_a)
        .def(py::self >= py::self, "other"_a)

        .def("__repr__", &py_coordinate::str)
        .def("__hash__", [](const py_coordinate& self) { return std::hash<py_coordinate>{}(self); })

        ;

    py::implicitly_convertible<py::tuple, py_coordinate>();

    /**
     * SiQAD coordinates.
     */
    py::class_<py_coordinate_siqad>(m, "siqad_coordinate")
        .def(py::init<>())
        .def(py::init<const int32_t, const int32_t>(), "x"_a, "y"_a)
        .def(py::init<const int32_t, const int32_t, const int8_t>(), "x"_a, "y"_a, "z"_a = 0)
        .def(py::init<const py_coordinate_siqad>(), "c"_a)
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_coordinate_siqad{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_coordinate_siqad{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             "tuple_repr"_a)

        .def_property(
            "x", [](py_coordinate_siqad& self) -> decltype(self.x) { return self.x; },
            [](py_coordinate_siqad& self, const decltype(self.x) value) { self.x = value; })
        .def_property(
            "y", [](py_coordinate_siqad& self) -> decltype(self.y) { return self.y; },
            [](py_coordinate_siqad& self, const decltype(self.y) value) { self.y = value; })
        .def_property(
            "z", [](py_coordinate_siqad& self) -> decltype(self.z) { return self.z; },
            [](py_coordinate_siqad& self, const decltype(self.z) value) { self.z = value; })

        .def(py::self == py::self, "other"_a)
        .def(py::self != py::self, "other"_a)
        .def(py::self < py::self, "other"_a)
        .def(py::self > py::self, "other"_a)
        .def(py::self <= py::self, "other"_a)
        .def(py::self >= py::self, "other"_a)

        .def("__repr__", &py_coordinate_siqad::str)
        .def("__hash__", [](const py_coordinate_siqad& self) { return std::hash<py_coordinate_siqad>{}(self); });

    py::implicitly_convertible<py::tuple, py_coordinate_siqad>();
}

}  // namespace pyfiction

#endif  // PYFICTION_COORDINATES_HPP
