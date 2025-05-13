//
// Created by Jan Drewniok on 12.08.23.
//

#ifndef FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
#define FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <cstddef>

namespace fiction
{

/**
 * This algorithm determines if positively charged SiDBs can occur in a given SiDB cell-level layout due to strong
 * electrostatic interaction.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to be analyzed.
 * @param sim_params Physical parameters used to determine whether positively charged SiDBs can occur.
 */
template <typename Lyt>
[[nodiscard]] bool can_positive_charges_occur(const Lyt& lyt, const sidb_simulation_parameters& sim_params) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    // The charge layout is initialized with negatively charged SiDBs. Therefore, the local electrostatic potentials are
    // maximal. In this extreme case, if the banding is not sufficient for any SiDB to be positively charged, it will
    // not be for any other charge distribution. Therefore, no positively charged SiDBs can occur.
    charge_distribution_surface<Lyt> charge_lyt{lyt};
    charge_lyt.assign_physical_parameters(sim_params);
    charge_lyt.assign_all_charge_states(sidb_charge_state::NEGATIVE);

    for (uint64_t i = 0; i < lyt.num_cells(); ++i)
    {
        if (-*charge_lyt.get_local_internal_potential_by_index(i) >
            charge_lyt.get_effective_charge_transition_thresholds(
                i)[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
        {
            return true;
        }
    }

    return false;
}

}  // namespace fiction

#endif  // FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
