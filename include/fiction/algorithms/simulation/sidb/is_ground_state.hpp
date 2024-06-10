//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_IS_GROUND_STATE_HPP
#define FICTION_IS_GROUND_STATE_HPP

#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math_utils.hpp"

#include <cmath>

namespace fiction
{

/**
 * This function checks if the ground state is found by the *QuickSim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param heuristic_results All found physically valid charge distribution surfaces obtained by a heuristic algorithm.
 * @param exhaustive_results All valid charge distribution surfaces determined by ExGS.
 * @return Returns `true` if the relative difference between the lowest energies of the two sets is less than
 * \f$0.00001\f$, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool is_ground_state(const sidb_simulation_result<Lyt>& heuristic_results,
                                   const sidb_simulation_result<Lyt>& exhaustive_results) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (exhaustive_results.charge_distributions.empty())
    {
        return false;
    }

    const auto min_energy_exact = minimum_energy(exhaustive_results.charge_distributions.cbegin(),
                                                 exhaustive_results.charge_distributions.cend());
    const auto min_energy_heuristic =
        minimum_energy(heuristic_results.charge_distributions.cbegin(), heuristic_results.charge_distributions.cend());

    return round_to_n_decimal_places(std::abs(min_energy_exact - min_energy_heuristic), 6) == 0;
}

}  // namespace fiction

#endif  // FICTION_IS_GROUND_STATE_HPP
