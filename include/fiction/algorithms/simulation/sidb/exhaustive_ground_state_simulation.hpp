//
// Created by Jan Drewniok on 18.12.22.
//

#ifndef FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
#define FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <iostream>
#include <vector>

namespace fiction
{

/**
 *  All metastable and physically valid charge distribution layouts are computed, stored in a vector and returned.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The layout to simulate.
 * @param params Simulation parameters.
 * @param ps Simulation statistics.
 * @return sidb_simulation_result is returned with all results.
 */
template <typename Lyt>
sidb_simulation_result<Lyt>
exhaustive_ground_state_simulation(const Lyt&                        lyt,
                                   const sidb_simulation_parameters& params = sidb_simulation_parameters{}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    sidb_simulation_result<Lyt> simulation_result{};
    simulation_result.algorithm_name      = "exgs";
    simulation_result.physical_parameters = params;
    mockturtle::stopwatch<>::duration time_counter{};
    {

        const mockturtle::stopwatch stop{time_counter};

        charge_distribution_surface charge_lyt{lyt};

        charge_lyt.set_physical_parameters(params);
        charge_lyt.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.update_after_charge_change();

        while (charge_lyt.get_charge_index().first < charge_lyt.get_max_charge_index())
        {

            if (charge_lyt.is_physically_valid())
            {
                simulation_result.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt});
            }

            charge_lyt.increase_charge_index_by_one();
        }

        if (charge_lyt.is_physically_valid())
        {
            simulation_result.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt});
        }
    }
    simulation_result.simulation_runtime = time_counter;

    return simulation_result;
}

}  // namespace fiction

#endif  // FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
