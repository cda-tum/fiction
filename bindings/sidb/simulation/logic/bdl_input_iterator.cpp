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

#include "pyfiction/bdl_parameters.hpp"
#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <cstdint>
#include <memory>
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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using bdl_input_iterator_options = fiction::sidb::simulation::logic::bdl_input_iterator_params;

/**
 * @brief Binds bdl input iterator helpers.
 * @param m Python module.
 */
void register_bdl_input_iterator(nb::module_& m)
{
    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_input_iterator;

    using fiction::sidb::simulation::logic::bdl_wire;

    nb::enum_<bdl_input_iterator_options::input_bdl_configuration>(m, "input_bdl_configuration")
        .value(
            "PERTURBER_ABSENCE_ENCODED", bdl_input_iterator_options::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_ABSENCE_ENCODED))
        .value(
            "PERTURBER_DISTANCE_ENCODED",
            bdl_input_iterator_options::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED,
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_params_input_bdl_configuration_PERTURBER_DISTANCE_ENCODED));

    nb::class_<bdl_input_iterator>(m, "bdl_input_iterator", DOC(fiction_sidb_simulation_logic_bdl_input_iterator))
        .def(
            "__init__",
            [](bdl_input_iterator* self, const layout& lyt,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.threshold_bdl_interdistance)       threshold_bdl_interdistance,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.bdl_pairs_params.minimum_distance) minimum_distance,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.bdl_pairs_params.maximum_distance) maximum_distance,
               const decltype(bdl_input_iterator_options{}.input_bdl_config)          input_bdl_config)
            {
                validate_bdl_pair_distances(minimum_distance, maximum_distance);
                const bdl_input_iterator_options params{
                    {.threshold_bdl_interdistance = threshold_bdl_interdistance,
                     .bdl_pairs_params = {.minimum_distance = minimum_distance, .maximum_distance = maximum_distance}},
                    input_bdl_config};
                std::construct_at(self, lyt, params);
            },
            nb::arg("lyt"), nb::kw_only(),
            nb::arg("threshold_bdl_interdistance") =
                bdl_input_iterator_options{}.bdl_wire_params.threshold_bdl_interdistance,
            nb::arg("minimum_distance") =
                bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.minimum_distance,
            nb::arg("maximum_distance") =
                bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.maximum_distance,
            nb::arg("input_bdl_config") = bdl_input_iterator_options{}.input_bdl_config,
            R"doc(Iterate over SiDB input patterns using the selected BDL encoding.

Args:
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
)doc")
        .def(
            "__iter__", [](bdl_input_iterator& self) -> bdl_input_iterator& { return self; },
            nb::rv_policy::reference_internal)
        .def(
            "__init__",
            [](bdl_input_iterator* self, const layout& lyt, const std::vector<bdl_wire>& input_wires,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.threshold_bdl_interdistance)       threshold_bdl_interdistance,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.bdl_pairs_params.minimum_distance) minimum_distance,
               const decltype(bdl_input_iterator_options{}
                                  .bdl_wire_params.bdl_pairs_params.maximum_distance) maximum_distance,
               const decltype(bdl_input_iterator_options{}.input_bdl_config)          input_bdl_config)
            {
                validate_bdl_pair_distances(minimum_distance, maximum_distance);
                const bdl_input_iterator_options params{
                    {.threshold_bdl_interdistance = threshold_bdl_interdistance,
                     .bdl_pairs_params = {.minimum_distance = minimum_distance, .maximum_distance = maximum_distance}},
                    input_bdl_config};
                std::construct_at(self, lyt, params, input_wires);
            },
            nb::arg("lyt"), nb::arg("input_wires"), nb::kw_only(),
            nb::arg("threshold_bdl_interdistance") =
                bdl_input_iterator_options{}.bdl_wire_params.threshold_bdl_interdistance,
            nb::arg("minimum_distance") =
                bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.minimum_distance,
            nb::arg("maximum_distance") =
                bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.maximum_distance,
            nb::arg("input_bdl_config") = bdl_input_iterator_options{}.input_bdl_config,
            R"doc(Iterate over SiDB input patterns using the selected BDL encoding.

