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
void gate_level_layout(pybind11::module& m, const std::string& topology, const std::string& coord_type)
{
    namespace py = pybind11;

    py::class_<GateLyt, LytBase>(m, fmt::format("{}_gate_layout_{}", topology, coord_type).c_str(),
                                 DOC(fiction_gate_level_layout))
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<fiction::coordinate<GateLyt>>&>(), py::arg("dimension"),
             DOC(fiction_gate_level_layout_gate_level_layout))
        .def(py::init(
                 [](const fiction::aspect_ratio<fiction::coordinate<GateLyt>>& dimension, const std::string& scheme_name,
                    const std::string& layout_name) -> GateLyt
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<GateLyt>(scheme_name); scheme.has_value())
                     {
                         return GateLyt{dimension, *scheme, layout_name};
                     }

                     throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                 }),
             py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
             DOC(fiction_gate_level_layout_gate_level_layout_2))

        .def("create_pi", &GateLyt::create_pi, py::arg("name") = std::string{}, py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_pi))
        .def("create_po", &GateLyt::create_po, py::arg("s"), py::arg("name") = std::string{},
             py::arg("t") = fiction::tile<GateLyt>{}, DOC(fiction_gate_level_layout_create_po))
        .def("is_pi", &GateLyt::is_pi, py::arg("n"), DOC(fiction_gate_level_layout_is_pi))
        .def("is_po", &GateLyt::is_po, py::arg("n"), DOC(fiction_gate_level_layout_is_po))
        .def("is_pi_tile", &GateLyt::is_pi_tile, py::arg("t"), DOC(fiction_gate_level_layout_is_pi_tile))
        .def("is_po_tile", &GateLyt::is_po_tile, py::arg("t"), DOC(fiction_gate_level_layout_is_po_tile))

        .def("is_inv", &GateLyt::is_inv, DOC(fiction_gate_level_layout_is_inv))
        .def("is_and", &GateLyt::is_and, DOC(fiction_gate_level_layout_is_and))
        .def("is_nand", &GateLyt::is_nand, DOC(fiction_gate_level_layout_is_nand))
        .def("is_or", &GateLyt::is_or, DOC(fiction_gate_level_layout_is_or))
        .def("is_nor", &GateLyt::is_nor, DOC(fiction_gate_level_layout_is_nor))
        .def("is_xor", &GateLyt::is_xor, DOC(fiction_gate_level_layout_is_xor))
        .def("is_xnor", &GateLyt::is_xnor, DOC(fiction_gate_level_layout_is_xnor))
        .def("is_lt", &GateLyt::is_lt, DOC(fiction_gate_level_layout_is_lt))
        .def("is_le", &GateLyt::is_le, DOC(fiction_gate_level_layout_is_le))
        .def("is_gt", &GateLyt::is_gt, DOC(fiction_gate_level_layout_is_gt))
        .def("is_ge", &GateLyt::is_ge, DOC(fiction_gate_level_layout_is_ge))
        .def("is_maj", &GateLyt::is_maj, DOC(fiction_gate_level_layout_is_maj))
        .def("is_fanout", &GateLyt::is_fanout, DOC(fiction_gate_level_layout_is_fanout))
        .def("is_wire", &GateLyt::is_wire, DOC(fiction_gate_level_layout_is_wire))

        .def("set_layout_name", &GateLyt::set_layout_name, py::arg("name"),
             DOC(fiction_gate_level_layout_set_layout_name))
        .def("get_layout_name", &GateLyt::get_layout_name, DOC(fiction_gate_level_layout_get_layout_name))
        .def("set_input_name", &GateLyt::set_input_name, py::arg("index"), py::arg("name"),
             DOC(fiction_gate_level_layout_set_input_name))
        .def("get_input_name", &GateLyt::get_input_name, py::arg("index"),
             DOC(fiction_gate_level_layout_get_input_name))
        .def("set_output_name", &GateLyt::set_output_name, py::arg("index"), py::arg("name"),
             DOC(fiction_gate_level_layout_set_output_name))
        .def("get_output_name", &GateLyt::get_output_name, py::arg("index"),
             DOC(fiction_gate_level_layout_get_output_name))
        .def(
            "get_name", [](const GateLyt& layout, const typename GateLyt::signal& signal)
            { return layout.get_name(signal); }, py::arg("s"), DOC(fiction_gate_level_layout_get_name))

        .def("create_buf", &GateLyt::create_buf, py::arg("a"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_buf))
        .def("create_not", &GateLyt::create_not, py::arg("a"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_not))
        .def("create_and", &GateLyt::create_and, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_and))
        .def("create_nand", &GateLyt::create_nand, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_nand))
        .def("create_or", &GateLyt::create_or, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_or))
        .def("create_nor", &GateLyt::create_nor, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_nor))
        .def("create_xor", &GateLyt::create_xor, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_xor))
        .def("create_xnor", &GateLyt::create_xnor, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_xnor))
        .def("create_lt", &GateLyt::create_lt, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_lt))
        .def("create_le", &GateLyt::create_le, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_le))
        .def("create_gt", &GateLyt::create_gt, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_gt))
        .def("create_ge", &GateLyt::create_ge, py::arg("a"), py::arg("b"), py::arg("t") = fiction::tile<GateLyt>{},
             DOC(fiction_gate_level_layout_create_ge))
        .def("create_maj", &GateLyt::create_maj, py::arg("a"), py::arg("b"), py::arg("c"),
             py::arg("t") = fiction::tile<GateLyt>{}, DOC(fiction_gate_level_layout_create_maj))

        .def("num_pis", &GateLyt::num_pis, DOC(fiction_gate_level_layout_num_pis))
        .def("num_pos", &GateLyt::num_pos, DOC(fiction_gate_level_layout_num_pos))
        .def("num_gates", &GateLyt::num_gates, DOC(fiction_gate_level_layout_num_gates))
        .def("num_wires", &GateLyt::num_wires, DOC(fiction_gate_level_layout_num_wires))
        .def("num_crossings", &GateLyt::num_crossings, DOC(fiction_gate_level_layout_num_crossings))
        .def("is_empty", &GateLyt::is_empty, DOC(fiction_gate_level_layout_is_empty))

        .def(
            "fanin_size", [](const GateLyt& layout, const uint64_t node)
            { return layout.template fanin_size<true>(static_cast<typename GateLyt::node>(node)); }, py::arg("n"),
            DOC(fiction_gate_level_layout_fanin_size))

        .def(
            "fanout_size", [](const GateLyt& layout, const uint64_t node)
            { return layout.template fanout_size<true>(static_cast<typename GateLyt::node>(node)); }, py::arg("n"),
            DOC(fiction_gate_level_layout_fanout_size))

        .def(
            "get_node", [](const GateLyt& layout, const typename LytBase::coordinate& coordinate)
            { return layout.get_node(coordinate); }, py::arg("t"), DOC(fiction_gate_level_layout_get_node))
        .def("get_tile", &GateLyt::get_tile, py::arg("n"), DOC(fiction_gate_level_layout_get_tile))
        .def("make_signal", &GateLyt::make_signal, py::arg("n"), DOC(fiction_gate_level_layout_make_signal))

        .def("move_node", &GateLyt::move_node, py::arg("n"), py::arg("t"),
             py::arg("new_children") = std::vector<typename LytBase::coordinate>{},
             DOC(fiction_gate_level_layout_move_node))

        .def("clear_tile", &GateLyt::clear_tile, py::arg("t"), DOC(fiction_gate_level_layout_clear_tile))

        .def("is_gate_tile", &GateLyt::is_gate_tile, py::arg("t"), DOC(fiction_gate_level_layout_is_gate_tile))
        .def("is_wire_tile", &GateLyt::is_wire_tile, py::arg("t"), DOC(fiction_gate_level_layout_is_wire_tile))
        .def("is_empty_tile", &GateLyt::is_empty_tile, py::arg("t"), DOC(fiction_gate_level_layout_is_empty_tile))

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
            { return layout.template incoming_data_flow<true>(tile); }, py::arg("t"))

        .def(
            "fanouts", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template outgoing_data_flow<true>(tile); }, py::arg("t"))

        .def(
            "is_incoming_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile, const typename GateLyt::signal& signal)
            { return layout.template is_incoming_signal<true>(tile, signal); }, py::arg("t"), py::arg("s"),
            DOC(fiction_gate_level_layout_is_incoming_signal))

        .def(
            "has_no_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_no_incoming_signal))

        .def(
            "has_northern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_northern_incoming_signal))

        .def(
            "has_north_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_north_eastern_incoming_signal))

        .def(
            "has_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_eastern_incoming_signal))

        .def(
            "has_south_eastern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_south_eastern_incoming_signal))

        .def(
            "has_southern_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_southern_incoming_signal))

        .def(
            "has_south_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_south_western_incoming_signal))

        .def(
            "has_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_western_incoming_signal))

        .def(
            "has_north_western_incoming_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_incoming_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_north_western_incoming_signal))

        .def(
            "is_outgoing_signal",
            [](const GateLyt& layout, const fiction::tile<GateLyt>& tile, const typename GateLyt::signal& signal)
            { return layout.template is_outgoing_signal<true>(tile, signal); }, py::arg("t"), py::arg("s"),
            DOC(fiction_gate_level_layout_is_outgoing_signal))

        .def(
            "has_no_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_no_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_no_outgoing_signal))

        .def(
            "has_northern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_northern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_northern_outgoing_signal))

        .def(
            "has_north_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_north_eastern_outgoing_signal))

        .def(
            "has_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_eastern_outgoing_signal))

        .def(
            "has_south_eastern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_eastern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_south_eastern_outgoing_signal))

        .def(
            "has_southern_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_southern_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_southern_outgoing_signal))

        .def(
            "has_south_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_south_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_south_western_outgoing_signal))

        .def(
            "has_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_western_outgoing_signal))

        .def(
            "has_north_western_outgoing_signal", [](const GateLyt& layout, const fiction::tile<GateLyt>& tile)
            { return layout.template has_north_western_outgoing_signal<true>(tile); }, py::arg("t"),
            DOC(fiction_gate_level_layout_has_north_western_outgoing_signal))

        .def(
            "bounding_box_2d",
            [](const GateLyt& layout)
            {
                const auto bb = fiction::bounding_box_2d<GateLyt>(layout);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))
        .def("is_dead", &GateLyt::is_dead, py::arg("n"), DOC(fiction_gate_level_layout_is_dead))
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
    detail::gate_level_layout<py_cartesian_clocked_layout<py_offset_coordinate>,
                              py_cartesian_gate_layout<py_offset_coordinate>>(m, "cartesian", "offset_coordinates");
    detail::gate_level_layout<py_cartesian_clocked_layout<py_cube_coordinate>,
                              py_cartesian_gate_layout<py_cube_coordinate>>(m, "cartesian", "cube_coordinates");
    /**
     * Gate-level clocked shifted Cartesian layout.
     */
    detail::gate_level_layout<py_shifted_cartesian_clocked_layout, py_shifted_cartesian_gate_layout>(
        m, "shifted_cartesian", "offset_coordinates");
    /**
     * Gate-level clocked hexagonal layout.
     */
    detail::gate_level_layout<py_hexagonal_clocked_layout, py_hexagonal_gate_layout>(m, "hexagonal",
                                                                                     "offset_coordinates");
}
/**
 * A "factory" function that Python users can call as
 *   <cartesian|shifted_cartesian|hexagonal>_gate_layout(dimension, scheme_name="open", coordinate_type="offset")
 * to create the correct layout type (offset or cube).
 */
inline void gate_level_layout_factory(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "cartesian_gate_layout",
        [](const py::tuple dimension, const std::string& scheme_name, const std::string& layout_name,
           const std::string& coordinate_type)
        {
            if (coordinate_type == "cube")
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout<py_cube_coordinate>>(dimension);
                if (const auto scheme =
                        fiction::get_clocking_scheme<py_cartesian_gate_layout<py_cube_coordinate>>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_cartesian_gate_layout<py_cube_coordinate>{ar, *scheme, layout_name});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
            else  // default: offset
            {
                const auto ar = extract_aspect_ratio<py_cartesian_layout<py_offset_coordinate>>(dimension);
                if (const auto scheme =
                        fiction::get_clocking_scheme<py_cartesian_gate_layout<py_offset_coordinate>>(scheme_name);
                    scheme.has_value())
                {
                    return py::cast(py_cartesian_gate_layout<py_offset_coordinate>{ar, *scheme, layout_name});
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open", py::arg("layout_name") = "",
        py::arg("coordinate_type") = "offset",
        R"doc(
            Creates and returns a cartesian_gate_layout instance, choosing the coordinate system
            based on the string argument. Valid options for `coordinate_type` are:
              - "offset" (default)
              - "cube"

            For the dimension, you can pass either:
              - A single tuple (x, y) or (x, y, z) to specify only the "max" coordinate, with min defaulting to (0,0,0),
              - Two nested tuples ((xmin, ymin), (xmax, ymax)) or 3D
                ((xmin, ymin, zmin), (xmax, ymax, zmax)) to specify min and max explicitly.
        )doc");

    m.def(
        "shifted_cartesian_gate_layout",
        [](const py::tuple dimension, const std::string& scheme_name, const std::string& layout_name)
        {
            const auto ar = extract_aspect_ratio<py_shifted_cartesian_layout>(dimension);
            if (const auto scheme = fiction::get_clocking_scheme<py_shifted_cartesian_gate_layout>(scheme_name);
                scheme.has_value())
            {
                return py::cast(py_shifted_cartesian_gate_layout{ar, *scheme, layout_name});
            }
            else
            {
                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open", py::arg("layout_name") = "",
        R"doc(
            Creates and returns a shifted_cartesian_gate_layout instance.

            For the dimension, you can pass either:
              - A single tuple (x, y) or (x, y, z) to specify only the "max" coordinate, with min defaulting to (0,0,0),
              - Two nested tuples ((xmin, ymin), (xmax, ymax)) or 3D
                ((xmin, ymin, zmin), (xmax, ymax, zmax)) to specify min and max explicitly.
        )doc");

    m.def(
        "hexagonal_gate_layout",
        [](const py::tuple dimension, const std::string& scheme_name, const std::string& layout_name)
        {
            const auto ar = extract_aspect_ratio<py_hexagonal_layout>(dimension);
            if (const auto scheme = fiction::get_clocking_scheme<py_hexagonal_gate_layout>(scheme_name);
                scheme.has_value())
            {
                return py::cast(py_hexagonal_gate_layout{ar, *scheme, layout_name});
            }
            else
            {
                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            }
        },
        py::arg("dimension") = py::make_tuple(0, 0, 0), py::arg("scheme_name") = "open", py::arg("layout_name") = "",
        R"doc(
            Creates and returns a hexagonal_gate_layout instance.

            For the dimension, you can pass either:
              - A single tuple (x, y) or (x, y, z) to specify only the "max" coordinate, with min defaulting to (0,0,0),
              - Two nested tuples ((xmin, ymin), (xmax, ymax)) or 3D
                ((xmin, ymin, zmin), (xmax, ymax, zmax)) to specify min and max explicitly.
        )doc");
}

}  // namespace pyfiction

#endif  // PYFICTION_GATE_LEVEL_LAYOUT_HPP
