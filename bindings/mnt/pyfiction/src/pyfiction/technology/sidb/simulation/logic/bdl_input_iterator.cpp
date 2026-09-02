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
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <string>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void bdl_input_iterator_impl(nanobind::module_& m, const std::string& lattice)
{
    namespace py = nanobind;

    py::class_<fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>>(
        m, fmt::format("bdl_input_iterator_{}", lattice).c_str(), DOC(fiction_sidb_simulation_logic_bdl_input_iterator))
        .def(py::init<const Lyt&, const fiction::sidb::simulation::logic::bdl_input_iterator_params&>(), py::arg("lyt"),
             py::arg("params") = fiction::sidb::simulation::logic::bdl_input_iterator_params{},
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_bdl_input_iterator))
        .def(
            "__next__",
            [](fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self) -> Lyt
            {
                if (self >= ((1ull << self.num_input_pairs()) - 1))
                {
                    throw py::stop_iteration();
                }

                auto result = *self;
                ++self;
                return result;
            },
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_mul))
        .def(
            "__eq__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self == n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_eq))
        .def(
            "__ne__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self != n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_ne))
        .def(
            "__lt__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self < n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_lt))
        .def(
            "__le__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self <= n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_le))
        .def(
            "__gt__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self > n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_gt))
        .def(
            "__ge__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const uint64_t n) -> bool
            { return self >= n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_ge))
        .def(
            "__add__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self,
               const int n) -> fiction::sidb::simulation::logic::bdl_input_iterator<Lyt> { return self + n; },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_add))
        .def(
            "__iadd__",
            [](fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self,
               const int n) -> fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>&
            {
                self += n;
                return self;
            },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_iadd))
        .def(
            "__sub__", [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self, const int n)
            { return self - n; }, py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_sub))
        .def(
            "__isub__",
            [](fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self,
               const int n) -> fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>&
            {
                self -= n;
                return self;
            },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_isub))
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self,
               int n) -> fiction::sidb::simulation::logic::bdl_input_iterator<Lyt> { return self[n]; },
            py::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_array))

        .def("num_input_pairs", &fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>::num_input_pairs,
             "Returns the number of input BDL pairs the iterator was constructed with.")
        .def(
            "get_layout",
            [](const fiction::sidb::simulation::logic::bdl_input_iterator<Lyt>& self) -> const Lyt& { return *self; },
            "Returns the layout that represents the current input state, equivalent to dereferencing the "
            "iterator.")

        ;

    m.def("generate_bdl_input_pattern_layouts",
          py::overload_cast<const Lyt&, const fiction::sidb::simulation::logic::bdl_input_iterator_params&>(
              &fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts<Lyt>),
          py::arg("lyt"), py::arg("params") = fiction::sidb::simulation::logic::bdl_input_iterator_params{},
          DOC(fiction_sidb_simulation_logic_generate_bdl_input_pattern_layouts));

    m.def("generate_bdl_input_pattern_layouts",
          py::overload_cast<const Lyt&, const fiction::sidb::simulation::logic::bdl_input_iterator_params&,
                            const std::vector<fiction::sidb::simulation::logic::bdl_wire<Lyt>>&>(
              &fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts<Lyt>),
          py::arg("lyt"), py::arg("params"), py::arg("input_wires"),
          DOC(fiction_sidb_simulation_logic_generate_bdl_input_pattern_layouts_2));
}

}  // namespace detail

void bdl_input_iterator(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Input BDL configuration
     */
    py::enum_<typename fiction::sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration>(
        m, "input_bdl_configuration")
        .value(
            "PERTURBER_ABSENCE_ENCODED",
            fiction::sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::
                PERTURBER_ABSENCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED))
        .value(
            "PERTURBER_DISTANCE_ENCODED",
            fiction::sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::
                PERTURBER_DISTANCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED));

    /**
     * BDL input iterator parameters.
     */
    py::class_<fiction::sidb::simulation::logic::bdl_input_iterator_params>(
        m, "bdl_input_iterator_params", DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("bdl_wire_params", &fiction::sidb::simulation::logic::bdl_input_iterator_params::bdl_wire_params,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_bdl_wire_params))
        .def_rw("input_bdl_config", &fiction::sidb::simulation::logic::bdl_input_iterator_params::input_bdl_config,
                DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_config));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::bdl_input_iterator_impl<py_sidb_100_lattice>(m, "100");
    detail::bdl_input_iterator_impl<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction
