//
// Created by Jan Drewniok on 18.01.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <limits>
#include <vector>

using namespace fiction;

using lattice = sidb_100_cell_clk_lyt_siqad;

TEST_CASE("Test minimum energy function", "[minimum-energy]")
{
    lattice lyt{};

    SECTION("layout with no SiDB placed")
    {
        const charge_distribution_surface                 charge_layout{lyt};
        std::vector<charge_distribution_surface<lattice>> all_lyts{};

        CHECK_THAT(minimum_energy(all_lyts.begin(), all_lyts.end()),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.00001));

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts.begin(), all_lyts.end()) - 0) < 0.00000001);
    }

    SECTION("layout with one SiDB placed")
    {
        lyt.assign_cell_type({0, 0}, lattice::cell_type::NORMAL);

        const charge_distribution_surface                 charge_layout{lyt};
        std::vector<charge_distribution_surface<lattice>> all_lyts{};

        CHECK_THAT(minimum_energy(all_lyts.cbegin(), all_lyts.cend()),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.00001));

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts.cbegin(), all_lyts.cend()) - 0) < 0.00000001);
    }

    SECTION("layout with three SiDBs placed")
    {
        lyt.assign_cell_type({0, 0}, lattice::cell_type::NORMAL);
        lyt.assign_cell_type({10, 10}, lattice::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9}, lattice::cell_type::NORMAL);

        charge_distribution_surface                       charge_layout_first{lyt};
        std::vector<charge_distribution_surface<lattice>> all_lyts{};

        CHECK_THAT(minimum_energy(all_lyts.cbegin(), all_lyts.cend()),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.00001));

        charge_layout_first.assign_charge_state({0, 0}, sidb_charge_state::NEUTRAL);

        charge_layout_first.update_local_potential();
        charge_layout_first.recompute_system_energy();
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};

        charge_layout_second.assign_charge_state({10, 10}, sidb_charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({9, 9}, sidb_charge_state::NEUTRAL);

        charge_layout_second.update_local_potential();
        charge_layout_second.recompute_system_energy();
        all_lyts.push_back(charge_layout_second);

        CHECK_THAT(minimum_energy(all_lyts.cbegin(), all_lyts.cend()), Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
}
