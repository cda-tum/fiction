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
 * @brief Inverts the screened Coulomb potential to obtain a distance from a potential.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/model/physical_constants.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace fiction::sidb::model
{

/**
 * The electrostatic potential on hydrogen-passivated silicon is typically modeled using a screened Coulomb potential.
 * This electrostatic potential is commonly employed to determine the electrostatic potential for a given distance
 * (between SiDB and point under consideration) and given physical parameters. However, the function provided here
 * serves the inverse purpose by calculating the distance for a given potential and given physical parameters.
 *
 * The search brackets the distance and bisects the bracket. The requested decimal precision is limited by
 * floating-point resolution.
 *
 * @param params The physical parameters for a given hydrogen-passivated silicon surface.
 * @param potential The electrostatic potential (unit: V) to be converted to a distance.
 * @param precision The precision level for the conversion, specifying the number of decimal places.
 * @return The distance (unit: nm) corresponding to the given electrostatic potential.
 * @throws std::invalid_argument If the potential is NaN or nonpositive, the permittivity or screening distance is
 * nonpositive or non-finite, or the precision yields a zero step.
 * @throws std::overflow_error If the distance exceeds the finite floating-point range.
 */
[[nodiscard]] inline double potential_to_distance_conversion(
    const double potential, const sidb::model::simulation_parameters& params = sidb::model::simulation_parameters{},
    const uint64_t precision = 2)
{
    if (std::isnan(potential) || potential <= 0 || !std::isfinite(params.epsilon_r) || params.epsilon_r <= 0 ||
        !std::isfinite(params.lambda_tf) || params.lambda_tf <= 0)
    {
        throw std::invalid_argument(
            "potential, epsilon_r, and lambda_tf must be positive; physical values must be finite");
    }
    /** @brief Logarithmic Coulomb scale avoids overflow for small positive permittivities. */
    const auto log_scale = std::log(model::K_E * model::ELEMENTARY_CHARGE * 1e9) - std::log(params.epsilon_r);
    /** @brief Logarithmic target potential. */
    const auto log_potential = std::log(potential);
    /** @brief Compare potentials without underflow at large distances. */
    const auto exceeds_potential = [&params, log_scale, log_potential](const double distance) noexcept
    { return log_scale - std::log(distance) - distance / params.lambda_tf > log_potential; };

    // calculate the step size based on the precision
    const double step_size = std::pow(10, -static_cast<double>(precision));
    if (step_size == 0)
    {
        throw std::invalid_argument("precision must yield a positive finite step");
    }

    /** @brief Lower distance bound. */
    double lower{};
    /** @brief Upper distance bound. */
    double upper{step_size};
    while (exceeds_potential(upper))
    {
        lower = upper;
        upper = std::min(upper, std::numeric_limits<double>::max() / 2) * 2;
        if (upper == lower)
        {
            throw std::overflow_error("distance exceeds the floating-point range");
        }
    }

    while (upper - lower > step_size)
    {
        /** @brief Interior distance, when floating-point resolution permits one. */
        const auto middle = std::midpoint(lower, upper);
        if (middle == lower || middle == upper)
        {
            break;
        }
        if (exceeds_potential(middle))
        {
            lower = middle;
        }
        else
        {
            upper = middle;
        }
    }

    /** @brief Number of complete decimal steps at the upper bound. */
    const auto steps = std::floor(upper / step_size);
    if (!std::isfinite(steps))
    {
        return upper;
    }
    /** @brief Decimal-grid distance, rounded up when the potential still exceeds the target. */
    const auto distance = steps * step_size;
    return exceeds_potential(distance) ? distance + step_size : distance;
}

}  // namespace fiction::sidb::model
