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

    m.def("design_sidb_gates", &fiction::design_sidb_gates<Lyt, py_tt>, py::arg("skeleton"), py::arg("spec"),
          py::arg("params") = fiction::design_sidb_gates_params<fiction::cell<Lyt>>{}, py::arg("stats") = nullptr,
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
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM))
        .value("PRUNING_ONLY",
               fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::design_sidb_gates_mode::PRUNING_ONLY,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_PRUNING_ONLY))

        ;
    /**
     * Termination condition selector type.
     */
    py::enum_<typename fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition>(
        m, "termination_condition")
        .value(
            "AFTER_FIRST_SOLUTION",
            fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_condition::AFTER_FIRST_SOLUTION)
        .value("ALL_COMBINATIONS_ENUMERATED",
               fiction::design_sidb_gates_params<
                   fiction::offset::ucoord_t>::termination_condition::ALL_COMBINATIONS_ENUMERATED)

        ;

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
        .def_readwrite("number_of_canvas_sidbs",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::number_of_canvas_sidbs,
                       DOC(fiction_design_sidb_gates_params_number_of_canvas_sidbs))
        .def_readwrite("termination_cond",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::termination_cond,
                       DOC(fiction_design_sidb_gates_params_termination_condition));

    detail::design_sidb_gates<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::design_sidb_gates<py_sidb_111_lattice<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_SIDB_GATES_HPP
