//
// Created by marcel on 07.06.22.
//

#ifndef PYFICTION_LOGIC_NETWORKS_HPP
#define PYFICTION_LOGIC_NETWORKS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/network_reader.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

// #include <mockturtle/networks/aig.hpp>
// #include <mockturtle/networks/mig.hpp>
// #include <mockturtle/networks/xag.hpp>

#include <functional>
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void network(pybind11::module& m, const std::string& network_name)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Network node.
     */
    py::class_<mockturtle::node<Ntk>>(m, fmt::format("{}_node", network_name).c_str())
        .def("__hash__", [](const mockturtle::node<Ntk>& n) { return std::hash<mockturtle::node<Ntk>>{}(n); })

        ;

    py::implicitly_convertible<py::int_, mockturtle::node<Ntk>>();

    /**
     * Network.
     */
    py::class_<Ntk>(m, network_name.c_str(), DOC(fiction_technology_network))
        .def(py::init<>())

        .def("size", &Ntk::size)
        .def("num_gates", &Ntk::num_gates)
        .def("num_pis", &Ntk::num_pis)
        .def("num_pos", &Ntk::num_pos)

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
                 ntk.foreach_po([&pos](const auto& po) { pos.push_back(po); });
                 return pos;
             })

        .def(
            "fanins",
            [](const Ntk& ntk, const mockturtle::node<Ntk>& n)
            {
                std::vector<mockturtle::node<Ntk>> fanins{};
                fanins.reserve(ntk.fanin_size(n));
                ntk.foreach_fanin(n, [&fanins](const auto& f) { fanins.push_back(f); });
                return fanins;
            },
            "n"_a)

        .def("is_constant", &Ntk::is_constant, "n"_a)
        .def("is_pi", &Ntk::is_pi, "n"_a)
        .def("is_po", &Ntk::is_po, "n"_a)

        // for some reason, the is_* functions need redefinition to match with Ntk
        .def(
            "is_buf", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_buf(n); }, "n"_a,
            DOC(fiction_technology_network_is_buf))
        .def(
            "is_fanout", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_fanout(n); }, "n"_a,
            DOC(fiction_technology_network_is_fanout))
        .def(
            "is_inv", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_inv(n); }, "n"_a,
            DOC(fiction_technology_network_is_inv))
        .def(
            "is_and", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_and(n); }, "n"_a,
            DOC(fiction_technology_network_is_and))
        .def(
            "is_or", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_or(n); }, "n"_a,
            DOC(fiction_technology_network_is_or))
        .def(
            "is_xor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_xor(n); }, "n"_a,
            DOC(fiction_technology_network_is_xor))
        .def(
            "is_maj", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_maj(n); }, "n"_a,
            DOC(fiction_technology_network_is_maj))
        .def(
            "is_nand", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_nand(n); }, "n"_a,
            DOC(fiction_technology_network_is_nand))
        .def(
            "is_nor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_nor(n); }, "n"_a,
            DOC(fiction_technology_network_is_nor))
        .def(
            "is_xnor", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.is_xnor(n); }, "n"_a,
            DOC(fiction_technology_network_is_xnor))
        .def(
            "has_name", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.has_name(n); }, "n"_a)
        .def(
            "get_name", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.get_name(n); }, "n"_a)
        .def(
            "has_output_name", [](const Ntk& ntk, const uint32_t index) { return ntk.has_output_name(index); },
            "index"_a)
        .def(
            "get_output_name", [](const Ntk& ntk, const uint32_t index) { return ntk.get_output_name(index); },
            "index"_a)
        .def(
            "po_index", [](const Ntk& ntk, const mockturtle::node<Ntk>& n) { return ntk.po_index(n); }, "n"_a)
        .def(
            "po_at", [](const Ntk& ntk, const uint32_t index) { return ntk.po_at(index); }, "index"_a);

    /**
     * Network parsing function.
     */
    m.def(
        fmt::format("read_{}", network_name).c_str(),
        [](const std::string& filename) -> Ntk
        {
            auto reader = fiction::network_reader<std::shared_ptr<Ntk>>(filename, std::cout);

            if (const auto ntks = reader.get_networks(); !ntks.empty())
            {
                auto ntk = *ntks.front();
                ntk.substitute_po_signals();
                return ntk;
            }

            throw std::runtime_error("Could not parse specification file");
        },
        "filename"_a);
}

}  // namespace detail

inline void logic_networks(pybind11::module& m)
{
    /**
     * Logic networks.
     */
    detail::network<py_logic_network>(m, "technology_network");
    //    detail::network<fiction::aig_nt>(m, "aig_network");
    //    detail::network<fiction::mig_nt>(m, "mig_network");
    //    detail::network<fiction::xag_nt>(m, "xag_network");
}

}  // namespace pyfiction

#endif  // PYFICTION_TECHNOLOGY_NETWORKS_HPP
