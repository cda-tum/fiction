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
 * @brief Finds the physical parameters under which a charge distribution is valid.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/domain.hpp"
#include "fiction/technology/sidb/simulation/logic/operational_domain.hpp"
#include "fiction/types.hpp"

#include <cstdint>

namespace fiction::sidb::simulation::analysis
{

/**
 * Determines the physical parameters under which a given charge distribution of a layout is physically valid:
 * every parameter point of the sweep dimensions in `params` is checked, and for each one where the charge
 * distribution is valid, its excited-state number (the number of valid configurations with lower energy under
 * those parameters) is recorded.
 *
 * @param lyt The layout.
 * @param cd The charge distribution whose validity is checked.
 * @param params Parameters; the sweep dimensions and the engine that determines the excited-state number.
 * @return The parameter points where `cd` is physically valid, each with its excited-state number; empty if `cd`
 * does not cover the layout's SiDBs in raster order.
 */
[[nodiscard]] inline domain<logic::parameter_point, uint64_t>
physically_valid_parameters(const layout& lyt, const charge_distribution& cd,
                            const logic::operational_domain_params& params = {}) noexcept
{
    if (lyt.sidbs() != cd.sites())
    {
        return {};
    }

    logic::operational_domain_stats st{};

    logic::detail::operational_domain_impl<tt, logic::operational_domain> p{lyt, params, st};

    return p.grid_search_for_physically_valid_parameters(cd);
}

}  // namespace fiction::sidb::simulation::analysis
