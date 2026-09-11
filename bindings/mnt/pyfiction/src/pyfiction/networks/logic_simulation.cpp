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
 * @brief Python bindings for simulating a logic network into truth tables.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>

#include <fmt/format.h>
#include <kitty/bit_operations.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename NtkOrLyt>
void logic_simulation_impl(nanobind::module_& m, const std::string& type_name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    /**
     * Simulate outputs in declaration order, including repeated labels.
     */
    const auto outputs = [](const NtkOrLyt& ntk)
    {
        if (ntk.num_pis() >= 38u)
        {
            throw std::invalid_argument("simulation requires fewer than 38 inputs");
        }
        const auto tables = mockturtle::simulate<py_tt>(
            ntk, mockturtle::default_simulator<py_tt>{static_cast<unsigned>(ntk.num_pis())});
        std::vector<std::pair<std::string, std::vector<bool>>> result{};
        result.reserve(ntk.num_pos());
        ntk.foreach_po(
            [&](const auto&, const auto i)
            {
                const auto        name = ntk.has_output_name(i) ? ntk.get_output_name(i) : fmt::format("po{}", i);
                std::vector<bool> bits{};
                bits.reserve(tables[i].num_bits());
                for (auto bit = tables[i].num_bits(); bit != 0u; --bit)
                {
                    bits.push_back(kitty::get_bit(tables[i], bit - 1u));
                }
                result.emplace_back(name, std::move(bits));
            });
        return result;
    };
    m.def("simulate_outputs", outputs, py::arg(type_name.c_str()),
          "Return (name, bits) pairs in output declaration order, preserving duplicate labels. "
          "Truth-table storage grows exponentially with the input count; fewer than 38 inputs "
          "is a representation bound, not a memory guarantee.");
    m.def(
        "simulate",
        [outputs](const NtkOrLyt& ntk)
        {
            std::unordered_map<std::string, std::vector<bool>> result{};
            for (auto&& [name, bits] : outputs(ntk))
            {
                result[name] = std::move(bits);
            }
            return result;
        },
        py::arg(type_name.c_str()));
}

}  // namespace detail

void logic_simulation(nanobind::module_& m)
{
    detail::logic_simulation_impl<py_logic_network>(m, "network");
    detail::logic_simulation_impl<py_aig_network>(m, "network");
    detail::logic_simulation_impl<py_xag_network>(m, "network");
    detail::logic_simulation_impl<py_mig_network>(m, "network");
    detail::logic_simulation_impl<py_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_shifted_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_hexagonal_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_odd_row_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_even_row_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_even_column_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_odd_row_hex_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_odd_column_hex_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_even_column_hex_gate_layout>(m, "layout");
}

}  // namespace pyfiction
