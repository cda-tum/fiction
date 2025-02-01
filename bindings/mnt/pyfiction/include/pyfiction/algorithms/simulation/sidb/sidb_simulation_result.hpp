//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_SIDB_SIMULATION_RESULT_HPP
#define PYFICTION_SIDB_SIMULATION_RESULT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>

#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <any>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace pyfiction
{

namespace detail
{

namespace py = pybind11;

// Helper function to convert std::any to Python objects
inline py::object convert_any_to_py(const std::any& value)
{
    try
    {
        if (value.type() == typeid(int))
        {
            return py::int_(std::any_cast<int>(value));
        }
        else if (value.type() == typeid(double))
        {
            return py::float_(std::any_cast<double>(value));
        }
        else if (value.type() == typeid(bool))
        {
            return py::bool_(std::any_cast<bool>(value));
        }
        else if (value.type() == typeid(std::string))
        {
            return py::str(std::any_cast<std::string>(value));
        }
        else if (value.type() == typeid(uint64_t))
        {
            return pybind11::int_(std::any_cast<uint64_t>(value));
        }
        else
        {
            throw std::runtime_error(std::string("Unsupported type in std::any: ") + value.type().name());
        }
    }
    catch (const std::exception& e)
    {
        throw std::runtime_error(std::string("Error in convert_any_to_py: ") + e.what());
    }
}

inline py::dict convert_map_to_py(const std::unordered_map<std::string, std::any>& map)
{
    pybind11::dict result;
    for (const auto& [key, value] : map)
    {
        try
        {
            result[key.c_str()] = convert_any_to_py(value);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(std::string("Error converting key '") + key + "': " + e.what());
        }
    }
    return result;
}

template <typename Lyt>
void sidb_simulation_result(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;

    py::class_<fiction::sidb_simulation_result<Lyt>>(m, fmt::format("sidb_simulation_result{}", lattice).c_str(),
                                                     DOC(fiction_sidb_simulation_result))
        .def(py::init<>())
        .def_readwrite("algorithm_name", &fiction::sidb_simulation_result<Lyt>::algorithm_name,
                       DOC(fiction_sidb_simulation_result_algorithm_name))
        .def_readwrite("simulation_runtime", &fiction::sidb_simulation_result<Lyt>::simulation_runtime,
                       DOC(fiction_sidb_simulation_result_simulation_runtime))
        .def_readwrite("charge_distributions", &fiction::sidb_simulation_result<Lyt>::charge_distributions,
                       DOC(fiction_sidb_simulation_result_charge_distributions))
        .def_readwrite("simulation_parameters", &fiction::sidb_simulation_result<Lyt>::simulation_parameters,
                       DOC(fiction_sidb_simulation_result_simulation_parameters))
        .def_property_readonly(
            "additional_simulation_parameters", [](const fiction::sidb_simulation_result<Lyt>& self)
            { return convert_map_to_py(self.additional_simulation_parameters); },
            DOC(fiction_sidb_simulation_result_additional_simulation_parameters));
}

}  // namespace detail

inline void sidb_simulation_result(pybind11::module& m)
{
    // Define simulation result for specific lattices
    detail::sidb_simulation_result<py_sidb_100_lattice>(m, "_100");
    detail::sidb_simulation_result<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_SIMULATION_RESULT_HPP
