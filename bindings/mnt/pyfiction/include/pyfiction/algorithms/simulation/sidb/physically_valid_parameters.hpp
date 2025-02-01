//
// Created by Jan Drewniok on 07.05.24.
//

#ifndef PYFICTION_PHYSICALLY_VALID_PARAMETERS_HPP
#define PYFICTION_PHYSICALLY_VALID_PARAMETERS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/physically_valid_parameters.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void physically_valid_parameters(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("physically_valid_parameters", &fiction::physically_valid_parameters<Lyt>, py::arg("cds"),
          py::arg("params") = fiction::operational_domain_params{}, DOC(fiction_physically_valid_parameters));
}

}  // namespace detail

inline void physically_valid_parameters(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::sidb_simulation_domain<fiction::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(py::init<>())
        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::sidb_simulation_domain<fiction::parameter_point, uint64_t>& domain,
               const fiction::parameter_point&                                            pp)
            {
                const auto result = domain.get_value(pp);

                // Check if the result has a value
                if (result.has_value())
                {
                    return std::get<0>(result.value());
                }
                // If no value is present, raise a Python ValueError
                throw py::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            py::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::physically_valid_parameters<py_charge_distribution_surface_100>(m);
    detail::physically_valid_parameters<py_charge_distribution_surface_111>(m);
}

}  // namespace pyfiction

#endif  // FICTION_PHYSICALLY_VALID_PARAMETERS_HPP
