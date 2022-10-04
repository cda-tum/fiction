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

#include <set>
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
    CHECK(!has_foreach_sidb_defect_v<TestType>);

    TestType lyt{};

    using defect_layout = sidb_surface<TestType>;
    CHECK(is_cell_level_layout_v<defect_layout>);
    CHECK(has_assign_sidb_defect_v<defect_layout>);
    CHECK(has_get_sidb_defect_v<defect_layout>);
    CHECK(has_foreach_sidb_defect_v<defect_layout>);

    defect_layout defect_lyt{};
    defect_layout defect_lyt_from_lyt{lyt};

    using defect_defect_layout = sidb_surface<defect_layout>;
    CHECK(is_cell_level_layout_v<defect_defect_layout>);
    CHECK(has_assign_sidb_defect_v<defect_defect_layout>);
    CHECK(has_get_sidb_defect_v<defect_defect_layout>);
    CHECK(has_foreach_sidb_defect_v<defect_defect_layout>);
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
                                     { CHECK(defect_layout.get_sidb_defect(c).type == sidb_defect_type::NONE); });

    CHECK(defect_layout.num_defects() == 0);
}

TEMPLATE_TEST_CASE(
    "Assigning surface defects", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    static const std::map<cell<TestType>, sidb_defect> defect_map{
        {{{0, 0}, sidb_defect{sidb_defect_type::NONE}},
         {{0, 1}, sidb_defect{sidb_defect_type::DB}},
         {{0, 2}, sidb_defect{sidb_defect_type::SI_VACANCY}},
         {{0, 3}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE}},
         {{0, 4}, sidb_defect{sidb_defect_type::DIHYDRIDE_PAIR}},
         {{0, 5}, sidb_defect{sidb_defect_type::ONE_BY_ONE}},
         {{0, 6}, sidb_defect{sidb_defect_type::THREE_BY_ONE}},
         {{0, 7}, sidb_defect{sidb_defect_type::SILOXANE}},
         {{0, 8}, sidb_defect{sidb_defect_type::RAISED_SI}},
         {{0, 9}, sidb_defect{sidb_defect_type::MISSING_DIMER}},
         {{0, 10}, sidb_defect{sidb_defect_type::ETCH_PIT}},
         {{0, 11}, sidb_defect{sidb_defect_type::STEP_EDGE}},
         {{0, 12}, sidb_defect{sidb_defect_type::GUNK}},
         {{0, 13}, sidb_defect{sidb_defect_type::UNKNOWN}}}};

    TestType lyt{aspect_ratio<TestType>{0, defect_map.size() - 1}};

    sidb_surface<TestType> defect_layout{lyt};

    // assign defects
    for (const auto& [c, d] : defect_map)
    {
        defect_layout.assign_sidb_defect(c, d);
    }

    // read defects
    for (const auto& [c, d] : defect_map)
    {
        CHECK(defect_layout.get_sidb_defect(c).type == d.type);
    }

    CHECK(defect_layout.num_defects() == defect_map.size() - 1);  // NONE is not a defect

    // erase defects
    defect_layout.foreach_sidb_defect(
        [&defect_layout](const auto& cd)
        { defect_layout.assign_sidb_defect(cd.first, sidb_defect{sidb_defect_type::NONE}); });

    // read defects
    defect_layout.foreach_sidb_defect([](const auto&) { CHECK(false); });
    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { CHECK(defect_layout.get_sidb_defect(c).type == sidb_defect_type::NONE); });

    CHECK(defect_layout.num_defects() == 0);
}

