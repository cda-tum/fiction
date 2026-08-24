//
// Created by marcel on 02.06.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/coordinates.hpp>

#include <cstdint>
#include <stdexcept>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"  // NOLINT(clang-diagnostic-unknown-warning-option)
#pragma GCC diagnostic ignored "-Wconversion"

namespace pyfiction
{

/**
 * Unsigned offset coordinates.
 */
void offset_coordinate(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<py_offset_coordinate>(m, "offset_coordinate", DOC(fiction_offset_ucoord_t))
        .def(py::init<>(), DOC(fiction_offset_ucoord_t_ucoord_t))
        .def(py::init<const uint64_t>(), py::arg("int_repr"), DOC(fiction_offset_ucoord_t_ucoord_t_4))
        .def(py::init<const decltype(py_offset_coordinate().x), const decltype(py_offset_coordinate().y),
                      const decltype(py_offset_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_offset_ucoord_t_ucoord_t_2))
        .def(py::init<const py_offset_coordinate>(), py::arg("c"))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_offset_coordinate> self, const py::tuple& t)
            {
                const auto size = t.size();

                if (size == 2)
                {
                    new (self.p) py_offset_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1]))};
                    return;
                }
                if (size == 3)
                {
                    new (self.p) py_offset_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1])),
                                                      py::int_(py::handle(t[2]))};
                    return;
                }

                throw std::runtime_error("Wrong number of dimensions provided for coordinate");
            },
            py::arg("tuple_repr"))

        .def_prop_rw(
            "x", [](py_offset_coordinate& self) -> decltype(self.x) { return self.x; },
            [](py_offset_coordinate& self, const decltype(self.x) value) { self.x = value; },
            DOC(fiction_offset_ucoord_t_x))
        .def_prop_rw(
            "y", [](py_offset_coordinate& self) -> decltype(self.y) { return self.y; },
            [](py_offset_coordinate& self, const decltype(self.y) value) { self.y = value; },
            DOC(fiction_offset_ucoord_t_y))
        .def_prop_rw(
            "z", [](py_offset_coordinate& self) -> decltype(self.z) { return self.z; },
            [](py_offset_coordinate& self, const decltype(self.z) value) { self.z = value; },
            DOC(fiction_offset_ucoord_t_z))

        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_offset_ucoord_t_operator_ge))
        // NOLINTEND(misc-redundant-expression)

        .def("__repr__", &py_offset_coordinate::str, DOC(fiction_offset_ucoord_t_str))
        .def(
            "__hash__", [](const py_offset_coordinate& self) { return std::hash<py_offset_coordinate>{}(self); },
            "Returns a hash value of the coordinate.")

        ;

    py::implicitly_convertible<py::tuple, py_offset_coordinate>();
}

/**
 * Signed cube coordinates.
 */
void cube_coordinate(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<py_cube_coordinate>(m, "cube_coordinate", DOC(fiction_cube_coord_t))
        .def(py::init<>(), DOC(fiction_cube_coord_t_coord_t))
        .def(py::init<const decltype(py_cube_coordinate().x), const decltype(py_cube_coordinate().y),
                      const decltype(py_cube_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_cube_coord_t_coord_t_2))
        .def(py::init<const py_cube_coordinate>(), py::arg("c"))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_cube_coordinate> self, const py::tuple& t)
            {
                const auto size = t.size();

                if (size == 2)
                {
                    new (self.p) py_cube_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1]))};
                    return;
                }
                if (size == 3)
                {
                    new (self.p) py_cube_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1])),
                                                    py::int_(py::handle(t[2]))};
                    return;
                }

                throw std::runtime_error("Wrong number of dimensions provided for coordinate");
            },
            py::arg("tuple_repr"))

        .def_prop_rw(
            "x", [](py_cube_coordinate& self) -> decltype(self.x) { return self.x; },
            [](py_cube_coordinate& self, const decltype(self.x) value) { self.x = value; }, DOC(fiction_cube_coord_t_x))
        .def_prop_rw(
            "y", [](py_cube_coordinate& self) -> decltype(self.y) { return self.y; },
            [](py_cube_coordinate& self, const decltype(self.y) value) { self.y = value; }, DOC(fiction_cube_coord_t_y))
        .def_prop_rw(
            "z", [](py_cube_coordinate& self) -> decltype(self.z) { return self.z; },
            [](py_cube_coordinate& self, const decltype(self.z) value) { self.z = value; }, DOC(fiction_cube_coord_t_z))

        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_cube_coord_t_operator_ge))
        // NOLINTEND(misc-redundant-expression)

        .def("__repr__", &py_cube_coordinate::str, DOC(fiction_cube_coord_t_str))
        .def(
            "__hash__", [](const py_cube_coordinate& self) { return std::hash<py_cube_coordinate>{}(self); },
            "Returns a hash value of the coordinate.")

        ;

    py::implicitly_convertible<py::tuple, py_cube_coordinate>();
}

