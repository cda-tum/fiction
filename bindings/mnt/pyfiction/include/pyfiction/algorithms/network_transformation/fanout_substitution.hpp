//
// Created by marcel on 10.01.23.
//

#ifndef PYFICTION_FANOUT_SUBSTITUTION_HPP
#define PYFICTION_FANOUT_SUBSTITUTION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void fanout_substitution(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("fanout_substitution", &fiction::fanout_substitution<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::fanout_substitution_params{}, DOC(fiction_fanout_substitution));

    m.def("is_fanout_substituted", &fiction::is_fanout_substituted<Ntk>, py::arg("network"),
          py::arg("params") = fiction::fanout_substitution_params{}, DOC(fiction_is_fanout_substituted));
}

}  // namespace detail

inline void fanout_substitution(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::fanout_substitution_params::substitution_strategy>(
        m, "substitution_strategy", DOC(fiction_fanout_substitution_params_substitution_strategy))
        .value("BREADTH", fiction::fanout_substitution_params::substitution_strategy::BREADTH,
               DOC(fiction_fanout_substitution_params_substitution_strategy_BREADTH))
        .value("DEPTH", fiction::fanout_substitution_params::substitution_strategy::DEPTH,
               DOC(fiction_fanout_substitution_params_substitution_strategy_DEPTH))

        ;

    py::class_<fiction::fanout_substitution_params>(m, "fanout_substitution_params",
                                                    DOC(fiction_fanout_substitution_params))
        .def(py::init<>())
        .def_readwrite("strategy", &fiction::fanout_substitution_params::strategy,
                       DOC(fiction_fanout_substitution_params_strategy))
        .def_readwrite("degree", &fiction::fanout_substitution_params::degree,
                       DOC(fiction_fanout_substitution_params_degree))
        .def_readwrite("threshold", &fiction::fanout_substitution_params::threshold,
                       DOC(fiction_fanout_substitution_params_threshold))

        ;

    detail::fanout_substitution<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_FANOUT_SUBSTITUTION_HPP
