//
// Created by Jan Drewniok on 28.01.24.
//

#ifndef FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP
#define FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"

#include <cassert>
#include <cstdint>
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
 * Calculates the minimum potential required to induce charge changes in an SiDB layout for all input combinations.
 *
 * @tparam Lyt Type representing the SiDB cell-level layout.
 * @tparam TT Type representing the truth table.
 * @param lyt The SiDB layout object.
 * @param spec Expected Boolean function of the layout, provided as a multi-output truth table.
 * @param params Parameters for assessing physical population stability.
 * @param charge_state_change Optional parameter indicating the direction of the considered charge state change (default
 * is 1).
 * @return The minimum potential required for charge change across all input combinations.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double calculate_min_bbr_for_all_inputs(const Lyt& lyt, const std::vector<TT>& spec,
                                                      const calculate_min_bbr_for_all_inputs_params& params = {},
                                                      const std::optional<int8_t> charge_state_change       = 1)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!has_offset_ucoord_v<Lyt>, "Lyt should not be based on offset coordinates");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    if (can_positive_charges_occur(lyt, params.assess_population_stability_params.simulation_parameters))
    {
        return 0.0;
    }

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    bdl_input_iterator<Lyt> bii{lyt, params.bdl_iterator_params};

    assert(bii.num_input_pairs() == spec.front().num_bits() / 2 &&
           "Number of truth table dimensions and input BDL pairs does not match");

    double minimal_pop_stability_for_all_inputs = std::numeric_limits<double>::infinity();
    // number of different input combinations
    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        const auto pop_stability =
            assess_physical_population_stability<Lyt>(lyt, params.assess_population_stability_params);
        if (!pop_stability.empty())
        {
            const auto ground_state_stability_for_given_input = pop_stability.front();

            if (charge_state_change.has_value())
            {
                if (charge_state_change.value() == 1)
                {
                    const auto potential_negative_to_neutral =
                        ground_state_stability_for_given_input.transition_from_to_with_cell_and_required_pot
                            .at(transition_type::NEGATIVE_TO_NEUTRAL)
                            .second;
                    if (potential_negative_to_neutral < minimal_pop_stability_for_all_inputs)
                    {
                        minimal_pop_stability_for_all_inputs = potential_negative_to_neutral;
                    }
                }
                if (charge_state_change.value() == -1)
                {
                    const auto potential_neutral_to_negative =
                        ground_state_stability_for_given_input.transition_from_to_with_cell_and_required_pot
                            .at(transition_type::NEUTRAL_TO_NEGATIVE)
                            .second;
                    if (potential_neutral_to_negative < minimal_pop_stability_for_all_inputs)
                    {
                        minimal_pop_stability_for_all_inputs = potential_neutral_to_negative;
                    }
                }
            }
        }
    }
    return minimal_pop_stability_for_all_inputs;
}

}  // namespace fiction

#endif  // FICTION_CALCULATE_MIN_BBR_FOR_ALL_INPUTS_HPP
