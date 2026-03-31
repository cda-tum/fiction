#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_domain.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void physically_valid_parameters_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("physically_valid_parameters", &fiction::physically_valid_parameters<Lyt>, py::arg("cds"),
          py::arg("params") = fiction::operational_domain_params{}, DOC(fiction_physically_valid_parameters));
}

}  // namespace detail

void physically_valid_parameters(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb_simulation_domain<fiction::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(py::init<>())
        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::sidb_simulation_domain<fiction::parameter_point, uint64_t>& domain,
               const fiction::parameter_point&                                            pp)
            {
                if (const auto result = domain.contains(pp); result.has_value())
                {
                    return std::get<0>(result.value());
                }
                // If no value is present, raise a Python ValueError
                throw py::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            py::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::physically_valid_parameters_impl<py_charge_distribution_surface_100>(m);
    detail::physically_valid_parameters_impl<py_charge_distribution_surface_111>(m);
}

}  // namespace pyfiction
