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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp>
#include <fiction/technology/sidb/simulation/domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void physically_valid_parameters(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(py::init<>(), "Default constructor.")
        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>&
                                                                        domain,
               const fiction::sidb::simulation::logic::parameter_point& pp)
            {
                if (const auto result = domain.contains(pp); result.has_value())
                {
                    return std::get<0>(result.value());
                }
                // If no value is present, raise a Python ValueError
                throw py::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            py::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    m.def("physically_valid_parameters", &fiction::sidb::simulation::analysis::physically_valid_parameters,
          py::arg("lyt"), py::arg("cd"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_params{},
          DOC(fiction_sidb_simulation_analysis_physically_valid_parameters));
}

}  // namespace pyfiction
