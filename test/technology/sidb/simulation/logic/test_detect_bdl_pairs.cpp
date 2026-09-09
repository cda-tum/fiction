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
 * @brief Tests for `fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <optional>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::simulation::logic;

TEST_CASE("Empty layout BDL detection", "[detect-bdl-pairs]")
{
    const layout lyt{};

    const auto result = detect_bdl_pairs(lyt, dot_tag::NORMAL);

    CHECK(result.empty());
}

TEST_CASE("BDL wire", "[detect-bdl-pairs]")
{
    layout lyt{lattice::si_100_2x1(), "Atomic wire"};

    lyt.assign_sidb({-1, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({3, 0, 0}, dot_tag::INPUT);

    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({12, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({18, 0, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({20, 0, 0}, dot_tag::OUTPUT);

    // output perturber
    lyt.assign_sidb({24, 0, 0}, dot_tag::NORMAL);

    detect_bdl_pairs_params params{};

    params.minimum_distance = 0.2;
    params.maximum_distance = 2.2;

    const auto& lat = lyt;

    const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
    const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);
    const auto normal_bdl_pairs = detect_bdl_pairs(lat, dot_tag::NORMAL, params);

    const auto all_bdl_pairs = detect_bdl_pairs(lat, std::nullopt, params);

    REQUIRE(input_bdl_pairs.size() == 1);
    REQUIRE(output_bdl_pairs.size() == 1);
    REQUIRE(normal_bdl_pairs.size() == 2);
    REQUIRE(all_bdl_pairs.size() == 4);
}

TEST_CASE("Atomic wire BDL detection", "[detect-bdl-pairs]")
{
    layout lyt{lattice::si_100_2x1(), "Atomic wire"};

    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({1, 0, 0}, dot_tag::INPUT);

    lyt.assign_sidb({2, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({3, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({5, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({6, 0, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({7, 0, 0}, dot_tag::OUTPUT);

    detect_bdl_pairs_params params{};

    const auto& lat = lyt;

    SECTION("default minimum distance")
    {
        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, dot_tag::NORMAL, params);

        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(normal_pair1.type == dot_tag::NORMAL);
        CHECK((normal_pair1.upper == lattice_site{2, 0, 0} || normal_pair1.upper == lattice_site{3, 0, 0}));
        CHECK((normal_pair1.lower == lattice_site{4, 0, 0} || normal_pair1.lower == lattice_site{5, 0, 0}));

        CHECK(normal_pair2.type == dot_tag::NORMAL);
        CHECK((normal_pair2.upper == lattice_site{2, 0, 0} || normal_pair2.upper == lattice_site{3, 0, 0}));
        CHECK((normal_pair2.lower == lattice_site{4, 0, 0} || normal_pair2.lower == lattice_site{5, 0, 0}));
    }
    SECTION("0.5 nm as minimum distance")
    {
        params.minimum_distance = 0.5;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, dot_tag::NORMAL, params);

        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(normal_pair1.type == dot_tag::NORMAL);
        CHECK((normal_pair1.upper == lattice_site{2, 0, 0} || normal_pair1.upper == lattice_site{3, 0, 0}));
        CHECK((normal_pair1.lower == lattice_site{4, 0, 0} || normal_pair1.lower == lattice_site{5, 0, 0}));

        CHECK(normal_pair2.type == dot_tag::NORMAL);
        CHECK((normal_pair2.upper == lattice_site{2, 0, 0} || normal_pair2.upper == lattice_site{3, 0, 0}));
        CHECK((normal_pair2.lower == lattice_site{4, 0, 0} || normal_pair2.lower == lattice_site{5, 0, 0}));
    }
    SECTION("0 nm as minimum distance")
    {
        params.minimum_distance = 0;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);
        const auto normal_bdl_pairs = detect_bdl_pairs(lat, dot_tag::NORMAL, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);
        REQUIRE(normal_bdl_pairs.size() == 2);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        const auto& normal_pair1 = normal_bdl_pairs[0];
        const auto& normal_pair2 = normal_bdl_pairs[1];

        CHECK(input_pair.type == dot_tag::INPUT);
        CHECK(input_pair.upper == lattice_site{0, 0, 0});
        CHECK(input_pair.lower == lattice_site{1, 0, 0});

        CHECK(output_pair.type == dot_tag::OUTPUT);
        CHECK(output_pair.upper == lattice_site{6, 0, 0});
        CHECK(output_pair.lower == lattice_site{7, 0, 0});

        CHECK(normal_pair1.type == dot_tag::NORMAL);
        CHECK((normal_pair1.upper == lattice_site{2, 0, 0} || normal_pair1.upper == lattice_site{4, 0, 0}));
        CHECK((normal_pair1.lower == lattice_site{3, 0, 0} || normal_pair1.lower == lattice_site{5, 0, 0}));

        CHECK(normal_pair2.type == dot_tag::NORMAL);
        CHECK((normal_pair2.upper == lattice_site{2, 0, 0} || normal_pair2.upper == lattice_site{4, 0, 0}));
        CHECK((normal_pair2.lower == lattice_site{3, 0, 0} || normal_pair2.lower == lattice_site{5, 0, 0}));
    }
}

TEST_CASE("BDL wire BDL detection", "[detect-bdl-pairs]")
{
    layout lyt{lattice::si_100_2x1(), "BDL wire"};

    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 0, 0}, dot_tag::INPUT);

    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({12, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({18, 0, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({20, 0, 0}, dot_tag::OUTPUT);

    detect_bdl_pairs_params params{};
    // set default minimum distance to 0 for testing
    params.minimum_distance = 0;

    const auto& lat = lyt;

    SECTION("default maximum distance")
    {
        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        CHECK(input_pair.type == dot_tag::INPUT);
        CHECK(input_pair.upper == lattice_site{0, 0, 0});
        CHECK(input_pair.lower == lattice_site{2, 0, 0});

        CHECK(output_pair.type == dot_tag::OUTPUT);
        CHECK(output_pair.upper == lattice_site{18, 0, 0});
        CHECK(output_pair.lower == lattice_site{20, 0, 0});
    }
    SECTION("1 nm maximum distance")
    {
        params.maximum_distance = 1;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 1);

        const auto& input_pair  = input_bdl_pairs.front();
        const auto& output_pair = output_bdl_pairs.front();

        CHECK(input_pair.type == dot_tag::INPUT);
        CHECK(input_pair.upper == lattice_site{0, 0, 0});
        CHECK(input_pair.lower == lattice_site{2, 0, 0});

        CHECK(output_pair.type == dot_tag::OUTPUT);
        CHECK(output_pair.upper == lattice_site{18, 0, 0});
        CHECK(output_pair.lower == lattice_site{20, 0, 0});
    }
    SECTION("0.5 nm maximum distance")
    {
        params.maximum_distance = 0.5;

        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);

        // the maximum distance is too small to detect any BDL pairs
        REQUIRE(input_bdl_pairs.empty());
        REQUIRE(output_bdl_pairs.empty());
    }
}

TEST_CASE("SiQAD's AND gate BDL detection", "[detect-bdl-pairs]")
{
    layout lyt{lattice::si_100_2x1(), "AND gate"};

    lyt.assign_sidb({0, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({2, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({20, 0, 1}, dot_tag::INPUT);
    lyt.assign_sidb({18, 1, 1}, dot_tag::INPUT);

    lyt.assign_sidb({4, 2, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 3, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 3, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 2, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({10, 7, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({10, 9, 1}, dot_tag::NORMAL);

    const auto& lat = lyt;

    const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT);
    const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT);

    REQUIRE(input_bdl_pairs.size() == 2);
    REQUIRE(output_bdl_pairs.size() == 1);

    const auto& input_pair1 = input_bdl_pairs[0];
    const auto& input_pair2 = input_bdl_pairs[1];
    const auto& output_pair = output_bdl_pairs.front();

    CHECK(input_pair1.type == dot_tag::INPUT);
    CHECK((input_pair1.upper == lattice_site{20, 0, 1} || input_pair1.upper == lattice_site{0, 0, 1}));
    CHECK((input_pair1.lower == lattice_site{18, 1, 1} || input_pair1.lower == lattice_site{2, 1, 1}));

    CHECK(input_pair2.type == dot_tag::INPUT);
    CHECK((input_pair2.upper == lattice_site{20, 0, 1} || input_pair2.upper == lattice_site{0, 0, 1}));
    CHECK((input_pair2.lower == lattice_site{18, 1, 1} || input_pair2.lower == lattice_site{2, 1, 1}));

    CHECK(output_pair.type == dot_tag::OUTPUT);
    CHECK(output_pair.upper == lattice_site{10, 6, 0});
    CHECK(output_pair.lower == lattice_site{10, 7, 0});
}

TEST_CASE("Bestagon fan-out BDL detection", "[detect-bdl-pairs]")
{
    layout lyt{lattice::si_100_2x1(), "Fan-out"};

    lyt.assign_sidb({2, 1, 0}, dot_tag::INPUT);
    lyt.assign_sidb({4, 2, 0}, dot_tag::INPUT);

    lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 4, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 5, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 6, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({20, 7, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({21, 8, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({19, 12, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({23, 12, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({20, 14, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({16, 16, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({14, 17, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({26, 16, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({28, 17, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 18, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({8, 19, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({32, 18, 0}, dot_tag::OUTPUT);
    lyt.assign_sidb({34, 19, 0}, dot_tag::OUTPUT);

    lyt.assign_sidb({4, 20, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({38, 20, 0}, dot_tag::NORMAL);

    const auto& lat = lyt;

    SECTION("Detect different BDL pair types")
    {
        const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT);
        const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT);

        REQUIRE(input_bdl_pairs.size() == 1);
        REQUIRE(output_bdl_pairs.size() == 2);

        const auto& input_pair   = input_bdl_pairs.front();
        const auto& output_pair1 = output_bdl_pairs[0];
        const auto& output_pair2 = output_bdl_pairs[1];

        CHECK(input_pair.type == dot_tag::INPUT);
        CHECK(input_pair.upper == lattice_site{2, 1, 0});
        CHECK(input_pair.lower == lattice_site{4, 2, 0});

        CHECK(output_pair1.type == dot_tag::OUTPUT);
        CHECK((output_pair1.upper == lattice_site{10, 18, 0} || output_pair1.upper == lattice_site{32, 18, 0}));
        CHECK((output_pair1.lower == lattice_site{8, 19, 0} || output_pair1.lower == lattice_site{34, 19, 0}));

        CHECK(output_pair2.type == dot_tag::OUTPUT);
        CHECK((output_pair2.upper == lattice_site{10, 18, 0} || output_pair2.upper == lattice_site{32, 18, 0}));
        CHECK((output_pair2.lower == lattice_site{8, 19, 0} || output_pair2.lower == lattice_site{34, 19, 0}));
    }

    SECTION("Detect all BDL pairs")
    {
        const auto all_bdl_pairs = detect_bdl_pairs(lat);
        CHECK(all_bdl_pairs.size() == 8);
    }
}
