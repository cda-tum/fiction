//
// Created by Jan Drewniok on 06.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <fiction/algorithms/simulation/sidb/occupation_function_pi.hpp>

using namespace fiction;

TEST_CASE("occupation function for simple energy distribution", "[occupation_function_pi]")
{


    SECTION("no states")
    {
        std::map<double, uint64_t> test_map{};
    }

    SECTION("a few states with degeneracy")
    {
        std::map<double, uint64_t> test_map{};
        test_map.insert(std::make_pair(0, 1));
        test_map.insert(std::make_pair(0.2, 3));
        test_map.insert(std::make_pair(0.4, 4));
        test_map.insert(std::make_pair(0.5, 6));
        test_map.insert(std::make_pair(1.5, 1));
        test_map.insert(std::make_pair(1.7, 0));

        CHECK(occupation_propability_pi(test_map, 10, 3) > 0);
        CHECK(occupation_propability_pi(test_map, 10, 5) == 0);
        CHECK(occupation_propability_pi(test_map, 0, 0) == 1);

        test_map.insert(std::make_pair(0, 2));
        CHECK(occupation_propability_pi(test_map, 0, 0) == 1);
    }
}