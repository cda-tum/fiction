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
 * @brief Python bindings for `fiction/layouts/cartesian_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author OpenAI (Codex)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/print_layout.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{
/**
 * Register one concrete topology.
 * @tparam Lyt Concrete layout.
 * @param m Python module.
 * @param name Python class name.
 */
template <typename Lyt>
void cartesian_layout(nanobind::module_& m, const char* name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    /**
     * Cartesian layout.
     */
    py::class_<Lyt>(m, name, DOC(fiction_cartesian_layout_overridden))
        .def(py::init<>(), DOC(fiction_layouts_cartesian_layout_cartesian_layout))
        .def(py::init<const fiction::aspect_ratio<Lyt>&>(), py::arg("dimension"),
             DOC(fiction_layouts_cartesian_layout_cartesian_layout))
        .def(
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

        .def("is_north_of", &Lyt::is_north_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_north_of))
        .def("is_east_of", &Lyt::is_east_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_east_of))
        .def("is_south_of", &Lyt::is_south_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_south_of))
        .def("is_west_of", &Lyt::is_west_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &Lyt::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &Lyt::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &Lyt::is_above, py::arg("c1"), py::arg("c2"), DOC(fiction_layouts_cartesian_layout_is_above))
        .def("is_below", &Lyt::is_below, py::arg("c1"), py::arg("c2"), DOC(fiction_layouts_cartesian_layout_is_below))
        .def("is_northwards_of", &Lyt::is_northwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &Lyt::is_eastwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &Lyt::is_southwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &Lyt::is_westwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_westwards_of))

        .def("is_at_northern_border", &Lyt::is_at_northern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &Lyt::is_at_eastern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &Lyt::is_at_southern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &Lyt::is_at_western_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &Lyt::is_at_any_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_any_border))

        .def("northern_border_of", &Lyt::northern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &Lyt::eastern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &Lyt::southern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_southern_border_of))
        .def("western_border_of", &Lyt::western_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_western_border_of))

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
                coords.reserve(lyt.area() * (lyt.z() + 1));
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
             DOC(fiction_layouts_cartesian_layout_adjacent_opposite_coordinates))

        .def(
            "__repr__",
            [](const Lyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

/**
 * Register supported cartesian topologies.
 * @param m Python module.
 */
void cartesian_layout(nanobind::module_& m)
{
    detail::cartesian_layout<py_cartesian_layout>(m, "cartesian_layout");
    detail::cartesian_layout<py_stacked_cartesian_layout>(m, "stacked_cartesian_layout");
}

}  // namespace pyfiction
