//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_HEXAGONAL_LAYOUT_HPP
#define PYFICTION_HEXAGONAL_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <vector>

namespace pyfiction
{

inline void hexagonal_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using hex_lyt = fiction::hexagonal_layout<fiction::offset::ucoord_t, fiction::even_row_hex>;

    /**
     * Hexagonal layout.
     */
    py::class_<hex_lyt>(m, "hexagonal_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<hex_lyt>&>(), "dimension"_a)
        .def("x", &hex_lyt::x)
        .def("y", &hex_lyt::y)
        .def("z", &hex_lyt::z)
        .def("area", &hex_lyt::area)
        .def("resize", &hex_lyt::resize, "dimension"_a)

        .def("north", &hex_lyt::north)
        .def("north_east", &hex_lyt::north_east)
        .def("east", &hex_lyt::east)
        .def("south_east", &hex_lyt::south_east)
        .def("south", &hex_lyt::south)
        .def("south_west", &hex_lyt::south_west)
        .def("west", &hex_lyt::west)
        .def("north_west", &hex_lyt::north_west)
        .def("above", &hex_lyt::above)
        .def("below", &hex_lyt::below)

        .def("is_north_of", &hex_lyt::is_north_of)
        .def("is_east_of", &hex_lyt::is_east_of)
        .def("is_south_of", &hex_lyt::is_south_of)
        .def("is_west_of", &hex_lyt::is_west_of)
        .def("is_adjacent_of", &hex_lyt::is_adjacent_of)
        .def("is_adjacent_elevation_of", &hex_lyt::is_adjacent_elevation_of)
        .def("is_above", &hex_lyt::is_above)
        .def("is_below", &hex_lyt::is_below)
        .def("is_northwards_of", &hex_lyt::is_northwards_of)
        .def("is_eastwards_of", &hex_lyt::is_eastwards_of)
        .def("is_southwards_of", &hex_lyt::is_southwards_of)
        .def("is_westwards_of", &hex_lyt::is_westwards_of)

        .def("is_at_northern_border", &hex_lyt::is_at_northern_border)
        .def("is_at_eastern_border", &hex_lyt::is_at_eastern_border)
        .def("is_at_southern_border", &hex_lyt::is_at_southern_border)
        .def("is_at_western_border", &hex_lyt::is_at_western_border)
        .def("is_at_any_border", &hex_lyt::is_at_any_border)

        .def("northern_border_of", &hex_lyt::northern_border_of)
        .def("eastern_border_of", &hex_lyt::eastern_border_of)
        .def("southern_border_of", &hex_lyt::southern_border_of)
        .def("western_border_of", &hex_lyt::western_border_of)

        .def("is_ground_layer", &hex_lyt::is_ground_layer)
        .def("is_crossing_layer", &hex_lyt::is_crossing_layer)

        .def("is_within_bounds", &hex_lyt::is_within_bounds)

        .def("coordinates",
             [](const hex_lyt& lyt)
             {
                 std::vector<fiction::coordinate<hex_lyt>> coords{};
                 coords.reserve(lyt.area() * (lyt.z() + 1));
                 lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("ground_coordinates",
             [](const hex_lyt& lyt)
             {
                 std::vector<fiction::coordinate<hex_lyt>> coords{};
                 coords.reserve(lyt.area());
                 lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("adjacent_coordinates", &hex_lyt::adjacent_coordinates)
        .def("adjacent_opposite_coordinates", &hex_lyt::adjacent_opposite_coordinates)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONAL_LAYOUT_HPP
