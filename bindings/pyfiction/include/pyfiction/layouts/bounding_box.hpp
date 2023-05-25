//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_BOUNDING_BOX_HPP
#define PYFICTION_BOUNDING_BOX_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <functional>
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename GateLyt>
void bounding_box_2d(pybind11::module& m, const std::string& layout_type)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Bounding Box.
     */
    py::class_<LytBase>(m, fmt::format("{}_bounding_box_2d", layout_type).c_str())
        .def(py::init<const GateLyt&>(), "layout"_a)
        .def("update_bounding_box", &LytBase::update_bounding_box)
        .def("get_min", &LytBase::get_min)
        .def("get_max", &LytBase::get_max)
        .def("get_x_size", &LytBase::get_x_size)
        .def("get_y_size", &LytBase::get_y_size)

        ;
}

}  // namespace detail

inline void bounding_boxes_2d(pybind11::module& m)
{
    /**
     * Bounding box.
     */
    detail::bounding_box_2d<py_cartesian_gate_layout_bounding_box, py_cartesian_gate_layout>(m, "cartesian_gate_layout");
    detail::bounding_box_2d<py_shifted_cartesian_gate_layout_bounding_box, py_shifted_cartesian_gate_layout>(m, "shifted_cartesian_gate_layout");
    detail::bounding_box_2d<py_hexagonal_gate_layout_bounding_box, py_hexagonal_gate_layout>(m, "hexagonal_gate_layout");
    detail::bounding_box_2d<py_cartesian_obstruction_layout_bounding_box, py_cartesian_obstruction_layout>(m, "cartesian_obstruction_layout");
    detail::bounding_box_2d<py_shifted_cartesian_obstruction_layout_bounding_box, py_shifted_cartesian_obstruction_layout>(m, "shifted_cartesian_obstruction_layout");
    detail::bounding_box_2d<py_hexagonal_obstruction_layout_bounding_box, py_hexagonal_obstruction_layout>(m, "hexagonal_obstruction_layout");
    detail::bounding_box_2d<py_qca_layout_bounding_box, py_qca_layout>(m, "qca_layout");
    detail::bounding_box_2d<py_inml_layout_bounding_box, py_inml_layout>(m, "inml_layout");
    detail::bounding_box_2d<py_sidb_layout_bounding_box, py_sidb_layout>(m, "sidb_layout");
}

}  // namespace pyfiction

#endif  // PYFICTION_BOUNDING_BOX_HPP
