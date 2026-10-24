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
 * @brief Catch2 assertions that a specification and an implementation are equivalent.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/verification/equivalence_checking.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>

template <typename Spec, typename Impl>
void check_eq(const Spec& spec, const Impl& impl)
{
    CHECK(fiction::verification::equivalence_checking(spec, impl) != fiction::verification::eq_type::NO);
}

/**
 * Checks by SAT on the `virtual_miter` that the implementation computes the same functions as the specification,
 * virtual primary inputs included.
 */
template <typename Spec, typename Impl>
void check_virtual_eq(const Spec& spec, const Impl& impl)
{
    const auto miter = fiction::verification::virtual_miter<fiction::networks::technology_network>(spec, impl);
    REQUIRE(miter.has_value());

    if (miter.has_value())  // the optional-access check does not model REQUIRE
    {
        mockturtle::equivalence_checking_stats st{};
        const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);

        REQUIRE(cec.has_value());
        CHECK(cec.value_or(false));
    }
}
