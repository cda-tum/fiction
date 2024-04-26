//
// Created by Jan Drewniok on 28.01.24.
//

#ifndef FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_SIDB_GATE_HPP
#define FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_SIDB_GATE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/utils/layout_utils.hpp"

namespace fiction
{
/**
 * This struct stores the parameters required to assess the population stability of an SiDB gate.
 */
struct assess_physical_population_stability_params_sidb_gate
{
    /**
     * Parameter to as
     */
    assess_physical_population_stability_params assess_population_stability_params{};
    detect_bdl_pairs_params                     detect_pair_params{};
};

template <typename Lyt, typename TT>
[[nodiscard]] double
assess_physical_population_stability_sidb_gate(const Lyt& lyt, const std::vector<TT>& spec,
                                               const assess_physical_population_stability_params_sidb_gate& params = {},
                                               const int8_t charge_state_change = 1) noexcept
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

    bdl_input_iterator<Lyt> bii{lyt, params.detect_pair_params};
    double                  maximal_pop_stability_for_all_inputs = std::numeric_limits<double>::infinity();
    // number of different input combinations
    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        const auto pop_stability =
            assess_physical_population_stability<Lyt>(lyt, params.assess_population_stability_params);
        if (!pop_stability.empty())
        {
            const auto stability_for_given_input = pop_stability.front().minimum_potential_difference_to_transition;
            double     potential                 = 0.0;

            if (charge_state_change == 1)
            {
                potential = std::abs(stability_for_given_input.at(transition_type::NEGATIVE_TO_NEUTRAL));
            }

            if (charge_state_change == -1)
            {
                potential = std::abs(stability_for_given_input.at(transition_type::NEUTRAL_TO_NEGATIVE));
            }

            if (potential < maximal_pop_stability_for_all_inputs)
            {
                maximal_pop_stability_for_all_inputs = potential;
            }
        }
    }
    return maximal_pop_stability_for_all_inputs;
}

}  // namespace fiction

#endif  // FICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_SIDB_GATE_HPP
