//
// Created by marcel on 05.04.23.
//

#ifndef FICTION_SIDB_SIMULATION_RESULT_HPP
#define FICTION_SIDB_SIMULATION_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/constants.hpp"

#include <any>
#include <chrono>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * This struct defines a unified return type for all SiDB simulation algorithms. It contains the name of the algorithm,
 * the total simulation runtime, the charge distributions determined by the algorithm, the physical parameters used in
 * the simulation, and (optional) algorithm-specific named simulation parameters.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct sidb_simulation_result
{
    /**
     * Default constructor. It only exists to allow for the use of `static_assert` statements that restrict the type of
     * `Lyt`.
     */
    sidb_simulation_result() noexcept
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Name of the algorithm used to determine the charge distributions.
     */
    std::string algorithm_name{};
    /**
     * Total simulation runtime in seconds.
     */
    std::chrono::duration<double> simulation_runtime{};
    /**
     * Charge distributions determined by the algorithm.
     */
    std::vector<charge_distribution_surface<Lyt>> charge_distributions{};
    /**
     * Physical parameters used in the simulation.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Additional named simulation parameters. This is used to store algorithm-dependent parameters that are not part of
     * the `sidb_simulation_parameters` struct.
     *
     * The key of the map is the name of the parameter, the element is the value of the parameter.
     */
    std::unordered_map<std::string, std::any> additional_simulation_parameters{};
    /**
     * This function computes the ground state of the charge distributions.
     *
     * @note If degenerate states exist in the simulation result, this function will return multiple ground states that
     * all possess the same system energy.
     *
     * @return A vector of charge distributions with the minimal energy.
     */
    [[nodiscard]] std::vector<charge_distribution_surface<Lyt>> groundstates() const noexcept
    {
        std::vector<charge_distribution_surface<Lyt>> groundstate_charge_distributions{};
        std::set<uint64_t>                            charge_indices{};

        // Find all unique charge indices. This is done because simulation results can have multiple identical charge
        // distributions.
        for (auto& cds : charge_distributions)
        {
            cds.charge_distribution_to_index();
            charge_indices.insert(cds.get_charge_index_and_base().first);
        }

        // Find the minimum energy
        double min_energy = std::numeric_limits<double>::infinity();
        if (!charge_distributions.empty())
        {
            min_energy = minimum_energy(charge_distributions.cbegin(), charge_distributions.cend());
        }

        for (const auto charge_index : charge_indices)
        {
            const auto cds_it =
                std::find_if(charge_distributions.cbegin(), charge_distributions.cend(),
                             [&](const auto& cds)
                             {
                                 return cds.get_charge_index_and_base().first == charge_index &&
                                        std::abs(cds.get_system_energy() - min_energy) < constants::ERROR_MARGIN;
                             });

            if (cds_it != charge_distributions.cend())
            {
                groundstate_charge_distributions.push_back(*cds_it);
            }
        }

        return groundstate_charge_distributions;
    }
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_RESULT_HPP
