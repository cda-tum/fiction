//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_IS_OPERATIONAL_HPP
#define FICTION_IS_OPERATIONAL_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Possible operational status of a layout.
 */
enum class operational_status : uint8_t
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
 * Condition which is used to decide if a layout is `operational` or `non-operational`.
 */
enum class operational_condition : uint8_t
{
    /**
     * Even if the I/O pins show kinks, the layout is still considered as `operational`.
     */
    TOLERATE_KINKS,
    /**
     * The I/O pins are not allowed to show kinks. If kinks exist, the layout is considered as `non-operational`.
     */
    REJECT_KINKS
};

/**
 * Parameters for the `is_operational` algorithm.
 */
struct is_operational_params
{
    /**
     * The simulation parameters for the physical simulation of the ground state.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * Parameters for the BDL input iterator.
     */
    bdl_input_iterator_params input_bdl_iterator_params{};
    /**
     * Condition which is used to decide if a layout is `operational` or `non-operational`.
     */
    operational_condition op_condition = operational_condition::TOLERATE_KINKS;
};

namespace detail
{
/**
 * Implementation of the `is_operational` algorithm for a given gate layout.
 *
 * This class provides an implementation of the `is_operational` algorithm for
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
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& tt, const is_operational_params& params) :
            layout{lyt},
            truth_table{tt},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii(bdl_input_iterator<Lyt>{lyt, params.input_bdl_iterator_params}),
            input_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT)},
            output_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT)}
    {}
    /**
     * Constructor to initialize the algorithm with a layout and parameters.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param tt Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     * @param input_wires BDL input wires of lyt.
     * @param output_wires BDL output wires of lyt.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& tt, const is_operational_params& params,
                        const std::vector<bdl_wire<Lyt>>& input_wires, const std::vector<bdl_wire<Lyt>>& output_wires) :
            layout{lyt},
            truth_table{tt},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii{bdl_input_iterator<Lyt>{layout, params.input_bdl_iterator_params, input_wires}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires}
    {}

    /**
     * Run the `is_operational` algorithm.
     *
     * This function executes the operational status checking algorithm for the gate layout
     * and parameters provided during initialization.
     *
     * @return The operational status of the gate layout (either `OPERATIONAL` or `NON_OPERATIONAL`).
     */
    [[nodiscard]] operational_status run() noexcept
    {
        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        bool at_least_one_non_operational_due_to_kinks = false;

        // number of different input combinations
        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            ++simulator_invocations;

            // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
            if (can_positive_charges_occur(*bii, parameters.simulation_parameters))
            {
                return operational_status::NON_OPERATIONAL;
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            const auto simulation_results = physical_simulation_of_layout(bii);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (simulation_results.charge_distributions.empty())
            {
                return operational_status::NON_OPERATIONAL;
            }

            const auto ground_states = determine_groundstate_from_simulation_results(simulation_results);

            for (const auto& gs : ground_states)
            {
                const auto op_status = verifiy_logic_match_of_cds(gs, i);
                if (op_status == operational_status::NON_OPERATIONAL &&
                    parameters.op_condition == operational_condition::TOLERATE_KINKS)
                {
                    return operational_status::NON_OPERATIONAL;
                }
                if (op_status == operational_status::NON_OPERATIONAL &&
                    parameters.op_condition == operational_condition::REJECT_KINKS)
                {
                    at_least_one_non_operational_due_to_kinks = true;
                    continue;
                }
            }
        }

        if (at_least_one_non_operational_due_to_kinks)
        {
            return operational_status::NON_OPERATIONAL;
        }

        // if we made it here, the layout is operational
        return operational_status::OPERATIONAL;
    }
    /**
     * Checks if the given charge distribution correctly encodes the expected logic for the given input pattern,
     * based on a provided truth table.
     *
     * Example:
     * In the ground state charge distribution of an AND gate, kinks are rejected for the gate to be considered
     * operational. Given an input pattern of `01`, this function will:
     * - Verify that the left input wire encodes `0`.
     * - Verify that the right input wire encodes `1`.
     * - Verify that the output wire encodes `0`.
     * Determines if the given charge distribution fulfills the correct logic based on the provided charge index and
     * truth table.
     *
     * @param given_cds The charge distribution surface to be checked for operation.
     * @param input_pattern Input pattern represented by the position of perturbers.
     * @return Operational status indicating if the layout is `operational` or `non-operational`.
     */
    [[nodiscard]] operational_status verifiy_logic_match_of_cds(const charge_distribution_surface<Lyt>& given_cds,
                                                                const uint64_t input_pattern) noexcept
    {
        non_operational_due_to_kinks = false;

        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
        if (can_positive_charges_occur(given_cds, parameters.simulation_parameters))
        {
            return operational_status::NON_OPERATIONAL;
        }

        // fetch the charge states of the output BDL pair
        for (auto output = 0u; output < output_bdl_pairs.size(); output++)
        {
            const auto charge_state_output_upper = given_cds.get_charge_state(output_bdl_pairs[output].upper);
            const auto charge_state_output_lower = given_cds.get_charge_state(output_bdl_pairs[output].lower);

            // if the output charge states are equal, the layout is not operational
            if (charge_state_output_lower == charge_state_output_upper)
            {
                return operational_status::NON_OPERATIONAL;
            }

            // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
            if (kitty::get_bit(truth_table[output], input_pattern))
            {
                if (!encodes_bit_one(given_cds, output_bdl_pairs[output], output_bdl_wires[output].port))
                {
                    return operational_status::NON_OPERATIONAL;
                }
            }
            // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
            else
            {
                if (!encodes_bit_zero(given_cds, output_bdl_pairs[output], output_bdl_wires[output].port))
                {
                    return operational_status::NON_OPERATIONAL;
                }
            }

            if (parameters.op_condition == operational_condition::REJECT_KINKS)
            {
                if (check_existence_of_kinks_in_input_wires(given_cds, input_pattern) ||
                    check_existence_of_kinks_in_output_wires(given_cds, input_pattern))
                {
                    non_operational_due_to_kinks = true;
                }
            }
        }

        if (non_operational_due_to_kinks)
        {
            return operational_status::NON_OPERATIONAL;
        }

        // if we made it here, the layout is operational
        return operational_status::OPERATIONAL;
    }
    /**
     * Determines the input combinations yielding the wrong output and the information if non-operational status is due
     * to kinks. `True` indicates that the layout is non-operational due to kinks, `false` otherwise.
     *
     * @return Set of pairs where the first element is the input pattern (e.g. 2-input Boolean function: 00 ^= 0;
     * 10 ^= 2) for which the wrong output is computed. The second entry indicates the information if the
     * non-operational status is due to kinks. `True` indicates that the layout is non-operational due to kinks, `false`
     * otherwise.
     */
    [[nodiscard]] std::set<std::pair<uint64_t, bool>>
    determine_non_operational_input_patterns_and_op_status_due_kinks() noexcept
    {
        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        std::set<std::pair<uint64_t, bool>> non_operational_inputs_and_op_status_due_to_kinks{};

        // number of different input combinations
        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            ++simulator_invocations;

            // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
            if (can_positive_charges_occur(*bii, parameters.simulation_parameters))
            {
                continue;
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            const auto simulation_results = physical_simulation_of_layout(bii);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (simulation_results.charge_distributions.empty())
            {
                continue;
            }

            const auto ground_states = determine_groundstate_from_simulation_results(simulation_results);

            for (const auto& gs : ground_states)
            {
                const auto op_status = verifiy_logic_match_of_cds(gs, i);
                if (op_status == operational_status::NON_OPERATIONAL)
                {
                    non_operational_inputs_and_op_status_due_to_kinks.insert({i, non_operational_due_to_kinks});
                }
            }
        }

        // if we made it here, the layout is operational
        return non_operational_inputs_and_op_status_due_to_kinks;
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

    /**
     * Returns whether the layout is non-operational due to kinks.
     *
     * @return `true` if the layout is non-operational due to kinks, `false` otherwise.
     */
    [[nodiscard]] bool is_non_operational_due_to_kinks() const noexcept
    {
        return non_operational_due_to_kinks;
    }

  private:
    /**
     * SiDB cell-level layout.
     */
    const Lyt layout;
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table{};
    /**
     * Parameters for the `is_operational` algorithm.
     */
    is_operational_params parameters;
    /**
     * Output BDL pairs.
     */
    std::vector<bdl_pair<cell<Lyt>>> output_bdl_pairs;
    /**
     * Iterator that iterates over all possible input states.
     */
    bdl_input_iterator<Lyt> bii;
    /**
     * Input BDL wires.
     */
    std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * Output BDL wires.
     */
    std::vector<bdl_wire<Lyt>> output_bdl_wires;
    /**
     * Number of simulator invocations.
     */
    std::size_t simulator_invocations{0};
    /**
     * Indicates whether the layout is non-operational due to kinks.
     */
    bool non_operational_due_to_kinks = false;
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
        assert(parameters.simulation_parameters.base == 2 && "base number is set to 3");

        if (parameters.sim_engine == sidb_simulation_engine::EXGS)
        {
            // perform an exhaustive ground state simulation
            return exhaustive_ground_state_simulation(*bdl_iterator, parameters.simulation_parameters);
        }
        if constexpr (!is_sidb_defect_surface_v<Lyt>)
        {
            if (parameters.sim_engine == sidb_simulation_engine::QUICKSIM)
            {
                // perform a heuristic simulation
                const quicksim_params qs_params{parameters.simulation_parameters, 500, 0.6};
                return quicksim(*bdl_iterator, qs_params);
            }
        }
        if (parameters.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform exact simulation
            const quickexact_params<cell<Lyt>> quickexact_params{
                parameters.simulation_parameters,
                fiction::quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};
            return quickexact(*bdl_iterator, quickexact_params);
        }

        assert(false && "unsupported simulation engine");

        return sidb_simulation_result<Lyt>{};
    }
    /**
     * This function iterates through the input wires and evaluates their charge states against the expected
     * states derived from the input pattern. A kink is considered to exist if an input wire's charge state does not
     * match the expected value (i.e., bit one or bit zero) for the given input index.
     *
     * @tparam Lyt SiDB cell-level layout type
     * @param ground_state The ground state charge distribution surface.
     * @param current_input_index The current input index used to retrieve the expected output from the truth table.
     * @return `true` if any input wire contains a kink (i.e., an unexpected charge state), `false` otherwise.
     */
    [[nodiscard]] bool check_existence_of_kinks_in_input_wires(const charge_distribution_surface<Lyt>& ground_state,
                                                               const uint64_t current_input_index) const noexcept
    {
        for (auto i = 0u; i < input_bdl_wires.size(); i++)
        {
            if (input_bdl_wires[input_bdl_wires.size() - 1 - i].port.dir == port_direction::SOUTH)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        if (!encodes_bit_one(ground_state, bdl, input_bdl_wires[input_bdl_wires.size() - 1 - i].port))
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        if (!encodes_bit_zero(ground_state, bdl, input_bdl_wires[input_bdl_wires.size() - 1 - i].port))
                        {
                            return true;
                        }
                    }
                }
            }
            else if (input_bdl_wires[input_bdl_wires.size() - 1 - i].port.dir == port_direction::NORTH)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        if (!encodes_bit_one(ground_state, bdl, input_bdl_wires[input_bdl_wires.size() - 1 - i].port))
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        if (!encodes_bit_zero(ground_state, bdl, input_bdl_wires[input_bdl_wires.size() - 1 - i].port))
                        {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    /**
     * This function iterates through the output wires and evaluates their charge states against the expected
     * states derived from the truth table. A kink is considered to exist if an output wire's charge state does not
     * match the expected value (i.e., bit one or bit zero) for the given input index.
     *
     * @tparam Lyt SiDB cell-level layout type
     * @param ground_state The ground state charge distribution surface.
     * @param current_input_index The current input index used to retrieve the expected output from the truth table.
     * @return `true` if any output wire contains a kink (i.e., an unexpected charge state), `false` otherwise.
     */
    [[nodiscard]] bool check_existence_of_kinks_in_output_wires(const charge_distribution_surface<Lyt>& ground_state,
                                                                const uint64_t current_input_index) const noexcept
    {
        for (auto i = 0u; i < output_bdl_wires.size(); i++)
        {
            for (const auto& bdl : output_bdl_wires[i].pairs)
            {
                if (kitty::get_bit(truth_table[i], current_input_index))
                {
                    if (!encodes_bit_one(ground_state, bdl, output_bdl_wires[i].port))
                    {
                        return true;
                    }
                }
                else
                {
                    if (!encodes_bit_zero(ground_state, bdl, output_bdl_wires[i].port))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * This function returns `true` if `0` is encoded in the charge state of the given BDL pair. `false` otherwise.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param ground_state The ground state charge distribution surface.
     * @param bdl BDL pair to be evaluated.
     * @return `true` if `0` is encoded, `false` otherwise.
     */
    [[nodiscard]] bool encodes_bit_zero(const charge_distribution_surface<Lyt>& ground_state,
                                        const bdl_pair<cell<Lyt>>& bdl, const port_direction port) const noexcept
    {
        if (port.dir == port_direction::SOUTH || port.dir == port_direction::EAST || port.dir == port_direction::NONE)
        {
            return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb_charge_state::NEGATIVE) &&
                                     (ground_state.get_charge_state(bdl.lower) == sidb_charge_state::NEUTRAL));
        }
        return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb_charge_state::NEUTRAL) &&
                                 (ground_state.get_charge_state(bdl.lower) == sidb_charge_state::NEGATIVE));
    }

    /**
     * This function returns `true` if `1` is encoded in the charge state of the given BDL pair. `false` otherwise.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param ground_state The ground state charge distribution surface.
     * @param bdl BDL pair to be evaluated.
     * @return `true` if `1` is encoded, `false` otherwise.
     */
    [[nodiscard]] bool encodes_bit_one(const charge_distribution_surface<Lyt>& ground_state,
                                       const bdl_pair<cell<Lyt>>& bdl, const port_direction port) const noexcept
    {
        if (port.dir == port_direction::SOUTH || port.dir == port_direction::EAST || port.dir == port_direction::NONE)
        {
            return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb_charge_state::NEUTRAL) &&
                                     (ground_state.get_charge_state(bdl.lower) == sidb_charge_state::NEGATIVE));
        }

        return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb_charge_state::NEGATIVE) &&
                                 (ground_state.get_charge_state(bdl.lower) == sidb_charge_state::NEUTRAL));
    }
};

}  // namespace detail

