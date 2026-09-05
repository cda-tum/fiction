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
 * @brief Labels charge distributions as ground or excited states with their energies.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/simulation/analysis/energy_distribution.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/simulation/logic/verify_logic_match.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::analysis
{

/**
 * Label to categorize ground and excited states of an SiDB layout.
 */
enum class state_type : uint8_t
{
    /**
     * A state is accepted if the charge distribution encodes the desired logic.
     */
    ACCEPTED,
    /**
     * A state is rejected if the charge distributiion does not encode the desired logic. Moreover, if kinks are
     * rejected, a charge distribution that encodes the logic, but does show kinks, is rejected.
     */
    REJECTED
};
/**
 *  Data type to collect electrostatic potential energies (in eV) of charge distributions with corresponding state types
 * (i.e., `true` = transparent, `false` = erroneous).
 */
using energy_and_state_type = std::vector<std::pair<double, state_type>>;

/**
 * This function takes in an SiDB energy distribution. For each charge distribution, the state type is determined (i.e.
 * erroneous, transparent) while kinks are accepted, meaning a state with kinks is considered transparent.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param energy_dist Energy distribution.
 * @param valid_charge_distributions Physically valid charge distributions.
 * @param output_bdl_pairs Output BDL pairs.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_index The index of the current input configuration.
 * @return Electrostatic potential energy of all charge distributions with state type.
 */
/**
 * Labels every energy level of an energy distribution by whether the physically valid charge distributions at that
 * level encode the expected output for the given input pattern (`ACCEPTED`) or not (`REJECTED`). Kinks in the
 * wires are tolerated: only the output BDL pairs are inspected.
 *
 * @tparam TT Truth table type.
 * @param energy_dist The energy distribution of the charge distributions.
 * @param valid_charge_distributions The physically valid charge distributions.
 * @param output_bdl_pairs The output BDL pairs of the layout.
 * @param spec The Boolean function(s) to implement.
 * @param input_index The input pattern the charge distributions were simulated for.
 * @return The energies with their state types, ascending by energy.
 */
template <typename TT>
[[nodiscard]] energy_and_state_type
calculate_energy_and_state_type_with_kinks_accepted(const energy_distribution&              energy_dist,
                                                    const std::vector<charge_distribution>& valid_charge_distributions,
                                                    const std::vector<logic::bdl_pair<lattice_site>>& output_bdl_pairs,
                                                    const std::vector<TT>& spec, const uint64_t input_index) noexcept
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!output_bdl_pairs.empty() && "No output cell provided.");
    assert((spec.size() == output_bdl_pairs.size()) && "Number of truth tables and output BDL pairs does not match");

    energy_and_state_type est{};

    energy_dist.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& cd : valid_charge_distributions)
            {
                if (std::abs(cd.energy() - energy) >= utils::math::ERROR_MARGIN)
                {
                    continue;
                }

                auto type_of_considered_state = state_type::ACCEPTED;

                for (std::size_t i = 0; i < output_bdl_pairs.size(); ++i)
                {
                    if (static_cast<bool>(-model::charge_state_to_sign(
                            cd.get_charge_state(output_bdl_pairs[i].lower))) != kitty::get_bit(spec[i], input_index))
                    {
                        type_of_considered_state = state_type::REJECTED;
                        break;
                    }
                }

                est.emplace_back(energy, type_of_considered_state);
            }
        });

    std::ranges::sort(est, [](const auto& a, const auto& b) { return a.first < b.first; });

    return est;
}
/**
 * Like `calculate_energy_and_state_type_with_kinks_accepted`, but a charge distribution with kinks in its wires is
 * `REJECTED` as well: every energy level is `ACCEPTED`, and additionally `REJECTED` if any of its charge
 * distributions fails the logic match with kinks rejected.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout the charge distributions belong to.
 * @param energy_dist The energy distribution of the charge distributions.
 * @param valid_charge_distributions The physically valid charge distributions.
 * @param spec The Boolean function(s) to implement.
 * @param input_index The input pattern the charge distributions were simulated for.
 * @param input_bdl_wires The input BDL wires of `lyt`.
 * @param output_bdl_wires The output BDL wires of `lyt`.
 * @return The energies with their state types.
 */
template <typename TT>
[[nodiscard]] energy_and_state_type
calculate_energy_and_state_type_with_kinks_rejected(const layout& lyt, const energy_distribution& energy_dist,
                                                    const std::vector<charge_distribution>& valid_charge_distributions,
                                                    const std::vector<TT>& spec, const uint64_t input_index,
                                                    const std::vector<logic::bdl_wire>& input_bdl_wires,
                                                    const std::vector<logic::bdl_wire>& output_bdl_wires) noexcept
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    energy_and_state_type est{};

    energy_dist.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& cd : valid_charge_distributions)
            {
                if (std::abs(cd.energy() - energy) >= utils::math::ERROR_MARGIN)
                {
                    continue;
                }

                est.emplace_back(energy, state_type::ACCEPTED);

                logic::is_operational_params params{};
                params.op_condition = logic::is_operational_params::operational_condition::REJECT_KINKS;

                const auto op_status =
                    logic::verify_logic_match(lyt, cd, params, spec, input_index, input_bdl_wires, output_bdl_wires);

                if (op_status == logic::operational_status::NON_OPERATIONAL)
                {
                    est.emplace_back(energy, state_type::REJECTED);
                    break;
                }
            }
        });

    return est;
}

}  // namespace fiction::sidb::simulation::analysis
