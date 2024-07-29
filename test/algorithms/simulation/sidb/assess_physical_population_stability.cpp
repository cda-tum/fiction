//
// Created by Jan Drewniok on 02.11.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Single SiDB", "[assess-physical-population-stability]")
{
    sidb_cell_clk_lyt_siqad lyt{};
    lyt.assign_cell_type({1, 1, 0}, sidb_technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    SECTION("Precision of distance_corresponding_to_potential is two")
    {
        const auto params = assess_physical_population_stability_params{sidb_simulation_parameters{2, -0.29}, 2};
        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 1);
        const auto& population_stability_detail = result.front();
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{1, 1, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEGATIVE_TO_NEUTRAL);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition == 0.29);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(0.77, 1e-5));
    }

    SECTION("Precision of distance_corresponding_to_potential is three")
    {
        const auto params = assess_physical_population_stability_params{sidb_simulation_parameters{2, -0.29}, 3};
        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 1);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{1, 1, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEGATIVE_TO_NEUTRAL);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition == 0.29);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(0.762, 1e-5));
    }
}

TEMPLATE_TEST_CASE("Three SiDBs with positive charge states", "[assess-physical-population-stability]",
                   sidb_cell_clk_lyt_siqad, cds_sidb_cell_clk_lyt_siqad)
{
    sidb_cell_clk_lyt_siqad lyt{};
    const auto              params = assess_physical_population_stability_params{};
    lyt.assign_cell_type({1, 1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    const auto result = assess_physical_population_stability(lat, params);
    REQUIRE(result.size() == 3);

    SECTION("Check correct energy order")
    {
        CHECK(result[0].system_energy < result[1].system_energy);
        CHECK(result[1].system_energy < result[2].system_energy);
    }

    SECTION("Ground state")
    {
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{2, 1, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEGATIVE_TO_NEUTRAL);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.43);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(0.56, 1e-5));
    }
    SECTION("1st excited state")
    {
        const auto& population_stability_detail = result[1];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{2, 1, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEGATIVE_TO_NEUTRAL);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.23);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(0.94, 1e-5));
    }

    SECTION("2nd excited state")
    {
        const auto& population_stability_detail = result[2];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{1, 1, 1});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.21);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(1.01, 1e-5));
    }
}

TEMPLATE_TEST_CASE("Bestagon AND gate", "[assess-physical-population-stability]", sidb_cell_clk_lyt_siqad,
                   cds_sidb_cell_clk_lyt_siqad)
{
    TestType lyt{};

    const auto params = assess_physical_population_stability_params{};

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({23, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    TestType lat{lyt};

    SECTION("no input specified")
    {
        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 8);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{2, 1, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.021);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(4.79, 1e-5));
    }

    SECTION("input 00")
    {
        lat.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::EMPTY);
        lat.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::EMPTY);
        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 2);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{14, 5, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.026);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(4.32, 1e-5));
    }

    SECTION("input 01")
    {
        lat.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::EMPTY);
        lat.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::EMPTY);

        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 4);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{32, 18, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.041);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(3.3, 1e-5));
    }

    SECTION("input 10")
    {
        lat.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::EMPTY);
        lat.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::EMPTY);

        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 8);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{19, 8, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.02);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(4.87, 1e-5));
    }

    SECTION("input 11")
    {
        lat.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::EMPTY);
        lat.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::EMPTY);

        const auto result = assess_physical_population_stability(lat, params);
        REQUIRE(result.size() == 2);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == siqad::coord_t{14, 5, 0});
        CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
        CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.026);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                     Catch::Matchers::WithinAbs(4.32, 1e-5));
    }
}
