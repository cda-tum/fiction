//
// Created by Jan Drewniok on 02.11.23.
//

#ifndef FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
#define FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP

#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace fiction
{

/**
 * Possible types of charge transitions that can occur in an SiDB layout. These transitions represent
 * changes in the charge state of SiDBs, including transitions from neutral to negative, negative to neutral, neutral to
 * positive, and positive to neutral.
 */
enum class transition_type
{
    /**
     * SiDB is neutrally charged, but is closest to being negatively charged.
     */
    NEUTRAL_TO_NEGATIVE,
    /**
     * SiDB is negatively charged, but is closest to being neutrally charged.
     */
    NEGATIVE_TO_NEUTRAL,
    /**
     * SiDB is neutrally charged, but is closest to being positively charged.
     */
    NEUTRAL_TO_POSITIVE,
    /**
     * SiDB is positively charged, but is closest to being neutrally charged.
     */
    POSITIVE_TO_NEUTRAL,
};

template <typename Lyt>
struct population_stability_information
{
    /**
     * SiDB cell which is closest to a charge transition.
     */
    typename Lyt::cell critical_cell{};
    /**
     * Charge transition from the current charge state to the closest one.
     */
    transition_type transition_from_to{};
    /**
     * Absolute electrostatic potential required for the charge state transition.
     */
    double minimum_potential_difference_to_transition{};
};

namespace detail
{

template <typename Lyt>
class assess_physical_population_stability_impl
{
  public:
    /**
     * Constructor for assess_physical_population_stability_impl.
     *
     * @param lyt SiDB layout.
     * @param parameter The simulation parameters used for the assessment.
     */
    assess_physical_population_stability_impl(const Lyt& lyt, const sidb_simulation_parameters& parameter) :
            layout{lyt},
            params{parameter}
    {}

    /**
     * Runs the assessment of population stability for a given SiDB layout using the provided simulation parameters.
     * This function determines the minimum electrostatic potential required for charge state transitions in the layout
     * and identifies the corresponding critical SiDB and the type of charge state transition.
     *
     * @return An unordered map where each key represents the charge distribution in ascending energy order, and the
     * value is a structure containing information about the critical SiDB, the type of charge state transition, and the
     * minimum electrostatic potential required for the transition.
     */
    [[nodiscard]] std::unordered_map<std::size_t, population_stability_information<Lyt>> run() noexcept
    {
        const quickexact_params<Lyt> parameter{params};
        const auto                   simulation_results = quickexact(layout, parameter);
        auto                         transition         = transition_type::NEUTRAL_TO_NEGATIVE;
        const auto energy_and_unique_charge_index       = collect_energy_and_charge_index(simulation_results);

        std::unordered_map<std::size_t, population_stability_information<Lyt>>
            valid_states_and_detailed_popstability_information{};

        std::size_t valid_state_counter = 0;

        // Access the unique indices
        for (const auto& [energy, index] : energy_and_unique_charge_index)
        {
            for (auto const& charge_lyt : simulation_results.charge_distributions)
            {
                if (charge_lyt.get_charge_index_and_base().first == index)
                {
                    typename Lyt::cell critical_cell{};
                    double minimum_potential_difference_to_transition = std::numeric_limits<double>::infinity();

                    charge_lyt.foreach_cell(
                        [this, &charge_lyt, &minimum_potential_difference_to_transition, &critical_cell,
                         &transition](const auto& c)
                        {
                            switch (charge_lyt.get_charge_state(c))
                            {
                                case sidb_charge_state::NEGATIVE:
                                {
                                    if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus) <
                                        minimum_potential_difference_to_transition)
                                    {
                                        minimum_potential_difference_to_transition =
                                            std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus);
                                        critical_cell = c;
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
                                            minimum_potential_difference_to_transition)
                                        {
                                            minimum_potential_difference_to_transition =
                                                std::abs(-*charge_lyt.get_local_potential(c) + params.mu_minus);
                                            critical_cell = c;
                                            transition    = transition_type::NEUTRAL_TO_NEGATIVE;
                                        }
                                    }
                                    else
                                    {
                                        if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus()) <
                                            minimum_potential_difference_to_transition)
                                        {
                                            minimum_potential_difference_to_transition =
                                                std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus());
                                            critical_cell = c;
                                            transition    = transition_type::NEUTRAL_TO_POSITIVE;
                                        }
                                    }
                                    break;
                                }
                                case sidb_charge_state::POSITIVE:
                                {
                                    if (std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus()) <
                                        minimum_potential_difference_to_transition)
                                    {
                                        minimum_potential_difference_to_transition =
                                            std::abs(-*charge_lyt.get_local_potential(c) + params.mu_plus());
                                        critical_cell = c;
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
                    valid_states_and_detailed_popstability_information.emplace(
                        valid_state_counter,
                        population_stability_information<Lyt>{critical_cell, transition,
                                                              minimum_potential_difference_to_transition});
                    valid_state_counter += 1;
                    break;
                }
            }
        }

        return valid_states_and_detailed_popstability_information;
    };

    /**
     * This function collects the system energy with the corresponding charge index information of all physically valid
     * charge distributions of a given SiDB layout.
     *
     * @param sim_results The simulation results, including all physically valid charge distributions.
     * @return A vector of pairs, each containing a double value representing the system energy and a uint64_t
     * representing the unique charge index. The vector is sorted in ascending order of the energy values.
     */
    [[nodiscard]] std::vector<std::pair<double, uint64_t>>
    collect_energy_and_charge_index(const sidb_simulation_result<Lyt>& sim_results) noexcept
    {
        std::unordered_map<std::size_t, std::tuple<typename Lyt::cell, transition_type, double>> results{};

        std::vector<std::pair<double, uint64_t>> energy_and_unique_charge_index{};

        std::transform(sim_results.charge_distributions.cbegin(), sim_results.charge_distributions.cend(),
                       std::back_inserter(energy_and_unique_charge_index),
                       [](const auto& ch_lyt) {
                           return std::make_pair(ch_lyt.get_system_energy(), ch_lyt.get_charge_index_and_base().first);
                       });

        // Sort the vector in ascending order of the double value
        std::sort(energy_and_unique_charge_index.begin(), energy_and_unique_charge_index.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

        return energy_and_unique_charge_index;
    }

  private:
    /**
     * Layout to analyze.
     */
    const Lyt& layout;
    /**
     * Parameters used for the physical simulation.
     */
    const sidb_simulation_parameters& params;
};

}  // namespace detail

/**
 * This function assesses the population stability of each physically valid charge distributions of a given SiDB layout.
 * It determines the minimum absolute electrostatic potential required to induce a charge distribution transition.
 * The function also identifies the critical SiDB and the charge state transition (i.e., the change from one charge
 * state to another).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which the population stability is assessed.
 * @param params The simulation parameters used for the assessment.
 * @return An unordered map where each key represents the charge distribution in ascending energy order, and the
 * value is a structure containing information about the critical SiDB, the type of charge state transition, and the
 * minimum electrostatic potential required for the transition.
 */
template <typename Lyt>
[[nodiscard]] std::unordered_map<std::size_t, population_stability_information<Lyt>>
assess_physical_population_stability(const Lyt& lyt, const sidb_simulation_parameters& params)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    detail::assess_physical_population_stability_impl<Lyt> p{lyt, params};
    return p.run();
};

}  // namespace fiction

#endif  // FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
