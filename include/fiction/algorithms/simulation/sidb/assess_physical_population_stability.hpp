//
// Created by Jan Drewniok on 02.11.23.
//

#ifndef FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
#define FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP

#include "fiction/algorithms/simulation/sidb/convert_potential_to_distance.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

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
/**
 * This struct encapsulates information related to the population stability of a charge distribution.
 * It includes details about the SiDB closest to a charge transition (critical cell), the specific
 * charge state transition, the electrostatic potential difference required for the
 * transition, the corresponding distance, and the total electrostatic energy of the
 * given charge distribution.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct population_stability_information
{
    /**
     * SiDB cell which is closest to a charge transition.
     */
    typename Lyt::cell critical_cell{};
    /**
     * This map collects all charge transition types, the corresponding critical cells and the required
     * electrostatic potential (unit: V) required to conduct the transition.
     */
    std::unordered_map<transition_type, std::pair<typename Lyt::cell, double>>
        transition_from_to_with_cell_and_required_pot{};
    /**
     * This map collects for all charge transition types, the electrostatic potential difference which is
     * required to conduct a charge change as a distance in nanometer. This is possible since the electrostatic
     * potential is connected to the distance.
     */
    std::unordered_map<transition_type, double> distance_corresponding_to_potential{};
    /**
     * Total electrostatic energy (unit: eV) of given charge distribution.
     */
    double system_energy{};
};

/**
 * This struct stores the parameters required to assess the population stability.
 */
struct assess_physical_population_stability_params
{
    /**
     * Parameters for the electrostatic potential.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * The precision level for the conversion from the minimum potential difference to the corresponding
     * distance.
     */
    uint64_t precision_for_distance_corresponding_to_potential = 2;
};

namespace detail
{
/**
 * This class implements the population stability assessment for a given SiDB layout.
 * It determines the minimum electrostatic potential required for charge state transitions within the layout and
 * identifies the corresponding critical SiDB along with the type of charge state transition.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
class assess_physical_population_stability_impl
{
  public:
    /**
     * Constructor for assess_physical_population_stability_impl.
     *
     * @param lyt SiDB layout.
     * @param parameters The simulation parameters used for the assessment.
     */
    assess_physical_population_stability_impl(const Lyt&                                         lyt,
                                              const assess_physical_population_stability_params& parameters) :
            layout{lyt},
            params{parameters}
    {}

    /**
     * Runs a population stability assessment for a given SiDB layout using the provided simulation parameters.
     * This function determines the minimum electrostatic potential required for charge state transitions within the
     * layout and identifies the corresponding critical SiDB along with the type of charge state transition.
     *
     * @return A vector of population stability information structures, where each element represents a charge
     * distribution in ascending energy order. Each structure contains details about the critical SiDB, the type of
     * charge state transition, and the minimum electrostatic potential required for the charge transition.
     */
    [[nodiscard]] std::vector<population_stability_information<Lyt>> run() noexcept
    {
        const quickexact_params<cell<Lyt>> quickexact_parameters{params.simulation_parameters};
        const auto                         simulation_results = quickexact(layout, quickexact_parameters);
        const auto energy_and_unique_charge_index             = collect_energy_and_charge_index(simulation_results);

        std::vector<population_stability_information<Lyt>> popstability_information{};
        popstability_information.reserve(simulation_results.charge_distributions.size());

        // Access the unique indices
        for (const auto& energy_and_index : energy_and_unique_charge_index)
        {
            const auto it = std::find_if(
                simulation_results.charge_distributions.begin(), simulation_results.charge_distributions.end(),
                [&](const charge_distribution_surface<Lyt>& charge_lyt)
                {
                    // Compare with the first element of the pair returned by get_charge_index_and_base()
                    return charge_lyt.get_charge_index_and_base().first == energy_and_index.charge_index;
                });

            if (it == simulation_results.charge_distributions.end())
            {
                continue;
            }

            const auto& charge_lyt = *it;

            population_stability_information<Lyt> population_stability_info{};

            population_stability_info.transition_from_to_with_cell_and_required_pot.insert(
                {transition_type::NEUTRAL_TO_NEGATIVE, {cell<Lyt>{}, std::numeric_limits<double>::infinity()}});
            population_stability_info.transition_from_to_with_cell_and_required_pot.insert(
                {transition_type::NEGATIVE_TO_NEUTRAL, {cell<Lyt>{}, std::numeric_limits<double>::infinity()}});
            population_stability_info.transition_from_to_with_cell_and_required_pot.insert(
                {transition_type::NEUTRAL_TO_POSITIVE, {cell<Lyt>{}, std::numeric_limits<double>::infinity()}});
            population_stability_info.transition_from_to_with_cell_and_required_pot.insert(
                {transition_type::POSITIVE_TO_NEUTRAL, {cell<Lyt>{}, std::numeric_limits<double>::infinity()}});

            charge_lyt.foreach_cell(
                [this, &charge_lyt, &population_stability_info](const auto& c)
                {
                    switch (charge_lyt.get_charge_state(c))
                    {
                        case sidb_charge_state::NEGATIVE:
                        {
                            population_stability_info = handle_negative_charges(*charge_lyt.get_local_potential(c), c,
                                                                                population_stability_info);
                            break;
                        }
                        case sidb_charge_state::NEUTRAL:
                        {
                            population_stability_info = handle_neutral_charges(*charge_lyt.get_local_potential(c), c,
                                                                               population_stability_info);
                            break;
                        }
                        case sidb_charge_state::POSITIVE:
                        {
                            population_stability_info = handle_positive_charges(*charge_lyt.get_local_potential(c), c,
                                                                                population_stability_info);
                            break;
                        }
                        case sidb_charge_state::NONE:
                        {
                            break;
                        }
                    }
                });
            population_stability_info.system_energy = charge_lyt.get_system_energy();

            auto minimum_potential_difference = std::numeric_limits<double>::infinity();

            for (const auto& transition : population_stability_info.transition_from_to_with_cell_and_required_pot)
            {
                population_stability_info.distance_corresponding_to_potential[transition.first] =
                    convert_potential_to_distance(transition.second.second, params.simulation_parameters,
                                                  params.precision_for_distance_corresponding_to_potential);
                if (transition.second.second < minimum_potential_difference)
                {
                    population_stability_info.critical_cell = transition.second.first;
                    minimum_potential_difference            = transition.second.second;
                }
            }
            popstability_information.push_back(population_stability_info);
        }

        return popstability_information;
    };

