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
 */

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/io/print_layout.hpp>

#include <cstdint>
#include <optional>
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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{
/**
 * @brief Register one concrete topology.
 * @tparam Lyt Concrete layout.
 * @param m Python module.
 * @param name Python class name.
 */
template <typename Lyt>
void register_cartesian_layout(nb::module_& m, const char* name)
{
    /**
     * Cartesian layout.
     */
    // Keep the GIL for constant-time geometry access and mutation; releasing it costs more than these operations.
    nb::class_<Lyt>(m, name, CARTESIAN_LAYOUT_DOC)
        .def(nb::init<>(),
             parameter_doc(DOC(fiction_layouts_cartesian_layout_cartesian_layout), {{"size", "extent"}}).c_str())
        .def(nb::init<const typename Lyt::extent&>(), nb::arg("extent"),
             parameter_doc(DOC(fiction_layouts_cartesian_layout_cartesian_layout), {{"size", "extent"}}).c_str())
        .def(
            "coord", [](const Lyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(coordinate_axis(x), coordinate_axis(y), coordinate_axis(z)); }, nb::arg("x"),
            nb::arg("y"), nb::arg("z") = 0l, DOC(fiction_layouts_cartesian_layout_coord))
        .def("width", &Lyt::width, "Returns the width count.")
        .def("height", &Lyt::height, "Returns the height count.")
        .def("layers", &Lyt::layers, "Returns the layers count.")
        .def("get_extent", &Lyt::get_extent, "Returns the layout extent.")
        .def("last_coordinate", &Lyt::last_coordinate, "Returns the last coordinate, or None for empty geometry.")
        .def("contains_coordinate", &Lyt::contains_coordinate, nb::arg("c"), "Tests the half-open geometry bounds.")
        .def("area", &Lyt::area, DOC(fiction_layouts_cartesian_layout_area))
        .def("resize", &Lyt::resize, nb::arg("extent"),
             parameter_doc(DOC(fiction_layouts_cartesian_layout_resize), {{"size", "extent"}}).c_str())

        .def("north", &Lyt::north, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_north))
        .def("north_east", &Lyt::north_east, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_north_east))
        .def("east", &Lyt::east, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_east))
        .def("south_east", &Lyt::south_east, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_south_east))
        .def("south", &Lyt::south, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_south))
        .def("south_west", &Lyt::south_west, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_south_west))
        .def("west", &Lyt::west, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_west))
        .def("north_west", &Lyt::north_west, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_north_west))
        .def("above", &Lyt::above, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_above))
        .def("below", &Lyt::below, nb::arg("c"), DOC(fiction_layouts_cartesian_layout_below))

        .def("is_north_of", &Lyt::is_north_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_north_of))
        .def("is_east_of", &Lyt::is_east_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_east_of))
        .def("is_south_of", &Lyt::is_south_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_south_of))
        .def("is_west_of", &Lyt::is_west_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &Lyt::is_adjacent_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &Lyt::is_adjacent_elevation_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &Lyt::is_above, nb::arg("c1"), nb::arg("c2"), DOC(fiction_layouts_cartesian_layout_is_above))
        .def("is_below", &Lyt::is_below, nb::arg("c1"), nb::arg("c2"), DOC(fiction_layouts_cartesian_layout_is_below))
        .def("is_northwards_of", &Lyt::is_northwards_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &Lyt::is_eastwards_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &Lyt::is_southwards_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &Lyt::is_westwards_of, nb::arg("c1"), nb::arg("c2"),
             DOC(fiction_layouts_cartesian_layout_is_westwards_of))

        .def("is_at_northern_border", &Lyt::is_at_northern_border, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &Lyt::is_at_eastern_border, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &Lyt::is_at_southern_border, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &Lyt::is_at_western_border, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &Lyt::is_at_any_border, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_at_any_border))

        .def("northern_border_of", &Lyt::northern_border_of, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &Lyt::eastern_border_of, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &Lyt::southern_border_of, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_southern_border_of))
        .def("western_border_of", &Lyt::western_border_of, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_western_border_of))

        .def("is_ground_layer", &Lyt::is_ground_layer, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &Lyt::is_crossing_layer, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_crossing_layer))

        .def("is_within_bounds", &Lyt::is_within_bounds, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const Lyt& lyt, const std::optional<fiction::layouts::layout_base::coordinate> start,
               const std::optional<fiction::layouts::layout_base::coordinate> stop)
            {
                std::vector<fiction::layouts::layout_base::coordinate> coords{};
                if (!start && !stop)
                {
                    coords.reserve(lyt.area() * lyt.layers());
                }
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); }, start, stop);
                return coords;
            },
            nb::arg("start") = nb::none(), nb::arg("stop") = nb::none(),
            "Returns coordinates in z/y/x order from the inclusive start to the exclusive stop. None uses the frame "
            "boundary.",
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "ground_coordinates",
            [](const Lyt& lyt, const std::optional<fiction::layouts::layout_base::coordinate> start,
               const std::optional<fiction::layouts::layout_base::coordinate> stop)
            {
                std::vector<fiction::layouts::layout_base::coordinate> coords{};
                if (!start && !stop)
                {
                    coords.reserve(lyt.layers() == 0 ? 0 : lyt.area());
                }
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); }, start, stop);
                return coords;
            },
            nb::arg("start") = nb::none(), nb::arg("stop") = nb::none(),
            "Returns layer-zero coordinates from the inclusive start to the exclusive stop. None uses the frame "
            "boundary. Bounds outside layer zero raise ValueError.",
            nb::call_guard<nb::gil_scoped_release>())
        .def("adjacent_coordinates", &Lyt::adjacent_coordinates, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &Lyt::adjacent_opposite_coordinates, nb::arg("c"),
             DOC(fiction_layouts_cartesian_layout_adjacent_opposite_coordinates))

        .def(
            "__copy__", [](const Lyt& lyt) { return Lyt{lyt}; }, "Returns an independent geometry copy.")
        .def(
            "__deepcopy__", [](const Lyt& lyt, const nb::dict&) { return Lyt{lyt}; }, nb::arg("memo"),
            "Returns an independent geometry copy.")

        .def(
            "__repr__",
            [](const Lyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.", nb::call_guard<nb::gil_scoped_release>())

        ;
}

}  // namespace detail

/**
 * @brief Register supported cartesian topologies.
 * @param m Python module.
 */
void register_cartesian_layout(nb::module_& m)
{
    detail::register_cartesian_layout<fiction::layouts::cartesian_layout>(m, "cartesian_layout");
}

}  // namespace pyfiction
