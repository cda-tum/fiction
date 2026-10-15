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

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/verification/critical_path_length_and_throughput.hpp>

#include <cstdint>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>  // NOLINT(misc-include-cleaner): enables return-value conversion

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/** @brief Binds iterative physical timing analysis. @tparam Lyt Layout type. @param m Python module. */
template <typename Lyt>
void register_critical_path_length_and_throughput_impl(nb::module_& m)
{
    m.def(
        "critical_path_length_and_throughput",
        [](const Lyt& lyt) -> std::pair<uint64_t, uint64_t>
        {
            const auto result = fiction::verification::critical_path_length_and_throughput(lyt);

            return {result.critical_path_length, result.throughput};
        },
        nb::arg("layout"), nb::call_guard<nb::gil_scoped_release>(),
        DOC(fiction_verification_critical_path_length_and_throughput));
}

}  // namespace detail

/** @brief Registers layout timing analysis. @param m Python module. */
void register_critical_path_length_and_throughput(nb::module_& m)
{
    detail::register_critical_path_length_and_throughput_impl<fiction::layouts::cartesian_gate_layout>(m);
    detail::register_critical_path_length_and_throughput_impl<fiction::layouts::shifted_cartesian_gate_layout>(m);
    detail::register_critical_path_length_and_throughput_impl<fiction::layouts::hexagonal_gate_layout>(m);
}

}  // namespace pyfiction
