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
 * @brief Native operational options constructed from Python keyword values.
 */

#pragma once

#include "pyfiction/bdl_parameters.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>

#include <cstdint>

namespace pyfiction
{

/** @brief Native operational options filled for each call. */
using operational_options = fiction::sidb::simulation::logic::is_operational_params;

/**
 * @brief Construct shared physical, BDL, and operational options.
 * @param timeout Execution budget in milliseconds.
 * @param base Number of charge states, either 2 or 3.
 * @param mu_minus Negative-charge transition energy in eV.
 * @param epsilon_r Relative permittivity.
 * @param lambda_tf Screening distance in nm.
 * @param sim_engine Simulation engine.
 * @param threshold_bdl_interdistance Maximum inter-pair distance in nm.
 * @param minimum_distance Minimum BDL pair distance in nm.
 * @param maximum_distance Maximum BDL pair distance in nm.
 * @param input_bdl_config Input encoding.
 * @param op_condition Operational condition.
 * @param strategy_to_analyze_operational_status Operational analysis strategy.
 * @return Fresh native operational options with the supplied values.
 * @throws nb::value_error If base is neither 2 nor 3, or BDL bounds are NaN or reversed.
 */
inline operational_options make_operational_parameters(
    const uint64_t timeout, const uint8_t base, const double mu_minus, const double epsilon_r, const double lambda_tf,
    const decltype(operational_options{}.sim_engine) sim_engine, const double threshold_bdl_interdistance,
    const double minimum_distance, const double maximum_distance,
    const decltype(operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
    const decltype(operational_options{}.op_condition)                               op_condition,
    const decltype(operational_options{}.strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
{
    validate_bdl_pair_distances(minimum_distance, maximum_distance);
    return operational_options{
        .sim_params                = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf),
        .sim_engine                = sim_engine,
        .input_bdl_iterator_params = {.bdl_wire_params  = {.threshold_bdl_interdistance = threshold_bdl_interdistance,
                                                           .bdl_pairs_params = {.minimum_distance = minimum_distance,
                                                                                .maximum_distance = maximum_distance}},
                                      .input_bdl_config = input_bdl_config},
        .op_condition              = op_condition,
        .strategy_to_analyze_operational_status = strategy_to_analyze_operational_status,
        .timeout                                = timeout};
}

}  // namespace pyfiction
