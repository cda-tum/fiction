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
     * Offset coordinates.
     */
    py::class_<py_coordinate_offset_ucoord>(m, "coordinate", DOC(fiction_offset_ucoord_t))
        .def(py::init<>(), DOC(fiction_offset_ucoord_t_ucoord_t))
        .def(py::init<const uint64_t>(), "int_repr"_a, DOC(fiction_offset_ucoord_t_get_dead))
        .def(py::init<const uint64_t, const uint64_t, const uint64_t>(), "x"_a, "y"_a, "z"_a = 0,
             DOC(fiction_offset_ucoord_t_ucoord_t_2))
        .def(py::init<const py_coordinate_offset_ucoord>(), "c"_a, DOC(fiction_offset_ucoord_t_get_dead))
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_coordinate_offset_ucoord{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_coordinate_offset_ucoord{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             "tuple_repr"_a)

        .def_property(
            "x", [](py_coordinate_offset_ucoord& self) -> decltype(self.x) { return self.x; },
            [](py_coordinate_offset_ucoord& self, const decltype(self.x) value) { self.x = value; })
        .def_property(
            "y", [](py_coordinate_offset_ucoord& self) -> decltype(self.y) { return self.y; },
            [](py_coordinate_offset_ucoord& self, const decltype(self.y) value) { self.y = value; })
        .def_property(
            "z", [](py_coordinate_offset_ucoord& self) -> decltype(self.z) { return self.z; },
            [](py_coordinate_offset_ucoord& self, const decltype(self.z) value) { self.z = value; })

        .def(py::self == py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_eq))
        .def(py::self != py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_ne))
        .def(py::self < py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_lt))
        .def(py::self > py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_gt))
        .def(py::self <= py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_le))
        .def(py::self >= py::self, "other"_a, DOC(fiction_offset_ucoord_t_operator_ge))
        .def("__repr__", &py_coordinate_offset_ucoord::str, DOC(fiction_offset_ucoord_t_str))
        .def("__hash__",
             [](const py_coordinate_offset_ucoord& self) { return std::hash<py_coordinate_offset_ucoord>{}(self); });

    py::implicitly_convertible<py::tuple, py_coordinate_offset_ucoord>();
    /**
     * Cube coordinates.
     */
    py::class_<py_coordinate_cube_coord>(m, "cube_coordinate", DOC(fiction_cube_coord_t))
        .def(py::init<>(), DOC(fiction_cube_coord_t))
        .def(py::init<const int32_t, const int32_t, const int32_t>(), "x"_a, "y"_a, "z"_a = 0,
             DOC(fiction_cube_coord_t_coord_t_2))
        .def(py::init<const py_coordinate_cube_coord>(), "c"_a, DOC(fiction_cube_coord_t_get_dead))
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_coordinate_cube_coord{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_coordinate_cube_coord{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             "tuple_repr"_a)

        .def_property(
            "x", [](py_coordinate_cube_coord& self) -> decltype(self.x) { return self.x; },
            [](py_coordinate_cube_coord& self, const decltype(self.x) value) { self.x = value; })
        .def_property(
            "y", [](py_coordinate_cube_coord& self) -> decltype(self.y) { return self.y; },
            [](py_coordinate_cube_coord& self, const decltype(self.y) value) { self.y = value; })
        .def_property(
            "z", [](py_coordinate_cube_coord& self) -> decltype(self.z) { return self.z; },
            [](py_coordinate_cube_coord& self, const decltype(self.z) value) { self.z = value; })

        .def(py::self == py::self, "other"_a, DOC(fiction_cube_coord_t_operator_eq))
        .def(py::self != py::self, "other"_a, DOC(fiction_cube_coord_t_operator_ne))
        .def(py::self < py::self, "other"_a, DOC(fiction_cube_coord_t_operator_lt))
        .def(py::self > py::self, "other"_a, DOC(fiction_cube_coord_t_operator_gt))
        .def(py::self <= py::self, "other"_a, DOC(fiction_cube_coord_t_operator_le))
        .def(py::self >= py::self, "other"_a, DOC(fiction_cube_coord_t_operator_ge))
        .def("__repr__", &py_coordinate_cube_coord::str, DOC(fiction_cube_coord_t_str))
        .def("__hash__",
             [](const py_coordinate_cube_coord& self) { return std::hash<py_coordinate_cube_coord>{}(self); });

    py::implicitly_convertible<py::tuple, py_coordinate_cube_coord>();
    /**
     * SiQAD coordinates.
     */
    py::class_<py_coordinate_siqad>(m, "siqad_coordinate", DOC(fiction_siqad_coord_t))
        .def(py::init<>(), DOC(fiction_siqad_coord_t_coord_t))
        .def(py::init<const int32_t, const int32_t>(), "x"_a, "y"_a, DOC(fiction_siqad_coord_t_coord_t_3))
        .def(py::init<const int32_t, const int32_t, const int8_t>(), "x"_a, "y"_a, "z"_a = 0,
             DOC(fiction_siqad_coord_t_coord_t_2))
        .def(py::init<const py_coordinate_siqad>(), "c"_a, DOC(fiction_siqad_coord_t_get_dead))
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

        .def(py::self == py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_eq))
        .def(py::self != py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_ne))
        .def(py::self < py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_lt))
        .def(py::self > py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_gt))
        .def(py::self <= py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_le))
        .def(py::self >= py::self, "other"_a, DOC(fiction_siqad_coord_t_operator_ge))

        .def("__repr__", &py_coordinate_siqad::str, DOC(fiction_siqad_coord_t_str))
        .def("__hash__", [](const py_coordinate_siqad& self) { return std::hash<py_coordinate_siqad>{}(self); });

    py::implicitly_convertible<py::tuple, py_coordinate_siqad>();

    m.def("to_siqad_coord", &fiction::siqad::to_siqad_coord<py_coordinate_offset_ucoord>, "coord"_a,
          DOC(fiction_siqad_to_siqad_coord));
    m.def("to_fiction_coord", &fiction::siqad::to_fiction_coord<py_coordinate_offset_ucoord>, "coord"_a,
          DOC(fiction_convert_to_fiction_coordinates));
}

}  // namespace pyfiction

#endif  // PYFICTION_COORDINATES_HPP
