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
 * @brief Shared monotonic deadlines for nested algorithms.
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace fiction::utils
{

/**
 * An algorithm reached its execution deadline without producing a complete result.
 */
class timeout_error : public std::runtime_error
{
  public:
    /**
     * @brief Constructs an execution-timeout error.
     */
    timeout_error() : std::runtime_error{"Execution deadline exceeded"} {}
};

/**
 * @brief Starts a millisecond budget without extending an enclosing deadline.
 *
 * @param timeout Milliseconds from now; the maximum value means unlimited.
 * @param enclosing Deadline shared with an enclosing algorithm.
 * @return The earlier deadline, saturated at the clock's maximum time point.
 */
[[nodiscard]] inline std::chrono::steady_clock::time_point
make_deadline(const uint64_t                              timeout,
              const std::chrono::steady_clock::time_point enclosing = std::chrono::steady_clock::time_point::max())
{
    if (timeout == std::numeric_limits<uint64_t>::max())
    {
        return enclosing;
    }
    const auto maximum =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::duration::max());
    if (std::cmp_greater(timeout, maximum.count()))
    {
        return enclosing;
    }
    const auto budget =
        std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::milliseconds{timeout});
    const auto now = std::chrono::steady_clock::now();
    if (now > std::chrono::steady_clock::time_point::max() - budget)
    {
        return enclosing;
    }
    return std::min(enclosing, now + budget);
}

/**
 * @brief Checks a shared monotonic deadline.
 *
 * @param deadline Expiration time; the maximum time point means unlimited.
 * @throws timeout_error if the deadline has been reached.
 */
inline void check_deadline(const std::chrono::steady_clock::time_point deadline)
{
    if (deadline != std::chrono::steady_clock::time_point::max() && std::chrono::steady_clock::now() >= deadline)
    {
        throw timeout_error{};
    }
}

}  // namespace fiction::utils
