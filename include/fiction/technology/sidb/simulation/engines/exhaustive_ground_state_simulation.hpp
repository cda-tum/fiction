/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief *ExGS*, the exhaustive ground-state simulation of SiDB layouts.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/detail/simulation_state.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>

namespace fiction::sidb::simulation::engines
{

/**
 * *Exhaustive Ground State Simulation* (*ExGS*) which was proposed in \"Computer-Aided Design of Atomic Silicon Quantum
 * Dots and Computational Applications\" by S. S. H. Ng (https://dx.doi.org/10.14288/1.0392909) enumerates every charge
 * distribution of the layout in base 2 or 3 and keeps the physically valid ones. Charged surface defects of the layout
 * enter the potential landscape.
 *
 * @param lyt Layout to simulate.
 * @param params Physical parameters.
 * @return The physically valid charge distributions.
 */
[[nodiscard]] inline result
exhaustive_ground_state_simulation(const layout&                       lyt,
                                   const model::simulation_parameters& params = model::simulation_parameters{}) noexcept
{
    result simulation_result{};
    simulation_result.algorithm_name = "ExGS";
    simulation_result.sim_params     = params;
    simulation_result.lyt            = lyt;

    if (lyt.num_cells() == 0)
    {
        return simulation_result;
    }

    mockturtle::stopwatch<>::duration time_counter{};
    {
        const mockturtle::stopwatch stop{time_counter};

        const potential_landscape            land{lyt, params};
        simulation::detail::simulation_state state{land, model::charge_state::NEGATIVE};

        while (state.charge_index() < state.max_charge_index())
        {
            if (state.is_physically_valid())
            {
                simulation_result.charge_distributions.push_back(state.snapshot());
            }

            state.increase_charge_index_by_one();
        }

        if (state.is_physically_valid())
        {
            simulation_result.charge_distributions.push_back(state.snapshot());
        }
    }
    simulation_result.simulation_runtime = time_counter;

    return simulation_result;
}

/**
 * *ExGS* on a Cartesian SiDB cell-level layout: the layout is converted with `to_sidb_layout`, simulated, and the
 * result converted back with `to_legacy_result`. This overload serves the algorithms that still consume
 * `legacy_result`.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to simulate.
 * @param params Physical parameters.
 * @return Simulation result over surfaces of `lyt`.
 */
template <typename Lyt>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] legacy_result<Lyt>
exhaustive_ground_state_simulation(const Lyt&                          lyt,
                                   const model::simulation_parameters& params = model::simulation_parameters{}) noexcept
{
    return to_legacy_result(exhaustive_ground_state_simulation(to_sidb_layout(lyt), params), lyt);
}

}  // namespace fiction::sidb::simulation::engines
