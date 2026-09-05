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
#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/simulation/detail/simulation_state.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/engines/clustercomplete.hpp"
#include "fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/engines/quicksim.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
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
 * Reasons why a layout is not operational.
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
     * Positive charges may occur.
     */
    POTENTIAL_POSITIVE_CHARGES,
    /**
     * No reason for non-operationality could be determined.
     */
    NONE,
};

/**
 * Reasons why a layout is invalid before any simulation ran.
 */
enum class layout_invalidity_reason : uint8_t
{
    /**
     * Positive charges may occur.
     */
    POTENTIAL_POSITIVE_CHARGES,
    /**
     * No physically valid charge distribution exists for the expected input and output pins.
     */
    PHYSICAL_INFEASIBILITY,
    /**
     * A wrong I/O assignment is energetically preferred.
     */
    IO_INSTABILITY,
};

/**
 * Implementation of the operational check. It applies every input pattern to the input BDL pairs of the layout,
 * simulates the ground states, and compares the charge states of the output BDL pairs with the expected truth-table
 * entries. With a canvas, the three pruning filters (positive charges, physical infeasibility, I/O instability) run
 * before any simulation on the layout's potential landscape.
 *
 * @tparam TT Truth table type.
 */
template <typename TT>
class is_operational_impl
{
  public:
    /**
     * Detects the BDL pairs and wires of `lyt` itself.
     *
     * @param lyt The layout to check.
     * @param spec The Boolean function(s) to implement.
     * @param params Parameters.
     */
    is_operational_impl(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params) :
            sidb_layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs{detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            bii{lyt, params.input_bdl_iterator_params},
            input_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT)},
            output_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT)}
    {}
    /**
     * Takes the wires of `lyt` from the caller.
     *
     * @param lyt The layout to check.
     * @param spec The Boolean function(s) to implement.
     * @param params Parameters.
     * @param input_wires The input BDL wires of `lyt`.
     * @param output_wires The output BDL wires of `lyt`.
     * @param initialize_bii Whether to set up the input iterator. `verify_logic_match` passes `false` since it
     * checks a given charge distribution.
     */
    is_operational_impl(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                        const std::vector<bdl_wire>& input_wires, const std::vector<bdl_wire>& output_wires,
                        const bool initialize_bii = true) :
            sidb_layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs{detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            bii{initialize_bii ? bdl_input_iterator{lyt, params.input_bdl_iterator_params, input_wires} :
                                 bdl_input_iterator{layout{}}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires}
    {}
    /**
     * Takes the wires and the canvas of `lyt` from the caller.
     *
     * @param lyt The layout to check.
     * @param spec The Boolean function(s) to implement.
     * @param params Parameters.
     * @param input_wires The input BDL wires of `lyt`.
     * @param output_wires The output BDL wires of `lyt`.
     * @param c_lyt The canvas: the SiDBs of `lyt` whose charge states the pruning filters enumerate.
     */
    is_operational_impl(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                        const std::vector<bdl_wire>& input_wires, const std::vector<bdl_wire>& output_wires,
                        layout c_lyt) :
            sidb_layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs{detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            bii{lyt, params.input_bdl_iterator_params, input_wires},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires},
            canvas_lyt{std::move(c_lyt)}
    {}
    /**
     * Detects the wires of `lyt` and takes the canvas from the caller.
     *
     * @param lyt The layout to check.
     * @param spec The Boolean function(s) to implement.
     * @param params Parameters.
     * @param c_lyt The canvas.
     */
    is_operational_impl(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                        layout c_lyt) :
            sidb_layout{lyt},
            truth_table{spec},
            parameters{params},
            output_bdl_pairs{detect_bdl_pairs(lyt, sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            bii{lyt, params.input_bdl_iterator_params},
            input_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT)},
            output_bdl_wires{
                detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::OUTPUT)},
            canvas_lyt{std::move(c_lyt)}
    {}
    /**
     * Takes one layout per input pattern instead of applying the patterns itself.
     *
     * @param input_pattern_lyts One layout per input pattern, pattern `0` first.
     * @param spec The Boolean function(s) to implement.
     * @param params Parameters.
     * @param input_wires The input BDL wires.
     * @param output_wires The output BDL wires.
     * @param c_lyt The canvas.
     */
    is_operational_impl(const std::vector<layout>& input_pattern_lyts, const std::vector<TT>& spec,
                        const is_operational_params& params, const std::vector<bdl_wire>& input_wires,
                        const std::vector<bdl_wire>& output_wires, layout c_lyt) :
            truth_table{spec},
            parameters{params},
            output_bdl_pairs{detect_bdl_pairs(input_pattern_lyts.front(), sidb_technology::cell_type::OUTPUT,
                                              params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params)},
            // the input pattern layouts make the iterator redundant
            bii{layout{}},
            input_bdl_wires{input_wires},
            output_bdl_wires{output_wires},
            canvas_lyt{std::move(c_lyt)},
            input_pattern_layouts{&input_pattern_lyts}
    {}
    /**
     * Runs the pruning filters for one input pattern: positive charges, physical infeasibility of the expected I/O
     * charge states, and I/O instability.
     *
     * @param input_pattern The input pattern.
     * @return The reason the layout is invalid, or `std::nullopt` if the filters accept it.
     */
    [[nodiscard]] std::optional<layout_invalidity_reason> is_layout_invalid(const uint64_t input_pattern) noexcept
    {
        const auto& lyt_with_input_pattern = layout_with_input_pattern(input_pattern);

        const potential_landscape land{lyt_with_input_pattern, parameters.sim_params};

        if (parameters.sim_params.base == 2 && analysis::can_positive_charges_occur(land))
        {
            return layout_invalidity_reason::POTENTIAL_POSITIVE_CHARGES;
        }

        simulation::detail::simulation_state state{land, model::charge_state::NEGATIVE};

        set_charge_distribution_of_input_pins(state, input_pattern);
        set_charge_distribution_of_output_pins(state, synthesis::evaluate_output(truth_table, input_pattern));

        if (const auto physical_validity = is_physical_validity_feasible(state); physical_validity.has_value())
        {
            if (const auto output_index = synthesis::evaluate_output(truth_table, input_pattern); is_io_signal_unstable(
                    state, truth_table.front().num_bits(), input_pattern, output_index, *physical_validity))
            {
                return layout_invalidity_reason::IO_INSTABILITY;
            }

            return std::nullopt;
        }

        return layout_invalidity_reason::PHYSICAL_INFEASIBILITY;
    }
    /**
     * Runs the operational check.
     *
     * @return The status and, if non-operational, the reason.
     */
    [[nodiscard]] std::pair<operational_status, non_operationality_reason> run() noexcept
    {
        if (canvas_filtering_applicable)
        {
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
            for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
            {
                const auto& lyt_with_input_pattern = layout_with_input_pattern(i);

                // if positively charged SiDBs can occur, the SiDB layout is considered non-operational
                if (parameters.sim_params.base == 2 &&
                    analysis::can_positive_charges_occur(lyt_with_input_pattern, parameters.sim_params))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::POTENTIAL_POSITIVE_CHARGES};
                }

                ++simulator_invocations;

                const auto simulation_results = physical_simulation_of_layout(lyt_with_input_pattern);

                // if no physically valid charge distributions were found, the layout is non-operational
                if (simulation_results.charge_distributions.empty())
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }

                for (const auto& gs : simulation_results.groundstates())
                {
                    const auto [op_status, non_op_reason] = verify_logic_match_of_cd(gs, i);

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

        return {operational_status::OPERATIONAL, non_operationality_reason::NONE};
    }
    /**
     * Checks whether a charge distribution encodes the expected output for an input pattern, and, if kinks are
     * rejected, whether the wires are free of kinks. The positive-charge check of `run()` is not repeated here.
     *
     * @param cd The charge distribution to check.
     * @param input_pattern The input pattern it belongs to.
     * @return The status and, if non-operational, the reason.
     */
    [[nodiscard]] std::pair<operational_status, non_operationality_reason>
    verify_logic_match_of_cd(const charge_distribution& cd, const uint64_t input_pattern) const noexcept
    {
        assert(!output_bdl_pairs.empty() && "No output cell provided.");

        for (std::size_t output = 0; output < output_bdl_pairs.size(); ++output)
        {
            const auto upper = cd.get_charge_state(output_bdl_pairs[output].upper);
            const auto lower = cd.get_charge_state(output_bdl_pairs[output].lower);

            // if the output charge states are equal, the layout is not operational
            if (lower == upper)
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
            }

            if (kitty::get_bit(truth_table[output], input_pattern))
            {
                if (!encodes_bit_one(cd, output_bdl_pairs[output], output_bdl_wires[output].port))
                {
                    return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
                }
            }
            else if (!encodes_bit_zero(cd, output_bdl_pairs[output], output_bdl_wires[output].port))
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::LOGIC_MISMATCH};
            }
        }

        if (parameters.op_condition == is_operational_params::operational_condition::REJECT_KINKS)
        {
            assert(!input_bdl_wires.empty() && "No input wires provided.");
            assert(!output_bdl_wires.empty() && "No output wires provided.");
            assert((truth_table.size() == output_bdl_wires.size()) &&
                   "Number of truth tables and output BDL wires don't not match");

            if (check_existence_of_kinks_in_input_wires(cd, input_pattern) ||
                check_existence_of_kinks_in_output_wires(cd, input_pattern))
            {
                return {operational_status::NON_OPERATIONAL, non_operationality_reason::KINKS};
            }
        }

        return {operational_status::OPERATIONAL, non_operationality_reason::NONE};
    }
    /**
     * Simulates every input pattern and collects the non-operational ones with their reasons.
     *
     * @return The non-operational input patterns and the reason for each.
     */
    [[nodiscard]] std::vector<std::pair<uint64_t, non_operationality_reason>>
    determine_non_operational_input_patterns_and_non_operationality_reason() noexcept
    {
        assert((truth_table.size() == output_bdl_wires.size()) &&
               "Number of truth tables and output BDL pairs does not match");

        std::vector<std::pair<uint64_t, non_operationality_reason>> non_operational{};

        for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
        {
            ++simulator_invocations;

            const auto& lyt_with_input_pattern = layout_with_input_pattern(i);

            if (parameters.sim_params.base == 2 &&
                analysis::can_positive_charges_occur(lyt_with_input_pattern, parameters.sim_params))
            {
                non_operational.emplace_back(i, non_operationality_reason::POTENTIAL_POSITIVE_CHARGES);
                continue;
            }

            const auto simulation_results = physical_simulation_of_layout(lyt_with_input_pattern);

            if (simulation_results.charge_distributions.empty())
            {
                continue;
            }

            for (const auto& gs : simulation_results.groundstates())
            {
                const auto [op_status, non_op_reason] = verify_logic_match_of_cd(gs, i);

                if (op_status == operational_status::NON_OPERATIONAL)
                {
                    non_operational.emplace_back(i, non_op_reason);
                }
            }
        }

        return non_operational;
    }
    /**
     * Number of simulator invocations so far.
     *
     * @return The count.
     */
    [[nodiscard]] std::size_t get_number_of_simulator_invocations() const noexcept
    {
        return simulator_invocations;
    }
    /**
     * Enumerates the charge states of the canvas SiDBs, with the remaining SiDBs' charges fixed as set in `state`,
     * and returns the lowest energy of a physically valid configuration.
     *
     * @param state Simulation state over the layout with the current input pattern; the canvas charges are varied.
     * @return The minimum energy, or `std::nullopt` if no configuration is physically valid.
     */
    [[nodiscard]] std::optional<double>
    is_physical_validity_feasible(simulation::detail::simulation_state& state) noexcept
    {
        assert(!canvas_lyt.is_empty() && "The canvas layout must not be empty.");

        const auto& lyt = state.landscape().get_layout();

        // the canvas SiDBs by their index in the layout; the first one is the dependent SiDB
        std::vector<std::size_t> canvas{};
        canvas.reserve(canvas_lyt.num_cells());

        for (const auto& site : canvas_lyt.sidbs())
        {
            const auto index = lyt.index_of(site);
            assert(index.has_value() && "canvas SiDB is not part of the layout");
            canvas.push_back(*index);
        }

        state.assign_dependent_cell(canvas.front());

        auto min_energy = std::numeric_limits<double>::infinity();

        const auto num_free  = canvas.size() - 1;
        const auto max_index = (uint64_t{1} << num_free) - 1;

        for (uint64_t canvas_index = 0;; ++canvas_index)
        {
            for (std::size_t j = 0; j < num_free; ++j)
            {
                state.assign_charge_state_by_index(canvas[j + 1],
                                                   ((canvas_index >> j) & uint64_t{1}) == 0 ?
                                                       model::charge_state::NEGATIVE :
                                                       model::charge_state::NEUTRAL,
                                                   simulation::detail::charge_index_mode::KEEP_CHARGE_INDEX);
            }

            state.update_after_charge_change(simulation::detail::dependent_cell_mode::VARIABLE,
                                             simulation::detail::energy_calculation::KEEP_OLD_ENERGY_VALUE);

            if (state.is_physically_valid())
            {
                state.recompute_energy();

                if (state.energy() + utils::math::ERROR_MARGIN < min_energy)
                {
                    min_energy = state.energy();
                }
            }

            if (canvas_index == max_index)
            {
                break;
            }
        }

        if (std::isinf(min_energy))
        {
            return std::nullopt;
        }

        return min_energy;
    }
    /**
     * Sets the charge states of the input wires to encode an input pattern (the input pairs themselves are set by
     * the layout); every other SiDB becomes negative.
     *
     * @param state The state to modify.
     * @param current_input_index The input pattern.
     */
    void set_charge_distribution_of_input_pins(simulation::detail::simulation_state& state,
                                               const uint64_t                        current_input_index) const noexcept
    {
        state.assign_all_charge_states(model::charge_state::NEGATIVE,
                                       simulation::detail::charge_index_mode::KEEP_CHARGE_INDEX);

        const auto& lyt = state.landscape().get_layout();

        const auto number_of_input_wires = input_bdl_wires.size();

        for (std::size_t i = 0; i < number_of_input_wires; ++i)
        {
            const auto& wire = input_bdl_wires[number_of_input_wires - 1 - i];
            const bool  forward =
                wire.port.dir == fcn::port_direction::SOUTH || wire.port.dir == fcn::port_direction::EAST;
            const bool bit_set = (current_input_index & (uint64_t{1} << i)) != 0;

            // a set bit on a forward wire puts the neutral SiDB on top; a cleared bit or a backward wire flips that
            const bool upper_neutral = forward == bit_set;

            for (const auto& bdl : wire.pairs)
            {
                if (bdl.type == sidb_technology::cell_type::INPUT)
                {
                    continue;
                }

                assign(state, lyt, bdl.upper,
                       upper_neutral ? model::charge_state::NEUTRAL : model::charge_state::NEGATIVE);
                assign(state, lyt, bdl.lower,
                       upper_neutral ? model::charge_state::NEGATIVE : model::charge_state::NEUTRAL);
            }
        }
    }
    /**
     * Sets the charge states of the output wires to encode an output pattern.
     *
     * @param state The state to modify.
     * @param output_wire_index The output pattern.
     */
    void set_charge_distribution_of_output_pins(simulation::detail::simulation_state& state,
                                                const uint64_t                        output_wire_index) const noexcept
    {
        const auto& lyt = state.landscape().get_layout();

        for (std::size_t i = 0; i < output_bdl_wires.size(); ++i)
        {
            const auto& wire    = output_bdl_wires[i];
            const bool  forward = wire.port.dir == fcn::port_direction::SOUTH ||
                                  wire.port.dir == fcn::port_direction::EAST ||
                                  wire.port.dir == fcn::port_direction::NONE;
            const bool  bit_set = (output_wire_index & (uint64_t{1} << i)) != 0;

            const bool upper_neutral = forward == bit_set;

            for (const auto& bdl : wire.pairs)
            {
                // input pairs on an output wire keep the charge the input pattern gave them
                if (bit_set && wire.port.dir != fcn::port_direction::SOUTH &&
                    wire.port.dir != fcn::port_direction::EAST && bdl.type == sidb_technology::cell_type::INPUT)
                {
                    continue;
                }

                assign(state, lyt, bdl.upper,
                       upper_neutral ? model::charge_state::NEUTRAL : model::charge_state::NEGATIVE);
                assign(state, lyt, bdl.lower,
                       upper_neutral ? model::charge_state::NEGATIVE : model::charge_state::NEUTRAL);
            }
        }
    }
    /**
     * Checks whether any wrong I/O assignment has a physically valid configuration with lower energy than the
     * expected one.
     *
     * @param state Simulation state over the layout with the current input pattern.
     * @param max_input_pattern_index Number of input patterns.
     * @param input_pattern The current input pattern.
     * @param logical_correct_output_pattern The expected output pattern.
     * @param minimal_energy_of_physically_valid_layout The minimum energy of the expected I/O assignment.
     * @return `true` if a wrong assignment is energetically preferred.
     */
    [[nodiscard]] bool is_io_signal_unstable(simulation::detail::simulation_state& state,
                                             const uint64_t max_input_pattern_index, const uint64_t input_pattern,
                                             const uint64_t logical_correct_output_pattern,
                                             const double   minimal_energy_of_physically_valid_layout) noexcept
    {
        const uint64_t max_output_pattern_index{uint64_t{1} << output_bdl_wires.size()};

        for (uint64_t kink_states_input = 0; kink_states_input < max_input_pattern_index; ++kink_states_input)
        {
            for (uint64_t output_wire_index = 0; output_wire_index < max_output_pattern_index; ++output_wire_index)
            {
                if (output_wire_index == logical_correct_output_pattern && kink_states_input == input_pattern)
                {
                    continue;
                }

                set_charge_distribution_of_input_pins(state, kink_states_input);
                set_charge_distribution_of_output_pins(state, output_wire_index);

                if (const auto physical_validity = is_physical_validity_feasible(state);
                    physical_validity.has_value() &&
                    *physical_validity + utils::math::ERROR_MARGIN < minimal_energy_of_physically_valid_layout)
                {
                    return true;
                }
            }
        }

        return false;
    }

  private:
    /**
     * The layout to check.
     */
    const layout sidb_layout{};
    /**
     * The Boolean function(s) to implement.
     */
    const std::vector<TT>& truth_table;
    /**
     * Parameters.
     */
    const is_operational_params& parameters;
    /**
     * The output BDL pairs.
     */
    std::vector<bdl_pair<lattice_site>> output_bdl_pairs;
    /**
     * Iterator over the input patterns.
     */
    bdl_input_iterator bii;
    /**
     * The input BDL wires.
     */
    std::vector<bdl_wire> input_bdl_wires;
    /**
     * The output BDL wires.
     */
    std::vector<bdl_wire> output_bdl_wires;
    /**
     * Number of simulator invocations.
     */
    std::size_t simulator_invocations{0};
    /**
     * The canvas.
     */
    layout canvas_lyt{};
    /**
     * Whether the pruning filters apply: a canvas is given, the strategy asks for filtering, and kinks are rejected.
     */
    const bool canvas_filtering_applicable{!canvas_lyt.is_empty() &&
                                           parameters.strategy_to_analyze_operational_status !=
                                               is_operational_params::operational_analysis_strategy::SIMULATION_ONLY &&
                                           parameters.op_condition ==
                                               is_operational_params::operational_condition::REJECT_KINKS};
    /**
     * Caller-supplied layouts, one per input pattern, or `nullptr`.
     */
    const std::vector<layout>* input_pattern_layouts{nullptr};
    /**
     * Assigns a charge state to the SiDB at `site` without touching the charge index.
     *
     * @param state The state to modify.
     * @param lyt The state's layout.
     * @param site The site.
     * @param cs The charge state.
     */
    static void assign(simulation::detail::simulation_state& state, const layout& lyt, const lattice_site& site,
                       const model::charge_state cs) noexcept
    {
        if (const auto index = lyt.index_of(site); index.has_value())
        {
            state.assign_charge_state_by_index(*index, cs, simulation::detail::charge_index_mode::KEEP_CHARGE_INDEX);
        }
    }
    /**
     * The layout with an input pattern applied.
     *
     * @param input_pattern The input pattern.
     * @return The layout.
     */
    [[nodiscard]] const layout& layout_with_input_pattern(const uint64_t input_pattern) noexcept
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
     * Simulates the layout with the configured engine.
     *
     * @param lyt_with_input_pattern The layout to simulate.
     * @return The simulation result; empty if the engine found no valid configuration.
     */
    [[nodiscard]] result physical_simulation_of_layout(const layout& lyt_with_input_pattern) const noexcept
    {
        if (parameters.sim_engine == engine::EXGS)
        {
            return engines::exhaustive_ground_state_simulation(lyt_with_input_pattern, parameters.sim_params);
        }
        if (parameters.sim_engine == engine::QUICKEXACT)
        {
            const engines::quickexact_params qe_params{
                .sim_params            = parameters.sim_params,
                .base_number_detection = engines::quickexact_params::automatic_base_number_detection::OFF};

            return engines::quickexact(lyt_with_input_pattern, qe_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        if (parameters.sim_engine == engine::CLUSTERCOMPLETE)
        {
            const engines::clustercomplete_params cc_params{.sim_params = parameters.sim_params};

            return engines::clustercomplete(lyt_with_input_pattern, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED
        if (parameters.sim_engine == engine::QUICKSIM)
        {
            assert(parameters.sim_params.base == 2 && "QuickSim does not support base-3 simulation");

            const engines::quicksim_params qs_params{.sim_params      = parameters.sim_params,
                                                     .iteration_steps = 500,
                                                     .alpha           = 0.6};

            if (const auto qs_result = engines::quicksim(lyt_with_input_pattern, qs_params); qs_result.has_value())
            {
                return *qs_result;
            }

            return result{};
        }

        assert(false && "unsupported simulation engine");

        return result{};
    }
    /**
     * Whether any pair of an input wire (the input pairs aside) does not encode the bit of the input pattern.
     *
     * @param cd The charge distribution.
     * @param current_input_index The input pattern.
     * @return `true` if a kink exists.
     */
    [[nodiscard]] bool check_existence_of_kinks_in_input_wires(const charge_distribution& cd,
                                                               const uint64_t current_input_index) const noexcept
    {
        return std::ranges::any_of(input_bdl_wires | std::views::reverse,
                                   [this, &cd, &current_input_index, i = 0u](const auto& wire) mutable
                                   {
                                       const auto current_bit_set = (current_input_index & (uint64_t{1} << i++)) != 0;

                                       return std::ranges::any_of(wire.pairs,
                                                                  [this, &cd, current_bit_set, &wire](const auto& bdl)
                                                                  {
                                                                      if (bdl.type == sidb_technology::cell_type::INPUT)
                                                                      {
                                                                          return false;
                                                                      }
                                                                      if (current_bit_set)
                                                                      {
                                                                          return !encodes_bit_one(cd, bdl, wire.port);
                                                                      }
                                                                      return !encodes_bit_zero(cd, bdl, wire.port);
                                                                  });
                                   });
    }
    /**
     * Whether any pair of an output wire does not encode the expected output bit.
     *
     * @param cd The charge distribution.
     * @param current_input_index The input pattern.
     * @return `true` if a kink exists.
     */
    [[nodiscard]] bool check_existence_of_kinks_in_output_wires(const charge_distribution& cd,
                                                                const uint64_t current_input_index) const noexcept
    {
        for (std::size_t i = 0; i < output_bdl_wires.size(); ++i)
        {
            for (const auto& bdl : output_bdl_wires[i].pairs)
            {
                if (kitty::get_bit(truth_table[i], current_input_index))
                {
                    if (!encodes_bit_one(cd, bdl, output_bdl_wires[i].port))
                    {
                        return true;
                    }
                }
                else if (!encodes_bit_zero(cd, bdl, output_bdl_wires[i].port))
                {
                    return true;
                }
            }
        }

        return false;
    }
    /**
     * Whether a BDL pair encodes bit `0` given the wire's port direction.
     *
     * @param cd The charge distribution.
     * @param bdl The pair.
     * @param port The wire's port.
     * @return `true` if the pair encodes `0`.
     */
    [[nodiscard]] static bool encodes_bit_zero(const charge_distribution& cd, const bdl_pair<lattice_site>& bdl,
                                               const fcn::port_direction port) noexcept
    {
        if (port.dir == fcn::port_direction::SOUTH || port.dir == fcn::port_direction::EAST ||
            port.dir == fcn::port_direction::NONE)
        {
            return cd.get_charge_state(bdl.upper) == model::charge_state::NEGATIVE &&
                   cd.get_charge_state(bdl.lower) == model::charge_state::NEUTRAL;
        }

        return cd.get_charge_state(bdl.upper) == model::charge_state::NEUTRAL &&
               cd.get_charge_state(bdl.lower) == model::charge_state::NEGATIVE;
    }
    /**
     * Whether a BDL pair encodes bit `1` given the wire's port direction.
     *
     * @param cd The charge distribution.
     * @param bdl The pair.
     * @param port The wire's port.
     * @return `true` if the pair encodes `1`.
     */
    [[nodiscard]] static bool encodes_bit_one(const charge_distribution& cd, const bdl_pair<lattice_site>& bdl,
                                              const fcn::port_direction port) noexcept
    {
        if (port.dir == fcn::port_direction::SOUTH || port.dir == fcn::port_direction::EAST ||
            port.dir == fcn::port_direction::NONE)
        {
            return cd.get_charge_state(bdl.upper) == model::charge_state::NEUTRAL &&
                   cd.get_charge_state(bdl.lower) == model::charge_state::NEGATIVE;
        }

        return cd.get_charge_state(bdl.upper) == model::charge_state::NEGATIVE &&
               cd.get_charge_state(bdl.lower) == model::charge_state::NEUTRAL;
    }
};

/**
 * Collects the logic cells of a layout into a canvas layout on the same lattice.
 *
 * @param lyt The layout.
 * @return A layout holding only the logic cells of `lyt`.
 */
[[nodiscard]] inline layout canvas_of(const layout& lyt)
{
    layout canvas{lyt.get_lattice()};

    for (const auto& c : lyt.cells_of_type(sidb_technology::cell_type::LOGIC))
    {
        canvas.assign_cell_type(c, sidb_technology::cell_type::LOGIC);
    }

    return canvas;
}
/**
 * Sanity checks shared by every entry point.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout.
 * @param spec The specification.
 */
template <typename TT>
void check_arguments([[maybe_unused]] const layout& lyt, [[maybe_unused]] const std::vector<TT>& spec) noexcept
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(lyt.num_pis() > 0 && "lyt needs input cells");
    assert(lyt.num_pos() > 0 && "lyt needs output cells");
    assert(!spec.empty());
    // all elements in spec must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());
}
/**
 * Builds the implementation for a layout with optional wires and canvas: the canvas defaults to the layout's logic
 * cells.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout.
 * @param spec The specification.
 * @param params Parameters.
 * @param input_wires The input wires, or `std::nullopt` to detect them.
 * @param output_wires The output wires, or `std::nullopt` to detect them.
 * @param canvas_lyt The canvas, or `std::nullopt` to use the logic cells.
 * @return The implementation object.
 */
template <typename TT>
[[nodiscard]] is_operational_impl<TT>
make_impl(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
          const std::optional<std::vector<bdl_wire>>& input_wires,
          const std::optional<std::vector<bdl_wire>>& output_wires, const std::optional<layout>& canvas_lyt)
{
    const auto canvas = canvas_lyt.has_value() ? *canvas_lyt : canvas_of(lyt);

    if (input_wires.has_value() && output_wires.has_value())
    {
        if (!canvas.is_empty())
        {
            return is_operational_impl<TT>{lyt, spec, params, *input_wires, *output_wires, canvas};
        }

        return is_operational_impl<TT>{lyt, spec, params, *input_wires, *output_wires};
    }

    if (!canvas.is_empty())
    {
        return is_operational_impl<TT>{lyt, spec, params, canvas};
    }

    return is_operational_impl<TT>{lyt, spec, params};
}
/**
 * The input patterns that are not operational for the given reasons.
 *
 * @tparam TT Truth table type.
 * @param p The implementation object.
 * @param num_patterns The number of input patterns.
 * @return All patterns that are operational.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t> operational_patterns_of(is_operational_impl<TT>& p, const uint64_t num_patterns)
{
    std::set<uint64_t> input_patterns{};

    for (uint64_t i = 0; i < num_patterns; ++i)
    {
        input_patterns.insert(i);
    }

    for (const auto& [input_pattern, _] : p.determine_non_operational_input_patterns_and_non_operationality_reason())
    {
        input_patterns.erase(input_pattern);
    }

    return input_patterns;
}
/**
 * The input patterns that kinks render non-operational.
 *
 * @tparam TT Truth table type.
 * @param p The implementation object, configured to reject kinks.
 * @return The kink-induced non-operational patterns.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t> kink_patterns_of(is_operational_impl<TT>& p)
{
    std::set<uint64_t> patterns{};

    for (const auto& [input_pattern, reason] :
         p.determine_non_operational_input_patterns_and_non_operationality_reason())
    {
        if (reason == non_operationality_reason::KINKS)
        {
            patterns.insert(input_pattern);
        }
    }

    return patterns;
}

}  // namespace detail

/**
 * Determines whether an SiDB layout implements the given Boolean function(s). Every input pattern is applied to the
 * input BDL pairs, the ground states are simulated with the configured engine, and the charge states of the output
 * BDL pairs are compared with the expected truth-table entries. If the layout carries logic cells, they form the
 * canvas of the pruning filters that run before any simulation whenever the parameters ask for filtering and reject
 * kinks.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return The operational status and the number of simulator invocations.
 */
template <typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params = {}) noexcept
{
    detail::check_arguments(lyt, spec);

    auto p = detail::make_impl(lyt, spec, params, std::nullopt, std::nullopt, std::nullopt);

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}
/**
 * Like the overload above, with the BDL wires and, optionally, the canvas given by the caller.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @param input_bdl_wire The input BDL wires of `lyt`.
 * @param output_bdl_wire The output BDL wires of `lyt`.
 * @param canvas_lyt The canvas; defaults to the logic cells of `lyt`.
 * @return The operational status and the number of simulator invocations.
 */
template <typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
               const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire,
               const std::optional<layout>& canvas_lyt = std::nullopt) noexcept
{
    detail::check_arguments(lyt, spec);

    auto p = detail::make_impl(lyt, spec, params, input_bdl_wire, output_bdl_wire, canvas_lyt);

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}
/**
 * Like the overloads above, but with one layout per input pattern given by the caller instead of applying the
 * patterns to the input BDL pairs.
 *
 * @tparam TT Truth table type.
 * @param input_pattern_layouts One layout per input pattern, pattern `0` first.
 * @param spec The Boolean function(s) to implement.
 * @param params Parameters.
 * @param input_bdl_wire The input BDL wires.
 * @param output_bdl_wire The output BDL wires.
 * @param canvas_lyt The canvas; defaults to the logic cells of the first layout.
 * @return The operational status and the number of simulator invocations.
 * @throws std::invalid_argument if `spec` is empty or the number of layouts does not match the number of patterns.
 */
template <typename TT>
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const std::vector<layout>& input_pattern_layouts, const std::vector<TT>& spec,
               const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire,
               const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    // this overload indexes a caller-supplied container, so a wrong size is an out-of-bounds read rather than a
    // wrong answer. It is also reachable from `pyfiction` with an arbitrary list, so the checks survive `NDEBUG`
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

    const auto canvas = canvas_lyt.has_value() ? *canvas_lyt : detail::canvas_of(input_pattern_layouts.front());

    detail::is_operational_impl<TT> p{input_pattern_layouts, spec, params, input_bdl_wire, output_bdl_wire, canvas};

    const auto [status, _] = p.run();

    return {status, p.get_number_of_simulator_invocations()};
}
/**
 * Determines the input patterns for which the layout is operational.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return The operational input patterns.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t> operational_input_patterns(const layout& lyt, const std::vector<TT>& spec,
                                                            const is_operational_params& params = {}) noexcept
{
    detail::check_arguments(lyt, spec);

    detail::is_operational_impl<TT> p{lyt, spec, params};

    return detail::operational_patterns_of(p, spec.front().num_bits());
}
/**
 * Like the overload above, with the BDL wires and, optionally, the canvas given by the caller.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @param input_bdl_wire The input BDL wires of `lyt`.
 * @param output_bdl_wire The output BDL wires of `lyt`.
 * @param canvas_lyt The canvas; defaults to none.
 * @return The operational input patterns.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t>
operational_input_patterns(const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
                           const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire,
                           const std::optional<layout>& canvas_lyt = std::nullopt) noexcept
{
    detail::check_arguments(lyt, spec);

    if (canvas_lyt.has_value())
    {
        detail::is_operational_impl<TT> p{lyt, spec, params, input_bdl_wire, output_bdl_wire, *canvas_lyt};

        return detail::operational_patterns_of(p, spec.front().num_bits());
    }

    detail::is_operational_impl<TT> p{lyt, spec, params, input_bdl_wire, output_bdl_wire};

    return detail::operational_patterns_of(p, spec.front().num_bits());
}
/**
 * Determines the input patterns for which kinks render the layout non-operational.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters; kinks are rejected regardless of `params.op_condition`.
 * @return The kink-induced non-operational input patterns.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t>
kink_induced_non_operational_input_patterns(const layout& lyt, const std::vector<TT>& spec,
                                            const is_operational_params& params = {}) noexcept
{
    detail::check_arguments(lyt, spec);

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    detail::is_operational_impl<TT> p{lyt, spec, params_with_rejecting_kinks};

    return detail::kink_patterns_of(p);
}
/**
 * Like the overload above, with the BDL wires and, optionally, the canvas given by the caller.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters; kinks are rejected regardless of `params.op_condition`.
 * @param input_bdl_wire The input BDL wires of `lyt`.
 * @param output_bdl_wire The output BDL wires of `lyt`.
 * @param canvas_lyt The canvas; defaults to none.
 * @return The kink-induced non-operational input patterns.
 */
