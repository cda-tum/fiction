//
// Created by Jan Drewniok on 17.07.24.
//

#ifndef PYFICTION_DETECT_BDL_WIRES_HPP
#define PYFICTION_DETECT_BDL_WIRES_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>  // Included for std::string
#include <vector>

namespace pyfiction
{

// todo update documentation

namespace detail
{

/**
 * @brief Registers the `bdl_wire` class and `detect_bdl_wires` function for a specific lattice type.
 *
 * @tparam Lyt The lattice type.
 * @param m The pybind11 module.
 * @param suffix The suffix to append to the class and function names to distinguish between different lattice types.
 */
template <typename Lyt>
void detect_bdl_wires(pybind11::module& m, const std::string& suffix = "")
{
    using namespace pybind11::literals;
    namespace py = pybind11;

    using bdl_wire_t = fiction::bdl_wire<Lyt>;

    // Construct the class name based on the suffix
    std::string class_name = "bdl_wire";
    if (!suffix.empty())
    {
        class_name += "_" + suffix;
    }

    // Register the bdl_wire class with the appropriate suffix
    py::class_<bdl_wire_t>(m, class_name.c_str(), DOC(fiction_bdl_wire))
        .def(py::init<>())                                                                  // Default constructor
        .def(py::init<std::vector<fiction::bdl_pair<fiction::offset::ucoord_t>>>(), "p"_a)  // Constructor with pairs
        .def_readwrite("pairs", &bdl_wire_t::pairs, DOC(fiction_bdl_wire_pairs))            // Expose the pairs member
        .def_readwrite("direction", &bdl_wire_t::direction,
                       DOC(fiction_bdl_wire_direction))  // Expose the direction member
        .def_readwrite("start_bdl_pair_wire",
                       &bdl_wire_t::start_bdl_pair_wire)                      // Expose the start_bdl_pair_wire member
        .def_readwrite("end_bdl_pair_wire", &bdl_wire_t::end_bdl_pair_wire);  // Expose the end_bdl_pair_wire member

    // Register the detect_bdl_wires function with a unique name based on the suffix
    std::string func_name = "detect_bdl_wires";
    if (!suffix.empty())
    {
        func_name += "_" + suffix;
    }

    m.def(func_name.c_str(), &fiction::detect_bdl_wires<Lyt>, "lyt"_a, "params"_a = fiction::detect_bdl_wires_params{},
          "wire_selection"_a = fiction::bdl_wire_selection::ALL, DOC(fiction_detect_bdl_wires));
}

}  // namespace detail

/**
 * @brief Registers all `bdl_wire` classes, enums, and related functions with the pybind11 module.
 *
 * @param m The pybind11 module.
 */
inline void detect_bdl_wires(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    // Enum for wire selection options
    py::enum_<fiction::bdl_wire_selection>(m, "bdl_wire_selection", DOC(fiction_bdl_wire_selection))
        .value("ALL", fiction::bdl_wire_selection::ALL, DOC(fiction_bdl_wire_selection_ALL))
        .value("INPUT", fiction::bdl_wire_selection::INPUT, DOC(fiction_bdl_wire_selection_INPUT))
        .value("OUTPUT", fiction::bdl_wire_selection::OUTPUT, DOC(fiction_bdl_wire_selection_OUTPUT))
        .export_values();  // Export enum values to Python namespace

    // Class for detect_bdl_wires_params
    py::class_<fiction::detect_bdl_wires_params>(m, "detect_bdl_wires_params", DOC(fiction_detect_bdl_wires_params))
        .def(py::init<>())  // Default constructor
        .def_readwrite("threshold_bdl_interdistance", &fiction::detect_bdl_wires_params::threshold_bdl_interdistance,
                       DOC(fiction_detect_bdl_wires_params_threshold_bdl_interdistance))
        .def_readwrite("bdl_pairs_params", &fiction::detect_bdl_wires_params::bdl_pairs_params,
                       DOC(fiction_detect_bdl_wires_params_bdl_pairs_params));

    // Register different lattice types with appropriate suffixes
    detail::detect_bdl_wires<py_sidb_100_lattice>(m, "100");
    detail::detect_bdl_wires<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction

#endif  // PYFICTION_DETECT_BDL_WIRES_HPP
