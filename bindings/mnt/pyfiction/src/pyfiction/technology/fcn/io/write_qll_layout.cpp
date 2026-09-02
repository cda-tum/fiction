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
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/fcn/io/write_qll_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_qll_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_qll_layout_function_pointer)(const Lyt&, const std::string_view&) =
        &fiction::fcn::io::write_qll_layout<Lyt>;

    m.def("write_qll_layout", write_qll_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_fcn_io_write_qll_layout));
}

}  // namespace detail

void write_qll_layout(nanobind::module_& m)
{
    detail::write_qll_layout<py_qca_layout>(m);
    detail::write_qll_layout<py_inml_layout>(m);
    detail::write_qll_layout<py_mol_qca_layout>(m);
}

}  // namespace pyfiction
