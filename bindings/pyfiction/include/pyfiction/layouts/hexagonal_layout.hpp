//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_HEXAGONAL_LAYOUT_HPP
#define PYFICTION_HEXAGONAL_LAYOUT_HPP

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
    py::class_<py_hexagonal_layout>(m, "hexagonal_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<py_hexagonal_layout>&>(), "dimension"_a)
        .def("x", &py_hexagonal_layout::x)
        .def("y", &py_hexagonal_layout::y)
        .def("z", &py_hexagonal_layout::z)
        .def("area", &py_hexagonal_layout::area)
        .def("resize", &py_hexagonal_layout::resize, "dimension"_a)

        .def("north", &py_hexagonal_layout::north, "c"_a)
        .def("north_east", &py_hexagonal_layout::north_east, "c"_a)
        .def("east", &py_hexagonal_layout::east, "c"_a)
        .def("south_east", &py_hexagonal_layout::south_east, "c"_a)
        .def("south", &py_hexagonal_layout::south, "c"_a)
        .def("south_west", &py_hexagonal_layout::south_west, "c"_a)
        .def("west", &py_hexagonal_layout::west, "c"_a)
        .def("north_west", &py_hexagonal_layout::north_west, "c"_a)
        .def("above", &py_hexagonal_layout::above, "c"_a)
        .def("below", &py_hexagonal_layout::below, "c"_a)

        .def("is_north_of", &py_hexagonal_layout::is_north_of, "c1"_a, "c2"_a)
        .def("is_east_of", &py_hexagonal_layout::is_east_of, "c1"_a, "c2"_a)
        .def("is_south_of", &py_hexagonal_layout::is_south_of, "c1"_a, "c2"_a)
        .def("is_west_of", &py_hexagonal_layout::is_west_of, "c1"_a, "c2"_a)
        .def("is_adjacent_of", &py_hexagonal_layout::is_adjacent_of, "c1"_a, "c2"_a)
        .def("is_adjacent_elevation_of", &py_hexagonal_layout::is_adjacent_elevation_of, "c1"_a, "c2"_a)
        .def("is_above", &py_hexagonal_layout::is_above, "c1"_a, "c2"_a)
        .def("is_below", &py_hexagonal_layout::is_below, "c1"_a, "c2"_a)
        .def("is_northwards_of", &py_hexagonal_layout::is_northwards_of, "c1"_a, "c2"_a)
        .def("is_eastwards_of", &py_hexagonal_layout::is_eastwards_of, "c1"_a, "c2"_a)
        .def("is_southwards_of", &py_hexagonal_layout::is_southwards_of, "c1"_a, "c2"_a)
        .def("is_westwards_of", &py_hexagonal_layout::is_westwards_of, "c1"_a, "c2"_a)

        .def("is_at_northern_border", &py_hexagonal_layout::is_at_northern_border, "c"_a)
        .def("is_at_eastern_border", &py_hexagonal_layout::is_at_eastern_border, "c"_a)
        .def("is_at_southern_border", &py_hexagonal_layout::is_at_southern_border, "c"_a)
        .def("is_at_western_border", &py_hexagonal_layout::is_at_western_border, "c"_a)
        .def("is_at_any_border", &py_hexagonal_layout::is_at_any_border, "c"_a)

        .def("northern_border_of", &py_hexagonal_layout::northern_border_of, "c"_a)
        .def("eastern_border_of", &py_hexagonal_layout::eastern_border_of, "c"_a)
        .def("southern_border_of", &py_hexagonal_layout::southern_border_of, "c"_a)
        .def("western_border_of", &py_hexagonal_layout::western_border_of, "c"_a)

        .def("is_ground_layer", &py_hexagonal_layout::is_ground_layer, "c"_a)
        .def("is_crossing_layer", &py_hexagonal_layout::is_crossing_layer, "c"_a)

        .def("is_within_bounds", &py_hexagonal_layout::is_within_bounds, "c"_a)

        .def("coordinates",
             [](const py_hexagonal_layout& lyt)
             {
                 std::vector<fiction::coordinate<py_hexagonal_layout>> coords{};
                 coords.reserve(lyt.area() * (lyt.z() + 1));
                 lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("ground_coordinates",
             [](const py_hexagonal_layout& lyt)
             {
                 std::vector<fiction::coordinate<py_hexagonal_layout>> coords{};
                 coords.reserve(lyt.area());
                 lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("adjacent_coordinates", &py_hexagonal_layout::adjacent_coordinates, "c"_a)
        .def("adjacent_opposite_coordinates", &py_hexagonal_layout::adjacent_opposite_coordinates, "c"_a)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONAL_LAYOUT_HPP
