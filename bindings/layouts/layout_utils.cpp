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
 * @brief Python bindings for `fiction/layouts/layout_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/layout_utils.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/mol_qca/layout.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/traits.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void normalize_layout_coordinates(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("normalize_layout_coordinates", &fiction::layouts::normalize_layout_coordinates<Lyt>, py::arg("lyt"),
          DOC(fiction_layouts_normalize_layout_coordinates));
}

}  // namespace detail

void layout_utils(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::normalize_layout_coordinates<fiction::qca::layout>(m);
    detail::normalize_layout_coordinates<fiction::mol_qca::layout>(m);
    detail::normalize_layout_coordinates<fiction::inml::layout>(m);
}

}  // namespace pyfiction
