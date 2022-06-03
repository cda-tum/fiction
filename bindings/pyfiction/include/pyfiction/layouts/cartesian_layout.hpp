//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_CARTESIAN_LAYOUT_HPP
#define PYFICTION_CARTESIAN_LAYOUT_HPP

#include "fiction/layouts/cartesian_layout.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <cstdint>
#include <vector>

namespace pyfiction
{

inline void cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Cartesian layout.
     */
    py::class_<fiction::cartesian_layout<fiction::offset::ucoord_t>>(m, "cartesian_layout")
        .def(py::init<>())
        .def(py::init<const fiction::offset::ucoord_t&>(), "dimension"_a)
        .def("x", &fiction::cartesian_layout<fiction::offset::ucoord_t>::x)
        .def("y", &fiction::cartesian_layout<fiction::offset::ucoord_t>::y)
        .def("z", &fiction::cartesian_layout<fiction::offset::ucoord_t>::z)
        .def("area", &fiction::cartesian_layout<fiction::offset::ucoord_t>::area)
        .def("resize", &fiction::cartesian_layout<fiction::offset::ucoord_t>::resize, "dimension"_a)

        .def("north", &fiction::cartesian_layout<fiction::offset::ucoord_t>::north)
        .def("north_east", &fiction::cartesian_layout<fiction::offset::ucoord_t>::north_east)
        .def("east", &fiction::cartesian_layout<fiction::offset::ucoord_t>::east)
        .def("south_east", &fiction::cartesian_layout<fiction::offset::ucoord_t>::south_east)
        .def("south", &fiction::cartesian_layout<fiction::offset::ucoord_t>::south)
        .def("south_west", &fiction::cartesian_layout<fiction::offset::ucoord_t>::south_west)
        .def("west", &fiction::cartesian_layout<fiction::offset::ucoord_t>::west)
        .def("north_west", &fiction::cartesian_layout<fiction::offset::ucoord_t>::north_west)
        .def("above", &fiction::cartesian_layout<fiction::offset::ucoord_t>::above)
        .def("below", &fiction::cartesian_layout<fiction::offset::ucoord_t>::below)

        .def("is_north_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_north_of)
        .def("is_east_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_east_of)
        .def("is_south_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_south_of)
        .def("is_west_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_west_of)
        .def("is_adjacent_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_adjacent_of)
        .def("is_adjacent_elevation_of",
             &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_adjacent_elevation_of)
        .def("is_above", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_above)
        .def("is_below", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_below)
        .def("is_northwards_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_northwards_of)
        .def("is_eastwards_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_eastwards_of)
        .def("is_southwards_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_southwards_of)
        .def("is_westwards_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_westwards_of)

        .def("is_at_northern_border", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_at_northern_border)
        .def("is_at_eastern_border", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_at_eastern_border)
        .def("is_at_southern_border", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_at_southern_border)
        .def("is_at_western_border", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_at_western_border)
        .def("is_at_any_border", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_at_any_border)

        .def("northern_border_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::northern_border_of)
        .def("eastern_border_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::eastern_border_of)
        .def("southern_border_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::southern_border_of)
        .def("western_border_of", &fiction::cartesian_layout<fiction::offset::ucoord_t>::western_border_of)

        .def("is_ground_layer", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_ground_layer)
        .def("is_crossing_layer", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_crossing_layer)

        .def("is_within_bounds", &fiction::cartesian_layout<fiction::offset::ucoord_t>::is_within_bounds)

        .def("coordinates",
             [](const fiction::cartesian_layout<fiction::offset::ucoord_t>& lyt)
             {
                 std::vector<fiction::offset::ucoord_t> coords{};
                 lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("ground_coordinates",
             [](const fiction::cartesian_layout<fiction::offset::ucoord_t>& lyt)
             {
                 std::vector<fiction::offset::ucoord_t> coords{};
                 lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                 return coords;
             })
        .def("adjacent_coordinates",
             [](const fiction::cartesian_layout<fiction::offset::ucoord_t>& lyt, const fiction::offset::ucoord_t& c)
             {
                 std::vector<fiction::offset::ucoord_t> coords{};
                 lyt.foreach_adjacent_coordinate(c, [&coords](const auto& ac) { coords.push_back(ac); });
                 return coords;
             })

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_CARTESIAN_LAYOUT_HPP
