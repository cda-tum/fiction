//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename NtkOrLyt>
void get_name(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("get_name", &fiction::networks::get_name<NtkOrLyt>, py::arg("ntk_or_lyt"), DOC(fiction_networks_get_name));
}

template <typename NtkOrLyt>
void set_name(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("set_name", &fiction::networks::set_name<NtkOrLyt>, py::arg("ntk_or_lyt"), py::arg("name"),
          DOC(fiction_networks_set_name));
}

}  // namespace detail

void name_utils(nanobind::module_& m)
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
