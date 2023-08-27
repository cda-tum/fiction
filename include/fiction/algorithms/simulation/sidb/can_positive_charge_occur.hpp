//
// Created by Jan Drewniok on 12.08.23.
//

#ifndef FICTION_CAN_POSITIVE_CHARGE_OCCUR_HPP
#define FICTION_CAN_POSITIVE_CHARGE_OCCUR_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace fiction
{

template <typename Lyt>
bool can_positive_charge_occur(const Lyt& lyt, const sidb_simulation_parameters& params = sidb_simulation_parameters{})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    if constexpr (has_get_charge_state_v<Lyt>)
    {}

    const charge_distribution_surface charge_lyt{lyt, params, sidb_charge_state::NEGATIVE};
    bool                              required = false;

    charge_lyt.foreach_cell(
        [&required, &params, charge_lyt](const auto& c)
        {
            if (const auto local_pot = charge_lyt.get_local_potential(c); local_pot.has_value())
            {
                if ((-(*local_pot) + params.mu_plus()) > -physical_constants::POP_STABILITY_ERR)
                {
                    required = true;
                    return false;  // pre-mature break
                }
            }
        });

    return required;
}

}  // namespace fiction

#endif  // FICTION_CAN_POSITIVE_CHARGE_OCCUR_HPP
