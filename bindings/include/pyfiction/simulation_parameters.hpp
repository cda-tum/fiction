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

#include <nanobind/nanobind.h>

/** @brief Namespace alias for nanobind. */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Construct physical values after validating the charge-state base.
 * @param base Number of charge states, either 2 or 3.
 * @param mu_minus Negative-charge transition energy in eV.
 * @param epsilon_r Relative permittivity.
 * @param lambda_tf Screening distance in nm.
 * @return Validated native physical simulation values.
 * @throws nb::value_error If base is neither 2 nor 3.
 */
inline fiction::sidb::model::simulation_parameters
make_simulation_parameters(const uint8_t base, const double mu_minus, const double epsilon_r, const double lambda_tf)
{
    if (base != 2 && base != 3)
    {
        throw nb::value_error("base must be 2 or 3");
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

}  // namespace pyfiction
