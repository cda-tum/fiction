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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/is_operational.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/operational_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/types.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using is_operational_options = fiction::sidb::simulation::logic::is_operational_params;

namespace detail
{

/**
 * @brief Registers the operational checks on `sidb_layout`.
 *
 * @param m The module.
 */
inline void register_is_operational_functions(nb::module_& m)
{
    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_wire;

    using wires = std::vector<bdl_wire>;

    m.def(
        "is_operational",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            const auto params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::is_operational(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Determine whether the SiDB layout implements the supplied truth tables for every input pattern.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.
Overloads accept supplied wires or one layout per input pattern.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "is_operational",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const wires& in,
           const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)                            base,
           const decltype(is_operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(is_operational_options{}.sim_engine)                                 sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                               minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                               maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            const auto params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::is_operational(lyt, spec, params, in, out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Determine whether the SiDB layout implements the supplied truth tables for every input pattern.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.
Overloads accept supplied wires or one layout per input pattern.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    canvas_lyt: Optional canvas layout for operational analysis.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "is_operational",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<fiction::synthesis::truth_table>& spec,
           const wires& in, const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)                            base,
           const decltype(is_operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(is_operational_options{}.sim_engine)                                 sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                               minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                               maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            const auto params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::is_operational(input_pattern_layouts, spec, params, in, out,
                                                                    canvas_lyt);
        },
        nb::arg("input_pattern_layouts"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"),
        nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Determine whether the SiDB layout implements the supplied truth tables for every input pattern.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.
Overloads accept supplied wires or one layout per input pattern.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    canvas_lyt: Optional canvas layout for operational analysis.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            const auto params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Return the input patterns for which the SiDB layout implements the truth tables.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const wires& in,
           const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)                            base,
           const decltype(is_operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(is_operational_options{}.sim_engine)                                 sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                               minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                               maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            const auto params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params, in, out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Return the input patterns for which the SiDB layout implements the truth tables.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    canvas_lyt: Optional canvas layout for operational analysis.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            const auto params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Return the input patterns for which kinks render the SiDB layout non-operational.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const wires& in,
           const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)                            base,
           const decltype(is_operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(is_operational_options{}.sim_engine)                                 sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                               minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                               maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            const auto params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params, in,
                                                                                                 out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Return the input patterns for which kinks render the SiDB layout non-operational.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    canvas_lyt: Optional canvas layout for operational analysis.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            const auto params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Check whether kinks cause the SiDB layout to be non-operational.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<fiction::synthesis::truth_table>& spec, const wires& in,
           const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)                            base,
           const decltype(is_operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(is_operational_options{}.sim_engine)                                 sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                               minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                               maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            const auto params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params, in, out,
                                                                                     canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt, R"doc(Check whether kinks cause the SiDB layout to be non-operational.
The keyword options select physical values, simulation, BDL encoding, and operational analysis.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    sim_engine: SiDB simulation engine used for operational analysis.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    op_condition: Condition used to decide whether the layout is operational.
    canvas_lyt: Optional canvas layout for operational analysis.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
}

}  // namespace detail

/**
 * @brief Binds is operational helpers.
 * @param m Python module.
 */
void register_is_operational(nb::module_& m)
{
    nb::enum_<fiction::sidb::simulation::logic::operational_status>(
        m, "operational_status", DOC(fiction_sidb_simulation_logic_operational_status))
        .value("OPERATIONAL", fiction::sidb::simulation::logic::operational_status::OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::sidb::simulation::logic::operational_status::NON_OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_NON_OPERATIONAL));

    nb::enum_<is_operational_options::operational_condition>(
        m, "operational_condition", DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition))
        .value("TOLERATE_KINKS", is_operational_options::operational_condition::TOLERATE_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_TOLERATE_KINKS))
        .value("REJECT_KINKS", is_operational_options::operational_condition::REJECT_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_REJECT_KINKS));

    nb::enum_<is_operational_options::operational_analysis_strategy>(
        m, "operational_analysis_strategy",
        DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy))
        .value("SIMULATION_ONLY", is_operational_options::operational_analysis_strategy::SIMULATION_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_SIMULATION_ONLY))
        .value("FILTER_ONLY", is_operational_options::operational_analysis_strategy::FILTER_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_ONLY))
        .value(
            "FILTER_THEN_SIMULATION", is_operational_options::operational_analysis_strategy::FILTER_THEN_SIMULATION,
            DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_THEN_SIMULATION));

    detail::register_is_operational_functions(m);
}

}  // namespace pyfiction
