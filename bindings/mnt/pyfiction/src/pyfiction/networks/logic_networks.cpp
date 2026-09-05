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
 * @brief Python bindings for the technology, AIG, XAG, and MIG network types.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/traits.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/views/depth_view.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Binds one network type. Node-type queries are bound only where the network implements them, so an AIG exposes
 * `is_and` but not `is_maj`. Output lookups by index (`po_index`, `po_at`) exist on the technology network alone,
 * whose signals are nodes; the mockturtle networks answer them with signals, which have no Python type.
 */
template <typename Ntk>
void network(nanobind::module_& m, const char* network_name, const char* doc)
{
    namespace py = nanobind;

    // Network node: mockturtle::node<Ntk> is a plain uint64_t, which nanobind already represents
    // as a Python int via its built-in integer type caster -- no separate class binding needed
    // (and nanobind disallows binding a type that already has a built-in caster).

    auto cls = py::class_<Ntk>(m, network_name, doc);

    cls.def(py::init<>(), "Default constructor.")

        .def("size", &Ntk::size)
        .def("num_gates", &Ntk::num_gates)
        .def("num_pis", &Ntk::num_pis)
        .def("num_pos", &Ntk::num_pos)
        .def(
            "depth", [](const Ntk& ntk) { return mockturtle::depth_view<Ntk>{ntk}.depth(); },
            "Returns the number of gates on the longest input-to-output path.")

        .def("nodes",
             [](const Ntk& ntk)
             {
                 std::vector<mockturtle::node<Ntk>> nodes{};
                 nodes.reserve(ntk.size());
                 ntk.foreach_node([&nodes](const auto& n) { nodes.push_back(n); });
                 return nodes;
             })
        .def("gates",
             [](const Ntk& ntk)
             {
                 std::vector<mockturtle::node<Ntk>> gates{};
                 gates.reserve(ntk.num_gates());
                 ntk.foreach_gate([&gates](const auto& g) { gates.push_back(g); });
                 return gates;
             })
        .def("pis",
             [](const Ntk& ntk)
             {
                 std::vector<mockturtle::node<Ntk>> pis{};
                 pis.reserve(ntk.num_pis());
                 ntk.foreach_pi([&pis](const auto& pi) { pis.push_back(pi); });
                 return pis;
             })
        .def("pos",
             [](const Ntk& ntk)
             {
                 std::vector<mockturtle::node<Ntk>> pos{};
                 pos.reserve(ntk.num_pos());
                 ntk.foreach_po([&pos, &ntk](const auto& po) { pos.push_back(ntk.get_node(po)); });
                 return pos;
             })

        .def(
            "fanins",
            [](const Ntk& ntk, const mockturtle::node<Ntk>& n)
            {
                std::vector<mockturtle::node<Ntk>> fanins{};
                fanins.reserve(ntk.fanin_size(n));
                ntk.foreach_fanin(n, [&fanins, &ntk](const auto& f) { fanins.push_back(ntk.get_node(f)); });
                return fanins;
            },
            py::arg("n"))

        .def("is_constant", &Ntk::is_constant, py::arg("n"))
        .def("is_pi", &Ntk::is_pi, py::arg("n"))

        .def(
            "has_name", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.has_name(ntk.make_signal(n)); },
            py::arg("n"))
        .def(
            "get_name", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.get_name(ntk.make_signal(n)); },
            py::arg("n"))
        .def(
            "set_name", [](Ntk& ntk, const mockturtle::node<Ntk>& n, const std::string& name)
            { ntk.set_name(ntk.make_signal(n), name); }, py::arg("n"), py::arg("name"))
        .def(
            "has_output_name", [](const Ntk& ntk, const uint32_t index) { return ntk.has_output_name(index); },
            py::arg("index"))
        .def(
            "get_output_name", [](const Ntk& ntk, const uint32_t index) { return ntk.get_output_name(index); },
            py::arg("index"))
        .def(
            "set_output_name", [](Ntk& ntk, const uint32_t index, const std::string& name)
            { ntk.set_output_name(index, name); }, py::arg("index"), py::arg("name"));

    // the is_* functions are wrapped in lambdas so that the bound overload is the one of Ntk itself
    if constexpr (fiction::has_is_po_v<Ntk>)
    {
        cls.def("is_po", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_po(n); }, py::arg("n"));
    }
    if constexpr (fiction::has_is_buf_v<Ntk>)
    {
        cls.def(
            "is_buf", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_buf(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_buf));
    }
    if constexpr (fiction::has_is_fanout_v<Ntk>)
    {
        cls.def(
            "is_fanout", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_fanout(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_fanout));
    }
    if constexpr (fiction::has_is_inv_v<Ntk>)
    {
        cls.def(
            "is_inv", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_inv(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_inv));
    }
    if constexpr (mockturtle::has_is_and_v<Ntk>)
    {
        cls.def(
            "is_and", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_and(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_and));
    }
    if constexpr (mockturtle::has_is_or_v<Ntk>)
    {
        cls.def(
            "is_or", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_or(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_or));
    }
    if constexpr (mockturtle::has_is_xor_v<Ntk>)
    {
        cls.def(
            "is_xor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_xor(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_xor));
    }
    if constexpr (mockturtle::has_is_maj_v<Ntk>)
    {
        cls.def(
            "is_maj", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_maj(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_maj));
    }
    if constexpr (fiction::has_is_nand_v<Ntk>)
    {
        cls.def(
            "is_nand", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_nand(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_nand));
    }
    if constexpr (fiction::has_is_nor_v<Ntk>)
    {
        cls.def(
            "is_nor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_nor(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_nor));
    }
    if constexpr (fiction::has_is_xnor_v<Ntk>)
    {
        cls.def(
            "is_xnor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_xnor(n); }, py::arg("n"),
            DOC(fiction_networks_technology_network_is_xnor));
    }
    if constexpr (std::is_same_v<Ntk, py_logic_network>)
    {
        cls.def(
               "po_index", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.po_index(n); }, py::arg("n"))
            .def("po_at", [](const Ntk& ntk, const uint32_t index) { return ntk.po_at(index); }, py::arg("index"));
    }
}

}  // namespace detail

void logic_networks(nanobind::module_& m)
{
    detail::network<py_logic_network>(m, "technology_network", DOC(fiction_networks_technology_network));
    detail::network<py_aig_network>(m, "aig_network", "An AND-inverter graph (AIG) with node and output names.");
    detail::network<py_xag_network>(m, "xag_network", "An XOR-AND-inverter graph (XAG) with node and output names.");
    detail::network<py_mig_network>(m, "mig_network", "A majority-inverter graph (MIG) with node and output names.");
}

}  // namespace pyfiction
