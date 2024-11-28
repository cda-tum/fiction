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
 * Condition which is used to decide if a layout is operational or non-operational.
 */
enum class operational_condition : uint8_t
{
    /**
     * Even if the I/O pins show kinks, the layout is still considered as operational.
     */
    TOLERATE_KINKS,
    /**
     * The I/O pins are not allowed to show kinks. If kinks exist, the layout is considered as non-operational.
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
     * Condition which is used to decide if a layout is operational or non-operational.
     */
    operational_condition op_condition = operational_condition::TOLERATE_KINKS;
};

namespace detail
{

/**
 * Reason why a layout is non-operational.
 */
enum class non_operationality_reason : uint8_t
{
    /**
     * Kinks induced the layout to become non-operational.
     */
    KINKS,
    /**
     * The layout is non-operational because of logic mismatch.
     */
    LOGIC_MISMATCH,
    /**
     * No reason for non-operationality could be determined.
     */
    NONE,
};

/**
 * Implementation of the `is_operational` algorithm for a given gate layout.
 *
 * This class provides an implementation of the `is_operational` algorithm for
 * a specified gate layout and parameters. It checks whether the gate layout is operational
 * by simulating its behavior for different input combinations and comparing the results
 * to expected outputs from a truth table.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
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
     * @return Pair with the first element indicating the operational status (either `OPERATIONAL` or `NON_OPERATIONAL`)
     * and the second element indicating the reason if it is non-operational.
     */
    [[nodiscard]] std::pair<operational_status, non_operationality_reason> run() noexcept
    {
        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        bool at_least_one_layout_is_kink_induced_non_operational = false;

        // number of different input combinations
        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            ++simulator_invocations;

            // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
            if (can_positive_charges_occur(*bii, parameters.simulation_parameters))
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            const auto simulation_results = physical_simulation_of_layout(bii);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (simulation_results.charge_distributions.empty())
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
            }

            const auto ground_states = determine_groundstate_from_simulation_results(simulation_results);

