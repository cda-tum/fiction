//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_DISTANCE_HPP
#define PYFICTION_DISTANCE_HPP

#include "pybind11/pybind11.h"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/distance.hpp>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void distance(pybind11::module& m)
{
    m.def("manhattan_distance", &fiction::manhattan_distance<Lyt>);
    m.def("euclidean_distance", &fiction::euclidean_distance<Lyt>);
}

}  // namespace detail

inline void distance(pybind11::module& m)
{
    detail::distance<py_cartesian_layout>(m);
    detail::distance<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DISTANCE_HPP
