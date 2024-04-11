//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_QUICKEXACT_HPP
#define PYFICTION_QUICKEXACT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quickexact(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<typename fiction::quickexact_params<Lyt>::automatic_base_number_detection>(
        m, fmt::format("automatic_base_number_detection{}", lattice).c_str(),
        DOC(fiction_quickexact_params_automatic_base_number_detection))
        .value("ON", fiction::quickexact_params<Lyt>::automatic_base_number_detection::ON,
               DOC(fiction_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", fiction::quickexact_params<Lyt>::automatic_base_number_detection::OFF,
               DOC(fiction_quickexact_params_automatic_base_number_detection_OFF))

        ;

    /**
     * QuickExact parameters.
     */
    py::class_<fiction::quickexact_params<Lyt>>(m, fmt::format("quickexact_params{}", lattice).c_str(),
                                                DOC(fiction_quickexact_params))
        .def(py::init<>())
        .def_readwrite(fmt::format("simulation_parameters{}", lattice).c_str(),
                       &fiction::quickexact_params<Lyt>::simulation_parameters,
                       DOC(fiction_quickexact_params_physical_parameters))
        .def_readwrite(fmt::format("base_number_detection{}", lattice).c_str(),
                       &fiction::quickexact_params<Lyt>::base_number_detection,
                       DOC(fiction_quickexact_params_base_number_detection))
        .def_readwrite(fmt::format("local_external_potential{}", lattice).c_str(),
                       &fiction::quickexact_params<Lyt>::local_external_potential,
                       DOC(fiction_quickexact_params_local_external_potential))
        .def_readwrite(fmt::format("global_potential{}", lattice).c_str(),
                       &fiction::quickexact_params<Lyt>::global_potential,
                       DOC(fiction_quickexact_params_global_potential))

        ;

    m.def(fmt::format("quickexact{}", lattice).c_str(), &fiction::quickexact<Lyt>, "lyt"_a,
          "params"_a = fiction::quickexact_params<Lyt>{}, DOC(fiction_quickexact));
}

}  // namespace detail

inline void quickexact(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::quickexact<py_sidb_100_lattice>(m, "_100");
    detail::quickexact<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_QUICKEXACT_HPP