TEMPLATE_TEST_CASE(
    "Charged and neutral defect extent", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio<TestType>{11, 9}};

    sidb_surface<TestType> defect_layout{lyt};

    SECTION("charged defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 4}) ==
              decltype(defect_layout.affected_sidbs({5, 4})){
                  {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3},
                  {7, 3}, {8, 3}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {2, 5}, {3, 5}, {4, 5},
                  {5, 5}, {6, 5}, {7, 5}, {8, 5}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}});

        CHECK(defect_layout.affected_sidbs({5, 5}) ==
              decltype(defect_layout.affected_sidbs({5, 5})){
                  {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4},
                  {7, 4}, {8, 4}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {2, 6}, {3, 6}, {4, 6},
                  {5, 6}, {6, 6}, {7, 6}, {8, 6}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){
                  {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {2, 3}, {3, 3}, {4, 3}, {5, 3},
                  {6, 3}, {7, 3}, {8, 3}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {2, 5},
                  {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6},
                  {7, 6}, {8, 6}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}});
    }
    SECTION("neutral defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect{sidb_defect_type::SILOXANE});
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect{sidb_defect_type::SILOXANE});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 4}) ==
              decltype(defect_layout.affected_sidbs({5, 4})){{4, 4}, {5, 4}, {6, 4}});

        CHECK(defect_layout.affected_sidbs({5, 5}) ==
              decltype(defect_layout.affected_sidbs({5, 5})){{4, 5}, {5, 5}, {6, 5}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){{4, 4}, {5, 4}, {6, 4}, {4, 5}, {5, 5}, {6, 5}});
    }
}

TEMPLATE_TEST_CASE(
    "Charged and neutral defect extent at layout edges", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio<TestType>{11, 9}};

    sidb_surface<TestType> defect_layout{lyt};

    SECTION("charged defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 0}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({0, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 0}) ==
              decltype(defect_layout.affected_sidbs({5, 0})){{2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
                                                             {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1},
                                                             {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}});

        CHECK(defect_layout.affected_sidbs({0, 5}) ==
              decltype(defect_layout.affected_sidbs({0, 5})){{0, 3}, {1, 3}, {2, 3}, {3, 3}, {0, 4}, {1, 4}, {2, 4},
                                                             {3, 4}, {0, 5}, {1, 5}, {2, 5}, {3, 5}, {0, 6}, {1, 6},
                                                             {2, 6}, {3, 6}, {0, 7}, {1, 7}, {2, 7}, {3, 7}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){
                  {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {2, 1}, {3, 1}, {4, 1}, {5, 1},
                  {6, 1}, {7, 1}, {8, 1}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {0, 3},
                  {1, 3}, {2, 3}, {3, 3}, {0, 4}, {1, 4}, {2, 4}, {3, 4}, {0, 5}, {1, 5}, {2, 5}, {3, 5},
                  {0, 6}, {1, 6}, {2, 6}, {3, 6}, {0, 7}, {1, 7}, {2, 7}, {3, 7}});
    }
    SECTION("neutral defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 0}, sidb_defect{sidb_defect_type::SILOXANE});
        defect_layout.assign_sidb_defect({0, 5}, sidb_defect{sidb_defect_type::SILOXANE});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 0}) ==
              decltype(defect_layout.affected_sidbs({5, 0})){{4, 0}, {5, 0}, {6, 0}});

        CHECK(defect_layout.affected_sidbs({0, 5}) == decltype(defect_layout.affected_sidbs({0, 5})){{0, 5}, {1, 5}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){{4, 0}, {5, 0}, {6, 0}, {0, 5}, {1, 5}});
    }
}

TEMPLATE_TEST_CASE(
    "Ignore defect types", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio<TestType>{4, 4}};

    SECTION("DB")
    {
        sidb_surface_params    params{std::set<sidb_defect_type>{sidb_defect_type::DB}};
        sidb_surface<TestType> defect_layout{lyt, params};

        defect_layout.assign_sidb_defect({2, 2}, sidb_defect{sidb_defect_type::DB});

        // number of defects should not count the ignored defect
        CHECK(defect_layout.num_defects() == 0);
    }
    SECTION("Siloxane + Silicon Vacancy")
    {
        sidb_surface_params params{
            std::set<sidb_defect_type>{{sidb_defect_type::SILOXANE, sidb_defect_type::SI_VACANCY}}};
        sidb_surface<TestType> defect_layout{lyt, params};

        defect_layout.assign_sidb_defect({2, 2}, sidb_defect{sidb_defect_type::SILOXANE});
        defect_layout.assign_sidb_defect({2, 3}, sidb_defect{sidb_defect_type::SI_VACANCY});

        // number of defects should not count the ignored defect
        CHECK(defect_layout.num_defects() == 0);
    }
}
