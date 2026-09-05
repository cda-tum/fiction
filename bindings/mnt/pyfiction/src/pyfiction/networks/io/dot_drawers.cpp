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

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void write_dot_network(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_dot_network",
        [](const Ntk& ntk, const std::string& filename) { fiction::networks::io::write_dot_network(ntk, filename); },
        py::arg("network"), py::arg("filename"), DOC(fiction_networks_io_write_dot_network));
}

}  // namespace detail

void dot_drawers(nanobind::module_& m)
{
    detail::write_dot_network<py_logic_network>(m);
    detail::write_dot_network<py_aig_network>(m);
    detail::write_dot_network<py_xag_network>(m);
    detail::write_dot_network<py_mig_network>(m);
}

}  // namespace pyfiction
