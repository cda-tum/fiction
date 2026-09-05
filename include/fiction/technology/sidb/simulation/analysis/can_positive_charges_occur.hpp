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
 * @brief Decides whether positively charged SiDBs are possible in a layout.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/traits.hpp"

#include <cstddef>

namespace fiction::sidb::simulation::analysis
{

/**
 * Checks whether positively charged SiDBs can occur in a layout under the given physical parameters, i.e., whether
 * the band bending of the fully negatively charged layout, which maximizes every local potential, pushes any SiDB
 * past its positive transition threshold. If it does not in this extreme case, it does not for any other charge
 * distribution either.
 *
 * @param land The potential landscape of the layout under the parameters to check.
 * @return `true` if at least one SiDB can be positively charged, `false` otherwise.
 */
[[nodiscard]] inline bool can_positive_charges_occur(const potential_landscape& land) noexcept
{
    const auto local_potentials =
        land.local_internal_potentials(charge_distribution{land.sites(), model::charge_state::NEGATIVE});

    for (std::size_t i = 0; i < land.num_sidbs(); ++i)
    {
        if (-local_potentials[i] >
            land.effective_charge_transition_thresholds(
                i)[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
        {
            return true;
        }
    }

    return false;
}
/**
 * Checks whether positively charged SiDBs can occur in a layout under the given physical parameters. See the
 * `potential_landscape` overload.
 *
 * @param lyt The layout to check.
 * @param sim_params The physical parameters.
 * @return `true` if at least one SiDB can be positively charged, `false` otherwise.
 */
[[nodiscard]] inline bool can_positive_charges_occur(const layout&                       lyt,
                                                     const model::simulation_parameters& sim_params) noexcept
{
    return can_positive_charges_occur(potential_landscape{lyt, sim_params});
}
}  // namespace fiction::sidb::simulation::analysis
