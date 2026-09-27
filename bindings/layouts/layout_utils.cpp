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

#include <fiction/layouts/layout_utils.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/mol_qca/layout.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds coordinate normalization for a cell layout.
 * @tparam Lyt Cell layout type.
 * @param m Python layouts module.
 */
template <typename Lyt>
void normalize_layout_coordinates(nanobind::module_& m)
{
    m.def("normalize_layout_coordinates", &fiction::layouts::normalize_layout_coordinates<Lyt>, nanobind::arg("lyt"),
          DOC(fiction_layouts_normalize_layout_coordinates));
}

}  // namespace detail

/**
 * @brief Registers normalization for QCA, molecular QCA, and iNML layouts.
 * @param m Python layouts module.
 */
void layout_utils(nanobind::module_& m)
{
    detail::normalize_layout_coordinates<fiction::qca::layout>(m);
    detail::normalize_layout_coordinates<fiction::mol_qca::layout>(m);
    detail::normalize_layout_coordinates<fiction::inml::layout>(m);
}

}  // namespace pyfiction
