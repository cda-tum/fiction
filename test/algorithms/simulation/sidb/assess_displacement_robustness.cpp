//
// Created by Jan Drewniok on 21.03.24.
//

#include <fiction/algorithms/simulation/sidb/assess_displacement_robustness.hpp>

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/types.hpp>

using namespace fiction;

using layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

TEST_CASE("assess_displacement_robustness", "[assess-displacement-robustness]")
{
    SECTION("empty layout")
    {
        layout lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({26, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({24, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, 2, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 3, 0}, sidb_technology::cell_type::NORMAL);

        // three canvas SiDBs
        lyt.assign_cell_type({12, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 7, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 11, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);

        displacement_robustness_params<tt> params{};
        params.tt = std::vector<tt>{create_or_tt()};
        params.operational_params.simulation_parameter = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        displacement_robustness_stats stats{};

        assess_displacement_robustness(lyt, params, &stats);
        const auto op = stats.num_operational_sidb_displacements;
        std::cout << op << std::endl;
    }
}
