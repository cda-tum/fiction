//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_NAME_UTILS_HPP
#define PYFICTION_NAME_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/name_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename NtkOrLyt>
void get_name(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("get_name", &fiction::get_name<NtkOrLyt>, py::arg("ntk_or_lyt"), DOC(fiction_get_name));
}

template <typename NtkOrLyt>
void set_name(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("set_name", &fiction::set_name<NtkOrLyt>, py::arg("ntk_or_lyt"), py::arg("name"), DOC(fiction_set_name));
}

}  // namespace detail

inline void name_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::get_name<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::get_name<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::get_name<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::get_name<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::get_name<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::get_name<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::get_name<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::get_name<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::get_name<py_hexagonal_obstruction_layout<py_offset_coordinate>>(m);
    detail::get_name<py_hexagonal_obstruction_layout<py_cube_coordinate>>(m);
    detail::get_name<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
    detail::get_name<py_hexagonal_gate_layout<py_cube_coordinate>>(m);
    detail::get_name<py_qca_layout<py_offset_coordinate>>(m);
    detail::get_name<py_qca_layout<py_cube_coordinate>>(m);
    detail::get_name<py_inml_layout<py_offset_coordinate>>(m);
    detail::get_name<py_inml_layout<py_cube_coordinate>>(m);
    detail::get_name<py_sidb_layout<py_offset_coordinate>>(m);
    detail::get_name<py_sidb_layout<py_cube_coordinate>>(m);
    detail::get_name<py_logic_network>(m);

    detail::set_name<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::set_name<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::set_name<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::set_name<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::set_name<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::set_name<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::set_name<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::set_name<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::set_name<py_hexagonal_obstruction_layout<py_offset_coordinate>>(m);
    detail::set_name<py_hexagonal_obstruction_layout<py_cube_coordinate>>(m);
    detail::set_name<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
    detail::set_name<py_hexagonal_gate_layout<py_cube_coordinate>>(m);
    detail::set_name<py_qca_layout<py_offset_coordinate>>(m);
    detail::set_name<py_qca_layout<py_cube_coordinate>>(m);
    detail::set_name<py_inml_layout<py_offset_coordinate>>(m);
    detail::set_name<py_inml_layout<py_cube_coordinate>>(m);
    detail::set_name<py_sidb_layout<py_offset_coordinate>>(m);
    detail::set_name<py_sidb_layout<py_cube_coordinate>>(m);
    detail::set_name<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_NAME_UTILS_HPP
