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

#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void operational_domain(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("operational_domain_grid_search", &fiction::operational_domain_grid_search<Lyt, py_tt>, "lyt"_a, "spec"_a,
          "params"_a = fiction::operational_domain_params{}, "stats"_a = nullptr,
          DOC(fiction_operational_domain_grid_search));

    m.def("operational_domain_random_sampling", &fiction::operational_domain_random_sampling<Lyt, py_tt>, "lyt"_a,
          "spec"_a, "samples"_a, "params"_a = fiction::operational_domain_params{}, "stats"_a = nullptr,
          DOC(fiction_operational_domain_random_sampling));

    m.def("operational_domain_flood_fill", &fiction::operational_domain_flood_fill<Lyt, py_tt>, "lyt"_a, "spec"_a,
          "samples"_a, "params"_a = fiction::operational_domain_params{}, "stats"_a = nullptr,
          DOC(fiction_operational_domain_flood_fill));

    m.def("operational_domain_contour_tracing", &fiction::operational_domain_contour_tracing<Lyt, py_tt>, "lyt"_a,
          "spec"_a, "samples"_a, "params"_a = fiction::operational_domain_params{}, "stats"_a = nullptr,
          DOC(fiction_operational_domain_contour_tracing));
}

}  // namespace detail

inline void operational_domain(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::sweep_parameter>(m, "sweep_parameter", DOC(fiction_sweep_parameter))
        .value("EPSILON_R", fiction::sweep_parameter::EPSILON_R, DOC(fiction_sweep_parameter_EPSILON_R))
        .value("LAMBDA_TF", fiction::sweep_parameter::LAMBDA_TF, DOC(fiction_sweep_parameter_LAMBDA_TF))
        .value("MU_MINUS", fiction::sweep_parameter::MU_MINUS, DOC(fiction_sweep_parameter_MU_MINUS))

        ;

    py::class_<fiction::parameter_point>(m, "parameter_point", DOC(fiction_parameter_point))
        .def(py::init<>())
        .def(py::init<const std::vector<double>>(), "values"_a)
        .def_readwrite("parameters", &fiction::parameter_point::parameters, DOC(fiction_parameter_point))

        .def(py::self == py::self, "other"_a, DOC(fiction_parameter_point_operator_eq))
        .def(py::self != py::self, "other"_a, DOC(fiction_parameter_point_operator_ne))

        .def("__hash__",
             [](const fiction::parameter_point& self) { return std::hash<fiction::parameter_point>{}(self); })

        ;

    py::class_<fiction::operational_domain<fiction::parameter_point, fiction::operational_status>>(
        m, "operational_domain", DOC(fiction_operational_domain))
        .def(py::init<>())
        .def_readwrite("dimensions",
                       &fiction::operational_domain<fiction::parameter_point, fiction::operational_status>::dimensions)
        .def_readwrite(
            "operational_values",
            &fiction::operational_domain<fiction::parameter_point, fiction::operational_status>::operational_values,
            DOC(fiction_operational_domain_operational_values));

    // add docu
    py::class_<fiction::operational_domain_value_range>(m, "operational_domain_value_range")
        .def(py::init<>())
        .def(py::init<fiction::sweep_parameter, double, double, double>(), "dimension"_a, "min"_a, "max"_a, "step"_a)
        .def_readwrite("dimension", &fiction::operational_domain_value_range::dimension)
        .def_readwrite("min", &fiction::operational_domain_value_range::min)
        .def_readwrite("max", &fiction::operational_domain_value_range::max)
        .def_readwrite("step", &fiction::operational_domain_value_range::step);

    py::class_<fiction::operational_domain_params>(m, "operational_domain_params",
                                                   DOC(fiction_operational_domain_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::operational_domain_params::simulation_parameters,
                       DOC(fiction_operational_domain_params_simulation_parameters))
        .def_readwrite("sim_engine", &fiction::operational_domain_params::sim_engine,
                       DOC(fiction_operational_domain_params_sim_engine))
        .def_readwrite("sweep_dimensions", &fiction::operational_domain_params::sweep_dimensions)
        .def_readwrite("bdl_params", &fiction::operational_domain_params::bdl_params,
                       DOC(fiction_operational_domain_params_bdl_params));

    py::class_<fiction::operational_domain_stats>(m, "operational_domain_stats", DOC(fiction_operational_domain_stats))
        .def(py::init<>())
        .def_readwrite("time_total", &fiction::operational_domain_stats::time_total,
                       DOC(fiction_operational_domain_stats_duration))
        .def_readwrite("num_simulator_invocations", &fiction::operational_domain_stats::num_simulator_invocations,
                       DOC(fiction_operational_domain_stats_num_simulator_invocations))
        .def_readwrite("num_evaluated_parameter_combinations",
                       &fiction::operational_domain_stats::num_evaluated_parameter_combinations,
                       DOC(fiction_operational_domain_stats_num_evaluated_parameter_combinations))
        .def_readwrite("num_operational_parameter_combinations",
                       &fiction::operational_domain_stats::num_operational_parameter_combinations,
                       DOC(fiction_operational_domain_stats_num_operational_parameter_combinations))
        .def_readwrite("num_non_operational_parameter_combinations",
                       &fiction::operational_domain_stats::num_non_operational_parameter_combinations,
                       DOC(fiction_operational_domain_stats_num_non_operational_parameter_combinations));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::operational_domain<py_sidb_100_lattice>(m);
    detail::operational_domain<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_OPERATIONAL_DOMAIN_HPP
