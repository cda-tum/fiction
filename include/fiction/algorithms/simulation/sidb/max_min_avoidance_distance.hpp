//
// Created by Jan Drewniok on 07.02.24.
//

#ifndef FICTION_MINIMUM_DISTANCE_DEFECT_HPP
#define FICTION_MINIMUM_DISTANCE_DEFECT_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence_of_sidb_gate_contour_tracing.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"

namespace fiction
{
template <typename Lyt>
[[nodiscard]] double max_min_avoidance_distance(const Lyt&                            lyt,
                                                const defect_operational_domain<Lyt>& defect_opdomain) noexcept
{
    double max_distance = 0;
    for (const auto& val : defect_opdomain.operational_values)
    {
        if (val.second == operational_status::OPERATIONAL)
        {
            continue;
        }
        auto min_distance = std::numeric_limits<double>::max();
        lyt.foreach_cell(
            [&val, &min_distance, &lyt](const auto& c)
            {
                if (sidb_nanometer_distance<Lyt>(lyt, c, val.first) < min_distance)
                {
                    min_distance = sidb_nanometer_distance<Lyt>(lyt, c, val.first);
                }
            });
        if (min_distance > max_distance)
        {
            max_distance = min_distance;
        }
    }
    return max_distance;
}
}  // namespace fiction

#endif  // FICTION_MINIMUM_DISTANCE_DEFECT_HPP
