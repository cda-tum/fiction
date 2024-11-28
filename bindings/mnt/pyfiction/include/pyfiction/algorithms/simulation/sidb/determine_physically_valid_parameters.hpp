//
// Created by Jan Drewniok on 07.05.24.
//

#ifndef PYFICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP
#define PYFICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/determine_physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void determine_physically_valid_parameters(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("determine_physically_valid_parameters", &fiction::determine_physically_valid_parameters<Lyt>, py::arg("cds"),
          py::arg("params") = fiction::operational_domain_params{}, DOC(fiction_determine_physically_valid_parameters));
}

}  // namespace detail

inline void determine_physically_valid_parameters(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::operational_domain<fiction::parameter_point, uint64_t>>(m, "physically_valid_parameters_domain",
                                                                                DOC(fiction_operational_domain))
        .def(py::init<>())
        .def_readwrite("dimensions", &fiction::operational_domain<fiction::parameter_point, uint64_t>::dimensions)

        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::operational_domain<fiction::parameter_point, uint64_t>& domain,
               const fiction::parameter_point&                                        pp)
            {
                auto result = domain.get_value(pp);

                // Check if the result has a value
                if (result.has_value())
                {
                    return result.value();
                }
                // If no value is present, raise a Python ValueError
                throw py::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            py::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::determine_physically_valid_parameters<py_charge_distribution_surface_100>(m);
    detail::determine_physically_valid_parameters<py_charge_distribution_surface_111>(m);
}

}  // namespace pyfiction

#endif  // FICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP
