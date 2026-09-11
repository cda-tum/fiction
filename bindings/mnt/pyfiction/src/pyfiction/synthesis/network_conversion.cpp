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

#include <cstdint>

#include <nanobind/nanobind.h>

namespace pyfiction
{

namespace detail
{

/**
 * The network types `convert_network` produces, under the names its `target` argument takes.
 */
enum class network_target : std::uint8_t
{
    TEC,
    AIG,
    XAG,
    MIG
};

template <typename NtkSrc>
void convert_network(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "convert_network",
        [](const NtkSrc& ntk, const network_target target) -> py::object
        {
            switch (target)
            {
                case network_target::AIG:
                    return py::cast(fiction::synthesis::convert_network<py_aig_network, NtkSrc>(ntk));
                case network_target::XAG:
                    return py::cast(fiction::synthesis::convert_network<py_xag_network, NtkSrc>(ntk));
                case network_target::MIG:
                    return py::cast(fiction::synthesis::convert_network<py_mig_network, NtkSrc>(ntk));
                default: return py::cast(fiction::synthesis::convert_network<py_logic_network, NtkSrc>(ntk));
            }
        },
        py::arg("network"), py::arg("target") = network_target::TEC, DOC(fiction_synthesis_convert_network));
}

}  // namespace detail

void network_conversion(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<detail::network_target>(m, "network_target", "The network types `convert_network` produces.")
        .value("TEC", detail::network_target::TEC, "A technology network.")
        .value("AIG", detail::network_target::AIG, "An AND-inverter graph.")
        .value("XAG", detail::network_target::XAG, "An XOR-AND-inverter graph.")
        .value("MIG", detail::network_target::MIG, "A majority-inverter graph.");

    detail::convert_network<py_logic_network>(m);
    detail::convert_network<py_aig_network>(m);
    detail::convert_network<py_xag_network>(m);
    detail::convert_network<py_mig_network>(m);
}

}  // namespace pyfiction
