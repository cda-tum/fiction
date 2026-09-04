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
 * @brief Python bindings for `fiction/technology/sidb/simulation/result.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/result.hpp>

#include <fmt/format.h>

#include <any>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <nanobind/nanobind.h>
// These headers register nanobind type casters without exposing directly referenced symbols.
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

namespace py = nanobind;

// Helper function to convert std::any to Python objects
inline py::object convert_any_to_py(const std::any& value)
{
    try
    {
        if (value.type() == typeid(int))
        {
            return py::int_(std::any_cast<int>(value));
        }
        if (value.type() == typeid(double))
        {
            return py::float_(std::any_cast<double>(value));
        }
        if (value.type() == typeid(bool))
        {
            return py::bool_(std::any_cast<bool>(value));
        }
        if (value.type() == typeid(std::string))
        {
            return py::str(std::any_cast<std::string>(value).c_str());
        }
        if (value.type() == typeid(uint64_t))
        {
            return py::int_(std::any_cast<uint64_t>(value));
        }
    }
    catch (const std::bad_any_cast& e)
    {
        throw std::runtime_error(std::string("Error casting std::any: ") + e.what());
    }

    throw std::runtime_error(std::string("Unsupported type in std::any: ") + value.type().name());
}

inline py::dict convert_map_to_py(const std::unordered_map<std::string, std::any>& map)
{
    nanobind::dict result;

    for (const auto& [key, value] : map)
    {
        try
        {
            result[key.c_str()] = convert_any_to_py(value);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("Error converting key: {}", e.what()));
        }
    }

    return result;
}

/**
 * Transitional binding of `legacy_result` over one Cartesian SiDB cell-level layout type; it goes away once every
 * consumer takes `sidb_simulation_result`.
 */
template <typename Lyt>
void legacy_result_impl(nanobind::module_& m, const std::string& lattice)
{
    using legacy = fiction::sidb::simulation::legacy_result<Lyt>;

    py::class_<legacy>(m, fmt::format("sidb_simulation_result{}", lattice).c_str(),
                       DOC(fiction_sidb_simulation_legacy_result))
        .def(py::init<>(), "Default constructor.")
        .def_rw("algorithm_name", &legacy::algorithm_name, DOC(fiction_sidb_simulation_legacy_result_algorithm_name))
        .def_rw("simulation_runtime", &legacy::simulation_runtime,
                DOC(fiction_sidb_simulation_legacy_result_simulation_runtime))
        .def_rw("charge_distributions", &legacy::charge_distributions,
                DOC(fiction_sidb_simulation_legacy_result_charge_distributions))
        .def_rw("simulation_parameters", &legacy::sim_params, DOC(fiction_sidb_simulation_legacy_result_sim_params))
        .def_prop_ro(
            "additional_simulation_parameters",
            [](const legacy& self) { return convert_map_to_py(self.additional_simulation_parameters); },
            DOC(fiction_sidb_simulation_legacy_result_additional_simulation_parameters))
        .def("groundstates", &legacy::groundstates, DOC(fiction_sidb_simulation_legacy_result_groundstates))

        ;
}

}  // namespace detail

void result(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::simulation::result;

    py::class_<result>(m, "sidb_simulation_result", DOC(fiction_sidb_simulation_result))
        .def(py::init<>(), "Default constructor.")
        .def_rw("algorithm_name", &result::algorithm_name, DOC(fiction_sidb_simulation_result_algorithm_name))
        .def_rw("simulation_runtime", &result::simulation_runtime,
                DOC(fiction_sidb_simulation_result_simulation_runtime))
        .def_rw("layout", &result::lyt, DOC(fiction_sidb_simulation_result_lyt))
        .def_rw("charge_distributions", &result::charge_distributions,
                DOC(fiction_sidb_simulation_result_charge_distributions))
        .def_rw("simulation_parameters", &result::sim_params, DOC(fiction_sidb_simulation_result_sim_params))
        .def_prop_ro(
            "additional_simulation_parameters",
            [](const result& self) { return detail::convert_map_to_py(self.additional_simulation_parameters); },
            DOC(fiction_sidb_simulation_result_additional_simulation_parameters))
        .def("charge_state", &result::charge_state, py::arg("distribution"), py::arg("site"),
             DOC(fiction_sidb_simulation_result_charge_state))
        .def("groundstates", &result::groundstates, DOC(fiction_sidb_simulation_result_groundstates))

        ;

    detail::legacy_result_impl<py_sidb_100_lattice>(m, "_100");
    detail::legacy_result_impl<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction
