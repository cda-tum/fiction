//
// Created by Jan Drewniok on 03.05.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/determine_physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/phmap_utils.hpp>

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
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension                              = sweep_parameter::EPSILON_R;
    op_domain_params.x_min                                    = 4.1;
    op_domain_params.x_max                                    = 6.0;
    op_domain_params.x_step                                   = 0.1;
    op_domain_params.y_dimension                              = sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                                    = 4.1;
    op_domain_params.y_max                                    = 6.0;
    op_domain_params.y_step                                   = 0.1;

    SECTION("Using the typical ground state as given CDS")
    {
        op_domain_params.x_step = 0.3;
        op_domain_params.y_step = 0.3;

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

        op_domain_params.operational_params.sim_engine = sidb_simulation_engine::EXGS;
        const auto valid_parameters_exgs               = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_exgs.operational_values.size() == 356);

        op_domain_params.operational_params.sim_engine = sidb_simulation_engine::QUICKSIM;
        const auto valid_parameters_quicksim           = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_quicksim.operational_values.size() == 356);
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
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.9, 5.5})
                  ->second == 1);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.8, 4.4})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.8, 4.4})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{6.0, 6.0})
                  ->second == 1);
    }
}

TEST_CASE(
    "Determine physical parameters for CDS (default physical parameters) of Bestagon AND gate, 10 input combination",
    "[determine-physically-valid-parameters], [quality]")
{
    auto lyt = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();

    lyt.assign_cell_type({36, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{lyt, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension                              = sweep_parameter::EPSILON_R;
    op_domain_params.x_min                                    = 5.0;
    op_domain_params.x_max                                    = 5.9;
    op_domain_params.x_step                                   = 0.1;
    op_domain_params.y_dimension                              = sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                                    = 5.0;
    op_domain_params.y_max                                    = 5.9;
    op_domain_params.y_step                                   = 0.1;

    SECTION("Using the ground state of default physical parameters as given CDS")
    {
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
        CHECK(valid_parameters.operational_values.size() == 100);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.6, 5.0})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.0, 5.9})
                  ->second == 2);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.4, 5.3})
                  ->second == 1);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.8, 5.3})
                  ->second == 0);
    }
}
