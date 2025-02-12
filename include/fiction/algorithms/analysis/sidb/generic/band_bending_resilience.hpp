//
// Created by Jan Drewniok on 28.01.24.
//

#ifndef FICTION_BAND_BENDING_RESILIENCE_HPP
#define FICTION_BAND_BENDING_RESILIENCE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/physical_population_stability.hpp"

#include <cassert>
#include <limits>
#include <optional>
#include <vector>

namespace fiction
{
/**
 * This struct stores the parameters required to simulate the band bending resilience of an SiDB layout
 */
struct band_bending_resilience_params
{
    /**
     * Parameters for the assessing physical population stability simulation
     */
    physical_population_stability_params assess_population_stability_params{};
    /**
     * Parameters for the input BDL iterator.
     */
    bdl_input_iterator_params bdl_iterator_params{};
};
/**
 * Calculates the band bending resilience. This is the minimum electrostatic potential required to induce a charge
 * change in an SiDB layout among all possible input combinations which was proposed in \"Unifying Figures of Merit: A
 * Versatile Cost Function for Silicon Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R.
 * Wille in IEEE NANO 2024 (https://ieeexplore.ieee.org/abstract/document/10628671).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt Layout for which the band bending resilience is calculated.
 * @param spec Expected Boolean function of the layout, provided as a multi-output truth table.
 * @param params Parameters for assessing physical population stability.
 * @param transition_type The optional type of charge transition to consider. This can be used if one is only interested
 * in a specific type of charge transition.
 * @return The minimum potential (in V) required for charge change across all input combinations.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double
band_bending_resilience(const Lyt& lyt, const std::vector<TT>& spec, const band_bending_resilience_params& params = {},
                        const std::optional<transition_type> transition_type = std::nullopt) noexcept
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
        const auto pop_stability = physical_population_stability<Lyt>(lyt, params.assess_population_stability_params);
        if (!pop_stability.empty())
        {
            const auto ground_state_stability_for_given_input = pop_stability.front();

            if (transition_type.has_value())
            {
                const auto potential_required_for_considered_transition =
                    ground_state_stability_for_given_input.transition_potentials.at(transition_type.value()).second;
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

#endif  // FICTION_BAND_BENDING_RESILIENCE_HPP
