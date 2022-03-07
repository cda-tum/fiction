//
// Created by marcel on 07.03.22.
//

#include "catch.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_surface.hpp>
#include <fiction/traits.hpp>

#include <type_traits>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Traits and construction", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    REQUIRE(is_cell_level_layout_v<TestType>);
    CHECK(!has_assign_sidb_defect_v<TestType>);
    CHECK(!has_get_sidb_defect_v<TestType>);

    TestType lyt{};

    using defect_layout = sidb_surface<TestType>;
    CHECK(is_cell_level_layout_v<defect_layout>);
    CHECK(has_assign_sidb_defect_v<defect_layout>);
    CHECK(has_get_sidb_defect_v<defect_layout>);

    defect_layout defect_lyt{};
    defect_layout defect_lyt_from_lyt{lyt};

    using defect_defect_layout = sidb_surface<defect_layout>;
    CHECK(is_cell_level_layout_v<defect_defect_layout>);
    CHECK(has_assign_sidb_defect_v<defect_defect_layout>);
    CHECK(has_get_sidb_defect_v<defect_defect_layout>);
}

TEMPLATE_TEST_CASE(
    "Non-defective surface", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{{4, 4}};

    sidb_surface<TestType> defect_layout{lyt};

    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { CHECK(defect_layout.get_sidb_defect(c) == sidb_defect_type::NONE); });
}

TEMPLATE_TEST_CASE(
    "Assigning surface defects", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio<TestType>{0, 10}};

    sidb_surface<TestType> defect_layout{lyt};

    // assign defects
    defect_layout.assign_sidb_defect({0, 0}, sidb_defect_type::NONE);
    defect_layout.assign_sidb_defect({0, 1}, sidb_defect_type::DB);
    defect_layout.assign_sidb_defect({0, 2}, sidb_defect_type::SI_VACANCY);
    defect_layout.assign_sidb_defect({0, 3}, sidb_defect_type::DIHYDRIDE_PAIR);
    defect_layout.assign_sidb_defect({0, 4}, sidb_defect_type::SINGLE_DIHYDRIDE);
    defect_layout.assign_sidb_defect({0, 5}, sidb_defect_type::ONE_BY_ONE);
    defect_layout.assign_sidb_defect({0, 6}, sidb_defect_type::THREE_BY_ONE);
    defect_layout.assign_sidb_defect({0, 7}, sidb_defect_type::SILOXANE);
    defect_layout.assign_sidb_defect({0, 8}, sidb_defect_type::RAISED_SI);
    defect_layout.assign_sidb_defect({0, 9}, sidb_defect_type::ETCH_PIT);
    defect_layout.assign_sidb_defect({0, 10}, sidb_defect_type::MISSING_DIMER);

    // read defects
    CHECK(defect_layout.get_sidb_defect({0, 0}) == sidb_defect_type::NONE);
    CHECK(defect_layout.get_sidb_defect({0, 1}) == sidb_defect_type::DB);
    CHECK(defect_layout.get_sidb_defect({0, 2}) == sidb_defect_type::SI_VACANCY);
    CHECK(defect_layout.get_sidb_defect({0, 3}) == sidb_defect_type::DIHYDRIDE_PAIR);
    CHECK(defect_layout.get_sidb_defect({0, 4}) == sidb_defect_type::SINGLE_DIHYDRIDE);
    CHECK(defect_layout.get_sidb_defect({0, 5}) == sidb_defect_type::ONE_BY_ONE);
    CHECK(defect_layout.get_sidb_defect({0, 6}) == sidb_defect_type::THREE_BY_ONE);
    CHECK(defect_layout.get_sidb_defect({0, 7}) == sidb_defect_type::SILOXANE);
    CHECK(defect_layout.get_sidb_defect({0, 8}) == sidb_defect_type::RAISED_SI);
    CHECK(defect_layout.get_sidb_defect({0, 9}) == sidb_defect_type::ETCH_PIT);
    CHECK(defect_layout.get_sidb_defect({0, 10}) == sidb_defect_type::MISSING_DIMER);

    // erase defects
    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { defect_layout.assign_sidb_defect(c, sidb_defect_type::NONE); });

    // read defects
    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { CHECK(defect_layout.get_sidb_defect(c) == sidb_defect_type::NONE); });
}
