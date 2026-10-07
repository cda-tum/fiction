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

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

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
void area(nanobind::module_& m, const char* doc)
{
    m.def(
        "area",
        [](const Lyt& lyt, const double width, const double height, const double hspace, const double vspace)
        {
            fiction::fcn::area_stats stats{};

            return fiction::fcn::area(
                lyt,
                fiction::fcn::area_params<Lyt>{.width = width, .height = height, .hspace = hspace, .vspace = vspace},
                &stats);
        },
        nanobind::arg("layout"), nanobind::arg("width") = Lyt::CELL_WIDTH, nanobind::arg("height") = Lyt::CELL_HEIGHT,
        nanobind::arg("hspace") = Lyt::CELL_HSPACE, nanobind::arg("vspace") = Lyt::CELL_VSPACE, doc);
}

}  // namespace detail

/**
 * @brief Registers `area` for QCA, molQCA, iNML, and SiDB layouts.
 *
 * @param m Python module.
 */
void area(nanobind::module_& m)
{
    detail::area<fiction::qca::layout>(m, DOC(fiction_fcn_area));
    detail::area<fiction::mol_qca::layout>(m, DOC(fiction_fcn_area));
    detail::area<fiction::inml::layout>(m, DOC(fiction_fcn_area));
    detail::area<fiction::sidb::layout>(m, DOC(fiction_fcn_area_3));
}

}  // namespace pyfiction
