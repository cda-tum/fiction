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
 * @brief Python bindings for `fiction/physical_design/path_finding/distance.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/physical_design/path_finding/distance.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/** @brief Binds distances between shared layout coordinates. @param m Physical-design module. */
void distance(nanobind::module_& m)
{
    namespace py = nanobind;
    m.def("manhattan_distance", &fiction::physical_design::path_finding::manhattan_distance, py::arg("source"),
          py::arg("target"), DOC(fiction_physical_design_path_finding_manhattan_distance));
    m.def("euclidean_distance", &fiction::physical_design::path_finding::euclidean_distance, py::arg("source"),
          py::arg("target"), DOC(fiction_physical_design_path_finding_euclidean_distance));
    m.def("squared_euclidean_distance", &fiction::physical_design::path_finding::squared_euclidean_distance,
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_squared_euclidean_distance));
    m.def("twoddwave_distance", &fiction::physical_design::path_finding::twoddwave_distance, py::arg("source"),
          py::arg("target"), DOC(fiction_physical_design_path_finding_twoddwave_distance));
    m.def("chebyshev_distance", &fiction::physical_design::path_finding::chebyshev_distance, py::arg("source"),
          py::arg("target"), DOC(fiction_physical_design_path_finding_chebyshev_distance));
}

}  // namespace pyfiction
