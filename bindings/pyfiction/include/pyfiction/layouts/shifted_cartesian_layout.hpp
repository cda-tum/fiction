//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_SHIFTED_CARTESIAN_LAYOUT_HPP
#define PYFICTION_SHIFTED_CARTESIAN_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <string>
#include <vector>

namespace pyfiction
{

inline void shifted_cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Shifted Cartesian layout.
     *
     * @note All functions had to be redefined, because in the regular C++ version, this layout extends a specific
     * hexagonal layout, which we do not expose in pyfiction.
     */
    py::class_<py_shifted_cartesian_layout>(m, "shifted_cartesian_layout",
                                            DOC(fiction_shifted_cartesian_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<py_shifted_cartesian_layout>&>(), "dimension"_a,
             DOC(fiction_shifted_cartesian_layout_shifted_cartesian_layout))
        .def(
            "coord", [](const py_shifted_cartesian_layout& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, "x"_a, "y"_a, "z"_a = 0l, DOC(fiction_cartesian_layout_coord))
        .def(
            "x", [](const py_shifted_cartesian_layout& lyt) { return lyt.x(); }, DOC(fiction_cartesian_layout_x))
        .def(
            "y", [](const py_shifted_cartesian_layout& lyt) { return lyt.y(); }, DOC(fiction_cartesian_layout_y))
        .def(
            "z", [](const py_shifted_cartesian_layout& lyt) { return lyt.z(); }, DOC(fiction_cartesian_layout_z))
        .def(
            "area", [](const py_shifted_cartesian_layout& lyt) { return lyt.area(); },
            DOC(fiction_cartesian_layout_area))
        .def(
            "resize", [](py_shifted_cartesian_layout& lyt, const py_offset_coordinate& dimension)
            { lyt.resize(dimension); }, "dimension"_a, DOC(fiction_cartesian_layout_resize))
        .def(
            "north", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.north(c); },
            "c"_a, DOC(fiction_cartesian_layout_north))
        .def(
            "north_east", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.north_east(c); }, "c"_a, DOC(fiction_cartesian_layout_north_east))
        .def(
            "east", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.east(c); },
            "c"_a, DOC(fiction_cartesian_layout_east))
        .def(
            "south_east", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.south_east(c); }, "c"_a, DOC(fiction_cartesian_layout_south_east))
        .def(
            "south", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.south(c); },
            "c"_a, DOC(fiction_cartesian_layout_south))
        .def(
            "south_west", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.south_west(c); }, "c"_a, DOC(fiction_cartesian_layout_south_west))
        .def(
            "west", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.west(c); },
            "c"_a, DOC(fiction_cartesian_layout_west))
        .def(
            "north_west", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.north_west(c); }, "c"_a, DOC(fiction_cartesian_layout_north_west))
        .def(
            "above", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.above(c); },
            "c"_a, DOC(fiction_cartesian_layout_above))
        .def(
            "below", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c) { return lyt.below(c); },
            "c"_a, DOC(fiction_cartesian_layout_below))

        .def(
            "is_north_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_north_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_north_of))
        .def(
            "is_east_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_east_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_east_of))
        .def(
            "is_south_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_south_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_south_of))
        .def(
            "is_west_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_west_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_west_of))
        .def(
            "is_adjacent_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_adjacent_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_adjacent_of))
        .def(
            "is_adjacent_elevation_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_adjacent_elevation_of(c1, c2); }, "c1"_a, "c2"_a,
            DOC(fiction_cartesian_layout_is_adjacent_elevation_of))
        .def(
            "is_above",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_above(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_above))
        .def(
            "is_below",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_below(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_below))
        .def(
            "is_northwards_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_northwards_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_northwards_of))
        .def(
            "is_eastwards_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_eastwards_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_eastwards_of))
        .def(
            "is_southwards_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_southwards_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_southwards_of))
        .def(
            "is_westwards_of",
            [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c1, const py_offset_coordinate& c2)
            { return lyt.is_westwards_of(c1, c2); }, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_westwards_of))

        .def(
            "is_at_northern_border", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_at_northern_border(c); }, "c"_a, DOC(fiction_cartesian_layout_is_at_northern_border))
        .def(
            "is_at_eastern_border", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_at_eastern_border(c); }, "c"_a, DOC(fiction_cartesian_layout_is_at_eastern_border))
        .def(
            "is_at_southern_border", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_at_southern_border(c); }, "c"_a, DOC(fiction_cartesian_layout_is_at_southern_border))
        .def(
            "is_at_western_border", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_at_western_border(c); }, "c"_a, DOC(fiction_cartesian_layout_is_at_western_border))
        .def(
            "is_at_any_border", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_at_any_border(c); }, "c"_a, DOC(fiction_cartesian_layout_is_at_any_border))

        .def(
            "northern_border_of", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.northern_border_of(c); }, "c"_a, DOC(fiction_cartesian_layout_northern_border_of))
        .def(
            "eastern_border_of", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.eastern_border_of(c); }, "c"_a, DOC(fiction_cartesian_layout_eastern_border_of))
        .def(
            "southern_border_of", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.southern_border_of(c); }, "c"_a, DOC(fiction_cartesian_layout_southern_border_of))
        .def(
            "western_border_of", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.western_border_of(c); }, "c"_a, DOC(fiction_cartesian_layout_western_border_of))

        .def(
            "is_ground_layer", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_ground_layer(c); }, "c"_a, DOC(fiction_cartesian_layout_is_ground_layer))
        .def(
            "is_crossing_layer", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_crossing_layer(c); }, "c"_a, DOC(fiction_cartesian_layout_is_crossing_layer))

        .def(
            "is_within_bounds", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.is_within_bounds(c); }, "c"_a, DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const py_shifted_cartesian_layout& lyt)
            {
                std::vector<fiction::coordinate<py_shifted_cartesian_layout>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const py_shifted_cartesian_layout& lyt)
            {
                std::vector<fiction::coordinate<py_shifted_cartesian_layout>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_ground_coordinates))
        .def(
            "adjacent_coordinates", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.adjacent_coordinates(c); }, "c"_a, DOC(fiction_cartesian_layout_adjacent_coordinates))
        .def(
            "adjacent_opposite_coordinates", [](const py_shifted_cartesian_layout& lyt, const py_offset_coordinate& c)
            { return lyt.adjacent_opposite_coordinates(c); }, "c"_a,
            DOC(fiction_cartesian_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const py_shifted_cartesian_layout& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_SHIFTED_CARTESIAN_LAYOUT_HPP
