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
 * @brief Python bindings for `fiction/layouts/cell_level_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/io/print_layout.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/layouts/layout_utils.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/technology/inml/technology.hpp>
#include <fiction/technology/qca/technology.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * @brief Registers the cell technology and cell-level layout bindings.
 *
 * @tparam Technology FCN cell technology.
 * @param m Python module.
 */
template <typename Technology>
void fcn_technology_cell_level_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // fetch technology name
    auto tech_name = std::string{fiction::tech_impl_name<Technology>};
    std::ranges::transform(tech_name, tech_name.begin(), ::tolower);
    if constexpr (std::is_same_v<Technology, fiction::qca::mol_qca_technology>)
    {
        // Keep the Python technology name readable by preserving the word boundary in molQCA.
        tech_name = "mol_qca";
    }

    /**
     * FCN cell technology.
     */
    const py::class_<Technology> tech(m, fmt::format("{}_technology", tech_name).c_str());

    if constexpr (std::is_enum_v<typename Technology::cell_mode>)
    {
        py::enum_<typename Technology::cell_mode>(tech, "cell_mode")
            .value("NORMAL", Technology::cell_mode::NORMAL)
            .value("ROTATED", Technology::cell_mode::ROTATED)
            .value("VERTICAL", Technology::cell_mode::VERTICAL)
            .value("CROSSOVER", Technology::cell_mode::CROSSOVER);
    }
    else
    {
        py::class_<typename Technology::cell_mode>(tech, "cell_mode").def(py::init<>());
    }
    py::enum_<typename Technology::cell_type> cell_type(tech, "cell_type");

    cell_type.value("EMPTY", Technology::cell_type::EMPTY);
    if constexpr (std::is_same_v<Technology, fiction::qca::mol_qca_technology>)
    {
        cell_type.value("NORMAL1", Technology::cell_type::NORMAL1,
                        DOC(fiction_qca_mol_qca_technology_cell_type_NORMAL1));
        cell_type.value("NORMAL2", Technology::cell_type::NORMAL2,
                        DOC(fiction_qca_mol_qca_technology_cell_type_NORMAL2));
        cell_type.value("NORMAL3", Technology::cell_type::NORMAL3,
                        DOC(fiction_qca_mol_qca_technology_cell_type_NORMAL3));
        cell_type.value("NORMAL4", Technology::cell_type::NORMAL4,
                        DOC(fiction_qca_mol_qca_technology_cell_type_NORMAL4));
    }
    else
    {
        cell_type.value("NORMAL", Technology::cell_type::NORMAL);
    }
    cell_type.value("INPUT", Technology::cell_type::INPUT);
    cell_type.value("OUTPUT", Technology::cell_type::OUTPUT);

    if constexpr (std::is_same_v<Technology, fiction::qca::qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0, DOC(fiction_qca_qca_technology_cell_type_CONST_0));
        cell_type.value("CONST_1", Technology::cell_type::CONST_1, DOC(fiction_qca_qca_technology_cell_type_CONST_1));
    }
    else if constexpr (std::is_same_v<Technology, fiction::qca::mol_qca_technology>)
    {
        cell_type.value("CONST_0", Technology::cell_type::CONST_0,
                        DOC(fiction_qca_mol_qca_technology_cell_type_CONST_0));
        cell_type.value("CONST_1", Technology::cell_type::CONST_1,
                        DOC(fiction_qca_mol_qca_technology_cell_type_CONST_1));
    }
    else if constexpr (std::is_same_v<Technology, fiction::inml::inml_technology>)
    {
        cell_type.value("SLANTED_EDGE_UP_MAGNET", Technology::cell_type::SLANTED_EDGE_UP_MAGNET,
                        DOC(fiction_inml_inml_technology_cell_type_SLANTED_EDGE_UP_MAGNET));
        cell_type.value("SLANTED_EDGE_DOWN_MAGNET", Technology::cell_type::SLANTED_EDGE_DOWN_MAGNET,
                        DOC(fiction_inml_inml_technology_cell_type_SLANTED_EDGE_DOWN_MAGNET));
        cell_type.value("INVERTER_MAGNET", Technology::cell_type::INVERTER_MAGNET,
                        DOC(fiction_inml_inml_technology_cell_type_INVERTER_MAGNET));
        cell_type.value("CROSSWIRE_MAGNET", Technology::cell_type::CROSSWIRE_MAGNET,
                        DOC(fiction_inml_inml_technology_cell_type_CROSSWIRE_MAGNET));
        cell_type.value("FANOUT_COUPLER_MAGNET", Technology::cell_type::FANOUT_COUPLER_MAGNET,
                        DOC(fiction_inml_inml_technology_cell_type_FANOUT_COUPLER_MAGNET));
    }
    // NOTE: more technologies go here
}

/**
 * Register a concrete cell layout without duplicating its technology enums.
 * @tparam CellLyt Cell layout.
 * @tparam ClockedLyt Registered base layout.
 * @param m Python module.
 * @param tech_name Class name prefix.
 */
template <typename CellLyt, typename ClockedLyt>
void cell_layout(nanobind::module_& m, const std::string& tech_name)
{
    namespace py = nanobind;

    /**
     * Cell-level clocked Cartesian layout.
     */
    const auto class_name = fmt::format("{}_layout", tech_name);

    py::class_<CellLyt, ClockedLyt>(m, class_name.c_str(), DOC(fiction_layouts_cell_level_layout))
        .def(py::init<>(), DOC(fiction_layouts_cell_level_layout_cell_level_layout))
        .def(py::init<const fiction::aspect_ratio<CellLyt>&>(), py::arg("dimension"),
             DOC(fiction_layouts_cell_level_layout_cell_level_layout))
        .def(
            "__init__",
            [](py::pointer_and_handle<CellLyt> self, const fiction::aspect_ratio<CellLyt>& dimension,
               const std::string& scheme_name, const std::string& layout_name)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme<CellLyt>(scheme_name);
                    scheme.has_value())
                {
                    new (self.p) CellLyt{dimension, *scheme, layout_name};
                    return;
                }

                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            },
            py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
            DOC(fiction_layouts_cell_level_layout_cell_level_layout_2))

        .def("assign_cell_mode", &CellLyt::assign_cell_mode, py::arg("c"), py::arg("mode"))
        .def("get_cell_mode", &CellLyt::get_cell_mode, py::arg("c"))
        .def("assign_cell_type", &CellLyt::assign_cell_type, py::arg("c"), py::arg("ct"),
             DOC(fiction_layouts_cell_level_layout_assign_cell_type))
        .def("get_cell_type", &CellLyt::get_cell_type, py::arg("c"),
             DOC(fiction_layouts_cell_level_layout_get_cell_type))
        .def("is_empty_cell", &CellLyt::is_empty_cell, py::arg("c"),
             DOC(fiction_layouts_cell_level_layout_is_empty_cell))
        .def("assign_cell_name", &CellLyt::assign_cell_name, py::arg("c"), py::arg("n"),
             DOC(fiction_layouts_cell_level_layout_assign_cell_name))
        .def("get_cell_name", &CellLyt::get_cell_name, py::arg("c"),
             DOC(fiction_layouts_cell_level_layout_get_cell_name))
        .def("set_layout_name", &CellLyt::set_layout_name, py::arg("name"),
             DOC(fiction_layouts_cell_level_layout_set_layout_name))
        .def("get_layout_name", &CellLyt::get_layout_name, DOC(fiction_layouts_cell_level_layout_get_layout_name))
        .def("num_cells", &CellLyt::num_cells, DOC(fiction_layouts_cell_level_layout_num_cells))
        .def("is_empty", &CellLyt::is_empty, DOC(fiction_layouts_cell_level_layout_is_empty))
        .def("num_pis", &CellLyt::num_pis, DOC(fiction_layouts_cell_level_layout_num_pis))
        .def("num_pos", &CellLyt::num_pos, DOC(fiction_layouts_cell_level_layout_num_pos))
        .def("is_pi", &CellLyt::is_pi, py::arg("c"), DOC(fiction_layouts_cell_level_layout_is_pi))
        .def("is_po", &CellLyt::is_po, py::arg("c"), DOC(fiction_layouts_cell_level_layout_is_po))

        .def("get_cell_type", &CellLyt::get_cell_type, py::arg("c"),
             DOC(fiction_layouts_cell_level_layout_get_cell_type))
        .def("get_cells_by_type", &CellLyt::get_cells_by_type, py::arg("type"),
             DOC(fiction_layouts_cell_level_layout_get_cells_by_type))
        .def("num_cells_of_given_type", &CellLyt::num_cells_of_given_type, py::arg("type"),
             DOC(fiction_layouts_cell_level_layout_num_cells_of_given_type))

        .def("cells",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> cells{};
                 cells.reserve(lyt.num_cells());
                 lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                 return cells;
             })
        .def("pis",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> pis{};
                 pis.reserve(lyt.num_pis());
                 lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                 return pis;
             })
        .def("pos",
             [](const CellLyt& lyt)
             {
                 std::vector<fiction::coordinate<CellLyt>> pos{};
                 pos.reserve(lyt.num_pos());
                 lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                 return pos;
             })
        .def(
            "bounding_box_2d",
            [](const CellLyt& lyt)
            {
                const auto bb = fiction::layouts::bounding_box_2d<CellLyt>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))

        .def(
            "__repr__",
            [](const CellLyt& lyt) -> std::string
            {
                std::stringstream stream{};

                fiction::layouts::io::print_layout(lyt, stream);

                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

/**
 * @brief Registers cell-level layouts for the supported FCN technologies.
 *
 * @param m Python module.
 */
void cell_level_layout(nanobind::module_& m)
{
    detail::fcn_technology_cell_level_layout<fiction::qca::qca_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::qca::mol_qca_technology>(m);
    detail::fcn_technology_cell_level_layout<fiction::inml::inml_technology>(m);
    detail::cell_layout<py_qca_layout, py_cartesian_clocked_layout>(m, "qca");
    detail::cell_layout<py_mol_qca_layout, py_cartesian_clocked_layout>(m, "mol_qca");
    detail::cell_layout<py_inml_layout, py_cartesian_clocked_layout>(m, "inml");
    detail::cell_layout<py_stacked_qca_layout, py_stacked_cartesian_clocked_layout>(m, "stacked_qca");
}

}  // namespace pyfiction
