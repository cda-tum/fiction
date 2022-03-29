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
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect_type::SI_VACANCY);
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect_type::SI_VACANCY);

        CHECK(defect_layout.affected_sidbs({5, 4}) ==
              std::set<coordinate<TestType>>{{2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {2, 3}, {3, 3},
                                             {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {2, 4}, {3, 4}, {4, 4}, {5, 4},
                                             {6, 4}, {7, 4}, {8, 4}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5},
                                             {8, 5}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}});

        CHECK(defect_layout.affected_sidbs({5, 5}) ==
              std::set<coordinate<TestType>>{{2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3}, {8, 3}, {2, 4}, {3, 4},
                                             {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {2, 5}, {3, 5}, {4, 5}, {5, 5},
                                             {6, 5}, {7, 5}, {8, 5}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6},
                                             {8, 6}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}});
    }
    SECTION("neutral defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 4}, sidb_defect_type::SILOXANE);
        defect_layout.assign_sidb_defect({5, 5}, sidb_defect_type::SILOXANE);

        CHECK(defect_layout.affected_sidbs({5, 4}) == std::set<coordinate<TestType>>{{4, 4}, {5, 4}, {6, 4}});

        CHECK(defect_layout.affected_sidbs({5, 5}) == std::set<coordinate<TestType>>{{4, 5}, {5, 5}, {6, 5}});
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
        defect_layout.assign_sidb_defect({5, 0}, sidb_defect_type::SI_VACANCY);
        defect_layout.assign_sidb_defect({0, 5}, sidb_defect_type::SI_VACANCY);

        CHECK(defect_layout.affected_sidbs({5, 0}) ==
              std::set<coordinate<TestType>>{{2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
                                             {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1},
                                             {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}});

        CHECK(defect_layout.affected_sidbs({0, 5}) ==
              std::set<coordinate<TestType>>{{0, 3}, {1, 3}, {2, 3}, {3, 3}, {0, 4}, {1, 4}, {2, 4},
                                             {3, 4}, {0, 5}, {1, 5}, {2, 5}, {3, 5}, {0, 6}, {1, 6},
                                             {2, 6}, {3, 6}, {0, 7}, {1, 7}, {2, 7}, {3, 7}});
    }
    SECTION("neutral defects")
    {
        // assign defects
        defect_layout.assign_sidb_defect({5, 0}, sidb_defect_type::SILOXANE);
        defect_layout.assign_sidb_defect({0, 5}, sidb_defect_type::SILOXANE);

        CHECK(defect_layout.affected_sidbs({5, 0}) == std::set<coordinate<TestType>>{{4, 0}, {5, 0}, {6, 0}});

        CHECK(defect_layout.affected_sidbs({0, 5}) == std::set<coordinate<TestType>>{{0, 5}, {1, 5}});
    }
}
