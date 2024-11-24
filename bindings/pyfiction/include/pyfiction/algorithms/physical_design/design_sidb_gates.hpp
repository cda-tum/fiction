//
// Created by marcel on 15.11.23.
//

#ifndef PYFICTION_DESIGN_SIDB_GATES_HPP
#define PYFICTION_DESIGN_SIDB_GATES_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/types.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void design_sidb_gates(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    m.def("design_sidb_gates", &fiction::design_sidb_gates<Lyt, py_tt>, "skeleton"_a, "spec"_a,
          "params"_a = fiction::design_sidb_gates_params<fiction::cell<Lyt>>{}, "stats"_a = nullptr,
          DOC(fiction_design_sidb_gates));
}

}  // namespace detail

inline void design_sidb_gates(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::design_sidb_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_design_sidb_gates_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::design_sidb_gates_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             });

    /**
     * Design approach selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode>(
        m, "design_sidb_gates_mode", DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode))
        .value("QUICKCELL",
               fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::QUICKCELL,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL))
        .value("AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_QUICKCELL))
        .value("RANDOM", fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::RANDOM,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM));

    /**
     * Termination condition selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition>(
        m, "termination_condition", DOC(fiction_design_sidb_gates_params_termination_condition))
        .value(
            "AFTER_FIRST_SOLUTION",
            fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition::AFTER_FIRST_SOLUTION,
            DOC(fiction_design_sidb_gates_params_termination_condition_AFTER_FIRST_SOLUTION))
        .value("ALL_COMBINATIONS_ENUMERATED",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::termination_condition::ALL_COMBINATIONS_ENUMERATED,
               DOC(fiction_design_sidb_gates_params_termination_condition_ALL_COMBINATIONS_ENUMERATED));

    /**
     * Post-design processes selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::post_design_mode>(
        m, "post_design_mode", DOC(fiction_design_sidb_gates_params_post_design_mode))
        .value("DO_NOTHING", fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::post_design_mode::DO_NOTHING,
               DOC(fiction_design_sidb_gates_params_post_design_mode_DO_NOTHING))
        .value("PREFER_ENERGETICALLY_ISOLATED_GROUND_STATES",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::post_design_mode::PREFER_ENERGETICALLY_ISOLATED_GROUND_STATES,
               DOC(fiction_design_sidb_gates_params_post_design_mode_PREFER_ENERGETICALLY_ISOLATED_GROUND_STATES));

    /**
     * Parameters.
     */
    py::class_<fiction::design_sidb_gates_params<fiction::offset::ucoord_t>>(m, "design_sidb_gates_params",
                                                                             DOC(fiction_design_sidb_gates_params))
        .def(py::init<>())
        .def_readwrite("operational_params",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::operational_params,
                       DOC(fiction_design_sidb_gates_params_operational_params))
        .def_readwrite("design_mode", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_mode,
                       DOC(fiction_design_sidb_gates_params_design_mode))
        .def_readwrite("canvas", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::canvas,
                       DOC(fiction_design_sidb_gates_params_canvas))
        .def_readwrite("number_of_sidbs",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::number_of_sidbs,
                       DOC(fiction_design_sidb_gates_params_number_of_sidbs))
        .def_readwrite("termination_cond",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_cond,
                       DOC(fiction_design_sidb_gates_params_termination_cond))
        .def_readwrite("post_design_process",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::post_design_process,
                       DOC(fiction_design_sidb_gates_params_post_design_process));

    detail::design_sidb_gates<py_sidb_100_lattice>(m);
    detail::design_sidb_gates<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_SIDB_GATES_HPP
