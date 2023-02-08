//
// Created by Jan Drewniok on 06.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <fiction/algorithms/simulation/sidb/occupation_function_erroneous.hpp>

using namespace fiction;

TEST_CASE("occupation probability of all erroneous charge distribution states", "[occupation_function_erroneous]")
{
    SECTION("a few states with degeneracy")
    {
        std::map<double, std::pair<uint64_t, bool>> test_map{};
        test_map[0] = std::make_pair(1, true);
        test_map[0.2] = std::make_pair(3, true);
        test_map[0.0004] = std::make_pair(6, false);
        test_map[1.0] = std::make_pair(4, true);
        test_map[0.8] = std::make_pair(5, false);

        CHECK(occupation_propability_erroneous(test_map, 10) > 0.0);
        CHECK(occupation_propability_erroneous(test_map, 0.01) > 0.0);
    }

    SECTION("one state / true")
    {
        std::map<double, std::pair<uint64_t, bool>> test_map{};
        test_map[0] = std::make_pair(1, true);

        CHECK(occupation_propability_erroneous(test_map, 1000) == 0.0);
        CHECK(occupation_propability_erroneous(test_map, 10) == 0.0);
        CHECK(occupation_propability_erroneous(test_map, 0.01) == 0.0);
    }

    SECTION("one state / false")
    {
        std::map<double, std::pair<uint64_t, bool>> test_map{};
        test_map[0] = std::make_pair(1, false);

        CHECK(occupation_propability_erroneous(test_map, 1000) == 1.0);
        CHECK(occupation_propability_erroneous(test_map, 10) == 1.0);
        CHECK(occupation_propability_erroneous(test_map, 0.01) == 1.0);
    }
}