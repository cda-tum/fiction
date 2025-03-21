//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP
#define PYFICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>

#include <pybind11/pybind11.h>

#include <utility>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void critical_path_length_and_throughput(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "critical_path_length_and_throughput",
        [](const Lyt& lyt) -> std::pair<uint64_t, uint64_t>
        {
            const auto result = fiction::critical_path_length_and_throughput(lyt);

            return {result.critical_path_length, result.throughput};
        },
        py::arg("layout"), DOC(fiction_critical_path_length_and_throughput));
}

}  // namespace detail

inline void critical_path_length_and_throughput(pybind11::module& m)
{
    detail::critical_path_length_and_throughput<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::critical_path_length_and_throughput<py_cartesian_gate_layout<py_cube_coordinate>>(m);
    detail::critical_path_length_and_throughput<py_shifted_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP
