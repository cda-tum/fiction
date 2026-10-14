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

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
// These headers register nanobind type casters without exposing directly referenced symbols.
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers SiDB charge distributions.
 *
 * @param m Python module.
 */
void register_charge_distribution(nb::module_& m)
{
    // Keep the GIL for charge-state access and mutation so calls on one distribution remain serialized.
    using fiction::sidb::charge_distribution;

    auto charge_distribution_class =
        nb::class_<charge_distribution>(m, "charge_distribution", DOC(fiction_sidb_charge_distribution))
            .def(nb::init<>(), DOC(fiction_sidb_charge_distribution_charge_distribution))
            .def(nb::init<const fiction::sidb::layout&, fiction::sidb::model::charge_state>(), nb::arg("layout"),
                 nb::arg("charge_state") = fiction::sidb::model::charge_state::NEGATIVE,
                 DOC(fiction_sidb_charge_distribution_charge_distribution_2))
            .def("sites", &charge_distribution::sites, DOC(fiction_sidb_charge_distribution_sites))
            .def("size", &charge_distribution::size, DOC(fiction_sidb_charge_distribution_size))
            .def("empty", &charge_distribution::empty, DOC(fiction_sidb_charge_distribution_empty))
            .def("index_of", &charge_distribution::index_of, nb::arg("site"),
                 DOC(fiction_sidb_charge_distribution_index_of))
            .def("get_charge_state", &charge_distribution::get_charge_state, nb::arg("site"),
                 DOC(fiction_sidb_charge_distribution_get_charge_state))
            .def("get_charge_state_by_index", &charge_distribution::get_charge_state_by_index, nb::arg("index"),
                 DOC(fiction_sidb_charge_distribution_get_charge_state_by_index))
            .def("assign_charge_state", &charge_distribution::assign_charge_state, nb::arg("site"),
                 nb::arg("charge_state"), DOC(fiction_sidb_charge_distribution_assign_charge_state))
            .def("assign_charge_state_by_index", &charge_distribution::assign_charge_state_by_index, nb::arg("index"),
                 nb::arg("charge_state"), DOC(fiction_sidb_charge_distribution_assign_charge_state_by_index))
            .def("assign_all_charge_states", &charge_distribution::assign_all_charge_states, nb::arg("charge_state"),
                 DOC(fiction_sidb_charge_distribution_assign_all_charge_states))
            .def("charge_states", &charge_distribution::charge_states,
                 DOC(fiction_sidb_charge_distribution_charge_states))
            .def("energy", &charge_distribution::energy, DOC(fiction_sidb_charge_distribution_energy))
            .def("assign_energy", &charge_distribution::assign_energy, nb::arg("energy"),
                 DOC(fiction_sidb_charge_distribution_assign_energy))
            .def("charge_exists", &charge_distribution::charge_exists, nb::arg("charge_state"),
                 DOC(fiction_sidb_charge_distribution_charge_exists))
            .def("num_negative_sidbs", &charge_distribution::num_negative_sidbs,
                 DOC(fiction_sidb_charge_distribution_num_negative_sidbs))
            .def("num_neutral_sidbs", &charge_distribution::num_neutral_sidbs,
                 DOC(fiction_sidb_charge_distribution_num_neutral_sidbs))
            .def("num_positive_sidbs", &charge_distribution::num_positive_sidbs,
                 DOC(fiction_sidb_charge_distribution_num_positive_sidbs))
            .def("charge_index", &charge_distribution::charge_index, nb::arg("base"),
                 DOC(fiction_sidb_charge_distribution_charge_index))
            .def("same_charge_states", &charge_distribution::same_charge_states, nb::arg("other"),
                 DOC(fiction_sidb_charge_distribution_same_charge_states))
            // nanobind uses `nb::self` on both sides to declare same-type operators.
            .def(nb::self == nb::self,  // NOLINT(misc-redundant-expression)
                 DOC(fiction_sidb_charge_distribution_operator_eq))
            .def(nb::self != nb::self)  // NOLINT(misc-redundant-expression)
            .def("__len__", &charge_distribution::size)
            .def("__repr__", [](const charge_distribution& cd)
                 { return fiction::sidb::model::charge_configuration_to_string(cd.charge_states()); })

        ;

    charge_distribution_class.attr("__hash__") = nb::none();
}

}  // namespace pyfiction
