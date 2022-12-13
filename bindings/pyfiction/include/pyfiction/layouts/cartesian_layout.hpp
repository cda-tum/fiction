//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_CARTESIAN_LAYOUT_HPP
#define PYFICTION_CARTESIAN_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <vector>

namespace pyfiction
{

inline void cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using cart_lyt = fiction::cartesian_layout<fiction::offset::ucoord_t>;

    /**
     * Cartesian layout.
     */
    py::class_<cart_lyt>(m, "cartesian_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<cart_lyt>&>(), "dimension"_a)
        .def("x", &cart_lyt::x)
        .def("y", &cart_lyt::y)
        .def("z", &cart_lyt::z)
        .def("area", &cart_lyt::area)
        .def("resize", &cart_lyt::resize, "dimension"_a)

        .def("north", &cart_lyt::north)
        .def("north_east", &cart_lyt::north_east)
        .def("east", &cart_lyt::east)
        .def("south_east", &cart_lyt::south_east)
        .def("south", &cart_lyt::south)
        .def("south_west", &cart_lyt::south_west)
        .def("west", &cart_lyt::west)
        .def("north_west", &cart_lyt::north_west)
        .def("above", &cart_lyt::above)
        .def("below", &cart_lyt::below)

        .def("is_north_of", &cart_lyt::is_north_of)
        .def("is_east_of", &cart_lyt::is_east_of)
        .def("is_south_of", &cart_lyt::is_south_of)
        .def("is_west_of", &cart_lyt::is_west_of)
        .def("is_adjacent_of", &cart_lyt::is_adjacent_of)
        .def("is_adjacent_elevation_of", &cart_lyt::is_adjacent_elevation_of)
        .def("is_above", &cart_lyt::is_above)
        .def("is_below", &cart_lyt::is_below)
        .def("is_northwards_of", &cart_lyt::is_northwards_of)
        .def("is_eastwards_of", &cart_lyt::is_eastwards_of)
        .def("is_southwards_of", &cart_lyt::is_southwards_of)
        .def("is_westwards_of", &cart_lyt::is_westwards_of)

        .def("is_at_northern_border", &cart_lyt::is_at_northern_border)
        .def("is_at_eastern_border", &cart_lyt::is_at_eastern_border)
        .def("is_at_southern_border", &cart_lyt::is_at_southern_border)
        .def("is_at_western_border", &cart_lyt::is_at_western_border)
        .def("is_at_any_border", &cart_lyt::is_at_any_border)

        .def("northern_border_of", &cart_lyt::northern_border_of)
        .def("eastern_border_of", &cart_lyt::eastern_border_of)
        .def("southern_border_of", &cart_lyt::southern_border_of)
        .def("western_border_of", &cart_lyt::western_border_of)

        .def("is_ground_layer", &cart_lyt::is_ground_layer)
        .def("is_crossing_layer", &cart_lyt::is_crossing_layer)

        .def("is_within_bounds", &cart_lyt::is_within_bounds)

        .def("coordinates",
             [](const cart_lyt& lyt)
             {
                 std::vector<fiction::coordinate<cart_lyt>> coords{};
                 coords.reserve(lyt.area() * (lyt.z() + 1));
                 lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("ground_coordinates",
             [](const cart_lyt& lyt)
             {
                 std::vector<fiction::coordinate<cart_lyt>> coords{};
                 coords.reserve(lyt.area());
                 lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("adjacent_coordinates", &cart_lyt::adjacent_coordinates)
        .def("adjacent_opposite_coordinates", &cart_lyt::adjacent_opposite_coordinates)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_CARTESIAN_LAYOUT_HPP
