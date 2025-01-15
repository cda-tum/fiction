//
// Created by Jan Drewniok on 13.09.24.
//

#ifndef PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP
#define PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain_ratio.hpp>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void compute_operational_ratio(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("operational_domain_ratio", &fiction::operational_domain_ratio<Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("pp"), py::arg("params") = fiction::operational_domain_ratio_params{},
          DOC(fiction_operational_domain_ratio));
}

}  // namespace detail

inline void compute_operational_ratio(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::operational_domain_ratio_params>(m, "operational_domain_ratio_params",
                                                         DOC(fiction_operational_domain_ratio_params))
        .def(py::init<>())
        .def_readwrite("op_domain_params", &fiction::operational_domain_ratio_params::op_domain_params,
                       DOC(fiction_operational_domain_ratio_params_op_domain_params));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::compute_operational_ratio<py_sidb_100_lattice>(m);
    detail::compute_operational_ratio<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP
