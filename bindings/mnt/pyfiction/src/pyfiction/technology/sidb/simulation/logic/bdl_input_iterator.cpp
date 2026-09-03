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

void bdl_input_iterator(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_input_iterator;
    using fiction::sidb::simulation::logic::bdl_input_iterator_params;
    using fiction::sidb::simulation::logic::bdl_wire;

    py::enum_<bdl_input_iterator_params::input_bdl_configuration>(m, "input_bdl_configuration")
        .value(
            "PERTURBER_ABSENCE_ENCODED", bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED))
        .value(
            "PERTURBER_DISTANCE_ENCODED",
            bdl_input_iterator_params::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED));

    py::class_<bdl_input_iterator_params>(m, "bdl_input_iterator_params",
                                          DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("bdl_wire_params", &bdl_input_iterator_params::bdl_wire_params,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_bdl_wire_params))
        .def_rw("input_bdl_config", &bdl_input_iterator_params::input_bdl_config,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_config));

    py::class_<bdl_input_iterator>(m, "bdl_input_iterator", DOC(fiction_sidb_simulation_logic_bdl_input_iterator))
        .def(py::init<const layout&, const bdl_input_iterator_params&>(), py::arg("lyt"),
             py::arg("params") = bdl_input_iterator_params{},
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_bdl_input_iterator))
        .def(py::init<const layout&, const bdl_input_iterator_params&, const std::vector<bdl_wire>&>(), py::arg("lyt"),
             py::arg("params"), py::arg("input_wires"),
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_bdl_input_iterator_2))
        .def(
            "__next__",
            [](bdl_input_iterator& self) -> layout
            {
                if (self >= ((uint64_t{1} << self.num_input_pairs()) - 1))
                {
                    throw py::stop_iteration();
                }

                auto result = *self;
                ++self;

                return result;
            },
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_mul))
        .def(
            "__eq__", [](const bdl_input_iterator& self, const uint64_t n) { return self == n; }, py::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_eq))
        .def(
            "__ne__", [](const bdl_input_iterator& self, const uint64_t n) { return self != n; }, py::arg("m"))
        .def(
            "__lt__", [](const bdl_input_iterator& self, const uint64_t n) { return self < n; }, py::arg("m"))
        .def(
            "__le__", [](const bdl_input_iterator& self, const uint64_t n) { return self <= n; }, py::arg("m"))
        .def(
            "__gt__", [](const bdl_input_iterator& self, const uint64_t n) { return self > n; }, py::arg("m"))
        .def(
            "__ge__", [](const bdl_input_iterator& self, const uint64_t n) { return self >= n; }, py::arg("m"))
        .def(
            "__add__", [](const bdl_input_iterator& self, const int n) { return self + n; }, py::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_add))
        .def(
            "__iadd__",
            [](bdl_input_iterator& self, const int n) -> bdl_input_iterator&
            {
                self += n;
                return self;
            },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_iadd))
        .def(
            "__sub__", [](const bdl_input_iterator& self, const int n) { return self - n; }, py::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_sub))
        .def(
            "__isub__",
            [](bdl_input_iterator& self, const int n) -> bdl_input_iterator&
            {
                self -= n;
                return self;
            },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_isub))
        .def(
            "__getitem__", [](const bdl_input_iterator& self, const int n) { return self[n]; }, py::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_array))
        .def("num_input_pairs", &bdl_input_iterator::num_input_pairs,
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_num_input_pairs))
        .def("get_current_input_index", &bdl_input_iterator::get_current_input_index,
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_get_current_input_index))
        .def(
            "get_layout", [](const bdl_input_iterator& self) -> const layout& { return *self; },
            "Returns the layout that represents the current input state, equivalent to dereferencing the iterator.");

    m.def(
        "generate_bdl_input_pattern_layouts", [](const layout& lyt, const bdl_input_iterator_params& params)
        { return fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(lyt, params); }, py::arg("lyt"),
        py::arg("params") = bdl_input_iterator_params{},
        DOC(fiction_sidb_simulation_logic_generate_bdl_input_pattern_layouts_2));
    m.def(
        "generate_bdl_input_pattern_layouts",
        [](const layout& lyt, const bdl_input_iterator_params& params, const std::vector<bdl_wire>& input_wires)
        { return fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(lyt, params, input_wires); },
        py::arg("lyt"), py::arg("params"), py::arg("input_wires"),
        DOC(fiction_sidb_simulation_logic_generate_bdl_input_pattern_layouts));
}

}  // namespace pyfiction
