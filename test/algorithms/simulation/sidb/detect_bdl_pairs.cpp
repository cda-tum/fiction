//
// Created by marcel on 21.07.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Empty layout I/O BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    const layout lyt{};

    const auto result = detect_io_bdl_pairs(lyt);

    CHECK(result.empty());
}

TEST_CASE("BDL wire I/O BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{0, 20}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    SECTION("default threshold")
    {
        const auto result = detect_io_bdl_pairs(lyt);

        REQUIRE(result.size() == 2);
        const auto& input_pair  = result[0];
        const auto& output_pair = result[1];

        CHECK(input_pair.type == layout::cell_type::INPUT);
        CHECK(input_pair.top == cell<layout>{0, 0, 0});
        CHECK(input_pair.bottom == cell<layout>{2, 0, 0});

        CHECK(output_pair.type == layout::cell_type::OUTPUT);
        CHECK(output_pair.top == cell<layout>{18, 0, 0});
        CHECK(output_pair.bottom == cell<layout>{20, 0, 0});
    }
    SECTION("1 nm threshold")
    {
        const auto result = detect_io_bdl_pairs(lyt, {1_nm});

        REQUIRE(result.size() == 2);
        const auto& input_pair  = result[0];
        const auto& output_pair = result[1];

        CHECK(input_pair.type == layout::cell_type::INPUT);
        CHECK(input_pair.top == cell<layout>{0, 0, 0});
        CHECK(input_pair.bottom == cell<layout>{2, 0, 0});

        CHECK(output_pair.type == layout::cell_type::OUTPUT);
        CHECK(output_pair.top == cell<layout>{18, 0, 0});
        CHECK(output_pair.bottom == cell<layout>{20, 0, 0});
    }
    SECTION("0.5 nm threshold")
    {
        const auto result = detect_io_bdl_pairs(lyt, {0.5_nm});

        // the threshold is too small to detect any BDL pairs
        REQUIRE(result.size() == 0);
    }
}

TEST_CASE("SiQAD's AND gate I/O BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    const auto result = detect_io_bdl_pairs(lyt);

    REQUIRE(result.size() == 3);
    const auto& input_pair1 = result[0];
    const auto& input_pair2 = result[1];
    const auto& output_pair = result[2];

    CHECK(input_pair1.type == layout::cell_type::INPUT);
    CHECK(input_pair1.top == cell<layout>{20, 0, 1});
    CHECK(input_pair1.bottom == cell<layout>{18, 1, 1});

    CHECK(input_pair2.type == layout::cell_type::INPUT);
    CHECK(input_pair2.top == cell<layout>{0, 0, 1});
    CHECK(input_pair2.bottom == cell<layout>{2, 1, 1});

    CHECK(output_pair.type == layout::cell_type::OUTPUT);
    CHECK(output_pair.top == cell<layout>{10, 6, 0});
    CHECK(output_pair.bottom == cell<layout>{10, 7, 0});
}
