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
 * @brief Python bindings for `fiction/technology/sidb/charge_distribution.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>

#include <functional>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
// These headers register nanobind type casters without exposing directly referenced symbols.
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void charge_distribution(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::charge_distribution;

    py::class_<charge_distribution>(m, "charge_distribution", DOC(fiction_sidb_charge_distribution))
        .def(py::init<>(), DOC(fiction_sidb_charge_distribution_charge_distribution))
        .def(py::init<const fiction::sidb::layout&, fiction::sidb::model::charge_state>(), py::arg("layout"),
             py::arg("charge_state") = fiction::sidb::model::charge_state::NEGATIVE,
             DOC(fiction_sidb_charge_distribution_charge_distribution_2))
        .def("sites", &charge_distribution::sites, DOC(fiction_sidb_charge_distribution_sites))
        .def("size", &charge_distribution::size, DOC(fiction_sidb_charge_distribution_size))
        .def("empty", &charge_distribution::empty, DOC(fiction_sidb_charge_distribution_empty))
        .def("index_of", &charge_distribution::index_of, py::arg("site"),
             DOC(fiction_sidb_charge_distribution_index_of))
        .def("get_charge_state", &charge_distribution::get_charge_state, py::arg("site"),
             DOC(fiction_sidb_charge_distribution_get_charge_state))
        .def("get_charge_state_by_index", &charge_distribution::get_charge_state_by_index, py::arg("index"),
             DOC(fiction_sidb_charge_distribution_get_charge_state_by_index))
        .def("assign_charge_state", &charge_distribution::assign_charge_state, py::arg("site"), py::arg("charge_state"),
             DOC(fiction_sidb_charge_distribution_assign_charge_state))
        .def("assign_charge_state_by_index", &charge_distribution::assign_charge_state_by_index, py::arg("index"),
             py::arg("charge_state"), DOC(fiction_sidb_charge_distribution_assign_charge_state_by_index))
        .def("assign_all_charge_states", &charge_distribution::assign_all_charge_states, py::arg("charge_state"),
             DOC(fiction_sidb_charge_distribution_assign_all_charge_states))
        .def("charge_states", &charge_distribution::charge_states, DOC(fiction_sidb_charge_distribution_charge_states))
        .def("energy", &charge_distribution::energy, DOC(fiction_sidb_charge_distribution_energy))
        .def("assign_energy", &charge_distribution::assign_energy, py::arg("energy"),
             DOC(fiction_sidb_charge_distribution_assign_energy))
        .def("charge_exists", &charge_distribution::charge_exists, py::arg("charge_state"),
             DOC(fiction_sidb_charge_distribution_charge_exists))
        .def("num_negative_sidbs", &charge_distribution::num_negative_sidbs,
             DOC(fiction_sidb_charge_distribution_num_negative_sidbs))
        .def("num_neutral_sidbs", &charge_distribution::num_neutral_sidbs,
             DOC(fiction_sidb_charge_distribution_num_neutral_sidbs))
        .def("num_positive_sidbs", &charge_distribution::num_positive_sidbs,
             DOC(fiction_sidb_charge_distribution_num_positive_sidbs))
        .def("charge_index", &charge_distribution::charge_index, py::arg("base"),
             DOC(fiction_sidb_charge_distribution_charge_index))
        .def("same_charge_states", &charge_distribution::same_charge_states, py::arg("other"),
             DOC(fiction_sidb_charge_distribution_same_charge_states))
        // nanobind uses `py::self` on both sides to declare same-type operators.
        .def(py::self == py::self,  // NOLINT(misc-redundant-expression)
             DOC(fiction_sidb_charge_distribution_operator_eq))
        .def(py::self != py::self)  // NOLINT(misc-redundant-expression)
        .def("__hash__", [](const charge_distribution& cd) { return std::hash<charge_distribution>{}(cd); })
        .def("__len__", &charge_distribution::size)
        .def("__repr__", [](const charge_distribution& cd)
             { return fiction::sidb::model::charge_configuration_to_string(cd.charge_states()); })

        ;
}

}  // namespace pyfiction
