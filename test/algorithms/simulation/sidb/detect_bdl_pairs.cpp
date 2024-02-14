//
// Created by marcel on 21.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb_lattice_layout.hpp>
#include <fiction/types.hpp>

using namespace fiction;

using lattice = sidb_lattice_layout<sidb_cell_clk_lyt_siqad>;

TEST_CASE("Empty layout BDL detection", "[detect-bdl-pairs]")
{
    const lattice lyt{};

    const auto result = detect_bdl_pairs(lyt, sidb_technology::cell_type::NORMAL);

    CHECK(result.empty());
}

TEST_CASE("Atomic wire BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{7, 0}, "Atomic wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::OUTPUT);

    detect_bdl_pairs_params params{};

    const lattice lat{lyt};

    SECTION("default lower threshold")
    {
        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::NORMAL, params);

        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(normal_pair1.type == layout::cell_type::NORMAL);
        CHECK((normal_pair1.upper == cell<layout>{2, 0, 0} || normal_pair1.upper == cell<layout>{3, 0, 0}));
        CHECK((normal_pair1.lower == cell<layout>{4, 0, 0} || normal_pair1.lower == cell<layout>{5, 0, 0}));

        CHECK(normal_pair2.type == layout::cell_type::NORMAL);
        CHECK((normal_pair2.upper == cell<layout>{2, 0, 0} || normal_pair2.upper == cell<layout>{3, 0, 0}));
        CHECK((normal_pair2.lower == cell<layout>{4, 0, 0} || normal_pair2.lower == cell<layout>{5, 0, 0}));
    }
    SECTION("0.5 nm lower threshold")
    {
        params.minimum_distance = 0.5;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::NORMAL, params);

        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(normal_pair1.type == layout::cell_type::NORMAL);
        CHECK((normal_pair1.upper == cell<layout>{2, 0, 0} || normal_pair1.upper == cell<layout>{3, 0, 0}));
        CHECK((normal_pair1.lower == cell<layout>{4, 0, 0} || normal_pair1.lower == cell<layout>{5, 0, 0}));

        CHECK(normal_pair2.type == layout::cell_type::NORMAL);
        CHECK((normal_pair2.upper == cell<layout>{2, 0, 0} || normal_pair2.upper == cell<layout>{3, 0, 0}));
        CHECK((normal_pair2.lower == cell<layout>{4, 0, 0} || normal_pair2.lower == cell<layout>{5, 0, 0}));
    }
    SECTION("0 nm lower threshold")
    {
        params.minimum_distance = 0;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::NORMAL, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(input_pair.type == layout::cell_type::INPUT);
        CHECK(input_pair.upper == cell<layout>{0, 0, 0});
        CHECK(input_pair.lower == cell<layout>{1, 0, 0});

        CHECK(output_pair.type == layout::cell_type::OUTPUT);
        CHECK(output_pair.upper == cell<layout>{6, 0, 0});
        CHECK(output_pair.lower == cell<layout>{7, 0, 0});

        CHECK(normal_pair1.type == layout::cell_type::NORMAL);
        CHECK((normal_pair1.upper == cell<layout>{2, 0, 0} || normal_pair1.upper == cell<layout>{4, 0, 0}));
        CHECK((normal_pair1.lower == cell<layout>{3, 0, 0} || normal_pair1.lower == cell<layout>{5, 0, 0}));

        CHECK(normal_pair2.type == layout::cell_type::NORMAL);
        CHECK((normal_pair2.upper == cell<layout>{2, 0, 0} || normal_pair2.upper == cell<layout>{4, 0, 0}));
        CHECK((normal_pair2.lower == cell<layout>{3, 0, 0} || normal_pair2.lower == cell<layout>{5, 0, 0}));
    }
}

