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
    CHECK(is_neutral_defect(sidb_defect{sidb_defect_type::UNKNOWN}));
}

TEST_CASE("Defect extent", "[sidb-defects]")
{
    CHECK(defect_extent(sidb_defect{sidb_defect_type::NONE}) == std::pair<uint16_t, uint16_t>{0, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::DB}) == std::pair<uint16_t, uint16_t>{3, 2});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::SI_VACANCY}) == std::pair<uint16_t, uint16_t>{3, 2});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::ONE_BY_ONE}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::THREE_BY_ONE}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::SILOXANE}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::RAISED_SI}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::ETCH_PIT}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::MISSING_DIMER}) == std::pair<uint16_t, uint16_t>{1, 0});
    CHECK(defect_extent(sidb_defect{sidb_defect_type::UNKNOWN}) == std::pair<uint16_t, uint16_t>{1, 0});
}