  private:
    /**
     * This struct represents the electrostatic energy and charge index of a charge distribution.
     */
    struct energy_and_charge_index
    {
        /**
         * Electrostatic energy of the charge distribution (unit: eV).
         */
        double energy;
        /**
         * Charge index of the charge distribution.
         */
        uint64_t charge_index;
    };
    /**
     * Layout to analyze.
     */
    const Lyt& layout;
    /**
     * Parameters required to assess the population stability.
     */
    const assess_physical_population_stability_params& params;

    /**
     * This function checks if the absolute difference between the given local potential and
     * µ- is smaller than the current minimum potential difference to transition for a negatively charged SiDB.
     * If `true`, it updates the population stability information with the new minimum difference and critical cell.
     *
     * @param local_potential The local potential associated with the cell.
     * @param c The cell for which the charge state is being considered (SiDB is negatively charged).
     * @param pop_stability_information The current population stability information.
     *
     * @return An updated population stability information with potential transition details.
     */
    [[nodiscard]] population_stability_information<Lyt>
    handle_negative_charges(const double local_potential, const typename Lyt::cell& c,
                            const population_stability_information<Lyt>& pop_stability_information) noexcept
    {
        auto updated_pop_stability_information = pop_stability_information;

        const auto required_potential_to_conduct_transition_negative_to_neutral =
            std::abs(-local_potential + params.simulation_parameters.mu_minus);

        if (required_potential_to_conduct_transition_negative_to_neutral <
            updated_pop_stability_information.transition_from_to_with_cell_and_required_pot
                .at(transition_type::NEGATIVE_TO_NEUTRAL)
                .second)
        {
            updated_pop_stability_information
                .transition_from_to_with_cell_and_required_pot[transition_type::NEGATIVE_TO_NEUTRAL] = {
                c, required_potential_to_conduct_transition_negative_to_neutral};
        }

        return updated_pop_stability_information;
    }