TEST_CASE("BDL wire BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 0}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    detect_bdl_pairs_params params{};
    // set default lower threshold to 0 for testing
    params.minimum_distance = 0;

    const lattice lat{lyt};

    SECTION("default upper threshold")
    {
        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        CHECK(input_pair.type == layout::cell_type::INPUT);
        CHECK(input_pair.upper == cell<layout>{0, 0, 0});
        CHECK(input_pair.lower == cell<layout>{2, 0, 0});

        CHECK(output_pair.type == layout::cell_type::OUTPUT);
        CHECK(output_pair.upper == cell<layout>{18, 0, 0});
        CHECK(output_pair.lower == cell<layout>{20, 0, 0});
    }
    SECTION("1 nm upper threshold")
    {
        params.maximum_distance = 1;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        CHECK(input_pair.type == layout::cell_type::INPUT);
        CHECK(input_pair.upper == cell<layout>{0, 0, 0});
        CHECK(input_pair.lower == cell<layout>{2, 0, 0});

        CHECK(output_pair.type == layout::cell_type::OUTPUT);
        CHECK(output_pair.upper == cell<layout>{18, 0, 0});
        CHECK(output_pair.lower == cell<layout>{20, 0, 0});
    }
    SECTION("0.5 nm upper threshold")
    {
        params.maximum_distance = 0.5;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT, params);

        // the threshold is too small to detect any BDL pairs
        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
    }
}

TEST_CASE("SiQAD's AND gate BDL detection", "[detect-bdl-pairs]")
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

    const lattice lat{lyt};

    const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT);
    const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT);

    REQUIRE(input_bdl_pairs.size() == 2);
    REQUIRE(output_bdl_pairs.size() == 1);

    const auto& input_pair1 = input_bdl_pairs[0];
    const auto& input_pair2 = input_bdl_pairs[1];
    const auto& output_pair = output_bdl_pairs.front();

    CHECK(input_pair1.type == layout::cell_type::INPUT);
    CHECK((input_pair1.upper == cell<layout>{20, 0, 1} || input_pair1.upper == cell<layout>{0, 0, 1}));
    CHECK((input_pair1.lower == cell<layout>{18, 1, 1} || input_pair1.lower == cell<layout>{2, 1, 1}));

    CHECK(input_pair2.type == layout::cell_type::INPUT);
    CHECK((input_pair2.upper == cell<layout>{20, 0, 1} || input_pair2.upper == cell<layout>{0, 0, 1}));
    CHECK((input_pair2.lower == cell<layout>{18, 1, 1} || input_pair2.lower == cell<layout>{2, 1, 1}));

    CHECK(output_pair.type == layout::cell_type::OUTPUT);
    CHECK(output_pair.upper == cell<layout>{10, 6, 0});
    CHECK(output_pair.lower == cell<layout>{10, 7, 0});
}

TEST_CASE("Bestagon fan-out BDL detection", "[detect-bdl-pairs]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{42, 21}, "Fan-out"};

    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 6, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({20, 7, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({19, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({23, 12, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({16, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 17, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({28, 17, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({8, 19, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({34, 19, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({4, 20, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({38, 20, 0}, sidb_technology::cell_type::NORMAL);

    const lattice lat{lyt};

    const auto input_bdl_pairs  = detect_bdl_pairs(lat, sidb_technology::cell_type::INPUT);
    const auto output_bdl_pairs = detect_bdl_pairs(lat, sidb_technology::cell_type::OUTPUT);

    REQUIRE(input_bdl_pairs.size() == 1);
    REQUIRE(output_bdl_pairs.size() == 2);

    const auto& input_pair   = input_bdl_pairs.front();
    const auto& output_pair1 = output_bdl_pairs[0];
    const auto& output_pair2 = output_bdl_pairs[1];

    CHECK(input_pair.type == layout::cell_type::INPUT);
    CHECK(input_pair.upper == cell<layout>{2, 1, 0});
    CHECK(input_pair.lower == cell<layout>{4, 2, 0});

    CHECK(output_pair1.type == layout::cell_type::OUTPUT);
    CHECK((output_pair1.upper == cell<layout>{10, 18, 0} || output_pair1.upper == cell<layout>{32, 18, 0}));
    CHECK((output_pair1.lower == cell<layout>{8, 19, 0} || output_pair1.lower == cell<layout>{34, 19, 0}));

    CHECK(output_pair2.type == layout::cell_type::OUTPUT);
    CHECK((output_pair2.upper == cell<layout>{10, 18, 0} || output_pair2.upper == cell<layout>{32, 18, 0}));
    CHECK((output_pair2.lower == cell<layout>{8, 19, 0} || output_pair2.lower == cell<layout>{34, 19, 0}));
}
