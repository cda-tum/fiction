/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/physical_design/routing_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/traits.hpp>

#include <algorithm>
#include <tuple>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/** @brief Registers crossing queries. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void is_crossable_wire(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("is_crossable_wire", &fiction::physical_design::is_crossable_wire<Lyt>, py::arg("lyt"), py::arg("src"),
          py::arg("successor"), DOC(fiction_physical_design_is_crossable_wire));
}

/** @brief Registers routing to explicit destination input ports. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void route_path(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "route_path",
        [](Lyt& lyt, const std::vector<fiction::coordinate<Lyt>>& path, const typename Lyt::input_port destination)
        {
            const fiction::physical_design::layout_coordinate_path<Lyt> converted_path{path.cbegin(), path.cend()};

            fiction::physical_design::route_path(lyt, converted_path, destination);
        },
        py::arg("layout"), py::arg("path"), py::arg("destination"), DOC(fiction_physical_design_route_path));
}

/** @brief Registers objectives including logical input indices. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void extract_routing_objectives(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "extract_routing_objectives",
        [](Lyt& lyt) -> std::vector<std::tuple<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>, uint32_t>>
        {
            std::vector<std::tuple<fiction::coordinate<Lyt>, fiction::coordinate<Lyt>, uint32_t>>
                converted_objectives{};

            const auto objectives = fiction::physical_design::extract_routing_objectives(lyt);

            std::for_each(
                objectives.cbegin(), objectives.cend(), [&converted_objectives](const auto& objective)
                { converted_objectives.emplace_back(objective.source, objective.target, objective.input_index); });

            return converted_objectives;
        },
        py::arg("layout"), DOC(fiction_physical_design_extract_routing_objectives));
}

/** @brief Registers removal of routing wires and connections. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void clear_routing(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("clear_routing", &fiction::physical_design::clear_routing<Lyt>, py::arg("lyt"),
          DOC(fiction_physical_design_clear_routing));
}

}  // namespace detail

/** @brief Registers routing helpers. @param m Python module. */
void routing_utils(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_crossable_wire<py_cartesian_gate_layout>(m);
    detail::is_crossable_wire<py_shifted_cartesian_gate_layout>(m);
    detail::is_crossable_wire<py_hexagonal_gate_layout>(m);

    detail::route_path<py_cartesian_gate_layout>(m);
    detail::route_path<py_shifted_cartesian_gate_layout>(m);
    detail::route_path<py_hexagonal_gate_layout>(m);

    detail::extract_routing_objectives<py_cartesian_gate_layout>(m);
    detail::extract_routing_objectives<py_shifted_cartesian_gate_layout>(m);
    detail::extract_routing_objectives<py_hexagonal_gate_layout>(m);

    detail::clear_routing<py_cartesian_gate_layout>(m);
    detail::clear_routing<py_shifted_cartesian_gate_layout>(m);
    detail::clear_routing<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
