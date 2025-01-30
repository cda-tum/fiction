//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_OPERATIONAL_DOMAIN_HPP
#define PYFICTION_OPERATIONAL_DOMAIN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt, typename OpDomain>
void operational_domain(pybind11::module& m, const std::string& metric)
{
    namespace py = pybind11;

    m.def(fmt::format("{}operational_domain_grid_search", metric).c_str(),
          &fiction::operational_domain_grid_search<OpDomain, Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_operational_domain_grid_search));

    m.def(fmt::format("{}operational_domain_random_sampling", metric).c_str(),
          &fiction::operational_domain_random_sampling<OpDomain, Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("samples"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_operational_domain_random_sampling));

    m.def(fmt::format("{}operational_domain_flood_fill", metric).c_str(),
          &fiction::operational_domain_flood_fill<OpDomain, Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("samples"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_operational_domain_flood_fill));

    m.def(fmt::format("{}operational_domain_contour_tracing", metric).c_str(),
          &fiction::operational_domain_contour_tracing<OpDomain, Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("samples"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_operational_domain_contour_tracing));
}

}  // namespace detail

inline void operational_domain(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::parameter_point>(m, "parameter_point", DOC(fiction_parameter_point))
        .def(py::init<>(), DOC(fiction_parameter_point_parameter_point))
        .def(py::init<const std::vector<double>>(), py::arg("values"), DOC(fiction_parameter_point_parameter_point_2))
        .def_readwrite("parameters", &fiction::parameter_point::parameters, DOC(fiction_parameter_point))

        .def(py::self == py::self, py::arg("other"), DOC(fiction_parameter_point_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_parameter_point_operator_ne))

        .def("__hash__",
             [](const fiction::parameter_point& self) { return std::hash<fiction::parameter_point>{}(self); })

        ;

    py::enum_<fiction::sweep_parameter>(m, "sweep_parameter", DOC(fiction_sweep_parameter))
        .value("EPSILON_R", fiction::sweep_parameter::EPSILON_R, DOC(fiction_sweep_parameter_EPSILON_R))
        .value("LAMBDA_TF", fiction::sweep_parameter::LAMBDA_TF, DOC(fiction_sweep_parameter_LAMBDA_TF))
        .value("MU_MINUS", fiction::sweep_parameter::MU_MINUS, DOC(fiction_sweep_parameter_MU_MINUS))

        ;

    // todo update docu
    py::class_<fiction::temperature_operational_domain>(m, "temperature_operational_domain",
                                                        DOC(fiction_operational_domain))
        .def(py::init<>())
        .def_readwrite("dimensions", &fiction::temperature_operational_domain::dimensions,
                       DOC(fiction_operational_domain_dimensions))
        .def_readwrite("domain_values", &fiction::temperature_operational_domain::domain_values)
        .def("get_value", &fiction::temperature_operational_domain::get_value, py::arg("point"))
        .def("add_value", &fiction::temperature_operational_domain::add_value, py::arg("key"), py::arg("value"));

    // todo update docu
    py::class_<fiction::operational_domain>(m, "operational_domain", DOC(fiction_operational_domain))
        .def(py::init<>())
        .def_readwrite("dimensions", &fiction::operational_domain::dimensions,
                       DOC(fiction_operational_domain_dimensions))
        .def_readwrite("domain_values", &fiction::operational_domain::domain_values)
        .def("get_value", &fiction::operational_domain::get_value, py::arg("point"))
        .def("add_value", &fiction::operational_domain::add_value, py::arg("key"), py::arg("value"))
        .def("get_domain", &fiction::operational_domain::get_domain)

        ;

    py::class_<fiction::operational_domain_value_range>(m, "operational_domain_value_range",
                                                        DOC(fiction_operational_domain_value_range))
        .def(py::init<fiction::sweep_parameter>(), py::arg("dimension"))
        .def(py::init<fiction::sweep_parameter, double, double, double>(), py::arg("dimension"), py::arg("min"),
             py::arg("max"), py::arg("step"))
        .def_readwrite("dimension", &fiction::operational_domain_value_range::dimension,
                       DOC(fiction_operational_domain_value_range_dimension))
        .def_readwrite("min", &fiction::operational_domain_value_range::min,
                       DOC(fiction_operational_domain_value_range_dimension))
        .def_readwrite("max", &fiction::operational_domain_value_range::max,
                       DOC(fiction_operational_domain_value_range_max))
        .def_readwrite("step", &fiction::operational_domain_value_range::step,
                       DOC(fiction_operational_domain_value_range_step))

        ;

    py::class_<fiction::operational_domain_params>(m, "operational_domain_params",
                                                   DOC(fiction_operational_domain_params))
        .def(py::init<>())
        .def_readwrite("operational_params", &fiction::operational_domain_params::operational_params,
                       DOC(fiction_operational_domain_params_operational_params))
        .def_readwrite("sweep_dimensions", &fiction::operational_domain_params::sweep_dimensions,
                       DOC(fiction_operational_domain_params_sweep_dimensions));

    py::class_<fiction::operational_domain_stats>(m, "operational_domain_stats", DOC(fiction_operational_domain_stats))
        .def(py::init<>())
        .def_readonly("time_total", &fiction::operational_domain_stats::time_total,
                      DOC(fiction_operational_domain_stats_duration))
        .def_readonly("num_simulator_invocations", &fiction::operational_domain_stats::num_simulator_invocations,
                      DOC(fiction_operational_domain_stats_num_simulator_invocations))
        .def_readonly("num_evaluated_parameter_combinations",
                      &fiction::operational_domain_stats::num_evaluated_parameter_combinations,
                      DOC(fiction_operational_domain_stats_num_evaluated_parameter_combinations))
        .def_readonly("num_operational_parameter_combinations",
                      &fiction::operational_domain_stats::num_operational_parameter_combinations,
                      DOC(fiction_operational_domain_stats_num_operational_parameter_combinations))
        .def_readonly("num_non_operational_parameter_combinations",
                      &fiction::operational_domain_stats::num_non_operational_parameter_combinations,
                      DOC(fiction_operational_domain_stats_num_non_operational_parameter_combinations))
        .def_readonly("num_total_parameter_points", &fiction::operational_domain_stats::num_total_parameter_points,
                      DOC(fiction_operational_domain_stats_num_total_parameter_points))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::operational_domain<py_sidb_100_lattice, fiction::operational_domain>(m, "");
    detail::operational_domain<py_sidb_111_lattice, fiction::operational_domain>(m, "");

    detail::operational_domain<py_sidb_111_lattice, fiction::temperature_operational_domain>(m, "");
    detail::operational_domain<py_sidb_111_lattice, fiction::temperature_operational_domain>(m, "");
}

}  // namespace pyfiction

#endif  // PYFICTION_OPERATIONAL_DOMAIN_HPP
