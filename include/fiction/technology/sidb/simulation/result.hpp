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
 * @brief The unified result type every SiDB simulation algorithm returns.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/minimum_energy.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <algorithm>
#include <any>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation
{

/**
 * The unified return type of every SiDB simulation algorithm: the name of the algorithm, its runtime, the layout it
 * simulated, the physically valid charge distributions it found over that layout, the physical parameters, and
 * optional algorithm-specific named parameters.
 *
 * The layout is stored once; each charge distribution holds one charge state per SiDB in the layout's raster order
 * plus its energy.
 */
struct result
{
    /**
     * Name of the algorithm used to determine the charge distributions.
     */
    std::string algorithm_name{};
    /**
     * Total simulation runtime in seconds.
     */
    std::chrono::duration<double> simulation_runtime{};
    /**
     * The simulated layout.
     */
    layout lyt{};
    /**
     * Charge distributions determined by the algorithm.
     */
    std::vector<charge_distribution> charge_distributions{};
    /**
     * Physical parameters used in the simulation.
     */
    model::simulation_parameters sim_params{};
    /**
     * Additional named simulation parameters. This is used to store algorithm-dependent parameters that are not part of
     * the `sidb::model::simulation_parameters` struct.
     *
     * The key of the map is the name of the parameter, the element is the value of the parameter.
     */
    std::unordered_map<std::string, std::any> additional_simulation_parameters{};
    /**
     * The charge state of an SiDB in one of the charge distributions.
     *
     * @param distribution Index into `charge_distributions`.
     * @param s Site of the SiDB.
     * @return The charge state, `charge_state::NONE` if `s` holds no SiDB.
     */
    [[nodiscard]] model::charge_state charge_state(const std::size_t distribution, const lattice_site& s) const noexcept
    {
        return charge_distributions[distribution].get_charge_state(s);
    }
    /**
     * The charge distributions of minimal energy. Distributions with identical charge states count once.
     *
     * @note If degenerate states exist in the simulation result, this function returns multiple ground states that all
     * possess the same energy.
     *
     * @return The charge distributions with the minimal energy.
     */
    [[nodiscard]] std::vector<charge_distribution> groundstates() const
    {
        std::vector<charge_distribution> ground{};

        if (charge_distributions.empty())
        {
            return ground;
        }

        const auto min_energy = analysis::minimum_energy(charge_distributions.cbegin(), charge_distributions.cend());

        for (const auto& cd : charge_distributions)
        {
            if (std::abs(cd.energy() - min_energy) < utils::math::ERROR_MARGIN &&
                std::ranges::none_of(ground, [&cd](const auto& g) { return g.same_charge_states(cd); }))
            {
                ground.push_back(cd);
            }
        }

        return ground;
    }
};

}  // namespace fiction::sidb::simulation
