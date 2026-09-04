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
 * @brief Decides whether an SiDB layout implements its Boolean function.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/synthesis/truth_tables.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/engines/clustercomplete.hpp"
#include "fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/engines/quicksim.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/technology/sidb/surfaces/charge_distribution_surface.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <fmt/format.h>
#include <kitty/bit_operations.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <ranges>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::logic
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
 * Parameters for the `is_operational` algorithm.
 */
struct is_operational_params
{
    /**
     * Condition to decide whether a layout is operational or non-operational.
     */
    enum class operational_condition : uint8_t
    {
        /**
         * Even if the I/O pins show kinks, the layout is still considered as operational.
         */
        TOLERATE_KINKS,
        /**
         * The I/O pins are not allowed to show kinks. If kinks exist, the layout is considered non-operational.
         */
        REJECT_KINKS
    };

    /**
     * Simulation method to determine if the layout is operational or non-operational. There are three possible
     * strategies:
     *
     * - `SIMULATION_ONLY`: This setting does not apply any filtering strategies to determine if the layout is
     * operational. Instead, it relies solely on physical simulation to make this determination.
     * - `FILTER_ONLY`: This setting does only apply filtering strategies to determine if the layout is
     * non-operational. If the layout passes all filtering strategies, it is considered operational. This is only an
     * approximation. It may be possible that the layout is non-operational, but the filtering strategies do not detect
     * it. Sweeping a parameter space this way is called the operational domain sketch. The filtering steps are only
     * defined when kinks are rejected, and they enumerate the charge configurations of the canvas that the layout's
     * `LOGIC` cells define, so this setting is only effective with `REJECT_KINKS` on a layout that has such cells.
     * - `FILTER_THEN_SIMULATION`: Before a physical simulation is conducted, the algorithm checks if filtering
     * strategies have detected whether the layout is non-operational. This only provides any runtime benefits if kinks
     * are rejected.
     */
    enum class operational_analysis_strategy : uint8_t
    {
        /**
         * Do not apply filter strategies to determine whether the layout is operational.
         * Instead, rely solely on physical simulation.
         */
        SIMULATION_ONLY,
        /**
         * Apply filtering exclusively to determine whether the layout is non-operational. If the layout
         * passes all filter steps, it is considered operational. Sweeping a parameter space this way is called the
         * operational domain sketch.
         *
         * @note This is an extremely fast approximation that may sometimes lead to false positives. It requires
         * `REJECT_KINKS` and a layout with `LOGIC` cells; without either, no filter step runs.
         */
        FILTER_ONLY,
        /**
         * Before a physical simulation is conducted, the algorithm checks if filter strategies can determine that the
         * layout is non-operational. This only provides any runtime benefits if kinks are rejected.
         */
        FILTER_THEN_SIMULATION
    };
    /**
     * The simulation parameters for the physical simulation of the ground state.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb::simulation::engine sim_engine{engine::QUICKEXACT};
    /**
     * Parameters for the BDL input iterator.
     */
    bdl_input_iterator_params input_bdl_iterator_params{};
    /**
     * Condition to decide whether a layout is operational or non-operational.
     */
    operational_condition op_condition = operational_condition::TOLERATE_KINKS;
    /**
     * Strategy to determine whether a layout is operational or non-operational.
     */
    operational_analysis_strategy strategy_to_analyze_operational_status =
        operational_analysis_strategy::SIMULATION_ONLY;
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
     * Positive charges may occur but the simulation base is set to `2`.
     */
    POTENTIAL_POSITIVE_CHARGES,
    /**
     * No reason for non-operationality could be determined.
     */
    NONE,
};
/**
 * Reason why the layout is not a valid gate implementation for the given Boolean function.
 */
enum class layout_invalidity_reason : uint8_t
{
    /**
     * Positive SiDBs can potentially occur.
     */
    POTENTIAL_POSITIVE_CHARGES,
    /**
     * The layout is physically infeasible, meaning no charge distribution of the canvas SiDBs satisfies the criteria
     * for physical validity.
     */
    PHYSICAL_INFEASIBILITY,
    /**
     * I/O signals are unstable, indicating that an information flip results in a lower energy state.
     */
    IO_INSTABILITY,
};

/**
 * Implementation of the `is_operational` algorithm for a given SiDB layout.
 *
 * This class provides an implementation of the `is_operational` algorithm for
 * a specified SiDB layout and parameters. It checks whether the SiDB layout is operational
 * by simulating its behavior for different input combinations and comparing the results
 * to expected outputs from a truth table.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 */
