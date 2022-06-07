//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_GATE_LEVEL_LAYOUT_HPP
#define PYFICTION_GATE_LEVEL_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

namespace pyfiction
{

inline void gate_level_clocked_cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using gate_clk_cart_lyt = fiction::gate_level_layout<fiction::synchronization_element_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>;

    /**
     * Gate-level clocked Cartesian layout.
     */
    py::class_<gate_clk_cart_lyt,
               fiction::synchronization_element_layout<fiction::clocked_layout<
                   fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>(
        m, "cartesian_gate_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<gate_clk_cart_lyt>&>(), "dimension"_a)
        .def(py::init(
                 [](const fiction::aspect_ratio<gate_clk_cart_lyt>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> gate_clk_cart_lyt
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<gate_clk_cart_lyt>(scheme_name);
                         scheme.has_value())
                     {
                         return gate_clk_cart_lyt{dimension, *scheme, layout_name};
                     }
                     else
                     {
                         throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                     }
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave", "layout_name"_a = "")

        .def("create_pi", &gate_clk_cart_lyt::create_pi)
        .def("create_po", &gate_clk_cart_lyt::create_po)
        .def("is_pi_tile", &gate_clk_cart_lyt::is_pi_tile)
        .def("is_po_tile", &gate_clk_cart_lyt::is_po_tile)

        .def("create_buf", &gate_clk_cart_lyt::create_buf)
        .def("create_not", &gate_clk_cart_lyt::create_not)
        .def("create_and", &gate_clk_cart_lyt::create_and)
        .def("create_nand", &gate_clk_cart_lyt::create_nand)
        .def("create_or", &gate_clk_cart_lyt::create_or)
        .def("create_nor", &gate_clk_cart_lyt::create_nor)
        .def("create_xor", &gate_clk_cart_lyt::create_xor)
        .def("create_xnor", &gate_clk_cart_lyt::create_xnor)
        .def("create_maj", &gate_clk_cart_lyt::create_maj)

        .def("num_pis", &gate_clk_cart_lyt::num_pis)
        .def("num_pos", &gate_clk_cart_lyt::num_pos)
        .def("num_gates", &gate_clk_cart_lyt::num_gates)
        .def("num_wires", &gate_clk_cart_lyt::num_wires)
        .def("is_empty", &gate_clk_cart_lyt::is_empty)

        .def("move_node", &gate_clk_cart_lyt::move_node)
        //            .def("get_node", &gate_clk_cart_lyt::get_node)

        //        .def("is_gate", &gate_clk_cart_lyt::is_gate)
        //        .def("is_wire", &gate_clk_cart_lyt::is_wire)
        //        .def("is_fanout", &gate_clk_cart_lyt::is_fanout)
        //        .def("is_inv", &gate_clk_cart_lyt::is_inv)
        //        .def("is_and", &gate_clk_cart_lyt::is_and)
        //        .def("is_nand", &gate_clk_cart_lyt::is_nand)
        //        .def("is_or", &gate_clk_cart_lyt::is_or)
        //        .def("is_nor", &gate_clk_cart_lyt::is_nor)
        //        .def("is_xor", &gate_clk_cart_lyt::is_xor)
        //        .def("is_xnor", &gate_clk_cart_lyt::is_xnor)
        //        .def("is_maj", &gate_clk_cart_lyt::is_maj)

        .def("is_gate_tile", &gate_clk_cart_lyt::is_gate_tile)
        .def("is_wire_tile", &gate_clk_cart_lyt::is_wire_tile)
        .def("is_empty_tile", &gate_clk_cart_lyt::is_empty_tile)

        .def("pis",
             [](const gate_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::tile<gate_clk_cart_lyt>> pis{};
                 lyt.foreach_pi([&pis, &lyt](const auto& pi) { pis.push_back(lyt.get_tile(pi)); });
                 return pis;
             })
        .def("pos",
             [](const gate_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::tile<gate_clk_cart_lyt>> pos{};
                 lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });
                 return pos;
             })
        .def("gates",
             [](const gate_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::tile<gate_clk_cart_lyt>> gates{};
                 lyt.foreach_gate([&gates, &lyt](const auto& g) { gates.push_back(lyt.get_tile(g)); });
                 return gates;
             })
        .def("wires",
             [](const gate_clk_cart_lyt& lyt)
             {
                 std::vector<fiction::tile<gate_clk_cart_lyt>> wires{};
                 lyt.foreach_wire([&wires, &lyt](const auto& w) { wires.push_back(lyt.get_tile(w)); });
                 return wires;
             })

        .def("incoming_data_flow", &gate_clk_cart_lyt::incoming_data_flow<std::set<fiction::tile<gate_clk_cart_lyt>>>)
        .def("outgoing_data_flow", &gate_clk_cart_lyt::outgoing_data_flow<std::set<fiction::tile<gate_clk_cart_lyt>>>)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_GATE_LEVEL_LAYOUT_HPP
