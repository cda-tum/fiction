//
// Created by Jan Drewniok on 13.09.24.
//

#ifndef PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP
#define PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/compute_operational_ratio.hpp>

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
    using namespace pybind11::literals;

    m.def("compute_operational_ratio", &fiction::compute_operational_ratio<Lyt, py_tt>, "lyt"_a, "spec"_a, "pp"_a,
          "params"_a = fiction::operational_domain_params{}, DOC(fiction_compute_operational_ratio));
}

}  // namespace detail

inline void compute_operational_ratio(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::compute_operational_ratio_params>(m, "compute_operational_ratio_params",
                                                          DOC(fiction_compute_operational_ratio_params))
        .def(py::init<>())
        .def_readwrite("op_domain_params", &fiction::compute_operational_ratio_params::op_domain_params,
                       DOC(fiction_compute_operational_ratio_params_op_domain_params));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::compute_operational_ratio<py_sidb_100_lattice>(m);
    detail::compute_operational_ratio<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_COMPUTE_OPERATIONAL_RATIO_HPP
