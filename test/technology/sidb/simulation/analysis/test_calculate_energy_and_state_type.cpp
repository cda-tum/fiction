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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author OpenAI Codex
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/types.hpp>

#include <algorithm>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

TEST_CASE("Single SiDB", "[calculate-energy-and-state-type]")
{
    const auto bestagon_and = blueprints::bestagon_and_gate();

    auto bii = bdl_input_iterator{bestagon_and};

    // set input index 1
    bii = 1;

    const auto output_bdls = detect_bdl_pairs(*bii, dot_tag::OUTPUT);

    const auto simulation_results = quickexact(*bii);

    const auto simulated_charge_distributions = simulation_results.charge_distributions;

    REQUIRE(!simulated_charge_distributions.empty());

    const auto energy_distribution = calculate_energy_distribution(simulated_charge_distributions);

    const auto energy_state_with_state_info = calculate_energy_and_state_type_with_kinks_accepted(
        energy_distribution, simulated_charge_distributions, output_bdls, std::vector<tt>{create_and_tt()}, 1);

    REQUIRE(energy_state_with_state_info.size() == 5);

    std::set<double> energy_values{};

    double energy_value = std::numeric_limits<double>::min();

    for (const auto& [energy, state] : energy_state_with_state_info)
    {
        // test if energy values are sorted
        CHECK(energy > energy_value);
        energy_value = energy;
        energy_values.insert(energy);
    }

    CHECK(energy_values.size() == 5);

    const auto ground_state   = energy_state_with_state_info.front();
    const auto first_excited  = energy_state_with_state_info[1];
    const auto second_excited = energy_state_with_state_info[2];
    const auto third_excited  = energy_state_with_state_info[3];
    const auto fourth_excited = energy_state_with_state_info[4];

    CHECK_THAT(ground_state.first, Catch::Matchers::WithinAbs(0.9915, 0.0001));
    CHECK(ground_state.second == state_type::ACCEPTED);

    CHECK_THAT(first_excited.first, Catch::Matchers::WithinAbs(0.9966, 0.0001));
    CHECK(first_excited.second == state_type::ACCEPTED);

    CHECK_THAT(second_excited.first, Catch::Matchers::WithinAbs(1.2827, 0.0001));
    CHECK(second_excited.second == state_type::REJECTED);

    CHECK_THAT(third_excited.first, Catch::Matchers::WithinAbs(1.2917, 0.0001));
    CHECK(third_excited.second == state_type::REJECTED);

    CHECK_THAT(fourth_excited.first, Catch::Matchers::WithinAbs(1.2976, 0.0001));
    CHECK(fourth_excited.second == state_type::REJECTED);

    const auto kinks_rejected = calculate_energy_and_state_type_with_kinks_rejected(
        *bii, energy_distribution, simulated_charge_distributions, {create_and_tt()}, 1,
        detect_bdl_wires(bestagon_and, {}, bdl_wire_selection::INPUT),
        detect_bdl_wires(bestagon_and, {}, bdl_wire_selection::OUTPUT));
    REQUIRE(kinks_rejected.size() == 5);
    for (std::size_t i = 0; i < kinks_rejected.size(); ++i)
    {
        CAPTURE(i);
        CHECK(kinks_rejected[i].first == energy_state_with_state_info[i].first);
        CHECK(kinks_rejected[i].second == (i == 0 ? state_type::ACCEPTED : state_type::REJECTED));
    }
}

TEST_CASE("Energy labeling propagates invalid lattice basis errors", "[calculate-energy-and-state-type]")
{
    layout       lyt{};
    lattice_site invalid{};
    invalid.z = 2;
    lyt.assign_sidb(invalid, dot_tag::OUTPUT);
    lyt.assign_sidb({2, 0, 0}, dot_tag::OUTPUT);
    const std::vector distributions{charge_distribution{lyt}};
    const auto        energies = calculate_energy_distribution(distributions);

    CHECK_THROWS_AS(calculate_energy_and_state_type_with_kinks_rejected(lyt, energies, distributions,
                                                                        std::vector{create_id_tt()}, 0, {}, {}),
                    std::out_of_range);
}

TEST_CASE("Kinks-rejected labels count each matching charge distribution once", "[calculate-energy-and-state-type]")
{
    const auto lyt          = blueprints::bestagon_and_gate();
    const auto input_wires  = detect_bdl_wires(lyt, {}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, {}, bdl_wire_selection::OUTPUT);
    auto       bii          = bdl_input_iterator{lyt};
    bii                     = 1;

    const auto groundstates = quickexact(*bii).groundstates();
    REQUIRE(!groundstates.empty());
    REQUIRE(output_wires.size() == 1);

    auto       accepted     = groundstates.front();
    auto       wrong_output = accepted;
    const auto output_pairs = detect_bdl_pairs(*bii, dot_tag::OUTPUT);
    REQUIRE(output_pairs.size() == 1);
    wrong_output.assign_charge_state(output_pairs.front().lower, model::charge_state::NEGATIVE);
    wrong_output.assign_charge_state(output_pairs.front().upper, model::charge_state::NEUTRAL);

    auto       kinked = accepted;
    const auto internal_pair =
        std::ranges::find_if(output_wires.front().pairs, [](const auto& pair) { return pair.type == dot_tag::NORMAL; });
    REQUIRE(internal_pair != output_wires.front().pairs.cend());
    kinked.assign_charge_state(internal_pair->lower, accepted.get_charge_state(internal_pair->upper));
    kinked.assign_charge_state(internal_pair->upper, accepted.get_charge_state(internal_pair->lower));

    const std::vector<tt> spec{create_and_tt()};
    CHECK(calculate_energy_and_state_type_with_kinks_accepted(calculate_energy_distribution({kinked}), {kinked},
                                                              output_pairs, spec, 1)
              .front()
              .second == state_type::ACCEPTED);

    accepted.assign_energy(3.0);
    wrong_output.assign_energy(1.0);
    kinked.assign_energy(2.0);
    std::vector distributions{accepted, wrong_output, kinked};

    SECTION("Ascending energies despite unsorted charge distributions")
    {
        const energy_and_state_type expected{{1.0, state_type::REJECTED},
                                             {2.0, state_type::REJECTED},
                                             {3.0, state_type::ACCEPTED}};
        CHECK(calculate_energy_and_state_type_with_kinks_rejected(*bii, calculate_energy_distribution(distributions),
                                                                  distributions, spec, 1, input_wires,
                                                                  output_wires) == expected);
    }
    SECTION("Equal energies retain each label regardless of input order")
    {
        for (auto& cd : distributions)
        {
            cd.assign_energy(1.0);
        }
        const auto energies = calculate_energy_distribution(distributions);
        for (auto order = 0; order < 2; ++order)
        {
            const auto labels = calculate_energy_and_state_type_with_kinks_rejected(*bii, energies, distributions, spec,
                                                                                    1, input_wires, output_wires);
            REQUIRE(labels.size() == distributions.size());
            CHECK(std::ranges::count(labels, std::pair{1.0, state_type::ACCEPTED}) == 1);
            CHECK(std::ranges::count(labels, std::pair{1.0, state_type::REJECTED}) == 2);
            CHECK_THAT(occupation_probability_gate_based(labels, 300), Catch::Matchers::WithinAbs(2.0 / 3.0, 1e-12));
            std::ranges::reverse(distributions);
        }
    }
    SECTION("An energy without matching distributions produces no labels")
    {
        energy_distribution energies{};
        energies.add_energy_state({4.0, 1});
        CHECK(calculate_energy_and_state_type_with_kinks_rejected(*bii, energies, distributions, spec, 1, input_wires,
                                                                  output_wires)
                  .empty());
    }
    SECTION("Only accepted states have zero error probability")
    {
        const std::vector only_accepted{accepted};
        const auto        labels = calculate_energy_and_state_type_with_kinks_rejected(
            *bii, calculate_energy_distribution(only_accepted), only_accepted, spec, 1, input_wires, output_wires);
        REQUIRE(labels.size() == 1);
        CHECK(occupation_probability_gate_based(labels, 300) == 0.0);
    }
    SECTION("Only rejected states have unit error probability")
    {
        const std::vector only_rejected{wrong_output, kinked};
        const auto        labels = calculate_energy_and_state_type_with_kinks_rejected(
            *bii, calculate_energy_distribution(only_rejected), only_rejected, spec, 1, input_wires, output_wires);
        REQUIRE(labels.size() == 2);
        CHECK(occupation_probability_gate_based(labels, 300) == 1.0);
    }
}
