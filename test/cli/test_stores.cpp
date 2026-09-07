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
 * @brief Tests SiDB CLI store descriptions and statistics.
 */

#include <catch2/catch_test_macros.hpp>

#include "../../cli/stores.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstdint>
#include <limits>
#include <string>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("SiDB store descriptions represent the full column range", "[cli-stores]")
{
    layout lyt{};
    lyt.assign_dot_tag({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_dot_tag({std::numeric_limits<int32_t>::max(), 0, 0}, dot_tag::NORMAL);
    CHECK(cli::log_sidb(lyt)["dots"] == 2);
    CHECK_FALSE(cli::log_sidb(lyt).contains("cells"));
    CHECK(cli::describe_sidb(lyt).find("dots: 2") != std::string::npos);
    CHECK(cli::log_sidb(lyt)["layout"]["x-size"] == 2'147'483'648LL);
    CHECK(cli::describe_sidb(lyt).find("2147483648") != std::string::npos);

    lyt.assign_dot_tag({std::numeric_limits<int32_t>::min(), 0, 0}, dot_tag::NORMAL);
    CHECK(cli::log_sidb(lyt)["layout"]["x-size"] == 4'294'967'296LL);
    CHECK(cli::describe_sidb(lyt).find("4294967296") != std::string::npos);
}
