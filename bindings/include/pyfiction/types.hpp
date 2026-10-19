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
 * @brief Checked layout conversions for Python bindings.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <fiction/layouts/layout_base.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * Converts a Python integer to a signed coordinate axis.
 *
 * @param value Python integer.
 * @return The axis value.
 * @throws std::overflow_error If `value` does not lie in \f$[-2^{31}, 2^{31} - 1]\f$.
 */
inline int32_t coordinate_axis(const int64_t value)
{
    if (value < std::numeric_limits<int32_t>::min() || value > std::numeric_limits<int32_t>::max())
    {
        throw std::overflow_error("A coordinate axis must lie between -2147483648 and 2147483647");
    }

    return static_cast<int32_t>(value);
}
}  // namespace pyfiction

namespace nanobind::detail
{

/**
 * @brief Describes coordinate tuple inputs while retaining the registered class caster.
 */
template <>
struct type_caster<fiction::layouts::layout_base::coordinate>
        : type_caster_base<fiction::layouts::layout_base::coordinate>
{
    /**
     * @brief Accepted Python inputs and the concrete Python return type.
     */
    // NOLINTNEXTLINE(readability-identifier-naming): nanobind requires the member name Name.
    static constexpr auto Name = const_name<fiction::layouts::layout_base::coordinate>() +
                                 io_name(" | tuple[int, int] | tuple[int, int, int]", "");
};

/**
 * @brief Describes size tuple inputs while retaining the registered class caster.
 */
template <>
struct type_caster<fiction::layouts::layout_base::extent> : type_caster_base<fiction::layouts::layout_base::extent>
{
    /** @brief Accepted Python inputs and the concrete return type. */
    // NOLINTNEXTLINE(readability-identifier-naming): nanobind requires the member name Name.
    static constexpr auto Name =
        const_name<fiction::layouts::layout_base::extent>() + io_name(" | tuple[int, int] | tuple[int, int, int]", "");
};

/**
 * @brief Keeps constructor self arguments as a single registered class.
 *
 * nanobind recognizes constructors by the exact self descriptor, without an input union.
 * @tparam Coordinate Registered value type constructed by the Python call.
 */
template <typename Coordinate>
struct coordinate_constructor_caster
{
    NB_TYPE_CASTER(nb::pointer_and_handle<Coordinate>, const_name<Coordinate>())

    /**
     * @brief Converts the constructor self argument, including uninitialized instances.
     * @param src Python instance being constructed.
     * @param flags Constructor conversion flags.
     * @param cleanup Temporary conversion storage.
     * @return Whether the instance belongs to the registered class.
     */
    bool from_python(nb::handle src, std::uint32_t flags, cleanup_list* cleanup) noexcept
    {
        // Keep the GIL: the registered-class caster inspects Python instances.
        type_caster_base<Coordinate> caster;
        if (!caster.from_python(src, flags, cleanup))
        {
            return false;
        }
        value = {caster.operator Coordinate*(), src};
        return true;
    }
};

/**
 * @brief Preserves coordinate constructor dispatch.
 */
template <>
struct type_caster<nb::pointer_and_handle<fiction::layouts::layout_base::coordinate>>
        : coordinate_constructor_caster<fiction::layouts::layout_base::coordinate>
{};

/** @brief Preserves extent constructor dispatch. */
template <>
struct type_caster<nb::pointer_and_handle<fiction::layouts::layout_base::extent>>
        : coordinate_constructor_caster<fiction::layouts::layout_base::extent>
{};

}  // namespace nanobind::detail
