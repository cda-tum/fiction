//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_COLOR_ROUTING_HPP
#define PYFICTION_COLOR_ROUTING_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/color_routing.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void color_routing(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "color_routing",
        [](Lyt& lyt, const std::vector<std::pair<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>>>& objectives,
           const fiction::color_routing_params& params)
        {
            std::vector<fiction::routing_objective<Lyt>> objs{};
            objs.reserve(objectives.size());

            for (const auto& obj : objectives)
            {
                objs.push_back({obj.first, obj.second});
            }

            return fiction::color_routing(lyt, objs, params);
        },
        "layout"_a, "objectives"_a, "params"_a = fiction::color_routing_params{});
}

}  // namespace detail

inline void color_routing(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * SAT engine selector type.
     */
    pybind11::enum_<fiction::graph_coloring_engine>(m, "graph_coloring_engine")
        .value("MCS", fiction::graph_coloring_engine::MCS)
        .value("DSATUR", fiction::graph_coloring_engine::DSATUR)
        .value("LMXRLF", fiction::graph_coloring_engine::LMXRLF)
        .value("TABUCOL", fiction::graph_coloring_engine::TABUCOL)
        .value("SAT", fiction::graph_coloring_engine::SAT)

        ;

    py::class_<fiction::color_routing_params>(m, "color_routing_params")
        .def(py::init<>())
        .def_readwrite("conduct_partial_routing", &fiction::color_routing_params::conduct_partial_routing)
        .def_readwrite("crossings", &fiction::color_routing_params::crossings)
        .def_readwrite("path_limit", &fiction::color_routing_params::path_limit)
        .def_readwrite("engine", &fiction::color_routing_params::engine)
        .def_readwrite("partial_sat", &fiction::color_routing_params::partial_sat)

        ;

    detail::color_routing<py_cartesian_gate_layout>(m);
    detail::color_routing<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_COLOR_ROUTING_HPP
