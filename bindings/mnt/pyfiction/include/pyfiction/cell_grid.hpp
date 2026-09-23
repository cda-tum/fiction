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
 * @brief Binding helpers for the methods that the QCA, molQCA, and iNML layouts share through `layouts::cell_grid` and
 * `layouts::tile_clocking`.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/io/print_layout.hpp>

#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts clocking scheme names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction::detail
{

/**
 * @brief Binds the cell types, cell names, inputs and outputs, and layout name of a cell grid layout.
 *
 * @tparam Lyt Cell grid layout type.
 * @param cls Python class of `Lyt`.
 */
template <typename Lyt>
void bind_cell_grid(nanobind::class_<Lyt, py_cartesian_layout>& cls)
{
    namespace py = nanobind;

    using cell = typename Lyt::cell;

    cls.def("assign_cell_type", &Lyt::assign_cell_type, py::arg("c"), py::arg("ct"),
            DOC(fiction_layouts_cell_grid_assign_cell_type))
        .def("get_cell_type", &Lyt::get_cell_type, py::arg("c"), DOC(fiction_layouts_cell_grid_get_cell_type))
        .def("is_empty_cell", &Lyt::is_empty_cell, py::arg("c"), DOC(fiction_layouts_cell_grid_is_empty_cell))
        .def("assign_cell_name", &Lyt::assign_cell_name, py::arg("c"), py::arg("n"),
             DOC(fiction_layouts_cell_grid_assign_cell_name))
        .def("get_cell_name", &Lyt::get_cell_name, py::arg("c"), DOC(fiction_layouts_cell_grid_get_cell_name))
        .def("set_layout_name", &Lyt::set_layout_name, py::arg("name"), DOC(fiction_layouts_cell_grid_set_layout_name))
        .def("get_layout_name", &Lyt::get_layout_name, DOC(fiction_layouts_cell_grid_get_layout_name))
        .def("num_cells", &Lyt::num_cells, DOC(fiction_layouts_cell_grid_num_cells))
        .def("is_empty", &Lyt::is_empty, DOC(fiction_layouts_cell_grid_is_empty))
        .def("num_pis", &Lyt::num_pis, DOC(fiction_layouts_cell_grid_num_pis))
        .def("num_pos", &Lyt::num_pos, DOC(fiction_layouts_cell_grid_num_pos))
        .def("is_pi", &Lyt::is_pi, py::arg("c"), DOC(fiction_layouts_cell_grid_is_pi))
        .def("is_po", &Lyt::is_po, py::arg("c"), DOC(fiction_layouts_cell_grid_is_po))
        .def(
            "cells",
            [](const Lyt& lyt)
            {
                std::vector<cell> cells{};
                cells.reserve(lyt.num_cells());
                lyt.foreach_cell([&cells](const auto& c) { cells.push_back(c); });
                return cells;
            },
            "Returns the positions of all cells, in unspecified order.")
        .def(
            "pis",
            [](const Lyt& lyt)
            {
                std::vector<cell> pis{};
                lyt.foreach_pi([&pis](const auto& c) { pis.push_back(c); });
                return pis;
            },
            "Returns the positions of all input cells, in unspecified order.")
        .def(
            "pos",
            [](const Lyt& lyt)
            {
                std::vector<cell> pos{};
                lyt.foreach_po([&pos](const auto& c) { pos.push_back(c); });
                return pos;
            },
            "Returns the positions of all output cells, in unspecified order.")
        .def(
            "bounding_box_2d",
            [](const Lyt& lyt)
            {
                const auto bb = fiction::layouts::bounding_box_2d<Lyt>(lyt);
                return std::make_pair(bb.get_min(), bb.get_max());
            },
            DOC(fiction_bounding_box_2d_overridden))
        .def(
            "__copy__", [](const Lyt& lyt) { return Lyt{lyt}; }, "Returns an independent copy of the layout.")
        .def(
            "__deepcopy__", [](const Lyt& lyt, const py::dict&) { return Lyt{lyt}; }, py::arg("memo"),
            "Returns an independent copy of the layout.")
        .def(py::self == py::self)  // NOLINT(misc-redundant-expression)
        .def(py::self != py::self)  // NOLINT(misc-redundant-expression)
        .def(
            "__repr__",
            [](const Lyt& lyt) -> std::string
            {
                std::stringstream stream{};

                fiction::layouts::io::print_layout(lyt, stream);

                return stream.str();
            },
            "Returns a string representation of the layout.");
}
/**
 * @brief Binds the tile-based clocking of a cell grid layout.
 *
 * @tparam Lyt Cell grid layout type with tile-based clocking.
 * @param cls Python class of `Lyt`.
 */
template <typename Lyt>
void bind_tile_clocking(nanobind::class_<Lyt, py_cartesian_layout>& cls)
{
    namespace py = nanobind;

    cls.def("get_tile_size_x", &Lyt::get_tile_size_x, DOC(fiction_layouts_tile_clocking_get_tile_size_x))
        .def("set_tile_size_x", &Lyt::set_tile_size_x, py::arg("size"),
             DOC(fiction_layouts_tile_clocking_set_tile_size_x))
        .def("get_tile_size_y", &Lyt::get_tile_size_y, DOC(fiction_layouts_tile_clocking_get_tile_size_y))
        .def("set_tile_size_y", &Lyt::set_tile_size_y, py::arg("size"),
             DOC(fiction_layouts_tile_clocking_set_tile_size_y))
        .def("get_clock_zone", &Lyt::get_clock_zone, py::arg("c"), DOC(fiction_layouts_tile_clocking_get_clock_zone))
        .def("assign_clock_number", &Lyt::assign_clock_number, py::arg("cz"), py::arg("cn"),
             DOC(fiction_layouts_tile_clocking_assign_clock_number))
        .def("get_clock_number", &Lyt::get_clock_number, py::arg("c"),
             DOC(fiction_layouts_tile_clocking_get_clock_number))
        .def("num_clocks", &Lyt::num_clocks, DOC(fiction_layouts_tile_clocking_num_clocks))
        .def("is_regularly_clocked", &Lyt::is_regularly_clocked,
             DOC(fiction_layouts_tile_clocking_is_regularly_clocked))
        .def("is_clocking_scheme", &Lyt::is_clocking_scheme, py::arg("name"),
             DOC(fiction_layouts_tile_clocking_is_clocking_scheme))
        .def(
            "get_clocking_scheme_name", [](const Lyt& lyt) { return std::string{lyt.get_clocking_scheme().name}; },
            "Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.")
        .def(
            "replace_clocking_scheme",
            [](Lyt& lyt, const std::string& name)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme<Lyt>(name); scheme)
                {
                    lyt.replace_clocking_scheme(*scheme);
                }
                else
                {
                    throw std::invalid_argument("Unknown clocking scheme");
                }
            },
            py::arg("name"),
            "Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are "
            "discarded. Raises ValueError for an unknown name.");
}
/**
 * @brief Binds the constructors of a cell grid layout with tile-based clocking: an empty layout, a layout of the given
 * dimension with open clocking, and a layout clocked by a predefined scheme.
 *
 * @tparam Lyt Cell grid layout type with tile-based clocking.
 * @param cls Python class of `Lyt`.
 */
template <typename Lyt>
void bind_clocked_constructors(nanobind::class_<Lyt, py_cartesian_layout>& cls)
{
    namespace py = nanobind;

    cls.def(py::init<>())
        .def(py::init<const typename Lyt::aspect_ratio&>(), py::arg("dimension"))
        .def(
            "__init__",
            [](Lyt* self, const typename Lyt::aspect_ratio& dimension, const std::string& scheme_name,
               const std::string& layout_name, const uint16_t tile_size_x, const uint16_t tile_size_y)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme<Lyt>(scheme_name); scheme.has_value())
                {
                    new (self) Lyt{dimension, *scheme, layout_name, tile_size_x, tile_size_y};
                    return;
                }

                throw std::invalid_argument("Given name does not refer to a supported clocking scheme");
            },
            py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave", py::arg("layout_name") = "",
            py::arg("tile_size_x") = 1u, py::arg("tile_size_y") = 1u,
            "Creates an empty layout of the given dimension, clocked by the predefined scheme of the given name, with "
            "clock zones of the given tile size. Raises ValueError for an unknown scheme or a zero tile size.");
}

}  // namespace pyfiction::detail
