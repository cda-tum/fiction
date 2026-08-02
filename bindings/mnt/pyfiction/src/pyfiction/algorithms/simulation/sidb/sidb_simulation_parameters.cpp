#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>

#include <fmt/format.h>

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void sidb_simulation_parameters(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb_simulation_parameters>(m, "sidb_simulation_parameters",
                                                    DOC(fiction_sidb_simulation_parameters))
        .def(py::init<const uint8_t, const double, const double, const double>(), py::arg("base_number") = 3,
             py::arg("mu_minus") = -0.32, py::arg("relative_permittivity") = 5.6, py::arg("screening_distance") = 5.0,
             DOC(fiction_sidb_simulation_parameters_sidb_simulation_parameters))
        .def(py::init<>(), DOC(fiction_sidb_simulation_parameters_sidb_simulation_parameters))
        .def_rw("epsilon_r", &fiction::sidb_simulation_parameters::epsilon_r,
                DOC(fiction_sidb_simulation_parameters_epsilon_r))
        .def_rw("lambda_tf", &fiction::sidb_simulation_parameters::lambda_tf,
                DOC(fiction_sidb_simulation_parameters_lambda_tf))
        .def_rw("mu_minus", &fiction::sidb_simulation_parameters::mu_minus,
                DOC(fiction_sidb_simulation_parameters_mu_minus))
        .def_rw("base", &fiction::sidb_simulation_parameters::base, DOC(fiction_sidb_simulation_parameters_base))
        .def("k", &fiction::sidb_simulation_parameters::k, DOC(fiction_sidb_simulation_parameters_k))
        .def("mu_plus", &fiction::sidb_simulation_parameters::mu_plus, DOC(fiction_sidb_simulation_parameters_mu_plus));
}

}  // namespace pyfiction
