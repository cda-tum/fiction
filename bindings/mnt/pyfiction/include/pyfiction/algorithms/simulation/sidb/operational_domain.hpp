//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_OPERATIONAL_DOMAIN_HPP
#define PYFICTION_OPERATIONAL_DOMAIN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>

#include <fmt/format.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void operational_domain(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("operational_domain_grid_search", &fiction::operational_domain_grid_search<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_operational_domain_grid_search));

    m.def("operational_domain_random_sampling", &fiction::operational_domain_random_sampling<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("samples"), py::arg("params") = fiction::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_operational_domain_random_sampling));

    m.def("operational_domain_flood_fill", &fiction::operational_domain_flood_fill<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"), py::arg("params") = fiction::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_operational_domain_flood_fill));

    m.def("operational_domain_contour_tracing", &fiction::operational_domain_contour_tracing<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("samples"), py::arg("params") = fiction::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_operational_domain_contour_tracing));
}

template <typename Lyt>
void critical_temperature_domain(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("critical_temperature_domain_grid_search", &fiction::critical_temperature_domain_grid_search<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_critical_temperature_domain_grid_search));

    m.def("critical_temperature_domain_random_sampling",
          &fiction::critical_temperature_domain_random_sampling<Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("samples"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_domain_random_sampling));

    m.def("critical_temperature_domain_flood_fill", &fiction::critical_temperature_domain_flood_fill<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("samples"), py::arg("params") = fiction::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_critical_temperature_domain_flood_fill));

    m.def("critical_temperature_domain_contour_tracing",
          &fiction::critical_temperature_domain_contour_tracing<Lyt, py_tt>, py::arg("lyt"), py::arg("spec"),
          py::arg("samples"), py::arg("params") = fiction::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_domain_contour_tracing));
}

}  // namespace detail

