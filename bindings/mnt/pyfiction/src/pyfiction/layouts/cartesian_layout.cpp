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
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/layout_dimension.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/print_layout.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
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

template <typename CartesianLyt>
void cartesian_layout(nanobind::module_& m, const char* class_name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    using coordinate = fiction::coordinate<CartesianLyt>;

    constexpr auto supports_explicit_minimum = requires(CartesianLyt& lyt, const coordinate& minimum,
                                                        const coordinate& maximum) { lyt.resize(minimum, maximum); };

    /**
     * Cartesian layout.
     */
    auto layout = py::class_<CartesianLyt>(m, class_name, DOC(fiction_cartesian_layout_overridden));

    layout.def(py::init<>(), DOC(fiction_layouts_cartesian_layout_cartesian_layout));

    if constexpr (supports_explicit_minimum)
    {
        layout
            .def(
                "__init__",
                [](py::pointer_and_handle<CartesianLyt> self, const std::pair<coordinate, coordinate>& dimension)
                { construct_layout_from_bounds(self, dimension); }, py::arg("dimension"),
                "Constructs a layout from inclusive minimum and maximum coordinates.")
            .def(
                "resize",
                [](CartesianLyt& lyt, const std::pair<coordinate, coordinate>& dimension)
                {
                    validate_layout_bounds(dimension);
                    lyt.resize(dimension.first, dimension.second);
                },
                py::arg("dimension"), "Resizes the layout to inclusive minimum and maximum coordinates.");
    }

    layout
        .def(
            "__init__",
            [](py::pointer_and_handle<CartesianLyt> self, const fiction::aspect_ratio<CartesianLyt>& dimension)
            { new (self.p) CartesianLyt{validate_layout_maximum(dimension)}; }, py::arg("dimension"),
            DOC(fiction_layouts_cartesian_layout_cartesian_layout))
        .def(
            "coord", [](const CartesianLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, py::arg("x"), py::arg("y"), py::arg("z") = 0l,
            DOC(fiction_layouts_cartesian_layout_coord))
        .def("x", &CartesianLyt::x, DOC(fiction_layouts_cartesian_layout_x))
        .def("y", &CartesianLyt::y, DOC(fiction_layouts_cartesian_layout_y))
        .def("z", &CartesianLyt::z, DOC(fiction_layouts_cartesian_layout_z))
        .def("x_min", &CartesianLyt::x_min, DOC(fiction_layouts_cartesian_layout_x_min))
        .def("y_min", &CartesianLyt::y_min, DOC(fiction_layouts_cartesian_layout_y_min))
        .def("z_min", &CartesianLyt::z_min, DOC(fiction_layouts_cartesian_layout_z_min))
        .def("area", &CartesianLyt::area, DOC(fiction_layouts_cartesian_layout_area))
        .def(
            "resize", [](CartesianLyt& lyt, const fiction::aspect_ratio<CartesianLyt>& dimension)
            { lyt.resize(validate_layout_maximum(dimension)); }, py::arg("dimension"),
            DOC(fiction_layouts_cartesian_layout_resize))

        .def("north", &CartesianLyt::north, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north))
        .def("north_east", &CartesianLyt::north_east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north_east))
        .def("east", &CartesianLyt::east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_east))
        .def("south_east", &CartesianLyt::south_east, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south_east))
        .def("south", &CartesianLyt::south, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south))
        .def("south_west", &CartesianLyt::south_west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_south_west))
        .def("west", &CartesianLyt::west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_west))
        .def("north_west", &CartesianLyt::north_west, py::arg("c"), DOC(fiction_layouts_cartesian_layout_north_west))
        .def("above", &CartesianLyt::above, py::arg("c"), DOC(fiction_layouts_cartesian_layout_above))
        .def("below", &CartesianLyt::below, py::arg("c"), DOC(fiction_layouts_cartesian_layout_below))

        .def("is_north_of", &CartesianLyt::is_north_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_north_of))
        .def("is_east_of", &CartesianLyt::is_east_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_east_of))
        .def("is_south_of", &CartesianLyt::is_south_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_south_of))
        .def("is_west_of", &CartesianLyt::is_west_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &CartesianLyt::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &CartesianLyt::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &CartesianLyt::is_above, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_above))
        .def("is_below", &CartesianLyt::is_below, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_below))
        .def("is_northwards_of", &CartesianLyt::is_northwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &CartesianLyt::is_eastwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &CartesianLyt::is_southwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &CartesianLyt::is_westwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_westwards_of))

        .def("is_at_northern_border", &CartesianLyt::is_at_northern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &CartesianLyt::is_at_eastern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &CartesianLyt::is_at_southern_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &CartesianLyt::is_at_western_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &CartesianLyt::is_at_any_border, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_any_border))

        .def("northern_border_of", &CartesianLyt::northern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &CartesianLyt::eastern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &CartesianLyt::southern_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_southern_border_of))
        .def("western_border_of", &CartesianLyt::western_border_of, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_western_border_of))

        .def("is_ground_layer", &CartesianLyt::is_ground_layer, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &CartesianLyt::is_crossing_layer, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_crossing_layer))

        .def("is_within_bounds", &CartesianLyt::is_within_bounds, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const CartesianLyt& lyt)
            {
                std::vector<fiction::coordinate<CartesianLyt>> coords{};
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_layouts_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const CartesianLyt& lyt)
            {
                std::vector<fiction::coordinate<CartesianLyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_layouts_cartesian_layout_ground_coordinates))
        .def("adjacent_coordinates", &CartesianLyt::adjacent_coordinates, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &CartesianLyt::adjacent_opposite_coordinates, py::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_opposite_coordinates))

        .def(
            "__repr__",
            [](const CartesianLyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

void cartesian_layout(nanobind::module_& m)
{
    detail::cartesian_layout<py_cartesian_layout>(m, "cartesian_layout");
    detail::cartesian_layout<py_cartesian_layout_cube>(m, "cartesian_layout_cube");
}

}  // namespace pyfiction
