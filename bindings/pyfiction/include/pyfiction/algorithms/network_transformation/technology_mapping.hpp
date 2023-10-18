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

    py::class_<fiction::technology_mapping_params>(m, "technology_mapping_params")
        .def(py::init<>())
        .def_readwrite("decay", &fiction::technology_mapping_params::decay)
        .def_readwrite("inv", &fiction::technology_mapping_params::inv)
        .def_readwrite("and2", &fiction::technology_mapping_params::and2)
        .def_readwrite("nand2", &fiction::technology_mapping_params::nand2)
        .def_readwrite("or2", &fiction::technology_mapping_params::or2)
        .def_readwrite("nor2", &fiction::technology_mapping_params::nor2)
        .def_readwrite("xor2", &fiction::technology_mapping_params::xor2)
        .def_readwrite("xnor2", &fiction::technology_mapping_params::xnor2)
        .def_readwrite("and3", &fiction::technology_mapping_params::and3)
        .def_readwrite("xor_and", &fiction::technology_mapping_params::xor_and)
        .def_readwrite("or_and", &fiction::technology_mapping_params::or_and)
        .def_readwrite("maj3", &fiction::technology_mapping_params::maj3)
        .def_readwrite("gamble", &fiction::technology_mapping_params::gamble)
        .def_readwrite("dot", &fiction::technology_mapping_params::dot)
        .def_readwrite("mux", &fiction::technology_mapping_params::mux)
        .def_readwrite("and_xor", &fiction::technology_mapping_params::and_xor);

    py::class_<fiction::technology_mapping_stats>(m, "technology_mapping_stats").def(py::init<>());

    m.def("and_or_not", []() { return fiction::and_or_not(); });
    m.def("and_or_not_maj", []() { return fiction::and_or_not_maj(); });
    m.def("all_standard_2_input_functions", []() { return fiction::all_standard_2_input_functions(); });
    m.def("all_standard_3_input_functions", []() { return fiction::all_standard_3_input_functions(); });
    m.def("all_supported_standard_functions", []() { return fiction::all_supported_standard_functions(); });

    m.def("technology_mapping", &fiction::technology_mapping<py_logic_network>, "network"_a,
          "parameters"_a = fiction::technology_mapping_params{}, "statistics"_a = nullptr);
}

}  // namespace pyfiction

#endif  // PYFICTION_TECHNOLOGY_MAPPING_HPP
