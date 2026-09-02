/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by Jan Drewniok on 11.04.24.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/model/nm_distance.hpp>

#include <fmt/format.h>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void nanometer_distance(nanobind::module_& m, const std::string& lattice = "")
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(fmt::format("sidb_nm_distance{}", lattice).c_str(), &fiction::sidb::model::nm_distance<Lyt>, py::arg("lyt"),
          py::arg("source"), py::arg("target"), DOC(fiction_sidb_model_nm_distance));
}

}  // namespace detail

void nm_distance(nanobind::module_& m)
{
    detail::nanometer_distance<py_sidb_100_lattice>(m, "_100");
    detail::nanometer_distance<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction
