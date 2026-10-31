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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>

#include <algorithm>
#include <optional>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::simulation::logic;

TEST_CASE("Empty layout BDL detection", "[detect-bdl-pairs]")
{
    const layout lyt{};

    const auto result = detect_bdl_pairs(lyt, dot_tag::NORMAL);

    CHECK(result.empty());
}

TEST_CASE("BDL pair distance bounds are inclusive", "[detect-bdl-pairs]")
{
    /** @brief A single pair whose physical distance defines both bounds. */
    layout lyt{};
    lyt.assign_sidb({0, 0, 0}, dot_tag::INPUT);
    lyt.assign_sidb({2, 0, 0}, dot_tag::INPUT);

    /** @brief Exact distance used by the public pair-detection contract. */
    const auto distance = lyt.get_lattice().nm_distance({0, 0, 0}, {2, 0, 0});

    CHECK(detect_bdl_pairs(lyt, dot_tag::INPUT,
                           detect_bdl_pairs_params{.minimum_distance = distance, .maximum_distance = distance}) ==
          std::vector<bdl_pair>{{dot_tag::INPUT, {0, 0, 0}, {2, 0, 0}}});
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

    const detect_bdl_pairs_params params{.minimum_distance = 0.2, .maximum_distance = 2.2};

    const auto& lat = lyt;

    const auto input_bdl_pairs  = detect_bdl_pairs(lat, dot_tag::INPUT, params);
    const auto output_bdl_pairs = detect_bdl_pairs(lat, dot_tag::OUTPUT, params);
    const auto normal_bdl_pairs = detect_bdl_pairs(lat, dot_tag::NORMAL, params);

    const auto all_bdl_pairs = detect_bdl_pairs(lat, std::nullopt, params);

    CHECK(input_bdl_pairs == std::vector<bdl_pair>{{dot_tag::INPUT, {-1, 0, 0}, {3, 0, 0}}});
    CHECK(output_bdl_pairs == std::vector<bdl_pair>{{dot_tag::OUTPUT, {18, 0, 0}, {20, 0, 0}}});
    CHECK(normal_bdl_pairs ==
          std::vector<bdl_pair>{{dot_tag::NORMAL, {6, 0, 0}, {8, 0, 0}}, {dot_tag::NORMAL, {12, 0, 0}, {14, 0, 0}}});
    CHECK(all_bdl_pairs == std::vector<bdl_pair>{{dot_tag::INPUT, {-1, 0, 0}, {3, 0, 0}},
                                                 {dot_tag::OUTPUT, {18, 0, 0}, {20, 0, 0}},
                                                 {dot_tag::NORMAL, {6, 0, 0}, {8, 0, 0}},
                                                 {dot_tag::NORMAL, {12, 0, 0}, {14, 0, 0}}});
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

    SECTION("Default and 0.5 nm minimum distances")
    {
        /** @brief Both bounds exclude adjacent atomic-wire dots. */
        const auto minimum_distance = GENERATE(detect_bdl_pairs_params{}.minimum_distance, 0.5);
        CAPTURE(minimum_distance);
        params.minimum_distance = minimum_distance;

        CHECK(detect_bdl_pairs(lat, dot_tag::INPUT, params).empty());
        CHECK(detect_bdl_pairs(lat, dot_tag::OUTPUT, params).empty());
        CHECK(detect_bdl_pairs(lat, dot_tag::NORMAL, params) ==
              std::vector<bdl_pair>{{dot_tag::NORMAL, {2, 0, 0}, {4, 0, 0}}, {dot_tag::NORMAL, {3, 0, 0}, {5, 0, 0}}});
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

        CHECK((normal_bdl_pairs == std::vector<bdl_pair>{{dot_tag::NORMAL, {2, 0, 0}, {3, 0, 0}},
                                                         {dot_tag::NORMAL, {4, 0, 0}, {5, 0, 0}}} ||
               normal_bdl_pairs == std::vector<bdl_pair>{{dot_tag::NORMAL, {2, 0, 0}, {5, 0, 0}},
                                                         {dot_tag::NORMAL, {3, 0, 0}, {4, 0, 0}}}));

        /** @brief All normal dots, retaining multiplicity to detect reused endpoints. */
        std::vector<lattice_site> paired_sites{normal_pair1.upper, normal_pair1.lower, normal_pair2.upper,
                                               normal_pair2.lower};
        std::ranges::sort(paired_sites);
        CHECK(paired_sites == std::vector<lattice_site>{{2, 0, 0}, {3, 0, 0}, {4, 0, 0}, {5, 0, 0}});
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

    // set default minimum distance to 0 for testing
    detect_bdl_pairs_params params{.minimum_distance = 0};

    const auto& lat = lyt;

    SECTION("Default and 1 nm maximum distances")
    {
        /** @brief Both bounds admit the 0.768 nm input and output pairs. */
        const auto maximum_distance = GENERATE(detect_bdl_pairs_params{}.maximum_distance, 1.0);
        CAPTURE(maximum_distance);
        params.maximum_distance = maximum_distance;

        CHECK(detect_bdl_pairs(lat, dot_tag::INPUT, params) ==
              std::vector<bdl_pair>{{dot_tag::INPUT, {0, 0, 0}, {2, 0, 0}}});
        CHECK(detect_bdl_pairs(lat, dot_tag::OUTPUT, params) ==
              std::vector<bdl_pair>{{dot_tag::OUTPUT, {18, 0, 0}, {20, 0, 0}}});
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
    /** @brief AND gate with separated input pairs and one output pair. */
    const auto lyt = blueprints::siqad_and_gate();

    CHECK(detect_bdl_pairs(lyt, dot_tag::INPUT) ==
          std::vector<bdl_pair>{{dot_tag::INPUT, {0, 0, 1}, {2, 1, 1}}, {dot_tag::INPUT, {20, 0, 1}, {18, 1, 1}}});
    CHECK(detect_bdl_pairs(lyt, dot_tag::OUTPUT) == std::vector<bdl_pair>{{dot_tag::OUTPUT, {10, 6, 0}, {10, 7, 0}}});
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
        CHECK(detect_bdl_pairs(lat, dot_tag::INPUT) == std::vector<bdl_pair>{{dot_tag::INPUT, {2, 1, 0}, {4, 2, 0}}});
        CHECK(detect_bdl_pairs(lat, dot_tag::OUTPUT) ==
              std::vector<bdl_pair>{{dot_tag::OUTPUT, {10, 18, 0}, {8, 19, 0}},
                                    {dot_tag::OUTPUT, {32, 18, 0}, {34, 19, 0}}});
    }

    SECTION("Detect all BDL pairs")
    {
        CHECK(detect_bdl_pairs(lat) == std::vector<bdl_pair>{{dot_tag::INPUT, {2, 1, 0}, {4, 2, 0}},
                                                             {dot_tag::OUTPUT, {10, 18, 0}, {8, 19, 0}},
                                                             {dot_tag::OUTPUT, {32, 18, 0}, {34, 19, 0}},
                                                             {dot_tag::NORMAL, {8, 3, 0}, {10, 4, 0}},
                                                             {dot_tag::NORMAL, {14, 5, 0}, {16, 6, 0}},
                                                             {dot_tag::NORMAL, {20, 7, 0}, {21, 8, 0}},
                                                             {dot_tag::NORMAL, {16, 16, 0}, {14, 17, 0}},
                                                             {dot_tag::NORMAL, {26, 16, 0}, {28, 17, 0}}});
    }
}
