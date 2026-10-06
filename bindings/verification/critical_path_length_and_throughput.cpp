/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/verification/critical_path_length_and_throughput.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/verification/critical_path_length_and_throughput.hpp>

#include <cstdint>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>  // NOLINT(misc-include-cleaner): enables return-value conversion

namespace pyfiction
{

namespace detail
{

/** @brief Binds iterative physical timing analysis. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void critical_path_length_and_throughput_impl(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "critical_path_length_and_throughput",
        [](const Lyt& lyt) -> std::pair<uint64_t, uint64_t>
        {
            const auto result = fiction::verification::critical_path_length_and_throughput(lyt);

            return {result.critical_path_length, result.throughput};
        },
        py::arg("layout"), py::call_guard<py::gil_scoped_release>(),
        DOC(fiction_verification_critical_path_length_and_throughput));
}

}  // namespace detail

/** @brief Registers layout timing analysis. @param m Python module. */
void critical_path_length_and_throughput(nanobind::module_& m)
{
    detail::critical_path_length_and_throughput_impl<py_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_shifted_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
