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
#include <type_traits>
#include <utility>
#include <variant>

#include <nanobind/nanobind.h>
#include <nanobind/stl/variant.h>  // NOLINT(misc-include-cleaner): converts the source network

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
 * @brief A network of any type that `convert_network` accepts.
 */
using any_network =
    std::variant<const py_tec_network*, const py_aig_network*, const py_xag_network*, const py_mig_network*>;

/**
 * @brief Converts a network into `NtkDest` while the GIL is released.
 *
 * @tparam NtkDest Target network type.
 * @param network Network to convert.
 * @return The converted network as a Python object.
 */
template <typename NtkDest>
nanobind::object convert_to(const any_network& network)
{
    return std::visit(
        [](const auto* ntk)
        {
            auto converted = [ntk]()
            {
                const nanobind::gil_scoped_release release{};
                return fiction::synthesis::convert_network<NtkDest, std::remove_cvref_t<decltype(*ntk)>>(*ntk);
            }();
            return nanobind::cast(std::move(converted));
        },
        network);
}

/**
 * @brief Converts a network into the network type that `target` names.
 *
 * @param network Network to convert.
 * @param target Target network type.
 * @return The converted network as a Python object.
 */
inline nanobind::object convert_to_target(const any_network& network, const network_target target)
{
    switch (target)
    {
        case network_target::AIG: return convert_to<py_aig_network>(network);
        case network_target::XAG: return convert_to<py_xag_network>(network);
        case network_target::MIG: return convert_to<py_mig_network>(network);
        default: return convert_to<py_tec_network>(network);
    }
}

/**
 * @brief Registers the `convert_network` overload whose `target` is `Target`.
 *
 * The overload hands every other target on to the next overload. Its signature names the result type
 * that `Target` selects, so a type checker infers the result of a call with a literal target.
 *
 * @tparam Target Target network type of the overload.
 * @param m Python module.
 * @param signature Python signature of the overload.
 */
template <network_target Target>
void convert_network_to(nanobind::module_& m, const char* signature)
{
    namespace py = nanobind;

    m.def(
        "convert_network",
        [](const any_network& network, const network_target target) -> py::object
        {
            if (target != Target)
            {
                throw py::next_overload();
            }

            return convert_to_target(network, target);
        },
        py::arg("network"), py::arg("target"), py::sig(signature));
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

    m.def(
        "convert_network",
        [](const detail::any_network& network, const detail::network_target target) -> py::object
        {
            if (target != detail::network_target::TEC)
            {
                throw py::next_overload();
            }

            return detail::convert_to_target(network, target);
        },
        py::arg("network"), py::arg("target") = detail::network_target::TEC,
        py::sig(
            "def convert_network(network: mnt.pyfiction.networks.technology_network | "
            "mnt.pyfiction.networks.aig_network | mnt.pyfiction.networks.xag_network | "
            "mnt.pyfiction.networks.mig_network, target: typing.Literal[mnt.pyfiction.synthesis.network_target.TEC] = "
            "mnt.pyfiction.synthesis.network_target.TEC) "
            "-> mnt.pyfiction.networks.technology_network"),
        DOC(fiction_synthesis_convert_network));
    detail::convert_network_to<detail::network_target::AIG>(
        m, "def convert_network(network: mnt.pyfiction.networks.technology_network | "
           "mnt.pyfiction.networks.aig_network | mnt.pyfiction.networks.xag_network | "
           "mnt.pyfiction.networks.mig_network, target: typing.Literal[mnt.pyfiction.synthesis.network_target.AIG]) "
           "-> mnt.pyfiction.networks.aig_network");
    detail::convert_network_to<detail::network_target::XAG>(
        m, "def convert_network(network: mnt.pyfiction.networks.technology_network | "
           "mnt.pyfiction.networks.aig_network | mnt.pyfiction.networks.xag_network | "
           "mnt.pyfiction.networks.mig_network, target: typing.Literal[mnt.pyfiction.synthesis.network_target.XAG]) "
           "-> mnt.pyfiction.networks.xag_network");
    detail::convert_network_to<detail::network_target::MIG>(
        m, "def convert_network(network: mnt.pyfiction.networks.technology_network | "
           "mnt.pyfiction.networks.aig_network | mnt.pyfiction.networks.xag_network | "
           "mnt.pyfiction.networks.mig_network, target: typing.Literal[mnt.pyfiction.synthesis.network_target.MIG]) "
           "-> mnt.pyfiction.networks.mig_network");
    // a target that is not a literal
    m.def("convert_network", &detail::convert_to_target, py::arg("network"), py::arg("target"),
          py::sig("def convert_network(network: mnt.pyfiction.networks.technology_network | "
                  "mnt.pyfiction.networks.aig_network | mnt.pyfiction.networks.xag_network | "
                  "mnt.pyfiction.networks.mig_network, target: mnt.pyfiction.synthesis.network_target) -> "
                  "mnt.pyfiction.networks.technology_network | mnt.pyfiction.networks.aig_network | "
                  "mnt.pyfiction.networks.xag_network | mnt.pyfiction.networks.mig_network"));
}

}  // namespace pyfiction
