//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_CRITICAL_TEMPERATURE_HPP
#define PYFICTION_CRITICAL_TEMPERATURE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void critical_temperature(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("critical_temperature_gate_based", &fiction::critical_temperature_gate_based<Lyt, py_tt>, py::arg("lyt"),
          py::arg("spec"), py::arg("params") = fiction::critical_temperature_params{}, py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_gate_based));

    m.def("critical_temperature_non_gate_based", &fiction::critical_temperature_non_gate_based<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::critical_temperature_params{}, py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_non_gate_based));
}

}  // namespace detail

inline void critical_temperature(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * Critical temperature statistics.
     */
    py::class_<fiction::critical_temperature_stats>(m, "critical_temperature_stats",
                                                    DOC(fiction_critical_temperature_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::critical_temperature_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def("report", &fiction::critical_temperature_stats::report, DOC(fiction_critical_temperature_stats_report))
        .def_readonly("algorithm_name", &fiction::critical_temperature_stats::algorithm_name,
                      DOC(fiction_critical_temperature_stats_algorithm_name))
        .def_readonly("num_valid_lyt", &fiction::critical_temperature_stats::num_valid_lyt,
                      DOC(fiction_critical_temperature_stats_num_valid_lyt))
        .def_readonly("is_ground_state_transparent",
                      &fiction::critical_temperature_stats::energy_between_ground_state_and_first_erroneous,
                      DOC(fiction_critical_temperature_stats_energy_between_ground_state_and_first_erroneous))

        ;

    /**
     * Critical temperature parameters.
     */
    py::class_<fiction::critical_temperature_params>(m, "critical_temperature_params",
                                                     DOC(fiction_critical_temperature_params))
        .def(py::init<>())
        .def_readwrite("operational_params", &fiction::critical_temperature_params::operational_params,
                       DOC(fiction_critical_temperature_params))
        .def_readwrite("confidence_level", &fiction::critical_temperature_params::confidence_level,
                       DOC(fiction_critical_temperature_params_confidence_level))
        .def_readwrite("max_temperature", &fiction::critical_temperature_params::max_temperature,
                       DOC(fiction_critical_temperature_params_max_temperature))
        .def_readwrite("iteration_steps", &fiction::critical_temperature_params::iteration_steps,
                       DOC(fiction_critical_temperature_params_iteration_steps))
        .def_readwrite("alpha", &fiction::critical_temperature_params::alpha,
                       DOC(fiction_critical_temperature_params_alpha))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::critical_temperature<py_sidb_100_lattice>(m);
    detail::critical_temperature<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CRITICAL_TEMPERATURE_HPP