            for (const auto& gs : ground_states)
            {
                const auto [op_status, non_op_reason] = verify_logic_match_of_cds(gs, i);
                if (op_status == operational_status::NON_OPERATIONAL &&
                    non_op_reason == non_operationality_reason::LOGIC_MISMATCH)
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }
                if (op_status == operational_status::NON_OPERATIONAL &&
                    non_op_reason == non_operationality_reason::KINKS)
                {
                    at_least_one_layout_is_kink_induced_non_operational = true;
                    continue;
                }
            }
        }

        if (at_least_one_layout_is_kink_induced_non_operational)
        {
            return {operational_status::NON_OPERATIONAL, non_operationality_reason::KINKS};
        }

        // if we made it here, the layout is operational
        return {operational_status::OPERATIONAL, non_operationality_reason::NONE};
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
     * @return Pair with the first element indicating the operational status (either `OPERATIONAL` or `NON_OPERATIONAL`)
     * and the second element indicating the reason if it is non-operational.
     */
    [[nodiscard]] std::pair<operational_status, non_operationality_reason>
    verify_logic_match_of_cds(const charge_distribution_surface<Lyt>& given_cds, const uint64_t input_pattern) noexcept
    {
        auto non_operational_reason = non_operationality_reason::LOGIC_MISMATCH;

        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
        if (can_positive_charges_occur(given_cds, parameters.simulation_parameters))
        {
            return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
        }

        // fetch the charge states of the output BDL pair
        for (auto output = 0u; output < output_bdl_pairs.size(); output++)
        {
            const auto charge_state_output_upper = given_cds.get_charge_state(output_bdl_pairs[output].upper);
            const auto charge_state_output_lower = given_cds.get_charge_state(output_bdl_pairs[output].lower);

            // if the output charge states are equal, the layout is not operational
            if (charge_state_output_lower == charge_state_output_upper)
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
            }

            // if the expected output is 1, the expected charge states are (upper, lower) = (0, -1)
            if (kitty::get_bit(truth_table[output], input_pattern))
            {
                if (!encodes_bit_one(given_cds, output_bdl_pairs[output], output_bdl_wires[output].port))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }
            }
            // if the expected output is 0, the expected charge states are (upper, lower) = (-1, 0)
            else
            {
                if (!encodes_bit_zero(given_cds, output_bdl_pairs[output], output_bdl_wires[output].port))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }
            }

            if (parameters.op_condition == operational_condition::REJECT_KINKS)
            {
                if (check_existence_of_kinks_in_input_wires(given_cds, input_pattern) ||
                    check_existence_of_kinks_in_output_wires(given_cds, input_pattern))
                {
                    non_operational_reason = non_operationality_reason::KINKS;
                }
            }
        }

        if (non_operational_reason == non_operationality_reason::KINKS)
        {
            return {operational_status::NON_OPERATIONAL, non_operationality_reason::KINKS};
        }

        // if we made it here, the layout is operational
        return {operational_status::OPERATIONAL, non_operationality_reason::NONE};
    }
    /**
     * Determines the input combinations for which the layout is non-operational and the reason why the layout is
     * non-operational.
     *
     * @return Vector of pairs where the first element of the pair is the input pattern (e.g. 2-input Boolean function:
     * 00 ^= 0; 10 ^= 2) for which the layout is non-operational. The second entry indicates the reason why the
     * layout is non-operational (`non_operationality_reason`) for the given input pattern.
     */
    [[nodiscard]] std::vector<std::pair<uint64_t, non_operationality_reason>>
    determine_non_operational_input_patterns_and_non_operationality_reason() noexcept
    {
        assert(!output_bdl_pairs.empty() && "No output cell provided.");
        assert((truth_table.size() == output_bdl_pairs.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        std::vector<std::pair<uint64_t, detail::non_operationality_reason>>
            non_operational_input_pattern_and_non_operationality_reason{};

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
                const auto [op_status, non_op_reason] = verify_logic_match_of_cds(gs, i);
                if (op_status == operational_status::NON_OPERATIONAL)
                {
                    non_operational_input_pattern_and_non_operationality_reason.emplace_back(i, non_op_reason);
                }
            }
        }

        // if we made it here, the layout is operational
        return non_operational_input_pattern_and_non_operationality_reason;
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
     * This function conducts physical simulation of the given layout (gate layout with certain input combination).
     * The simulation results are stored in the `sim_result` variable.
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
        return std::any_of(input_bdl_wires.crbegin(), input_bdl_wires.crend(),
                           [this, &ground_state, &current_input_index, i = 0u](const auto& wire) mutable
                           {
                               const auto current_bit_set = (current_input_index & (uint64_t{1ull} << i++)) != 0ull;
                               return std::any_of(wire.pairs.cbegin(), wire.pairs.cend(),
                                                  [this, &ground_state, &current_bit_set, &wire](const auto& bdl)
                                                  {
                                                      if (bdl.type == sidb_technology::INPUT)
                                                      {
                                                          return false;  // Skip processing for input type.
                                                      }

                                                      if (current_bit_set)
                                                      {
                                                          return !encodes_bit_one(ground_state, bdl, wire.port);
                                                      }

                                                      return !encodes_bit_zero(ground_state, bdl, wire.port);
                                                  });
                           });
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
 * @tparam TT Type of the truth table.
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
    // all elements in spec must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (input_bdl_wire.has_value() && output_bdl_wire.has_value())
    {
        detail::is_operational_impl<Lyt, TT> p{lyt, spec, params, input_bdl_wire.value(), output_bdl_wire.value()};

        const auto [status, _] = p.run();

        return {status, p.get_number_of_simulator_invocations()};
    }

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}
/**
 * This function determines the input combinations for which the layout is operational.
 *
 * @tparam Lyt SiDB cell-level layout type.
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
    // all elements in spec must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    std::set<uint64_t> input_patterns{};

    // all possible input patterns
    for (auto i = 0u; i < spec.front().num_bits(); ++i)
    {
        input_patterns.insert(i);
    }

    const auto non_op_patterns_and_non_op_reason =
        p.determine_non_operational_input_patterns_and_non_operationality_reason();

    for (const auto& [input_pattern, _] : non_op_patterns_and_non_op_reason)
    {
        input_patterns.erase(input_pattern);
    }

    return input_patterns;
}
/**
 * This function determines all input combinations for which kinks induce the SiDB layout to become non-operational.
 * This means that the layout is operational if kinks would be accepted.
 *
 * @note "Kink induced non-operational" refers to the non-operational status being exclusively caused by kinks with an
 * otherwise correct logic match.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB layout.
 * @param spec Vector of truth table specifications.
 * @param params Parameters for the `is_operational` algorithm.
 * @return The input combinations where kinks induce the SiDB layout to become non-operational.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::set<uint64_t>
kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec,
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

    std::set<uint64_t> kink_induced_non_op_patterns{};

    const auto input_patterns_and_non_op_reason =
        p.determine_non_operational_input_patterns_and_non_operationality_reason();

    for (const auto& [input_pattern, status] : input_patterns_and_non_op_reason)
    {
        if (status == detail::non_operationality_reason::KINKS)
        {
            kink_induced_non_op_patterns.insert(input_pattern);
        }
    }

    return kink_induced_non_op_patterns;
}
/**
 * This function determines if the layout is only considered as non-operational because of kinks. This means that
 * the layout would be considered as operational, if kinks were accepted.
 *
 * @note "Kink induced non-operational" refers to the non-operational status being exclusively caused by kinks with an
 * otherwise correct logic match.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @return Bool that indicates whether kinks induce the layout to become non-operational. `true` if the layout is
 * non-operational due to kinks, `false` otherwise.
 */
template <typename Lyt, typename TT>
[[nodiscard]] bool is_kink_induced_non_operational(
    const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {},
    const std::optional<std::vector<bdl_wire<Lyt>>>& input_bdl_wire  = std::nullopt,
    const std::optional<std::vector<bdl_wire<Lyt>>>& output_bdl_wire = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = operational_condition::REJECT_KINKS;

    if (input_bdl_wire.has_value() && output_bdl_wire.has_value())
    {
        detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks, input_bdl_wire.value(),
                                               output_bdl_wire.value()};

        const auto [op_status, non_op_reason] = p.run();

        return op_status == operational_status::NON_OPERATIONAL &&
               non_op_reason == detail::non_operationality_reason::KINKS;
    }

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks};

    const auto [op_status, non_op_reason] = p.run();

    return op_status == operational_status::NON_OPERATIONAL &&
           non_op_reason == detail::non_operationality_reason::KINKS;
}
/**
 * This function calculates the count of input combinations for which the SiDB-based logic, represented by the
 * provided layout (`lyt`) and truth table specifications (`spec`), produces the correct output.
 *
 * @tparam Lyt Type of the cell-level layout.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB layout.
 * @param spec Vector of truth table specifications.
 * @param params Parameters to simualte if a input combination is operational.
 * @return The count of operational input combinations.
 *
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::size_t number_of_operational_input_combinations(const Lyt& lyt, const std::vector<TT>& spec,
                                                                   const is_operational_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    return p.count_number_of_non_operational_input_combinations();
}

}  // namespace fiction

#endif  // FICTION_IS_OPERATIONAL_HPP
