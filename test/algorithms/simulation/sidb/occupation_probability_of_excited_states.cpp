//
// Created by Jan Drewniok on 06.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp>
#include <fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp>
#include <fiction/utils/math_utils.hpp>

using namespace fiction;

TEST_CASE("occupation probability of all erroneous charge distribution states", "[occupation_probability_erroneous]")
{
    SECTION("empty energy distribution")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 0.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 0.0);

        const std::map<double, uint64_t> distribution{{0.1, 2}};

        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 10), 6) == 0.0);
        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 0.01), 6) == 0.0);
    }

    SECTION("one state with degeneracy")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::TRANSPARENT);
        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::ERRONEOUS);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 0.5);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 0.5);

        const std::map<double, uint64_t> distribution{{0.1, 2}};

        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 10), 6) == 0);
        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 0.01), 6) == 0);
    }

    SECTION("a few states with degeneracy")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::TRANSPARENT);
        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::ERRONEOUS);

        energy_distribution_transparent_erroneous.emplace_back(0.2, sidb_charge_distribution_type::TRANSPARENT);
        energy_distribution_transparent_erroneous.emplace_back(0.2, sidb_charge_distribution_type::TRANSPARENT);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.001) == 0.5);

        const std::map<double, uint64_t> distribution{{0.1, 2}, {0.2, 2}};
        CHECK(occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(round_to_n_decimal_places(
                  occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10E10), 6) == 0.25);
        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 10E10), 6) == 0.5);
    }

    SECTION("a few states with degeneracy, different oder of the energy_distribution_transparent_erroneous entries")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};

        energy_distribution_transparent_erroneous.emplace_back(0.2, sidb_charge_distribution_type::TRANSPARENT);
        energy_distribution_transparent_erroneous.emplace_back(0.2, sidb_charge_distribution_type::TRANSPARENT);

        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::TRANSPARENT);
        energy_distribution_transparent_erroneous.emplace_back(0.1, sidb_charge_distribution_type::ERRONEOUS);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.001) == 0.5);
        const std::map<double, uint64_t> distribution{{0.2, 2}, {0.1, 2}};
        CHECK(occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(round_to_n_decimal_places(
                  occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10E10), 2) == 0.25);
        CHECK(round_to_n_decimal_places(occupation_probability_non_gate_based(distribution, 10E10), 6) == 0.5);
    }

    SECTION("one state / true")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(1, sidb_charge_distribution_type::TRANSPARENT);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 1000) == 0.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 0.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 0.0);

        const std::map<double, uint64_t> distribution{{1, 1}};

        CHECK(occupation_probability_non_gate_based(distribution, 1000) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 10) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 0.01) == 0.0);
    }

    SECTION("one state / false")
    {
        sidb_energy_and_state_type energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(1, sidb_charge_distribution_type::ERRONEOUS);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 1000) == 1.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 1.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 1.0);

        const std::map<double, uint64_t> distribution{{1, 1}};

        CHECK(occupation_probability_non_gate_based(distribution, 1000) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 10) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 0.01) == 0.0);
    }
}
