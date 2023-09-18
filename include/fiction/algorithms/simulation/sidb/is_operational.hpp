//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_IS_OPERATIONAL_HPP
#define FICTION_IS_OPERATIONAL_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace fiction
{

struct is_operational_params
{
    /**
     * Possible operational status of a layout.
     */
    enum class operational_status
    {
        /**
         * The layout is operational.
         */
        OPERATIONAL,
        /**
         * The layout is non-operational.
         */
        NON_OPERATIONAL
    };
    /**
     * The simulation parameters for the physical simulation of the ground state.
     */
    sidb_simulation_parameters simulation_parameter{};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * Parameters for the BDL pair detection algorithms.
     */
    detect_bdl_pairs_params bdl_params{};
};

namespace detail
{
/**
 * Implementation of the `is_sp_operational` algorithm for a given gate layout.
 *
 * This class provides an implementation of the `is_sp_operational` algorithm for
 * a specified gate layout and parameters. It checks whether the gate layout is operational
 * by simulating its behavior for different input combinations and comparing the results
 * to expected outputs from a truth table.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 */
template <typename Lyt, typename TT>
class is_operational_impl
{
  public:
    /**
     * Constructor to initialize the algorithm with a layout and parameters.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param params Parameters for the `is_sp_operational` algorithm.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& tt, const is_operational_params& params) :
            layout{lyt},
            truth_table{tt},
            parameter{params},
            output_bdl_pairs(detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT, parameter.bdl_params)),
            bii(bdl_input_iterator<Lyt>{layout, parameter.bdl_params})
    {}

    /**
     * Run the `is_sp_operational` algorithm.
     *
     * This function executes the operational status checking algorithm for the gate layout
     * and parameters provided during initialization.
     *
     * @return The operational status of the gate layout (either `OPERATIONAL` or `NON_OPERATIONAL`).
     */
    [[nodiscard]] is_operational_params::operational_status run() noexcept
    {
        // number of different input combinations
        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            ++simulator_invocations;
            // collects expected output values for a given input configuration.
            const auto expected_outputs = outputs_for_given_input(i);

            // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
            if (can_positive_charges_occur(*bii, parameter.simulation_parameter))
            {
                return is_operational_params::operational_status::NON_OPERATIONAL;
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            const auto simulation_results = physical_simulation_of_layout(bii);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (simulation_results.charge_distributions.empty())
            {
                return is_operational_params::operational_status::NON_OPERATIONAL;
            }

            // find the ground state, which is the charge distribution with the lowest energy
            const auto ground_state = std::min_element(
                simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
                [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

            // ground state is degenerate
            if ((energy_distribution(simulation_results.charge_distributions).begin()->second) > 1)
            {
                return is_operational_params::operational_status::NON_OPERATIONAL;
            }

            // fetch the charge states of the output BDL pair
            for (auto output = 0u; output < output_bdl_pairs.size(); output++)
            {
                const auto charge_state_output_upper = ground_state->get_charge_state(output_bdl_pairs[output].upper);
                const auto charge_state_output_lower = ground_state->get_charge_state(output_bdl_pairs[output].lower);

                // if the output charge states are equal, the layout is not operational
                if (charge_state_output_lower == charge_state_output_upper)
                {
                    return is_operational_params::operational_status::NON_OPERATIONAL;
                }

                // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
                if (expected_outputs[output])
                {
                    if (charge_state_output_upper != sidb_charge_state::NEUTRAL ||
                        charge_state_output_lower != sidb_charge_state::NEGATIVE)
                    {
                        return is_operational_params::operational_status::NON_OPERATIONAL;
                    }
                }
                // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
                else
                {
                    if (charge_state_output_upper != sidb_charge_state::NEGATIVE ||
                        charge_state_output_lower != sidb_charge_state::NEUTRAL)
                    {
                        return is_operational_params::operational_status::NON_OPERATIONAL;
                    }
                }
            }
        }

        // if we made it here, the layout is operational
        return is_operational_params::operational_status::OPERATIONAL;
    }
    /**
     * Returns the total number of simulator invocations.
     *
     * @return The number of simulator invocations.
     */
    [[nodiscard]] std::size_t get_number_of_simulator_invocations() const noexcept
    {
        return simulator_invocations;
    }

  private:
    /**
     * SiDB cell-level layout.
     */
    const Lyt& layout{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
    /**
     * Parameters for the `is_sp_operational` algorithm.
     */
    is_operational_params parameter{};
    /**
     * Number of simulator invocations.
     */
    std::size_t simulator_invocations{0};
    /**
     * Output BDL pairs.
     */
    std::vector<bdl_pair<Lyt>> output_bdl_pairs{};
    /**
     * Iterator that iterates over all possible input states.
     */
    bdl_input_iterator<Lyt> bii{};
    /**
     * This function conducts physical simulation of the given layout (gate layout with certain input combination). The
     * simulation results are stored in the `sim_result` variable.
     *
     * @param bdl_iterator A reference to a BDL input iterator representing the gate layout at a given input
     * combination. The simulation is performed based on the configuration represented by the iterator.
     * @return Simulation results.
     */
    [[nodiscard]] sidb_simulation_result<Lyt>
    physical_simulation_of_layout(const bdl_input_iterator<Lyt>& bdl_iterator) noexcept
    {
        assert(parameter.simulation_parameter.base == 2 && "base number is set to 3");
        if (parameter.sim_engine == sidb_simulation_engine::EXGS)
        {
            // perform an exhaustive ground state simulation
            return exhaustive_ground_state_simulation(*bdl_iterator, parameter.simulation_parameter);
        }
        if (parameter.sim_engine == sidb_simulation_engine::QUICKSIM)
        {
            // perform a heuristic simulation
            const quicksim_params qs_params{parameter.simulation_parameter, 500, 0.6};
            return quicksim(*bdl_iterator, qs_params);
        }
        if (parameter.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform exact simulation
            const quickexact_params<Lyt> quickexact_params{
                parameter.simulation_parameter, fiction::quickexact_params<Lyt>::automatic_base_number_detection::OFF};
            return quickexact(*bdl_iterator, quickexact_params);
        }

        assert(false && "unsupported simulation engine");

        return sidb_simulation_result<Lyt>{};
    }
    /**
     * Collects expected output values for a given input configuration.
     *
     * This function determines the expected output values for a specific input configuration
     * based on the provided truth table and stores them in the `expected_outputs` vector.
     * The function takes into account the number of variables and bits in the truth table
     * to correctly extract the expected output values for different gate types.
     *
     * @param input_bit The index of the current input configuration.
     * @return Vector representing the expected output for the given input.
     */
    [[nodiscard]] std::vector<bool> outputs_for_given_input(const uint64_t input_bit)
    {
        std::vector<bool> expected_outputs;
        expected_outputs.reserve(truth_table.size());

        std::transform(truth_table.begin(), truth_table.end(), std::back_inserter(expected_outputs),
                       [input_bit](const auto& output) { return kitty::get_bit(output, input_bit) != 0u; });

        return expected_outputs;
    }
};

}  // namespace detail

/**
 * Check the Operational Status of an SiDB gate layout.
 *
 * This function checks the operational status of a given gate layout using the
 * `is_sp_operational` algorithm. It determines whether the gate layout
 * is operational and returns the correct result for all \f$ 2^n \f$ input combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Vector of truth tables that specify the behavior of the gate. Each truth table represents an output of
 * the Boolean function.
 * @param parameter Parameters for the `is_sp_operational` algorithm.
 * @return A pair containing the operational status of the gate layout
 *         (either `OPERATIONAL` or `NON_OPERATIONAL) and the number of input combinations tested.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::pair<is_operational_params::operational_status, std::size_t>
is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& parameter)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, parameter};

    return {p.run(), p.get_number_of_simulator_invocations()};
}

}  // namespace fiction

#endif  // FICTION_IS_OPERATIONAL_HPP
