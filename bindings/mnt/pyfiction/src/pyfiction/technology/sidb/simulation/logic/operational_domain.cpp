/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/operational_domain.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <fmt/format.h>

#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
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
void operational_domain_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("operational_domain_grid_search",
          &fiction::sidb::simulation::logic::operational_domain_grid_search<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_sidb_simulation_logic_operational_domain_grid_search));

    m.def("operational_domain_random_sampling",
          &fiction::sidb::simulation::logic::operational_domain_random_sampling<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_operational_domain_random_sampling));

    m.def("operational_domain_flood_fill", &fiction::sidb::simulation::logic::operational_domain_flood_fill<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_operational_domain_flood_fill));

    m.def("operational_domain_contour_tracing",
          &fiction::sidb::simulation::logic::operational_domain_contour_tracing<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_operational_domain_contour_tracing));
}

template <typename Lyt>
void critical_temperature_domain_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("critical_temperature_domain_grid_search",
          &fiction::sidb::simulation::logic::critical_temperature_domain_grid_search<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{},
          py::arg("stats") = nullptr, DOC(fiction_sidb_simulation_logic_critical_temperature_domain_grid_search));

    m.def("critical_temperature_domain_random_sampling",
          &fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_critical_temperature_domain_random_sampling));

    m.def("critical_temperature_domain_flood_fill",
          &fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_critical_temperature_domain_flood_fill));

    m.def("critical_temperature_domain_contour_tracing",
          &fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("samples"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{}, py::arg("stats") = nullptr,
          DOC(fiction_sidb_simulation_logic_critical_temperature_domain_contour_tracing));
}

}  // namespace detail