Args:
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
)doc")
        .def(
            "__next__",
            [](bdl_input_iterator& self) -> layout
            {
                if (self >= (uint64_t{1} << self.num_input_pairs()))
                {
                    throw nb::stop_iteration();
                }

                auto result = *self;
                ++self;

                return result;
            },
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_mul))
        .def(
            "__eq__", [](const bdl_input_iterator& self, const uint64_t n) { return self == n; }, nb::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_eq))
        .def(
            "__ne__", [](const bdl_input_iterator& self, const uint64_t n) { return self != n; }, nb::arg("m"))
        .def(
            "__lt__", [](const bdl_input_iterator& self, const uint64_t n) { return self < n; }, nb::arg("m"))
        .def(
            "__le__", [](const bdl_input_iterator& self, const uint64_t n) { return self <= n; }, nb::arg("m"))
        .def(
            "__gt__", [](const bdl_input_iterator& self, const uint64_t n) { return self > n; }, nb::arg("m"))
        .def(
            "__ge__", [](const bdl_input_iterator& self, const uint64_t n) { return self >= n; }, nb::arg("m"))
        .def(
            "__add__", [](const bdl_input_iterator& self, const int n) { return self + n; }, nb::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_add))
        .def(
            "__iadd__",
            [](bdl_input_iterator& self, const int n) -> bdl_input_iterator&
            {
                self += n;
                return self;
            },
            nb::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_iadd))
        .def(
            "__sub__", [](const bdl_input_iterator& self, const int n) { return self - n; }, nb::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_sub))
        .def(
            "__isub__",
            [](bdl_input_iterator& self, const int n) -> bdl_input_iterator&
            {
                self -= n;
                return self;
            },
            nb::arg("m"), DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_isub))
        .def(
            "__getitem__", [](const bdl_input_iterator& self, const int n) { return self[n]; }, nb::arg("m"),
            DOC(fiction_sidb_simulation_logic_bdl_input_iterator_operator_array))
        .def("num_input_pairs", &bdl_input_iterator::num_input_pairs,
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_num_input_pairs))
        .def("get_current_input_index", &bdl_input_iterator::get_current_input_index,
             DOC(fiction_sidb_simulation_logic_bdl_input_iterator_get_current_input_index))
        .def(
            "get_layout", [](const bdl_input_iterator& self) -> const layout& { return *self; },
            "Returns the layout that represents the current input state, equivalent to dereferencing the iterator.");

    m.def(
        "generate_bdl_input_pattern_layouts",
        [](const layout&                                                          lyt,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.threshold_bdl_interdistance)       threshold_bdl_interdistance,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.bdl_pairs_params.minimum_distance) minimum_distance,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.bdl_pairs_params.maximum_distance) maximum_distance,
           const decltype(bdl_input_iterator_options{}.input_bdl_config)          input_bdl_config)
        {
            validate_bdl_pair_distances(minimum_distance, maximum_distance);
            const bdl_input_iterator_options params{
                {.threshold_bdl_interdistance = threshold_bdl_interdistance,
                 .bdl_pairs_params = {.minimum_distance = minimum_distance, .maximum_distance = maximum_distance}},
                input_bdl_config};
            return fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(),
        nb::arg("threshold_bdl_interdistance") =
            bdl_input_iterator_options{}.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") = bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") = bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = bdl_input_iterator_options{}.input_bdl_config,
        R"doc(Return one SiDB layout per input pattern, starting with pattern zero.
The keyword options select BDL detection and input encoding.
The overload with input_wires bypasses automatic wire detection.

Args:
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
)doc");
    m.def(
        "generate_bdl_input_pattern_layouts",
        [](const layout& lyt, const std::vector<bdl_wire>& input_wires,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.threshold_bdl_interdistance)       threshold_bdl_interdistance,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.bdl_pairs_params.minimum_distance) minimum_distance,
           const decltype(bdl_input_iterator_options{}
                              .bdl_wire_params.bdl_pairs_params.maximum_distance) maximum_distance,
           const decltype(bdl_input_iterator_options{}.input_bdl_config)          input_bdl_config)
        {
            validate_bdl_pair_distances(minimum_distance, maximum_distance);
            const bdl_input_iterator_options params{
                {.threshold_bdl_interdistance = threshold_bdl_interdistance,
                 .bdl_pairs_params = {.minimum_distance = minimum_distance, .maximum_distance = maximum_distance}},
                input_bdl_config};
            return fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(lyt, params, input_wires);
        },
        nb::arg("lyt"), nb::arg("input_wires"), nb::kw_only(),
        nb::arg("threshold_bdl_interdistance") =
            bdl_input_iterator_options{}.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") = bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") = bdl_input_iterator_options{}.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = bdl_input_iterator_options{}.input_bdl_config,
        R"doc(Return one SiDB layout per input pattern, starting with pattern zero.
The keyword options select BDL detection and input encoding.
The overload with input_wires bypasses automatic wire detection.

Args:
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
)doc");
}

}  // namespace pyfiction
