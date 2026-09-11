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

#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/utils/atomic_write.hpp>

#include <mockturtle/io/write_aiger.hpp>
#include <mockturtle/io/write_blif.hpp>
#include <mockturtle/io/write_verilog.hpp>

#include <ostream>
#include <string>
#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

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
void network_writers(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_verilog",
        [](const Ntk& ntk, const std::string& filename)
        {
            // the readers only take the module named `top`, which mockturtle's writer replaces with the
            // network name unless told otherwise
            mockturtle::write_verilog_params params{};
            params.module_name = "top";

            if constexpr (std::is_same_v<Ntk, py_logic_network>)
            {
                // gate-level Verilog has no buffers or fan-out nodes, so a technology network is written as
                // an equivalent XAG
                fiction::detail::atomic_write(
                    filename,
                    [&](std::ostream& os)
                    {
                        mockturtle::write_verilog(fiction::synthesis::convert_network<py_xag_network>(ntk), os, params);
                    });
            }
            else
            {
                fiction::detail::atomic_write(filename,
                                              [&](std::ostream& os) { mockturtle::write_verilog(ntk, os, params); });
            }
        },
        py::arg("network"), py::arg("filename"),
        "Writes the network as a gate-level Verilog file whose module is named `top`, as the readers expect. A "
        "technology network is written as an equivalent XAG, because gate-level Verilog has no buffers or fan-out "
        "nodes. A file that cannot be written raises a `RuntimeError`.");

    m.def(
        "write_blif", [](const Ntk& ntk, const std::string& filename)
        { fiction::detail::atomic_write(filename, [&](std::ostream& os) { mockturtle::write_blif(ntk, os); }); },
        py::arg("network"), py::arg("filename"),
        "Writes the network as a BLIF file. A file that cannot be written raises a `RuntimeError`.");
}

}  // namespace detail

/**
 * @brief Register network writer overloads.
 * @param m Python module.
 */
void network_writers(nanobind::module_& m)
{
    namespace py = nanobind;

    detail::network_writers<py_logic_network>(m);
    detail::network_writers<py_aig_network>(m);
    detail::network_writers<py_xag_network>(m);
    detail::network_writers<py_mig_network>(m);

    // the AIGER format encodes AND gates and inverted edges only, so only an AIG can be written to it. The file
    // is binary: written through a text stream, Windows turns every newline byte of the gate deltas into two
    m.def(
        "write_aiger", [](const py_aig_network& ntk, const std::string& filename)
        { fiction::detail::atomic_write(filename, [&](std::ostream& os) { mockturtle::write_aiger(ntk, os); }); },
        py::arg("network"), py::arg("filename"),
        "Writes the AIG as a binary AIGER file, including its input, output, and network names. A file that "
        "cannot be written raises a `RuntimeError`.");
}

}  // namespace pyfiction
