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
    using namespace pybind11::literals;

    m.def("get_name", &fiction::get_name<NtkOrLyt>, "ntk_or_lyt"_a, DOC(fiction_get_name));
}

template <typename NtkOrLyt>
void set_name(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("set_name", &fiction::set_name<NtkOrLyt>, "ntk_or_lyt"_a, "name"_a, DOC(fiction_set_name));
}

}  // namespace detail

inline void name_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::get_name<py_cartesian_obstruction_layout>(m);
    detail::get_name<py_cartesian_gate_layout>(m);
    detail::get_name<py_shifted_cartesian_obstruction_layout>(m);
    detail::get_name<py_shifted_cartesian_gate_layout>(m);
    detail::get_name<py_hexagonal_obstruction_layout>(m);
    detail::get_name<py_hexagonal_gate_layout>(m);
    detail::get_name<py_qca_layout>(m);
    detail::get_name<py_inml_layout>(m);
    detail::get_name<py_sidb_layout>(m);
    detail::get_name<py_logic_network>(m);

    detail::set_name<py_cartesian_obstruction_layout>(m);
    detail::set_name<py_cartesian_gate_layout>(m);
    detail::set_name<py_shifted_cartesian_obstruction_layout>(m);
    detail::set_name<py_shifted_cartesian_gate_layout>(m);
    detail::set_name<py_hexagonal_obstruction_layout>(m);
    detail::set_name<py_hexagonal_gate_layout>(m);
    detail::set_name<py_qca_layout>(m);
    detail::set_name<py_inml_layout>(m);
    detail::set_name<py_sidb_layout>(m);
    detail::set_name<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_NAME_UTILS_HPP
