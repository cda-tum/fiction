//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout ExGS simulation", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    exgs_stats<TestType>             exgs_stats{};
    const sidb_simulation_parameters params{2, -0.32};

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

    CHECK(exgs_stats.valid_lyts.empty());
}

  TEMPLATE_TEST_CASE("Single SiDB ExGS simulation", "[ExGS]",
                    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
 {
     TestType lyt{{20, 10}};
     lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

     exgs_stats<TestType>             exgs_stats{};
     const sidb_simulation_parameters params{2, -0.32};

     exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

     REQUIRE(exgs_stats.valid_lyts.size() == 1);
     CHECK(exgs_stats.valid_lyts.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
 }

 TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with one negatively charge defect in proximity", "[ExGS]",
                    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
 {
     TestType lyt{{20, 10}};
     lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

     exgs_stats<TestType>             exgs_stats{};
     const sidb_simulation_parameters params{2, -0.32};

     //const sidb_defect{sidb_defect_type::UNKNOWN, -1.0};
     //std::unordered_map<typename TestType::cell, const sidb_defect> defects = {{3,0,0}, sidb_defect{}};
     exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

     REQUIRE(exgs_stats.valid_lyts.size() == 1);
     CHECK(exgs_stats.valid_lyts.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
 }

 TEMPLATE_TEST_CASE("ExGS simulation of a one BDL pair", "[ExGS]",
                    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
 {
     TestType lyt{{20, 10}};
     lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
     lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

     exgs_stats<TestType>             exgs_stats{};
     const sidb_simulation_parameters params{2, -0.25};

     exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

     REQUIRE(exgs_stats.valid_lyts.size() == 2);
     for (const auto&layouts : exgs_stats.valid_lyts)
     {
         uint64_t counter_negative = 0;
         uint64_t counter_neutral  = 0;
         for (uint64_t i = 0; i < 2; i++)
         {
             if (layouts.get_charge_state_by_index(i) == sidb_charge_state::NEGATIVE)
             {
                 counter_negative += 1;
             }
             else
             {
                 counter_neutral += 1;
             }
         }
         CHECK(counter_neutral == 1);
         CHECK(counter_negative == 1);
     }

 }

TEMPLATE_TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

    exgs_stats<TestType>             exgs_stats{};
    const sidb_simulation_parameters params{2, -0.32};

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
    auto size_before = exgs_stats.valid_lyts.size();

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);
    auto size_after = exgs_stats.valid_lyts.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!exgs_stats.valid_lyts.empty());

    const auto& charge_lyt_first = exgs_stats.valid_lyts.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({13, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.24602741408, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a one-pair BDL wire with two perturbers", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{50, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, TestType::cell_type::NORMAL);

    exgs_stats<TestType>             exgs_stats{};
    const sidb_simulation_parameters params{2, -0.32};

    charge_distribution_surface charge_layout_kon{lyt, params};

    charge_layout_kon.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({5, 0, 0}, sidb_charge_state::NEUTRAL);
    charge_layout_kon.assign_charge_state({7, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({15, 0, 0}, sidb_charge_state::NEGATIVE);

    charge_layout_kon.update_after_charge_change();

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

    REQUIRE(!exgs_stats.valid_lyts.empty());

    const auto& charge_lyt_first = exgs_stats.valid_lyts.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.1152574819, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB arrangement", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({-11, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, TestType::cell_type::NORMAL);

    exgs_stats<TestType>             exgs_stats{};
    const sidb_simulation_parameters params{2, -0.32};

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

    REQUIRE(!exgs_stats.valid_lyts.empty());

    const auto& charge_lyt_first = exgs_stats.valid_lyts.front();

    CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.31915040629512115, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB OR gate with input 01", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    exgs_stats<TestType>             exgs_stats{};
    const sidb_simulation_parameters params{2, -0.28};

    exhaustive_ground_state_simulation<TestType>(lyt, params, &exgs_stats);

    REQUIRE(!exgs_stats.valid_lyts.empty());
    const auto& charge_lyt_first = exgs_stats.valid_lyts.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.46621669, fiction::physical_constants::POP_STABILITY_ERR));
}
