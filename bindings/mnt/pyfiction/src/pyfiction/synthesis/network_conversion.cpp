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
 * @brief Python bindings for converting logic networks into technology networks.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_conversion.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

namespace detail
{

template <typename NtkSrc>
void convert_network(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("convert_network", &fiction::synthesis::convert_network<py_logic_network, NtkSrc>, py::arg("network"),
          DOC(fiction_synthesis_convert_network));
}

}  // namespace detail

void network_conversion(nanobind::module_& m)
{
    detail::convert_network<py_logic_network>(m);
    detail::convert_network<py_aig_network>(m);
    detail::convert_network<py_xag_network>(m);
    detail::convert_network<py_mig_network>(m);
}

}  // namespace pyfiction
