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
 * @brief Tests for `fiction/technology/sidb/model/defect.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/sidb/model/defect.hpp>

#include <cstdint>
#include <utility>

using namespace fiction;
using namespace fiction::sidb::model;

TEST_CASE("Defect types determine charge classification and default extent", "[sidb-defects]")
{
    /** @brief No extent for an uncharged NONE defect. */
    constexpr std::pair none{uint16_t{0}, uint16_t{0}};
    /** @brief Default extent for neutral defect types. */
    constexpr std::pair<uint16_t, uint16_t> neutral{NEUTRAL_DEFECT_HORIZONTAL_SPACING, NEUTRAL_DEFECT_VERTICAL_SPACING};
    /** @brief Default extent for charged defect types. */
    constexpr std::pair<uint16_t, uint16_t> charged{CHARGED_DEFECT_HORIZONTAL_SPACING, CHARGED_DEFECT_VERTICAL_SPACING};
    /** @brief Each defect type has independent classification and extent expectations. */
    const auto [type, is_charged, is_neutral, extent] =
        GENERATE_COPY(table<defect_type, bool, bool, std::pair<uint16_t, uint16_t>>(
            {{defect_type::NONE, false, false, none},
             {defect_type::DB, true, false, charged},
             {defect_type::SI_VACANCY, true, false, charged},
             {defect_type::ARSENIC, true, false, charged},
             {defect_type::SINGLE_DIHYDRIDE, false, true, neutral},
             {defect_type::DIHYDRIDE_PAIR, false, true, neutral},
             {defect_type::ONE_BY_ONE, false, true, neutral},
             {defect_type::THREE_BY_ONE, false, true, neutral},
             {defect_type::SILOXANE, false, true, neutral},
             {defect_type::RAISED_SI, false, true, neutral},
             {defect_type::MISSING_DIMER, false, true, neutral},
             {defect_type::ETCH_PIT, false, true, neutral},
             {defect_type::STEP_EDGE, false, true, neutral},
             {defect_type::GUNK, false, true, neutral},
             {defect_type::UNKNOWN, false, true, neutral}}));
    CAPTURE(static_cast<unsigned>(type));
    CHECK(is_charged_defect_type(defect{type}) == is_charged);
    CHECK(is_neutral_defect_type(defect{type}) == is_neutral);
    CHECK(defect_extent(defect{type}) == extent);
}

TEST_CASE("Explicit defect charges determine overridden extent", "[sidb-defects]")
{
    /** @brief Override for neutral defects. */
    constexpr std::pair neutral{uint16_t{1}, uint16_t{0}};
    /** @brief Override for charged defects. */
    constexpr std::pair charged{uint16_t{1}, uint16_t{1}};

    CHECK(defect_extent(defect{defect_type::UNKNOWN, 0}, charged, neutral) == neutral);
    CHECK(defect_extent(defect{defect_type::UNKNOWN, -1}, charged, neutral) == charged);
}

TEST_CASE("Test for units", "[sidb-defects]")
{
    const defect defect_one{defect_type::NONE};
    CHECK(defect_one.charge == 0);
    CHECK(defect_one.epsilon_r == 0);
    CHECK(defect_one.lambda_tf == 0.0);

    const defect defect_two{defect_type::NONE, 2};
    CHECK(is_positively_charged_defect(defect_two));
    CHECK(defect_two.charge == 2);
    CHECK(defect_two.epsilon_r == 0);
    CHECK(defect_two.lambda_tf == 0.0);

    const defect defect_three{defect_type::NONE, -2, 5};
    CHECK(is_negatively_charged_defect(defect_three));
    CHECK(defect_three.charge == -2);
    CHECK(defect_three.epsilon_r == 5);
    CHECK(defect_three.lambda_tf == 0.0);
}
TEST_CASE("Compare Defect", "[sidb-defects]")
{
    SECTION("Different types")
    {
        const defect defect_one{defect_type::GUNK};
        const defect defect_two{defect_type::UNKNOWN};
        CHECK(defect_one != defect_two);
    }

    SECTION("Different charge")
    {
        const defect defect_one{defect_type::UNKNOWN, -5};
        const defect defect_two{defect_type::UNKNOWN, -1};
        CHECK(defect_one != defect_two);
    }

    SECTION("Different epsilon_r")
    {
        const defect defect_one{defect_type::UNKNOWN, -1, 2};
        const defect defect_two{defect_type::UNKNOWN, -1, 5};
        CHECK(defect_one != defect_two);
    }

    SECTION("Different lambda_tf")
    {
        const defect defect_one{defect_type::UNKNOWN, -1, 2, 4};
        const defect defect_two{defect_type::UNKNOWN, -1, 2, 5};
        CHECK(defect_one != defect_two);
    }

    SECTION("Completely different")
    {
        const defect defect_one{defect_type::UNKNOWN, -1, 2, 4};
        const defect defect_two{defect_type::DB, 5, 5, 0.3};
        CHECK(defect_one != defect_two);
    }

    SECTION("Identical Defects")
    {
        const defect defect_one{defect_type::UNKNOWN, -1, 2, 4};
        const defect defect_two{defect_type::UNKNOWN, -1, 2, 4};
        CHECK(defect_one == defect_two);
    }
}
