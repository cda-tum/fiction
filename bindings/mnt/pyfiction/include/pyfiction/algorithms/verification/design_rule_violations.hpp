//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
#define PYFICTION_DESIGN_RULE_VIOLATIONS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/verification/design_rule_violations.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void gate_level_drvs(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt, fiction::gate_level_drv_params params = {},
           const bool print_report = false) -> std::pair<std::size_t, std::size_t>
        {
            std::ostringstream report_stream{};
            params.out = &report_stream;

            fiction::gate_level_drv_stats stats{};

            fiction::gate_level_drvs(lyt, params, &stats);

            if (print_report)
            {
                pybind11::print(report_stream.str());
            }

            return {stats.warnings, stats.drvs};
        },
        py::arg("layout"), py::arg("params") = fiction::gate_level_drv_params{}, py::arg("print_report") = false,
        DOC(fiction_gate_level_drvs));
}

}  // namespace detail

inline void design_rule_violations(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::gate_level_drv_params>(m, "gate_level_drv_params", DOC(fiction_gate_level_drv_params))
        .def(py::init<>())
        .def_readwrite("unplaced_nodes", &fiction::gate_level_drv_params::unplaced_nodes,
                       DOC(fiction_gate_level_drv_params_unplaced_nodes))
        .def_readwrite("placed_dead_nodes", &fiction::gate_level_drv_params::placed_dead_nodes,
                       DOC(fiction_gate_level_drv_params_placed_dead_nodes))
        .def_readwrite("non_adjacent_connections", &fiction::gate_level_drv_params::non_adjacent_connections,
                       DOC(fiction_gate_level_drv_params_non_adjacent_connections))
        .def_readwrite("missing_connections", &fiction::gate_level_drv_params::missing_connections,
                       DOC(fiction_gate_level_drv_params_missing_connections))
        .def_readwrite("crossing_gates", &fiction::gate_level_drv_params::crossing_gates,
                       DOC(fiction_gate_level_drv_params_crossing_gates))
        .def_readwrite("clocked_data_flow", &fiction::gate_level_drv_params::clocked_data_flow,
                       DOC(fiction_gate_level_drv_params_clocked_data_flow))
        .def_readwrite("has_io", &fiction::gate_level_drv_params::has_io, DOC(fiction_gate_level_drv_params_has_io))
        .def_readwrite("empty_io", &fiction::gate_level_drv_params::empty_io,
                       DOC(fiction_gate_level_drv_params_empty_io))
        .def_readwrite("io_pins", &fiction::gate_level_drv_params::io_pins, DOC(fiction_gate_level_drv_params_io_pins))
        .def_readwrite("border_io", &fiction::gate_level_drv_params::border_io,
                       DOC(fiction_gate_level_drv_params_border_io))

        ;

    detail::gate_level_drvs<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::gate_level_drvs<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::gate_level_drvs<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
