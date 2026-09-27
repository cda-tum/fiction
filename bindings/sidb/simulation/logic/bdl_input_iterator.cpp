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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner): converts caller-supplied BDL wires.

namespace pyfiction
{

/**
 * @brief Registers input encoding options and a private Python iterator.
 * @param m Python module.
 */
void bdl_input_iterator(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_input_iterator;
    using fiction::sidb::simulation::logic::bdl_input_iterator_params;
    using fiction::sidb::simulation::logic::bdl_wire;

    py::enum_<bdl_input_iterator_params::input_bdl_configuration>(m, "InputEncoding")
        .value(
            "PERTURBER_ABSENCE_ENCODED", bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED))
        .value(
            "PERTURBER_DISTANCE_ENCODED",
            bdl_input_iterator_params::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED));

    py::class_<bdl_input_iterator_params>(m, "InputPatternParams",
                                          DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("bdl_wire_params", &bdl_input_iterator_params::bdl_wire_params,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_bdl_wire_params))
        .def_rw("input_bdl_config", &bdl_input_iterator_params::input_bdl_config,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_config));

    py::class_<bdl_input_iterator>(m, "_InputPatterns", DOC(fiction_sidb_simulation_logic_bdl_input_iterator))
        .def(py::init<const layout&, const bdl_input_iterator_params&>(), py::arg("lyt"),
             py::arg("params") = bdl_input_iterator_params{},
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_bdl_input_iterator))
        .def(
            "__iter__", [](bdl_input_iterator& self) -> bdl_input_iterator& { return self; },
            py::rv_policy::reference_internal)
        .def(py::init<const layout&, const bdl_input_iterator_params&, const std::vector<bdl_wire>&>(), py::arg("lyt"),
             py::arg("params"), py::arg("input_wires"),
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_bdl_input_iterator_2))
        .def(
            "__next__",
            [](bdl_input_iterator& self) -> layout
            {
                if (self >= (uint64_t{1} << self.num_input_pairs()))
                {
                    throw py::stop_iteration();
                }

                auto result = *self;
                ++self;

                return result;
            },
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_mul))
        .def("is_valid", &bdl_input_iterator::is_valid, DOC(fiction_sidb_simulation_logic_bdl_input_iterator_is_valid));
}

}  // namespace pyfiction
