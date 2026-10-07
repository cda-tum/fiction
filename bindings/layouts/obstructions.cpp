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
 * @brief Python bindings for explicit routing constraints.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"  // IWYU pragma: keep; the type caster of `coordinate` accepts tuples

#include <fiction/layouts/obstructions.hpp>

#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner): Converts directed connection pairs to Python tuples.
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner): Converts obstruction collections to Python lists.

namespace pyfiction
{

/** @brief Registers explicit routing constraints on offset coordinates. @param m Python module. */
void obstructions(nanobind::module_& m)
{
    namespace py = nanobind;
    using data   = fiction::layouts::obstructions;

    py::class_<data>(m, "obstructions", DOC(fiction_layouts_obstructions))
        .def(py::init<>(), "Creates empty routing constraints.")
        .def("obstruct_coordinate", &data::obstruct_coordinate, py::arg("c"),
             DOC(fiction_layouts_obstructions_obstruct_coordinate))
        .def("obstruct_connection", &data::obstruct_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_obstructions_obstruct_connection))
        .def("clear_obstructed_coordinate", &data::clear_obstructed_coordinate, py::arg("c"),
             DOC(fiction_layouts_obstructions_clear_obstructed_coordinate))
        .def("clear_obstructed_connection", &data::clear_obstructed_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_obstructions_clear_obstructed_connection))
        .def("clear_obstructed_coordinates", &data::clear_obstructed_coordinates,
             DOC(fiction_layouts_obstructions_clear_obstructed_coordinates))
        .def("clear_obstructed_connections", &data::clear_obstructed_connections,
             DOC(fiction_layouts_obstructions_clear_obstructed_connections))
        .def(
            "obstructed_coordinates",
            [](const data& constraints)
            {
                std::vector<fiction::layouts::layout_base::coordinate> coordinates{};
                constraints.foreach_obstructed_coordinate([&coordinates](const auto& c) { coordinates.push_back(c); });
                return coordinates;
            },
            "Returns explicit coordinate obstructions in unspecified order.")
        .def(
            "obstructed_connections",
            [](const data& constraints)
            {
                std::vector<
                    std::pair<fiction::layouts::layout_base::coordinate, fiction::layouts::layout_base::coordinate>>
                    connections{};
                constraints.foreach_obstructed_connection([&connections](const auto& source, const auto& target)
                                                          { connections.emplace_back(source, target); });
                return connections;
            },
            "Returns explicit directed-connection obstructions in unspecified order.")
        .def("is_obstructed_coordinate", &data::is_obstructed_coordinate, py::arg("c"),
             DOC(fiction_layouts_obstructions_is_obstructed_coordinate))
        .def("is_obstructed_connection", &data::is_obstructed_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_obstructions_is_obstructed_connection));
}

}  // namespace pyfiction
