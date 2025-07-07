//
// Created by marcel on 07.03.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <unordered_set>
#include <utility>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "SiDB surface traits and construction", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    SECTION("Without SiDB lattice")
    {
        REQUIRE(is_cell_level_layout_v<TestType>);
        CHECK(!has_assign_sidb_defect_v<TestType>);
        CHECK(!has_get_sidb_defect_v<TestType>);
        CHECK(!has_foreach_sidb_defect_v<TestType>);
        CHECK(!is_sidb_defect_surface_v<TestType>);

        TestType lyt{};

        using defect_layout = sidb_defect_surface<TestType>;
        CHECK(is_cell_level_layout_v<defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);

        const defect_layout defect_lyt{};
        const defect_layout defect_lyt_from_lyt{defect_lyt};

        using defect_defect_layout = sidb_defect_surface<defect_layout>;
        CHECK(is_cell_level_layout_v<defect_defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);
    }
    SECTION("With SiDB lattice")
    {
        using lattice = sidb_lattice<sidb_100_lattice, TestType>;
        REQUIRE(is_cell_level_layout_v<lattice>);
        CHECK(!has_assign_sidb_defect_v<lattice>);
        CHECK(!has_get_sidb_defect_v<lattice>);
        CHECK(!has_foreach_sidb_defect_v<lattice>);
        CHECK(!is_sidb_defect_surface_v<lattice>);
        CHECK(is_sidb_lattice_100_v<lattice>);

        using defect_layout = sidb_defect_surface<lattice>;
        CHECK(is_cell_level_layout_v<defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);
        CHECK(is_sidb_lattice_100_v<lattice>);

        using defect_defect_layout = sidb_defect_surface<defect_layout>;
        CHECK(is_cell_level_layout_v<defect_defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);
        CHECK(is_sidb_lattice_100_v<lattice>);
    }

    SECTION("With SiDB lattice, Si-111")
    {
        using lattice = sidb_lattice<sidb_111_lattice, TestType>;
        REQUIRE(is_cell_level_layout_v<lattice>);
        CHECK(!has_assign_sidb_defect_v<lattice>);
        CHECK(!has_get_sidb_defect_v<lattice>);
        CHECK(!has_foreach_sidb_defect_v<lattice>);
        CHECK(!is_sidb_defect_surface_v<lattice>);
        CHECK(is_sidb_lattice_111_v<lattice>);

        using defect_layout = sidb_defect_surface<lattice>;
        CHECK(is_cell_level_layout_v<defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);
        CHECK(is_sidb_lattice_111_v<lattice>);

        using defect_defect_layout = sidb_defect_surface<defect_layout>;
        CHECK(is_cell_level_layout_v<defect_defect_layout>);
        CHECK(has_assign_sidb_defect_v<defect_defect_layout>);
        CHECK(has_get_sidb_defect_v<defect_defect_layout>);
        CHECK(has_foreach_sidb_defect_v<defect_defect_layout>);
        CHECK(is_sidb_defect_surface_v<defect_layout>);
        CHECK(is_sidb_lattice_111_v<lattice>);
    }
}

TEMPLATE_TEST_CASE(
    "SiDB surface construction with aspect ratio as input", "[sidb-surface]",
    (sidb_defect_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>>),
    (sidb_defect_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>),
    (sidb_defect_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>),
    (sidb_defect_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>>),
    (sidb_defect_surface<
        cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>>))
{
    TestType defect_layout{{4, 4}};

    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { CHECK(defect_layout.get_sidb_defect(c).type == sidb_defect_type::NONE); });

    CHECK(defect_layout.num_defects() == 0);
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
    "Overwrite SiDB defect", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{{1, 1}};

    sidb_defect_surface<TestType> defect_layout{lyt};

    defect_layout.assign_sidb_defect({0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1});
    CHECK(defect_layout.get_sidb_defect({0, 0}).type == sidb_defect_type::UNKNOWN);
    CHECK(defect_layout.get_sidb_defect({0, 0}).charge == 1);
    CHECK(defect_layout.num_defects() == 1);

    defect_layout.assign_sidb_defect({0, 0}, sidb_defect{sidb_defect_type::DB, -1});
    CHECK(defect_layout.get_sidb_defect({0, 0}).charge == -1);
    CHECK(defect_layout.num_defects() == 1);
}

TEMPLATE_TEST_CASE(
    "Non-defective SiDB surface", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{{4, 4}};

    sidb_defect_surface<TestType> defect_layout{lyt};

    defect_layout.foreach_coordinate([&defect_layout](const auto& c)
                                     { CHECK(defect_layout.get_sidb_defect(c).type == sidb_defect_type::NONE); });

    CHECK(defect_layout.num_defects() == 0);
}

