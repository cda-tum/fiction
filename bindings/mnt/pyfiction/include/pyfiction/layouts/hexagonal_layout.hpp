//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_HEXAGONAL_LAYOUT_HPP
#define PYFICTION_HEXAGONAL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

namespace pyfiction
{

inline void hexagonal_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Hexagonal layout.
     */
    py::class_<py_hexagonal_layout>(m, "hexagonal_layout", DOC(fiction_hexagonal_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<py_hexagonal_layout>&>(), "dimension"_a,
             DOC(fiction_hexagonal_layout_hexagonal_layout))
        .def(
            "coord", [](const py_hexagonal_layout& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, "x"_a, "y"_a, "z"_a = 0l, DOC(fiction_hexagonal_layout_coord))
        .def("x", &py_hexagonal_layout::x, DOC(fiction_hexagonal_layout_x))
        .def("y", &py_hexagonal_layout::y, DOC(fiction_hexagonal_layout_y))
        .def("z", &py_hexagonal_layout::z, DOC(fiction_hexagonal_layout_z))
        .def("area", &py_hexagonal_layout::area, DOC(fiction_hexagonal_layout_area))
        .def("resize", &py_hexagonal_layout::resize, "dimension"_a, DOC(fiction_hexagonal_layout_resize))

        .def("north", &py_hexagonal_layout::north, "c"_a, DOC(fiction_hexagonal_layout_north))
        .def("north_east", &py_hexagonal_layout::north_east, "c"_a, DOC(fiction_hexagonal_layout_north_east))
        .def("east", &py_hexagonal_layout::east, "c"_a, DOC(fiction_hexagonal_layout_east))
        .def("south_east", &py_hexagonal_layout::south_east, "c"_a, DOC(fiction_hexagonal_layout_south_east))
        .def("south", &py_hexagonal_layout::south, "c"_a, DOC(fiction_hexagonal_layout_south))
        .def("south_west", &py_hexagonal_layout::south_west, "c"_a, DOC(fiction_hexagonal_layout_south_west))
        .def("west", &py_hexagonal_layout::west, "c"_a, DOC(fiction_hexagonal_layout_west))
        .def("north_west", &py_hexagonal_layout::north_west, "c"_a, DOC(fiction_hexagonal_layout_north_west))
        .def("above", &py_hexagonal_layout::above, "c"_a, DOC(fiction_hexagonal_layout_above))
        .def("below", &py_hexagonal_layout::below, "c"_a, DOC(fiction_hexagonal_layout_below))

        .def("is_north_of", &py_hexagonal_layout::is_north_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_north_of))
        .def("is_east_of", &py_hexagonal_layout::is_east_of, "c1"_a, "c2"_a, DOC(fiction_hexagonal_layout_is_east_of))
        .def("is_south_of", &py_hexagonal_layout::is_south_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_south_of))
        .def("is_west_of", &py_hexagonal_layout::is_west_of, "c1"_a, "c2"_a, DOC(fiction_hexagonal_layout_is_west_of))
        .def("is_adjacent_of", &py_hexagonal_layout::is_adjacent_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &py_hexagonal_layout::is_adjacent_elevation_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_adjacent_elevation_of))
        .def("is_above", &py_hexagonal_layout::is_above, "c1"_a, "c2"_a, DOC(fiction_hexagonal_layout_is_above))
        .def("is_below", &py_hexagonal_layout::is_below, "c1"_a, "c2"_a, DOC(fiction_hexagonal_layout_is_below))
        .def("is_northwards_of", &py_hexagonal_layout::is_northwards_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_northwards_of))
        .def("is_eastwards_of", &py_hexagonal_layout::is_eastwards_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_eastwards_of))
        .def("is_southwards_of", &py_hexagonal_layout::is_southwards_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_southwards_of))
        .def("is_westwards_of", &py_hexagonal_layout::is_westwards_of, "c1"_a, "c2"_a,
             DOC(fiction_hexagonal_layout_is_westwards_of))

        .def("is_at_northern_border", &py_hexagonal_layout::is_at_northern_border, "c"_a,
             DOC(fiction_hexagonal_layout_is_at_northern_border))
        .def("is_at_eastern_border", &py_hexagonal_layout::is_at_eastern_border, "c"_a,
             DOC(fiction_hexagonal_layout_is_at_eastern_border))
        .def("is_at_southern_border", &py_hexagonal_layout::is_at_southern_border, "c"_a,
             DOC(fiction_hexagonal_layout_is_at_southern_border))
        .def("is_at_western_border", &py_hexagonal_layout::is_at_western_border, "c"_a,
             DOC(fiction_hexagonal_layout_is_at_western_border))
        .def("is_at_any_border", &py_hexagonal_layout::is_at_any_border, "c"_a,
             DOC(fiction_hexagonal_layout_is_at_any_border))

        .def("northern_border_of", &py_hexagonal_layout::northern_border_of, "c"_a,
             DOC(fiction_hexagonal_layout_northern_border_of))
        .def("eastern_border_of", &py_hexagonal_layout::eastern_border_of, "c"_a,
             DOC(fiction_hexagonal_layout_eastern_border_of))
        .def("southern_border_of", &py_hexagonal_layout::southern_border_of, "c"_a,
             DOC(fiction_hexagonal_layout_southern_border_of))
        .def("western_border_of", &py_hexagonal_layout::western_border_of, "c"_a,
             DOC(fiction_hexagonal_layout_western_border_of))

        .def("is_ground_layer", &py_hexagonal_layout::is_ground_layer, "c"_a,
             DOC(fiction_hexagonal_layout_is_ground_layer))
        .def("is_crossing_layer", &py_hexagonal_layout::is_crossing_layer, "c"_a,
             DOC(fiction_hexagonal_layout_is_crossing_layer))

        .def("is_within_bounds", &py_hexagonal_layout::is_within_bounds, "c"_a,
             DOC(fiction_hexagonal_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const py_hexagonal_layout& lyt)
            {
                std::vector<fiction::coordinate<py_hexagonal_layout>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_hexagonal_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const py_hexagonal_layout& lyt)
            {
                std::vector<fiction::coordinate<py_hexagonal_layout>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_hexagonal_layout_ground_coordinates))
        .def("adjacent_coordinates", &py_hexagonal_layout::adjacent_coordinates, "c"_a,
             DOC(fiction_hexagonal_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &py_hexagonal_layout::adjacent_opposite_coordinates, "c"_a,
             DOC(fiction_hexagonal_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const py_hexagonal_layout& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONAL_LAYOUT_HPP