/**
 * Signed SiQAD coordinates.
 */
void siqad_coordinate(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<py_siqad_coordinate>(m, "siqad_coordinate", DOC(fiction_siqad_coord_t))
        .def(py::init<>(), DOC(fiction_siqad_coord_t_coord_t))
        .def(py::init<const decltype(py_siqad_coordinate().x), const decltype(py_siqad_coordinate().y),
                      const decltype(py_siqad_coordinate().z)>(),
             py::arg("x"), py::arg("y"), py::arg("z") = 0, DOC(fiction_siqad_coord_t_coord_t_2))
        .def(py::init<const py_siqad_coordinate>(), py::arg("c"))
        .def(
            "__init__",
            [](py::pointer_and_handle<py_siqad_coordinate> self, const py::tuple& t)
            {
                const auto size = t.size();

                if (size == 2)
                {
                    new (self.p) py_siqad_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1]))};
                    return;
                }
                if (size == 3)
                {
                    new (self.p) py_siqad_coordinate{py::int_(py::handle(t[0])), py::int_(py::handle(t[1])),
                                                     py::int_(py::handle(t[2]))};
                    return;
                }

                throw std::runtime_error("Wrong number of dimensions provided for coordinate");
            },
            py::arg("tuple_repr"))

        .def_prop_rw(
            "x", [](py_siqad_coordinate& self) -> decltype(self.x) { return self.x; },
            [](py_siqad_coordinate& self, const decltype(self.x) value) { self.x = value; },
            DOC(fiction_siqad_coord_t_x))
        .def_prop_rw(
            "y", [](py_siqad_coordinate& self) -> decltype(self.y) { return self.y; },
            [](py_siqad_coordinate& self, const decltype(self.y) value) { self.y = value; },
            DOC(fiction_siqad_coord_t_y))
        .def_prop_rw(
            "z", [](py_siqad_coordinate& self) -> decltype(self.z) { return self.z; },
            [](py_siqad_coordinate& self, const decltype(self.z) value) { self.z = value; },
            DOC(fiction_siqad_coord_t_z))

        // NOLINTBEGIN(misc-redundant-expression): nanobind operator bindings intentionally compare placeholder objects.
        .def(py::self == py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_ne))
        .def(py::self < py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_lt))
        .def(py::self > py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_gt))
        .def(py::self <= py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_le))
        .def(py::self >= py::self, py::arg("other"), DOC(fiction_siqad_coord_t_operator_ge))
        // NOLINTEND(misc-redundant-expression)

        .def("__repr__", &py_siqad_coordinate::str, DOC(fiction_siqad_coord_t_str))
        .def(
            "__hash__", [](const py_siqad_coordinate& self) { return std::hash<py_siqad_coordinate>{}(self); },
            "Returns a hash value of the coordinate.")

        ;

    py::implicitly_convertible<py::tuple, py_siqad_coordinate>();
}

void coordinate_utility(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def("offset_area", &fiction::fcn::area<py_offset_coordinate>, py::arg("coord"), DOC(fiction_area));
    m.def("cube_area", &fiction::fcn::area<py_cube_coordinate>, py::arg("coord"), DOC(fiction_area));
    m.def("siqad_area", &fiction::fcn::area<py_siqad_coordinate>, py::arg("coord"), DOC(fiction_area));

    m.def("offset_volume", &fiction::volume<py_offset_coordinate>, py::arg("coord"), DOC(fiction_volume));
    m.def("cube_volume", &fiction::volume<py_cube_coordinate>, py::arg("coord"), DOC(fiction_volume));
    m.def("siqad_volume", &fiction::volume<py_siqad_coordinate>, py::arg("coord"), DOC(fiction_volume));

    m.def("to_offset_coord", &fiction::layouts::coords::to_fiction_coord<py_offset_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_fiction_coord));
    m.def("to_cube_coord", &fiction::layouts::coords::to_fiction_coord<py_cube_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_fiction_coord));

    m.def("to_siqad_coord", &fiction::layouts::coords::to_siqad_coord<py_offset_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_siqad_coord));
    m.def("to_siqad_coord", &fiction::layouts::coords::to_siqad_coord<py_cube_coordinate>, py::arg("coord"),
          DOC(fiction_siqad_to_siqad_coord));
}

}  // namespace pyfiction

#pragma GCC diagnostic pop
