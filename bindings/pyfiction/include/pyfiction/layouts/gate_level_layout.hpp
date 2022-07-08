//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_GATE_LEVEL_LAYOUT_HPP
#define PYFICTION_GATE_LEVEL_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>
#include <fiction/algorithms/verification/design_rule_violations.hpp>
#include <fiction/algorithms/verification/equivalence_checking.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <set>
#include <sstream>
#include <string>
#include <utility>

namespace pyfiction
{

namespace detail
{

template <typename LytBase>
void gate_level_layout(pybind11::module& m, const std::string& topology)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using gate_layout = fiction::gate_level_layout<LytBase>;

    py::class_<gate_layout, LytBase>(m, fmt::format("{}_gate_layout", topology).c_str())
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<gate_layout>&>(), "dimension"_a)
        .def(py::init(
                 [](const fiction::aspect_ratio<gate_layout>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> gate_layout
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<gate_layout>(scheme_name); scheme.has_value())
                     {
                         return gate_layout{dimension, *scheme, layout_name};
                     }
                     else
                     {
                         throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                     }
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave", "layout_name"_a = "")

        .def("create_pi", &gate_layout::create_pi)
        .def("create_po", &gate_layout::create_po)
        .def("is_pi_tile", &gate_layout::is_pi_tile)
        .def("is_po_tile", &gate_layout::is_po_tile)

        .def("create_buf", &gate_layout::create_buf)
        .def("create_not", &gate_layout::create_not)
        .def("create_and", &gate_layout::create_and)
        .def("create_nand", &gate_layout::create_nand)
        .def("create_or", &gate_layout::create_or)
        .def("create_nor", &gate_layout::create_nor)
        .def("create_xor", &gate_layout::create_xor)
        .def("create_xnor", &gate_layout::create_xnor)
        .def("create_maj", &gate_layout::create_maj)

        .def("num_pis", &gate_layout::num_pis)
        .def("num_pos", &gate_layout::num_pos)
        .def("num_gates", &gate_layout::num_gates)
        .def("num_wires", &gate_layout::num_wires)
        .def("is_empty", &gate_layout::is_empty)

        .def("move_node", &gate_layout::move_node)
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

        .def("is_gate_tile", &gate_layout::is_gate_tile)
        .def("is_wire_tile", &gate_layout::is_wire_tile)
        .def("is_empty_tile", &gate_layout::is_empty_tile)

        .def("pis",
             [](const gate_layout& lyt)
             {
                 std::vector<fiction::tile<gate_layout>> pis{};
                 lyt.foreach_pi([&pis, &lyt](const auto& pi) { pis.push_back(lyt.get_tile(pi)); });
                 return pis;
             })
        .def("pos",
             [](const gate_layout& lyt)
             {
                 std::vector<fiction::tile<gate_layout>> pos{};
                 lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });
                 return pos;
             })
        .def("gates",
             [](const gate_layout& lyt)
             {
                 std::vector<fiction::tile<gate_layout>> gates{};
                 lyt.foreach_gate([&gates, &lyt](const auto& g) { gates.push_back(lyt.get_tile(g)); });
                 return gates;
             })
        .def("wires",
             [](const gate_layout& lyt)
             {
                 std::vector<fiction::tile<gate_layout>> wires{};
                 lyt.foreach_wire([&wires, &lyt](const auto& w) { wires.push_back(lyt.get_tile(w)); });
                 return wires;
             })

        .def("incoming_data_flow", &gate_layout::template incoming_data_flow<std::set<fiction::tile<gate_layout>>>)
        .def("outgoing_data_flow", &gate_layout::template outgoing_data_flow<std::set<fiction::tile<gate_layout>>>)

        .def("__repr__",
             [](const gate_layout& lyt) -> std::string
             {
                 std::stringstream stream{};
                 fiction::print_gate_level_layout(stream, lyt);
                 return stream.str();
             })

        ;

    /**
     * Algorithms defined on gate-level layouts.
     */
    m.def("critical_path_length_and_throughput",
          [](const gate_layout& lyt) -> std::pair<uint64_t, uint64_t>
          {
              fiction::critical_path_length_and_throughput_stats stats{};
              fiction::critical_path_length_and_throughput(lyt, &stats);

              return {stats.critical_path_length, stats.throughput};
          });

    m.def("gate_level_drvs", [](const gate_layout& lyt) { fiction::gate_level_drvs(lyt); });

    m.def("equivalence_checking",
          [](const fiction::tec_nt& spec, const gate_layout& impl) -> fiction::eq_type
          {
              fiction::equivalence_checking_stats stats{};
              fiction::equivalence_checking(spec, impl, &stats);

              return stats.eq;
          });
}

}  // namespace detail

inline void gate_level_layouts(pybind11::module& m)
{
    /**
     * Result type for equivalence checking.
     */
    pybind11::enum_<fiction::eq_type>(m, "eq_type")
        .value("NO", fiction::eq_type::NO)
        .value("WEAK", fiction::eq_type::WEAK)
        .value("STRONG", fiction::eq_type::STRONG)

        ;

    using gate_clk_cart_lyt_base =
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>;

    /**
     * Gate-level clocked Cartesian layout.
     */
    detail::gate_level_layout<gate_clk_cart_lyt_base>(m, "cartesian");

    using gate_clk_hex_lyt_base = fiction::clocked_layout<
        fiction::tile_based_layout<fiction::hexagonal_layout<fiction::offset::ucoord_t, fiction::even_row_hex>>>;

    /**
     * Gate-level clocked hexagonal layout.
     */
    detail::gate_level_layout<gate_clk_hex_lyt_base>(m, "hexagonal");
}

}  // namespace pyfiction

#endif  // PYFICTION_GATE_LEVEL_LAYOUT_HPP
