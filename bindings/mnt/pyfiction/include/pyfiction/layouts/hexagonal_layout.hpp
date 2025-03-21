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
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

/**
 * A helper template that creates a Python binding for a given C++ HexLyt type.
 */
template <typename HexLyt>
inline void hexagonal_layout(pybind11::module& m, const std::string& coord_type)
{
    namespace py = pybind11;

    /**
     * Hexagonal layout.
     */
    py::class_<HexLyt>(m, fmt::format("hexagonal_layout_{}", coord_type).c_str(),
                       DOC(fiction_hexagonal_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<fiction::coordinate<HexLyt>>&>(), py::arg("ar"),
             DOC(fiction_hexagonal_layout_hexagonal_layout))
        .def(
            "coord", [](const HexLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, py::arg("x"), py::arg("y"), py::arg("z") = 0l,
            DOC(fiction_hexagonal_layout_coord))
        .def("x", &HexLyt::x, DOC(fiction_hexagonal_layout_x))
        .def("y", &HexLyt::y, DOC(fiction_hexagonal_layout_y))
        .def("z", &HexLyt::z, DOC(fiction_hexagonal_layout_z))
        .def("area", &HexLyt::area, DOC(fiction_hexagonal_layout_area))
        .def("volume", &HexLyt::volume, DOC(fiction_hexagonal_layout_volume))
        .def(
            "resize", [](HexLyt& lyt, const fiction::aspect_ratio_type_t<HexLyt>& dimension) { lyt.resize(dimension); },
            py::arg("dimension"), DOC(fiction_hexagonal_layout_resize))
        .def(
            "resize",
            [&](HexLyt& layout, py::tuple dimension)
            {
                auto ar = extract_aspect_ratio<HexLyt>(dimension);
                layout.resize(ar);
            },
            py::arg("dimension"), DOC(fiction_hexagonal_layout_resize_2))
        .def("north", &HexLyt::north, py::arg("c"), DOC(fiction_hexagonal_layout_north))
        .def("north_east", &HexLyt::north_east, py::arg("c"), DOC(fiction_hexagonal_layout_north_east))
        .def("east", &HexLyt::east, py::arg("c"), DOC(fiction_hexagonal_layout_east))
        .def("south_east", &HexLyt::south_east, py::arg("c"), DOC(fiction_hexagonal_layout_south_east))
        .def("south", &HexLyt::south, py::arg("c"), DOC(fiction_hexagonal_layout_south))
        .def("south_west", &HexLyt::south_west, py::arg("c"), DOC(fiction_hexagonal_layout_south_west))
        .def("west", &HexLyt::west, py::arg("c"), DOC(fiction_hexagonal_layout_west))
        .def("north_west", &HexLyt::north_west, py::arg("c"), DOC(fiction_hexagonal_layout_north_west))
        .def("above", &HexLyt::above, py::arg("c"), DOC(fiction_hexagonal_layout_above))
        .def("below", &HexLyt::below, py::arg("c"), DOC(fiction_hexagonal_layout_below))

        .def("is_north_of", &HexLyt::is_north_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_north_of))
        .def("is_east_of", &HexLyt::is_east_of, py::arg("c1"), py::arg("c2"), DOC(fiction_hexagonal_layout_is_east_of))
        .def("is_south_of", &HexLyt::is_south_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_south_of))
        .def("is_west_of", &HexLyt::is_west_of, py::arg("c1"), py::arg("c2"), DOC(fiction_hexagonal_layout_is_west_of))
        .def("is_adjacent_of", &HexLyt::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &HexLyt::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_adjacent_elevation_of))
        .def("is_above", &HexLyt::is_above, py::arg("c1"), py::arg("c2"), DOC(fiction_hexagonal_layout_is_above))
        .def("is_below", &HexLyt::is_below, py::arg("c1"), py::arg("c2"), DOC(fiction_hexagonal_layout_is_below))
        .def("is_northwards_of", &HexLyt::is_northwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_northwards_of))
        .def("is_eastwards_of", &HexLyt::is_eastwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_eastwards_of))
        .def("is_southwards_of", &HexLyt::is_southwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_southwards_of))
        .def("is_westwards_of", &HexLyt::is_westwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_hexagonal_layout_is_westwards_of))

        .def("is_at_northern_border", &HexLyt::is_at_northern_border, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_at_northern_border))
        .def("is_at_eastern_border", &HexLyt::is_at_eastern_border, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_at_eastern_border))
        .def("is_at_southern_border", &HexLyt::is_at_southern_border, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_at_southern_border))
        .def("is_at_western_border", &HexLyt::is_at_western_border, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_at_western_border))
        .def("is_at_any_border", &HexLyt::is_at_any_border, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_at_any_border))

        .def("northern_border_of", &HexLyt::northern_border_of, py::arg("c"),
             DOC(fiction_hexagonal_layout_northern_border_of))
        .def("eastern_border_of", &HexLyt::eastern_border_of, py::arg("c"),
             DOC(fiction_hexagonal_layout_eastern_border_of))
        .def("southern_border_of", &HexLyt::southern_border_of, py::arg("c"),
             DOC(fiction_hexagonal_layout_southern_border_of))
        .def("western_border_of", &HexLyt::western_border_of, py::arg("c"),
             DOC(fiction_hexagonal_layout_western_border_of))

        .def("is_ground_layer", &HexLyt::is_ground_layer, py::arg("c"), DOC(fiction_hexagonal_layout_is_ground_layer))
        .def("is_crossing_layer", &HexLyt::is_crossing_layer, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_crossing_layer))

        .def("is_within_bounds", &HexLyt::is_within_bounds, py::arg("c"),
             DOC(fiction_hexagonal_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const HexLyt& lyt)
            {
                std::vector<fiction::coordinate<HexLyt>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_hexagonal_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const HexLyt& lyt)
            {
                std::vector<fiction::coordinate<HexLyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_hexagonal_layout_ground_coordinates))
        .def("adjacent_coordinates", &HexLyt::adjacent_coordinates, py::arg("c"),
             DOC(fiction_hexagonal_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &HexLyt::adjacent_opposite_coordinates, py::arg("c"),
             DOC(fiction_hexagonal_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const HexLyt& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace detail

inline void hexagonal_layouts(pybind11::module& m)
{
    /**
     * Hexagonal layout with offset coordinates.
     */
    detail::hexagonal_layout<py_hexagonal_layout>(m, "offset_coordinates");
}

/**
 * A "factory" function that Python users can call as
 *   hexagonal_layout(dimension).
 */
inline void hexagonal_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "hexagonal_layout",
        [](const py::tuple dimension)
        {
            const auto ar = extract_aspect_ratio<py_hexagonal_layout>(dimension);
            return py::cast(py_hexagonal_layout{ar});
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0),
        R"doc(
            Creates and returns a hexagonal_layout instance.

            For the dimension, you can pass either:
              - A single tuple (x, y) or (x, y, z) to specify only the "max" coordinate, with min defaulting to (0,0,0),
              - Two nested tuples ((xmin, ymin), (xmax, ymax)) or 3D
                ((xmin, ymin, zmin), (xmax, ymax, zmax)) to specify min and max explicitly.
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONAL_LAYOUT_HPP
