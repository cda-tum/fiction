//
// Created by simon on 18.10.23.
//

#ifndef PYFICTION_TECHNOLOGY_MAPPING_HPP
#define PYFICTION_TECHNOLOGY_MAPPING_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/network_transformation/technology_mapping.hpp>

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

/**
 * Performs technology mapping on the given network.
 */
inline void technology_mapping(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::technology_mapping_params>(m, "technology_mapping_params",
                                                   DOC(fiction_technology_mapping_params))
        .def(py::init<>())
        .def_readwrite("decay", &fiction::technology_mapping_params::decay,
                       DOC(fiction_technology_mapping_params_decay))
        .def_readwrite("inv", &fiction::technology_mapping_params::inv, DOC(fiction_technology_mapping_params_inv))
        .def_readwrite("and2", &fiction::technology_mapping_params::and2, DOC(fiction_technology_mapping_params_and2))
        .def_readwrite("nand2", &fiction::technology_mapping_params::nand2,
                       DOC(fiction_technology_mapping_params_nand2))
        .def_readwrite("or2", &fiction::technology_mapping_params::or2, DOC(fiction_technology_mapping_params_or2))
        .def_readwrite("nor2", &fiction::technology_mapping_params::nor2, DOC(fiction_technology_mapping_params_nor2))
        .def_readwrite("xor2", &fiction::technology_mapping_params::xor2, DOC(fiction_technology_mapping_params_xor2))
        .def_readwrite("xnor2", &fiction::technology_mapping_params::xnor2,
                       DOC(fiction_technology_mapping_params_xnor2))
        .def_readwrite("and3", &fiction::technology_mapping_params::and3, DOC(fiction_technology_mapping_params_and3))
        .def_readwrite("xor_and", &fiction::technology_mapping_params::xor_and,
                       DOC(fiction_technology_mapping_params_xor_and))
        .def_readwrite("or_and", &fiction::technology_mapping_params::or_and,
                       DOC(fiction_technology_mapping_params_or_and))
        .def_readwrite("maj3", &fiction::technology_mapping_params::maj3, DOC(fiction_technology_mapping_params_maj3))
        .def_readwrite("gamble", &fiction::technology_mapping_params::gamble,
                       DOC(fiction_technology_mapping_params_gamble))
        .def_readwrite("dot", &fiction::technology_mapping_params::dot, DOC(fiction_technology_mapping_params_dot))
        .def_readwrite("mux", &fiction::technology_mapping_params::mux, DOC(fiction_technology_mapping_params_mux))
        .def_readwrite("and_xor", &fiction::technology_mapping_params::and_xor,
                       DOC(fiction_technology_mapping_params_and_xor));

    py::class_<fiction::technology_mapping_stats>(m, "technology_mapping_stats", DOC(fiction_technology_mapping_stats))
        .def(py::init<>());

    m.def(
        "and_or_not", []() { return fiction::and_or_not(); }, DOC(fiction_and_or_not));
    m.def(
        "and_or_not_maj", []() { return fiction::and_or_not_maj(); }, DOC(fiction_and_or_not_maj));
    m.def(
        "all_standard_2_input_functions", []() { return fiction::all_standard_2_input_functions(); },
        DOC(fiction_all_standard_2_input_functions));
    m.def(
        "all_standard_3_input_functions", []() { return fiction::all_standard_3_input_functions(); },
        DOC(fiction_all_standard_3_input_functions));
    m.def(
        "all_supported_standard_functions", []() { return fiction::all_supported_standard_functions(); },
        DOC(fiction_all_supported_standard_functions));

    m.def("technology_mapping", &fiction::technology_mapping<py_logic_network>, "network"_a,
          "parameters"_a = fiction::technology_mapping_params{}, "statistics"_a = nullptr,
          DOC(fiction_technology_mapping));
}

}  // namespace pyfiction

#endif  // PYFICTION_TECHNOLOGY_MAPPING_HPP
