//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_K_SHORTEST_PATHS_HPP
#define PYFICTION_K_SHORTEST_PATHS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/k_shortest_paths.hpp>
#include <fiction/traits.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void yen_k_shortest_paths(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "yen_k_shortest_paths",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const uint32_t k, const fiction::yen_k_shortest_paths_params& params)
        {
            const auto k_paths = fiction::yen_k_shortest_paths<fiction::layout_coordinate_path<Lyt>, Lyt>(
                lyt, {source, target}, k, params);

            std::vector<std::vector<fiction::coordinate<Lyt>>> paths{};
            paths.reserve(k_paths.size());

            for (const auto& path : k_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        py::arg("layout"), py::arg("source"), py::arg("target"), py::arg("k"),
        py::arg("params") = fiction::yen_k_shortest_paths_params{}, DOC(fiction_yen_k_shortest_paths));
}

}  // namespace detail

inline void yen_k_shortest_paths(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::yen_k_shortest_paths_params>(m, "yen_k_shortest_paths_params",
                                                     DOC(fiction_yen_k_shortest_paths_params))
        .def(py::init<>())
        .def_readwrite("a_star_params", &fiction::yen_k_shortest_paths_params::astar_params,
                       DOC(fiction_yen_k_shortest_paths_params_astar_params))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::yen_k_shortest_paths<py_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_clocked_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_cartesian_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_obstruction_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_obstruction_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_clocked_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_clocked_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_layout<py_offset_coordinate>>(m);
    detail::yen_k_shortest_paths<py_shifted_cartesian_layout<py_cube_coordinate>>(m);
    detail::yen_k_shortest_paths<py_hexagonal_obstruction_layout>(m);
    detail::yen_k_shortest_paths<py_hexagonal_gate_layout>(m);
    detail::yen_k_shortest_paths<py_hexagonal_clocked_layout>(m);
    detail::yen_k_shortest_paths<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_K_SHORTEST_PATHS_HPP