/**
 * Determine the operational status of an SiDB layout.
 *
 * This function checks the operational status of a given gate layout using the `is_operational` algorithm. It
 * determines whether the gate layout is operational and returns the correct result for all \f$2^n\f$ input
 * combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the layout behavior.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @return A pair containing the operational status of the gate layout (either `OPERATIONAL` or `NON_OPERATIONAL`) and
 * the number of input combinations tested.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {},
               const std::optional<std::vector<bdl_wire<Lyt>>>& input_bdl_wire  = std::nullopt,
               const std::optional<std::vector<bdl_wire<Lyt>>>& output_bdl_wire = std::nullopt)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (input_bdl_wire.has_value() && output_bdl_wire.has_value())
    {
        detail::is_operational_impl<Lyt, TT> p{lyt, spec, params, input_bdl_wire.value(), output_bdl_wire.value()};

        return {p.run(), p.get_number_of_simulator_invocations()};
    }

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    return {p.run(), p.get_number_of_simulator_invocations()};
}
/**
 * This function determines the input combinations for which the SiDB-based logic, represented by the
 * provided layout (`lyt`) and truth table specifications (`spec`), produces the correct output.
 *
 * @tparam Lyt Type of the cell-level layout.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB layout.
 * @param spec Vector of truth table specifications.
 * @param params Parameters to simulate if a input combination is operational.
 * @return The count of operational input combinations.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::set<uint64_t> operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec,
                                                            const is_operational_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    std::set<uint64_t> input_patterns{};

    // all possible input patterns
    for (auto i = 0u; i < spec.front().num_bits(); ++i)
    {
        input_patterns.insert(i);
    }

    const auto non_op_patterns_and_op_status = p.determine_non_operational_input_patterns_and_op_status_due_kinks();

    for (const auto& pattern : non_op_patterns_and_op_status)
    {
        input_patterns.erase(pattern.first);
    }

    return input_patterns;
}

/**
 * This function determines the input combinations for which the SiDB-based logic, represented by the
 * provided layout (`lyt`) and truth table specifications (`spec`), are non-operational due to kinks.
 *
 * @tparam Lyt Type of the cell-level layout.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB layout.
 * @param spec Vector of truth table specifications.
 * @param params Parameters to simulate if a input combination is operational.
 * @return The non-operational input combinations due to kinks.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::set<uint64_t>
non_operational_input_patterns_due_to_kinks(const Lyt& lyt, const std::vector<TT>& spec,
                                            const is_operational_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = operational_condition::REJECT_KINKS;

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks};

    std::set<uint64_t> non_op_patterns_due_to_kinks{};

    const auto non_op_patterns = p.determine_non_operational_input_patterns_and_op_status_due_kinks();

    for (const auto& pattern_and_non_op_kinks : non_op_patterns)
    {
        if (pattern_and_non_op_kinks.second)
        {
            non_op_patterns_due_to_kinks.insert(pattern_and_non_op_kinks.first);
        }
    }

    return non_op_patterns_due_to_kinks;
}

/**
 * Determines if the layout is non-operational due to kinks.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the layout behavior.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @return Bool that indicates whether the layout is non-operational due to kinks. `true` if the layout is not usable
 * due to kinks, `false` otherwise.
 *
 */
template <typename Lyt, typename TT>
[[nodiscard]] bool
is_non_operational_due_to_kinks(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {},
                                const std::optional<std::vector<bdl_wire<Lyt>>>& input_bdl_wire  = std::nullopt,
                                const std::optional<std::vector<bdl_wire<Lyt>>>& output_bdl_wire = std::nullopt)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = operational_condition::REJECT_KINKS;

    if (input_bdl_wire.has_value() && output_bdl_wire.has_value())
    {
        detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks, input_bdl_wire.value(),
                                               output_bdl_wire.value()};

        const auto op_status = p.run();

        return static_cast<bool>(op_status != operational_status::OPERATIONAL);
    }

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks};

    const auto op_status = p.run();

    return static_cast<bool>(op_status != operational_status::OPERATIONAL);
}

}  // namespace fiction

#endif  // FICTION_IS_OPERATIONAL_HPP
