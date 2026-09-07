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
 * @brief Tests for SiDB simulation engine name lookup.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/simulation/engine.hpp>

#include <string>

using namespace fiction;
using namespace fiction::sidb::simulation;

TEST_CASE("Look up SiDB simulation engines by case-insensitive name", "[sidb-simulation-engine]")
{
    CHECK(get_engine("eXgS") == engine::EXGS);
    CHECK(get_engine("quickexact") == engine::QUICKEXACT);
    CHECK(get_engine("QUICKSIM") == engine::QUICKSIM);
#if (FICTION_ALGLIB_ENABLED)
    CHECK(get_engine("clustercomplete") == engine::CLUSTERCOMPLETE);
#else
    CHECK_FALSE(get_engine("clustercomplete").has_value());
#endif
    CHECK_FALSE(get_engine("").has_value());
    CHECK_FALSE(get_engine("unknown").has_value());

    for (unsigned int value = 0; value <= 255; ++value)
    {
        CHECK_FALSE(get_engine(std::string(1, static_cast<char>(value))).has_value());
    }
}
