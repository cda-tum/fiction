//
// Created by Jan Drewniok on 06.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/calculating_energy_and_state_type.hpp>
#include <fiction/algorithms/simulation/sidb/occupation_probability_excited_states.hpp>

using namespace fiction;

TEST_CASE("occupation probability of all erroneous charge distribution states", "[occupation_probability_erroneous]")
{
    SECTION("one state with degeneracy")
    {
        energy_and_state_type energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 0.5);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 0.5);

        const std::map<double, uint64_t> distribution{{0.1, 2}};

        CHECK(occupation_probability_non_gate_based(distribution, 10) == 0);
        CHECK(occupation_probability_non_gate_based(distribution, 0.01) == 0);
    }

    SECTION("a few states with degeneracy")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.001) == 0.5);

        const std::map<double, uint64_t> distribution{{0.1, 2}, {0.2, 2}};
        CHECK(occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10E20) == 0.25);
        CHECK(occupation_probability_non_gate_based(distribution, 10E20) == 0.5);
    }

    SECTION("a few states with degeneracy, different oder of the energy_distribution_transparent_erroneous entries")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.001) == 0.5);
        const std::map<double, uint64_t> distribution{{0.2, 2}, {0.1, 2}};
        CHECK(occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10E20) == 0.25);
        CHECK(occupation_probability_non_gate_based(distribution, 10E20) == 0.5);
    }

    SECTION("one state / true")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(1, true));
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
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(1, false));

        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 1000) == 1.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 10) == 1.0);
        CHECK(occupation_probability_gate_based(energy_distribution_transparent_erroneous, 0.01) == 1.0);

        const std::map<double, uint64_t> distribution{{1, 1}};

        CHECK(occupation_probability_non_gate_based(distribution, 1000) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 10) == 0.0);
        CHECK(occupation_probability_non_gate_based(distribution, 0.01) == 0.0);
    }
}
