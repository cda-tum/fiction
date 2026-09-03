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
 * @brief Python bindings for `fiction/technology/qca/io/write_fqca_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/qca/io/write_fqca_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_fqca_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_fqca_layout_function_pointer)(const Lyt&, const std::string_view&,
                                                     const fiction::qca::io::write_fqca_layout_params) =
        &fiction::qca::io::write_fqca_layout<Lyt>;

    m.def("write_fqca_layout", write_fqca_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::qca::io::write_fqca_layout_params{}, DOC(fiction_qca_io_write_fqca_layout));
}

}  // namespace detail

void write_fqca_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::qca::io::out_of_cell_names_exception>(
        m, "out_of_cell_names_exception",
        PyExc_IndexError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    py::class_<fiction::qca::io::write_fqca_layout_params>(m, "write_fqca_layout_params",
                                                           DOC(fiction_qca_io_write_fqca_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("create_inter_layer_via_cells",
                &fiction::qca::io::write_fqca_layout_params::create_inter_layer_via_cells,
                DOC(fiction_qca_io_write_fqca_layout_params_create_inter_layer_via_cells))

        ;

    detail::write_fqca_layout<py_qca_layout>(m);
}

}  // namespace pyfiction
