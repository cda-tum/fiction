//
// Created by Jan Drewniok on 17.07.24.
//

#ifndef PYFICTION_DETECT_BDL_WIRES_HPP
#define PYFICTION_DETECT_BDL_WIRES_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void detect_bdl_wires(pybind11::module& m, const std::string& lattice = "")
{
    using namespace pybind11::literals;
    namespace py = pybind11;

    using bdl_wire_t = fiction::bdl_wire<Lyt>;

    py::class_<bdl_wire_t>(m, fmt::format("bdl_wire_{}", lattice).c_str(), DOC(fiction_bdl_wire))
        .def(py::init<>(), DOC(fiction_bdl_wire_bdl_wire))
        .def(py::init<std::vector<fiction::bdl_pair<fiction::offset::ucoord_t>>>(), "p"_a,
             DOC(fiction_bdl_wire_bdl_wire_2))
        .def_readwrite("pairs", &bdl_wire_t::pairs, DOC(fiction_bdl_wire_pairs))
        .def_readwrite("direction", &bdl_wire_t::port, DOC(fiction_bdl_wire_port))
        .def_readwrite("first_bdl_pair", &bdl_wire_t::first_bdl_pair_wire,
                       DOC(fiction_bdl_wire_first_bdl_pair_wire))
        .def_readwrite("last_bdl_pair", &bdl_wire_t::last_bdl_pair_wire, DOC(fiction_bdl_wire_last_bdl_pair_wire));

    m.def(fmt::format("detect_bdl_wires_{}", lattice).c_str(), &fiction::detect_bdl_wires<Lyt>, "lyt"_a,
          "params"_a = fiction::detect_bdl_wires_params{}, "wire_selection"_a = fiction::bdl_wire_selection::ALL,
          DOC(fiction_detect_bdl_wires));
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
        .def(py::init<>(), DOC(fiction_detect_bdl_wires_params))
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
