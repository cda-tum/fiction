//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_COORDINATES_HPP
#define PYFICTION_COORDINATES_HPP

#include "fiction/layouts/coordinates.hpp"
#include "pybind11/operators.h"
#include "pybind11/pybind11.h"

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
    py::class_<fiction::offset::ucoord_t>(m, "coordinate")
        .def(py::init<>())
        .def(py::init<const uint64_t, const uint64_t, const uint64_t>(), "x"_a, "y"_a, "z"_a = 0)
        .def(py::init<const fiction::offset::ucoord_t>(), "c"_a)
        .def(py::init(
            [](const py::tuple& t)
            {
                if (const auto size = t.size(); size == 2)
                {
                    return fiction::offset::ucoord_t{py::int_(t[0]), py::int_(t[1])};
                }
                else if (size == 3)
                {
                    return fiction::offset::ucoord_t{py::int_(t[0]), py::int_(t[1]), py::int_(t[2])};
                }

                throw std::runtime_error("Wrong number of dimensions provided for coordinate");
            }))
        .def_property(
            "x", [](fiction::offset::ucoord_t& self) -> decltype(self.x) { return self.x; },
            [](fiction::offset::ucoord_t& self, const decltype(self.x) value) { self.x = value; })
        .def_property(
            "y", [](fiction::offset::ucoord_t& self) -> decltype(self.y) { return self.y; },
            [](fiction::offset::ucoord_t& self, const decltype(self.y) value) { self.y = value; })
        .def_property(
            "z", [](fiction::offset::ucoord_t& self) -> decltype(self.z) { return self.z; },
            [](fiction::offset::ucoord_t& self, const decltype(self.z) value) { self.z = value; })
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self)
        .def(py::self > py::self)
        .def(py::self <= py::self)
        .def(py::self >= py::self)
        .def("__repr__", &fiction::offset::ucoord_t::str);

    py::implicitly_convertible<py::tuple, fiction::offset::ucoord_t>();
}

}  // namespace pyfiction

#endif  // PYFICTION_COORDINATES_HPP
