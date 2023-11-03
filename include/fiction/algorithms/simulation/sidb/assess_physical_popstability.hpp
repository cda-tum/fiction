//
// Created by Jan Drewniok on 02.11.23.
//

#ifndef FICTION_QUANTITY_STABILITY_OF_GROUND_STATE_HPP
#define FICTION_QUANTITY_STABILITY_OF_GROUND_STATE_HPP

#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <cmath>
#include <limits>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace fiction
{

/**
 * This defines the various types of charge transitions that can occur in a SiDB layout. These transitions represent
 * changes in the charge state of SiDBs, including transitions from neutral to negative, negative to neutral, neutral to
 * positive, and positive to neutral.
 */
enum class transition_type
{
    NEUTRAL_TO_NEGATIVE,
    NEGATIVE_TO_NEUTRAL,
    NEUTRAL_TO_POSITIVE,
    POSITIVE_TO_NEUTRAL,
};

/**
 * This function calculates the minimum electrostatic potential to the closest charge transition for a SiDB layout at
 * given physical parameters. It records information about the transition closest SiDB, the transition type, and the
 * potential difference.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which pop stability is assessed.
 * @param params The simulation parameters used for the assessment.
 * @return An unordered map where each key represents the charge distribution in ascending energy order (i.e., key = 0
 * is the ground state of the system), and the value is a tuple containing the transition closest SiDB, the closest
 * transition type (e.g., NEUTRAL_TO_NEGATIVE), and the minimum electrostatic potential.
 */
template <typename Lyt>
std::unordered_map<std::size_t, std::tuple<typename Lyt::cell, enum transition_type, double>>
assess_physical_popstability(const Lyt& lyt, const sidb_simulation_parameters& params)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    const quickexact_params<Lyt> parameter{params};
    auto                         simulation_results = quickexact(lyt, parameter);
    std::unordered_map<std::size_t, std::tuple<typename Lyt::cell, enum transition_type, double>> results {};
    auto transition = transition_type::NEUTRAL_TO_NEGATIVE;

    std::sort(simulation_results.charge_distributions.begin(), simulation_results.charge_distributions.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

    for (auto i = 0u; i < simulation_results.charge_distributions.size(); i++)
    {
        const auto         charge_lyt{simulation_results.charge_distributions[i]};
        typename Lyt::cell unstable_cell{};
        double             minimum_potential_difference_to_ext_transition = std::numeric_limits<double>::max();

        charge_lyt.foreach_cell(
            [&charge_lyt, &minimum_potential_difference_to_ext_transition, &unstable_cell, &params,
             &transition](const auto& c)
            {
                switch (charge_lyt.get_charge_state(c))
                {
                    case sidb_charge_state::NEGATIVE:
                    {
                        if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus) <
                            minimum_potential_difference_to_ext_transition)
                        {
                            minimum_potential_difference_to_ext_transition =
                                std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus);
                            unstable_cell = c;
                            transition    = transition_type::NEGATIVE_TO_NEUTRAL;
                        }
                        break;
                    }
                    case sidb_charge_state::NEUTRAL:
                    {
                        if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus) <
                            std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus()))
                        {
                            if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus) <
                                minimum_potential_difference_to_ext_transition)
                            {
                                minimum_potential_difference_to_ext_transition =
                                    std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus);
                                unstable_cell = c;
                                transition    = transition_type::NEUTRAL_TO_NEGATIVE;
                            }
                        }
                        else
                        {
                            if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus()) <
                                minimum_potential_difference_to_ext_transition)
                            {
                                minimum_potential_difference_to_ext_transition =
                                    std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus());
                                unstable_cell = c;
                                transition    = transition_type::NEUTRAL_TO_POSITIVE;
                            }
                        }
                        break;
                    }
                    case sidb_charge_state::POSITIVE:
                    {
                        if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus()) <
                            minimum_potential_difference_to_ext_transition)
                        {
                            minimum_potential_difference_to_ext_transition =
                                std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus());
                            unstable_cell = c;
                            transition    = transition_type::POSITIVE_TO_NEUTRAL;
                        }
                        break;
                    }
                    case sidb_charge_state::NONE:
                    {
                        break;
                    }
                }
            });
        results.emplace(i, std::make_tuple(unstable_cell, transition, minimum_potential_difference_to_ext_transition));
    }

    return results;
};

}  // namespace fiction

#endif  // FICTION_QUANTITY_STABILITY_OF_GROUND_STATE_HPP
