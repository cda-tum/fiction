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
 * @brief Validated physical simulation values for Python bindings.
 */

#pragma once

#include <fiction/technology/sidb/model/simulation_parameters.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

#include <nanobind/nanobind.h>

/** @brief Namespace alias for nanobind. */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Physical simulation model shared by the keyword adapters. */
using simulation_model = fiction::sidb::model::simulation_parameters;

/**
 * @brief Construct physical values after validating the charge-state base and screening values.
 * @param base Number of charge states, either 2 or 3.
 * @param mu_minus Negative-charge transition energy in eV.
 * @param epsilon_r Relative permittivity.
 * @param lambda_tf Screening distance in nm.
 * @return Validated native physical simulation values.
 * @throws nb::value_error If base is neither 2 nor 3, or a screening value is nonpositive or non-finite.
 */
inline fiction::sidb::model::simulation_parameters
make_simulation_parameters(const uint8_t base, const double mu_minus, const double epsilon_r, const double lambda_tf)
{
    if (base != 2 && base != 3)
    {
        throw nb::value_error("base must be 2 or 3");
    }
    if (!std::isfinite(epsilon_r) || epsilon_r <= 0)
    {
        throw nb::value_error("epsilon_r must be positive and finite");
    }
    if (!std::isfinite(lambda_tf) || lambda_tf <= 0)
    {
        throw nb::value_error("lambda_tf must be positive and finite");
    }
    return fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
}

/**
 * @brief Validate that the native distance scan has a positive finite increment.
 * @param precision Number of decimal places.
 * @throws nb::value_error If the distance increment is zero or non-finite.
 */
inline void validate_distance_conversion_precision(const uint64_t precision)
{
    /** @brief Distance increment used by the native scan. */
    const auto step = std::pow(10, -static_cast<double>(precision));
    if (step == 0 || !std::isfinite(step))
    {
        throw nb::value_error("precision must yield a positive finite step");
    }
}

/**
 * @brief Validate a probability used by analysis algorithms.
 * @param confidence_level Required success probability.
 * @throws nb::value_error If the confidence is outside the open unit interval.
 */
inline void validate_confidence_level(const double confidence_level)
{
    if (!(confidence_level > 0 && confidence_level < 1))
    {
        throw nb::value_error("confidence_level must be strictly between 0 and 1");
    }
}

/**
 * @brief Validate the temperature range used by the native centikelvin loop.
 * @param max_temperature Maximum temperature in K.
 * @throws nb::value_error If the temperature is negative, non-finite, or exceeds the loop's range.
 */
inline void validate_max_temperature(const double max_temperature)
{
    if (!(max_temperature >= 0 && max_temperature < static_cast<double>(std::numeric_limits<uint64_t>::max()) / 100))
    {
        throw nb::value_error("max_temperature must be nonnegative and fit the centikelvin iteration range");
    }
}

}  // namespace pyfiction
