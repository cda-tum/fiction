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

#include "fiction/layouts/layout_base.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/technology/mol_qca/sim7_mol_library.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/technology/sidb/bestagon_library.hpp>
#include <fiction/utils/progress.hpp>

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
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * Apply a gate library and report unsupported gates with their tile coordinates.
 *
 * @tparam GateLibrary Gate library.
 * @tparam GateLyt Source gate-level layout.
 * @param layout Source layout.
 * @param on_progress Receives completed gate mappings.
 * @return The layout type that `GateLibrary` produces.
 * @throws std::invalid_argument If the library cannot implement a gate or its orientation.
 */
template <typename GateLibrary, typename GateLyt>
typename GateLibrary::layout checked_apply_gate_library(const GateLyt&                           layout,
                                                        const fiction::utils::progress_callback& on_progress = {})
{
    try
    {
        return fiction::physical_design::apply_gate_library<GateLibrary>(layout, on_progress);
    }
    catch (const fiction::fcn::unsupported_gate_type_exception<fiction::layouts::layout_base::coordinate>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate type at tile {}", error.where()));
    }
    catch (const fiction::fcn::unsupported_gate_orientation_exception<fiction::layouts::layout_base::coordinate,
                                                                      fiction::fcn::port_position>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate orientation at tile {}", error.where()));
    }
    catch (const fiction::fcn::unsupported_gate_orientation_exception<fiction::layouts::layout_base::coordinate,
                                                                      fiction::fcn::port_direction>& error)
    {
        throw std::invalid_argument(fmt::format("unsupported gate orientation at tile {}", error.where()));
    }
}

/**
 * @brief Bind a gate library for its supported topology.
 * @tparam GateLibrary Gate library.
 * @tparam GateLyt Source layout.
 * @param m Python module.
 * @param lib_name Library name.
 */
template <typename GateLibrary, typename GateLyt>
void register_apply_gate_library(nb::module_& m, const std::string& lib_name)
{
    m.def(fmt::format("apply_{}_library", lib_name).c_str(), &checked_apply_gate_library<GateLibrary, GateLyt>,
          nb::arg("layout"), nb::arg("on_progress").none() = nb::none(), nb::call_guard<nb::gil_scoped_release>(),
          R"doc(Apply the selected gate library to produce a cell-level layout.
The cell grid spans the gate-level layout. QCA and iNML tile-based clock zones
follow the gate-level clocking. Supported synchronization elements apply each
gate tile's delay to its entire clock zone, including cells added later. molQCA
cell types carry the clock phases defined by the gate library. Input and output
cells retain their object names. The Bestagon library produces an SiDB layout
on the H-Si(100)-2x1 lattice.

Args:
    layout: Gate-level layout to map.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.

Returns:
    The QCA, molQCA, iNML, or SiDB layout produced by the selected library.

Raises:
    ValueError: The library cannot implement a gate type or orientation; the message names its tile.
)doc");
}

}  // namespace detail

/**
 * @brief Register the four supported gate libraries.
 * @param m Python module.
 */
void register_apply_gate_library(nb::module_& m)
{
    detail::register_apply_gate_library<fiction::qca::qca_one_library, py_cartesian_gate_layout>(m, "qca_one");
    detail::register_apply_gate_library<fiction::mol_qca::sim7_mol_library, py_cartesian_gate_layout>(m, "sim7_mol");
    detail::register_apply_gate_library<fiction::inml::topolinano_library, py_shifted_cartesian_gate_layout>(
        m, "topolinano");
    detail::register_apply_gate_library<fiction::sidb::bestagon_library, py_hexagonal_gate_layout>(m, "bestagon");
}

}  // namespace pyfiction