    /**
     * This function checks if the absolute difference between the given local potential and
     * µ- or µ+ is smaller than the current minimum potential difference.
     * If `true`, it updates the population stability information with the new minimum difference and critical cell.
     *
     * @param local_potential The local potential associated with the cell.
     * @param c The cell for which the charge state is being considered (SiDB is neutrally charged).
     * @param pop_stability_information The current population stability information.
     *
     * @return An updated population stability information with potential transition details.
     */
    [[nodiscard]] population_stability_information<Lyt>
    handle_neutral_charges(const double local_potential, const typename Lyt::cell& c,
                           const population_stability_information<Lyt>& pop_stability_information) noexcept
    {
        auto updated_pop_stability_information = pop_stability_information;

        const auto required_potential_to_conduct_transition_neutral_to_negative =
            std::abs(-local_potential + params.simulation_parameters.mu_minus);
        if (required_potential_to_conduct_transition_neutral_to_negative <
            std::abs(-local_potential + params.simulation_parameters.mu_plus()))
        {
            if (std::abs(-local_potential + params.simulation_parameters.mu_minus) <
                updated_pop_stability_information.transition_from_to_with_cell_and_required_pot
                    .at(transition_type::NEUTRAL_TO_NEGATIVE)
                    .second)
            {
                updated_pop_stability_information
                    .transition_from_to_with_cell_and_required_pot[transition_type::NEUTRAL_TO_NEGATIVE] = {
                    c, required_potential_to_conduct_transition_neutral_to_negative};
            }
        }
        else
        {
            const auto required_potential_to_conduct_transition_neutral_to_positive =
                std::abs(-local_potential + params.simulation_parameters.mu_plus());
            if (required_potential_to_conduct_transition_neutral_to_positive <
                updated_pop_stability_information.transition_from_to_with_cell_and_required_pot
                    .at(transition_type::NEUTRAL_TO_POSITIVE)
                    .second)
            {
                updated_pop_stability_information
                    .transition_from_to_with_cell_and_required_pot[transition_type::NEUTRAL_TO_POSITIVE] = {
                    c, required_potential_to_conduct_transition_neutral_to_positive};
            }
        }

        return updated_pop_stability_information;
    }

    /**
     * This function checks if the absolute difference between the given local potential and µ+ is smaller than the
     * current minimum potential difference. If true`, it updates the
     * population stability information with the new minimum difference and critical cell.
     *
     * @param local_potential The local potential associated with the cell.
     * @param c The cell for which the charge state is being considered (SiDB is positively charged).
     * @param pop_stability_information The current population stability information.
     *
     * @return An updated population stability information with potential transition details.
     */
    [[nodiscard]] population_stability_information<Lyt>
    handle_positive_charges(const double local_potential, const typename Lyt::cell& c,
                            const population_stability_information<Lyt>& pop_stability_information) noexcept
    {
        auto updated_pop_stability_information = pop_stability_information;

        const auto required_potential_to_conduct_transition_from_positive_to_neutral =
            std::abs(-local_potential + params.simulation_parameters.mu_plus());

        if (required_potential_to_conduct_transition_from_positive_to_neutral <
            updated_pop_stability_information.transition_from_to_with_cell_and_required_pot
                .at(transition_type::POSITIVE_TO_NEUTRAL)
                .second)
        {
            updated_pop_stability_information
                .transition_from_to_with_cell_and_required_pot[transition_type::POSITIVE_TO_NEUTRAL] = {
                c, required_potential_to_conduct_transition_from_positive_to_neutral};
        }

        return updated_pop_stability_information;
    }

    /**
     * Collects the system energy with the corresponding charge index information of all physically valid
     * charge distributions of a given SiDB layout.
     *
     * @param sim_results The simulation results, including all physically valid charge distributions.
     * @return A vector of energy_and_charge_index pairs, where each pair consists of a double value representing
     * the system energy and a uint64_t representing the unique charge index. The vector is sorted in ascending order
     * of the energy values.
     */
    [[nodiscard]] std::vector<energy_and_charge_index>
    collect_energy_and_charge_index(const sidb_simulation_result<Lyt>& sim_results) const noexcept
    {
        std::vector<energy_and_charge_index> energy_charge_index{};
        energy_charge_index.reserve(sim_results.charge_distributions.size());

        std::transform(
            sim_results.charge_distributions.cbegin(), sim_results.charge_distributions.cend(),
            std::back_inserter(energy_charge_index), [](const auto& ch_lyt)
            { return energy_and_charge_index{ch_lyt.get_system_energy(), ch_lyt.get_charge_index_and_base().first}; });

        // Sort the vector in ascending order of the energy value
        std::sort(energy_charge_index.begin(), energy_charge_index.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.energy < rhs.energy; });

        return energy_charge_index;
    }
};

}  // namespace detail

/**
 * This function assesses the population stability of each physically valid charge distributions of a given SiDB layout.
 * It determines the minimum absolute electrostatic potential required to induce a charge distribution transition.
 * The function also identifies the SiDB for which this is the case (critical SiDB) and the corresponding charge state
 * transition (i.e., the change from one charge state to another).
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which the population stability is assessed.
 * @param params Parameters used to assess the population stability.
 * @return A vector of population stability information for all physically valid charge distributions of the given SiDB
 * layout.
 */
template <typename Lyt>
[[nodiscard]] std::vector<population_stability_information<Lyt>>
assess_physical_population_stability(const Lyt& lyt, const assess_physical_population_stability_params& params) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    detail::assess_physical_population_stability_impl<Lyt> p{lyt, params};
    return p.run();
}

}  // namespace fiction

#endif  // FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
