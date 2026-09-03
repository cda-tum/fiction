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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void operational_domain_ratio(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::operational_domain_ratio_params;
    using fiction::sidb::simulation::logic::parameter_point;

    py::class_<operational_domain_ratio_params>(m, "operational_domain_ratio_params",
                                                DOC(fiction_sidb_simulation_logic_operational_domain_ratio_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("op_domain_params", &operational_domain_ratio_params::op_domain_params,
                DOC(fiction_sidb_simulation_logic_operational_domain_ratio_params_op_domain_params));

    m.def(
        "operational_domain_ratio",
        [](const layout& lyt, const std::vector<py_tt>& spec, const parameter_point& pp,
           const operational_domain_ratio_params& params)
        { return fiction::sidb::simulation::logic::operational_domain_ratio(lyt, spec, pp, params); },
        py::arg("lyt"), py::arg("spec"), py::arg("pp"), py::arg("params") = operational_domain_ratio_params{},
        DOC(fiction_sidb_simulation_logic_operational_domain_ratio));
}

}  // namespace pyfiction
