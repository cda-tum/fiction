//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_IS_OPERATIONAL_HPP
#define PYFICTION_IS_OPERATIONAL_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>
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

}  // namespace detail

inline void is_operational(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::operational_status>(m, "operational_status", DOC(fiction_operational_status))
        .value("OPERATIONAL", fiction::operational_status::OPERATIONAL, DOC(fiction_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::operational_status::NON_OPERATIONAL,
               DOC(fiction_operational_status_NON_OPERATIONAL));

    py::enum_<fiction::is_operational_params::operational_condition>(
        m, "operational_condition", DOC(fiction_is_operational_params_operational_condition))
        .value("TOLERATE_KINKS", fiction::is_operational_params::operational_condition::TOLERATE_KINKS,
               DOC(fiction_is_operational_params_operational_condition_TOLERATE_KINKS))
        .value("REJECT_KINKS", fiction::is_operational_params::operational_condition::REJECT_KINKS,
               DOC(fiction_is_operational_params_operational_condition_REJECT_KINKS));

    py::class_<fiction::is_operational_params>(m, "is_operational_params", DOC(fiction_is_operational_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::is_operational_params::simulation_parameters,
                       DOC(fiction_is_operational_params_simulation_parameters))
        .def_readwrite("sim_engine", &fiction::is_operational_params::sim_engine,
                       DOC(fiction_is_operational_params_sim_engine))
        .def_readwrite("input_bdl_iterator_params", &fiction::is_operational_params::input_bdl_iterator_params,
                       DOC(fiction_is_operational_params_input_bdl_iterator_params))
        .def_readwrite("op_condition", &fiction::is_operational_params::op_condition,
                       DOC(fiction_is_operational_params_op_condition))
        .def_readwrite("strategy_to_analyze_operational_status",
                       &fiction::is_operational_params::strategy_to_analyze_operational_status,
                       DOC(fiction_is_operational_params_strategy_to_analyze_operational_status));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::is_operational<py_sidb_100_lattice>(m);
    detail::is_operational<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_IS_OPERATIONAL_HPP