template <typename TT>
[[nodiscard]] std::set<uint64_t> kink_induced_non_operational_input_patterns(
    const layout& lyt, const std::vector<TT>& spec, const is_operational_params& params,
    const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire,
    const std::optional<layout>& canvas_lyt = std::nullopt) noexcept
{
    detail::check_arguments(lyt, spec);

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    if (canvas_lyt.has_value())
    {
        detail::is_operational_impl<TT> p{
            lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire, *canvas_lyt};

        return detail::kink_patterns_of(p);
    }

    detail::is_operational_impl<TT> p{lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire};

    return detail::kink_patterns_of(p);
}
/**
 * Determines whether kinks are the reason the layout is non-operational.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters; kinks are rejected regardless of `params.op_condition`.
 * @return `true` if the layout is non-operational because of kinks.
 */
template <typename TT>
[[nodiscard]] bool is_kink_induced_non_operational(const layout& lyt, const std::vector<TT>& spec,
                                                   const is_operational_params& params = {}) noexcept
{
    detail::check_arguments(lyt, spec);

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    detail::is_operational_impl<TT> p{lyt, spec, params_with_rejecting_kinks};

    const auto [op_status, non_op_reason] = p.run();

    return op_status == operational_status::NON_OPERATIONAL &&
           non_op_reason == detail::non_operationality_reason::KINKS;
}
/**
 * Like the overload above, with the BDL wires and, optionally, the canvas given by the caller.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters; kinks are rejected regardless of `params.op_condition`.
 * @param input_bdl_wire The input BDL wires of `lyt`.
 * @param output_bdl_wire The output BDL wires of `lyt`.
 * @param canvas_lyt The canvas; defaults to none.
 * @return `true` if the layout is non-operational because of kinks.
 */
