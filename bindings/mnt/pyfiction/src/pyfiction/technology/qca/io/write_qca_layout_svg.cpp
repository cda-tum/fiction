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
 * @brief Python bindings for `fiction/technology/qca/io/write_qca_layout_svg.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/qca/io/write_qca_layout_svg.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{
namespace detail
{

void write_qca_layout_svg_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // QCA plot
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_qca_layout_svg_pointer)(const py_qca_layout&, const std::string_view&,
                                               const fiction::qca::io::write_qca_layout_svg_params&) =
        &fiction::qca::io::write_qca_layout_svg<py_qca_layout>;

    m.def("write_qca_layout_svg", write_qca_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::qca::io::write_qca_layout_svg_params{},
          DOC(fiction_qca_io_write_qca_layout_svg));
}

void write_mol_qca_layout_svg_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // MolQCA plot
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_mol_qca_layout_svg_pointer)(const py_mol_qca_layout&, const std::string_view&,
                                                   const fiction::qca::io::write_qca_layout_svg_params&) =
        &fiction::qca::io::write_mol_qca_layout_svg<py_mol_qca_layout>;

    m.def("write_mol_qca_layout_svg", write_mol_qca_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::qca::io::write_qca_layout_svg_params{},
          DOC(fiction_qca_io_write_mol_qca_layout_svg));
}

}  // namespace detail

void write_qca_layout_svg(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::qca::io::write_qca_layout_svg_params>(m, "write_qca_layout_svg_params",
                                                              DOC(fiction_qca_io_write_qca_layout_svg_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simple", &fiction::qca::io::write_qca_layout_svg_params::simple,
                DOC(fiction_qca_io_write_qca_layout_svg_params_simple));

    detail::write_qca_layout_svg_impl(m);
    detail::write_mol_qca_layout_svg_impl(m);
}

}  // namespace pyfiction
