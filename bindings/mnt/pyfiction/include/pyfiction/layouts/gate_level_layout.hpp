//
// Created by marcel on 04.06.22.
//

#ifndef PYFICTION_GATE_LEVEL_LAYOUT_HPP
#define PYFICTION_GATE_LEVEL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>
#include <string>
#include <utility>

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename GateLyt>
void gate_level_layout(pybind11::module& m, const std::string& topology)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<GateLyt, LytBase>(m, fmt::format("{}_gate_layout", topology).c_str(), DOC(fiction_gate_level_layout))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<GateLyt>&>(), "dimension"_a,
             DOC(fiction_gate_level_layout_gate_level_layout))
        .def(py::init(
                 [](const fiction::aspect_ratio<GateLyt>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> GateLyt
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<GateLyt>(scheme_name); scheme.has_value())
                     {
                         return GateLyt{dimension, *scheme, layout_name};
                     }

                     throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave", "layout_name"_a = "",
             DOC(fiction_gate_level_layout_gate_level_layout_2))

        .def("create_pi", &GateLyt::create_pi, "name"_a = std::string{}, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_pi))
        .def("create_po", &GateLyt::create_po, "s"_a, "name"_a = std::string{}, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_po))
        .def("is_pi", &GateLyt::is_pi, "n"_a, DOC(fiction_gate_level_layout_is_pi))
        .def("is_po", &GateLyt::is_po, "n"_a, DOC(fiction_gate_level_layout_is_po))
        .def("is_pi_tile", &GateLyt::is_pi_tile, "t"_a, DOC(fiction_gate_level_layout_is_pi_tile))
        .def("is_po_tile", &GateLyt::is_po_tile, "t"_a, DOC(fiction_gate_level_layout_is_po_tile))

        .def("is_inv", &GateLyt::is_inv, DOC(fiction_gate_level_layout_is_inv))
        .def("is_and", &GateLyt::is_and, DOC(fiction_gate_level_layout_is_and))
        .def("is_nand", &GateLyt::is_nand, DOC(fiction_gate_level_layout_is_nand))
        .def("is_or", &GateLyt::is_or, DOC(fiction_gate_level_layout_is_or))
        .def("is_nor", &GateLyt::is_nor, DOC(fiction_gate_level_layout_is_nor))
        .def("is_xor", &GateLyt::is_xor, DOC(fiction_gate_level_layout_is_xor))
        .def("is_xnor", &GateLyt::is_xnor, DOC(fiction_gate_level_layout_is_xnor))
        .def("is_maj", &GateLyt::is_maj, DOC(fiction_gate_level_layout_is_maj))
        .def("is_fanout", &GateLyt::is_fanout, DOC(fiction_gate_level_layout_is_fanout))
        .def("is_wire", &GateLyt::is_wire, DOC(fiction_gate_level_layout_is_wire))

        .def("set_layout_name", &GateLyt::set_layout_name, "name"_a, DOC(fiction_gate_level_layout_set_layout_name))
        .def("get_layout_name", &GateLyt::get_layout_name, DOC(fiction_gate_level_layout_get_layout_name))
        .def("set_input_name", &GateLyt::set_input_name, "index"_a, "name"_a,
             DOC(fiction_gate_level_layout_set_input_name))
        .def("get_input_name", &GateLyt::get_input_name, "index"_a, DOC(fiction_gate_level_layout_get_input_name))
        .def("set_output_name", &GateLyt::set_output_name, "index"_a, "name"_a,
             DOC(fiction_gate_level_layout_set_output_name))
        .def("get_output_name", &GateLyt::get_output_name, "index"_a, DOC(fiction_gate_level_layout_get_output_name))
        .def(
            "get_name", [](const GateLyt& layout, const typename GateLyt::signal& signal)
            { return layout.get_name(signal); }, "s"_a, DOC(fiction_gate_level_layout_get_name))

        .def("create_buf", &GateLyt::create_buf, "a"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_buf))
        .def("create_not", &GateLyt::create_not, "a"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_not))
        .def("create_and", &GateLyt::create_and, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_and))
        .def("create_nand", &GateLyt::create_nand, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_nand))
        .def("create_or", &GateLyt::create_or, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_or))
        .def("create_nor", &GateLyt::create_nor, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_nor))
        .def("create_xor", &GateLyt::create_xor, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_xor))
        .def("create_xnor", &GateLyt::create_xnor, "a"_a, "b"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_xnor))
        .def("create_maj", &GateLyt::create_maj, "a"_a, "b"_a, "c"_a, "t"_a = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_maj))

        .def("num_pis", &GateLyt::num_pis, DOC(fiction_gate_level_layout_num_pis))
        .def("num_pos", &GateLyt::num_pos, DOC(fiction_gate_level_layout_num_pos))
        .def("num_gates", &GateLyt::num_gates, DOC(fiction_gate_level_layout_num_gates))
        .def("num_wires", &GateLyt::num_wires, DOC(fiction_gate_level_layout_num_wires))
        .def("num_crossings", &GateLyt::num_crossings, DOC(fiction_gate_level_layout_num_crossings))
        .def("is_empty", &GateLyt::is_empty, DOC(fiction_gate_level_layout_is_empty))

        .def(
            "fanin_size", [](const GateLyt& layout, const uint64_t node)
            { return layout.template fanin_size<true>(static_cast<typename GateLyt::node>(node)); }, "n"_a,
            DOC(fiction_gate_level_layout_fanin_size))

        .def(
            "fanout_size", [](const GateLyt& layout, const uint64_t node)
            { return layout.template fanout_size<true>(static_cast<typename GateLyt::node>(node)); }, "n"_a,
            DOC(fiction_gate_level_layout_fanout_size))

        .def(
            "get_node", [](const GateLyt& layout, const py_offset_coordinate& coordinate)
            { return layout.get_node(coordinate); }, "t"_a, DOC(fiction_gate_level_layout_get_node))
        .def("get_tile", &GateLyt::get_tile, "n"_a, DOC(fiction_gate_level_layout_get_tile))
        .def("make_signal", &GateLyt::make_signal, "n"_a, DOC(fiction_gate_level_layout_make_signal))

        .def("move_node", &GateLyt::move_node, "n"_a, "t"_a, "new_children"_a = std::vector<py_offset_coordinate>{},
             DOC(fiction_gate_level_layout_move_node))

        .def("clear_tile", &GateLyt::clear_tile, "t"_a, DOC(fiction_gate_level_layout_clear_tile))

        .def("is_gate_tile", &GateLyt::is_gate_tile, "t"_a, DOC(fiction_gate_level_layout_is_gate_tile))
        .def("is_wire_tile", &GateLyt::is_wire_tile, "t"_a, DOC(fiction_gate_level_layout_is_wire_tile))
        .def("is_empty_tile", &GateLyt::is_empty_tile, "t"_a, DOC(fiction_gate_level_layout_is_empty_tile))

        .def("pis",
             [](const GateLyt& lyt)
             {
                 std::vector<fiction::tile<GateLyt>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis, &lyt](const auto& pi) { pis.push_back(lyt.get_tile(pi)); });
                 return pis;
             })
        .def("pos",
             [](const GateLyt& lyt)
             {
                 std::vector<fiction::tile<GateLyt>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos, &lyt](const auto& po) { pos.push_back(lyt.get_tile(lyt.get_node(po))); });
                 return pos;
             })
        .def("gates",
             [](const GateLyt& lyt)
             {
                 std::vector<fiction::tile<GateLyt>> gates{};
                 gates.reserve(lyt.num_gates());
                 lyt.foreach_gate([&gates, &lyt](const auto& g) { gates.push_back(lyt.get_tile(g)); });
                 return gates;
             })
        .def("wires",
             [](const GateLyt& lyt)
             {
                 std::vector<fiction::tile<GateLyt>> wires{};
                 wires.reserve(lyt.num_wires());
                 lyt.foreach_wire([&wires, &lyt](const auto& w) { wires.push_back(lyt.get_tile(w)); });
                 return wires;
             })

        .def(
            "fanins", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template incoming_data_flow<true>(tile); }, "t"_a)

        .def(
            "fanouts", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template outgoing_data_flow<true>(tile); }, "t"_a)

        .def(
            "is_incoming_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile, const typename GateLyt::signal& signal)
            { return layout.template is_incoming_signal<true>(tile, signal); }, "t"_a, "s"_a,
            DOC(fiction_gate_level_layout_is_incoming_signal))

        .def(
            "has_no_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_no_incoming_signal))

        .def(
            "has_northern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_northern_incoming_signal))

        .def(
            "has_north_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_north_eastern_incoming_signal))

        .def(
            "has_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_eastern_incoming_signal))

        .def(
            "has_south_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_south_eastern_incoming_signal))

        .def(
            "has_southern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_southern_incoming_signal))

        .def(
            "has_south_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_south_western_incoming_signal))

        .def(
            "has_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_western_incoming_signal))

        .def(
            "has_north_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_incoming_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_north_western_incoming_signal))

        .def(
            "is_outgoing_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile, const typename GateLyt::signal& signal)
            { return layout.template is_outgoing_signal<true>(tile, signal); }, "t"_a, "s"_a,
            DOC(fiction_gate_level_layout_is_outgoing_signal))

        .def(
            "has_no_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_no_outgoing_signal))

        .def(
            "has_northern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_northern_outgoing_signal))

        .def(
            "has_north_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_north_eastern_outgoing_signal))

        .def(
            "has_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_eastern_outgoing_signal))

        .def(
            "has_south_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_south_eastern_outgoing_signal))

        .def(
            "has_southern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_southern_outgoing_signal))

        .def(
            "has_south_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_south_western_outgoing_signal))

        .def(
            "has_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_western_outgoing_signal))

        .def(
            "has_north_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_outgoing_signal<true>(tile); }, "t"_a,
            DOC(fiction_gate_level_layout_has_north_western_outgoing_signal))

        .def(
            "bounding_box_2d",
            [](const GateLyt& layout)
            {
                const auto bb = fiction::bounding_box_2d<GateLyt>(layout);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))
        .def("is_dead", &GateLyt::is_dead, "n"_a, DOC(fiction_gate_level_layout_is_dead))
        .def("__repr__",
             [](const GateLyt& lyt) -> std::string
             {
                 std::stringstream stream{};
                 fiction::print_layout(lyt, stream);
                 return stream.str();
             })

        ;
}

}  // namespace detail

inline void gate_level_layouts(pybind11::module& m)
{
    /**
     * Gate-level clocked Cartesian layout.
     */
    detail::gate_level_layout<py_cartesian_clocked_layout, py_cartesian_gate_layout>(m, "cartesian");
    /**
     * Gate-level clocked shifted Cartesian layout.
     */
    detail::gate_level_layout<py_shifted_cartesian_clocked_layout, py_shifted_cartesian_gate_layout>(
        m, "shifted_cartesian");
    /**
     * Gate-level clocked hexagonal layout.
     */
    detail::gate_level_layout<py_hexagonal_clocked_layout, py_hexagonal_gate_layout>(m, "hexagonal");
}

}  // namespace pyfiction

#endif  // PYFICTION_GATE_LEVEL_LAYOUT_HPP
