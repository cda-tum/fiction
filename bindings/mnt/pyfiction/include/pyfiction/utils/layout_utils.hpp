//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_LAYOUT_UTILS_HPP
#define PYFICTION_LAYOUT_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <pybind11/pybind11.h>

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

// template <typename Lyt>
// void convert_to_siqad_coordinates(pybind11::module& m)
// {
//     namespace py = pybind11;
//
//     m.def("convert_layout_to_siqad_coordinates", &fiction::convert_layout_to_siqad_coordinates<Lyt>, py::arg("lyt"),
//           DOC(fiction_convert_layout_to_siqad_coordinates));
// }

inline void random_coordinate(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("random_offset_coordinate", &fiction::random_coordinate<fiction::offset::ucoord_t>, py::arg("coordinate1"),
          py::arg("coordinate2"), DOC(fiction_random_coordinate));

    m.def("random_cube_coordinate", &fiction::random_coordinate<fiction::cube::coord_t>, py::arg("coordinate1"),
          py::arg("coordinate2"), DOC(fiction_random_coordinate));

    m.def("random_siqad_coordinate", &fiction::random_coordinate<fiction::siqad::coord_t>, py::arg("coordinate1"),
          py::arg("coordinate2"), DOC(fiction_random_coordinate));
}

}  // namespace detail

inline void layout_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::num_adjacent_coordinates<py_cartesian_layout>(m);
    detail::num_adjacent_coordinates<py_shifted_cartesian_layout>(m);
    detail::num_adjacent_coordinates<py_hexagonal_layout>(m);

    detail::normalize_layout_coordinates<py_qca_layout>(m);
    detail::normalize_layout_coordinates<py_inml_layout>(m);
    detail::normalize_layout_coordinates<py_sidb_layout>(m);

    // detail::convert_to_siqad_coordinates<py_sidb_layout>(m);

    detail::random_coordinate(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_LAYOUT_UTILS_HPP
