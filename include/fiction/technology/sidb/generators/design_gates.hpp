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
 * @brief Designs SiDB gate implementations for a given Boolean function and skeleton.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/generators/random_layout_generator.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math/combination_utils.hpp"

#include <fmt/format.h>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace fiction::sidb::generators
{

/**
 * Parameters of the gate designers.
 */
struct design_gates_params
{
    /**
     * When to stop.
     */
    enum class termination_condition : uint8_t
    {
        /**
         * Stop after the first operational gate.
         */
        AFTER_FIRST_SOLUTION,
        /**
         * Enumerate every combination of canvas SiDBs.
         */
        ALL_COMBINATIONS_ENUMERATED
    };
    /**
     * The design mode.
     */
    enum class design_gates_mode : uint8_t
    {
        /**
         * *QuickCell*: prune the canvas layouts with the filters of the operational check, then simulate the rest.
         */
        QUICKCELL,
        /**
         * Simulate every canvas layout.
         */
        AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        /**
         * Place canvas SiDBs at random until an operational gate is found or the attempt limit is reached.
         */
        RANDOM,
        /**
         * Only run the pruning filters of *QuickCell* and return the surviving candidates.
         */
        PRUNING_ONLY
    };
    /**
     * Parameters of the operational check.
     */
    simulation::logic::is_operational_params operational_params{};
    /**
     * The design mode.
     */
    design_gates_mode design_mode = design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
    /**
     * The canvas: the area canvas SiDBs are placed in, as two opposite corners.
     */
    std::pair<lattice_site, lattice_site> canvas = {site_at_row(24, 17), site_at_row(34, 28)};
    /**
     * Number of canvas SiDBs.
     */
    std::size_t number_of_canvas_sidbs = 1;
    /**
     * Maximum number of layouts evaluated by random gate design across all threads.
     */
    std::size_t maximal_random_design_attempts = 1'000'000;
    /**
     * When to stop.
     */
    termination_condition termination_cond = termination_condition::AFTER_FIRST_SOLUTION;
};

/**
 * Statistics of the gate designers.
 */
struct design_gates_stats
{
    /**
     * Total runtime.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Runtime of the pruning.
     */
    mockturtle::stopwatch<>::duration pruning_total{0};
    /**
     * The simulation engine used.
     */
    simulation::engine sim_engine{simulation::engine::QUICKEXACT};
    /**
     * Number of canvas layouts.
     */
    std::size_t number_of_layouts{0};
    /**
     * Number of canvas layouts that survived the first pruning filter.
     */
    std::size_t number_of_layouts_after_first_pruning{0};
    /**
     * Number of canvas layouts that survived the second pruning filter.
     */
    std::size_t number_of_layouts_after_second_pruning{0};
    /**
     * Number of canvas layouts that survived the third pruning filter.
     */
    std::size_t number_of_layouts_after_third_pruning{0};
    /**
     * Prints the statistics.
     *
     * @param out The stream to print to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

/**
 * Implementation of the gate designers.
 *
 * @tparam TT Truth table type.
 */
template <typename TT>
class design_gates_impl
{
  public:
    /**
     * Constructor.
     *
     * @param skeleton The skeleton: the input and output wires the gate has to connect.
     * @param spec The Boolean function(s) to implement.
     * @param ps Parameters.
     * @param st Statistics.
     */
    // NOLINTBEGIN(modernize-pass-by-value): Passing the large stored snapshots by value adds a copy.
    design_gates_impl(const layout& skeleton, const std::vector<TT>& spec, const design_gates_params& ps,
                      design_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{spec},
            params{ps},
            all_sidbs_in_canvas{sites_in_area(params.canvas.first, params.canvas.second)},
            stats{st},
            input_bdl_wires{simulation::logic::detect_bdl_wires(
                skeleton_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                simulation::logic::bdl_wire_selection::INPUT)},
            output_bdl_wires{simulation::logic::detect_bdl_wires(
                skeleton_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                simulation::logic::bdl_wire_selection::OUTPUT)},
            all_canvas_layouts{determine_all_possible_canvas_layouts()}
    {
        stats.number_of_layouts = all_canvas_layouts.size();
        stats.sim_engine        = params.operational_params.sim_engine;
    }
    // NOLINTEND(modernize-pass-by-value)
    /**
     * Simulates every combination of canvas SiDBs.
     *
     * @return The operational gates.
     */
    [[nodiscard]] std::vector<layout> run_automatic_exhaustive_gate_designer() const
    {
        const mockturtle::stopwatch stop{stats.time_total};

        auto all_combinations = utils::math::determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_canvas_sidbs, all_sidbs_in_canvas.size());

        std::vector<layout> designed_gate_layouts{};

        if (all_combinations.empty())
        {
            return designed_gate_layouts;
        }

        std::mutex        mutex{};
        std::atomic<bool> solution_found = false;

        std::shuffle(all_combinations.begin(), all_combinations.end(),
                     std::default_random_engine(std::random_device{}()));

        const auto check = [this, &mutex, &designed_gate_layouts, &solution_found](const auto& combination) noexcept
        {
            const auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);

            if (const auto [status, sim_calls] = simulation::logic::is_operational(
                    layout_with_added_cells, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                status == simulation::logic::operational_status::OPERATIONAL)
            {
                {
                    const std::scoped_lock lock{mutex};
                    designed_gate_layouts.push_back(layout_with_added_cells);
                }
                solution_found = true;
            }
        };

        for_each_in_parallel(all_combinations, check, solution_found);

        return designed_gate_layouts;
    }
    /**
     * Places canvas SiDBs at random until an operational gate is found.
     *
     * @return The operational gate, or an empty vector if none was found.
     */
    [[nodiscard]] std::vector<layout> run_random_design() const
    {
        std::vector<layout> randomly_designed_gate_layouts{};

        // without a single admissible canvas layout, no random placement can ever succeed
        if (all_canvas_layouts.empty())
        {
            return randomly_designed_gate_layouts;
        }

        const generate_random_layout_params parameter{.coordinate_pair = params.canvas,
                                                      .number_of_sidbs = params.number_of_canvas_sidbs,
                                                      .positive_sidbs =
                                                          generate_random_layout_params::positive_charges::ALLOWED};

        const auto num_threads = std::max(std::min(number_of_threads, all_canvas_layouts.size()), std::size_t{1});

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        std::mutex         mutex{};
        std::atomic<bool>  gate_layout_is_found(false);
        std::atomic_size_t attempt_counter{0};

        for (std::size_t z = 0; z < num_threads; ++z)
        {
            threads.emplace_back(
                [this, &gate_layout_is_found, &attempt_counter, &mutex, &parameter, &randomly_designed_gate_layouts]
                {
                    while (!gate_layout_is_found)
                    {
                        if (attempt_counter.fetch_add(1, std::memory_order_relaxed) >=
                            params.maximal_random_design_attempts)
                        {
                            break;
                        }

                        auto result_lyt = generate_random_layout(parameter, skeleton_layout);

                        if (!result_lyt.has_value())
                        {
                            continue;
                        }

                        // neutral defects do not take part in the simulation
                        for (const auto& [s, d] : skeleton_layout.defects())
                        {
                            if (model::is_neutrally_charged_defect(d))
                            {
                                result_lyt->assign_defect(s, model::defect{model::defect_type::NONE});
                            }
                        }

                        if (const auto [status, sim_calls] = simulation::logic::is_operational(
                                *result_lyt, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                            status == simulation::logic::operational_status::OPERATIONAL)
                        {
                            const std::scoped_lock lock{mutex};

                            for (const auto& [s, d] : skeleton_layout.defects())
                            {
                                if (model::is_neutrally_charged_defect(d))
                                {
                                    result_lyt->assign_defect(s, d);
                                }
                            }

                            randomly_designed_gate_layouts.push_back(*result_lyt);
                            gate_layout_is_found = true;

                            break;
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        return randomly_designed_gate_layouts;
    }
    /**
     * *QuickCell*: prunes the canvas layouts with the filters of the operational check and simulates the survivors.
     *
     * @return The operational gates, or the pruned candidates in `PRUNING_ONLY` mode.
     */
    [[nodiscard]] std::vector<layout> run_quickcell()
    {
        const mockturtle::stopwatch stop{stats.time_total};

        std::vector<layout> gate_candidates{};

        {
            const mockturtle::stopwatch stop_pruning{stats.pruning_total};
            gate_candidates = run_pruning();
        }

        stats.number_of_layouts_after_first_pruning =
            all_canvas_layouts.size() - number_of_discarded_layouts_at_first_pruning.load();
        stats.number_of_layouts_after_second_pruning =
            stats.number_of_layouts_after_first_pruning - number_of_discarded_layouts_at_second_pruning.load();
        stats.number_of_layouts_after_third_pruning =
            stats.number_of_layouts_after_second_pruning - number_of_discarded_layouts_at_third_pruning.load();

        std::vector<layout> gate_layouts{};

        if (gate_candidates.empty())
        {
            return gate_layouts;
        }

        if (params.design_mode == design_gates_params::design_gates_mode::PRUNING_ONLY)
        {
            return gate_candidates;
        }

        gate_layouts.reserve(gate_candidates.size());

        std::mutex        mutex{};
        std::atomic<bool> gate_design_found = false;

        params.operational_params.strategy_to_analyze_operational_status =
            simulation::logic::is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;

        const auto check = [this, &gate_layouts, &mutex, &gate_design_found](const auto& candidate) noexcept
        {
            if (const auto [status, sim_calls] = simulation::logic::is_operational(
                    candidate, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                status == simulation::logic::operational_status::OPERATIONAL)
            {
                {
                    const std::scoped_lock lock{mutex};
                    gate_layouts.push_back(candidate);
                }
                gate_design_found = true;
            }
        };

        for_each_in_parallel(gate_candidates, check, gate_design_found);

        return gate_layouts;
    }

  private:
    /**
     * The skeleton.
     */
    const layout skeleton_layout;
    /**
     * The Boolean function(s).
     */
    const std::vector<TT> truth_table;
    /**
     * Parameters.
     */
    design_gates_params params;
    /**
     * All sites of the canvas.
     */
    std::vector<lattice_site> all_sidbs_in_canvas;
    /**
     * Statistics.
     */
    design_gates_stats& stats;
    /**
     * The input wires of the skeleton.
     */
    const std::vector<simulation::logic::bdl_wire> input_bdl_wires;
    /**
     * The output wires of the skeleton.
     */
    const std::vector<simulation::logic::bdl_wire> output_bdl_wires;
    /**
     * All canvas layouts: one per combination of canvas SiDBs.
     */
    const std::vector<layout> all_canvas_layouts{};
    /**
     * Number of canvas layouts the first pruning filter discarded.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_first_pruning{0};
    /**
     * Number of canvas layouts the second pruning filter discarded.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_second_pruning{0};
    /**
     * Number of canvas layouts the third pruning filter discarded.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_third_pruning{0};
    /**
     * Number of threads to use.
     */
    std::size_t number_of_threads{std::thread::hardware_concurrency()};
    /**
     * Runs `fn` on every item on the configured number of threads; stops early once `done` is set and the
     * termination condition asks for the first solution only.
     *
     * @tparam Items Container type.
     * @tparam Fn Callable type.
     * @param items The items.
     * @param fn The function.
     * @param done The stop flag.
     */
    template <typename Items, typename Fn>
    void for_each_in_parallel(const Items& items, const Fn& fn, std::atomic<bool>& done) const
    {
        const std::size_t num_threads = std::max(std::min(number_of_threads, items.size()), std::size_t{1});
        const std::size_t chunk_size  = (items.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [this, i, chunk_size, &items, &fn, &done]
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, items.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        if (done &&
                            params.termination_cond == design_gates_params::termination_condition::AFTER_FIRST_SOLUTION)
                        {
                            return;
                        }

                        fn(items[j]);
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }
    /**
     * Runs the pruning filters of the operational check on every canvas layout.
     *
     * @return The surviving candidates.
     */
    [[nodiscard]] std::vector<layout> run_pruning() noexcept
    {
        std::vector<layout> gate_candidates{};

        if (all_canvas_layouts.empty())
        {
            return gate_candidates;
        }

        gate_candidates.reserve(all_canvas_layouts.size());

        std::mutex mutex{};

        const auto conduct_pruning_steps = [&](const layout& canvas_lyt)
        {
            if (canvas_lyt.is_empty())
            {
                return;
            }

            auto current_layout = skeleton_layout;

            canvas_lyt.foreach_cell([&current_layout](const auto& c)
                                    { current_layout.assign_cell_type(c, sidb_technology::cell_type::LOGIC); });

            simulation::logic::detail::is_operational_impl<TT> is_operational_impl{
                current_layout, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires, canvas_lyt};

            for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
            {
                const auto reason = is_operational_impl.is_layout_invalid(i);

                if (!reason.has_value())
                {
                    continue;
                }

                switch (*reason)
                {
                    case simulation::logic::detail::layout_invalidity_reason::POTENTIAL_POSITIVE_CHARGES:
                    {
                        ++number_of_discarded_layouts_at_first_pruning;
                        break;
                    }
                    case simulation::logic::detail::layout_invalidity_reason::PHYSICAL_INFEASIBILITY:
                    {
                        ++number_of_discarded_layouts_at_second_pruning;
                        break;
                    }
                    case simulation::logic::detail::layout_invalidity_reason::IO_INSTABILITY:
                    {
                        ++number_of_discarded_layouts_at_third_pruning;
                        break;
                    }
                }

                return;
            }

            const std::scoped_lock lock{mutex};
            gate_candidates.push_back(current_layout);
        };

        std::atomic<bool> never{false};
        for_each_in_parallel(all_canvas_layouts, conduct_pruning_steps, never);

        return gate_candidates;
    }
    /**
     * Builds one canvas layout per combination of canvas SiDBs.
     *
     * @return The canvas layouts.
     */
    [[nodiscard]] std::vector<layout> determine_all_possible_canvas_layouts() const noexcept
    {
        const auto all_combinations = utils::math::determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_canvas_sidbs, all_sidbs_in_canvas.size());

        std::vector<layout> canvas_layouts{};
        canvas_layouts.reserve(all_combinations.size());

        for (const auto& combination : all_combinations)
        {
            if (const auto canvas = design_canvas_layout(combination); canvas.has_value())
            {
                canvas_layouts.push_back(*canvas);
            }
        }

        return canvas_layouts;
    }
    /**
     * Adds the canvas SiDBs of a combination to the skeleton as logic cells; sites that hold a defect stay empty.
     *
     * @param cell_indices Indices into the canvas sites.
     * @return The skeleton with the canvas SiDBs.
     */
    [[nodiscard]] layout skeleton_layout_with_canvas_sidbs(const std::vector<std::size_t>& cell_indices) const noexcept
    {
        auto lyt_copy = skeleton_layout;

        for (const auto i : cell_indices)
        {
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            const auto& s = all_sidbs_in_canvas[i];

            if (lyt_copy.get_cell_type(s) == sidb_technology::cell_type::EMPTY &&
                skeleton_layout.get_defect(s).type == model::defect_type::NONE)
            {
                lyt_copy.assign_cell_type(s, sidb_technology::cell_type::LOGIC);
            }
        }

        return lyt_copy;
    }
    /**
     * Builds the canvas layout of a combination: its canvas SiDBs plus the skeleton's own logic cells. A
     * combination that puts an SiDB onto a defect yields no layout.
     *
     * @param cell_indices Indices into the canvas sites.
     * @return The canvas layout, or `std::nullopt`.
     */
    [[nodiscard]] std::optional<layout>
    design_canvas_layout(const std::vector<std::size_t>& cell_indices) const noexcept
    {
        layout lyt{skeleton_layout.get_lattice()};

        for (const auto i : cell_indices)
        {
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            const auto& s = all_sidbs_in_canvas[i];

            if (skeleton_layout.get_cell_type(s) == sidb_technology::cell_type::EMPTY)
            {
                if (skeleton_layout.get_defect(s).type != model::defect_type::NONE)
                {
                    return std::nullopt;
                }

                lyt.assign_cell_type(s, sidb_technology::cell_type::LOGIC);
            }
        }

        for (const auto& c : skeleton_layout.cells_of_type(sidb_technology::cell_type::LOGIC))
        {
            lyt.assign_cell_type(c, sidb_technology::cell_type::LOGIC);
        }

        return lyt;
    }
};

}  // namespace detail

/**
 * Designs SiDB gates on a skeleton: canvas SiDBs are placed in the canvas area so that the skeleton's input and
 * output wires implement the given Boolean function(s). The design mode chooses between simulating every
 * combination of canvas SiDBs, *QuickCell*'s pruning followed by simulation, random placement, and pruning only.
 *
 * @tparam TT Truth table type.
 * @param skeleton The skeleton with its input and output wires.
 * @param spec The Boolean function(s) to implement; must not be empty.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The designed gates.
 * @throws std::invalid_argument if `spec` is empty.
 */
template <typename TT>
[[nodiscard]] std::vector<layout> design_gates(const layout& skeleton, const std::vector<TT>& spec,
                                               const design_gates_params& params = {},
                                               design_gates_stats*        stats  = nullptr)
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    if (spec.empty())
    {
        throw std::invalid_argument{"spec must not be empty"};
    }

    assert(skeleton.num_pis() > 0 && "skeleton needs input cells");
    assert(skeleton.num_pos() > 0 && "skeleton needs output cells");
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.end());

    design_gates_stats            st{};
    detail::design_gates_impl<TT> p{skeleton, spec, params, st};

    std::vector<layout> result{};

    if (params.design_mode == design_gates_params::design_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER)
    {
        result = p.run_automatic_exhaustive_gate_designer();
    }
    else if (params.design_mode == design_gates_params::design_gates_mode::RANDOM)
    {
        result = p.run_random_design();
    }
    else
    {
        result = p.run_quickcell();
    }

    if (stats)
    {
        *stats = st;
    }

    return result;
}
/**
 * Transitional overload for SiDB cell-level layouts: the skeleton is converted with `to_sidb_layout` and the gates
 * with `to_cell_level_layout`.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param skeleton The skeleton.
 * @param spec The Boolean function(s) to implement; must not be empty.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The designed gates.
 * @throws std::invalid_argument if `spec` is empty.
 */
template <typename Lyt, typename TT>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::vector<Lyt> design_gates(const Lyt& skeleton, const std::vector<TT>& spec,
                                            const design_gates_params& params = {}, design_gates_stats* stats = nullptr)
{
    const auto gates = design_gates(to_sidb_layout(skeleton), spec, params, stats);

    std::vector<Lyt> converted{};
    converted.reserve(gates.size());

    for (const auto& g : gates)
    {
        converted.push_back(to_cell_level_layout<Lyt>(g));
    }

    return converted;
}

}  // namespace fiction::sidb::generators
