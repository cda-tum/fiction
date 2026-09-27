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
 * @brief Geometry operations shared by Python domain layouts.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"  // IWYU pragma: keep; supplies coordinate tuple input casters.

#include <fiction/traits.hpp>

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>  // IWYU pragma: keep; converts coordinate collections returned by layout methods.

namespace pyfiction::detail
{
/**
 * @brief Binds geometry directly on a domain layout.
 * @tparam Lyt Concrete gate or cell layout.
 * @param cls Domain layout class.
 */
template <typename Lyt>
void bind_geometry(nanobind::class_<Lyt>& cls)
{
    namespace py = nanobind;
    cls.def(
           "coord",
           [](const Lyt& layout, const int64_t x, const int64_t y, const int64_t z) { return layout.coord(x, y, z); },
           py::arg("x"), py::arg("y"), py::arg("z") = 0l, DOC(fiction_layouts_cartesian_layout_coord))
        .def("x", &Lyt::x, DOC(fiction_layouts_cartesian_layout_x))
        .def("y", &Lyt::y, DOC(fiction_layouts_cartesian_layout_y))
        .def("z", &Lyt::z, DOC(fiction_layouts_cartesian_layout_z))
        .def("area", &Lyt::area, DOC(fiction_layouts_cartesian_layout_area))
        .def("resize", &Lyt::resize, py::arg("dimension"), DOC(fiction_layouts_cartesian_layout_resize))
        .def("north", &Lyt::north, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north))
        .def("north_east", &Lyt::north_east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north_east))
        .def("east", &Lyt::east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_east))
        .def("south_east", &Lyt::south_east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south_east))
        .def("south", &Lyt::south, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south))
        .def("south_west", &Lyt::south_west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south_west))
        .def("west", &Lyt::west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_west))
        .def("north_west", &Lyt::north_west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north_west))
        .def("above", &Lyt::above, py::arg("c"), DOC(fiction_layouts_cartesian_layout_above))
        .def("below", &Lyt::below, py::arg("c"), DOC(fiction_layouts_cartesian_layout_below))
        .def("is_adjacent_of", &Lyt::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &Lyt::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_elevation_of))
        .def("is_ground_layer", &Lyt::is_ground_layer, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &Lyt::is_crossing_layer, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_crossing_layer))
        .def("is_within_bounds", &Lyt::is_within_bounds, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_within_bounds))
        .def(
            "coordinates",
            [](const Lyt& lyt)
            {
                std::vector<fiction::coordinate<Lyt>> coords{};
                coords.reserve(lyt.area() * (static_cast<uint64_t>(lyt.z()) + 1u));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_layouts_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const Lyt& lyt)
            {
                std::vector<fiction::coordinate<Lyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_layouts_cartesian_layout_ground_coordinates))
        .def("adjacent_coordinates", &Lyt::adjacent_coordinates, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &Lyt::adjacent_opposite_coordinates, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_opposite_coordinates));
}
}  // namespace pyfiction::detail
