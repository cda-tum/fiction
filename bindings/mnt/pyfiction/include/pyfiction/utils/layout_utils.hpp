//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_LAYOUT_UTILS_HPP
#define PYFICTION_LAYOUT_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/layout_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void num_adjacent_coordinates(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("num_adjacent_coordinates", &fiction::num_adjacent_coordinates<Lyt>, py::arg("lyt"), py::arg("c"),
          DOC(fiction_num_adjacent_coordinates));
}

template <typename Lyt>
void normalize_layout_coordinates(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("normalize_layout_coordinates", &fiction::normalize_layout_coordinates<Lyt>, py::arg("lyt"),
          DOC(fiction_normalize_layout_coordinates));
}

template <typename Lyt>
void convert_layout_to_siqad_coordinates(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("convert_layout_to_siqad_coordinates", &fiction::convert_layout_to_siqad_coordinates<Lyt>, py::arg("lyt"),
          DOC(fiction_convert_layout_to_siqad_coordinates));
}

template <typename Lyt>
void random_coordinate(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("random_coordinate", &fiction::random_coordinate<fiction::coordinate<Lyt>>, py::arg("coordinate1"),
          py::arg("coordinate_2"), DOC(fiction_random_coordinate));
}

}  // namespace detail

inline void layout_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::num_adjacent_coordinates<py_cartesian_obstruction_layout>(m);
    detail::num_adjacent_coordinates<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::num_adjacent_coordinates<py_shifted_cartesian_obstruction_layout>(m);
    detail::num_adjacent_coordinates<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::num_adjacent_coordinates<py_hexagonal_obstruction_layout>(m);
    detail::num_adjacent_coordinates<py_hexagonal_gate_layout<py_offset_coordinate>>(m);

    detail::normalize_layout_coordinates<py_qca_layout<py_offset_coordinate>>(m);
    detail::normalize_layout_coordinates<py_inml_layout<py_offset_coordinate>>(m);
    detail::normalize_layout_coordinates<py_sidb_layout<py_offset_coordinate>>(m);

    detail::convert_layout_to_siqad_coordinates<py_sidb_layout<py_offset_coordinate>>(m);

    detail::random_coordinate<py_cartesian_obstruction_layout>(m);
    detail::random_coordinate<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::random_coordinate<py_shifted_cartesian_obstruction_layout>(m);
    detail::random_coordinate<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::random_coordinate<py_hexagonal_obstruction_layout>(m);
    detail::random_coordinate<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
    detail::random_coordinate<py_qca_layout<py_offset_coordinate>>(m);
    detail::random_coordinate<py_inml_layout<py_offset_coordinate>>(m);
    detail::random_coordinate<py_sidb_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_LAYOUT_UTILS_HPP