template <typename TT>
[[nodiscard]] bool is_kink_induced_non_operational(const layout& lyt, const std::vector<TT>& spec,
                                                   const is_operational_params& params,
                                                   const std::vector<bdl_wire>& input_bdl_wire,
                                                   const std::vector<bdl_wire>& output_bdl_wire,
                                                   const std::optional<layout>& canvas_lyt = std::nullopt) noexcept
{
    detail::check_arguments(lyt, spec);

    is_operational_params params_with_rejecting_kinks = params;
    params_with_rejecting_kinks.op_condition          = is_operational_params::operational_condition::REJECT_KINKS;

    const auto run = [&](auto& p)
    {
        const auto [op_status, non_op_reason] = p.run();

        return op_status == operational_status::NON_OPERATIONAL &&
               non_op_reason == detail::non_operationality_reason::KINKS;
    };

    if (canvas_lyt.has_value())
    {
        detail::is_operational_impl<TT> p{
            lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire, *canvas_lyt};

        return run(p);
    }

    detail::is_operational_impl<TT> p{lyt, spec, params_with_rejecting_kinks, input_bdl_wire, output_bdl_wire};

    return run(p);
}

// ---------------------------------------------------------------------------------------------------------------
// Transitional: overloads for SiDB cell-level layouts, converted with `to_sidb_layout`. They serve the algorithms
// that still run on such layouts and disappear once every consumer takes `sidb::layout`.
// ---------------------------------------------------------------------------------------------------------------

