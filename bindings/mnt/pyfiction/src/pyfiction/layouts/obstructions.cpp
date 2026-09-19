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
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstructions.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{
namespace detail
{
/**
 * @brief Registers obstruction data for one coordinate type.
 * @tparam Coordinate Coordinate type.
 * @param m Python module.
 * @param name Python class name.
 */
template <typename Coordinate>
void register_obstructions(nanobind::module_& m, const char* name)
{
    namespace py = nanobind;
    using data   = fiction::layouts::obstructions<Coordinate>;
    py::class_<data>(m, name, DOC(fiction_layouts_obstructions))
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
        .def("is_obstructed_coordinate", &data::is_obstructed_coordinate, py::arg("c"),
             DOC(fiction_layouts_obstructions_is_obstructed_coordinate))
        .def("is_obstructed_connection", &data::is_obstructed_connection, py::arg("src"), py::arg("tgt"),
             DOC(fiction_layouts_obstructions_is_obstructed_connection));
}
}  // namespace detail

/** @brief Registers explicit routing constraints. @param m Python module. */
void obstructions(nanobind::module_& m)
{
    detail::register_obstructions<py_offset_coordinate>(m, "RoutingObstructions");
    detail::register_obstructions<py_cube_coordinate>(m, "CubeRoutingObstructions");
}
}  // namespace pyfiction
