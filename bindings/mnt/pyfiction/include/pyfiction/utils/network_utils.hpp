//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_NETWORK_UTILS_HPP
#define PYFICTION_NETWORK_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/network_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void has_high_degree_fanin_nodes(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("has_high_degree_fanin_nodes", &fiction::has_high_degree_fanin_nodes<Ntk>, py::arg("ntk"),
          py::arg("threshold") = 2, DOC(fiction_has_high_degree_fanin_nodes));
}

}  // namespace detail

inline void network_utils(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::high_degree_fanin_exception>(m, "high_degree_fanin_exception", PyExc_ValueError);

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::has_high_degree_fanin_nodes<py_logic_network>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_NETWORK_UTILS_HPP
