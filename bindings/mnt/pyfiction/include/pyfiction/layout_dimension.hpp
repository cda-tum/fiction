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
 * @brief Validation and construction helpers for Python layout dimensions.
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include <stdexcept>
#include <utility>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * Validates a maximum coordinate for a layout whose minimum is the origin.
 *
 * @tparam Coordinate Coordinate type.
 * @param maximum Maximum coordinate.
 * @return The validated maximum coordinate.
 * @throws std::invalid_argument If the maximum is below the origin in any dimension.
 */
template <typename Coordinate>
[[nodiscard]] Coordinate validate_layout_maximum(const Coordinate& maximum)
{
    const auto origin = Coordinate{0, 0, 0};
    if (origin.x > maximum.x || origin.y > maximum.y || origin.z > maximum.z)
    {
        throw std::invalid_argument("Layout maximum must not be below the origin");
    }

    return maximum;
}

/**
 * Validates inclusive layout bounds.
 *
 * @tparam Coordinate Coordinate type.
 * @param bounds Minimum and maximum coordinates.
 * @throws std::invalid_argument If the minimum exceeds the maximum in any dimension.
 */
template <typename Coordinate>
void validate_layout_bounds(const std::pair<Coordinate, Coordinate>& bounds)
{
    const auto& [minimum, maximum] = bounds;
    if (minimum.x > maximum.x || minimum.y > maximum.y || minimum.z > maximum.z)
    {
        throw std::invalid_argument("Layout minimum must not exceed its maximum");
    }
}

/**
 * Constructs a layout with inclusive minimum and maximum coordinates.
 *
 * @tparam Layout Layout type.
 * @tparam Coordinate Coordinate type.
 * @tparam Args Constructor argument types.
 * @param self Uninitialized Python layout instance.
 * @param bounds Minimum and maximum coordinates.
 * @param args Additional constructor arguments.
 */
template <typename Layout, typename Coordinate, typename... Args>
void construct_layout_from_bounds(nanobind::pointer_and_handle<Layout>     self,
                                  const std::pair<Coordinate, Coordinate>& bounds, const Args&... args)
{
    validate_layout_bounds(bounds);
    new (self.p) Layout{Coordinate{0, 0, 0}, args...};
    self.p->resize(bounds.first, bounds.second);
}

}  // namespace pyfiction
