//
// Created by marcel on 15.11.23.
//

#ifndef PYFICTION_DESIGN_SIDB_GATES_HPP
#define PYFICTION_DESIGN_SIDB_GATES_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

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
          "params"_a      = fiction::design_sidb_gates_params<fiction::offset::ucoord_t>{},
          "design_mode"_a = fiction::design_sidb_gates_mode::EXHAUSTIVE, "stats"_a = nullptr,
          DOC(fiction_design_sidb_gates));
}

}  // namespace detail

inline void design_sidb_gates(pybind11::module& m)
{
    namespace py = pybind11;
    /**
     * Stats object
     */
    py::class_<fiction::design_sidb_gates_stats>(m, "design_sidb_gates_stats", DOC(fiction_exact_physical_design_stats))
        .def(py::init<>());
    /**
     * Design approach selector type.
     */
    py::enum_<fiction::design_sidb_gates_mode>(m, "design_sidb_gates_mode",
                                               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode))
        .value("EXHAUSTIVE", fiction::design_sidb_gates_mode::EXHAUSTIVE,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_EXHAUSTIVE))
        .value("RANDOM", fiction::design_sidb_gates_mode::RANDOM,
               DOC(fiction_design_sidb_gates_params_design_sidb_gates_mode_RANDOM));
    /**
     * Parameters.
     */
    py::class_<fiction::design_sidb_gates_params<fiction::offset::ucoord_t>>(m, "design_sidb_gates_params",
                                                                             DOC(fiction_design_sidb_gates_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::simulation_parameters,
                       DOC(fiction_design_sidb_gates_params))
        .def_readwrite("canvas", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::canvas,
                       DOC(fiction_design_sidb_gates_params_canvas))
        .def_readwrite("number_of_sidbs",
                       &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::number_of_sidbs,
                       DOC(fiction_design_sidb_gates_params_number_of_sidbs))
        .def_readwrite("sim_engine", &fiction::design_sidb_gates_params<fiction::offset::ucoord_t>::sim_engine,
                       DOC(fiction_design_sidb_gates_params_sim_engine));

    detail::design_sidb_gates<py_sidb_100_lattice>(m);
    detail::design_sidb_gates<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_SIDB_GATES_HPP
