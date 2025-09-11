//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_IS_OPERATIONAL_HPP
#define PYFICTION_IS_OPERATIONAL_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_operational(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("is_operational",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&>(
              &fiction::is_operational<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::is_operational_params{},
          DOC(fiction_is_operational));

    m.def("is_operational",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&,
                            const std::vector<fiction::bdl_wire<Lyt>>&, const std::vector<fiction::bdl_wire<Lyt>>&,
                            const std::optional<Lyt>&>(&fiction::is_operational<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
          py::arg("canvas_lyt") = std::nullopt, DOC(fiction_is_operational_2));

    m.def("operational_input_patterns",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&>(
              &fiction::operational_input_patterns<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::is_operational_params{},
          DOC(fiction_operational_input_patterns));

    m.def("operational_input_patterns",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&,
                            const std::vector<fiction::bdl_wire<Lyt>>&, const std::vector<fiction::bdl_wire<Lyt>>&,
                            const std::optional<Lyt>&>(&fiction::operational_input_patterns<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::is_operational_params{},
          py::arg("input_bdl_wire"), py::arg("output_bdl_wire"), py::arg("canvas_lyt") = std::nullopt,
          DOC(fiction_operational_input_patterns_2));

    m.def("kink_induced_non_operational_input_patterns",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&>(
              &fiction::kink_induced_non_operational_input_patterns<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::is_operational_params{},
          DOC(fiction_kink_induced_non_operational_input_patterns));

    m.def(
        "kink_induced_non_operational_input_patterns",
        py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&,
                          const std::vector<fiction::bdl_wire<Lyt>>&, const std::vector<fiction::bdl_wire<Lyt>>&,
                          const std::optional<Lyt>&>(&fiction::kink_induced_non_operational_input_patterns<Lyt, py_tt>),
        py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
        py::arg("canvas_lyt") = std::nullopt, DOC(fiction_kink_induced_non_operational_input_patterns_2));

    m.def("is_kink_induced_non_operational",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&>(
              &fiction::is_kink_induced_non_operational<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::is_operational_params{},
          DOC(fiction_is_kink_induced_non_operational));

    m.def("is_kink_induced_non_operational",
          py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::is_operational_params&,
                            const std::vector<fiction::bdl_wire<Lyt>>&, const std::vector<fiction::bdl_wire<Lyt>>&,
                            const std::optional<Lyt>&>(&fiction::is_kink_induced_non_operational<Lyt, py_tt>),
          py::arg("lyt"), py::arg("spec"), py::arg("params"), py::arg("input_bdl_wire"), py::arg("output_bdl_wire"),
          py::arg("canvas_lyt") = std::nullopt, DOC(fiction_is_kink_induced_non_operational_2));
}

template <typename Lyt>
void operational_assessment(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;

    // operational assessments are defined to be read-only

    py::class_<typename fiction::operational_assessment<Lyt>::operational_assessment_for_input>(
        m, fmt::format("operational_assessment_for_input{}", lattice).c_str(),
        DOC(fiction_operational_assessment_operational_assessment_for_input))
        .def(py::init<fiction::operational_status>(), py::arg(" op_status"),
             DOC(fiction_operational_assessment_operational_assessment_for_input_operational_assessment_for_input))
        .def_readonly("status", &fiction::operational_assessment<Lyt>::operational_assessment_for_input::status,
                      DOC(fiction_operational_assessment_operational_assessment_for_input_status))
        .def_readonly("simulation_results",
                      &fiction::operational_assessment<Lyt>::operational_assessment_for_input::simulation_results,
                      DOC(fiction_operational_assessment_operational_assessment_for_input_simulation_results));

    py::class_<fiction::operational_assessment<Lyt>>(m, fmt::format("operational_assessment{}", lattice).c_str(),
                                                     DOC(fiction_operational_assessment))
        .def(py::init<fiction::operational_status>(), py::arg("op_status"),
             DOC(fiction_operational_assessment_operational_assessment))
        .def_readonly("status", &fiction::operational_assessment<Lyt>::status,
                      DOC(fiction_operational_assessment_status))
        .def_readonly("assessment_per_input", &fiction::operational_assessment<Lyt>::assessment_per_input,
                      DOC(fiction_operational_assessment_assessment_per_input))
        .def_readonly("simulator_invocations", &fiction::operational_assessment<Lyt>::simulator_invocations,
                      DOC(fiction_operational_assessment_simulator_invocations));
}

}  // namespace detail

inline void is_operational(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::operational_status>(m, "operational_status", DOC(fiction_operational_status))
        .value("OPERATIONAL", fiction::operational_status::OPERATIONAL, DOC(fiction_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::operational_status::NON_OPERATIONAL,
               DOC(fiction_operational_status_NON_OPERATIONAL));

    py::enum_<fiction::is_operational_params::operational_condition_kinks>(
        m, "operational_condition_kinks", DOC(fiction_is_operational_params_operational_condition_kinks))
        .value("TOLERATE_KINKS", fiction::is_operational_params::operational_condition_kinks::TOLERATE_KINKS,
               DOC(fiction_is_operational_params_operational_condition_kinks_TOLERATE_KINKS))
        .value("REJECT_KINKS", fiction::is_operational_params::operational_condition_kinks::REJECT_KINKS,
               DOC(fiction_is_operational_params_operational_condition_kinks_REJECT_KINKS));

    py::enum_<fiction::is_operational_params::operational_condition_positive_charges>(
        m, "operational_condition_positive_charges",
        DOC(fiction_is_operational_params_operational_condition_positive_charges))
        .value("REJECT_POSITIVE_CHARGES",
               fiction::is_operational_params::operational_condition_positive_charges::REJECT_POSITIVE_CHARGES,
               DOC(fiction_is_operational_params_operational_condition_positive_charges_REJECT_POSITIVE_CHARGES))
        .value("TOLERATE_POSITIVE_CHARGES",
               fiction::is_operational_params::operational_condition_positive_charges::TOLERATE_POSITIVE_CHARGES,
               DOC(fiction_is_operational_params_operational_condition_positive_charges_TOLERATE_POSITIVE_CHARGES));

    py::enum_<fiction::is_operational_params::operational_analysis_strategy>(
        m, "operational_analysis_strategy", DOC(fiction_is_operational_params_operational_analysis_strategy))
        .value("SIMULATION_ONLY", fiction::is_operational_params::operational_analysis_strategy::SIMULATION_ONLY,
               DOC(fiction_is_operational_params_operational_analysis_strategy_SIMULATION_ONLY))
        .value("FILTER_ONLY", fiction::is_operational_params::operational_analysis_strategy::FILTER_ONLY,
               DOC(fiction_is_operational_params_operational_analysis_strategy_FILTER_ONLY))
        .value("FILTER_THEN_SIMULATION",
               fiction::is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION,
               DOC(fiction_is_operational_params_operational_analysis_strategy_FILTER_THEN_SIMULATION));

    py::enum_<fiction::is_operational_params::termination_condition>(
        m, "is_operational_params_termination_condition", DOC(fiction_is_operational_params_termination_condition))
        .value("ON_FIRST_NON_OPERATIONAL",
               fiction::is_operational_params::termination_condition::ON_FIRST_NON_OPERATIONAL,
               DOC(fiction_is_operational_params_termination_condition_ON_FIRST_NON_OPERATIONAL))
        .value("ALL_INPUT_COMBINATIONS_ASSESSED",
               fiction::is_operational_params::termination_condition::ALL_INPUT_COMBINATIONS_ASSESSED,
               DOC(fiction_is_operational_params_termination_condition_ALL_INPUT_COMBINATIONS_ASSESSED));

    py::enum_<fiction::is_operational_params::simulation_results_mode>(
        m, "simulation_results_mode", DOC(fiction_is_operational_params_simulation_results_mode))
        .value("KEEP_SIMULATION_RESULTS",
               fiction::is_operational_params::simulation_results_mode::KEEP_SIMULATION_RESULTS,
               DOC(fiction_is_operational_params_simulation_results_mode_KEEP_SIMULATION_RESULTS))
        .value("DISCARD_SIMULATION_RESULTS",
               fiction::is_operational_params::simulation_results_mode::DISCARD_SIMULATION_RESULTS,
               DOC(fiction_is_operational_params_simulation_results_mode_DISCARD_SIMULATION_RESULTS));

    py::class_<fiction::is_operational_params>(m, "is_operational_params", DOC(fiction_is_operational_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::is_operational_params::simulation_parameters,
                       DOC(fiction_is_operational_params_simulation_parameters))
        .def_readwrite("sim_engine", &fiction::is_operational_params::sim_engine,
                       DOC(fiction_is_operational_params_sim_engine))
        .def_readwrite("input_bdl_iterator_params", &fiction::is_operational_params::input_bdl_iterator_params,
                       DOC(fiction_is_operational_params_input_bdl_iterator_params))
        .def_readwrite("op_condition_kinks", &fiction::is_operational_params::op_condition_kinks,
                       DOC(fiction_is_operational_params_op_condition_kinks))
        .def_readwrite("op_condition_positive_charges", &fiction::is_operational_params::op_condition_positive_charges,
                       DOC(fiction_is_operational_params_op_condition_positive_charges))
        .def_readwrite("strategy_to_analyze_operational_status",
                       &fiction::is_operational_params::strategy_to_analyze_operational_status,
                       DOC(fiction_is_operational_params_strategy_to_analyze_operational_status))
        .def_readwrite("termination_condition", &fiction::is_operational_params::termination_cond,
                       DOC(fiction_is_operational_params_termination_condition))
        .def_readwrite("simulation_results_retention", &fiction::is_operational_params::simulation_results_retention,
                       DOC(fiction_is_operational_params_simulation_results_retention));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::operational_assessment<py_sidb_100_lattice>(m, "_100");
    detail::operational_assessment<py_sidb_111_lattice>(m, "_111");

    detail::is_operational<py_sidb_100_lattice>(m);
    detail::is_operational<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_IS_OPERATIONAL_HPP
