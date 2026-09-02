/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void compute_operational_ratio_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("operational_domain_ratio", &fiction::sidb::simulation::logic::operational_domain_ratio<Lyt, py_tt>,
          py::arg("lyt"), py::arg("spec"), py::arg("pp"),
          py::arg("params") = fiction::sidb::simulation::logic::operational_domain_ratio_params{},
          DOC(fiction_sidb_simulation_logic_operational_domain_ratio));
}

}  // namespace detail

void operational_domain_ratio(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::logic::operational_domain_ratio_params>(
        m, "operational_domain_ratio_params", DOC(fiction_sidb_simulation_logic_operational_domain_ratio_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("op_domain_params",
                &fiction::sidb::simulation::logic::operational_domain_ratio_params::op_domain_params,
                DOC(fiction_sidb_simulation_logic_operational_domain_ratio_params_op_domain_params));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::compute_operational_ratio_impl<py_sidb_100_lattice>(m);
    detail::compute_operational_ratio_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
