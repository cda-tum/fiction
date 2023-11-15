//
// Created by Jan Drewniok on 10.11.23.
//

#ifndef FICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP
#define FICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/physical_constants.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace fiction
{

/**
 * The electrostatic potential on hydrogen-passivated silicon is typically modeled using a screened Coulomb potential.
 * This electrostatic potential is commonly employed to determine the electrostatic potential for a given distance
 * (between SiDB and point under consideration) and given physical parameters. However, the function provided here
 * serves the inverse purpose by calculating the distance for a given potential and given physical parameters.
 *
 * @note Runtime depends exponentially on the provided precision.
 *
 * @param params The physical parameters for a given hydrogen-passivated silicon surface.
 * @param potential The electrostatic potential (unit: V) to be converted to a distance.
 * @param precision The precision level for the conversion, specifying the number of decimal places.
 * @return The distance (unit: nm) corresponding to the given electrostatic potential.
 */
[[nodiscard]] inline double
convert_potential_to_distance(const double                      potential,
                              const sidb_simulation_parameters& params    = sidb_simulation_parameters{},
                              const uint64_t                    precision = 2) noexcept
{
    // function to calculate the electrostatic potential for a given distance and given physical parameters on the H-Si
    // surface
    const auto calculate_potential_for_given_distance = [&params](const double distance) noexcept
    {
        return params.k() * params.epsilon_r / params.epsilon_r / (distance * 1e-9) *
               std::exp(-distance / params.lambda_tf) * physical_constants::ELEMENTARY_CHARGE;
    };

    // calculate the step size based on the precision
    const double step_size = std::pow(10, -static_cast<double>(precision));

    // initialize distance and potential for the initial step
    double distance                     = step_size;
    double potential_for_given_distance = calculate_potential_for_given_distance(distance);

    // as long as the electrostatic potential is still larger than the given potential, the distance is increased
    while (potential_for_given_distance > potential)
    {
        distance += step_size;
        potential_for_given_distance = calculate_potential_for_given_distance(distance);
    }

    return distance;
}

}  // namespace fiction

#endif  // FICTION_CONVERT_POTENTIAL_TO_DISTANCE_HPP
