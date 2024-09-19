//
// Created by Jan Drewniok on 03.05.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/determine_physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Determine physical parameters for CDS of SiQAD Y-shaped AND gate, 10 input combination",
          "[determine-physically-valid-parameters]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type({-2, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 7, 1}, sidb_technology::cell_type::NORMAL);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{lyt, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;

    op_domain_params.sweep_dimensions = {operational_domain_value_range{sweep_parameter::EPSILON_R, 4.1, 6.0, 0.1},
                                         operational_domain_value_range{sweep_parameter::LAMBDA_TF, 4.1, 6.0, 0.1}};

    SECTION("Using the typical ground state as given CDS")
    {
        op_domain_params.sweep_dimensions[0].step = 0.3;
        op_domain_params.sweep_dimensions[1].step = 0.3;

        cds.assign_charge_state({-2, -1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 4, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 5, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 7, 1}, sidb_charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters.operational_values.size() == 43);

        op_domain_params.sim_engine      = sidb_simulation_engine::EXGS;
        const auto valid_parameters_exgs = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_exgs.operational_values.size() == 43);

        op_domain_params.sim_engine          = sidb_simulation_engine::QUICKSIM;
        const auto valid_parameters_quicksim = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_quicksim.operational_values.size() == 43);
    }

    SECTION("Using the 2nd excited charge distribution for default physical parameters as given CDS")
    {
        cds.assign_charge_state({-2, -1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 4, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 5, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 7, 1}, sidb_charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters.operational_values.size() == 98);

        const auto p1 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.9, 5.5}});
        REQUIRE(p1.has_value());
        CHECK(p1->second == 1);

        const auto p2 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.8, 4.4}});
        REQUIRE(p2.has_value());
        CHECK(p2->second == 0);

        const auto p3 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.8, 4.4}});
        REQUIRE(p3.has_value());
        CHECK(p3->second == 0);

        const auto p4 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{6.0, 6.0}});
        REQUIRE(p4.has_value());
        CHECK(p4->second == 1);
    }
}

TEST_CASE(
    "Determine physical parameters for CDS (default physical parameters) of Bestagon AND gate, 10 input combination",
    "[determine-physically-valid-parameters], [quality]")
{
    auto bestagon_and = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();

    bestagon_and.assign_cell_type({36, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    bestagon_and.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{bestagon_and, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;

    SECTION("Using the ground state of default physical parameters as given CDS, two dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {operational_domain_value_range{sweep_parameter::EPSILON_R, 5.0, 5.9, 0.1},
                                             operational_domain_value_range{sweep_parameter::LAMBDA_TF, 5.0, 5.9, 0.1}};

        cds.assign_charge_state({38, 0, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 0}, sidb_charge_state::NEGATIVE);

        cds.assign_charge_state({6, 2, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({32, 2, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({8, 3, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({30, 3, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({12, 4, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({26, 4, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({14, 5, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({24, 5, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 8, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({18, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({23, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({18, 11, 1}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 13, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({20, 14, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({24, 15, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({26, 16, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({30, 17, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({32, 18, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({36, 19, 0}, sidb_charge_state::NEGATIVE);

        cds.update_after_charge_change();

        const auto valid_parameters = determine_physically_valid_parameters(cds, op_domain_params);
        REQUIRE(valid_parameters.operational_values.size() == 100);

        const auto p1 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.6, 5.0}});
        REQUIRE(p1.has_value());
        CHECK(p1->second == 0);

        const auto p2 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.0, 5.9}});
        REQUIRE(p2.has_value());
        CHECK(p2->second == 2);

        const auto p3 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.4, 5.3}});
        REQUIRE(p3.has_value());
        CHECK(p3->second == 1);

        const auto p4 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.8, 5.3}});
        REQUIRE(p4.has_value());
        CHECK(p4->second == 0);
    }

    SECTION("Using the ground state of default physical parameters as given CDS, three dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {
            operational_domain_value_range{sweep_parameter::EPSILON_R, 5.5, 5.7, 0.1},
            operational_domain_value_range{sweep_parameter::LAMBDA_TF, 5.0, 5.2, 0.1},
            operational_domain_value_range{sweep_parameter::MU_MINUS, -0.33, -0.31, 0.01}};

        cds.assign_charge_state({38, 0, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 0}, sidb_charge_state::NEGATIVE);

        cds.assign_charge_state({6, 2, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({32, 2, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({8, 3, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({30, 3, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({12, 4, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({26, 4, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({14, 5, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({24, 5, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 8, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({18, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({23, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({18, 11, 1}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 13, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({20, 14, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({24, 15, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({26, 16, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({30, 17, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({32, 18, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({36, 19, 0}, sidb_charge_state::NEGATIVE);

        cds.update_after_charge_change();

        const auto valid_parameters = determine_physically_valid_parameters(cds, op_domain_params);
        REQUIRE(valid_parameters.operational_values.size() == 27);
        const auto p1 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.6, 5.0, -0.32}});
        REQUIRE(p1.has_value());
        CHECK(p1->second == 0);

        const auto p2 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.6, 5.0, -0.33}});
        REQUIRE(p2.has_value());
        CHECK(p2->second == 0);

        const auto p3 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.6, 5.0, -0.31}});
        REQUIRE(p3.has_value());
        CHECK(p3->second == 1);

        const auto p4 =
            detail::contains_parameter_point(valid_parameters.operational_values, parameter_point{{5.7, 5.2, -0.33}});
        REQUIRE(p4.has_value());
        CHECK(p4->second == 0);
    }
}
