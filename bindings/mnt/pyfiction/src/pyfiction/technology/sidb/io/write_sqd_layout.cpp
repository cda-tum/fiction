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
 * @brief Python bindings for `fiction/technology/sidb/io/write_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{
/**
 * @brief Registers SQD export.
 *
 * @tparam Lyt SiDB layout type.
 * @param m Python module.
 */
template <typename Lyt>
void write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_sqd_layout_function_pointer)(const Lyt&, const std::string_view&) =
        &fiction::sidb::io::write_sqd_layout<Lyt>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_sidb_io_write_sqd_layout));
}
}  // namespace detail

/**
 * @brief Registers SQD export.
 *
 * @param m Python module.
 */
void write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    detail::write_sqd_layout<py_sidb_111_lattice>(m);
    detail::write_sqd_layout<py_sidb_100_lattice>(m);
    detail::write_sqd_layout<py_sidb_layout>(m);

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_sqd_layout_function_pointer)(const fiction::sidb::layout&, const std::string_view&) =
        &fiction::sidb::io::write_sqd_layout;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_sidb_io_write_sqd_layout_4));
}

}  // namespace pyfiction
