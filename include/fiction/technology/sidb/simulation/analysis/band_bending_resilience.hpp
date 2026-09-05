/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Minimum potential that induces a charge transition in an SiDB layout.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/traits.hpp"

#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <limits>
#include <optional>
#include <vector>

namespace fiction::sidb::simulation::analysis
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
    sidb::simulation::logic::bdl_input_iterator_params bdl_iterator_params{};
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
/**
 * Calculates the band bending resilience of an SiDB gate: the minimum potential change (unit: V) that any charge
 * transition requires in the ground state of any input pattern. A larger value means the gate tolerates more band
 * bending before its ground state changes.
 *
 * @tparam TT Truth table type.
 * @param lyt The gate layout.
 * @param spec The Boolean function(s) the gate implements; determines the number of input patterns.
 * @param params Parameters.
 * @param transition_type The transition to consider; all transitions if omitted.
 * @return The minimum potential difference over all input patterns.
 */
template <typename TT>
[[nodiscard]] double
band_bending_resilience(const layout& lyt, const std::vector<TT>& spec,
                        const band_bending_resilience_params& params          = {},
                        const std::optional<transition_type>  transition_type = std::nullopt) noexcept
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");
    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    logic::bdl_input_iterator bii{lyt, params.bdl_iterator_params};

    double minimal_pop_stability_for_all_inputs = std::numeric_limits<double>::infinity();

    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        const auto pop_stability = physical_population_stability(*bii, params.assess_population_stability_params);

        if (pop_stability.empty())
        {
            continue;
        }

        const auto& ground_state_stability = pop_stability.front();

        if (transition_type.has_value())
        {
            minimal_pop_stability_for_all_inputs =
                std::min(minimal_pop_stability_for_all_inputs,
                         ground_state_stability.transition_potentials.at(*transition_type).second);
        }
        else
        {
            for (const auto& transition : ground_state_stability.transition_potentials)
            {
                minimal_pop_stability_for_all_inputs =
                    std::min(minimal_pop_stability_for_all_inputs, transition.second.second);
            }
        }
    }

    return minimal_pop_stability_for_all_inputs;
}

/**
 * Transitional overload for SiDB cell-level layouts, converted with `to_sidb_layout`; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The gate layout.
 * @param spec The Boolean function(s) the gate implements.
 * @param params Parameters.
 * @param transition_type The transition to consider; all transitions if omitted.
 * @return The minimum potential difference over all input patterns.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt> && has_sidb_technology_v<Lyt>)
[[nodiscard]] double
band_bending_resilience(const Lyt& lyt, const std::vector<TT>& spec, const band_bending_resilience_params& params = {},
                        const std::optional<transition_type> transition_type = std::nullopt) noexcept
{
    return band_bending_resilience(to_sidb_layout(lyt), spec, params, transition_type);
}

}  // namespace fiction::sidb::simulation::analysis
