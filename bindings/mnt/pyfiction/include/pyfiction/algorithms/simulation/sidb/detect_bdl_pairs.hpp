//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_DETECT_BDL_PAIRS_HPP
#define PYFICTION_DETECT_BDL_PAIRS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void detect_bdl_pairs(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("detect_bdl_pairs", &fiction::detect_bdl_pairs<Lyt>, py::arg("lyt"), py::arg("type") = std::nullopt,
          py::arg("params") = fiction::detect_bdl_pairs_params{}, DOC(fiction_detect_bdl_pairs));
}

}  // namespace detail

inline void detect_bdl_pairs(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::bdl_pair<fiction::offset::ucoord_t>>(m, "bdl_pair", DOC(fiction_bdl_pair))
        .def(py::init<>(), DOC(fiction_bdl_pair_bdl_pair))
        .def(py::init<fiction::sidb_technology::cell_type, fiction::offset::ucoord_t, fiction::offset::ucoord_t>(),
             py::arg("t"), py::arg("u"), py::arg("l"), DOC(fiction_bdl_pair_bdl_pair_2))
        .def_readonly("type", &fiction::bdl_pair<fiction::offset::ucoord_t>::type, DOC(fiction_bdl_pair_type))
        .def_readonly("upper", &fiction::bdl_pair<fiction::offset::ucoord_t>::upper, DOC(fiction_bdl_pair_upper))
        .def_readonly("lower", &fiction::bdl_pair<fiction::offset::ucoord_t>::lower, DOC(fiction_bdl_pair_lower));

    py::class_<fiction::detect_bdl_pairs_params>(m, "detect_bdl_pairs_params", DOC(fiction_detect_bdl_pairs_params))
        .def(py::init<>())
        .def_readwrite("minimum_distance", &fiction::detect_bdl_pairs_params::minimum_distance,
                       DOC(fiction_detect_bdl_pairs_params_minimum_distance))
        .def_readwrite("maximum_distance", &fiction::detect_bdl_pairs_params::maximum_distance,
                       DOC(fiction_detect_bdl_pairs_params_maximum_distance))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::detect_bdl_pairs<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::detect_bdl_pairs<py_sidb_111_lattice<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DETECT_BDL_PAIRS_HPP
