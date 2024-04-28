//
// Created by marcel on 05.04.23.
//

#ifndef FICTION_SIDB_SIMULATION_RESULT_HPP
#define FICTION_SIDB_SIMULATION_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <any>
#include <chrono>
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
 * @tparam Lyt Cell-level layout type.
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
     * Total simulation runtime.
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
};

}  // namespace fiction

#endif  // FICTION_SIDB_SIMULATION_RESULT_HPP
