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

template <typename CoordLyt>
inline fiction::aspect_ratio_type_t<CoordLyt> extract_aspect_ratio(pybind11::tuple dimension)
{
    namespace py = pybind11;

    // 1) Figure out what coordinate type this layout uses:
    //    "cube" => fiction::cube::coord_t
    //    "offset" => fiction::offset::ucoord_t
    using coordinate_t = typename CoordLyt::coordinate;

    // 2) Decide on signed or unsigned integral to parse from Python
    //    – Typically you want signed for "cube" (which can go negative)
    //    – Typically you want unsigned for "offset" (non-negative)
    using parse_integral_t =
        std::conditional_t<std::is_same_v<coordinate_t, fiction::cube::coord_t>, int32_t, uint64_t>;

    // 3) A small lambda that does the cast from Python int => parse_integral_t
    auto get_val = [&](py::handle h) { return h.cast<parse_integral_t>(); };

    // 4) dimension must be a Python tuple
    if (!py::isinstance<py::tuple>(dimension))
        throw std::runtime_error("dimension must be a tuple or nested tuples.");

    py::tuple  dimension_tuple = dimension.cast<py::tuple>();
    const auto N               = dimension_tuple.size();

    // -- CASE A: Check if dimension_tuple is 2-length and each element is a sub-tuple => (min, max)
    //            i.e. dimension_tuple = ((xmin, ymin[, zmin]), (xmax, ymax[, zmax]))
    if (N == 2 && py::isinstance<py::tuple>(dimension_tuple[0]) && py::isinstance<py::tuple>(dimension_tuple[1]))
    {
        py::tuple tmin = dimension_tuple[0].cast<py::tuple>();
        py::tuple tmax = dimension_tuple[1].cast<py::tuple>();

        if (tmin.size() < 2 || tmin.size() > 3)
            throw std::runtime_error("Min tuple must have 2 or 3 elements.");
        if (tmax.size() < 2 || tmax.size() > 3)
            throw std::runtime_error("Max tuple must have 2 or 3 elements.");

        // parse min:
        auto xmin = get_val(tmin[0]);
        auto ymin = get_val(tmin[1]);
        auto zmin = (tmin.size() == 3) ? get_val(tmin[2]) : parse_integral_t{0};

        // parse max:
        auto xmax = get_val(tmax[0]);
        auto ymax = get_val(tmax[1]);
        auto zmax = (tmax.size() == 3) ? get_val(tmax[2]) : parse_integral_t{0};

        // Make actual coordinate objects for aspect_ratio(min, max)
        coordinate_t cmin{xmin, ymin, zmin};
        coordinate_t cmax{xmax, ymax, zmax};

        // We can now call aspect_ratio(cmin, cmax)
        return fiction::aspect_ratio_type_t<CoordLyt>{cmin, cmax};
    }

    // -- CASE B: Single-tuple => "max" only => aspect_ratio(x, y, z)
    //            dimension_tuple = (x, y[, z])
    else
    {
        if (N < 2 || N > 3)
            throw std::runtime_error("dimension must be (x,y) or (x,y,z) if passing only one tuple.");

        auto x = get_val(dimension_tuple[0]);
        auto y = get_val(dimension_tuple[1]);
        auto z = (N == 3) ? get_val(dimension_tuple[2]) : parse_integral_t{0};

        // The aspect_ratio constructor template <X,Y,Z> aspect_ratio(X x, Y y, Z z)
        // sets min=(0,0,0) and max=(x,y,z).
        // Just pass them as integrals and let the template do the rest:
        return fiction::aspect_ratio_type_t<CoordLyt>{x, y, z};
    }
}

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
        .def(py::init<const fiction::aspect_ratio<fiction::coordinate<CartLyt>>&>(), py::arg("ar"),
             DOC(fiction_cartesian_layout_cartesian_layout))
        .def(
            "coord", [](const CartLyt& layout, const int64_t x, const int64_t y, const int64_t z)
            { return layout.coord(x, y, z); }, py::arg("x"), py::arg("y"), py::arg("z") = 0l,
            DOC(fiction_cartesian_layout_coord))
        .def("x", &CartLyt::x, DOC(fiction_cartesian_layout_x))
        .def("y", &CartLyt::y, DOC(fiction_cartesian_layout_y))
        .def("z", &CartLyt::z, DOC(fiction_cartesian_layout_z))
        .def("x_min", &CartLyt::x_min, DOC(fiction_cartesian_layout_x_min))
        .def("y_min", &CartLyt::y_min, DOC(fiction_cartesian_layout_y_min))
        .def("z_min", &CartLyt::z_min, DOC(fiction_cartesian_layout_z_min))
        .def("x_size", &CartLyt::x_size, DOC(fiction_cartesian_layout_x_size))
        .def("y_size", &CartLyt::y_size, DOC(fiction_cartesian_layout_y_size))
        .def("z_size", &CartLyt::z_size, DOC(fiction_cartesian_layout_z_size))
        .def("area", &CartLyt::area, DOC(fiction_cartesian_layout_area))
        .def("volume", &CartLyt::volume, DOC(fiction_cartesian_layout_volume))
        .def(
            "resize", [](CartLyt& lyt, const fiction::aspect_ratio_type_t<CartLyt>& dimension)
            { lyt.resize(dimension); }, py::arg("dimension"), DOC(fiction_cartesian_layout_resize))
        .def(
            "resize",
            [&](CartLyt& layout, py::tuple dimension)
            {
                auto ar = extract_aspect_ratio<CartLyt>(dimension);
                layout.resize(ar);
            },
            py::arg("dimension"), DOC(fiction_cartesian_layout_resize_2))

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
    detail::cartesian_layout<py_cartesian_layout<py_offset_coordinate>>(m, "offset_coordinates");
    /**
     * Cartesian layout with cube coordinates.
     */
    detail::cartesian_layout<py_cartesian_layout<py_cube_coordinate>>(m, "cube_coordinates");
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
                const auto ar = extract_aspect_ratio<py_cartesian_layout<py_cube_coordinate>>(dimension);
                return py::cast(py_cartesian_layout<py_cube_coordinate>{ar});
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout<py_offset_coordinate>>(dimension);
                return py::cast(py_cartesian_layout<py_offset_coordinate>{ar});
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("coordinate_type") = "offset",
        R"doc(
        Creates and returns a cartesian_layout instance.

        For the dimension, you can pass either:
          - A single tuple (x, y) or (x, y, z) to specify only the "max" coordinate, with min defaulting to (0,0,0),
          - Two nested tuples ((xmin, ymin), (xmax, ymax)) or 3D
            ((xmin, ymin, zmin), (xmax, ymax, zmax)) to specify min and max explicitly.

        coordinate_type can be:
          - "offset": uses offset::ucoord_t (non-negative),
          - "cube":   uses cube::coord_t    (can be signed).
    )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_CARTESIAN_LAYOUT_HPP
