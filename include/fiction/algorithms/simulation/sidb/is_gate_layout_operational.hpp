//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_IS_GATE_LAYOUT_OPERATIONAL_HPP
#define FICTION_IS_GATE_LAYOUT_OPERATIONAL_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace fiction
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

struct is_gate_layout_operational_params
{
    /**
     * The simulation parameters for the physical simulation of the ground state.
     */
    sidb_simulation_parameters sim_params{};
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
 * Implementation of the `is_gate_layout_operational` algorithm for a given gate layout.
 *
 * This class provides an implementation of the `is_gate_layout_operational` algorithm for
 * a specified gate layout and parameters. It checks whether the gate layout is operational
 * by simulating its behavior for different input combinations and comparing the results
 * to expected outputs from a truth table.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt, typename TT>
class is_gate_layout_operational_impl
{
  public:
    /**
     * Constructor to initialize the algorithm with a layout and parameters.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param params Parameters for the `is_gate_layout_operational` algorithm.
     */
    is_gate_layout_operational_impl(const Lyt& lyt, const TT& tt, const is_gate_layout_operational_params& params) :
            layout{lyt.clone()},
            truth_table{tt},
            parameter{params}
    {}

    /**
     * Run the `is_gate_layout_operational` algorithm.
     *
     * This function executes the operational status checking algorithm for the gate layout
     * and parameters provided during initialization.
     *
     * @return The operational status of the gate layout (either OPERATIONAL or NON_OPERATIONAL).
     */
    operational_status run() noexcept
    {
        // initialize a BDL input iterator
        bdl_input_iterator<Lyt> bii{layout, parameter.bdl_params};

        // number of input BDl pairs are stored
        number_of_input_bdls = bii.get_number_of_inputs();
        const auto output_bdl_pairs =
            detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT, parameter.bdl_params);
        // number of output BDL pairs are stored
        number_of_output_bdls = output_bdl_pairs.size();

