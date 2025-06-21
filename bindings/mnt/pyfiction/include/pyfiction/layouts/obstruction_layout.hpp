//
// Created by marcel on 09.02.23.
//

#ifndef PYFICTION_OBSTRUCTION_LAYOUT_HPP
#define PYFICTION_OBSTRUCTION_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/obstruction_layout.hpp>

#include <pybind11/pybind11.h>

#include <stdexcept>
#include <string>

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
    detail::obstruction_layout<py_shifted_cartesian_gate_layout, py_shifted_cartesian_obstruction_layout>(
        m, "shifted_cartesian", "offset_coordinates");
    /**
     * Hexagonal obstruction layout with offset coordinates.
     */
    detail::obstruction_layout<py_hexagonal_gate_layout, py_hexagonal_obstruction_layout>(m, "hexagonal",
                                                                                          "offset_coordinates");
}

inline void obstruction_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "obstruction_layout",
        [](py::object layout_obj)
        {
            // cartesian + offset
            if (py::isinstance<py_cartesian_gate_layout<py_offset_coordinate>>(layout_obj))
            {
                // Perform the safe cast
                auto& lyt = layout_obj.cast<py_cartesian_gate_layout<py_offset_coordinate>&>();
                return py::cast(py_cartesian_obstruction_layout<py_offset_coordinate>{lyt});
            }
            // cartesian + cube
            else if (py::isinstance<py_cartesian_gate_layout<py_cube_coordinate>>(layout_obj))
            {
                auto& lyt = layout_obj.cast<py_cartesian_gate_layout<py_cube_coordinate>&>();
                return py::cast(py_cartesian_obstruction_layout<py_cube_coordinate>{lyt});
            }
            // shifted cart + offset
            else if (py::isinstance<py_shifted_cartesian_gate_layout>(layout_obj))
            {
                auto& lyt = layout_obj.cast<py_shifted_cartesian_gate_layout&>();
                return py::cast(py_shifted_cartesian_obstruction_layout{lyt});
            }
            // hex + offset
            else if (py::isinstance<py_hexagonal_gate_layout>(layout_obj))
            {
                auto& lyt = layout_obj.cast<py_hexagonal_gate_layout&>();
                return py::cast(py_hexagonal_obstruction_layout{lyt});
            }
            // None matched → throw
            else
            {
                throw std::invalid_argument(
                    "[pyfiction] The given layout object is not recognized as a valid cartesian/hex/shifted layout "
                    "in offset or cube coordinates.");
            }
        },
        R"doc(
            Creates and returns an obstruction_layout from the given `layout_obj` by checking its type with
            `py::isinstance`. For example, if it's a `cartesian_gate_layout<py_cube_coordinate>`, we return a
            `cartesian_obstruction_layout_cube_coordinates`. If no recognized type is found, an exception is raised.

            Example usage (Python)::

                from pyfiction import cartesian_gate_layout, obstruction_layout
                lay = cartesian_gate_layout((10, 10))
                obs_lay = obstruction_layout(lay)
                obs_lay.obstruct_coordinate((5, 2))
                assert obs_lay.is_obstructed_coordinate((5, 2))
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_OBSTRUCTION_LAYOUT_HPP
