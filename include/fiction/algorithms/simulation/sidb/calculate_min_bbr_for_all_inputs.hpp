//
// Created by Jan Drewniok on 28.01.24.
//

#ifndef FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP
#define FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"

#include <cassert>
#include <limits>
#include <optional>
#include <vector>

namespace fiction
{
/**
 * This struct stores the parameters required to assess the population stability of an SiDB gate.
 */
struct calculate_min_bbr_for_all_inputs_params
{
    /**
     * Parameters for the `assessing physical population stability` simulation
     */
    assess_physical_population_stability_params assess_population_stability_params{};
    /**
     * Parameters for the input BDL iterator.
     */
    bdl_input_iterator_params bdl_iterator_params{};
};
/**
 * Calculates the minimum electrostatic potential (a.k.a. band bending resilience) required to induce a charge change in
 * an SiDB layout among all input combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout for which the minimum band bending resilience is calculated.
 * @param spec Expected Boolean function of the layout, provided as a multi-output truth table.
 * @param params Parameters for assessing physical population stability.
 * @param transition_type The optional type of charge transition to consider. This can be used if one is only interested
 * in a specific type of charge transition.
 * @return The minimum potential required for charge change across all input combinations.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double calculate_min_bbr_for_all_inputs(
    const Lyt& lyt, const std::vector<TT>& spec, const calculate_min_bbr_for_all_inputs_params& params = {},
    const std::optional<transition_type> transition_type = transition_type::NEGATIVE_TO_NEUTRAL)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!has_offset_ucoord_v<Lyt>, "Lyt should not be based on offset coordinates");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    bdl_input_iterator<Lyt> bii{lyt, params.bdl_iterator_params};

    double minimal_pop_stability_for_all_inputs = std::numeric_limits<double>::infinity();
    // number of different input combinations
    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        const auto pop_stability =
            assess_physical_population_stability<Lyt>(lyt, params.assess_population_stability_params);
        if (!pop_stability.empty())
        {
            const auto ground_state_stability_for_given_input = pop_stability.front();

            if (transition_type.has_value())
            {
                const auto potential_required_for_considered_transition =
                    ground_state_stability_for_given_input.transition_from_to_with_cell_and_required_pot
                        .at(transition_type.value())
                        .second;
                if (potential_required_for_considered_transition < minimal_pop_stability_for_all_inputs)
                {
                    minimal_pop_stability_for_all_inputs = potential_required_for_considered_transition;
                }
            }
        }
    }
    return minimal_pop_stability_for_all_inputs;
}

}  // namespace fiction

#endif  // FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP
