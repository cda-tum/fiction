//
// Created by marcel on 15.11.23.
//

#ifndef PYFICTION_DESIGN_SIDB_GATES_HPP
#define PYFICTION_DESIGN_SIDB_GATES_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void design_sidb_gates(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("design_sidb_gates", &fiction::design_sidb_gates<Lyt, py_tt>, py::arg("skeleton"), py::arg("spec"),
          py::arg("params") = fiction::design_sidb_gates_params<Lyt>{}, py::arg("stats") = nullptr,
          DOC(fiction_design_sidb_gates));
}

template <typename Lyt>
void design_sidb_gates_params(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;

    // TODO: it seems unnecessary to define these selector types for each lattice

    /**
     * Design approach selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<Lyt>::design_sidb_gates_mode>(
        m, fmt::format("design_sidb_gates_mode{}", lattice).c_str(),
        DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode))
        .value("QUICKCELL", fiction::design_sidb_gates_params<Lyt>::design_sidb_gates_mode::QUICKCELL,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL))
        .value("EXHAUSTIVE_GATE_DESIGNER",
               fiction::design_sidb_gates_params<Lyt>::design_sidb_gates_mode::EXHAUSTIVE_GATE_DESIGNER,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL))
        .value("RANDOM", fiction::design_sidb_gates_params<Lyt>::design_sidb_gates_mode::RANDOM,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM));

    /**
     * Termination condition selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<Lyt>::termination_condition>(
        m, fmt::format("design_sidb_gates_params_termination_condition{}", lattice).c_str(),
        DOC(fiction_design_sidb_gates_params_termination_condition))
        .value("AFTER_FIRST_SOLUTION",
               fiction::design_sidb_gates_params<Lyt>::termination_condition::AFTER_FIRST_SOLUTION,
               DOC(fiction_design_sidb_gates_params_termination_condition_AFTER_FIRST_SOLUTION))
        .value("ALL_COMBINATIONS_ENUMERATED",
               fiction::design_sidb_gates_params<Lyt>::termination_condition::ALL_COMBINATIONS_ENUMERATED,
               DOC(fiction_design_sidb_gates_params_termination_condition_ALL_COMBINATIONS_ENUMERATED));

    /**
     * Parameters.
     */
    py::class_<fiction::design_sidb_gates_params<Lyt>>(m, fmt::format("design_sidb_gates_params{}", lattice).c_str(),
                                                       DOC(fiction_design_sidb_gates_params))
        .def(py::init<>())
        .def_readwrite("operational_params", &fiction::design_sidb_gates_params<Lyt>::operational_params,
                       DOC(fiction_design_sidb_gates_params_operational_params))
        .def_readwrite("design_mode", &fiction::design_sidb_gates_params<Lyt>::design_mode,
                       DOC(fiction_design_sidb_gates_params_design_mode))
        .def_readwrite("canvas", &fiction::design_sidb_gates_params<Lyt>::canvas,
                       DOC(fiction_design_sidb_gates_params_canvas_overridden))
        .def_readwrite("number_of_sidbs", &fiction::design_sidb_gates_params<Lyt>::number_of_sidbs,
                       DOC(fiction_design_sidb_gates_params_number_of_sidbs))
        .def_readwrite("termination_cond", &fiction::design_sidb_gates_params<Lyt>::termination_cond,
                       DOC(fiction_design_sidb_gates_params_termination_cond))
        .def_readwrite("post_design_process", &fiction::design_sidb_gates_params<Lyt>::post_design_process,
                       DOC(fiction_design_sidb_gates_params_post_design_process));
}

}  // namespace detail

inline void design_sidb_gates(pybind11::module& m)
{
    namespace py = pybind11;

    // stats are defined to be read-only

    py::class_<fiction::design_sidb_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_design_sidb_gates_stats))
        .def(py::init<>())
        .def_readonly("time_total", &fiction::design_sidb_gates_stats::time_total,
                      DOC(fiction_design_sidb_gates_stats_time_total_overridden))
        .def_readonly("pruning_total", &fiction::design_sidb_gates_stats::pruning_total,
                      DOC(fiction_design_sidb_gates_stats_pruning_total_overridden))
        .def_readonly("sim_engine", &fiction::design_sidb_gates_stats::sim_engine,
                      DOC(fiction_design_sidb_gates_stats_sim_engine))
        .def_readonly("number_of_layouts", &fiction::design_sidb_gates_stats::number_of_layouts,
                      DOC(fiction_design_sidb_gates_stats_number_of_layouts))
        .def_readonly("number_of_layouts_after_first_pruning",
                      &fiction::design_sidb_gates_stats::number_of_layouts_after_first_pruning,
                      DOC(fiction_design_sidb_gates_stats_number_of_layouts_after_first_pruning))
        .def_readonly("number_of_layouts_after_second_pruning",
                      &fiction::design_sidb_gates_stats::number_of_layouts_after_second_pruning,
                      DOC(fiction_design_sidb_gates_stats_number_of_layouts_after_second_pruning))
        .def_readonly("number_of_layouts_after_third_pruning",
                      &fiction::design_sidb_gates_stats::number_of_layouts_after_third_pruning,
                      DOC(fiction_design_sidb_gates_stats_number_of_layouts_after_third_pruning))
        .def("__repr__",
             [](const fiction::design_sidb_gates_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             });

    detail::design_sidb_gates_params<py_sidb_100_lattice>(m, "_100");
    detail::design_sidb_gates_params<py_sidb_111_lattice>(m, "_111");

    detail::design_sidb_gates<py_sidb_100_lattice>(m);
    detail::design_sidb_gates<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_SIDB_GATES_HPP
