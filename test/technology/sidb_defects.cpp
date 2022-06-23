//
// Created by marcel on 28.03.22.
//

#include "catch.hpp"

#include <fiction/technology/sidb_defects.hpp>

using namespace fiction;

TEST_CASE("Charged and uncharged defect types", "[sidb-defects]")
{
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::NONE}));
    CHECK(is_charged_defect(sidb_defect{sidb_defect_type::DB}));
    CHECK(is_charged_defect(sidb_defect{sidb_defect_type::SI_VACANCY}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::ONE_BY_ONE}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::THREE_BY_ONE}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::SILOXANE}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::RAISED_SI}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::ETCH_PIT}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::MISSING_DIMER}));
    CHECK(!is_charged_defect(sidb_defect{sidb_defect_type::UNKNOWN}));

    CHECK(!is_neutral_defect(sidb_defect{sidb_defect_type::NONE}));
    CHECK(!is_neutral_defect(sidb_defect{sidb_defect_type::DB}));
    CHECK(!is_neutral_defect(sidb_defect{sidb_defect_type::SI_VACANCY}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::ONE_BY_ONE}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::THREE_BY_ONE}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::SILOXANE}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::RAISED_SI}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::ETCH_PIT}));
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::MISSING_DIMER}));
    CHECK(!is_neutral_defect(sidb_defect{sidb_defect_type::UNKNOWN}));
}
