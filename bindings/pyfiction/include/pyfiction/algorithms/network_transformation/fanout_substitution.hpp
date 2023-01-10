//
// Created by marcel on 10.01.23.
//

#ifndef PYFICTION_FANOUT_SUBSTITUTION_HPP
#define PYFICTION_FANOUT_SUBSTITUTION_HPP

#include "pybind11/pybind11.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void fanout_substitution(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("fanout_substitution", &fiction::fanout_substitution<py_logic_network, Ntk>, "network"_a,
          "params"_a = fiction::fanout_substitution_params{});

    m.def("is_fanout_substituted", &fiction::is_fanout_substituted<Ntk>, "network"_a,
          "params"_a = fiction::fanout_substitution_params{});
}

}  // namespace detail

inline void fanout_substitution(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::fanout_substitution_params::substitution_strategy>(m, "substitution_strategy")
        .value("BREADTH", fiction::fanout_substitution_params::substitution_strategy::BREADTH)
        .value("DEPTH", fiction::fanout_substitution_params::substitution_strategy::DEPTH)

        ;

    py::class_<fiction::fanout_substitution_params>(m, "fanout_substitution_params")
        .def(py::init<>())
        .def_readwrite("strategy", &fiction::fanout_substitution_params::strategy)
        .def_readwrite("degree", &fiction::fanout_substitution_params::degree)
        .def_readwrite("threshold", &fiction::fanout_substitution_params::threshold)

        ;

    detail::fanout_substitution<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_FANOUT_SUBSTITUTION_HPP
