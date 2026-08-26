//
// Created by Jan Drewniok on 20.04.23.
//

#ifndef FICTION_TECHNOLOGY_SIDB_SIMULATION_ANALYSIS_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
#define FICTION_TECHNOLOGY_SIDB_SIMULATION_ANALYSIS_CALCULATE_ENERGY_AND_STATE_TYPE_HPP

#include "fiction/technology/fcn/constants.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/primitives/charge_distribution_surface.hpp"
#include "fiction/technology/sidb/simulation/analysis/energy_distribution.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/simulation/logic/verify_logic_match.hpp"
#include "fiction/traits.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
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
template <typename Lyt, typename TT>
[[nodiscard]] energy_and_state_type calculate_energy_and_state_type_with_kinks_accepted(
    const energy_distribution&                                             energy_dist,
    const std::vector<sidb::primitives::charge_distribution_surface<Lyt>>& valid_charge_distributions,
    const std::vector<sidb::simulation::logic::bdl_pair<cell<Lyt>>>& output_bdl_pairs, const std::vector<TT>& spec,
    const uint64_t input_index) noexcept

{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!output_bdl_pairs.empty() && "No output cell provided.");
    assert((spec.size() == output_bdl_pairs.size()) && "Number of truth tables and output BDL pairs does not match");

    energy_and_state_type est{};

    energy_dist.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& valid_layout : valid_charge_distributions)
            {
                if (std::abs(valid_layout.get_electrostatic_potential_energy() - energy) < fcn::constants::ERROR_MARGIN)
                {
                    state_type type_of_considered_state = state_type::ACCEPTED;

                    for (auto i = 0u; i < output_bdl_pairs.size(); i++)
                    {
                        if (static_cast<bool>(-sidb::model::charge_state_to_sign(valid_layout.get_charge_state(
                                output_bdl_pairs[i].lower))) != kitty::get_bit(spec[i], input_index))
                        {
                            // The output SiDB matches the truth table entry. Hence, the state is called transparent.
                            type_of_considered_state = state_type::REJECTED;
                            break;
                        }
                    }
                    est.emplace_back(energy, type_of_considered_state);
                }
            }
        });

    std::ranges::sort(est, [](const auto& a, const auto& b) { return a.first < b.first; });

    return est;
}

/**
 * This function takes in an SiDB energy distribution. For each charge distribution, the state type is determined (i.e.
 * erroneous, transparent) while kinks are rejected, meaning a state with kinks is considered erroneous.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param energy_dist Energy distribution.
 * @param valid_charge_distributions Physically valid charge distributions.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_index The index of the current input configuration.
 * @param input_bdl_wires Input BDL wires.
 * @param output_bdl_wires Output BDL wires.
 * @return Electrostatic potential energy of all charge distributions with state type.
 */
template <typename Lyt, typename TT>
[[nodiscard]] energy_and_state_type calculate_energy_and_state_type_with_kinks_rejected(
    const energy_distribution&                                             energy_dist,
    const std::vector<sidb::primitives::charge_distribution_surface<Lyt>>& valid_charge_distributions,
    const std::vector<TT>& spec, const uint64_t input_index,
    const std::vector<sidb::simulation::logic::bdl_wire<Lyt>>& input_bdl_wires,
    const std::vector<sidb::simulation::logic::bdl_wire<Lyt>>& output_bdl_wires) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    energy_and_state_type est{};

    energy_dist.for_each(
        [&](const double energy, const uint64_t occurrence [[maybe_unused]])
        {
            for (const auto& valid_layout : valid_charge_distributions)
            {
                if (std::abs(valid_layout.get_electrostatic_potential_energy() - energy) < fcn::constants::ERROR_MARGIN)
                {
                    // The output SiDB matches the truth table entry. Hence, state is called transparent.
                    est.emplace_back(energy, state_type::ACCEPTED);

                    sidb::simulation::logic::is_operational_params params{};
                    params.op_condition =
                        sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;

                    const auto op_status = sidb::simulation::logic::verify_logic_match(
                        valid_layout, params, spec, input_index, input_bdl_wires, output_bdl_wires);
                    if (op_status == sidb::simulation::logic::operational_status::NON_OPERATIONAL)
                    {
                        // The output SiDB matches the truth table entry. Hence, state is called transparent.
                        est.emplace_back(energy, state_type::REJECTED);
                        break;
                    }
                }
            }
        });

    return est;
}

}  // namespace fiction::sidb::simulation::analysis
#endif  // FICTION_TECHNOLOGY_SIDB_SIMULATION_ANALYSIS_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
