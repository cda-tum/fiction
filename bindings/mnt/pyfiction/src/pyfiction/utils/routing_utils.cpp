//
// Created by marcel on 13.02.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <algorithm>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_crossable_wire(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("is_crossable_wire", &fiction::is_crossable_wire<Lyt>, py::arg("lyt"), py::arg("src"), py::arg("successor"),
          DOC(fiction_is_crossable_wire));
}

template <typename Lyt>
void route_path(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

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
void extract_routing_objectives(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

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
void clear_routing(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("clear_routing", &fiction::clear_routing<Lyt>, py::arg("lyt"), DOC(fiction_clear_routing));
}

}  // namespace detail

void routing_utils(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_crossable_wire<py_cartesian_obstruction_layout>(m);
    detail::is_crossable_wire<py_cartesian_gate_layout>(m);
    detail::is_crossable_wire<py_shifted_cartesian_obstruction_layout>(m);
    detail::is_crossable_wire<py_shifted_cartesian_gate_layout>(m);
    detail::is_crossable_wire<py_hexagonal_obstruction_layout>(m);
    detail::is_crossable_wire<py_hexagonal_gate_layout>(m);

    detail::route_path<py_cartesian_obstruction_layout>(m);
    detail::route_path<py_cartesian_gate_layout>(m);
    detail::route_path<py_shifted_cartesian_obstruction_layout>(m);
    detail::route_path<py_shifted_cartesian_gate_layout>(m);
    detail::route_path<py_hexagonal_obstruction_layout>(m);
    detail::route_path<py_hexagonal_gate_layout>(m);

    detail::extract_routing_objectives<py_cartesian_obstruction_layout>(m);
    detail::extract_routing_objectives<py_cartesian_gate_layout>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_obstruction_layout>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_gate_layout>(m);
    detail::extract_routing_objectives<py_hexagonal_obstruction_layout>(m);
    detail::extract_routing_objectives<py_hexagonal_gate_layout>(m);

    detail::clear_routing<py_cartesian_obstruction_layout>(m);
    detail::clear_routing<py_cartesian_gate_layout>(m);
    detail::clear_routing<py_shifted_cartesian_obstruction_layout>(m);
    detail::clear_routing<py_shifted_cartesian_gate_layout>(m);
    detail::clear_routing<py_hexagonal_obstruction_layout>(m);
    detail::clear_routing<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
