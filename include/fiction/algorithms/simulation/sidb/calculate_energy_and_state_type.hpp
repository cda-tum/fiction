//
// Created by Jan Drewniok on 20.04.23.
//

#ifndef FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
#define FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/verify_logic_match.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math_utils.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

namespace fiction
{

/**
 *  Data type to collect electrostatic potential energies (in eV) of charge distributions with corresponding state types
 * (i.e., `true` = transparent, `false` = erroneous).
 */
using sidb_energy_and_state_type = std::vector<std::pair<double, bool>>;

/**
 * This function takes in an SiDB energy distribution. For each charge distribution, the state type is determined (i.e.
 * erroneous, transparent) while kinks are accepted, meaning a state with kinks is considered transparent.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param energy_distribution Energy distribution.
 * @param valid_charge_distributions Physically valid charge distributions.
 * @param output_bdl_pairs Output BDL pairs.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_index The index of the current input configuration.
 * @return Electrostatic potential energy of all charge distributions with state type.
 */
template <typename Lyt, typename TT>
[[nodiscard]] sidb_energy_and_state_type calculate_energy_and_state_type_with_kinks_accepted(
    const energy_distribution&                           energy_distribution,
    const std::vector<charge_distribution_surface<Lyt>>& valid_charge_distributions,
    const std::vector<bdl_pair<cell<Lyt>>>& output_bdl_pairs, const std::vector<TT>& spec,
    const uint64_t input_index) noexcept

{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!output_bdl_pairs.empty() && "No output cell provided.");
    assert((spec.size() == output_bdl_pairs.size()) && "Number of truth tables and output BDL pairs does not match");

    sidb_energy_and_state_type energy_and_state_type{};

    energy_distribution.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& valid_layout : valid_charge_distributions)
            {
                if (std::abs(valid_layout.get_electrostatic_potential_energy() - energy) < constants::ERROR_MARGIN)
                {
                    bool correct_output = true;

                    for (auto i = 0u; i < output_bdl_pairs.size(); i++)
                    {
                        if (static_cast<bool>(-charge_state_to_sign(valid_layout.get_charge_state(
                                output_bdl_pairs[i].lower))) != kitty::get_bit(spec[i], input_index))
                        {
                            correct_output = false;
                        }
                    }

                    // The output SiDB matches the truth table entry. Hence, the state is called transparent.
                    energy_and_state_type.emplace_back(energy, correct_output);
                }
            }
        });

    return energy_and_state_type;
}

/**
 * This function takes in an SiDB energy distribution. For each charge distribution, the state type is determined (i.e.
 * erroneous, transparent) while kinks are rejected, meaning a state with kinks is considered erroneous.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param energy_distribution Energy distribution.
 * @param valid_charge_distributions Physically valid charge distributions.
 * @param output_bdl_pairs Output BDL pairs.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_index The index of the current input configuration.
 * @param input_bdl_wires Input BDL wires.
 * @param output_bdl_wires Output BDL wires.
 * @return Electrostatic potential energy of all charge distributions with state type.
 */
template <typename Lyt, typename TT>
[[nodiscard]] sidb_energy_and_state_type calculate_energy_and_state_type_with_kinks_rejected(
    const energy_distribution&                           energy_distribution,
    const std::vector<charge_distribution_surface<Lyt>>& valid_charge_distributions, const std::vector<TT>& spec,
    const uint64_t input_index, const std::vector<bdl_wire<Lyt>>& input_bdl_wires,
    std::vector<bdl_wire<Lyt>>& output_bdl_wires) noexcept

{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    sidb_energy_and_state_type energy_and_state_type{};

    energy_distribution.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& valid_layout : valid_charge_distributions)
            {
                if (std::abs(valid_layout.get_electrostatic_potential_energy() - energy) < constants::ERROR_MARGIN)
                {
                    bool correct_output = true;

                    is_operational_params params{};
                    params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

                    const auto operational_status =
                        verify_logic_match(valid_layout, params, spec, input_index, input_bdl_wires, output_bdl_wires);
                    if (operational_status == operational_status::NON_OPERATIONAL)
                    {
                        correct_output = false;
                    }

                    // The output SiDB matches the truth table entry. Hence, state is called transparent.
                    energy_and_state_type.emplace_back(energy, correct_output);
                }
            }
        });

    return energy_and_state_type;
}

}  // namespace fiction

#endif  // FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