/**
 * Transitional overload for SiDB cell-level layouts; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return The operational status and the number of simulator invocations.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::pair<operational_status, std::size_t>
is_operational(const Lyt& lyt, const std::vector<TT>& spec, const is_operational_params& params = {}) noexcept
{
    return is_operational(to_sidb_layout(lyt), spec, params);
}
/**
 * Transitional overload for SiDB cell-level layouts; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return The operational input patterns.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::set<uint64_t> operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec,
                                                            const is_operational_params& params = {}) noexcept
{
    return operational_input_patterns(to_sidb_layout(lyt), spec, params);
}
/**
 * Transitional overload for SiDB cell-level layouts; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return The kink-induced non-operational input patterns.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::set<uint64_t>
kink_induced_non_operational_input_patterns(const Lyt& lyt, const std::vector<TT>& spec,
                                            const is_operational_params& params = {}) noexcept
{
    return kink_induced_non_operational_input_patterns(to_sidb_layout(lyt), spec, params);
}
/**
 * Transitional overload for SiDB cell-level layouts; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The layout to check.
 * @param spec The Boolean function(s) it has to implement.
 * @param params Parameters.
 * @return `true` if the layout is non-operational because of kinks.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] bool is_kink_induced_non_operational(const Lyt& lyt, const std::vector<TT>& spec,
                                                   const is_operational_params& params = {}) noexcept
{
    return is_kink_induced_non_operational(to_sidb_layout(lyt), spec, params);
}

}  // namespace fiction::sidb::simulation::logic
