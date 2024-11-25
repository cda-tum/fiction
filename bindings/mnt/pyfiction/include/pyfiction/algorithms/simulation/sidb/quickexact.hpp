//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_QUICKEXACT_HPP
#define PYFICTION_QUICKEXACT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quickexact(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("quickexact", &fiction::quickexact<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::quickexact_params<fiction::offset::ucoord_t>{}, DOC(fiction_quickexact));
}

}  // namespace detail

inline void quickexact(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    namespace py = pybind11;
    namespace py = pybind11;

    py::enum_<typename fiction::quickexact_params<fiction::offset::ucoord_t>::automatic_base_number_detection>(
        m, "automatic_base_number_detection", DOC(fiction_quickexact_params_automatic_base_number_detection))
        .value("ON", fiction::quickexact_params<fiction::offset::ucoord_t>::automatic_base_number_detection::ON,
               DOC(fiction_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", fiction::quickexact_params<fiction::offset::ucoord_t>::automatic_base_number_detection::OFF,
               DOC(fiction_quickexact_params_automatic_base_number_detection_OFF));

    /**
     * QuickExact parameters.
     */
    py::class_<fiction::quickexact_params<fiction::offset::ucoord_t>>(m, "quickexact_params",
                                                                      DOC(fiction_quickexact_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters",
                       &fiction::quickexact_params<fiction::offset::ucoord_t>::simulation_parameters,
                       DOC(fiction_quickexact_params))
        .def_readwrite("base_number_detection",
                       &fiction::quickexact_params<fiction::offset::ucoord_t>::base_number_detection,
                       DOC(fiction_quickexact_params_base_number_detection))
        .def_readwrite("local_external_potential",
                       &fiction::quickexact_params<fiction::offset::ucoord_t>::local_external_potential,
                       DOC(fiction_quickexact_params_local_external_potential))
        .def_readwrite("global_potential", &fiction::quickexact_params<fiction::offset::ucoord_t>::global_potential,
                       DOC(fiction_quickexact_params_global_potential));

    detail::quickexact<py_sidb_100_lattice>(m);
    detail::quickexact<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_QUICKEXACT_HPP
