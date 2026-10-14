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
 * @brief Python bindings for the Graphviz DOT drawer of logic networks.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/io/dot_drawers.hpp>

#include <string>

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
 * @brief Binds write dot network helpers.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void register_write_dot_network(nb::module_& m)
{
    m.def(
        "write_dot_network",
        [](const Ntk& ntk, const std::string& filename, const bool indexes)
        {
            if (indexes)
            {
                fiction::networks::io::write_dot_network<Ntk, fiction::networks::io::technology_dot_drawer<Ntk, true>>(
                    ntk, filename);
            }
            else
            {
                fiction::networks::io::write_dot_network<Ntk, fiction::networks::io::technology_dot_drawer<Ntk, false>>(
                    ntk, filename);
            }
        },
        nb::arg("network"), nb::arg("filename"), nb::arg("indexes") = true, DOC(fiction_networks_io_write_dot_network),
        nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

/**
 * @brief Binds dot drawers helpers.
 * @param m Python module.
 */
void register_dot_drawers(nb::module_& m)
{
    detail::register_write_dot_network<py_tec_network>(m);
    detail::register_write_dot_network<py_aig_network>(m);
    detail::register_write_dot_network<py_xag_network>(m);
    detail::register_write_dot_network<py_mig_network>(m);
}

}  // namespace pyfiction
