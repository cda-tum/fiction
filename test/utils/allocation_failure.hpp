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
 * @brief One-shot allocation failures for test executables with one source file.
 *
 * Include this helper once per executable, outside a shared precompiled header. The replaceable global operators
 * have external linkage and are not inline. Aligned allocation uses the platform's unmodified operators.
 */

#pragma once

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdlib>
#include <new>
#include <optional>

/** @brief Allocation-failure control for test executables. */
namespace fiction::test
{
/**
 * @brief Skips fault injection when MSVC checked STL allocates iterator proxies in noexcept constructors.
 * Call before enabling the allocation budget. Ordinary test cases run without fault injection.
 */
inline void require_allocation_failure_support()
{
#if defined(_MSC_VER) && _ITERATOR_DEBUG_LEVEL > 0
    SKIP("MSVC checked STL allocates iterator proxies in noexcept constructors; allocation failure terminates");
#endif
}
/**
 * Number of successful allocations before the test injects a failure; unset disables injection.
 */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables): Global new reads this thread-local budget.
thread_local std::optional<std::size_t> allocation_budget{};
}  // namespace fiction::test
/**
 * Allocates memory and injects a failure when the test allocation budget is exhausted.
 *
 * @param size Requested byte count.
 * @return Allocated memory.
 * @throws std::bad_alloc if allocation fails or the test exhausts its budget.
 */
void* operator new(const std::size_t size)
{
    if (fiction::test::allocation_budget.has_value())
    {
        if (*fiction::test::allocation_budget == 0)
        {
            fiction::test::allocation_budget.reset();
            throw std::bad_alloc{};
        }
        --*fiction::test::allocation_budget;
    }
    // The global new replacement must use malloc to avoid recursion.
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,hicpp-no-malloc)
    if (auto* const memory = std::malloc(size == 0 ? 1 : size))
    {
        return memory;
    }
    throw std::bad_alloc{};
}
/**
 * Releases memory allocated by the test's global allocation replacement.
 *
 * @param memory Memory to release.
 */
void operator delete(void* const memory) noexcept
{
    // Matches malloc in the global new replacement.
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,hicpp-no-malloc)
    std::free(memory);
}
/**
 * Releases a sized allocation through the matching global deallocator.
 *
 * @param memory Memory to release.
 */
void operator delete(void* const memory, std::size_t) noexcept
{
    ::operator delete(memory);
}
