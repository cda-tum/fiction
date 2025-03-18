//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_A_STAR_HPP
#define PYFICTION_A_STAR_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void a_star(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "a_star",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const fiction::a_star_params& params)
        {
            return static_cast<std::vector<fiction::coordinate<Lyt>>>(
                fiction::a_star<fiction::layout_coordinate_path<Lyt>, Lyt>(lyt, {source, target},
                                                                           fiction::manhattan_distance_functor<Lyt>(),
                                                                           fiction::unit_cost_functor<Lyt>(), params));
        },
        py::arg("layout"), py::arg("source"), py::arg("target"), py::arg("params") = fiction::a_star_params{},
        DOC(fiction_a_star));

    m.def("a_star_distance", &fiction::a_star_distance<Lyt, double>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_a_star_distance));
}

}  // namespace detail

inline void a_star(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::a_star_params>(m, "a_star_params", DOC(fiction_a_star_params))
        .def(py::init<>())
        .def_readwrite("crossings", &fiction::a_star_params::crossings, DOC(fiction_a_star_params_crossings))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::a_star<py_cartesian_obstruction_layout>(m);
    detail::a_star<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::a_star<py_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::a_star<py_cartesian_layout<py_offset_coordinate>>(m);
    detail::a_star<py_shifted_cartesian_obstruction_layout>(m);
    detail::a_star<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::a_star<py_shifted_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::a_star<py_shifted_cartesian_layout<py_offset_coordinate>>(m);
    detail::a_star<py_hexagonal_obstruction_layout>(m);
    detail::a_star<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
    detail::a_star<py_hexagonal_clocked_layout<py_offset_coordinate>>(m);
    detail::a_star<py_hexagonal_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_A_STAR_HPP
