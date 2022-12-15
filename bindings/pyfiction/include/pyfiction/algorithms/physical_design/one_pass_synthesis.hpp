//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_ONE_PASS_SYNTHESIS_HPP
#define PYFICTION_ONE_PASS_SYNTHESIS_HPP

#if (MUGEN)

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/one_pass_synthesis.hpp>

#include <optional>
#include <sstream>

namespace pyfiction
{

/**
 * SMT-based one_pass_synthesis physical design.
 */
inline void one_pass_synthesis(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::one_pass_synthesis_params>(m, "one_pass_synthesis_params")
        .def(py::init<>())
        .def_readwrite("scheme", &fiction::one_pass_synthesis_params::scheme)
        .def_readwrite("upper_bound", &fiction::one_pass_synthesis_params::upper_bound)
        .def_readwrite("fixed_size", &fiction::one_pass_synthesis_params::fixed_size)
        .def_readwrite("num_threads", &fiction::one_pass_synthesis_params::num_threads)
        .def_readwrite("crossings", &fiction::one_pass_synthesis_params::crossings)
        .def_readwrite("enable_wires", &fiction::one_pass_synthesis_params::enable_wires)
        .def_readwrite("enable_not", &fiction::one_pass_synthesis_params::enable_not)
        .def_readwrite("enable_and", &fiction::one_pass_synthesis_params::enable_and)
        .def_readwrite("enable_or", &fiction::one_pass_synthesis_params::enable_or)
        .def_readwrite("enable_maj", &fiction::one_pass_synthesis_params::enable_maj)
        .def_readwrite("enable_maj", &fiction::one_pass_synthesis_params::enable_maj)
        .def_readwrite("timeout", &fiction::one_pass_synthesis_params::timeout)

        ;

    py::class_<fiction::one_pass_synthesis_stats>(m, "one_pass_synthesis_stats")
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::one_pass_synthesis_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    std::optional<py_cartesian_gate_layout> (*one_pass_function_pointer)(
        const fiction::tec_nt&, fiction::one_pass_synthesis_params, fiction::one_pass_synthesis_stats*) =
        &fiction::one_pass_synthesis<py_cartesian_gate_layout, py_logic_network>;

    m.def("one_pass_synthesis", one_pass_function_pointer, "network"_a,
          "parameters"_a = fiction::one_pass_synthesis_params{}, "statistics"_a = nullptr);
}

}  // namespace pyfiction

#else  // MUGEN

#include "pybind11/pybind11.h"

namespace pyfiction
{

/**
 * Disable SMT-based exact physical design.
 */
inline void one_pass_synthesis([[maybe_unused]] pybind11::module& m) {}

}  // namespace pyfiction

#endif  // MUGEN

#endif  // PYFICTION_ONE_PASS_SYNTHESIS_HPP
