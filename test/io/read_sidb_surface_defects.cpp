//
// Created by marcel on 29.03.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/io/read_sidb_surface_defects.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <sstream>
#include <string>
#include <type_traits>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Read empty surface", "[read-sidb-surface-defects]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    static constexpr const char* empty_sidb_surface               = "[]";
    static constexpr const char* sidb_surface_of_one_empty_row    = "[[]]";
    static constexpr const char* sidb_surface_of_three_empty_rows = "[[][][]]";

    std::istringstream empty_surface_stream{empty_sidb_surface};
    std::istringstream empty_row_surface_stream{sidb_surface_of_one_empty_row};
    std::istringstream three_empty_rows_surface_stream{sidb_surface_of_three_empty_rows};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(lyt.is_empty_cell({0, 0}));
        CHECK(lyt.get_sidb_defect({0, 0}).type == sidb_defect_type::NONE);
    };

    check(read_sidb_surface_defects<TestType>(empty_surface_stream));
    check(read_sidb_surface_defects<TestType>(empty_row_surface_stream));
    check(read_sidb_surface_defects<TestType>(three_empty_rows_surface_stream));
}

TEMPLATE_TEST_CASE(
    "Read non-defective surface", "[read-sidb-surface-defects]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    static constexpr const char* sidb_surface = "[[0 0 0 0] [0 0 0 0] [0 0 0 0]]";

    std::istringstream surface_stream{sidb_surface};

    const auto lyt = read_sidb_surface_defects<TestType>(surface_stream);

    CHECK(lyt.x() == 3);
    CHECK(lyt.y() == 2);

    lyt.foreach_cell(
        [&lyt](const auto& c)
        {
            CHECK(lyt.is_empty_cell(c));
            CHECK(lyt.get_sidb_defect(c).type == sidb_defect_type::NONE);
        });
}

TEMPLATE_TEST_CASE(
    "Read defective surface", "[read-sidb-surface-defects]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    static constexpr const char* sidb_surface = "[[0 1 2 3]"
                                                " [4 5 6 7]"
                                                " [8 9 10 0]]";

    std::istringstream surface_stream{sidb_surface};

    const auto lyt = read_sidb_surface_defects<TestType>(surface_stream);

    CHECK(lyt.x() == 3);
    CHECK(lyt.y() == 2);

    CHECK(lyt.get_sidb_defect({0, 0}).type == sidb_defect_type::NONE);
    CHECK(lyt.get_sidb_defect({1, 0}).type == sidb_defect_type::DB);
    CHECK(lyt.get_sidb_defect({2, 0}).type == sidb_defect_type::SI_VACANCY);
    CHECK(lyt.get_sidb_defect({3, 0}).type == sidb_defect_type::DIHYDRIDE_PAIR);
    CHECK(lyt.get_sidb_defect({0, 1}).type == sidb_defect_type::SINGLE_DIHYDRIDE);
    CHECK(lyt.get_sidb_defect({1, 1}).type == sidb_defect_type::ONE_BY_ONE);
    CHECK(lyt.get_sidb_defect({2, 1}).type == sidb_defect_type::THREE_BY_ONE);
    CHECK(lyt.get_sidb_defect({3, 1}).type == sidb_defect_type::SILOXANE);
    CHECK(lyt.get_sidb_defect({0, 2}).type == sidb_defect_type::RAISED_SI);
    CHECK(lyt.get_sidb_defect({1, 2}).type == sidb_defect_type::ETCH_PIT);
    CHECK(lyt.get_sidb_defect({2, 2}).type == sidb_defect_type::MISSING_DIMER);
    CHECK(lyt.get_sidb_defect({3, 2}).type == sidb_defect_type::NONE);
}

TEMPLATE_TEST_CASE(
    "Exceptions", "[read-sidb-surface-defects]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    SECTION("unsupported_defect_index_exception")
    {
        static constexpr const char* sidb_surface = "[[0 1 2 3]"
                                                    " [4 5 6 7]"
                                                    " [8 9 10 11]]";  // 11 is not a supported defect type

        std::istringstream surface_stream{sidb_surface};

        CHECK_THROWS_AS(read_sidb_surface_defects<TestType>(surface_stream), unsupported_defect_index_exception);
    }
    SECTION("missing_sidb_position_exception")
    {
        static constexpr const char* sidb_surface = "[[0 1 2 3]"
                                                    " [4 5 6]"  // the second row has a missing SiDB position
                                                    " [8 9 10 0]]";

        std::istringstream surface_stream{sidb_surface};

        CHECK_THROWS_AS(read_sidb_surface_defects<TestType>(surface_stream), missing_sidb_position_exception);
    }
}

TEMPLATE_TEST_CASE(
    "Edge cases", "[read-sidb-surface-defects]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>))
{
    SECTION("ignore lines")
    {
        SECTION("due to negative numbers")
        {
            static constexpr const char* sidb_surface = "[[0 0 0 0]"
                                                        " [0 -5 0 0]"  // -5 is negative; this line is being ignored
                                                        " [0 0 0 0]]";

            std::istringstream surface_stream{sidb_surface};

            const auto lyt = read_sidb_surface_defects<TestType>(surface_stream);

            CHECK(lyt.x() == 3);
            CHECK(lyt.y() == 1);  // only 2 rows are being parsed
        }
        SECTION("due to non-numeral characters")
        {
            static constexpr const char* sidb_surface = "[[0 0 0 0]"
                                                        " [0 Z 0 0]"  // Z is not a numeral; this line is being ignored
                                                        " [0 0 0 0]]";

            std::istringstream surface_stream{sidb_surface};

            const auto lyt = read_sidb_surface_defects<TestType>(surface_stream);

            CHECK(lyt.x() == 3);
            CHECK(lyt.y() == 1);  // only two rows are being parsed
        }
    }
}
