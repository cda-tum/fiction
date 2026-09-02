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
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/model/nm_position.hpp>
#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void nm_position(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("sidb_nm_position", &fiction::sidb::model::nm_position<Lyt>, py::arg("lyt"), py::arg("c"),
          DOC(fiction_sidb_model_nm_position));
}

}  // namespace detail

void nm_position(nanobind::module_& m)
{
    detail::nm_position<py_charge_distribution_surface>(m);
    detail::nm_position<py_sidb_layout>(m);
    detail::nm_position<py_sidb_lattice<fiction::sidb::surfaces::lattice_100>>(m);
    detail::nm_position<py_sidb_lattice<fiction::sidb::surfaces::lattice_111>>(m);
}

}  // namespace pyfiction
