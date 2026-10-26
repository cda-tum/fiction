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
 * @brief Tests for `fiction/synthesis/io/tt_reader.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/synthesis/io/tt_reader.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace fiction;
using namespace fiction::synthesis;
using namespace fiction::synthesis::io;

TEST_CASE("Read truth tables", "[tt-reader]")
{
    // from https://people.eecs.berkeley.edu/~alanmi/temp5/
    static constexpr const char* file = "0x6996000000006996 (!(f+e)*(d+c+b+a))\n"
                                        "0x0990900906606006 (!(e+d+c)*(f+b+a))\n"
                                        "0x0000966996690000 ((f+e)*!(d+c+b+a))\n"
                                        "0x6006066090090990 ((e+d+c)*!(f+b+a))";

    std::stringstream stream{std::string{file}};

    tt_reader<kitty::dynamic_truth_table> reader{stream};

    const auto check = [&reader](const auto num_vars, const std::string& hex)
    {
        auto read_tt = reader.next();
        REQUIRE(read_tt.has_value());

        kitty::dynamic_truth_table test_tt{num_vars};
        kitty::create_from_hex_string(test_tt, hex);
        CHECK(kitty::equal(*read_tt, test_tt));
    };

    check(6u, "6996000000006996");
    check(6u, "0990900906606006");
    check(6u, "0000966996690000");
    check(6u, "6006066090090990");
    CHECK_FALSE(reader.next().has_value());
    CHECK_FALSE(reader.next().has_value());
}

TEST_CASE("Empty truth-table input", "[tt-reader]")
{
    std::stringstream stream{GENERATE("", " \t\r\n")};
    tt_reader<>       reader{stream};

    CHECK_FALSE(reader.next().has_value());
    CHECK_FALSE(reader.next().has_value());
}

TEST_CASE("Unreadable truth-table file", "[tt-reader]")
{
    CHECK_THROWS_AS(tt_reader<>{std::string{TEST_PATH} + "/missing-tt-reader-directory/input.tt"},
                    std::ifstream::failure);
}

TEST_CASE("Invalid truth-table records", "[tt-reader]")
{
    std::stringstream stream{GENERATE("0x8", "0x expr", "x expr", "xx8 expr", "0xg expr", "0xG expr", "0x/ expr",
                                      "0x: expr", "0x@ expr", "0x` expr", "0x\xff expr", "0xabc expr")};
    tt_reader<>       reader{stream};

    CHECK_THROWS_AS(reader.next(), std::invalid_argument);
    CHECK_THROWS_AS(reader.next(), std::invalid_argument);
}

TEST_CASE("Truth-table digit counts and hexadecimal case", "[tt-reader]")
{
    const auto hex = GENERATE(std::string{"f"}, std::string{"e8"}, std::string{"abCd"}, std::string{"0123456789ABCDEF"},
                              std::string{"0123456789abcdef0123456789ABCDEF"});
    const auto* const prefix = GENERATE("0x", "0X");
    std::stringstream stream{prefix + hex + " expression"};
    tt_reader<>       reader{stream};

    const auto result = reader.next();
    REQUIRE(result.has_value());
    if (result.has_value())
    {
        CHECK(result->num_bits() == hex.size() * 4u);
        kitty::dynamic_truth_table expected{result->num_vars()};
        kitty::create_from_hex_string(expected, hex);
        CHECK(kitty::equal(*result, expected));
    }
    CHECK_FALSE(reader.next().has_value());
}

TEST_CASE("Incomplete record after a truth table", "[tt-reader]")
{
    std::stringstream stream{"0x8 (a*b)\n0x6"};
    tt_reader<>       reader{stream};

    REQUIRE(reader.next().has_value());
    CHECK_THROWS_AS(reader.next(), std::invalid_argument);
}