inline void operational_domain(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::parameter_point>(m, "parameter_point", DOC(fiction_parameter_point))
        .def(py::init<>(), DOC(fiction_parameter_point_parameter_point))
        .def(py::init<const std::vector<double>>(), py::arg("values"), DOC(fiction_parameter_point_parameter_point_2))
        .def("get_parameters", &fiction::parameter_point::get_parameters, DOC(fiction_parameter_point_get_parameters))

        .def(py::self == py::self, py::arg("other"), DOC(fiction_parameter_point_operator_eq))
        .def(py::self != py::self, py::arg("other"), DOC(fiction_parameter_point_operator_ne))

        .def("__hash__",
             [](const fiction::parameter_point& self) { return std::hash<fiction::parameter_point>{}(self); })
        .def("__str__", [](const fiction::parameter_point& self) { return fmt::format("{}", self.get_parameters()); })
        .def("__getitem__",
             [](const fiction::parameter_point& self, const std::size_t index) { return self.get_parameters()[index]; })

        ;

    py::enum_<fiction::sweep_parameter>(m, "sweep_parameter", DOC(fiction_sweep_parameter))
        .value("EPSILON_R", fiction::sweep_parameter::EPSILON_R, DOC(fiction_sweep_parameter_EPSILON_R))
        .value("LAMBDA_TF", fiction::sweep_parameter::LAMBDA_TF, DOC(fiction_sweep_parameter_LAMBDA_TF))
        .value("MU_MINUS", fiction::sweep_parameter::MU_MINUS, DOC(fiction_sweep_parameter_MU_MINUS))

        ;

    py::class_<fiction::critical_temperature_domain>(m, "critical_temperature_domain",
                                                     DOC(fiction_critical_temperature_domain))
        .def(py::init<>())
        .def(py::init<const std::vector<fiction::sweep_parameter>>(), py::arg("dims"))
        .def("get_dimension", &fiction::critical_temperature_domain::get_dimension, py::arg("index"),
             DOC(fiction_critical_temperature_domain_get_dimension))
        .def("get_number_of_dimensions", &fiction::critical_temperature_domain::get_number_of_dimensions,
             DOC(fiction_critical_temperature_domain_get_number_of_dimensions))
        .def("minimum_ct", &fiction::critical_temperature_domain::minimum_ct,
             DOC(fiction_critical_temperature_domain_minimum_ct))
        .def("maximum_ct", &fiction::critical_temperature_domain::maximum_ct,
             DOC(fiction_critical_temperature_domain_maximum_ct))

        // Pythonic interface functions
        .def("__getitem__",
             [](const fiction::critical_temperature_domain& self, const fiction::parameter_point& key)
             {
                 const auto val = self.contains(key);
                 if (!val.has_value())
                 {
                     throw py::key_error("Key not found");
                 }
                 return val.value();
             })
        .def("__setitem__",
             [](fiction::critical_temperature_domain& self, const fiction::parameter_point& key,
                const std::tuple<fiction::operational_status, double>& value) { self.add_value(key, value); })
        .def("__contains__", [](const fiction::critical_temperature_domain& self, const fiction::parameter_point& key)
             { return self.contains(key).has_value(); })
        .def("__len__", [](const fiction::critical_temperature_domain& self) { return self.size(); })
        .def("__iter__",
             [](const fiction::critical_temperature_domain& self)
             {
                 std::vector<fiction::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });

                 const py::list py_keys = py::cast(keys);
                 return py::iter(py_keys);
             })
        .def("keys",
             [](const fiction::critical_temperature_domain& self)
             {
                 std::vector<fiction::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                 return keys;
             })
        .def("values",
             [](const fiction::critical_temperature_domain& self)
             {
                 std::vector<std::tuple<fiction::operational_status, double>> values{};
                 values.reserve(self.size());
                 self.for_each([&values](const auto&, const auto& value) { values.push_back(value); });
                 return values;
             })
        .def("items",
             [](const fiction::critical_temperature_domain& self)
             {
                 std::vector<std::pair<fiction::parameter_point, std::tuple<fiction::operational_status, double>>>
                     items{};
                 items.reserve(self.size());
                 self.for_each([&items](const auto& key, const auto& value) { items.emplace_back(key, value); });
                 return items;
             })

        ;

    py::class_<fiction::operational_domain>(m, "operational_domain", DOC(fiction_operational_domain))
        .def(py::init<>(), DOC(fiction_operational_domain_operational_domain))
        .def(py::init<const std::vector<fiction::sweep_parameter>>(), py::arg("dims"),
             DOC(fiction_operational_domain_operational_domain_2))
        .def("add_dimension", &fiction::operational_domain::add_dimension, py::arg("dim"),
             DOC(fiction_operational_domain_add_dimension))
        .def("get_dimension", &fiction::operational_domain::get_dimension, py::arg("index"),
             DOC(fiction_operational_domain_get_dimension))
        .def("get_number_of_dimensions", &fiction::operational_domain::get_number_of_dimensions,
             DOC(fiction_operational_domain_get_number_of_dimensions))

        // Pythonic interface functions
        .def("__getitem__",
             [](const fiction::operational_domain& self, const fiction::parameter_point& key)
             {
                 const auto val = self.contains(key);
                 if (!val.has_value())
                 {
                     throw py::key_error("Key not found");
                 }

                 return std::get<0>(val.value());
             })
        .def("__setitem__", [](fiction::operational_domain& self, const fiction::parameter_point& key,
                               const fiction::operational_status& value) { self.add_value(key, {value}); })
        .def("__contains__", [](const fiction::operational_domain& self, const fiction::parameter_point& key)
             { return self.contains(key).has_value(); })
        .def("__len__", [](const fiction::operational_domain& self) { return self.size(); })
        .def("__iter__",
             [](const fiction::operational_domain& self)
             {
                 std::vector<fiction::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });

                 const py::list py_keys = py::cast(keys);
                 return py::iter(py_keys);
             })
        .def("keys",
             [](const fiction::operational_domain& self)
             {
                 std::vector<fiction::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                 return keys;
             })
        .def("values",
             [](const fiction::operational_domain& self)
             {
                 std::vector<fiction::operational_status> values{};
                 values.reserve(self.size());
                 self.for_each([&values](const auto&, const auto& value) { values.push_back(std::get<0>(value)); });
                 return values;
             })
        .def("items",
             [](const fiction::operational_domain& self)
             {
                 std::vector<std::pair<fiction::parameter_point, fiction::operational_status>> items{};
                 items.reserve(self.size());
                 self.for_each([&items](const auto& key, const auto& value)
                               { items.emplace_back(key, std::get<0>(value)); });
                 return items;
             })

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

    detail::operational_domain<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::operational_domain<py_sidb_111_lattice<py_offset_coordinate>>(m);

    detail::critical_temperature_domain<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::critical_temperature_domain<py_sidb_111_lattice<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_OPERATIONAL_DOMAIN_HPP
