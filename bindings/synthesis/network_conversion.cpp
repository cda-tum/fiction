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
 * @brief Python bindings for converting logic networks between AIG, XAG, MIG, and technology networks.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_conversion.hpp>

#include <cstdint>
#include <utility>

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
    /**
     * @brief Technology network.
     */
    TEC,
    /**
     * @brief AND-inverter graph.
     */
    AIG,
    /**
     * @brief XOR-AND-inverter graph.
     */
    XAG,
    /**
     * @brief Majority-inverter graph.
     */
    MIG
};

/**
 * @brief Register conversions from one source network type.
 * @tparam NtkSrc Source network type.
 * @param m Python module.
 */
template <typename NtkSrc>
void convert_network(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "convert_network",
        [](const NtkSrc& ntk, const network_target target) -> py::object
        {
            switch (target)
            {
                case network_target::AIG:
                {
                    auto converted = [&]()
                    {
                        const py::gil_scoped_release release{};
                        return fiction::synthesis::convert_network<py_aig_network, NtkSrc>(ntk);
                    }();
                    return py::cast(std::move(converted));
                }
                case network_target::XAG:
                {
                    auto converted = [&]()
                    {
                        const py::gil_scoped_release release{};
                        return fiction::synthesis::convert_network<py_xag_network, NtkSrc>(ntk);
                    }();
                    return py::cast(std::move(converted));
                }
                case network_target::MIG:
                {
                    auto converted = [&]()
                    {
                        const py::gil_scoped_release release{};
                        return fiction::synthesis::convert_network<py_mig_network, NtkSrc>(ntk);
                    }();
                    return py::cast(std::move(converted));
                }
                default:
                {
                    auto converted = [&]()
                    {
                        const py::gil_scoped_release release{};
                        return fiction::synthesis::convert_network<py_tec_network, NtkSrc>(ntk);
                    }();
                    return py::cast(std::move(converted));
                }
            }
        },
        py::arg("network"), py::arg("target") = network_target::TEC, DOC(fiction_synthesis_convert_network));
}

}  // namespace detail

/**
 * @brief Register conversion targets and network overloads.
 * @param m Python module.
 */
void network_conversion(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<detail::network_target>(m, "network_target", "The network types `convert_network` produces.")
        .value("TEC", detail::network_target::TEC, "A technology network.")
        .value("AIG", detail::network_target::AIG, "An AND-inverter graph.")
        .value("XAG", detail::network_target::XAG, "An XOR-AND-inverter graph.")
        .value("MIG", detail::network_target::MIG, "A majority-inverter graph.");

    detail::convert_network<py_tec_network>(m);
    detail::convert_network<py_aig_network>(m);
    detail::convert_network<py_xag_network>(m);
    detail::convert_network<py_mig_network>(m);
}

}  // namespace pyfiction