        // for each input combination
        for (auto i = 0u; i < number_of_input_bdls * 2; ++i, ++bii)
        {
            ++simulator_invocations;
            // collects expected output values for a given input configuration.
            outputs_for_given_input(i);

            // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
            if (can_positive_charges_occur(*bii, parameter.sim_params))
            {
                return operational_status::NON_OPERATIONAL;
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            physical_simulation_of_layout(bii);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (sim_result.charge_distributions.empty())
            {
                return operational_status::NON_OPERATIONAL;
            }

            // find the ground state, which is the charge distribution with the lowest energy
            const auto ground_state = std::min_element(
                sim_result.charge_distributions.cbegin(), sim_result.charge_distributions.cend(),
                [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

            // ground state is degenerate
            if ((energy_distribution(sim_result.charge_distributions).begin()->second) > 1)
            {
                return operational_status::NON_OPERATIONAL;
            }

            // fetch the charge states of the output BDL pair
            for (auto output = 0u; output < expected_outputs.size(); output++)
            {
                const auto charge_state_output_upper = ground_state->get_charge_state(output_bdl_pairs[output].upper);
                const auto charge_state_output_lower = ground_state->get_charge_state(output_bdl_pairs[output].lower);

                // if the output charge states are equal, the layout is not operational
                if (charge_state_output_lower == charge_state_output_upper)
                {
                    return operational_status::NON_OPERATIONAL;
                }

                // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
                if (expected_outputs[output])
                {
                    if (charge_state_output_upper != sidb_charge_state::NEUTRAL ||
                        charge_state_output_lower != sidb_charge_state::NEGATIVE)
                    {
                        return operational_status::NON_OPERATIONAL;
                    }
                }
                // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
                else
                {
                    if (charge_state_output_upper != sidb_charge_state::NEGATIVE ||
                        charge_state_output_lower != sidb_charge_state::NEUTRAL)
                    {
                        return operational_status::NON_OPERATIONAL;
                    }
                }
            }
        }

        // if we made it here, the layout is operational
        return operational_status::OPERATIONAL;
    }

    [[nodiscard]] std::size_t get_counter() const noexcept
    {
        return simulator_invocations;
    }

  private:
    /**
     * SiDB cell-level layout.
     */
    Lyt layout{};
    /**
     * The specification of the layout.
     */
    const TT& truth_table;
    /**
     * Parameters for the `is_gate_layout_operational` algorithm.
     */
    is_gate_layout_operational_params parameter{};
    /**
     * Number of simulator invocations.
     */
    std::size_t simulator_invocations{0};
    /**
     * The expected output of the layout for the i-th bit of the truth table
     */
    std::vector<bool> expected_outputs{};
    /**
     * Determines the outputs for the current input index.
     *
     * @param input_bit current input index.
     */
    sidb_simulation_result<Lyt> sim_result{};
    /**
     * Number of output BDL pairs.
     */
    uint64_t number_of_output_bdls{};
    /**
     * Number of input BDL pairs.
     */
    uint64_t number_of_input_bdls{};
    /**
     * This function conducts physical simulation of the given layout (gate layout with certain input combination). The
     * simulation results are stored in the `sim_result` variable.
     *
     * @param bdl_iterator A reference to a BDL input iterator representing the gate layout at a given input
     * combination. The simulation is performed based on the configuration represented by the iterator.
     */
    void physical_simulation_of_layout(const bdl_input_iterator<Lyt>& bdl_iterator) noexcept
    {
        sim_result = {};
        assert(parameter.sim_params.base == 2 && "base number is set to 3");
        if (parameter.sim_engine == sidb_simulation_engine::EXGS)
        {
            // perform an exhaustive ground state simulation
            sim_result = exhaustive_ground_state_simulation(*bdl_iterator, parameter.sim_params);
        }
        else if (parameter.sim_engine == sidb_simulation_engine::QUICKSIM)
        {
            // perform a heuristic simulation
            const quicksim_params qs_params{parameter.sim_params, 500, 0.6};
            sim_result = quicksim(*bdl_iterator, qs_params);
        }
        else if (parameter.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform exact simulation
            const quickexact_params<Lyt> quickexact_params{parameter.sim_params, automatic_base_number_detection::OFF};
            sim_result = quickexact((*bdl_iterator), quickexact_params);
        }
        else
        {
            assert(false && "unsupported simulation engine");
        }
    }
    /**
     * collects expected output values for a given input configuration.
     *
     * This function determines the expected output values for a specific input configuration
     * based on the provided truth table and stores them in the `expected_outputs` vector.
     * The function takes into account the number of variables and bits in the truth table
     * to correctly extract the expected output values for different gate types.
     *
     * @param input_bit The index of the current input configuration.
     */
    void outputs_for_given_input(const uint64_t input_bit)
    {
        expected_outputs = {};
        if (truth_table.num_vars() == 3)  // truth table of (double wire, cx, etc.) consists of three variables
        {
            // double wire, cx, etc
            if (truth_table.num_bits() == 8)  // number of bits of truth table
            {
                assert((number_of_input_bdls == 2 && number_of_output_bdls == 2) &&
                       "Number of input or output BDL pairs is not correct");
                // truth table entries for given inputs are collected
                expected_outputs.push_back(kitty::get_bit(truth_table, input_bit * 2 + 1) != 0u);
                expected_outputs.push_back(kitty::get_bit(truth_table, input_bit * 2) != 0u);
            }
        }

        // wire, inverter, etc
        else if (truth_table.num_vars() == 1 && truth_table.num_bits() == 2)
        {
            assert((number_of_input_bdls == 1 && number_of_output_bdls == 1) &&
                   "Number of input or output BDL pairs is not correct");
            // truth table entry for given input is collected
            expected_outputs.push_back(kitty::get_bit(truth_table, input_bit) != 0u);
        }

        // and, or, nand, fo2, etc
        else if (truth_table.num_vars() == 2)
        {
            if (truth_table.num_bits() == 4 && truth_table != create_fan_out_tt())  // and, or, nand, etc
            {
                assert((number_of_input_bdls == 2 && number_of_output_bdls == 1) &&
                       "Number of input or output BDL pairs is not correct");
                // truth table entry for given inputs is collected
                expected_outputs.push_back(kitty::get_bit(truth_table, input_bit) != 0u);
            }
            // fo2
            else
            {
                assert((number_of_input_bdls == 1 && number_of_output_bdls == 2) &&
                       "Number of onput or output BDL pairs is not correct");
                // truth table entries for given input are collected
                expected_outputs.push_back(kitty::get_bit(truth_table, input_bit * 2 + 1) != 0u);
                expected_outputs.push_back(kitty::get_bit(truth_table, input_bit * 2) != 0u);
            }
        }
    }
};
}  // namespace detail

/**
 * Check the Operational Status of an SiDB gate layout.
 *
 * This function checks the operational status of a given gate layout using the
 * `is_gate_layout_operational` algorithm. It determines whether the gate layout
 * is operational and returns the correct result for all \f$ 2^n \f$ input combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec The truth table specifying the gate behavior.
 * @param parameter Parameters for the `is_gate_layout_operational` algorithm.
 * @return A pair containing the operational status of the gate layout
 *         (either OPERATIONAL or NON_OPERATIONAL) and the number of input combinations tested.
 */
template <typename Lyt, typename TT>
std::pair<operational_status, std::size_t>
is_gate_layout_operational(const Lyt& lyt, const TT& spec, const is_gate_layout_operational_params& parameter)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    detail::is_gate_layout_operational_impl<Lyt, TT> p{lyt, spec, parameter};

    return {p.run(), p.get_counter()};
}

}  // namespace fiction

#endif  // FICTION_IS_GATE_LAYOUT_OPERATIONAL_HPP
