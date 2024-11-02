//
// Created by marcel on 02.06.22.
//

#ifndef PYFICTION_CARTESIAN_LAYOUT_HPP
#define PYFICTION_CARTESIAN_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename CartesianLyt>
inline void cartesian_layout_impl(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    std::string coordinate_type = "";
    /**
     * Cartesian layout.
     */
    if constexpr (fiction::has_cube_coord_v<CartesianLyt>)
    {
        coordinate_type = "cube";
    }

    py::class_<CartesianLyt>(m, fmt::format("cartesian_layout{}", coordinate_type).c_str(),
                             DOC(fiction_cartesian_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<CartesianLyt>&>(), "dimension"_a,
             DOC(fiction_cartesian_layout_cartesian_layout))
        .def(
            "coord", [](const CartesianLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, "x"_a, "y"_a, "z"_a = 0l, DOC(fiction_cartesian_layout_coord))
        .def("x", &CartesianLyt::x, DOC(fiction_cartesian_layout_x))
        .def("y", &CartesianLyt::y, DOC(fiction_cartesian_layout_y))
        .def("z", &CartesianLyt::z, DOC(fiction_cartesian_layout_z))
        .def("area", &CartesianLyt::area, DOC(fiction_cartesian_layout_area))
        .def("resize", &CartesianLyt::resize, "dimension"_a, DOC(fiction_cartesian_layout_resize))

        .def("north", &CartesianLyt::north, "c"_a, DOC(fiction_cartesian_layout_north))
        .def("north_east", &CartesianLyt::north_east, "c"_a, DOC(fiction_cartesian_layout_north_east))
        .def("east", &CartesianLyt::east, "c"_a, DOC(fiction_cartesian_layout_east))
        .def("south_east", &CartesianLyt::south_east, "c"_a, DOC(fiction_cartesian_layout_south_east))
        .def("south", &CartesianLyt::south, "c"_a, DOC(fiction_cartesian_layout_south))
        .def("south_west", &CartesianLyt::south_west, "c"_a, DOC(fiction_cartesian_layout_south_west))
        .def("west", &CartesianLyt::west, "c"_a, DOC(fiction_cartesian_layout_west))
        .def("north_west", &CartesianLyt::north_west, "c"_a, DOC(fiction_cartesian_layout_north_west))
        .def("above", &CartesianLyt::above, "c"_a, DOC(fiction_cartesian_layout_above))
        .def("below", &CartesianLyt::below, "c"_a, DOC(fiction_cartesian_layout_below))

        .def("is_north_of", &CartesianLyt::is_north_of, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_north_of))
        .def("is_east_of", &CartesianLyt::is_east_of, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_east_of))
        .def("is_south_of", &CartesianLyt::is_south_of, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_south_of))
        .def("is_west_of", &CartesianLyt::is_west_of, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &CartesianLyt::is_adjacent_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &CartesianLyt::is_adjacent_elevation_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &CartesianLyt::is_above, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_above))
        .def("is_below", &CartesianLyt::is_below, "c1"_a, "c2"_a, DOC(fiction_cartesian_layout_is_below))
        .def("is_northwards_of", &CartesianLyt::is_northwards_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &CartesianLyt::is_eastwards_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &CartesianLyt::is_southwards_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &CartesianLyt::is_westwards_of, "c1"_a, "c2"_a,
             DOC(fiction_cartesian_layout_is_westwards_of))

        .def("is_at_northern_border", &CartesianLyt::is_at_northern_border, "c"_a,
             DOC(fiction_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &CartesianLyt::is_at_eastern_border, "c"_a,
             DOC(fiction_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &CartesianLyt::is_at_southern_border, "c"_a,
             DOC(fiction_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &CartesianLyt::is_at_western_border, "c"_a,
             DOC(fiction_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &CartesianLyt::is_at_any_border, "c"_a, DOC(fiction_cartesian_layout_is_at_any_border))

        .def("northern_border_of", &CartesianLyt::northern_border_of, "c"_a,
             DOC(fiction_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &CartesianLyt::eastern_border_of, "c"_a,
             DOC(fiction_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &CartesianLyt::southern_border_of, "c"_a,
             DOC(fiction_cartesian_layout_southern_border_of))
        .def("western_border_of", &CartesianLyt::western_border_of, "c"_a,
             DOC(fiction_cartesian_layout_western_border_of))

        .def("is_ground_layer", &CartesianLyt::is_ground_layer, "c"_a, DOC(fiction_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &CartesianLyt::is_crossing_layer, "c"_a,
             DOC(fiction_cartesian_layout_is_crossing_layer))

        .def("is_within_bounds", &CartesianLyt::is_within_bounds, "c"_a, DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const CartesianLyt& lyt)
            {
                std::vector<fiction::coordinate<CartesianLyt>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const CartesianLyt& lyt)
            {
                std::vector<fiction::coordinate<CartesianLyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_ground_coordinates))
        .def("adjacent_coordinates", &CartesianLyt::adjacent_coordinates, "c"_a,
             DOC(fiction_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &CartesianLyt::adjacent_opposite_coordinates, "c"_a,
             DOC(fiction_cartesian_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const CartesianLyt& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

};  // namespace detail

inline void cartesian_layout(pybind11::module& m)
{
    detail::cartesian_layout_impl<py_cartesian_layout>(m);
    detail::cartesian_layout_impl<py_cartesian_layout_cube>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CARTESIAN_LAYOUT_HPP
