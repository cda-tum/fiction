//
// Created by marcel on 13.02.23.
//

#ifndef PYFICTION_ROUTING_UTILS_HPP
#define PYFICTION_ROUTING_UTILS_HPP

#include "pyfiction/docs.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/routing_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void route_path(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "route_path",
        [](Lyt& lyt, const std::vector<fiction::coordinate<Lyt>>& path)
        {
            fiction::layout_coordinate_path<Lyt> converted_path{};
            converted_path.resize(path.size());
            converted_path.assign(path.cbegin(), path.cend());

            fiction::route_path(lyt, converted_path);
        },
        "layout"_a, "path"_a, DOC(fiction_route_path));
}

}  // namespace detail

inline void routing_utils(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::route_path<py_cartesian_obstruction_layout>(m);
    detail::route_path<py_cartesian_gate_layout>(m);
    detail::route_path<py_hexagonal_obstruction_layout>(m);
    detail::route_path<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ROUTING_UTILS_HPP
