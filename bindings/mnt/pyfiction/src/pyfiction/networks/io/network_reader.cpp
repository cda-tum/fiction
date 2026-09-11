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
 * @brief Python bindings for reading logic networks from Verilog, AIGER, and BLIF files.
 * @author Marcel Walter (marcelwa)
 * @author OpenAI (Codex)
 */

#include "pyfiction/types.hpp"

#include <fiction/networks/io/network_reader.hpp>

#include <fmt/format.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network_reader(nanobind::module_& m, const char* function_name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        function_name,
        [](const std::string& filename, const std::string& format) -> Ntk
        {
            std::ostringstream diagnostics{};

            auto reader = fiction::networks::io::network_reader<std::shared_ptr<Ntk>>(filename, diagnostics, format);

            if (const auto ntks = reader.get_networks(); !ntks.empty())
            {
                auto ntk = *ntks.front();

                if constexpr (std::is_same_v<Ntk, py_logic_network>)
                {
                    ntk.substitute_po_signals();
                }

                return ntk;
            }

            throw std::runtime_error(fmt::format("could not parse '{}': {}", filename, diagnostics.str()));
        },
        py::arg("filename"), py::arg("format") = "",
        "Reads a logic network from a Verilog (`.v`), AIGER (`.aig`), or BLIF (`.blif`) file, or the first "
        "network from a directory of such files. Raises `RuntimeError` with the parser's diagnostics when no "
        "network can be read.");
}

}  // namespace detail

void network_reader(nanobind::module_& m)
{
    detail::network_reader<py_logic_network>(m, "read_technology_network");
    detail::network_reader<py_aig_network>(m, "read_aig_network");
    detail::network_reader<py_xag_network>(m, "read_xag_network");
    detail::network_reader<py_mig_network>(m, "read_mig_network");
}

}  // namespace pyfiction
