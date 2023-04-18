//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
#define PYFICTION_DESIGN_RULE_VIOLATIONS_HPP

#include "pyfiction/docs.hpp"
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
    using namespace pybind11::literals;

    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt, fiction::gate_level_drv_params params = {},
           const bool print_report = false) -> std::pair<std::size_t, std::size_t>
        {
            std::ostringstream null_stream{};
            if (!print_report)
            {
                params.out = &null_stream;
            }

            fiction::gate_level_drv_stats stats{};

            fiction::gate_level_drvs(lyt, params, &stats);

            return {stats.warnings, stats.drvs};
        },
        "layout"_a, "params"_a = fiction::gate_level_drv_params{}, "print_report"_a = false,
        DOC(fiction_detail_gate_level_drvs_impl));
}

}  // namespace detail

inline void design_rule_violations(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::gate_level_drv_params>(m, "gate_level_drv_params")
        .def(py::init<>())
        .def_readwrite("unplaced_nodes", &fiction::gate_level_drv_params::unplaced_nodes,
                       DOC(fiction_detail_gate_level_drvs_impl_unplaced_nodes_check))
        .def_readwrite("placed_dead_nodes", &fiction::gate_level_drv_params::placed_dead_nodes,
                       DOC(fiction_detail_gate_level_drvs_impl_placed_dead_nodes_check))
        .def_readwrite("non_adjacent_connections", &fiction::gate_level_drv_params::non_adjacent_connections,
                       DOC(fiction_detail_gate_level_drvs_impl_non_adjacent_connections_check))
        .def_readwrite("missing_connections", &fiction::gate_level_drv_params::missing_connections,
                       DOC(fiction_detail_gate_level_drvs_impl_missing_connections_check))
        .def_readwrite("crossing_gates", &fiction::gate_level_drv_params::crossing_gates,
                       DOC(fiction_detail_gate_level_drvs_impl_crossing_gates_check))
        .def_readwrite("clocked_data_flow", &fiction::gate_level_drv_params::clocked_data_flow,
                       DOC(fiction_detail_gate_level_drvs_impl_clocked_data_flow_check))
        .def_readwrite("has_io", &fiction::gate_level_drv_params::has_io,
                       DOC(fiction_detail_gate_level_drvs_impl_has_io_check))
        .def_readwrite("empty_io", &fiction::gate_level_drv_params::empty_io,
                       DOC(fiction_detail_gate_level_drvs_impl_empty_io_check))
        .def_readwrite("io_pins", &fiction::gate_level_drv_params::io_pins,
                       DOC(fiction_detail_gate_level_drvs_impl_io_pin_check))
        .def_readwrite("border_io", &fiction::gate_level_drv_params::border_io,
                       DOC(fiction_detail_gate_level_drvs_impl_border_io_check))

        ;

    detail::gate_level_drvs<py_cartesian_gate_layout>(m);
    detail::gate_level_drvs<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DESIGN_RULE_VIOLATIONS_HPP
