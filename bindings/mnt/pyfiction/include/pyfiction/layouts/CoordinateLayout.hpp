//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_COORDINATELAYOUT_HPP
#define PYFICTION_COORDINATELAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace pyfiction
{

namespace detail
{

class py_coordinate_layout
{
  public:
    using Coord = fiction::offset::ucoord_t;

    py_coordinate_layout(const std::string& topology)
    {
        initialize_layout(topology);
    }

    py_coordinate_layout(const std::string& topology, const Coord& dimension)
    {
        initialize_layout(topology, dimension);
    }

    auto coord(int64_t x, int64_t y, int64_t z = 0) const
    {
        return std::visit([&](const auto& lyt) { return lyt.coord(x, y, z); }, underlying_layout);
    }

    auto x() const
    {
        return std::visit([](const auto& lyt) { return lyt.x(); }, underlying_layout);
    }

    auto y() const
    {
        return std::visit([](const auto& lyt) { return lyt.y(); }, underlying_layout);
    }

    auto z() const
    {
        return std::visit([](const auto& lyt) { return lyt.z(); }, underlying_layout);
    }

    auto area() const
    {
        return std::visit([](const auto& lyt) { return lyt.area(); }, underlying_layout);
    }

    void resize(const Coord& dimension)
    {
        std::visit([&](auto& lyt) { lyt.resize(dimension); }, underlying_layout);
    }

    auto north(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.north(c); }, underlying_layout);
    }

    auto north_east(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.north_east(c); }, underlying_layout);
    }

    auto east(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.east(c); }, underlying_layout);
    }

    auto south_east(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.south_east(c); }, underlying_layout);
    }

    auto south(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.south(c); }, underlying_layout);
    }

    auto south_west(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.south_west(c); }, underlying_layout);
    }

    auto west(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.west(c); }, underlying_layout);
    }

    auto north_west(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.north_west(c); }, underlying_layout);
    }

    auto above(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.above(c); }, underlying_layout);
    }

    auto below(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.below(c); }, underlying_layout);
    }

    bool is_north_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_north_of(c1, c2); }, underlying_layout);
    }

    bool is_east_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_east_of(c1, c2); }, underlying_layout);
    }

    bool is_south_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_south_of(c1, c2); }, underlying_layout);
    }

    bool is_west_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_west_of(c1, c2); }, underlying_layout);
    }

    bool is_adjacent_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_adjacent_of(c1, c2); }, underlying_layout);
    }

    bool is_adjacent_elevation_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_adjacent_elevation_of(c1, c2); }, underlying_layout);
    }

    bool is_above(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_above(c1, c2); }, underlying_layout);
    }

    bool is_below(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_below(c1, c2); }, underlying_layout);
    }

    bool is_northwards_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_northwards_of(c1, c2); }, underlying_layout);
    }

    bool is_eastwards_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_eastwards_of(c1, c2); }, underlying_layout);
    }

    bool is_southwards_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_southwards_of(c1, c2); }, underlying_layout);
    }

    bool is_westwards_of(const Coord& c1, const Coord& c2) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_westwards_of(c1, c2); }, underlying_layout);
    }

    bool is_at_northern_border(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_at_northern_border(c); }, underlying_layout);
    }

    bool is_at_eastern_border(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_at_eastern_border(c); }, underlying_layout);
    }

    bool is_at_southern_border(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_at_southern_border(c); }, underlying_layout);
    }

    bool is_at_western_border(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_at_western_border(c); }, underlying_layout);
    }

    bool is_at_any_border(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_at_any_border(c); }, underlying_layout);
    }

    auto northern_border_of(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.northern_border_of(c); }, underlying_layout);
    }

    auto eastern_border_of(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.eastern_border_of(c); }, underlying_layout);
    }

    auto southern_border_of(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.southern_border_of(c); }, underlying_layout);
    }

    auto western_border_of(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.western_border_of(c); }, underlying_layout);
    }

    bool is_ground_layer(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_ground_layer(c); }, underlying_layout);
    }

    bool is_crossing_layer(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_crossing_layer(c); }, underlying_layout);
    }

    bool is_within_bounds(const Coord& c) const
    {
        return std::visit([&](const auto& lyt) { return lyt.is_within_bounds(c); }, underlying_layout);
    }

    auto coordinates() const
    {
        return std::visit(
            [](const auto& lyt)
            {
                std::vector<Coord> coords;
                coords.reserve(lyt.area() * (lyt.z() + 1));
                lyt.foreach_coordinate([&](const auto& c) { coords.push_back(c); });
                return coords;
            },
            underlying_layout);
    }

    auto ground_coordinates() const
    {
        return std::visit(
            [](const auto& lyt)
            {
                std::vector<Coord> coords;
                coords.reserve(lyt.area());
                lyt.foreach_ground_coordinate([&](const auto& c) { coords.push_back(c); });
                return coords;
            },
            underlying_layout);
    }

    auto adjacent_coordinates(const Coord& c) const
    {
        return std::visit(
            [&](const auto& lyt)
            {
                std::vector<Coord> adj_coords;
                lyt.foreach_adjacent_coordinate(c, [&](const auto& adj_c) { adj_coords.push_back(adj_c); });
                return adj_coords;
            },
            underlying_layout);
    }

    auto adjacent_opposite_coordinates(const Coord& c) const
    {
        return std::visit(
            [&](const auto& lyt)
            {
                std::vector<std::pair<Coord, Coord>> adj_opp_coords;
                lyt.foreach_adjacent_opposite_coordinates(c,
                                                          [&](const auto& adj_c) { adj_opp_coords.push_back(adj_c); });
                return adj_opp_coords;
            },
            underlying_layout);
    }

    std::string __repr__() const
    {
        return std::visit(
            [](const auto& lyt) -> std::string
            {
                std::stringstream stream{};
                print_layout(lyt, stream);
                return stream.str();
            },
            underlying_layout);
    }

  private:
    void initialize_layout(std::string topology)
    {
        std::transform(topology.begin(), topology.end(), topology.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (topology == "cartesian")
        {
            underlying_layout = py_cartesian_layout();
        }
        else if (topology == "shifted_cartesian")
        {
            underlying_layout = py_shifted_cartesian_layout();
        }
        else if (topology == "hexagonal")
        {
            underlying_layout = py_hexagonal_layout();
        }
        else
        {
            throw std::runtime_error(fmt::format("'{}' is not a supported topology", topology));
        }
    }

    void initialize_layout(std::string topology, const Coord& dimension)
    {
        std::transform(topology.begin(), topology.end(), topology.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (topology == "cartesian")
        {
            underlying_layout = py_cartesian_layout(dimension);
        }
        else if (topology == "shifted_cartesian")
        {
            underlying_layout = py_shifted_cartesian_layout(dimension);
        }
        else if (topology == "hexagonal")
        {
            underlying_layout = py_hexagonal_layout(dimension);
        }
        else
        {
            throw std::runtime_error(fmt::format("'{}' is not a supported topology", topology));
        }
    }

    std::variant<py_cartesian_layout, py_shifted_cartesian_layout, py_hexagonal_layout> underlying_layout{};
};

inline void coordinate_layout(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<py_coordinate_layout>(m, "CoordinateLayout", DOC(fiction_cartesian_layout_overridden))
        .def(py::init<const std::string&>(), py::arg("topology"))
        .def(py::init<const std::string&, const fiction::offset::ucoord_t&>(), py::arg("topology"),
             py::arg("dimension"))
        .def("coord", &py_coordinate_layout::coord, py::arg("x"), py::arg("y"), py::arg("z") = 0,
             DOC(fiction_cartesian_layout_coord))
        .def("x", &py_coordinate_layout::x, DOC(fiction_cartesian_layout_x))
        .def("y", &py_coordinate_layout::y, DOC(fiction_cartesian_layout_y))
        .def("z", &py_coordinate_layout::z, DOC(fiction_cartesian_layout_z))
        .def("area", &py_coordinate_layout::area, DOC(fiction_cartesian_layout_area))
        .def("resize", &py_coordinate_layout::resize, py::arg("dimension"), DOC(fiction_cartesian_layout_resize))
        .def("north", &py_coordinate_layout::north, py::arg("c"), DOC(fiction_cartesian_layout_north))
        .def("north_east", &py_coordinate_layout::north_east, py::arg("c"), DOC(fiction_cartesian_layout_north_east))
        .def("east", &py_coordinate_layout::east, py::arg("c"), DOC(fiction_cartesian_layout_east))
        .def("south_east", &py_coordinate_layout::south_east, py::arg("c"), DOC(fiction_cartesian_layout_south_east))
        .def("south", &py_coordinate_layout::south, py::arg("c"), DOC(fiction_cartesian_layout_south))
        .def("south_west", &py_coordinate_layout::south_west, py::arg("c"), DOC(fiction_cartesian_layout_south_west))
        .def("west", &py_coordinate_layout::west, py::arg("c"), DOC(fiction_cartesian_layout_west))
        .def("north_west", &py_coordinate_layout::north_west, py::arg("c"), DOC(fiction_cartesian_layout_north_west))
        .def("above", &py_coordinate_layout::above, py::arg("c"), DOC(fiction_cartesian_layout_above))
        .def("below", &py_coordinate_layout::below, py::arg("c"), DOC(fiction_cartesian_layout_below))
        .def("is_north_of", &py_coordinate_layout::is_north_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_north_of))
        .def("is_east_of", &py_coordinate_layout::is_east_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_east_of))
        .def("is_south_of", &py_coordinate_layout::is_south_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_south_of))
        .def("is_west_of", &py_coordinate_layout::is_west_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_west_of))
        .def("is_adjacent_of", &py_coordinate_layout::is_adjacent_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_adjacent_of))
        .def("is_adjacent_elevation_of", &py_coordinate_layout::is_adjacent_elevation_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_adjacent_elevation_of))
        .def("is_above", &py_coordinate_layout::is_above, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_above))
        .def("is_below", &py_coordinate_layout::is_below, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_below))
        .def("is_northwards_of", &py_coordinate_layout::is_northwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_northwards_of))
        .def("is_eastwards_of", &py_coordinate_layout::is_eastwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_eastwards_of))
        .def("is_southwards_of", &py_coordinate_layout::is_southwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_southwards_of))
        .def("is_westwards_of", &py_coordinate_layout::is_westwards_of, py::arg("c1"), py::arg("c2"),
             DOC(fiction_cartesian_layout_is_westwards_of))
        .def("is_at_northern_border", &py_coordinate_layout::is_at_northern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_northern_border))
        .def("is_at_eastern_border", &py_coordinate_layout::is_at_eastern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_eastern_border))
        .def("is_at_southern_border", &py_coordinate_layout::is_at_southern_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_southern_border))
        .def("is_at_western_border", &py_coordinate_layout::is_at_western_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_western_border))
        .def("is_at_any_border", &py_coordinate_layout::is_at_any_border, py::arg("c"),
             DOC(fiction_cartesian_layout_is_at_any_border))
        .def("northern_border_of", &py_coordinate_layout::northern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_northern_border_of))
        .def("eastern_border_of", &py_coordinate_layout::eastern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_eastern_border_of))
        .def("southern_border_of", &py_coordinate_layout::southern_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_southern_border_of))
        .def("western_border_of", &py_coordinate_layout::western_border_of, py::arg("c"),
             DOC(fiction_cartesian_layout_western_border_of))
        .def("is_ground_layer", &py_coordinate_layout::is_ground_layer, py::arg("c"),
             DOC(fiction_cartesian_layout_is_ground_layer))
        .def("is_crossing_layer", &py_coordinate_layout::is_crossing_layer, py::arg("c"),
             DOC(fiction_cartesian_layout_is_crossing_layer))
        .def("is_within_bounds", &py_coordinate_layout::is_within_bounds, py::arg("c"),
             DOC(fiction_cartesian_layout_is_within_bounds))
        .def("coordinates", &py_coordinate_layout::coordinates, DOC(fiction_cartesian_layout_coordinates))
        .def("ground_coordinates", &py_coordinate_layout::ground_coordinates,
             DOC(fiction_cartesian_layout_ground_coordinates))
        .def("adjacent_coordinates", &py_coordinate_layout::adjacent_coordinates, py::arg("c"),
             DOC(fiction_cartesian_layout_adjacent_coordinates))
        .def("adjacent_opposite_coordinates", &py_coordinate_layout::adjacent_opposite_coordinates, py::arg("c"),
             DOC(fiction_cartesian_layout_adjacent_opposite_coordinates))
        .def("__repr__", &py_coordinate_layout::__repr__);
}

}  // namespace detail

inline void coordinate_layout(pybind11::module& m)
{
    detail::coordinate_layout(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_COORDINATELAYOUT_HPP
