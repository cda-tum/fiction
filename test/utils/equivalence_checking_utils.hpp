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
 * @brief Catch2 assertion that a specification and an implementation are equivalent.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <catch2/catch_test_macros.hpp>

#include <fiction/verification/equivalence_checking.hpp>

template <typename Spec, typename Impl>
void check_eq(const Spec& spec, const Impl& impl)
{
    CHECK(fiction::verification::equivalence_checking(spec, impl) != fiction::verification::eq_type::NO);
}
