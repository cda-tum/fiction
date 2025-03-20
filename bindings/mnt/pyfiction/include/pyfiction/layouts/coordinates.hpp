//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_COORDINATES_HPP
#define PYFICTION_COORDINATES_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/coordinates.hpp>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <cstdint>
#include <stdexcept>
#include <string>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

namespace pyfiction
{

namespace detail
{
/**
 * Unsigned offset coordinates.
 */
inline void offset_coordinate(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<py_offset_coordinate>(m, "offset_coordinate", DOC(fiction_offset_ucoord_t))
        .def(py::init<>(), DOC(fiction_offset_ucoord_t_ucoord_t))
        .def(py::init<const uint64_t>(), py::arg("int_repr"), DOC(fiction_offset_ucoord_t_ucoord_t_4))
        .def(py::init<const decltype(py_offset_coordinate().x), const decltype(py_offset_coordinate().y),
                      const decltype(py_offset_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_offset_ucoord_t_ucoord_t_2))
        .def(py::init<const py_offset_coordinate>(), py::arg("c"))
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_offset_coordinate{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_offset_coordinate{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             py::arg("tuple_repr"))

        .def_property(
            "x", [](const py_offset_coordinate& self) -> uint64_t { return self.x; },
            [](py_offset_coordinate& self, const uint64_t value) { self.x = value; }, DOC(fiction_offset_ucoord_t_x))
        .def_property(
            "y", [](const py_offset_coordinate& self) -> uint64_t { return self.y; },
            [](py_offset_coordinate& self, const uint64_t value) { self.y = value; }, DOC(fiction_offset_ucoord_t_y))
        .def_property(
            "z", [](const py_offset_coordinate& self) -> uint64_t { return self.z; },
            [](py_offset_coordinate& self, const uint64_t value) { self.z = value; }, DOC(fiction_offset_ucoord_t_z))

        .def(py::self == py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_ge))

        .def("__repr__", &py_offset_coordinate::str, DOC(fiction_offset_ucoord_t_str))
        .def("__hash__", [](const py_offset_coordinate& self) { return std::hash<py_offset_coordinate>{}(self); })

        ;

    py::implicitly_convertible<py::tuple, py_offset_coordinate>();
}

/**
 * Signed cube coordinates.
 */
inline void cube_coordinate(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<py_cube_coordinate>(m, "cube_coordinate", DOC(fiction_cube_coord_t))
        .def(py::init<>(), DOC(fiction_cube_coord_t_coord_t))
        .def(py::init<const uint64_t>(), py::arg("int_repr"), DOC(fiction_cube_coord_t_coord_t_2))
        .def(py::init<const decltype(py_cube_coordinate().x), const decltype(py_cube_coordinate().y),
                      const decltype(py_cube_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_cube_coord_t_coord_t_2))
        .def(py::init<const py_cube_coordinate>(), py::arg("c"))
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_cube_coordinate{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_cube_coordinate{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             py::arg("tuple_repr"))

        .def_property(
            "x", [](py_cube_coordinate& self) -> int32_t { return self.x; },
            [](py_cube_coordinate& self, const int32_t value) { self.x = value; }, DOC(fiction_cube_coord_t_x))
        .def_property(
            "y", [](py_cube_coordinate& self) -> int32_t { return self.y; },
            [](py_cube_coordinate& self, const int32_t value) { self.y = value; }, DOC(fiction_cube_coord_t_y))
        .def_property(
            "z", [](py_cube_coordinate& self) -> int32_t { return self.z; },
            [](py_cube_coordinate& self, const int32_t value) { self.z = value; }, DOC(fiction_cube_coord_t_z))

        .def(py::self == py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_ge))

        .def("__repr__", &py_cube_coordinate::str, DOC(fiction_cube_coord_t_str))
        .def("__hash__", [](const py_cube_coordinate& self) { return std::hash<py_cube_coordinate>{}(self); })

        ;

    py::implicitly_convertible<py::tuple, py_cube_coordinate>();
}

/**
 * Signed SiQAD coordinates.
 */
inline void siqad_coordinate(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<py_siqad_coordinate>(m, "siqad_coordinate", DOC(fiction_siqad_coord_t))
        .def(py::init<>(), DOC(fiction_siqad_coord_t_coord_t))
        .def(py::init<const decltype(py_siqad_coordinate().x), const decltype(py_siqad_coordinate().y),
                      const decltype(py_siqad_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_siqad_coord_t_coord_t_2))
        .def(py::init<const py_siqad_coordinate>(), py::arg("c"))
        .def(py::init(
                 [](const py::tuple& t)
                 {
                     const auto size = t.size();

                     if (size == 2)
                     {
                         return py_siqad_coordinate{py::int_(t[0]), py::int_(t[1])};
                     }
                     if (size == 3)
                     {
                         return py_siqad_coordinate{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                     }

                     throw std::runtime_error("Wrong number of dimensions provided for coordinate");
                 }),
             py::arg("tuple_repr"))

        .def_property(
            "x", [](const py_siqad_coordinate& self) -> int32_t { return self.x; },
            [](py_siqad_coordinate& self, const int32_t value) { self.x = value; }, DOC(fiction_siqad_coord_t_x))
        .def_property(
            "y", [](const py_siqad_coordinate& self) -> int32_t { return self.y; },
            [](py_siqad_coordinate& self, const int32_t value) { self.y = value; }, DOC(fiction_siqad_coord_t_y))
        .def_property(
            "z", [](const py_siqad_coordinate& self) -> int8_t { return self.z; },
            [](py_siqad_coordinate& self, const int8_t value) { self.z = value; }, DOC(fiction_siqad_coord_t_z))

        .def(py::self == py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_ge))

        .def("__repr__", &py_siqad_coordinate::str, DOC(fiction_siqad_coord_t_str))
        .def("__hash__", [](const py_siqad_coordinate& self) { return std::hash<py_siqad_coordinate>{}(self); })

        ;

    py::implicitly_convertible<py::tuple, py_siqad_coordinate>();
}

inline void coordinate_utility(pybind11::module& m)
{
    namespace py = pybind11;

    //    m.def("offset_area", &fiction::area<py_offset_coordinate>, py::arg("coord"), DOC(fiction_area));
    //    m.def("cube_area", &fiction::area<py_cube_coordinate>, py::arg("coord"), DOC(fiction_area));
    //    m.def("siqad_area", &fiction::area<py_siqad_coordinate>, py::arg("coord"), DOC(fiction_area));
    //
    //    m.def("offset_volume", &fiction::volume<py_offset_coordinate>, py::arg("coord"), DOC(fiction_volume));
    //    m.def("cube_volume", &fiction::volume<py_cube_coordinate>, py::arg("coord"), DOC(fiction_volume));
    //    m.def("siqad_volume", &fiction::volume<py_siqad_coordinate>, py::arg("coord"), DOC(fiction_volume));

    m.def("to_offset_coord", &fiction::siqad::to_fiction_coord<py_offset_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_fiction_coord));
    m.def("to_cube_coord", &fiction::siqad::to_fiction_coord<py_cube_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_fiction_coord));

