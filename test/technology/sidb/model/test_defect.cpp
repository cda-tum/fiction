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

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/model/defect.hpp>

#include <cstdint>
#include <utility>

using namespace fiction;
using namespace fiction::sidb::model;

TEST_CASE("Charged and uncharged defect types", "[sidb-defects]")
{
    CHECK(!is_charged_defect_type(defect{defect_type::NONE}));
    CHECK(is_charged_defect_type(defect{defect_type::DB}));
    CHECK(is_charged_defect_type(defect{defect_type::SI_VACANCY}));
    CHECK(is_charged_defect_type(defect{defect_type::ARSENIC}));
    CHECK(!is_charged_defect_type(defect{defect_type::SINGLE_DIHYDRIDE}));
    CHECK(!is_charged_defect_type(defect{defect_type::DIHYDRIDE_PAIR}));
    CHECK(!is_charged_defect_type(defect{defect_type::ONE_BY_ONE}));
    CHECK(!is_charged_defect_type(defect{defect_type::THREE_BY_ONE}));
    CHECK(!is_charged_defect_type(defect{defect_type::SILOXANE}));
    CHECK(!is_charged_defect_type(defect{defect_type::RAISED_SI}));
    CHECK(!is_charged_defect_type(defect{defect_type::MISSING_DIMER}));
    CHECK(!is_charged_defect_type(defect{defect_type::ETCH_PIT}));
    CHECK(!is_charged_defect_type(defect{defect_type::STEP_EDGE}));
    CHECK(!is_charged_defect_type(defect{defect_type::GUNK}));
    CHECK(!is_charged_defect_type(defect{defect_type::UNKNOWN}));

    CHECK(!is_neutral_defect_type(defect{defect_type::NONE}));
    CHECK(!is_neutral_defect_type(defect{defect_type::DB}));
    CHECK(!is_neutral_defect_type(defect{defect_type::SI_VACANCY}));
    CHECK(is_neutral_defect_type(defect{defect_type::SINGLE_DIHYDRIDE}));
    CHECK(is_neutral_defect_type(defect{defect_type::DIHYDRIDE_PAIR}));
    CHECK(is_neutral_defect_type(defect{defect_type::ONE_BY_ONE}));
    CHECK(is_neutral_defect_type(defect{defect_type::THREE_BY_ONE}));
    CHECK(is_neutral_defect_type(defect{defect_type::SILOXANE}));
    CHECK(is_neutral_defect_type(defect{defect_type::RAISED_SI}));
    CHECK(is_neutral_defect_type(defect{defect_type::MISSING_DIMER}));
    CHECK(is_neutral_defect_type(defect{defect_type::ETCH_PIT}));
    CHECK(is_neutral_defect_type(defect{defect_type::STEP_EDGE}));
    CHECK(is_neutral_defect_type(defect{defect_type::GUNK}));
    CHECK(is_neutral_defect_type(defect{defect_type::UNKNOWN}));
}

TEST_CASE("Defect extent", "[sidb-defects]")
{
    static constexpr std::pair<uint16_t, uint16_t> no_spacing{0, 0};
    static constexpr std::pair<uint16_t, uint16_t> neutral_spacing{NEUTRAL_DEFECT_HORIZONTAL_SPACING,
                                                                   NEUTRAL_DEFECT_VERTICAL_SPACING};
    static constexpr std::pair<uint16_t, uint16_t> charged_spacing{CHARGED_DEFECT_HORIZONTAL_SPACING,
                                                                   CHARGED_DEFECT_VERTICAL_SPACING};

    static constexpr std::pair<uint16_t, uint16_t> neutral_spacing_overwrite{1, 0};
    static constexpr std::pair<uint16_t, uint16_t> charged_spacing_overwrite{1, 1};

    CHECK(defect_extent(defect{defect_type::NONE}) == no_spacing);
    CHECK(defect_extent(defect{defect_type::DB}) == charged_spacing);
    CHECK(defect_extent(defect{defect_type::SI_VACANCY}) == charged_spacing);
    CHECK(defect_extent(defect{defect_type::ARSENIC}) == charged_spacing);
    CHECK(defect_extent(defect{defect_type::SINGLE_DIHYDRIDE}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::DIHYDRIDE_PAIR}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::ONE_BY_ONE}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::THREE_BY_ONE}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::SILOXANE}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::RAISED_SI}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::MISSING_DIMER}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::ETCH_PIT}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::STEP_EDGE}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::GUNK}) == neutral_spacing);
    CHECK(defect_extent(defect{defect_type::UNKNOWN}) == neutral_spacing);

    CHECK(defect_extent(defect{defect_type::UNKNOWN, 0}, charged_spacing_overwrite, neutral_spacing_overwrite) ==
          neutral_spacing_overwrite);
    CHECK(defect_extent(defect{defect_type::UNKNOWN, -1}, charged_spacing_overwrite, neutral_spacing_overwrite) ==
          charged_spacing_overwrite);
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
