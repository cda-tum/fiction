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
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>

#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <cassert>
#include <iostream>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
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

    m.def(
        "simulate",
        [](const NtkOrLyt& ntk_or_lyt) -> std::unordered_map<std::string, std::vector<bool>>
        {
            /**
             * Stores truth tables.
             */
            std::vector<py_tt> tables;
            tables.reserve(ntk_or_lyt.num_pos());
            /**
             * Stores PO names in order.
             */
            std::vector<std::string> po_names;
            po_names.reserve(ntk_or_lyt.num_pos());

            const auto store_po_names = [&po_names](const NtkOrLyt& network_or_layout)
            {
                network_or_layout.foreach_po(
                    [&network_or_layout, &po_names]([[maybe_unused]] const auto& po, auto i)
                    {
                        po_names.emplace_back(network_or_layout.has_output_name(i) ?
                                                  network_or_layout.get_output_name(i) :
                                                  fmt::format("po{}", i));
                    });
            };

            const auto simulate = [&tables](const NtkOrLyt& network_or_layout)
            {
                tables = mockturtle::simulate<py_tt>(
                    network_or_layout,
                    // NOLINTNEXTLINE
                    mockturtle::default_simulator<py_tt>{static_cast<unsigned>(network_or_layout.num_pis())});
            };

            store_po_names(ntk_or_lyt);

            try
            {
                simulate(ntk_or_lyt);
            }
            catch (const std::bad_alloc&)
            {
                std::cout << "[e] " << fiction::networks::get_name(ntk_or_lyt)
                          << " has too many inputs to store its truth table" << std::endl;
                throw;
            }

            std::unordered_map<std::string, std::vector<bool>> tts{};
            assert(tables.size() == po_names.size() && "Number of POs and truth tables must be equal");

            for (auto i = 0ul; i < tables.size(); ++i)
            {
                const auto binary_repr = kitty::to_binary(tables[i]);

                std::vector<bool> binary{};
                binary.reserve(binary_repr.size());

                for (const auto& c : binary_repr)
                {
                    binary.emplace_back(c == '1');
                }

                tts[po_names[i]] = binary;
            }

            return tts;
        },
        py::arg(type_name.c_str()));
}

}  // namespace detail

void logic_simulation(nanobind::module_& m)
{
    detail::logic_simulation_impl<py_logic_network>(m, "network");
    detail::logic_simulation_impl<py_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_shifted_cartesian_gate_layout>(m, "layout");
    detail::logic_simulation_impl<py_hexagonal_gate_layout>(m, "layout");
}

}  // namespace pyfiction
