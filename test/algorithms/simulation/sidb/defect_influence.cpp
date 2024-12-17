//
// Created by Jan Drewniok on 26.06.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/defect_influence.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/technology/sidb_nm_distance.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math_utils.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("Test influence distance function", "[maximum-defect-influence-position-and-distance]")
{
    SECTION("empty layout")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, 10.6, 5.9};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{}};
        const sidb_cell_clk_lyt_siqad lyt{};

        const sidb_100_cell_clk_lyt_siqad lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK(distance == 0);
        CHECK(defect_pos == coordinate<sidb_cell_clk_lyt_siqad>());
    }

    SECTION("layout with one SiDB")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{}, {2, 2}};

        sidb_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad> lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK_THAT(round_to_n_decimal_places(distance, 6),
                   Catch::Matchers::WithinAbs(0.665060, physical_constants::POP_STABILITY_ERR));
        CHECK((((defect_pos.x == -1) && (defect_pos.y == -1) && (defect_pos.z == 1)) ||
               ((defect_pos.x == 1) && (defect_pos.y == -1) && (defect_pos.z == 1))));
    }

    SECTION("layout with one SiDB, negative defect, smaller lambda_tf")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 1};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{}};
        sidb_cell_clk_lyt_siqad       lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_siqad lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK_THAT(round_to_n_decimal_places(distance, 4) -
                       round_to_n_decimal_places(sidb_nm_distance(lat, {0, 0, 0}, {-1, 0, 1}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("layout with one SiDB, negative defect, large lambda_tf")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 20};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{}, {2, 2}};
        sidb_cell_clk_lyt_siqad       lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_siqad lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK_THAT(round_to_n_decimal_places(distance, 4) -
                       round_to_n_decimal_places(sidb_nm_distance(lat, {0, 0, 0}, {0, 1, 0}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("layout with one pertuber and one DB pair, negative defect")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{}};
        sidb_cell_clk_lyt_siqad       lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_siqad lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK_THAT(round_to_n_decimal_places(distance, 4) -
                       round_to_n_decimal_places(sidb_nm_distance(lat, {6, 0, 0}, {10, 0, 0}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{2}};
        sidb_cell_clk_lyt_siqad       lyt{};

        lyt.assign_cell_type({10, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        lyt.assign_cell_type({2, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 4, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 5, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 7, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_siqad lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK(defect_pos.x == 12);
        CHECK(defect_pos.y == 4);
        CHECK(defect_pos.z == 1);

        CHECK_THAT(distance, Catch::Matchers::WithinAbs(2.8999201713, physical_constants::POP_STABILITY_ERR));

        // number of threads given by the hardware
        const sidb_defect high_screening{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 1};
        const defect_influence_params sim_params_high_screening{high_screening, sidb_simulation_parameters{2}};

        const auto [defect_pos_high_screening, distance_high_screening] =
            defect_influence(lat, sim_params_high_screening);

        CHECK(distance_high_screening < distance);
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01, using cube coordinate")
    {
        const sidb_defect             defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const defect_influence_params sim_params{defect, sidb_simulation_parameters{2}};
        sidb_cell_clk_lyt_cube        lyt{{30, 30}};

        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 0, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{0, 1, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 1, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 2, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 4, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 5, 1}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 7, 1}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_cube lat{lyt};

        const auto [defect_pos, distance] = defect_influence(lat, sim_params);
        CHECK(defect_pos.x == 12);
        CHECK(defect_pos.y == 9);
        CHECK(defect_pos.z == 0);

        CHECK_THAT(distance, Catch::Matchers::WithinAbs(2.8999201713, physical_constants::POP_STABILITY_ERR));
    }
}

TEST_CASE("Test influence distance function for Gate")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    const defect_influence_of_sidb_gate_params defect_avoidance_params_vacancy{
        defect_influence_params{sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9},
                                sidb_simulation_parameters{2},
                                {10, 0}}};

    const defect_influence_of_sidb_gate_params defect_avoidance_params_arsenic{
        defect_influence_params{sidb_defect{sidb_defect_type::UNKNOWN, 1, 9.7, 2.1}, sidb_simulation_parameters{2}}};

    const auto cell_distance_vacancy = defect_influence_of_sidb_gate(lyt, defect_avoidance_params_vacancy);

    const auto cell_distance_arsenic = defect_influence_of_sidb_gate(lyt, defect_avoidance_params_arsenic);

    CHECK(cell_distance_vacancy.first == siqad::coord_t{-4, 3, 1});
    CHECK(cell_distance_arsenic.first == siqad::coord_t{3, 6, 0});
    CHECK_THAT(std::abs(cell_distance_vacancy.second - 2.769), Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(cell_distance_arsenic.second - 2.376), Catch::Matchers::WithinAbs(0.00, 0.01));
}
