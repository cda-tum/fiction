//
// Created by Jan Drewniok on 07.02.24.
//

#ifndef FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP
#define FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"

#include <limits>

namespace fiction
{
/**
 * Calculates the defect avoidance distance of a given gate layout by a given atomic defect. This means that a defect
 * must be further away than this distance for the SiDB gate to be operational. This function requires both the defect
 * operational domain and the layout as input.
 *
 * @tparam Lyt Type representing the SiDB cell-level layout.
 * @param lyt The cell-level layout for which the defect operational domain was computed.
 * @param defect_opdomain The defect operational domain associated with the layout.
 * @return The maximum minimum defect influence distance.
 */
template <typename Lyt>
[[nodiscard]] double defect_avoidance_distance(const Lyt&                                      lyt,
                                               const defect_influence_operational_domain<Lyt>& defect_opdomain) noexcept
{
    double max_distance = 0;
    for (const auto& val : defect_opdomain.operational_values)
    {
        if (val.second == operational_status::OPERATIONAL)
        {
            continue;
        }
        auto min_distance = std::numeric_limits<double>::infinity();
        lyt.foreach_cell(
            [&val, &min_distance, &lyt](const auto& c)
            {
                if (sidb_nm_distance<Lyt>(lyt, c, val.first) < min_distance)
                {
                    min_distance = sidb_nm_distance<Lyt>(lyt, c, val.first);
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

#endif  // FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP
