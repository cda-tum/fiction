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
 * @brief Tests for `fiction/technology/sidb/skeleton_bestagon_library.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/skeleton_bestagon_library.hpp>
#include <fiction/traits.hpp>

#include <type_traits>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("Bestagon traits", "[sidb-dynamic-gate-library]")
{
    CHECK(std::is_same_v<skeleton_bestagon_library::layout, sidb::layout>);
    CHECK(has_get_functional_implementations_v<skeleton_bestagon_library>);
    CHECK(has_get_gate_ports_v<skeleton_bestagon_library>);
}
