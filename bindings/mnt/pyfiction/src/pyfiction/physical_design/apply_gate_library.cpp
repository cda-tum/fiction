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
 * @brief Python bindings for `fiction/physical_design/apply_gate_library.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/qca/sim7_mol_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <stdexcept>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Apply a gate library and report unsupported gates with their tile coordinates.
 *
 * @tparam CellLyt Destination cell-level layout.
 * @tparam GateLibrary Gate library.
 * @tparam GateLyt Source gate-level layout.
 * @param layout Source layout.
 * @return Mapped cell-level layout.
 * @throws std::invalid_argument If the library cannot implement a gate or its orientation.
 */
template <typename CellLyt, typename GateLibrary, typename GateLyt>
CellLyt checked_apply_gate_library(const GateLyt& layout)
{
    try
    {
        return fiction::physical_design::apply_gate_library<CellLyt, GateLibrary>(layout);
    }
    catch (const fiction::fcn::unsupported_gate_type_exception<fiction::tile<GateLyt>>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate type at tile {}", error.where()));
    }
    catch (const fiction::fcn::unsupported_gate_orientation_exception<fiction::tile<GateLyt>,
                                                                      fiction::fcn::port_position>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate orientation at tile {}", error.where()));
    }
    catch (const fiction::fcn::unsupported_gate_orientation_exception<fiction::tile<GateLyt>,
                                                                      fiction::fcn::port_direction>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate orientation at tile {}", error.where()));
    }
}

/**
 * Bind a gate library for its supported topology.
 * @tparam GateLibrary Gate library.
 * @tparam GateLyt Source layout.
 * @param m Python module.
 * @param lib_name Library name.
 */
template <typename GateLibrary, typename GateLyt>
void apply_gate_library(nanobind::module_& m, const std::string& lib_name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    /** Destination layout for the library technology. */
    using py_cartesian_technology_cell_layout = py_cartesian_cell_layout<fiction::technology<GateLibrary>>;

    m.def(fmt::format("apply_{}_library", lib_name).c_str(),
          &checked_apply_gate_library<py_cartesian_technology_cell_layout, GateLibrary, GateLyt>, py::arg("layout"),
          DOC(fiction_physical_design_apply_gate_library));
}

}  // namespace detail

/** Register the four supported gate libraries.
 * @param m Python module.
 */
void apply_gate_library(nanobind::module_& m)
{
    detail::apply_gate_library<fiction::qca::qca_one_library, py_cartesian_gate_layout>(m, "qca_one");
    detail::apply_gate_library<fiction::qca::sim7_mol_library, py_cartesian_gate_layout>(m, "sim7_mol");
    detail::apply_gate_library<fiction::inml::topolinano_library, py_shifted_cartesian_gate_layout>(m, "topolinano");

    namespace py = nanobind;

    // the SiDB gate library yields an SiDB layout over the H-Si(100) 2x1 lattice
    m.def(
        "apply_bestagon_library",
        [](const py_hexagonal_gate_layout& lyt)
        {
            return fiction::sidb::to_sidb_layout(
                detail::checked_apply_gate_library<py_cartesian_cell_layout<fiction::sidb::sidb_technology>,
                                                   fiction::sidb::bestagon_library, py_hexagonal_gate_layout>(lyt));
        },
        py::arg("layout"), DOC(fiction_physical_design_apply_gate_library));
}

}  // namespace pyfiction
