//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_ENUMERATE_ALL_PATHS_HPP
#define PYFICTION_ENUMERATE_ALL_PATHS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/enumerate_all_paths.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void enumerate_all_paths(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "enumerate_all_paths",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const fiction::enumerate_all_paths_params& params)
        {
            const auto all_paths =
                fiction::enumerate_all_paths<fiction::layout_coordinate_path<Lyt>, Lyt>(lyt, {source, target}, params);

            std::vector<std::vector<fiction::coordinate<Lyt>>> paths{};
            paths.reserve(all_paths.size());

            for (const auto& path : all_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        py::arg("layout"), py::arg("source"), py::arg("target"),
        py::arg("params") = fiction::enumerate_all_paths_params{}, DOC(fiction_enumerate_all_paths));
}

}  // namespace detail

inline void enumerate_all_paths(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::enumerate_all_paths_params>(m, "enumerate_all_paths_params",
                                                    DOC(fiction_enumerate_all_paths_params))
        .def(py::init<>())
        .def_readwrite("crossings", &fiction::enumerate_all_paths_params::crossings,
                       DOC(fiction_enumerate_all_paths_params_crossings))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::enumerate_all_paths<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_clocked_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_cartesian_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_clocked_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_shifted_cartesian_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_obstruction_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_obstruction_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_gate_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_gate_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_clocked_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_clocked_layout<py_cube_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_layout<py_offset_coordinate>>(m);
    detail::enumerate_all_paths<py_hexagonal_layout<py_cube_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ENUMERATE_ALL_PATHS_HPP