TEMPLATE_TEST_CASE(
    "Assigning SiDB surface defects", "[sidb-surface]",
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

    TestType lyt{aspect_ratio_t<TestType>{0, defect_map.size() - 1}};

    sidb_defect_surface<TestType> defect_layout{lyt};

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
    "Charged and neutral SiDB defect extent", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio_t<TestType>{11, 9}};

    sidb_defect_surface<TestType> defect_layout{lyt};

    SECTION("charged defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 4}) ==
              decltype(defect_layout.affected_sidbs({5, 4})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.affected_sidbs({5, 5}) ==
              decltype(defect_layout.affected_sidbs({5, 5})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});
    }

    SECTION("charged defects, user_defined_spacing_charged_defects is true, (26,13) as charged_defect_spacing")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 4}, std::make_pair(26, 13)) ==
              decltype(defect_layout.affected_sidbs({5, 4})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.affected_sidbs({5, 5}, std::make_pair(26, 13)) ==
              decltype(defect_layout.affected_sidbs({5, 5})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.all_affected_sidbs(std::make_pair(26, 13)) ==
              decltype(defect_layout.all_affected_sidbs()){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});
    }

    SECTION("charged defects, user_defined_spacing_charged_defects is true, (0,0) as charged_defect_spacing")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 4}, std::make_pair(0, 0)) ==
              decltype(defect_layout.affected_sidbs({5, 4})){{{5, 4}}});

        CHECK(defect_layout.affected_sidbs({5, 4}, std::make_pair(2, 1)) ==
              decltype(defect_layout.affected_sidbs({5, 4})){{{3, 3},
                                                              {4, 3},
                                                              {5, 3},
                                                              {6, 3},
                                                              {7, 3},
                                                              {3, 4},
                                                              {4, 4},
                                                              {5, 4},
                                                              {6, 4},
                                                              {7, 4},
                                                              {3, 5},
                                                              {4, 5},
                                                              {5, 5},
                                                              {6, 5},
                                                              {7, 5}}});
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
    "Charged and neutral SiDB defect extent at layout edges", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio_t<TestType>{11, 9}};

    sidb_defect_surface<TestType> defect_layout{lyt};

    SECTION("charged defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 0}, sidb_defect{sidb_defect_type::SI_VACANCY});
        defect_layout.assign_sidb_defect({0, 5}, sidb_defect{sidb_defect_type::SI_VACANCY});

        CHECK(defect_layout.num_defects() == 2);

        CHECK(defect_layout.affected_sidbs({5, 0}) ==
              decltype(defect_layout.affected_sidbs({5, 0})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.affected_sidbs({0, 5}) ==
              decltype(defect_layout.affected_sidbs({0, 5})){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});

        CHECK(defect_layout.all_affected_sidbs() ==
              decltype(defect_layout.all_affected_sidbs()){
                  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
                   {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}, {10, 1}, {11, 1},
                   {0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {9, 2}, {10, 2}, {11, 2},
                   {0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {9, 3}, {10, 3}, {11, 3},
                   {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4},
                   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5}, {10, 5}, {11, 5},
                   {0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}, {10, 6}, {11, 6},
                   {0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}, {9, 7}, {10, 7}, {11, 7},
                   {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}, {6, 8}, {7, 8}, {8, 8}, {9, 8}, {10, 8}, {11, 8},
                   {0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}, {6, 9}, {7, 9}, {8, 9}, {9, 9}, {10, 9}, {11, 9}}});
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
    "Ignore SiDB defect types", "[sidb-surface]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    TestType lyt{aspect_ratio_t<TestType>{4, 4}};

    SECTION("DB")
    {
        const sidb_defect_surface_params params{std::unordered_set<sidb_defect_type>{sidb_defect_type::DB}};
        sidb_defect_surface<TestType>    defect_layout{lyt, params};

        defect_layout.assign_sidb_defect({2, 2}, sidb_defect{sidb_defect_type::DB});

        // number of defects should not count the ignored defect
        CHECK(defect_layout.num_defects() == 0);
    }
    SECTION("Siloxane + Silicon Vacancy")
    {
        const sidb_defect_surface_params params{
            std::unordered_set<sidb_defect_type>{{sidb_defect_type::SILOXANE, sidb_defect_type::SI_VACANCY}}};
        sidb_defect_surface<TestType> defect_layout{lyt, params};

        defect_layout.assign_sidb_defect({2, 2}, sidb_defect{sidb_defect_type::SILOXANE});
        defect_layout.assign_sidb_defect({2, 3}, sidb_defect{sidb_defect_type::SI_VACANCY});

        // number of defects should not count the ignored defect
        CHECK(defect_layout.num_defects() == 0);

        defect_layout.assign_sidb_defect({0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1});
        defect_layout.assign_sidb_defect({3, 3}, sidb_defect{sidb_defect_type::UNKNOWN, 1});
        defect_layout.assign_sidb_defect({1, 1}, sidb_defect{sidb_defect_type::STEP_EDGE});

        CHECK(defect_layout.num_charged_defects() == 2);
        CHECK(defect_layout.num_neutral_defects() == 1);
    }
}
