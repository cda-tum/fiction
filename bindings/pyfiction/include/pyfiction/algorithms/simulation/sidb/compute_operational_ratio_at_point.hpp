//
// Created by Jan Drewniok on 13.09.24.
//

#ifndef PYFICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP
#define PYFICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/compute_operational_ratio_at_point.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void compute_operational_ratio(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    m.def("compute_operational_ratio_at_point", &fiction::compute_operational_ratio_at_point<Lyt, py_tt>, "lyt"_a,
          "spec"_a, "params"_a, "pp"_a, DOC(fiction_compute_operational_ratio_at_point));
}

}  // namespace detail

inline void compute_operational_ratio(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::compute_operational_ratio_at_point_params>(m, "compute_operational_ratio_at_point_params", DOC(fiction_compute_operational_ratio_at_point_params))
        .def(py::init<>())
        .def_readwrite("operational_domain_params",
                       &fiction::compute_operational_ratio_at_point_params::op_domain_params, DOC(fiction_compute_operational_ratio_at_point_params_op_domain_params));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::compute_operational_ratio<py_sidb_100_lattice>(m);
    detail::compute_operational_ratio<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP
