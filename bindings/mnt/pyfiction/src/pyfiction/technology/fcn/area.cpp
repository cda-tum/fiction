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
#include "pyfiction/types.hpp"

#include <fiction/technology/fcn/area.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/traits.hpp>

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

template <typename Lyt>
void area(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    using tech = fiction::technology<Lyt>;

    m.def(
        "area",
        [](const Lyt& lyt, const double width = tech::CELL_WIDTH, const double height = tech::CELL_HEIGHT,
           const double hspace = tech::CELL_HSPACE, const double vspace = tech::CELL_VSPACE)
        {
            fiction::fcn::area_stats              stats{};
            const fiction::fcn::area_params<tech> params{width, height, hspace, vspace};

            return fiction::fcn::area<Lyt>(lyt, params, &stats);
        },
        py::arg("layout"), py::arg("width") = tech::CELL_WIDTH, py::arg("height") = tech::CELL_HEIGHT,
        py::arg("hspace") = tech::CELL_HSPACE, py::arg("vspace") = tech::CELL_VSPACE,
        DOC(fiction_layouts_coords_area_of));
}

}  // namespace detail

void area(nanobind::module_& m)
{
    detail::area<py_qca_layout>(m);
    detail::area<py_inml_layout>(m);

    namespace py    = nanobind;
    using sidb_tech = fiction::sidb::sidb_technology;

    m.def(
        "area",
        [](const fiction::sidb::layout& lyt, const double width, const double height, const double hspace,
           const double vspace)
        {
            fiction::fcn::area_stats                   stats{};
            const fiction::fcn::area_params<sidb_tech> params{width, height, hspace, vspace};
            return fiction::fcn::area(lyt, params, &stats);
        },
        py::arg("layout"), py::arg("width") = sidb_tech::CELL_WIDTH, py::arg("height") = sidb_tech::CELL_HEIGHT,
        py::arg("hspace") = sidb_tech::CELL_HSPACE, py::arg("vspace") = sidb_tech::CELL_VSPACE,
        DOC(fiction_fcn_area_3));
}

}  // namespace pyfiction
