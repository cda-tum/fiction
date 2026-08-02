#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_domain.hpp>

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void physically_valid_parameters_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("physically_valid_parameters", &fiction::physically_valid_parameters<Lyt>, py::arg("cds"),
          py::arg("params") = fiction::operational_domain_params{}, DOC(fiction_physically_valid_parameters));
}

}  // namespace detail

void physically_valid_parameters(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb_simulation_domain<fiction::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(py::init<>(), "Default constructor.")
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
