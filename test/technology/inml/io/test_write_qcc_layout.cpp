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

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/inml/io/write_qcc_layout.hpp>
#include <fiction/technology/inml/layout.hpp>

#include <sstream>
#include <string>

using namespace fiction;

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
