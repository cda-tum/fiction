//
// Created by marcel on 13.02.23.
//

#ifndef PYFICTION_ROUTING_UTILS_HPP
#define PYFICTION_ROUTING_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_crossable_wire(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("is_crossable_wire", &fiction::is_crossable_wire<Lyt>, py::arg("lyt"), py::arg("src"), py::arg("successor"),
          DOC(fiction_is_crossable_wire));
}

template <typename Lyt>
void route_path(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "route_path",
        [](Lyt& lyt, const std::vector<fiction::coordinate<Lyt>>& path)
        {
            fiction::layout_coordinate_path<Lyt> converted_path{};
            converted_path.resize(path.size());
            converted_path.assign(path.cbegin(), path.cend());

            fiction::route_path(lyt, converted_path);
        },
        py::arg("layout"), py::arg("path"), DOC(fiction_route_path));
}

template <typename Lyt>
void extract_routing_objectives(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "extract_routing_objectives",
        [](Lyt& lyt) -> std::vector<std::pair<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>>>
        {
            std::vector<std::pair<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>>> converted_objectives{};

            const auto objectives = fiction::extract_routing_objectives(lyt);

            std::for_each(objectives.cbegin(), objectives.cend(), [&converted_objectives](const auto& objective)
                          { converted_objectives.emplace_back(objective.source, objective.target); });

            return converted_objectives;
        },
        py::arg("layout"), DOC(fiction_extract_routing_objectives));
}

template <typename Lyt>
void clear_routing(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("clear_routing", &fiction::clear_routing<Lyt>, py::arg("lyt"), DOC(fiction_clear_routing));
}

}  // namespace detail

inline void routing_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_crossable_wire<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::is_crossable_wire<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::is_crossable_wire<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::is_crossable_wire<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::is_crossable_wire<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::is_crossable_wire<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::is_crossable_wire<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::is_crossable_wire<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::is_crossable_wire<py_hexagonal_obstruction_layout>(m);
    detail::is_crossable_wire<py_hexagonal_gate_layout>(m);

    detail::route_path<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::route_path<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::route_path<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::route_path<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::route_path<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::route_path<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::route_path<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::route_path<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::route_path<py_hexagonal_obstruction_layout>(m);
    detail::route_path<py_hexagonal_gate_layout>(m);

    detail::extract_routing_objectives<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::extract_routing_objectives<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::extract_routing_objectives<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::extract_routing_objectives<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::extract_routing_objectives<py_hexagonal_obstruction_layout>(m);
    detail::extract_routing_objectives<py_hexagonal_gate_layout>(m);

    detail::clear_routing<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::clear_routing<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::clear_routing<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::clear_routing<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::clear_routing<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::clear_routing<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::clear_routing<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::clear_routing<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::clear_routing<py_hexagonal_obstruction_layout>(m);
    detail::clear_routing<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ROUTING_UTILS_HPP
