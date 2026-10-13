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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Bind a reader that preserves the parsed network's output signals.
 * @tparam Ntk Network type to read.
 * @param m Python module.
 * @param signature Python signature including the selected return type.
 */
template <typename Ntk>
void register_network_reader(nb::module_& m, const char* signature)
{
    const auto read = [](const std::string& filename, const std::string& format, const nb::handle network_type) -> Ntk
    {
        if (!network_type.is(nb::type<Ntk>()))
        {
            throw nb::next_overload();
        }
        const nb::gil_scoped_release release{};
        std::ostringstream           diagnostics{};

        auto reader = fiction::networks::io::network_reader<std::shared_ptr<Ntk>>(filename, diagnostics, format);

        if (const auto ntks = reader.get_networks(); !ntks.empty())
        {
            return *ntks.front();
        }

        throw std::runtime_error(fmt::format("could not parse '{}': {}", filename, diagnostics.str()));
    };
    if constexpr (std::is_same_v<Ntk, py_tec_network>)
    {
        m.def("read_network", read, nb::arg("filename"), nb::arg("format") = "", nb::kw_only(),
              nb::arg("network_type") = nb::type<Ntk>(), nb::sig(signature),
              "Read Verilog, AIGER, or BLIF, or the first network from a directory. "
              "Select a registered network class with network_type (technology_network by default). "
              "Preserve output signals without adding output buffers. Raise RuntimeError with parser diagnostics "
              "when no network can be read. BLIF supports technology_network only.");
    }
    else
    {
        m.def("read_network", read, nb::arg("filename"), nb::arg("format") = "", nb::kw_only(), nb::arg("network_type"),
              nb::sig(signature));
    }
}

}  // namespace detail

/**
 * @brief Register readers for the supported logic network types.
 * @param m Python module.
 */
void register_network_reader(nb::module_& m)
{
    detail::register_network_reader<py_tec_network>(
        m, "def read_network(filename: str, format: str = \"\", *, network_type: "
           "type[mnt.pyfiction.networks.technology_network] = mnt.pyfiction.networks.technology_network) -> "
           "mnt.pyfiction.networks.technology_network");
    detail::register_network_reader<py_aig_network>(
        m, "def read_network(filename: str, format: str = \"\", *, network_type: "
           "type[mnt.pyfiction.networks.aig_network]) -> mnt.pyfiction.networks.aig_network");
    detail::register_network_reader<py_xag_network>(
        m, "def read_network(filename: str, format: str = \"\", *, network_type: "
           "type[mnt.pyfiction.networks.xag_network]) -> mnt.pyfiction.networks.xag_network");
    detail::register_network_reader<py_mig_network>(
        m, "def read_network(filename: str, format: str = \"\", *, network_type: "
           "type[mnt.pyfiction.networks.mig_network]) -> mnt.pyfiction.networks.mig_network");
    m.def(
        "read_network", [](const std::string&, const std::string&, const nb::handle) -> nb::object
        { throw nb::type_error("network_type must be a registered network class"); }, nb::arg("filename"),
        nb::arg("format") = "", nb::kw_only(), nb::arg("network_type"),
        nb::sig("def read_network(filename: str, format: str = \"\", *, network_type: "
                "type[mnt.pyfiction.networks.technology_network | mnt.pyfiction.networks.aig_network | "
                "mnt.pyfiction.networks.xag_network | mnt.pyfiction.networks.mig_network]) -> "
                "mnt.pyfiction.networks.technology_network | mnt.pyfiction.networks.aig_network | "
                "mnt.pyfiction.networks.xag_network | mnt.pyfiction.networks.mig_network"));
}

}  // namespace pyfiction
