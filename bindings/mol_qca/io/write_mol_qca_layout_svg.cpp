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
 * @brief Python bindings for `fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/progress.hpp"

#include <fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp>
#include <fiction/technology/mol_qca/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Registers the molQCA SVG writer and its parameters.
 *
 * @param m Python module.
 */
void write_mol_qca_layout_svg(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::mol_qca::io::write_mol_qca_layout_svg_params;

    py::class_<write_mol_qca_layout_svg_params>(m, "write_mol_qca_layout_svg_params",
                                                DOC(fiction_mol_qca_io_write_mol_qca_layout_svg_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("on_progress", &write_mol_qca_layout_svg_params::on_progress, pyfiction::ON_PROGRESS_GETTER,
                pyfiction::CALLBACK_SETTER, "Receives serialization progress.")
        .def_rw("simple", &write_mol_qca_layout_svg_params::simple,
                DOC(fiction_mol_qca_io_write_mol_qca_layout_svg_params_simple));

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_pointer)(const fiction::mol_qca::layout&, const std::string_view&,
                                const write_mol_qca_layout_svg_params&) =
        &fiction::mol_qca::io::write_mol_qca_layout_svg;

    m.def("write_mol_qca_layout_svg", write_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = write_mol_qca_layout_svg_params{}, DOC(fiction_mol_qca_io_write_mol_qca_layout_svg),
          py::call_guard<py::gil_scoped_release>());
}

}  // namespace pyfiction
