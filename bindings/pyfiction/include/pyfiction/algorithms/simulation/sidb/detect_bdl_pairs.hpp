//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_DETECT_BDL_PAIRS_HPP
#define PYFICTION_DETECT_BDL_PAIRS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void detect_bdl_pairs(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::bdl_pair<Lyt>>(m, fmt::format("bdl_pair{}", lattice).c_str(), DOC(fiction_bdl_pair))
        .def(py::init<>(), DOC(fiction_bdl_pair_bdl_pair))
        .def(py::init<fiction::sidb_technology::cell_type, fiction::cell<Lyt>, fiction::cell<Lyt>>(), "t"_a, "u"_a,
             "l"_a, DOC(fiction_bdl_pair_bdl_pair_2))
        .def_readonly("type", &fiction::bdl_pair<Lyt>::type, DOC(fiction_bdl_pair_type))
        .def_readonly("upper", &fiction::bdl_pair<Lyt>::upper, DOC(fiction_bdl_pair_upper))
        .def_readonly("lower", &fiction::bdl_pair<Lyt>::lower, DOC(fiction_bdl_pair_lower))

        ;

    m.def(fmt::format("detect_bdl_pairs{}", lattice).c_str(), &fiction::detect_bdl_pairs<Lyt>, "lyt"_a, "type"_a,
          "params"_a = fiction::detect_bdl_pairs_params{}, DOC(fiction_detect_bdl_pairs));
}

}  // namespace detail

inline void detect_bdl_pairs(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::detect_bdl_pairs_params>(m, "detect_bdl_pairs_params", DOC(fiction_detect_bdl_pairs_params))
        .def(py::init<>())
        .def_readwrite("minimum_distance", &fiction::detect_bdl_pairs_params::minimum_distance,
                       DOC(fiction_detect_bdl_pairs_params_minimum_distance))
        .def_readwrite("maximum_distance", &fiction::detect_bdl_pairs_params::maximum_distance,
                       DOC(fiction_detect_bdl_pairs_params_maximum_distance));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::detect_bdl_pairs<py_sidb_100_lattice>(m, "_100");
    detail::detect_bdl_pairs<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_DETECT_BDL_PAIRS_HPP
