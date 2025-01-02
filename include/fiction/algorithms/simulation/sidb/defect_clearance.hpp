//
// Created by Jan Drewniok on 07.02.24.
//

#ifndef FICTION_DEFECT_CLEARANCE_HPP
#define FICTION_DEFECT_CLEARANCE_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence.hpp"
#include "fiction/technology/sidb_nm_distance.hpp"
#include "fiction/traits.hpp"

#include <limits>

namespace fiction
{
/**
 * Results of the defect clearance calculation.
 *
 * @tparam CellType Cell type of the layout.
 */
template <typename CellType>
struct defect_clearance_result
{
    /**
     * Position with maximum distance to the SiDB layout at which the placement of an SiDB defect still causes the gate
     * to fail.
     */
    CellType defect_position{};
    /**
     * The maximum of the minimum distances between any SiDB of the layout and the defect responsible for gate
     * failure (unit: nm).
     */
    double defect_clearance_distance{};
};
/**
 * Computes the defect clearance for a given SiDB layout based on a defect influence domain. The defect clearance is
 * the maximum distance at which a defect can influence the layout. It calculates the minimum distance from each SiDB to
 * any influential defect position.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt SiDB layout for which the defect clearance is computed.
 * @param defect_inf_domain Defect influence domain of the given SiDB layout.
 * @return Defect clearance result.
 */
template <typename Lyt>
[[nodiscard]] defect_clearance_result<cell<Lyt>>
calculate_defect_clearance(const Lyt& lyt, const defect_influence_domain<Lyt>& defect_inf_domain) noexcept
{
    double    max_distance         = 0;
    cell<Lyt> max_distance_postion = {};

    for (const auto& val : defect_inf_domain.influence_information)
    {
        if (val.second == defect_influence_status::NON_INFLUENTIAL)
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

    return defect_clearance_result<cell<Lyt>>{max_distance_postion, max_distance};
}

}  // namespace fiction

#endif  // FICTION_DEFECT_CLEARANCE_HPP
