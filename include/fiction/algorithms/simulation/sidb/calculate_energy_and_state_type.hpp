//
// Created by Jan Drewniok on 20.04.23.
//

#ifndef FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
#define FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/does_cds_match_logic_for_input_pattern.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
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
    const sidb_energy_distribution&                      energy_distribution,
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

    for (const auto& [energy, occurrence] : energy_distribution)
    {
        // round the energy value to six decimal places to overcome potential rounding errors.
        const auto energy_value = round_to_n_decimal_places(energy, 6);
        for (const auto& valid_layout : valid_charge_distributions)
        {
            // round the energy value of the given valid_layout to six decimal places to overcome possible rounding
            // errors and to provide comparability with the energy_value from before.
            if (std::abs(round_to_n_decimal_places(valid_layout.get_system_energy(), 6) - energy_value) <
                physical_constants::POP_STABILITY_ERR)
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

                // The output SiDB matches the truth table entry. Hence, state is called transparent.
                energy_and_state_type.emplace_back(energy, correct_output);
            }
        }
    }

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
 * @param input_and_output_wires Input and output BDL wires.
 * @return Electrostatic potential energy of all charge distributions with state type.
 */
template <typename Lyt, typename TT>
[[nodiscard]] sidb_energy_and_state_type calculate_energy_and_state_type_with_kinks_rejected(
    const sidb_energy_distribution&                      energy_distribution,
    const std::vector<charge_distribution_surface<Lyt>>& valid_charge_distributions, const std::vector<TT>& spec,
    const uint64_t input_index, const std::pair<bdl_wires<Lyt>, bdl_wires<Lyt>>& input_and_output_wires) noexcept

{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    sidb_energy_and_state_type energy_and_state_type{};

    for (const auto& [energy, occurrence] : energy_distribution)
    {
        // round the energy value to six decimal places to overcome potential rounding errors.
        const auto energy_value = round_to_n_decimal_places(energy, 6);
        for (const auto& valid_layout : valid_charge_distributions)
        {
            // round the energy value of the given valid_layout to six decimal places to overcome possible rounding
            // errors and to provide comparability with the energy_value from before.
            if (std::abs(round_to_n_decimal_places(valid_layout.get_system_energy(), 6) - energy_value) <
                physical_constants::POP_STABILITY_ERR)
            {
                bool correct_output = true;

                is_operational_params params{};
                params.op_condition = operational_condition::REJECT_KINKS;

                const auto operational_status = does_cds_match_logic_for_input_pattern(
                    valid_layout, params, spec, input_index, input_and_output_wires);
                if (operational_status == operational_status::NON_OPERATIONAL)
                {
                    correct_output = false;
                }

                // The output SiDB matches the truth table entry. Hence, state is called transparent.
                energy_and_state_type.emplace_back(energy, correct_output);
            }
        }
    }

    return energy_and_state_type;
}

}  // namespace fiction

#endif  // FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
