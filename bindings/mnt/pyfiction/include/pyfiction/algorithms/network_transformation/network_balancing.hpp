//
// Created by marcel on 25.01.23.
//

#ifndef PYFICTION_NETWORK_BALANCING_HPP
#define PYFICTION_NETWORK_BALANCING_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/network_balancing.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network_balancing(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("network_balancing", &fiction::network_balancing<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_network_balancing));

    m.def("is_balanced", &fiction::is_balanced<Ntk>, py::arg("network"),
          py::arg("params") = fiction::network_balancing_params{}, DOC(fiction_is_balanced));
}

}  // namespace detail

inline void network_balancing(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::network_balancing_params>(m, "network_balancing_params", DOC(fiction_network_balancing_params))
        .def(py::init<>())
        .def_readwrite("unify_outputs", &fiction::network_balancing_params::unify_outputs,
                       DOC(fiction_network_balancing_params_unify_outputs))

        ;

    detail::network_balancing<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_NETWORK_BALANCING_HPP
