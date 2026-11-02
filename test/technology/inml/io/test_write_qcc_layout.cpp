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
 * @brief Tests for iNML QCC serialization.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/inml/io/write_qcc_layout.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

using namespace fiction;
using namespace fiction::inml;
using namespace fiction::inml::io;

TEST_CASE("QCC dimensions retain their maximum-index schema", "[qcc]")
{
    inml::layout layout{{3, 1}};
    layout.assign_cell_type({0, 0}, inml::magnet_type::INPUT);
    layout.assign_cell_type({1, 0}, inml::magnet_type::NORMAL);
    layout.assign_cell_type({2, 0}, inml::magnet_type::OUTPUT);
    std::ostringstream stream{};
    inml::io::write_qcc_layout(layout, stream);
    CHECK(stream.str().find("maxX=\"2\" maxY=\"0\"") != std::string::npos);
    CHECK(stream.str().find("x=\"2\" y=\"0\"") != std::string::npos);
}

TEST_CASE("QCC rejects misplaced terminals before writing", "[qcc]")
{
    const auto [type, x] = GENERATE(Catch::Generators::table<magnet_type, unsigned>(
        {{magnet_type::INPUT, 1}, {magnet_type::INPUT, 2}, {magnet_type::OUTPUT, 0}, {magnet_type::OUTPUT, 1}}));
    CAPTURE(static_cast<char>(type), x);
    inml::layout lyt{{3, 1}};
    for (const auto column : {0u, 1u, 2u})
    {
        lyt.assign_cell_type({column, 0}, magnet_type::NORMAL);
    }
    lyt.assign_cell_type({x, 0}, type);
    std::ostringstream stream{};

    CHECK_THROWS_AS(write_qcc_layout(lyt, stream), std::invalid_argument);
    CHECK(stream.str().empty());

    lyt.assign_cell_type({x, 0}, magnet_type::NORMAL);
    CHECK_NOTHROW(write_qcc_layout(lyt, stream));
    CHECK(stream.str().find("</qcacomponent>") != std::string::npos);
}
