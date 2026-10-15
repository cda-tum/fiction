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
 * @brief Python bindings for `fiction/technology/fcn/area.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/fcn/area.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/mol_qca/layout.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds `area` for a layout type, with the layout's cell dimensions as default arguments.
 *
 * @tparam Lyt Layout type.
 * @param m Python module.
 * @param doc Docstring of the bound `area` overload.
 */
template <typename Lyt>
void register_area(nb::module_& m, const char* doc)
{
    m.def(
        "area",
        [](const Lyt& lyt, const double width, const double height, const double hspace, const double vspace)
        {
            const fiction::fcn::area_params<Lyt> params{.width  = width,
                                                        .height = height,
                                                        .hspace = hspace,
                                                        .vspace = vspace};

            if constexpr (std::is_same_v<Lyt, fiction::sidb::layout>)
            {
                const nb::gil_scoped_release release{};
                return fiction::fcn::area(lyt, params);
            }

            // Keep the GIL: cell-grid area uses only scalar dimensions.
            return fiction::fcn::area(lyt, params);
        },
        nb::arg("layout"), nb::arg("width") = Lyt::CELL_WIDTH, nb::arg("height") = Lyt::CELL_HEIGHT,
        nb::arg("hspace") = Lyt::CELL_HSPACE, nb::arg("vspace") = Lyt::CELL_VSPACE, doc);
}

}  // namespace detail

/**
 * @brief Registers `area` for QCA, molQCA, iNML, and SiDB layouts.
 *
 * @param m Python module.
 */
void register_area(nb::module_& m)
{
    /** @brief Area documentation for layouts with a Cartesian extent. */
    constexpr auto cell_grid_doc = R"doc(Return the area of the layout's Cartesian extent in square nanometers.
Every cell position contributes its cell dimensions, including empty positions.
Neighboring positions contribute their horizontal and vertical spacing.

Args:
    layout: QCA, molQCA, or iNML layout to measure.
    width: Cell width in nm; defaults to the layout type's CELL_WIDTH.
    height: Cell height in nm; defaults to the layout type's CELL_HEIGHT.
    hspace: Horizontal spacing in nm; defaults to the layout type's CELL_HSPACE.
    vspace: Vertical spacing in nm; defaults to the layout type's CELL_VSPACE.

Returns:
    The area as a float in square nanometers.
)doc";
    /** @brief Area documentation for the occupied SiDB bounding box. */
    constexpr auto sidb_doc = R"doc(Return the SiDB layout's bounding-box area in square nanometers.
The bounding box covers SiDBs and defects. Every column and single-SiDB row
contributes its dimensions and the spacing between neighboring columns and rows.
An empty layout has zero area.

Args:
    layout: SiDB layout to measure.
    width: Column width in nm; defaults to the layout type's CELL_WIDTH.
    height: Row height in nm; defaults to the layout type's CELL_HEIGHT.
    hspace: Horizontal spacing in nm; defaults to the layout type's CELL_HSPACE.
    vspace: Vertical spacing in nm; defaults to the layout type's CELL_VSPACE.

Returns:
    The area as a float in square nanometers.
)doc";
    detail::register_area<fiction::qca::layout>(m, cell_grid_doc);
    detail::register_area<fiction::mol_qca::layout>(m, cell_grid_doc);
    detail::register_area<fiction::inml::layout>(m, cell_grid_doc);
    detail::register_area<fiction::sidb::layout>(m, sidb_doc);
}

}  // namespace pyfiction
