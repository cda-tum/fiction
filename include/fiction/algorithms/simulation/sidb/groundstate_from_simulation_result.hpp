//
// Created by Jan Drewniok on 12.02.24.
//

#ifndef FICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP
#define FICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/constants.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <set>
#include <vector>

namespace fiction
{

/**
 * This function calculates the ground state charge distributions from the provided simulation results.
 * The ground state charge distributions are those with energy closest to the minimum energy found in the simulation
 * results.
 *
 * @note When degenerate states exist, there are multiple ground states with the same energy.
 *
 * @tparam Lyt The layout type used in the simulation results.
 * @param simulation_results The simulation results containing charge distributions.
 * @return A vector of charge distributions with the minimal energy.
 */
template <typename Lyt>
[[nodiscard]] std::vector<charge_distribution_surface<Lyt>>
groundstate_from_simulation_result(const sidb_simulation_result<Lyt>& simulation_result) noexcept
{
    std::vector<charge_distribution_surface<Lyt>> groundstate_charge_distributions{};
    std::set<uint64_t>                            charge_indices{};

    // in case, the charge indices are not updated.
    auto charge_configurations_copy = simulation_result.charge_distributions;

    // Find all unique charge indices. This is done because simulation results can have multiple identical charge
    // distributions.
    for (auto& cds : charge_configurations_copy)
    {
        cds.charge_distribution_to_index();
        charge_indices.insert(cds.get_charge_index_and_base().first);
    }

    // Find the minimum energy
    double min_energy = std::numeric_limits<double>::infinity();
    if (!charge_configurations_copy.empty())
    {
        min_energy = std::min_element(charge_configurations_copy.begin(), charge_configurations_copy.end(),
                                      [](const auto& lhs, const auto& rhs)
                                      { return lhs.get_system_energy() < rhs.get_system_energy(); })
                         ->get_system_energy();
    }

    for (const auto charge_index : charge_indices)
    {
        const auto cds_it =
            std::find_if(charge_configurations_copy.cbegin(), charge_configurations_copy.cend(),
                         [&](const auto& cds)
                         {
                             return cds.get_charge_index_and_base().first == charge_index &&
                                    std::abs(cds.get_system_energy() - min_energy) < constants::ERROR_MARGIN;
                         });

        if (cds_it != charge_configurations_copy.cend())
        {
            groundstate_charge_distributions.push_back(*cds_it);
        }
    }

    return groundstate_charge_distributions;
}

}  // namespace fiction

#endif  // FICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP
