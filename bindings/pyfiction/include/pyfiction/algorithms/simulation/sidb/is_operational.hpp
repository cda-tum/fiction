//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_IS_OPERATIONAL_HPP
#define PYFICTION_IS_OPERATIONAL_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_operational(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("is_operational", &fiction::is_operational<Lyt, py_tt>, "lyt"_a, "spec"_a,
          "params"_a = fiction::is_operational_params{}, DOC(fiction_is_operational));

    m.def("operational_input_patterns", &fiction::operational_input_patterns<Lyt, py_tt>, "lyt"_a, "spec"_a,
          "params"_a = fiction::is_operational_params{}, DOC(fiction_is_operational));
}

}  // namespace detail

inline void is_operational(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::operational_status>(m, "operational_status", DOC(fiction_operational_status))
        .value("OPERATIONAL", fiction::operational_status::OPERATIONAL, DOC(fiction_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::operational_status::NON_OPERATIONAL,
               DOC(fiction_operational_status_NON_OPERATIONAL));

    // TODO add docu
    py::enum_<fiction::operational_condition>(m, "operational_condition")
        .value("ALLOWING_KINKS", fiction::operational_condition::ALLOWING_KINKS)
        .value("FORBIDDING_KINKS", fiction::operational_condition::FORBIDDING_KINKS);

    py::class_<fiction::is_operational_params>(m, "is_operational_params", DOC(fiction_is_operational_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::is_operational_params::simulation_parameters,
                       DOC(fiction_is_operational_params))
        .def_readwrite("sim_engine", &fiction::is_operational_params::sim_engine,
                       DOC(fiction_is_operational_params_sim_engine))
        .def_readwrite("bdl_wire_params", &fiction::is_operational_params::bdl_wire_params)
        .def_readwrite("op_condition", &fiction::is_operational_params::op_condition);

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_operational<py_sidb_100_lattice>(m);
    detail::is_operational<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_IS_OPERATIONAL_HPP
