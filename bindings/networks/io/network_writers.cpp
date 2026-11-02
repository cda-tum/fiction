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
 * @brief Python bindings for writing logic networks to Verilog, BLIF, and AIGER files.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/atomic_write.hpp>

#include <mockturtle/io/write_aiger.hpp>
#include <mockturtle/io/write_blif.hpp>
#include <mockturtle/io/write_verilog.hpp>

#include <ostream>
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
 * @brief Register transactional network writers for one network type.
 * @tparam Ntk Source network type.
 * @param m Python module.
 */
template <typename Ntk>
void register_network_writers(nb::module_& m)
{
    m.def(
        "write_verilog",
        [](const Ntk& ntk, const std::string& filename)
        {
            // the readers only take the module named `top`, which mockturtle's writer replaces with the
            // network name unless told otherwise
            const mockturtle::write_verilog_params params{.module_name = "top", .input_names = {}, .output_names = {}};

            if constexpr (std::is_same_v<Ntk, fiction::networks::named_technology_network>)
            {
                // gate-level Verilog has no buffers or fan-out nodes, so a technology network is written as
                // an equivalent XAG
                fiction::detail::atomic_write(
                    filename,
                    [&](std::ostream& os)
                    {
                        mockturtle::write_verilog(
                            fiction::synthesis::convert_network<fiction::networks::named_xag_network>(ntk), os, params);
                    });
            }
            else
            {
                fiction::detail::atomic_write(filename,
                                              [&](std::ostream& os) { mockturtle::write_verilog(ntk, os, params); });
            }
        },
        nb::arg("network"), nb::arg("filename"),
        "Writes the network as a gate-level Verilog file whose module is named `top`, as the readers expect. A "
        "technology network is written as an equivalent XAG, because gate-level Verilog has no buffers or fan-out "
        "nodes. A file that cannot be written raises a `RuntimeError`.",
        nb::call_guard<nb::gil_scoped_release>());

    m.def(
        "write_blif", [](const Ntk& ntk, const std::string& filename)
        { fiction::detail::atomic_write(filename, [&](std::ostream& os) { mockturtle::write_blif(ntk, os); }); },
        nb::arg("network"), nb::arg("filename"),
        "Writes the network as a BLIF file. A file that cannot be written raises a `RuntimeError`.",
        nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

/**
 * @brief Register network writer overloads.
 * @param m Python module.
 */
void register_network_writers(nb::module_& m)
{
    detail::register_network_writers<fiction::networks::named_technology_network>(m);
    detail::register_network_writers<fiction::networks::named_aig_network>(m);
    detail::register_network_writers<fiction::networks::named_xag_network>(m);
    detail::register_network_writers<fiction::networks::named_mig_network>(m);

    // AIGER encodes AND gates and inverted edges; other network types require conversion to an AIG.
    m.def(
        "write_aiger", [](const fiction::networks::named_aig_network& ntk, const std::string& filename)
        { fiction::detail::atomic_write(filename, [&](std::ostream& os) { mockturtle::write_aiger(ntk, os); }); },
        nb::arg("network"), nb::arg("filename"),
        "Writes the AIG as a binary AIGER file, including its input, output, and network names. A file that "
        "cannot be written raises a `RuntimeError`.",
        nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace pyfiction
