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
 * @brief Decides up front whether an SiDB gate design task admits no solution.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"

#include <kitty/traits.hpp>

#include <cassert>
#include <cstddef>
#include <vector>

namespace fiction::sidb::generators
{

/**
 * This struct contains parameters to determine if SiDB gate design is impossible.
 */
struct is_gate_design_impossible_params
{
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * Parameters used for the BDL input iterator.
     */
    sidb::simulation::logic::bdl_input_iterator_params bdl_iterator_params{};
};
/**
 * Checks whether a gate can be designed on a skeleton with defects at all: if the charged defects push one SiDB
 * of an output BDL pair past its neutral transition threshold for any input pattern, that SiDB can never be
 * negatively charged, the pair loses its BDL property, and no canvas can fix that.
 *
 * @tparam TT Truth table type.
 * @param skeleton_with_defects The skeleton, including the defects of the surface it sits on.
 * @param spec The Boolean function(s) to implement.
 * @param params Parameters.
 * @return `true` if no gate can be designed on the skeleton.
 */
template <typename TT>
[[nodiscard]] bool is_gate_design_impossible(const layout& skeleton_with_defects, const std::vector<TT>& spec,
                                             const is_gate_design_impossible_params& params = {})
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(skeleton_with_defects.num_pis() > 0 && "lyt needs input cells");
    assert(skeleton_with_defects.num_pos() > 0 && "lyt needs output cells");

    const auto output_pairs =
        simulation::logic::detect_bdl_pairs(skeleton_with_defects, sidb_technology::cell_type::OUTPUT,
                                            params.bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

    assert(!output_pairs.empty() && "lyt needs output BDL pairs");

    simulation::logic::bdl_input_iterator bii{skeleton_with_defects, params.bdl_iterator_params};

    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        const simulation::potential_landscape land{*bii, params.sim_params};

        // with every SiDB neutral, the local potentials are those the defects cause
        const auto potentials =
            land.local_internal_potentials(charge_distribution{land.sites(), model::charge_state::NEUTRAL});

        const auto can_never_be_negative = [&land, &potentials](const lattice_site& s)
        {
            const auto index = land.get_layout().index_of(s);
            assert(index.has_value() && "BDL SiDB is not part of the layout");

            return -potentials[*index] > land.effective_charge_transition_thresholds(*index)[static_cast<std::size_t>(
                                             simulation::charge_transition_threshold_bounds::NEUTRAL_LOWER_BOUND)];
        };

        for (const auto& bdl : output_pairs)
        {
            if (can_never_be_negative(bdl.lower) || can_never_be_negative(bdl.upper))
            {
                return true;
            }
        }
    }

    return false;
}
/**
 * Transitional overload for SiDB cell-level layouts, converted with `to_sidb_layout`; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param skeleton_with_defects The skeleton with defects.
 * @param spec The Boolean function(s) to implement.
 * @param params Parameters.
 * @return `true` if no gate can be designed on the skeleton.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] bool is_gate_design_impossible(const Lyt& skeleton_with_defects, const std::vector<TT>& spec,
                                             const is_gate_design_impossible_params& params = {})
{
    return is_gate_design_impossible(to_sidb_layout(skeleton_with_defects), spec, params);
}

}  // namespace fiction::sidb::generators
