//
// Created by Jan Drewniok on 07.02.24.
//

#ifndef FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP
#define FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/technology/sidb_nm_distance.hpp"
#include "fiction/traits.hpp"

#include <limits>

namespace fiction
{
/**
 * Results of the defect avoidance distance calculation.
 *
 * @tparam CellType
 */
template <typename CellType>
struct defect_avoidance_distance_result
{
    /**
     * position with maximum distance to the SiDB layout at which the placement of an SiDB defect still causes the gate
     * to fail.
     */
    CellType max_distance_postion_of_non_operational_defect{};
    /**
     * Minimum distance between an SiDB of the gate and the defect at the maximum distance that causes the gate to fail.
     */
    double minimum_defect_clearance{};
};
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
[[nodiscard]] defect_avoidance_distance_result<cell<Lyt>>
defect_avoidance_distance(const Lyt& lyt, const defect_operational_domain<Lyt>& defect_opdomain) noexcept
{
    double    max_distance         = 0;
    cell<Lyt> max_distance_postion = {};

    for (const auto& val : defect_opdomain.operational_values)
    {
        if (val.second == operational_status::OPERATIONAL)
        {
            continue;
        }

        auto      min_distance          = std::numeric_limits<double>::infinity();
        cell<Lyt> min_distance_position = {};

        lyt.foreach_cell(
            [&val, &min_distance, &min_distance_position, &lyt](const auto& c)
            {
                if (sidb_nm_distance<Lyt>(lyt, c, val.first) < min_distance)
                {
                    min_distance          = sidb_nm_distance<Lyt>(lyt, c, val.first);
                    min_distance_position = val.first;
                }
            });

        if (min_distance > max_distance)
        {
            max_distance         = min_distance;
            max_distance_postion = min_distance_position;
        }
    }

    return defect_avoidance_distance_result<cell<Lyt>>{max_distance_postion, max_distance};
}

}  // namespace fiction

#endif  // FICTION_DEFECT_AVOIDANCE_DISTANCE_HPP
