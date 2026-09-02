#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/result.hpp>

#include <any>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
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

template <typename Lyt>
void sidb_simulation_result_impl(nanobind::module_& m, const std::string& lattice = "")
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::result<Lyt>>(m, fmt::format("sidb_simulation_result{}", lattice).c_str(),
                                                       DOC(fiction_sidb_simulation_result))
        .def(py::init<>(), "Default constructor.")
        .def_rw("algorithm_name", &fiction::sidb::simulation::result<Lyt>::algorithm_name,
                DOC(fiction_sidb_simulation_result_algorithm_name))
        .def_rw("simulation_runtime", &fiction::sidb::simulation::result<Lyt>::simulation_runtime,
                DOC(fiction_sidb_simulation_result_simulation_runtime))
        .def_rw("charge_distributions", &fiction::sidb::simulation::result<Lyt>::charge_distributions,
                DOC(fiction_sidb_simulation_result_charge_distributions))
        .def_rw("simulation_parameters", &fiction::sidb::simulation::result<Lyt>::sim_params,
                DOC(fiction_sidb_simulation_result_sim_params))
        .def_prop_ro(
            "additional_simulation_parameters", [](const fiction::sidb::simulation::result<Lyt>& self)
            { return convert_map_to_py(self.additional_simulation_parameters); },
            DOC(fiction_sidb_simulation_result_additional_simulation_parameters))
        .def("groundstates", &fiction::sidb::simulation::result<Lyt>::groundstates,
             DOC(fiction_sidb_simulation_result_groundstates))

        ;
}

}  // namespace detail

void result(nanobind::module_& m)
{
    // Define simulation result for specific lattices
    detail::sidb_simulation_result_impl<py_sidb_100_lattice>(m, "_100");
    detail::sidb_simulation_result_impl<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction
