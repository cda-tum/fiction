//
// Created by marcel on 09.02.23.
//

#ifndef PYFICTION_OBSTRUCTION_LAYOUT_HPP
#define PYFICTION_OBSTRUCTION_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstruction_layout.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename ObstrLyt>
void obstruction_layout(pybind11::module& m, const std::string& topology, const std::string& coord_type)
{
    namespace py = pybind11;

    py::class_<ObstrLyt, LytBase>(m, fmt::format("{}_obstruction_layout_{}", topology, coord_type).c_str(),
                                  DOC(fiction_obstruction_layout))
        .def(py::init<>())
        .def(py::init<const LytBase&>(), py::arg("layout"), DOC(fiction_obstruction_layout_obstruction_layout))

        .def("obstruct_coordinate", &ObstrLyt::obstruct_coordinate, py::arg("c"))
        .def("obstruct_connection", &ObstrLyt::obstruct_connection, py::arg("src"), py::arg("tgt"))
        .def("is_obstructed_coordinate", &ObstrLyt::is_obstructed_coordinate, py::arg("c"))
        .def("is_obstructed_connection", &ObstrLyt::is_obstructed_connection, py::arg("src"), py::arg("tgt"))
        .def("clear_obstructed_coordinate", &ObstrLyt::clear_obstructed_coordinate, py::arg("c"))
        .def("clear_obstructed_connection", &ObstrLyt::clear_obstructed_connection, py::arg("src"), py::arg("tgt"))
        .def("clear_obstructed_coordinates", &ObstrLyt::clear_obstructed_coordinates)
        .def("clear_obstructed_connections", &ObstrLyt::clear_obstructed_connections)

        ;
}

}  // namespace detail

inline void obstruction_layouts(pybind11::module& m)
{
    /**
     * Cartesian obstruction layout with offset coordinates.
     */
    detail::obstruction_layout<py_cartesian_gate_layout<py_offset_coordinate>,
                               py_cartesian_obstruction_layout<py_offset_coordinate>>(m, "cartesian",
                                                                                      "offset_coordinates");
    /**
     * Cartesian obstruction layout with cube coordinates.
     */
    detail::obstruction_layout<py_cartesian_gate_layout<py_cube_coordinate>,
                               py_cartesian_obstruction_layout<py_cube_coordinate>>(m, "cartesian", "cube_coordinates");
    /**
     * Shifted Cartesian obstruction layout with offset coordinates.
     */
    detail::obstruction_layout<py_shifted_cartesian_gate_layout<py_offset_coordinate>,
                               py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m, "shifted_cartesian",
                                                                                              "offset_coordinates");
    /**
     * Shifted Cartesian obstruction layout with cube coordinates.
     */
    detail::obstruction_layout<py_shifted_cartesian_gate_layout<py_cube_coordinate>,
                               py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m, "shifted_cartesian",
                                                                                            "cube_coordinates");
    /**
     * Hexagonal obstruction layout with offset coordinates.
     */
    detail::obstruction_layout<py_hexagonal_gate_layout<py_offset_coordinate>,
                               py_hexagonal_obstruction_layout<py_offset_coordinate>>(m, "hexagonal",
                                                                                      "offset_coordinates");
    /**
     * Hexagonal obstruction layout with cube coordinates.
     */
    detail::obstruction_layout<py_hexagonal_gate_layout<py_cube_coordinate>,
                               py_hexagonal_obstruction_layout<py_cube_coordinate>>(m, "hexagonal", "cube_coordinates");
}

inline void obstruction_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "obstruction_layout",
        [](py::object layout_obj)
        {
            // Try each known base type in turn. If a cast fails, we catch py::cast_error and move on.
            // Once a cast succeeds, build the corresponding obstruction_layout and return it.

            // cartesian + offset
            try
            {
                auto& lyt = layout_obj.cast<py_cartesian_gate_layout<py_offset_coordinate>&>();
                return py::cast(py_cartesian_obstruction_layout<py_offset_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // cartesian + cube
            try
            {
                auto& lyt = layout_obj.cast<py_cartesian_gate_layout<py_cube_coordinate>&>();
                return py::cast(py_cartesian_obstruction_layout<py_cube_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // shifted cart + offset
            try
            {
                auto& lyt = layout_obj.cast<py_shifted_cartesian_gate_layout<py_offset_coordinate>&>();
                return py::cast(py_shifted_cartesian_obstruction_layout<py_offset_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // shifted cart + cube
            try
            {
                auto& lyt = layout_obj.cast<py_shifted_cartesian_gate_layout<py_cube_coordinate>&>();
                return py::cast(py_shifted_cartesian_obstruction_layout<py_cube_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // hex + offset
            try
            {
                auto& lyt = layout_obj.cast<py_hexagonal_gate_layout<py_offset_coordinate>&>();
                return py::cast(py_hexagonal_obstruction_layout<py_offset_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // hex + cube
            try
            {
                auto& lyt = layout_obj.cast<py_hexagonal_gate_layout<py_cube_coordinate>&>();
                return py::cast(py_hexagonal_obstruction_layout<py_cube_coordinate>{lyt});
            }
            catch (const py::cast_error&)
            {}

            // If none matched, raise an error
            throw std::invalid_argument(
                "[pyfiction] The given layout object is not a recognized cartesian/hex/shifted layout "
                "in offset or cube coordinates.");
        },
        R"doc(
            Creates and returns an obstruction_layout by inferring the base layout type from
            the given `layout_obj`. For example, if you pass a `hexagonal_layout` with cube
            coordinates, you'll get a `hexagonal_obstruction_layout_cube_coordinates`. No extra
            parameters are needed.
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_OBSTRUCTION_LAYOUT_HPP
