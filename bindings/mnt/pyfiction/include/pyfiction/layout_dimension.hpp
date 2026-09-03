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
 * @brief Parsing of Python layout dimensions.
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include <optional>
#include <stdexcept>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * A parsed maximum coordinate with an optional explicit minimum coordinate.
 *
 * @tparam Coordinate Coordinate type.
 */
template <typename Coordinate>
struct parsed_layout_dimension
{
    /** Maximum coordinate. */
    Coordinate maximum;
    /** Explicit minimum coordinate, if present. */
    std::optional<Coordinate> minimum;
};

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
 * Parses a flat maximum coordinate or a nested pair of minimum and maximum coordinates.
 *
 * @tparam Coordinate Coordinate type.
 * @tparam AllowExplicitMinimum Whether an explicit minimum coordinate is supported.
 * @param dimension Python coordinate or tuple.
 * @return Parsed layout dimension.
 * @throws std::invalid_argument If the bounds are malformed, unsupported, or reversed.
 */
template <typename Coordinate, bool AllowExplicitMinimum = true>
[[nodiscard]] parsed_layout_dimension<Coordinate> parse_layout_dimension(const nanobind::handle& dimension)
{
    namespace py = nanobind;

    if (!py::isinstance<py::tuple>(dimension))
    {
        return {validate_layout_maximum(py::cast<Coordinate>(dimension, true)), std::nullopt};
    }

    const auto tuple           = py::cast<py::tuple>(dimension);
    const auto first_is_tuple  = tuple.size() > 0 && py::isinstance<py::tuple>(tuple[0]);
    const auto second_is_tuple = tuple.size() > 1 && py::isinstance<py::tuple>(tuple[1]);

    if (!first_is_tuple && !second_is_tuple)
    {
        return {validate_layout_maximum(py::cast<Coordinate>(tuple, true)), std::nullopt};
    }

    if (tuple.size() != 2 || !first_is_tuple || !second_is_tuple)
    {
        throw std::invalid_argument("Layout bounds must be a pair of coordinate tuples");
    }
    if constexpr (!AllowExplicitMinimum)
    {
        throw std::invalid_argument("Explicit minimum coordinates are not supported by this layout");
    }

    const auto minimum = py::cast<Coordinate>(tuple[0], true);
    const auto maximum = py::cast<Coordinate>(tuple[1], true);
    if (minimum.x > maximum.x || minimum.y > maximum.y || minimum.z > maximum.z)
    {
        throw std::invalid_argument("Layout minimum must not exceed its maximum");
    }

    return {maximum, minimum};
}

}  // namespace pyfiction
