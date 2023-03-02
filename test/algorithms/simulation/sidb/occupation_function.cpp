//
// Created by Jan Drewniok on 06.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/occupation_function.hpp>

using namespace fiction;

TEST_CASE("occupation probability of all erroneous charge distribution states", "[occupation_function_erroneous]")
{
    SECTION("one state with degeneracy")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10) == 0.5);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01) == 0.5);

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10, false) == 0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01, false) == 0);
    }

    SECTION("a few states with degeneracy")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.001) == 0.5);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.001, false) == 0.0);

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10E20) == 0.25);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10E20, false) == 0.5);
    }

    SECTION("a few states with degeneracy, different oder of the energy_distribution_transparent_erroneous entries")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.2, true));

        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, true));
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(0.1, false));

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.001) == 0.5);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.001, false) == 0.0);

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10E20) == 0.25);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10E20, false) == 0.5);
    }

    SECTION("one state / true")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(1, true));
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 1000) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01) == 0.0);

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 1000, false) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10, false) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01, false) == 0.0);
    }

    SECTION("one state / false")
    {
        std::vector<std::pair<double, bool>> energy_distribution_transparent_erroneous{};
        energy_distribution_transparent_erroneous.emplace_back(std::make_pair(1, false));

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 1000) == 1.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10) == 1.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01) == 1.0);

        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 1000, false) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 10, false) == 0.0);
        CHECK(occupation_probability(energy_distribution_transparent_erroneous, 0.01, false) == 0.0);
    }
}