template <typename Lyt, typename TT>
class is_operational_impl
{
  public:
    /**
     * Constructor to initialize the algorithm with a layout and parameters.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param tt Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params) :
            layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(lyt, sidb::sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii(bdl_input_iterator<Lyt>{lyt, params.input_bdl_iterator_params}),
            input_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT)},
            output_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT)},
            number_of_output_wires{output_bdl_wires.size()},
            number_of_input_wires{input_bdl_wires.size()}
    {}
    /**
     * Constructor to initialize the algorithm with a layout, parameters, input and output wires.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param tt Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     * @param input_wires BDL input wires of lyt.
     * @param output_wires BDL output wires of lyt.
     * @param initialize_bii If `true`, the BDL input iterator is initialized, `false` otherwise. This parameter is only
     * needed in special cases (verify_logic_match.hpp).
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& tt, const is_operational_params& params,
                        const std::vector<bdl_wire<Lyt>>& input_wires, const std::vector<bdl_wire<Lyt>>& output_wires,
                        const bool initialize_bii = true) :
            layout{lyt},
            truth_table{tt},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(layout, sidb::sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii{initialize_bii ? bdl_input_iterator<Lyt>{layout, params.input_bdl_iterator_params, input_wires} :
                                 bdl_input_iterator<Lyt>{Lyt{}}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires},
            number_of_output_wires{output_bdl_wires.size()},
            number_of_input_wires{input_bdl_wires.size()}
    {}

    /**
     * Constructor to initialize the algorithm with a layout, parameters, input and output wires, and a canvas layout.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     * @param input_wires BDL input wires of lyt.
     * @param output_wires BDL output wires of lyt.
     * @param c_lyt Canvas layout.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                        const std::vector<bdl_wire<Lyt>>& input_wires, const std::vector<bdl_wire<Lyt>>& output_wires,
                        const Lyt& c_lyt) :
            layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(lyt, sidb::sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii{bdl_input_iterator<Lyt>{layout, params.input_bdl_iterator_params, input_wires}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires},
            number_of_output_wires{output_bdl_wires.size()},
            number_of_input_wires{input_bdl_wires.size()},
            canvas_lyt{c_lyt}
    {}

    /**
     * Constructor to initialize the algorithm with a layout and parameters.
     *
     * @param lyt The SiDB cell-level layout to be checked.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     */
    is_operational_impl(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                        const Lyt& c_lyt) :
            layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(lyt, sidb::sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            bii(bdl_input_iterator<Lyt>{lyt, params.input_bdl_iterator_params}),
            input_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT)},
            output_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT)},
            number_of_output_wires{output_bdl_wires.size()},
            number_of_input_wires{input_bdl_wires.size()},
            canvas_lyt{c_lyt}
    {}

    /**
     * Constructor to initialize the algorithm with pre-generated input pattern layouts.
     *
     * The layouts are not copied and must outlive this object. They are only read, so the same layouts may be shared
     * by concurrently running instances.
     *
     * @param input_pattern_lyts One layout per input pattern, indexed by input pattern, as generated by
     * `generate_bdl_input_pattern_layouts`.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param params Parameters for the `is_operational` algorithm.
     * @param input_wires BDL input wires of the layout.
     * @param output_wires BDL output wires of the layout.
     * @param c_lyt Canvas layout.
     */
    is_operational_impl(const std::vector<Lyt>& input_pattern_lyts, const std::vector<TT>& spec,
                        const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_wires,
                        const std::vector<bdl_wire<Lyt>>& output_wires, const Lyt& c_lyt) :
            truth_table{spec},
            parameters{params},
            output_bdl_pairs(detect_bdl_pairs(input_pattern_lyts.front(), sidb::sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)),
            // the input pattern layouts make the iterator redundant; this is the same no-op instantiation that
            // `verify_logic_match` uses
            bii{bdl_input_iterator<Lyt>{Lyt{}}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires},
            number_of_output_wires{output_bdl_wires.size()},
            number_of_input_wires{input_bdl_wires.size()},
            canvas_lyt{c_lyt},
            input_pattern_layouts{&input_pattern_lyts}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    /**
     * This function evaluates whether the given layout is invalid, i.e., it cannot implement the given Boolean
     * function. This is done in three separate filtering steps: (1) discarding SiDB layouts with
     * potentially positively charged SiDBs, (2) utilizing an efficient method to identify and discard SiDB layouts that
     * do not satisfy physical model constraints under the I/O pin conditions required for the desired Boolean function,
     * and (3) detecting I/O signal instability.
     *
     * @param input_pattern The current input pattern.
     * @return A `layout_invalidity_reason` object indicating why the layout is non-operational; or `std::nullopt` if it
     * could not certainly be determined to be in fact non-operational.
     */
    [[nodiscard]] std::optional<layout_invalidity_reason> is_layout_invalid(const uint64_t input_pattern) noexcept
    {
        const auto& lyt_with_input_pattern = layout_with_input_pattern(input_pattern);

        sidb::surfaces::charge_distribution_surface<Lyt> cds_layout{lyt_with_input_pattern};
        cds_layout.assign_all_charge_states(sidb::model::charge_state::NEGATIVE);
        cds_layout.assign_physical_parameters(parameters.sim_params);

        if ((parameters.sim_params.base == 2) &&
            (analysis::can_positive_charges_occur(lyt_with_input_pattern, parameters.sim_params)))
        {
            return layout_invalidity_reason::POTENTIAL_POSITIVE_CHARGES;
        }

        const auto input_index = input_pattern;

        set_charge_distribution_of_input_pins(cds_layout, input_index);
        set_charge_distribution_of_output_pins(cds_layout, synthesis::evaluate_output(truth_table, input_index));

        if (const auto physical_validity = is_physical_validity_feasible(cds_layout); physical_validity.has_value())
        {
            if (const auto output_index = synthesis::evaluate_output(truth_table, input_index); is_io_signal_unstable(
                    cds_layout, truth_table.front().num_bits(), input_index, output_index, physical_validity.value()))
            {
                return layout_invalidity_reason::IO_INSTABILITY;
            };

            return std::nullopt;
        }

        return layout_invalidity_reason::PHYSICAL_INFEASIBILITY;
    }

    /**
     * Run the `is_operational` algorithm.
     *
     * This function executes the operational status checking algorithm for the given SiDB layout
     * and parameters provided during initialization.
     *
     * @return Pair with the first element indicating the operational status (either `OPERATIONAL` or `NON_OPERATIONAL`)
     * and the second element indicating the reason if it is non-operational.
     */
    [[nodiscard]] std::pair<operational_status, non_operationality_reason> run() noexcept
    {
        if (canvas_filtering_applicable)
        {
            // number of different input combinations
            for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
            {
                if (is_layout_invalid(i))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }
            }
        }
        // if the layout is not discarded during the three filtering steps, it is considered operational.
        // This is only an approximation.
        if (parameters.strategy_to_analyze_operational_status ==
                is_operational_params::operational_analysis_strategy::FILTER_ONLY &&
            canvas_filtering_applicable)
        {
            return {operational_status::OPERATIONAL, non_operationality_reason::NONE};
        }

        if (parameters.strategy_to_analyze_operational_status ==
                is_operational_params::operational_analysis_strategy::SIMULATION_ONLY ||
            parameters.strategy_to_analyze_operational_status ==
                is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION ||
            !canvas_filtering_applicable)
        {
            // number of different input combinations
            for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
            {
                const auto& lyt_with_input_pattern = layout_with_input_pattern(i);

                // if positively charged SiDBs can occur, the SiDB layout is considered non-operational
                if ((parameters.sim_params.base == 2) &&
                    (analysis::can_positive_charges_occur(lyt_with_input_pattern, parameters.sim_params)))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::POTENTIAL_POSITIVE_CHARGES};
                }

                ++simulator_invocations;
                // performs physical simulation of a given SiDB layout at a given input combination
                const auto simulation_results = physical_simulation_of_layout(lyt_with_input_pattern);

                // if no physically valid charge distributions were found, the layout is non-operational
                if (simulation_results.charge_distributions.empty())
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }

                const auto ground_states = simulation_results.groundstates();

                for (const auto& gs : ground_states)
                {
                    const auto [op_status, non_op_reason] = verify_logic_match_of_cds(gs, i);
                    if (op_status == operational_status::NON_OPERATIONAL &&
                        non_op_reason == non_operationality_reason::LOGIC_MISMATCH)
                    {
                        return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                    }
                    if (op_status == operational_status::NON_OPERATIONAL &&
                        non_op_reason == non_operationality_reason::KINKS &&
                        parameters.op_condition == is_operational_params::operational_condition::REJECT_KINKS)
                    {
                        return {operational_status::NON_OPERATIONAL, non_operationality_reason::KINKS};
                    }
                }
            }
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
    verify_logic_match_of_cds(const sidb::surfaces::charge_distribution_surface<Lyt>& given_cds,
                              const uint64_t                                          input_pattern) noexcept
    {
        auto non_operational_reason = non_operationality_reason::LOGIC_MISMATCH;

        // if positively charged SiDBs can occur, the SiDB layout is considered non-operational
        if (parameters.sim_params.base == 2 && analysis::can_positive_charges_occur(given_cds, parameters.sim_params))
        {
            return {operational_status::NON_OPERATIONAL, non_operationality_reason::POTENTIAL_POSITIVE_CHARGES};
        }

        assert(!output_bdl_pairs.empty() && "No output cell provided.");

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
        }

        if (parameters.op_condition == is_operational_params::operational_condition::REJECT_KINKS)
        {
            assert(!input_bdl_wires.empty() && "No input wires provided.");
            assert(!output_bdl_wires.empty() && "No output wires provided.");
            assert((truth_table.size() == output_bdl_wires.size()) &&
                   "Number of truth tables and output BDL wires don't not match");

            if (check_existence_of_kinks_in_input_wires(given_cds, input_pattern) ||
                check_existence_of_kinks_in_output_wires(given_cds, input_pattern))
            {
                non_operational_reason = non_operationality_reason::KINKS;
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
        assert((truth_table.size() == output_bdl_wires.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        std::vector<std::pair<uint64_t, detail::non_operationality_reason>>
            non_operational_input_pattern_and_non_operationality_reason{};

        // number of different input combinations
        for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
        {
            ++simulator_invocations;

            const auto& lyt_with_input_pattern = layout_with_input_pattern(i);

            // if positively charged SiDBs can occur, the SiDB layout is considered non-operational
            if ((parameters.sim_params.base == 2) &&
                (analysis::can_positive_charges_occur(lyt_with_input_pattern, parameters.sim_params)))
            {
                non_operational_input_pattern_and_non_operationality_reason.emplace_back(
                    i, non_operationality_reason::POTENTIAL_POSITIVE_CHARGES);
                continue;
            }

            // performs physical simulation of a given SiDB layout at a given input combination
            const auto simulation_results = physical_simulation_of_layout(lyt_with_input_pattern);

            // if no physically valid charge distributions were found, the layout is non-operational
            if (simulation_results.charge_distributions.empty())
            {
                continue;
            }

            const auto ground_states = simulation_results.groundstates();

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

    /**
     * This function determines if there is a charge distribution of the canvas SiDBs for which the charge distribution
     * of the whole layout is physically valid.
     *
     * @param cds_layout The charge distribution surface layout to be evaluated.
     * @return The minimum energy value if a physically valid configuration is found, `std::nullopt`
     * otherwise.
     */
    [[nodiscard]] std::optional<double>
    is_physical_validity_feasible(sidb::surfaces::charge_distribution_surface<Lyt>& cds_layout) noexcept
    {
        assert(!canvas_lyt.is_empty() && "The canvas layout must not be empty.");

        auto min_energy = std::numeric_limits<double>::infinity();

        uint64_t canvas_charge_index = 0;

        auto& cds_canvas = canvas_charge_distribution();
        cds_canvas.assign_charge_index(canvas_charge_index);
        cds_layout.assign_dependent_cell(cds_canvas.get_sidb_order().front());

        const auto max_index = cds_canvas.get_max_charge_index();

        assert(max_index == static_cast<uint64_t>(std::pow(2, cds_canvas.num_cells() - 1) - 1) &&
               "The maximum charge index is incorrect. Probably, the dependent cell is not set.");

        while (canvas_charge_index <= max_index)
        {
            cds_canvas.foreach_cell(
                [&cds_layout, &cds_canvas](const auto& c)
                {
                    cds_layout.assign_charge_state(c, cds_canvas.get_charge_state(c),
                                                   sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                });
            cds_layout.update_after_charge_change(sidb::surfaces::dependent_cell_mode::VARIABLE,
                                                  sidb::surfaces::energy_calculation::KEEP_OLD_ENERGY_VALUE);

            if (cds_layout.is_physically_valid())
            {
                cds_layout.recompute_electrostatic_potential_energy();
                if (cds_layout.get_electrostatic_potential_energy() + fiction::utils::math::ERROR_MARGIN < min_energy)
                {
                    min_energy = cds_layout.get_electrostatic_potential_energy();
                }
            }

            if (canvas_charge_index == max_index)
            {
                break;
            }

            canvas_charge_index++;
            cds_canvas.assign_charge_index(canvas_charge_index,
                                           sidb::surfaces::charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION);
        }

        if (std::isinf(min_energy))
        {
            return std::nullopt;
        }

        return min_energy;
    }

    /**
     * This function assigns the charge states of the input pins in the layout according to the input index provided.
     * This means that when a zero is applied, each BDL pair in the wire is set to zero.
     *
     * @param cds The charge distribution surface layout to be modified.
     * @param current_input_index The index representing the current input pattern.
     */
    void set_charge_distribution_of_input_pins(sidb::surfaces::charge_distribution_surface<Lyt>& cds,
                                               const uint64_t current_input_index) const noexcept
    {
        cds.assign_all_charge_states(sidb::model::charge_state::NEGATIVE,
                                     sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);

        for (auto i = 0u; i < number_of_input_wires; i++)
        {
            if (input_bdl_wires[number_of_input_wires - 1 - i].port.dir == fcn::port_direction::SOUTH ||
                input_bdl_wires[number_of_input_wires - 1 - i].port.dir == fcn::port_direction::EAST)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
        }
    }

    /**
     * This function assigns the charge states of the output pins in the layout according to the input index provided.
     * This means that when a zero is applied, each BDL pair in the wire is set to zero.
     *
     * @param cds The charge distribution surface layout to be modified.
     * @param output_wire_index The index representing the current input pattern of the output wire.
     */
    void set_charge_distribution_of_output_pins(sidb::surfaces::charge_distribution_surface<Lyt>& cds,
                                                const uint64_t output_wire_index) const noexcept
    {
        for (auto i = 0u; i < number_of_output_wires; i++)
        {
            if (output_bdl_wires[i].port.dir == fcn::port_direction::SOUTH ||
                output_bdl_wires[i].port.dir == fcn::port_direction::EAST)
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else if (output_bdl_wires[i].port.dir == fcn::port_direction::NONE)
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
                        {
                            continue;
                        }
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        cds.assign_charge_state(bdl.upper, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                        cds.assign_charge_state(bdl.lower, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
        }
    }
    /**
     * This function iterates through various input patterns and output wire indices to determine if any configuration
     * results in a physically valid layout with energy below the given energy value, indicating I/O signal instability.
     *
     * @param cds_layout The charge distribution surface layout to be modified and checked.
     * @param max_input_pattern_index The maximum index for input pattern
     * @param input_pattern The specific input pattern for which the stability check is conducted.
     * @param logical_correct_output_pattern The expected correct output pattern for the given input.
     * @param minimal_energy_of_physically_valid_layout The minimum energy threshold below which the layout is
     * considered unstable.
     * @return `true` if the I/O signal is unstable, `false` otherwise.
     */
    [[nodiscard]] bool is_io_signal_unstable(sidb::surfaces::charge_distribution_surface<Lyt>& cds_layout,
                                             const uint64_t max_input_pattern_index, const uint64_t input_pattern,
                                             const uint64_t logical_correct_output_pattern,
                                             const double   minimal_energy_of_physically_valid_layout) noexcept
    {
        // the number of output patterns is fixed for this layout, so it is determined once instead of on every
        // iteration of the inner loop
        const uint64_t max_output_pattern_index{uint64_t{1} << output_bdl_wires.size()};

        for (uint64_t kink_states_input = 0; kink_states_input < max_input_pattern_index; ++kink_states_input)
        {
            for (uint64_t output_wire_index = 0; output_wire_index < max_output_pattern_index; ++output_wire_index)
            {
                if (output_wire_index == logical_correct_output_pattern && kink_states_input == input_pattern)
                {
                    continue;
                }

                set_charge_distribution_of_input_pins(cds_layout, kink_states_input);
                set_charge_distribution_of_output_pins(cds_layout, output_wire_index);

                const auto physical_validity = is_physical_validity_feasible(cds_layout);

                if (physical_validity.has_value())
                {
                    if (physical_validity.value() + fiction::utils::math::ERROR_MARGIN <
                        minimal_energy_of_physically_valid_layout)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
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
    const is_operational_params& parameters;
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
     * Number of output BDL wires.
     */
    const std::size_t number_of_output_wires;
    /**
     * Number of input BDL wires.
     */
    const std::size_t number_of_input_wires;
    /**
     * Layout consisting of all canvas SiDBs.
     */
    Lyt canvas_lyt{};
    /**
     * Whether the canvas-based filtering steps can be applied. They need a canvas to enumerate, they are skipped by
     * `SIMULATION_ONLY`, and they are only defined for `REJECT_KINKS`.
     *
     * This is the single place the condition is decided. The entry points build a canvas whenever the layout has
     * `LOGIC` cells and leave it to `run()` to determine whether the filtering applies, so that the same layout and the
     * same parameters take the same path regardless of which overload the caller reached.
     */
    const bool canvas_filtering_applicable{!canvas_lyt.is_empty() &&
                                           parameters.strategy_to_analyze_operational_status !=
                                               is_operational_params::operational_analysis_strategy::SIMULATION_ONLY &&
                                           parameters.op_condition ==
                                               is_operational_params::operational_condition::REJECT_KINKS};
    /**
     * Pre-generated layouts, one per input pattern, or `nullptr` if the BDL input iterator is used instead. Not owned
     * by this object and only ever read.
     */
    const std::vector<Lyt>* input_pattern_layouts{nullptr};
    /**
     * The charge distribution surface of the canvas layout, enumerated by `is_physical_validity_feasible`. It is built
     * on first use and reused afterwards, since the canvas does not change over this object's lifetime. Empty until
     * then, so that the strategies that never inspect the canvas do not pay for it.
     */
    std::optional<sidb::surfaces::charge_distribution_surface<Lyt>> canvas_cds{};

    /**
     * Returns the charge distribution surface of the canvas layout, constructing it on first use.
     *
     * Constructing it means computing the potential matrix over the canvas SiDBs, which
     * `is_physical_validity_feasible` would otherwise repeat on each of its calls even though the canvas is fixed. The
     * caller is responsible for resetting the charge index; the base number and the dependent cell are set here and
     * stay valid.
     *
     * @return The canvas charge distribution surface.
     */
    [[nodiscard]] sidb::surfaces::charge_distribution_surface<Lyt>& canvas_charge_distribution() noexcept
    {
        if (!canvas_cds.has_value())
        {
            canvas_cds.emplace(canvas_lyt);
            canvas_cds->assign_base_number(2);
            canvas_cds->assign_dependent_cell(canvas_cds->get_sidb_order().front());
        }

        return *canvas_cds;
    }

    /**
     * Returns the layout with the given input pattern applied.
     *
     * Reads from the pre-generated input pattern layouts if they were supplied, and drives the BDL input iterator to
     * the requested pattern otherwise.
     *
     * @param input_pattern The input pattern to apply.
     * @return The layout with `input_pattern` applied.
     */
    [[nodiscard]] const Lyt& layout_with_input_pattern(const uint64_t input_pattern) noexcept
    {
        if (input_pattern_layouts != nullptr)
        {
            assert(input_pattern < input_pattern_layouts->size() && "input pattern out of range");

            return (*input_pattern_layouts)[input_pattern];
        }

        bii = input_pattern;

        return *bii;
    }

    /**
     * This function conducts physical simulation of the given SiDB layout.
     * The simulation results are stored in the `sim_result` variable.
     *
     * @param lyt_with_input_pattern The SiDB layout with a given input combination applied.
     * @return Simulation results.
     */
    [[nodiscard]] sidb::simulation::legacy_result<Lyt>
    physical_simulation_of_layout(const Lyt& lyt_with_input_pattern) noexcept
    {
        if (parameters.sim_engine == engine::EXGS)
        {
            // perform exhaustive ground state simulation
            return sidb::simulation::engines::exhaustive_ground_state_simulation(lyt_with_input_pattern,
                                                                                 parameters.sim_params);
        }
        if (parameters.sim_engine == engine::QUICKEXACT)
        {
            // perform QuickExact exact simulation
            const sidb::simulation::engines::quickexact_params qe_params{
                .sim_params = parameters.sim_params,
                .base_number_detection =
                    fiction::sidb::simulation::engines::quickexact_params::automatic_base_number_detection::OFF};
            return sidb::simulation::engines::quickexact(lyt_with_input_pattern, qe_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        if (parameters.sim_engine == engine::CLUSTERCOMPLETE)
        {
            // perform ClusterComplete exact simulation
            const sidb::simulation::engines::clustercomplete_params cc_params{.sim_params = parameters.sim_params};
            return sidb::simulation::engines::clustercomplete(lyt_with_input_pattern, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED
        if constexpr (!is_sidb_defect_surface_v<Lyt>)
        {
            if (parameters.sim_engine == engine::QUICKSIM)
            {
                assert(parameters.sim_params.base == 2 && "QuickSim does not support base-3 simulation");

                // perform QuickSim heuristic simulation
                const sidb::simulation::engines::quicksim_params qs_params{.sim_params      = parameters.sim_params,
                                                                           .iteration_steps = 500,
                                                                           .alpha           = 0.6};

                if (const auto qs_result = sidb::simulation::engines::quicksim(lyt_with_input_pattern, qs_params);
                    qs_result.has_value())
                {
                    return qs_result.value();
                }
                return sidb::simulation::legacy_result<Lyt>{};  // return empty result if no valid charge distribution
                                                                // was found
            }
        }

        assert(false && "unsupported simulation engine");

        return sidb::simulation::legacy_result<Lyt>{};
    }
    /**
     * This function iterates through the input wires and evaluates their charge states against the expected
     * states derived from the input pattern. A kink is considered to exist if an input wire's charge state does not
     * match the expected value (i.e., bit one or bit zero) for the given input index.
     *
     * @param ground_state The ground state charge distribution surface.
     * @param current_input_index The current input index used to retrieve the expected output from the truth table.
     * @return `true` if any input wire contains a kink (i.e., an unexpected charge state), `false` otherwise.
     */
    [[nodiscard]] bool
    check_existence_of_kinks_in_input_wires(const sidb::surfaces::charge_distribution_surface<Lyt>& ground_state,
                                            const uint64_t current_input_index) const noexcept
    {
        return std::ranges::any_of(
            input_bdl_wires | std::views::reverse,
            [this, &ground_state, &current_input_index, i = 0u](const auto& wire) mutable
            {
                const auto current_bit_set = (current_input_index & (uint64_t{1ull} << i++)) != 0ull;
                return std::ranges::any_of(wire.pairs,
                                           [this, &ground_state, &current_bit_set, &wire](const auto& bdl)
                                           {
                                               if (bdl.type == sidb::sidb_technology::cell_type::INPUT)
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
     * @param ground_state The ground state charge distribution surface.
     * @param current_input_index The current input index used to retrieve the expected output from the truth table.
     * @return `true` if any output wire contains a kink (i.e., an unexpected charge state), `false` otherwise.
     */
    [[nodiscard]] bool
    check_existence_of_kinks_in_output_wires(const sidb::surfaces::charge_distribution_surface<Lyt>& ground_state,
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
     * @param ground_state The ground state charge distribution surface.
     * @param bdl BDL pair to be evaluated.
     * @return `true` if `0` is encoded, `false` otherwise.
     */
    [[nodiscard]] bool encodes_bit_zero(const sidb::surfaces::charge_distribution_surface<Lyt>& ground_state,
                                        const bdl_pair<cell<Lyt>>& bdl, const fcn::port_direction port) const noexcept
    {
        if (port.dir == fcn::port_direction::SOUTH || port.dir == fcn::port_direction::EAST ||
            port.dir == fcn::port_direction::NONE)
        {
            return static_cast<bool>(
                (ground_state.get_charge_state(bdl.upper) == sidb::model::charge_state::NEGATIVE) &&
                (ground_state.get_charge_state(bdl.lower) == sidb::model::charge_state::NEUTRAL));
        }
        return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb::model::charge_state::NEUTRAL) &&
                                 (ground_state.get_charge_state(bdl.lower) == sidb::model::charge_state::NEGATIVE));
    }

    /**
     * This function returns `true` if `1` is encoded in the charge state of the given BDL pair. `false` otherwise.
     *
     * @param ground_state The ground state charge distribution surface.
     * @param bdl BDL pair to be evaluated.
     * @return `true` if `1` is encoded, `false` otherwise.
     */
    [[nodiscard]] bool encodes_bit_one(const sidb::surfaces::charge_distribution_surface<Lyt>& ground_state,
                                       const bdl_pair<cell<Lyt>>& bdl, const fcn::port_direction port) const noexcept
    {
        if (port.dir == fcn::port_direction::SOUTH || port.dir == fcn::port_direction::EAST ||
            port.dir == fcn::port_direction::NONE)
        {
            return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb::model::charge_state::NEUTRAL) &&
                                     (ground_state.get_charge_state(bdl.lower) == sidb::model::charge_state::NEGATIVE));
        }

        return static_cast<bool>((ground_state.get_charge_state(bdl.upper) == sidb::model::charge_state::NEGATIVE) &&
                                 (ground_state.get_charge_state(bdl.lower) == sidb::model::charge_state::NEUTRAL));
    }
};

}  // namespace detail

/**
 * Determine the operational status of an SiDB layout.
 *
 * This function checks the operational status of a given SiDB layout using the `is_operational` algorithm. It
 * determines whether the SiDB layout is operational and returns the correct result for all \f$2^n\f$ input
 * combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @return A pair containing the operational status of the SiDB layout (either `OPERATIONAL` or `NON_OPERATIONAL`) and
 * the number of input combinations tested.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    const auto logic_cells = lyt.get_cells_by_type(fiction::technology<Lyt>::cell_type::LOGIC);

    // if there are logic cells, we can design the canvas layout consisting of all logic cells. Whether the canvas is
    // actually used is decided by `is_operational_impl::run()`, so that every entry point takes the same path
    if (!logic_cells.empty())
    {
        Lyt canvas_lyt{};

        // assign all logic cells to the canvas layout
        for (const auto& c : logic_cells)
        {
            canvas_lyt.assign_cell_type(c, fiction::technology<Lyt>::cell_type::LOGIC);
        }

        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params, canvas_lyt};

        const auto [status, _] = p.run();

        return {status, p.get_number_of_simulator_invocations()};
    }

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}

/**
 * Determine the operational status of an SiDB layout.
 *
 * This function checks the operational status of a given SiDB layout using the `is_operational` algorithm. It
 * determines whether the SiDB layout is operational and returns the correct result for all \f$2^n\f$ input
 * combinations.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB cell-level layout to be checked.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @param canvas_lyt Optional canvas layout.
 * @return A pair containing the operational status of the SiDB layout (either `OPERATIONAL` or `NON_OPERATIONAL`) and
 * the number of input combinations tested.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params,
               const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire,
               const std::optional<Lyt>& canvas_lyt = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (canvas_lyt.has_value())
    {
        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            lyt, spec, params, input_bdl_wire, output_bdl_wire, canvas_lyt.value()};

        const auto [status, _] = p.run();

        return {status, p.get_number_of_simulator_invocations()};
    }

    const auto logic_cells = lyt.get_cells_by_type(fiction::technology<Lyt>::cell_type::LOGIC);

    if (!logic_cells.empty())
    {
        Lyt c_lyt{};

        for (const auto& c : logic_cells)
        {
            c_lyt.assign_cell_type(c, fiction::technology<Lyt>::cell_type::LOGIC);
        }

        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            lyt, spec, params, input_bdl_wire, output_bdl_wire, c_lyt};

        const auto [status, _] = p.run();

        return {status, p.get_number_of_simulator_invocations()};
    }

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params, input_bdl_wire,
                                                                             output_bdl_wire};

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}

/**
 * Determine the operational status of an SiDB layout from its pre-generated input pattern layouts.
 *
 * The layout is operational only if it produces the correct output for every input pattern, so this overload takes
 * one layout per input pattern and reports the layout operational only if all of them are. Since the input
 * configuration does not depend on the simulation parameters, a caller that evaluates the same layout under many
 * parameter settings can generate the layouts once with `generate_bdl_input_pattern_layouts` and pass them to every
 * call, instead of re-deriving them each time.
 *
 * The layouts are only read and may be shared by concurrent calls.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param input_pattern_layouts One layout per input pattern, indexed by input pattern, as generated by
 * `generate_bdl_input_pattern_layouts`. All of them must be operational for the layout to be operational.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the `is_operational` algorithm.
 * @param input_bdl_wire BDL input wires of the layout.
 * @param output_bdl_wire BDL output wires of the layout.
 * @param canvas_lyt Optional canvas layout.
 * @return A pair containing the operational status of the SiDB layout (either `OPERATIONAL` or `NON_OPERATIONAL`) and
 * the number of input combinations tested.
 * @throws std::invalid_argument if `spec` is empty, or if the number of input pattern layouts does not match the
 * number of input combinations of `spec`.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const std::vector<Lyt>& input_pattern_layouts, const std::vector<TT>& spec,
               const is_operational_params& params, const std::vector<bdl_wire<Lyt>>& input_bdl_wire,
               const std::vector<bdl_wire<Lyt>>& output_bdl_wire, const std::optional<Lyt>& canvas_lyt = std::nullopt)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // unlike the other overloads, this one indexes a caller-supplied container, so a wrong size is an out-of-bounds
    // read rather than a wrong answer. It is also reachable from `pyfiction` with an arbitrary list, so the checks
    // have to survive `NDEBUG`
    if (spec.empty())
    {
        throw std::invalid_argument("spec is empty");
    }

    if (input_pattern_layouts.size() != spec.front().num_bits())
    {
        throw std::invalid_argument(
            fmt::format("expected {} input pattern layouts for a {}-input specification, but got {}",
                        spec.front().num_bits(), spec.front().num_vars(), input_pattern_layouts.size()));
    }

    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (canvas_lyt.has_value())
    {
        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            input_pattern_layouts, spec, params, input_bdl_wire, output_bdl_wire, canvas_lyt.value()};

        const auto [status, _] = p.run();

        return {status, p.get_number_of_simulator_invocations()};
    }

    Lyt c_lyt{};

    for (const auto& c : input_pattern_layouts.front().get_cells_by_type(fiction::technology<Lyt>::cell_type::LOGIC))
    {
        c_lyt.assign_cell_type(c, fiction::technology<Lyt>::cell_type::LOGIC);
    }

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
        input_pattern_layouts, spec, params, input_bdl_wire, output_bdl_wire, c_lyt};

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
 * @return The operational input combinations.
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
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params};

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
 * This function determines the input combinations for which the layout is operational.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The SiDB layout.
 * @param spec Vector of truth table specifications.
 * @param params Parameters to simulate if a input combination is operational.
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @param canvas_lyt Optional canvas layout.
 * @return The count of operational input combinations.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::set<uint64_t>
operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                           const std::vector<bdl_wire<Lyt>>& input_bdl_wire,
                           const std::vector<bdl_wire<Lyt>>& output_bdl_wire,
                           const std::optional<Lyt>&         canvas_lyt = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (canvas_lyt.has_value())
    {
        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            lyt, spec, params, input_bdl_wire, output_bdl_wire, canvas_lyt.value()};

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

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params, input_bdl_wire,
                                                                             output_bdl_wire};

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
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;

    params_with_rejecting_kinks.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks};

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
 * @param input_bdl_wire Optional BDL input wires of lyt.
 * @param output_bdl_wire Optional BDL output wires of lyt.
 * @param canvas_lyt Optional canvas layout.
 * @return The input combinations where kinks induce the SiDB layout to become non-operational.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::set<uint64_t> kink_induced_non_operational_input_patterns(
    const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params,
    const std::vector<bdl_wire<Lyt>>& input_bdl_wire, const std::vector<bdl_wire<Lyt>>& output_bdl_wire,
    const std::optional<Lyt>& canvas_lyt = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;

    params_with_rejecting_kinks.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

    if (canvas_lyt.has_value())
    {
        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire, canvas_lyt.value()};

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

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks,
                                                                             input_bdl_wire, output_bdl_wire};

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
 * This function determines if the layout is only considered non-operational because of kinks. This means that
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
 * @return Bool that indicates whether kinks induce the layout to become non-operational. `true` if the layout is
 * non-operational due to kinks, `false` otherwise.
 */
template <typename Lyt, typename TT>
[[nodiscard]] bool is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec,
                                                   const is_operational_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks};

    const auto [op_status, non_op_reason] = p.run();

    return op_status == operational_status::NON_OPERATIONAL &&
           non_op_reason == detail::non_operationality_reason::KINKS;
}

/**
 * This function determines if the layout is only considered non-operational because of kinks. This means that
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
 * @param canvas_lyt Optional canvas layout.
 * @return Bool that indicates whether kinks induce the layout to become non-operational. `true` if the layout is
 * non-operational due to kinks, `false` otherwise.
 */
template <typename Lyt, typename TT>
[[nodiscard]] bool is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec,
                                                   const is_operational_params&      params,
                                                   const std::vector<bdl_wire<Lyt>>& input_bdl_wire,
                                                   const std::vector<bdl_wire<Lyt>>& output_bdl_wire,
                                                   const std::optional<Lyt>&         canvas_lyt = std::nullopt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");

    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    if (canvas_lyt.has_value())
    {
        fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{
            lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire, canvas_lyt.value()};

        const auto [op_status, non_op_reason] = p.run();

        return op_status == operational_status::NON_OPERATIONAL &&
               non_op_reason == detail::non_operationality_reason::KINKS;
    }

    fiction::sidb::simulation::logic::detail::is_operational_impl<Lyt, TT> p{lyt, spec, params_with_rejecting_kinks,
                                                                             input_bdl_wire, output_bdl_wire};

    const auto [op_status, non_op_reason] = p.run();

    return op_status == operational_status::NON_OPERATIONAL &&
           non_op_reason == detail::non_operationality_reason::KINKS;
}

}  // namespace fiction::sidb::simulation::logic
