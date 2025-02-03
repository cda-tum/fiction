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
#include <stdexcept>
#include <string>
#include <vector>

namespace pyfiction
{

namespace detail
{

/**
 * A helper template that creates a Python binding for a given C++ CartLyt type.
 * \param coord_type is appended to the Python class name so you get "cartesian_layout_offset_coordinates" or
 * "cartesian_layout_cube_coordinates".
 */
template <typename CartLyt>
void cartesian_layout(pybind11::module& m, const std::string& coord_type)
{
    namespace py = pybind11;

    /**
     * Cartesian layout.
     */
    py::class_<CartLyt>(m, fmt::format("cartesian_layout_{}", coord_type).c_str(),
                        DOC(fiction_cartesian_layout_overridden))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<CartLyt>&>(), py::arg("dimension"),
             DOC(fiction_cartesian_layout_cartesian_layout))
        .def(
            "coord", [](const CartLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, py::arg("x"), py::arg("y"), py::arg("z") = 0l,
            DOC(fiction_cartesian_layout_coord))
        .def("x", &CartLyt::x, DOC(fiction_cartesian_layout_x))
        .def("y", &CartLyt::y, DOC(fiction_cartesian_layout_y))
        .def("z", &CartLyt::z, DOC(fiction_cartesian_layout_z))
        .def("area", &CartLyt::area, DOC(fiction_cartesian_layout_area))
        .def("resize", &CartLyt::resize, py::arg("dimension"), DOC(fiction_cartesian_layout_resize))

        .def("north", &CartLyt::north, py::arg("c"), DOC(fiction_cartesian_layout_north))
        .def("north_east", &CartLyt::north_east, py::arg("c"), DOC(fiction_cartesian_layout_north_east))
        .def("east", &CartLyt::east, py::arg("c"), DOC(fiction_cartesian_layout_east))
        .def("south_east", &CartLyt::south_east, py::arg("c"), DOC(fiction_cartesian_layout_south_east))
        .def("south", &CartLyt::south, py::arg("c"), DOC(fiction_cartesian_layout_south))
        .def("south_west", &CartLyt::south_west, py::arg("c"), DOC(fiction_cartesian_layout_south_west))
        .def("west", &CartLyt::west, py::arg("c"), DOC(fiction_cartesian_layout_west))
        .def("north_west", &CartLyt::north_west, py::arg("c"), DOC(fiction_cartesian_layout_north_west))
        .def("above", &CartLyt::above, py::arg("c"), DOC(fiction_cartesian_layout_above))
        .def("below", &CartLyt::below, py::arg("c"), DOC(fiction_cartesian_layout_below))

        .def("is_north_of", &CartLyt::is_north_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_north_of))
        .def("is_east_of", &CartLyt::is_east_of, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_east_of))
        .def("is_south_of", &CartLyt::is_south_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_south_of))
        .def("is_west_of", &CartLyt::is_west_of, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &CartLyt::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &CartLyt::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &CartLyt::is_above, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_above))
        .def("is_below", &CartLyt::is_below, py::arg("c1"), py::arg("c2"), DOC(fiction_cartesian_layout_is_below))
        .def("is_northwards_of", &CartLyt::is_northwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &CartLyt::is_eastwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &CartLyt::is_southwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &CartLyt::is_westwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_westwards_of))

        .def("is_at_northern_border", &CartLyt::is_at_northern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &CartLyt::is_at_eastern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &CartLyt::is_at_southern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &CartLyt::is_at_western_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &CartLyt::is_at_any_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_any_border))

        .def("northern_border_of", &CartLyt::northern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &CartLyt::eastern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &CartLyt::southern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_southern_border_of))
        .def("western_border_of", &CartLyt::western_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_western_border_of))

        .def("is_ground_layer", &CartLyt::is_ground_layer, py::arg("c"), DOC(fiction_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &CartLyt::is_crossing_layer, py::arg("c"),
             DOC(fiction_cartesian_layout_is_crossing_layer))

        .def("is_within_bounds", &CartLyt::is_within_bounds, py::arg("c"),
             DOC(fiction_cartesian_layout_is_within_bounds))

        .def(
            "coordinates",
            [](const CartLyt& lyt)
            {
                std::vector<fiction::coordinate<CartLyt>> coords{};
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_coordinates))
        .def(
            "ground_coordinates",
            [](const CartLyt& lyt)
            {
                std::vector<fiction::coordinate<CartLyt>> coords{};
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&coords](const auto& c) { coords.push_back(c); });
                return coords;
            },
            DOC(fiction_cartesian_layout_ground_coordinates))
        .def("adjacent_coordinates", &CartLyt::adjacent_coordinates, py::arg("c"),
             DOC(fiction_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &CartLyt::adjacent_opposite_coordinates, py::arg("c"),
             DOC(fiction_cartesian_layout_adjacent_opposite_coordinates))

        .def("__repr__",
             [](const CartLyt& lyt) -> std::string
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
 *   - "cartesian_layout_offset_coordinates"
 *   - "cartesian_layout_cube_coordinates"
 */
inline void cartesian_layouts(pybind11::module& m)
{
    /**
     * Cartesian layout with offset coordinates.
     */
    detail::cartesian_layout<py_cartesian_layout>(m, "offset_coordinates");
    /**
     * Cartesian layout with cube coordinates.
     */
    detail::cartesian_layout<py_cartesian_layout_cube_coordinates>(m, "cube_coordinates");
}

template <typename CoordLyt>
fiction::aspect_ratio_t<CoordLyt> extract_aspect_ratio(pybind11::tuple dimension)
{
    if (dimension.size() < 2 || dimension.size() > 3)
    {
        throw std::runtime_error("dimension must be a 2-tuple (x, y) or a 3-tuple (x, y, z).");
    }

    // Extract x, y from the tuple
    uint64_t x = dimension[0].cast<uint64_t>();
    uint64_t y = dimension[1].cast<uint64_t>();
    // Default z = 0
    uint64_t z = 0;
    // If a 3rd element is provided, override z
    if (dimension.size() == 3)
    {
        z = dimension[2].cast<uint64_t>();
    }

    fiction::aspect_ratio_t<CoordLyt> ar{x, y, z};

    return ar;
}
/**
 * A "factory" function that Python users can call as
 *   cartesian_layout(dimension, coordinate_type="offset")
 * to create the correct layout type (offset or cube).
 */
inline void cartesian_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "cartesian_layout",
        [](const py::tuple dimension, const std::string& coordinate_type)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout_cube_coordinates>(dimension);
                return py::cast(py_cartesian_layout_cube_coordinates{ar});
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout>(dimension);
                return py::cast(py_cartesian_layout{ar});
            }
        },
        py::arg("dimension"),
        py::arg("coordinate_type") = "offset",  // default
        R"doc(
            Creates and returns a cartesian_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:

                - "offset" (default)
                - "cube"
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_CARTESIAN_LAYOUT_HPP