    m.def("to_siqad_coord", &fiction::siqad::to_siqad_coord<py_offset_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_siqad_coord));
    m.def("to_siqad_coord", &fiction::siqad::to_siqad_coord<py_cube_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_siqad_coord));
}

template <typename CoordinateType>
inline void aspect_ratio_bindings(pybind11::module& m, const std::string& coordinate_type)
{
    namespace py = pybind11;

    // todo add docu
    py::class_<fiction::aspect_ratio<CoordinateType>>(m, fmt::format("aspect_ratio{}", coordinate_type).c_str())
        .def(py::init<>())
        .def(py::init<const CoordinateType&>(), py::arg("max"))
        .def(py::init<const CoordinateType&, const CoordinateType&>(), py::arg("min"), py::arg("max"))
        .def("x", &fiction::aspect_ratio<CoordinateType>::x)
        .def("y", &fiction::aspect_ratio<CoordinateType>::y)
        .def("z", &fiction::aspect_ratio<CoordinateType>::z)
        .def("area", &fiction::aspect_ratio<CoordinateType>::area)
        .def("volume", &fiction::aspect_ratio<CoordinateType>::volume)
        .def("__eq__", &fiction::aspect_ratio<CoordinateType>::operator==)
        .def_property_readonly("x_min", &fiction::aspect_ratio<CoordinateType>::x_min)
        .def_property_readonly("x_size", &fiction::aspect_ratio<CoordinateType>::x_size)
        .def_property_readonly("y_min", &fiction::aspect_ratio<CoordinateType>::y_min)
        .def_property_readonly("y_size", &fiction::aspect_ratio<CoordinateType>::y_size)
        .def_property_readonly("z_min", &fiction::aspect_ratio<CoordinateType>::z_min)
        .def_property_readonly("z_size", &fiction::aspect_ratio<CoordinateType>::z_size);
}

}  // namespace detail

inline void coordinates(pybind11::module& m)
{
    detail::offset_coordinate(m);
    detail::cube_coordinate(m);
    detail::siqad_coordinate(m);
    detail::coordinate_utility(m);

    detail::aspect_ratio_bindings<py_offset_coordinate>(m, "_offset");
    detail::aspect_ratio_bindings<py_cube_coordinate>(m, "_cube");
    detail::aspect_ratio_bindings<py_siqad_coordinate>(m, "_siqad");
}

}  // namespace pyfiction

#pragma GCC diagnostic pop

#endif  // PYFICTION_COORDINATES_HPP