void operational_domain(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::logic::parameter_point>(m, "parameter_point",
                                                                  DOC(fiction_sidb_simulation_logic_parameter_point))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_parameter_point_parameter_point))
        .def(py::init<const std::vector<double>>(), py::arg("values"),
             DOC(fiction_sidb_simulation_logic_parameter_point_parameter_point_2))
        .def("get_parameters", &fiction::sidb::simulation::logic::parameter_point::get_parameters,
             DOC(fiction_sidb_simulation_logic_parameter_point_get_parameters))

        // NOLINTBEGIN(misc-redundant-expression)
        .def(py::self == py::self, py::arg("other"), DOC(fiction_sidb_simulation_logic_parameter_point_operator_eq))
        // `parameter_point` no longer declares an `operator!=`; C++20 rewrites `a != b` as `!(a == b)`, so the
        // binding still works, but there is no generated docstring to reference for it anymore
        .def(py::self != py::self, py::arg("other"),
             "Inequality operator. Checks if this parameter point is not equal to another point within the "
             "tolerance defined by `constants::ERROR_MARGIN`.\n\n"
             ":param other: Other parameter point to compare with.\n"
             ":return: ``True`` iff the parameter points are not equal.")
        // NOLINTEND(misc-redundant-expression)

        .def(
            "__hash__", [](const fiction::sidb::simulation::logic::parameter_point& self)
            { return std::hash<fiction::sidb::simulation::logic::parameter_point>{}(self); },
            "Returns a hash value of the parameter point.")
        .def(
            "__str__", [](const fiction::sidb::simulation::logic::parameter_point& self)
            { return fmt::format("{}", self.get_parameters()); },
            "Returns a string representation of the parameter point.")
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::parameter_point& self, const std::size_t index)
            {
                const auto& parameters = self.get_parameters();
                if (index >= parameters.size())
                {
                    throw py::index_error("parameter index out of range");
                }
                return parameters[index];
            },
            "Returns the value of the parameter at the given index. Raises `IndexError` if the index is out of range.")

        ;

    py::enum_<fiction::sidb::simulation::logic::sweep_parameter>(m, "sweep_parameter",
                                                                 DOC(fiction_sidb_simulation_logic_sweep_parameter))
        .value("EPSILON_R", fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_EPSILON_R))
        .value("LAMBDA_TF", fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_LAMBDA_TF))
        .value("MU_MINUS", fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_MU_MINUS))

        ;

    py::class_<fiction::sidb::simulation::logic::critical_temperature_domain>(
        m, "critical_temperature_domain", DOC(fiction_sidb_simulation_logic_critical_temperature_domain))
        .def(py::init<>(), "Default constructor.")
        .def(py::init<const std::vector<fiction::sidb::simulation::logic::sweep_parameter>>(), py::arg("dims"),
             "Constructs a critical temperature domain with the given sweep dimensions.")
        .def("get_dimension", &fiction::sidb::simulation::logic::critical_temperature_domain::get_dimension,
             py::arg("index"), DOC(fiction_sidb_simulation_logic_critical_temperature_domain_get_dimension))
        .def("get_number_of_dimensions",
             &fiction::sidb::simulation::logic::critical_temperature_domain::get_number_of_dimensions,
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_get_number_of_dimensions))
        .def("minimum_ct", &fiction::sidb::simulation::logic::critical_temperature_domain::minimum_ct,
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_minimum_ct))
        .def("maximum_ct", &fiction::sidb::simulation::logic::critical_temperature_domain::maximum_ct,
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_maximum_ct))

        // Pythonic interface functions
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self,
               const fiction::sidb::simulation::logic::parameter_point&             key)
            {
                const auto val = self.contains(key);
                if (!val.has_value())
                {
                    throw py::key_error("Key not found");
                }
                return val.value();
            },
            "Returns the value stored for the given parameter point, raising a KeyError if it does not exist.")
        .def(
            "__setitem__",
            [](fiction::sidb::simulation::logic::critical_temperature_domain&                  self,
               const fiction::sidb::simulation::logic::parameter_point&                        key,
               const std::tuple<fiction::sidb::simulation::logic::operational_status, double>& value)
            { self.add_value(key, value); },
            "Sets the value stored for the given parameter point.")
        .def(
            "__contains__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self,
               const fiction::sidb::simulation::logic::parameter_point& key) { return self.contains(key).has_value(); },
            "Checks whether the given parameter point is contained in the domain.")
        .def(
            "__len__", [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            { return self.size(); }, "Returns the number of parameter points stored in the domain.")
        .def(
            "__iter__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                keys.reserve(self.size());
                self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });

                const py::object py_keys = py::cast(keys);
                return py::iter(py_keys);
            },
            "Returns an iterator over the parameter points stored in the domain.")
        .def("keys",
             [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
             {
                 std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                 return keys;
             })
        .def("values",
             [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
             {
                 std::vector<std::tuple<fiction::sidb::simulation::logic::operational_status, double>> values{};
                 values.reserve(self.size());
                 self.for_each([&values](const auto&, const auto& value) { values.push_back(value); });
                 return values;
             })
        .def("items",
             [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
             {
                 std::vector<std::pair<fiction::sidb::simulation::logic::parameter_point,
                                       std::tuple<fiction::sidb::simulation::logic::operational_status, double>>>
                     items{};
                 items.reserve(self.size());
                 self.for_each([&items](const auto& key, const auto& value) { items.emplace_back(key, value); });
                 return items;
             })

        ;

    py::class_<fiction::sidb::simulation::logic::operational_domain>(
        m, "operational_domain", DOC(fiction_sidb_simulation_logic_operational_domain))
        .def(py::init<>(), DOC(fiction_sidb_simulation_logic_operational_domain_operational_domain))
        .def(py::init<const std::vector<fiction::sidb::simulation::logic::sweep_parameter>>(), py::arg("dims"),
             DOC(fiction_sidb_simulation_logic_operational_domain_operational_domain_2))
        .def("add_dimension", &fiction::sidb::simulation::logic::operational_domain::add_dimension, py::arg("dim"),
             DOC(fiction_sidb_simulation_logic_operational_domain_add_dimension))
        .def("get_dimension", &fiction::sidb::simulation::logic::operational_domain::get_dimension, py::arg("index"),
             DOC(fiction_sidb_simulation_logic_operational_domain_get_dimension))
        .def("get_number_of_dimensions",
             &fiction::sidb::simulation::logic::operational_domain::get_number_of_dimensions,
             DOC(fiction_sidb_simulation_logic_operational_domain_get_number_of_dimensions))

        // Pythonic interface functions
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::operational_domain& self,
               const fiction::sidb::simulation::logic::parameter_point&    key)
            {
                const auto val = self.contains(key);
                if (!val.has_value())
                {
                    throw py::key_error("Key not found");
                }

                return std::get<0>(val.value());
            },
            "Returns the operational status stored for the given parameter point, raising a KeyError if it does "
            "not exist.")
        .def(
            "__setitem__",
            [](fiction::sidb::simulation::logic::operational_domain&       self,
               const fiction::sidb::simulation::logic::parameter_point&    key,
               const fiction::sidb::simulation::logic::operational_status& value) { self.add_value(key, {value}); },
            "Sets the operational status stored for the given parameter point.")
        .def(
            "__contains__",
            [](const fiction::sidb::simulation::logic::operational_domain& self,
               const fiction::sidb::simulation::logic::parameter_point& key) { return self.contains(key).has_value(); },
            "Checks whether the given parameter point is contained in the domain.")
        .def(
            "__len__", [](const fiction::sidb::simulation::logic::operational_domain& self) { return self.size(); },
            "Returns the number of parameter points stored in the domain.")
        .def(
            "__iter__",
            [](const fiction::sidb::simulation::logic::operational_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                keys.reserve(self.size());
                self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });

                const py::object py_keys = py::cast(keys);
                return py::iter(py_keys);
            },
            "Returns an iterator over the parameter points stored in the domain.")
        .def("keys",
             [](const fiction::sidb::simulation::logic::operational_domain& self)
             {
                 std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                 keys.reserve(self.size());
                 self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                 return keys;
             })
        .def("values",
             [](const fiction::sidb::simulation::logic::operational_domain& self)
             {
                 std::vector<fiction::sidb::simulation::logic::operational_status> values{};
                 values.reserve(self.size());
                 self.for_each([&values](const auto&, const auto& value) { values.push_back(std::get<0>(value)); });
                 return values;
             })
        .def("items",
             [](const fiction::sidb::simulation::logic::operational_domain& self)
             {
                 std::vector<std::pair<fiction::sidb::simulation::logic::parameter_point,
                                       fiction::sidb::simulation::logic::operational_status>>
                     items{};
                 items.reserve(self.size());
                 self.for_each([&items](const auto& key, const auto& value)
                               { items.emplace_back(key, std::get<0>(value)); });
                 return items;
             })

        ;

    py::class_<fiction::sidb::simulation::logic::operational_domain_value_range>(
        m, "operational_domain_value_range", DOC(fiction_sidb_simulation_logic_operational_domain_value_range))
        .def(py::init<fiction::sidb::simulation::logic::sweep_parameter>(), py::arg("dimension"))
        .def(py::init<fiction::sidb::simulation::logic::sweep_parameter, double, double, double>(),
             py::arg("dimension"), py::arg("min"), py::arg("max"), py::arg("step"))
        .def_rw("dimension", &fiction::sidb::simulation::logic::operational_domain_value_range::dimension,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_dimension))
        .def_rw("min", &fiction::sidb::simulation::logic::operational_domain_value_range::min,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_min))
        .def_rw("max", &fiction::sidb::simulation::logic::operational_domain_value_range::max,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_max))
        .def_rw("step", &fiction::sidb::simulation::logic::operational_domain_value_range::step,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_step))

        ;

    py::class_<fiction::sidb::simulation::logic::operational_domain_params>(
        m, "operational_domain_params", DOC(fiction_sidb_simulation_logic_operational_domain_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_params", &fiction::sidb::simulation::logic::operational_domain_params::operational_params,
                DOC(fiction_sidb_simulation_logic_operational_domain_params_operational_params))
        .def_rw("sweep_dimensions", &fiction::sidb::simulation::logic::operational_domain_params::sweep_dimensions,
                DOC(fiction_sidb_simulation_logic_operational_domain_params_sweep_dimensions))
        .def_rw("number_of_threads", &fiction::sidb::simulation::logic::operational_domain_params::number_of_threads,
                DOC(fiction_sidb_simulation_logic_operational_domain_params_number_of_threads));

    py::class_<fiction::sidb::simulation::logic::operational_domain_stats>(
        m, "operational_domain_stats", DOC(fiction_sidb_simulation_logic_operational_domain_stats))
        .def(py::init<>(), "Default constructor.")
        .def_ro("time_total", &fiction::sidb::simulation::logic::operational_domain_stats::time_total,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_time_total))
        .def_ro("num_simulator_invocations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_simulator_invocations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_simulator_invocations))
        .def_ro("num_evaluated_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_evaluated_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_evaluated_parameter_combinations))
        .def_ro("num_operational_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_operational_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_operational_parameter_combinations))
        .def_ro("num_non_operational_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_non_operational_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_non_operational_parameter_combinations))
        .def_ro("num_total_parameter_points",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_total_parameter_points,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_total_parameter_points))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::operational_domain_impl<py_sidb_100_lattice>(m);
    detail::operational_domain_impl<py_sidb_111_lattice>(m);

    detail::critical_temperature_domain_impl<py_sidb_100_lattice>(m);
    detail::critical_temperature_domain_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
