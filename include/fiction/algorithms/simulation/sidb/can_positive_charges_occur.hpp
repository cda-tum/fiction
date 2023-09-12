//
// Created by Jan Drewniok on 12.08.23.
//

#ifndef FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
#define FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

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
bool can_positive_charges_occur(const Lyt& lyt, const sidb_simulation_parameters& sim_params)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    bool       result  = false;
    const auto mu_plus = sim_params.mu_plus();

    // The charge layout is initialized with negatively charged SiDBs. Therefore, the local electrostatic potentials are
    // maximal. In this extreme case, if the banding is not sufficient for any SiDB to be positively charged, it will
    // not be for any other charge distribution. Therefore, no positively charged SiDBs can occur.
    const charge_distribution_surface charge_lyt{lyt, sim_params, sidb_charge_state::NEGATIVE};
    charge_lyt.foreach_cell(
        [&result, &mu_plus, charge_lyt](const auto& c)
        {
            if (const auto local_pot = charge_lyt.get_local_potential(c); local_pot.has_value())
            {
                if ((-(*local_pot) + mu_plus) > -physical_constants::POP_STABILITY_ERR)
                {
                    result = true;
                    return false;  // break
                }
            }
            return true;
        });

    return result;
}

}  // namespace fiction

#endif  // FICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
