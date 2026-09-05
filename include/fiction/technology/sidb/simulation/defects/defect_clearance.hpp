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
 * @brief Minimum distance a defect must keep for a layout to stay operational.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/defects/defect_influence.hpp"

#include <limits>
#include <tuple>

namespace fiction::sidb::simulation::defects
{

/**
 * The defect clearance of a layout: the farthest influential defect position, measured by the distance to the
 * closest SiDB, and that distance.
 */
struct defect_clearance
{
    /**
     * The influential defect position farthest from the layout.
     */
    lattice_site defect_position{};
    /**
     * Its distance to the closest SiDB (unit: nm).
     */
    double defect_clearance_distance{};
};

/**
 * Computes the defect clearance from a defect influence domain: for every influential position, the distance to the
 * closest SiDB of the layout is determined, and the maximum of those distances is the clearance. Any defect farther
 * away than that does not influence the layout.
 *
 * @param lyt The layout.
 * @param defect_inf_domain The defect influence domain of `lyt`.
 * @return The defect clearance.
 */
[[nodiscard]] inline defect_clearance
calculate_defect_clearance(const layout& lyt, const defect_influence_domain& defect_inf_domain) noexcept
{
    const auto& lat = lyt.get_lattice();

    double       max_distance          = 0;
    lattice_site max_distance_position = {};

    defect_inf_domain.for_each(
        [&](const auto& defect_pos, const auto& val)
        {
            if (std::get<0>(val) == defect_influence_status::NON_INFLUENTIAL)
            {
                return;
            }

            auto min_distance = std::numeric_limits<double>::infinity();

            lyt.foreach_cell(
                [&](const auto& c)
                {
                    if (const auto d = lat.nm_distance(c, defect_pos); d < min_distance)
                    {
                        min_distance = d;
                    }
                });

            if (min_distance > max_distance)
            {
                max_distance          = min_distance;
                max_distance_position = defect_pos;
            }
        });

    return defect_clearance{max_distance_position, max_distance};
}

}  // namespace fiction::sidb::simulation::defects
