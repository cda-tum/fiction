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

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quickexact(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<typename fiction::quickexact_params<Lyt>::automatic_base_number_detection>(
        m, "automatic_base_number_detection", DOC(fiction_quickexact_params_automatic_base_number_detection))
        .value("ON", fiction::quickexact_params<Lyt>::automatic_base_number_detection::ON,
               DOC(fiction_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", fiction::quickexact_params<Lyt>::automatic_base_number_detection::OFF,
               DOC(fiction_quickexact_params_automatic_base_number_detection_OFF))

        ;

    /**
     * QuickExact parameters.
     */
    py::class_<fiction::quickexact_params<Lyt>>(m, "quickexact_params", DOC(fiction_quickexact_params))
        .def(py::init<>())
        .def_readwrite("physical_parameters", &fiction::quickexact_params<Lyt>::physical_parameters,
                       DOC(fiction_quickexact_params_physical_parameters))
        .def_readwrite("base_number_detection", &fiction::quickexact_params<Lyt>::base_number_detection,
                       DOC(fiction_quickexact_params_base_number_detection))
        .def_readwrite("local_external_potential", &fiction::quickexact_params<Lyt>::local_external_potential,
                       DOC(fiction_quickexact_params_local_external_potential))
        .def_readwrite("global_potential", &fiction::quickexact_params<Lyt>::global_potential,
                       DOC(fiction_quickexact_params_global_potential))

        ;

    m.def(
        "quickexact",
        [](const Lyt&                                      lyt,
           const typename fiction::quickexact_params<Lyt>& params) -> fiction::sidb_simulation_result<py_cds_base>
        {
            fiction::quickexact_params<py_cds_base> converted_params{
                params.physical_parameters,
                params.base_number_detection == fiction::quickexact_params<Lyt>::automatic_base_number_detection::ON ?
                    fiction::quickexact_params<py_cds_base>::automatic_base_number_detection::ON :
                    fiction::quickexact_params<py_cds_base>::automatic_base_number_detection::OFF,
                params.local_external_potential, params.global_potential};

            return fiction::quickexact<py_cds_base>(lyt, converted_params);
        },
        "lyt"_a, "params"_a = fiction::quickexact_params<Lyt>{}, DOC(fiction_quickexact));
}

}  // namespace detail

inline void quickexact(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::quickexact<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_QUICKEXACT_HPP
