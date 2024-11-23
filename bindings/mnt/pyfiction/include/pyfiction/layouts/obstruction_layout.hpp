//
// Created by marcel on 09.02.23.
//

#ifndef PYFICTION_OBSTRUCTION_LAYOUT_HPP
#define PYFICTION_OBSTRUCTION_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstruction_layout.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename ObstrLyt>
void obstruction_layout(pybind11::module& m, const std::string& topology)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<ObstrLyt, LytBase>(m, fmt::format("{}_obstruction_layout", topology).c_str(),
                                  DOC(fiction_obstruction_layout))
        .def(py::init<>())
        .def(py::init<const LytBase&>(), "layout"_a, DOC(fiction_obstruction_layout_obstruction_layout))

        .def("obstruct_coordinate", &ObstrLyt::obstruct_coordinate, "c"_a)
        .def("obstruct_connection", &ObstrLyt::obstruct_connection, "src"_a, "tgt"_a)
        .def("is_obstructed_coordinate", &ObstrLyt::is_obstructed_coordinate, "c"_a)
        .def("is_obstructed_connection", &ObstrLyt::is_obstructed_connection, "src"_a, "tgt"_a)
        .def("clear_obstructed_coordinate", &ObstrLyt::clear_obstructed_coordinate, "c"_a)
        .def("clear_obstructed_connection", &ObstrLyt::clear_obstructed_connection, "src"_a, "tgt"_a)
        .def("clear_obstructed_coordinates", &ObstrLyt::clear_obstructed_coordinates)
        .def("clear_obstructed_connections", &ObstrLyt::clear_obstructed_connections)

        ;
}

}  // namespace detail

inline void obstruction_layouts(pybind11::module& m)
{
    /**
     * Cartesian obstruction layout.
     */
    detail::obstruction_layout<py_cartesian_gate_layout, py_cartesian_obstruction_layout>(m, "cartesian");
    /**
     * Shifted Cartesian obstruction layout.
     */
    detail::obstruction_layout<py_shifted_cartesian_gate_layout, py_shifted_cartesian_obstruction_layout>(
        m, "shifted_cartesian");
    /**
     * Hexagonal obstruction layout.
     */
    detail::obstruction_layout<py_hexagonal_gate_layout, py_hexagonal_obstruction_layout>(m, "hexagonal");
}

}  // namespace pyfiction

#endif  // PYFICTION_OBSTRUCTION_LAYOUT_HPP
