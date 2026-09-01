//
// Created by Jan Drewniok on 03.05.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/utils/physically_valid_parameters.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Determine physical parameters for CDS of SiQAD Y-shaped AND gate, 10 input combination",
          "[determine-physically-valid-parameters]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type({-2, -1, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 1, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 4, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 5, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 7, 1}, sidb::sidb_technology::cell_type::NORMAL);

    sidb::model::simulation_parameters sim_params{};
    sim_params.base = 2;

    sidb::surfaces::charge_distribution_surface cds{lyt, sim_params};

    sidb::simulation::logic::operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;

    op_domain_params.sweep_dimensions = {sidb::simulation::logic::operational_domain_value_range{
                                             sidb::simulation::logic::sweep_parameter::EPSILON_R, 4.1, 6.0, 0.1},
                                         sidb::simulation::logic::operational_domain_value_range{
                                             sidb::simulation::logic::sweep_parameter::LAMBDA_TF, 4.1, 6.0, 0.1}};

    SECTION("Using the typical ground state as given CDS")
    {
        op_domain_params.sweep_dimensions[0].step = 0.3;
        op_domain_params.sweep_dimensions[1].step = 0.3;

        cds.assign_charge_state({-2, -1, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({6, 4, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({6, 5, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({6, 7, 1}, sidb::model::charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters.size() == 43);

        op_domain_params.operational_params.sim_engine = sidb::simulation::engine::EXGS;
        const auto valid_parameters_exgs = sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_exgs.size() == 43);

        op_domain_params.operational_params.sim_engine = sidb::simulation::engine::QUICKSIM;
        const auto valid_parameters_quicksim =
            sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters_quicksim.size() == 43);
    }

    SECTION("Using the 2nd excited charge distribution for default physical parameters as given CDS")
    {
        cds.assign_charge_state({-2, -1, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({6, 4, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({6, 5, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({6, 7, 1}, sidb::model::charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters.size() == 98);

        const auto p1 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.9, 5.5}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value()) == 1);

        const auto p2 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.8, 4.4}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value()) == 0);

        const auto p3 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.8, 4.4}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value()) == 0);

        const auto p4 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{6.0, 6.0}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value()) == 1);
    }
}

TEST_CASE(
    "Determine physical parameters for CDS (default physical parameters) of Bestagon AND gate, 10 input combination",
    "[determine-physically-valid-parameters], [quality]")
{
    auto bestagon_and = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();

    bestagon_and.assign_cell_type({36, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    bestagon_and.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);

    sidb::model::simulation_parameters sim_params{};
    sim_params.base = 2;

    sidb::surfaces::charge_distribution_surface cds{bestagon_and, sim_params};

    sidb::simulation::logic::operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;

    SECTION("Using the ground state of default physical parameters as given CDS, two dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {sidb::simulation::logic::operational_domain_value_range{
                                                 sidb::simulation::logic::sweep_parameter::EPSILON_R, 5.0, 5.9, 0.1},
                                             sidb::simulation::logic::operational_domain_value_range{
                                                 sidb::simulation::logic::sweep_parameter::LAMBDA_TF, 5.0, 5.9, 0.1}};

        cds.assign_charge_state({38, 0, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 0}, sidb::model::charge_state::NEGATIVE);

        cds.assign_charge_state({6, 2, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({32, 2, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({8, 3, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({30, 3, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({12, 4, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({26, 4, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({14, 5, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({24, 5, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({19, 8, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({18, 9, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({23, 9, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({18, 11, 1}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({19, 13, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({20, 14, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({24, 15, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({26, 16, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({30, 17, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({32, 18, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({36, 19, 0}, sidb::model::charge_state::NEGATIVE);

        cds.update_after_charge_change();

        const auto valid_parameters = sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        REQUIRE(valid_parameters.size() == 100);

        const auto p1 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.6, 5.0}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value()) == 0);

        const auto p2 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.0, 5.9}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value()) == 2);

        const auto p3 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.4, 5.3}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value()) == 1);

        const auto p4 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.8, 5.3}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value()) == 0);
    }

    SECTION("Using the ground state of default physical parameters as given CDS, three dimensional sweep")
    {
        op_domain_params.sweep_dimensions = {
            sidb::simulation::logic::operational_domain_value_range{sidb::simulation::logic::sweep_parameter::EPSILON_R,
                                                                    5.5, 5.7, 0.1},
            sidb::simulation::logic::operational_domain_value_range{sidb::simulation::logic::sweep_parameter::LAMBDA_TF,
                                                                    5.0, 5.2, 0.1},
            sidb::simulation::logic::operational_domain_value_range{sidb::simulation::logic::sweep_parameter::MU_MINUS,
                                                                    -0.33, -0.31, 0.01}};

        cds.assign_charge_state({38, 0, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 0}, sidb::model::charge_state::NEGATIVE);

        cds.assign_charge_state({6, 2, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({32, 2, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({8, 3, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({30, 3, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({12, 4, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({26, 4, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({14, 5, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({24, 5, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({19, 8, 0}, sidb::model::charge_state::NEUTRAL);
        cds.assign_charge_state({18, 9, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({23, 9, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({18, 11, 1}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({19, 13, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({20, 14, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({24, 15, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({26, 16, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({30, 17, 0}, sidb::model::charge_state::NEGATIVE);
        cds.assign_charge_state({32, 18, 0}, sidb::model::charge_state::NEUTRAL);

        cds.assign_charge_state({36, 19, 0}, sidb::model::charge_state::NEGATIVE);

        cds.update_after_charge_change();

        const auto valid_parameters = sidb::simulation::utils::physically_valid_parameters(cds, op_domain_params);
        REQUIRE(valid_parameters.size() == 27);
        const auto p1 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.6, 5.0, -0.32}});
        REQUIRE(p1.has_value());
        CHECK(std::get<0>(p1.value()) == 0);

        const auto p2 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.6, 5.0, -0.33}});
        REQUIRE(p2.has_value());
        CHECK(std::get<0>(p2.value()) == 0);

        const auto p3 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.6, 5.0, -0.31}});
        REQUIRE(p3.has_value());
        CHECK(std::get<0>(p3.value()) == 1);

        const auto p4 = valid_parameters.contains(sidb::simulation::logic::parameter_point{{5.7, 5.2, -0.33}});
        REQUIRE(p4.has_value());
        CHECK(std::get<0>(p4.value()) == 0);
    }
}
