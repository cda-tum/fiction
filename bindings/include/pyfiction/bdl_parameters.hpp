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
 * @brief Validated BDL pair distance bounds for Python bindings.
 */

#pragma once

#include <nanobind/nanobind.h>

/** @brief Namespace alias for nanobind. */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Validate BDL pair distance bounds before native detection.
 * @param minimum_distance Minimum pair separation in nm.
 * @param maximum_distance Maximum pair separation in nm.
 * @throws nb::value_error If either bound is NaN or the interval is reversed.
 */
inline void validate_bdl_pair_distances(const double minimum_distance, const double maximum_distance)
{
    if (!(minimum_distance <= maximum_distance))
    {
        throw nb::value_error("minimum_distance must not exceed maximum_distance and neither may be NaN");
    }
}

}  // namespace pyfiction
