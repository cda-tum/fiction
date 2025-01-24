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
#include <sstream>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

/**
 * A helper template that creates a Python binding for a given C++ ShiftedCartLyt type.
 * \param coord_type is appended to the Python class name so you get
 *        "shifted_cartesian_layout_offset_coordinates" or "shifted_cartesian_layout_cube_coordinates".
 */
template <typename ShiftedCartLyt, typename CoordType>
inline void shifted_cartesian_layout(pybind11::module& m, const std::string& coord_type)
{
    namespace py = pybind11;
    namespace py = pybind11;

    /**
     * Shifted Cartesian layout.
     *
     * @note All functions had to be redefined, because in the regular C++ version, this layout extends a specific
     * hexagonal layout, which we do not expose in pyfiction.
     */
    py::class_<ShiftedCartLyt>(m, fmt::format("shifted_cartesian_layout_{}", coord_type).c_str(),
                               DOC(fiction_shifted_cartesian_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<ShiftedCartLyt>&>(), py::arg("dimension"),
             DOC(fiction_shifted_cartesian_layout_shifted_cartesian_layout))
        .def(
            "coord", [](const ShiftedCartLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, py::arg("x"), py::arg("y"), py::arg("z") = 0l,
            DOC(fiction_cartesian_layout_coord))
        .def(
            "x", [](const ShiftedCartLyt& lyt) { return lyt.x(); }, DOC(fiction_cartesian_layout_x))
        .def(
            "y", [](const ShiftedCartLyt& lyt) { return lyt.y(); }, DOC(fiction_cartesian_layout_y))
        .def(
            "z", [](const ShiftedCartLyt& lyt) { return lyt.z(); }, DOC(fiction_cartesian_layout_z))
        .def(
            "area", [](const ShiftedCartLyt& lyt) { return lyt.area(); }, DOC(fiction_cartesian_layout_area))
        .def(
            "resize", [](ShiftedCartLyt& lyt, const CoordType& dimension) { lyt.resize(dimension); },
            py::arg("dimension"), DOC(fiction_cartesian_layout_resize))
        .def(
            "north", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.north(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_north))
        .def(
            "north_east", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.north_east(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_north_east))
        .def(
            "east", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.east(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_east))
        .def(
            "south_east", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.south_east(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_south_east))
        .def(
            "south", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.south(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_south))
        .def(
            "south_west", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.south_west(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_south_west))
        .def(
            "west", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.west(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_west))
        .def(
            "north_west", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.north_west(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_north_west))
        .def(
            "above", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.above(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_above))
        .def(
            "below", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.below(c); }, py::arg("c"),
            DOC(fiction_cartesian_layout_below))

        .def(
            "is_north_of",
            [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2) { return lyt.is_north_of(c1, c2); },
            py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_north_of))
        .def(
            "is_east_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_east_of(c1, c2); }, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_east_of))
        .def(
            "is_south_of",
            [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2) { return lyt.is_south_of(c1, c2); },
            py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_south_of))
        .def(
            "is_west_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_west_of(c1, c2); }, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_west_of))
        .def(
            "is_adjacent_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_adjacent_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_adjacent_of))
        .def(
            "is_adjacent_elevation_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_adjacent_elevation_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_adjacent_elevation_of))
        .def(
            "is_above", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_above(c1, c2); }, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_above))
        .def(
            "is_below", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_below(c1, c2); }, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_below))
        .def(
            "is_northwards_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_northwards_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_northwards_of))
        .def(
            "is_eastwards_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_eastwards_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_eastwards_of))
        .def(
            "is_southwards_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_southwards_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_southwards_of))
        .def(
            "is_westwards_of", [](const ShiftedCartLyt& lyt, const CoordType& c1, const CoordType& c2)
            { return lyt.is_westwards_of(c1, c2); }, py::arg("c1"), py::arg("c2"),
            DOC(fiction_cartesian_layout_is_westwards_of))

        .def(
            "is_at_northern_border", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.is_at_northern_border(c); }, py::arg("c"), DOC(fiction_cartesian_layout_is_at_northern_border))
        .def(
            "is_at_eastern_border", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.is_at_eastern_border(c); }, py::arg("c"), DOC(fiction_cartesian_layout_is_at_eastern_border))
        .def(
            "is_at_southern_border", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.is_at_southern_border(c); }, py::arg("c"), DOC(fiction_cartesian_layout_is_at_southern_border))
        .def(
            "is_at_western_border", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.is_at_western_border(c); }, py::arg("c"), DOC(fiction_cartesian_layout_is_at_western_border))
        .def(
            "is_at_any_border", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.is_at_any_border(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_is_at_any_border))

        .def(
            "northern_border_of", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.northern_border_of(c); }, py::arg("c"), DOC(fiction_cartesian_layout_northern_border_of))
        .def(
            "eastern_border_of", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.eastern_border_of(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_eastern_border_of))
        .def(
            "southern_border_of", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.southern_border_of(c); }, py::arg("c"), DOC(fiction_cartesian_layout_southern_border_of))
        .def(
            "western_border_of", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.western_border_of(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_western_border_of))

        .def(
            "is_ground_layer", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.is_ground_layer(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_is_ground_layer))
        .def(
            "is_crossing_layer", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.is_crossing_layer(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_is_crossing_layer))

        .def(
            "is_within_bounds", [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.is_within_bounds(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const ShiftedCartLyt& lyt)
            {
                std::vector<fiction::coordinate<ShiftedCartLyt>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const ShiftedCartLyt& lyt)
            {
                std::vector<fiction::coordinate<ShiftedCartLyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_ground_coordinates))
        .def(
            "adjacent_coordinates", [](const ShiftedCartLyt& lyt, const CoordType& c)
            { return lyt.adjacent_coordinates(c); }, py::arg("c"), DOC(fiction_cartesian_layout_adjacent_coordinates))
        .def(
            "adjacent_opposite_coordinates",
            [](const ShiftedCartLyt& lyt, const CoordType& c) { return lyt.adjacent_opposite_coordinates(c); },
            py::arg("c"), DOC(fiction_cartesian_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const ShiftedCartLyt& lyt) -> std::string
             {
                 std::stringstream stream{};
                 print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace detail

/**
 * Register two different layout classes under two different Python names:
 *   - "shifted_cartesian_layout_offset_coordinates"
 *   - "shifted_cartesian_layout_cube_coordinates"
 */
inline void shifted_cartesian_layouts(pybind11::module& m)
{
    /**
     * Shifted Cartesian layout with offset coordinates.
     */
    detail::shifted_cartesian_layout<py_shifted_cartesian_layout, py_offset_coordinate>(m, "offset_coordinates");
    /**
     * Shifted Cartesian layout with cube coordinates.
     */
    detail::shifted_cartesian_layout<py_shifted_cartesian_layout_cube_coordinates, py_cube_coordinate>(
        m, "cube_coordinates");
}

/**
 * A "factory" function that Python users can call as
 *   shifted_cartesian_layout(dimension, coordinate_type="offset")
 * to create the correct layout type (offset or cube).
 */
inline void shifted_cartesian_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "shifted_cartesian_layout",
        [](const py::tuple dimension, const std::string& coordinate_type)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_shifted_cartesian_layout_cube_coordinates>(dimension);
                return py::cast(py_shifted_cartesian_layout_cube_coordinates{ar});
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_shifted_cartesian_layout>(dimension);
                return py::cast(py_shifted_cartesian_layout{ar});
            }
        },
        py::arg("dimension"),
        py::arg("coordinate_type") = "offset",  // default
        R"doc(
            Creates and returns a shifted_cartesian_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:

                - "offset" (default)
                - "cube"
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_SHIFTED_CARTESIAN_LAYOUT_HPP
