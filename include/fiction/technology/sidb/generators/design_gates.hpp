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

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/utils/execution_timeout.hpp"
#include "fiction/utils/math/combination_utils.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <fmt/format.h>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <iterator>
#include <limits>
#include <mutex>
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
    /**
     * Timeout in milliseconds, including candidate generation and simulation. The maximum value means unlimited;
     * zero expires immediately. Checks are cooperative, so allocation and non-interruptible setup can exceed the
     * budget. Finite budgets support QUICKEXACT, EXGS, and QUICKSIM, but not CLUSTERCOMPLETE.
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
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
     * Number of canvas layouts, saturated at `std::size_t`'s maximum.
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
 */
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
     * @throws std::invalid_argument if the input wire count differs from the specification.
     */
    // NOLINTBEGIN(modernize-pass-by-value): Passing the large stored snapshots by value adds a copy.
    design_gates_impl(const layout& skeleton, const std::vector<kitty::dynamic_truth_table>& spec,
                      const design_gates_params& ps, design_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{spec},
            params{ps},
            available_sidbs_in_canvas{
                [this]
                {
                    auto sites =
                        sites_in_area(params.canvas.first, params.canvas.second, params.operational_params.deadline);
                    std::erase_if(sites,
                                  [this](const auto& s)
                                  {
                                      utils::check_deadline(params.operational_params.deadline);
                                      return !skeleton_layout.is_empty_site(s) ||
                                             skeleton_layout.get_defect(s).type != model::defect_type::NONE;
                                  });
                    return sites;
                }()},
            stats{st},
            input_bdl_wires{simulation::logic::detect_bdl_wires(
                skeleton_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                simulation::logic::bdl_wire_selection::INPUT)},
            output_bdl_wires{simulation::logic::detect_bdl_wires(
                skeleton_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                simulation::logic::bdl_wire_selection::OUTPUT)},
            all_canvas_layouts{params.design_mode == design_gates_params::design_gates_mode::QUICKCELL ||
                                       params.design_mode == design_gates_params::design_gates_mode::PRUNING_ONLY ?
                                   determine_all_possible_canvas_layouts() :
                                   std::vector<layout>{}}
    {
        if (input_bdl_wires.size() != truth_table.front().num_vars())
        {
            throw std::invalid_argument{"input wire count must match the specification"};
        }

        const auto available    = available_sidbs_in_canvas.size();
        stats.number_of_layouts = params.number_of_canvas_sidbs == 0 ?
                                      0 :
                                      static_cast<std::size_t>(std::min<uint64_t>(
                                          utils::math::binomial_coefficient(available, params.number_of_canvas_sidbs),
                                          std::numeric_limits<std::size_t>::max()));
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
            params.number_of_canvas_sidbs, available_sidbs_in_canvas.size(), params.operational_params.deadline);

        std::vector<layout> designed_gate_layouts{};

        if (all_combinations.empty())
        {
            return designed_gate_layouts;
        }

        std::mutex        mutex{};
        std::atomic<bool> solution_found = false;

        std::shuffle(all_combinations.begin(), all_combinations.end(),
                     std::default_random_engine(std::random_device{}()));

        const auto check = [this, &mutex, &designed_gate_layouts, &solution_found](const auto& combination)
        {
            const auto layout_with_added_dots = skeleton_layout_with_canvas_sidbs(combination);

            if (const auto [status, sim_calls] = simulation::logic::is_operational(
                    layout_with_added_dots, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                status == simulation::logic::operational_status::OPERATIONAL)
            {
                {
                    const std::scoped_lock lock{mutex};
                    designed_gate_layouts.push_back(layout_with_added_dots);
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
        std::vector<layout> gates{};

        if (stats.number_of_layouts == 0 || params.maximal_random_design_attempts == 0)
        {
            return gates;
        }

        const auto affected_sites = skeleton_layout.all_affected_sidbs(std::make_pair(uint16_t{0}, uint16_t{0}));
        auto       sites          = available_sidbs_in_canvas;
        std::erase_if(sites, [&](const auto& s) { return affected_sites.contains(s); });

        if (sites.size() < params.number_of_canvas_sidbs)
        {
            return gates;
        }

        const auto num_threads =
            std::max(std::min({number_of_threads, stats.number_of_layouts, params.maximal_random_design_attempts}),
                     std::size_t{1});

        std::mutex         mutex{};
        std::atomic<bool>  gate_layout_is_found(false);
        std::atomic_size_t attempt_counter{0};

        std::vector<std::future<void>> workers{};
        workers.reserve(num_threads);

        try
        {
            for (std::size_t z = 0; z < num_threads; ++z)
            {
                workers.emplace_back(std::async(
                    std::launch::async,
                    [this, &gate_layout_is_found, &attempt_counter, &mutex, &sites, &gates]
                    {
                        std::mt19937_64 generator{std::random_device{}()};

                        try
                        {
                            while (!gate_layout_is_found)
                            {
                                utils::check_deadline(params.operational_params.deadline);
                                if (attempt_counter.fetch_add(1, std::memory_order_relaxed) >=
                                    params.maximal_random_design_attempts)
                                {
                                    break;
                                }

                                std::vector<lattice_site> selected_sites{};
                                selected_sites.reserve(params.number_of_canvas_sidbs);
                                std::ranges::sample(sites, std::back_inserter(selected_sites),
                                                    static_cast<std::ptrdiff_t>(params.number_of_canvas_sidbs),
                                                    generator);

                                auto result_lyt = skeleton_layout;
                                for (const auto& site : selected_sites)
                                {
                                    result_lyt.assign_sidb(site, dot_tag::LOGIC);
                                }

                                // neutral defects do not take part in the simulation
                                for (const auto& [s, d] : skeleton_layout.defects())
                                {
                                    if (model::is_neutrally_charged_defect(d))
                                    {
                                        result_lyt.assign_defect(s, model::defect{model::defect_type::NONE});
                                    }
                                }

                                if (const auto [status, sim_calls] = simulation::logic::is_operational(
                                        result_lyt, truth_table, params.operational_params, input_bdl_wires,
                                        output_bdl_wires);
                                    status == simulation::logic::operational_status::OPERATIONAL)
                                {
                                    const std::scoped_lock lock{mutex};

                                    for (const auto& [s, d] : skeleton_layout.defects())
                                    {
                                        if (model::is_neutrally_charged_defect(d))
                                        {
                                            result_lyt.assign_defect(s, d);
                                        }
                                    }

                                    gates.push_back(std::move(result_lyt));
                                    gate_layout_is_found = true;

                                    break;
                                }
                            }
                        }
                        catch (...)
                        {
                            gate_layout_is_found = true;
                            throw;
                        }
                    }));
            }
        }
        catch (...)
        {
            gate_layout_is_found = true;
            throw;
        }

        for (auto& worker : workers)
        {
            worker.get();
        }

        return gates;
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

        const auto check = [this, &gate_layouts, &mutex, &gate_design_found](const auto& candidate)
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
    const std::vector<kitty::dynamic_truth_table> truth_table;
    /**
     * Parameters.
     */
    design_gates_params params;
    /**
     * Empty, defect-free sites of the canvas.
     */
    const std::vector<lattice_site> available_sidbs_in_canvas;
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
     * termination condition asks for the first solution only. Worker exceptions propagate after all started workers
     * finish.
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

        std::vector<std::future<void>> workers{};
        workers.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            utils::check_deadline(params.operational_params.deadline);
            workers.emplace_back(
                std::async(std::launch::async,
                           [this, i, chunk_size, &items, &fn, &done]
                           {
                               const std::size_t start_index = i * chunk_size;
                               const std::size_t end_index   = std::min(start_index + chunk_size, items.size());

                               for (std::size_t j = start_index; j < end_index; ++j)
                               {
                                   utils::check_deadline(params.operational_params.deadline);
                                   if (done && params.termination_cond ==
                                                   design_gates_params::termination_condition::AFTER_FIRST_SOLUTION)
                                   {
                                       return;
                                   }

                                   fn(items[j]);
                               }
                           }));
        }

        for (auto& worker : workers)
        {
            worker.get();
        }
    }
    /**
     * Runs the pruning filters of the operational check on every canvas layout.
     *
     * @return The surviving candidates.
     */
    [[nodiscard]] std::vector<layout> run_pruning()
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

            canvas_lyt.foreach_dot([&current_layout](const auto& c) { current_layout.assign_sidb(c, dot_tag::LOGIC); });

            simulation::logic::detail::is_operational_impl is_operational_impl{
                current_layout, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires, canvas_lyt};

            for (auto i = 0u; i < truth_table.front().num_bits(); ++i)
            {
                utils::check_deadline(params.operational_params.deadline);
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
    [[nodiscard]] std::vector<layout> determine_all_possible_canvas_layouts() const
    {
        const auto all_combinations = utils::math::determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_canvas_sidbs, available_sidbs_in_canvas.size(), params.operational_params.deadline);

        std::vector<layout> canvas_layouts{};
        canvas_layouts.reserve(all_combinations.size());

        for (const auto& combination : all_combinations)
        {
            utils::check_deadline(params.operational_params.deadline);
            canvas_layouts.push_back(design_canvas_layout(combination));
        }

        return canvas_layouts;
    }
    /**
     * Adds the canvas SiDBs of a combination to the skeleton as logic dots; sites that hold a defect stay empty.
     *
     * @param site_indices Indices into the canvas sites.
     * @return The skeleton with the canvas SiDBs.
     */
    [[nodiscard]] layout skeleton_layout_with_canvas_sidbs(const std::vector<std::size_t>& site_indices) const
    {
        auto lyt_copy = skeleton_layout;

        for (const auto i : site_indices)
        {
            assert(i < available_sidbs_in_canvas.size() && "site indices are out-of-range");

            lyt_copy.assign_sidb(available_sidbs_in_canvas[i], dot_tag::LOGIC);
        }

        return lyt_copy;
    }
    /**
     * Builds the canvas layout of a combination: its canvas SiDBs plus the skeleton's own logic dots.
     *
     * @param site_indices Indices into the canvas sites.
     * @return The canvas layout.
     */
    [[nodiscard]] layout design_canvas_layout(const std::vector<std::size_t>& site_indices) const
    {
        layout lyt{skeleton_layout.get_lattice()};

        for (const auto i : site_indices)
        {
            assert(i < available_sidbs_in_canvas.size() && "site indices are out-of-range");

            lyt.assign_sidb(available_sidbs_in_canvas[i], dot_tag::LOGIC);
        }

        for (const auto& c : skeleton_layout.dots_with_tag(dot_tag::LOGIC))
        {
            lyt.assign_sidb(c, dot_tag::LOGIC);
        }

        return lyt;
    }
};

}  // namespace detail

/**
 * @brief Designs SiDB gates on a skeleton: canvas SiDBs are placed in the canvas area so that the skeleton's input and
 * output wires implement the given Boolean function(s). The design mode chooses between simulating every
 * combination of canvas SiDBs, *QuickCell*'s pruning followed by simulation, random placement, and pruning only.
 * Worker exceptions propagate to the caller after all started workers finish.
 * Random placement samples at most `maximal_random_design_attempts` candidates without enumerating canvas layouts.
 * The timeout covers setup and all search phases. Expiration discards partial results and stops all workers before
 * throwing. Allocation and non-interruptible setup may exceed the cooperative deadline.
 *
 * *QuickCell* is described in "Towards Fast Automatic Design of Silicon Dangling Bond Logic" by J. Drewniok,
 * M. Walter, S. S. H. Ng, K. Walus, and R. Wille in DATE 2025
 * (https://ieeexplore.ieee.org/abstract/document/10992885) and "QuickCell: Fast Automatic Design of Standard Cells
 * for Silicon Dangling Bond Logic" by the same authors in TCAD 2025
 * (https://ieeexplore.ieee.org/document/11146893).
 * The exhaustive designer is described in "Minimal Design of SiDB Gates: An Optimal Basis for Circuits Based on
 * Silicon Dangling Bonds" by J. Drewniok, M. Walter, and R. Wille in NANOARCH 2023
 * (https://dl.acm.org/doi/10.1145/3611315.3633241).
 *
 * @param skeleton The skeleton with its input and output wires.
 * @param spec The Boolean function(s) to implement; must not be empty.
 * @param params Parameters.
 * @param stats Statistics.
 * @return The designed gates.
 * @throws std::invalid_argument if `spec` is empty or the input wire count differs from the specification.
 * @throws utils::timeout_error if the gate-design deadline is reached.
 */
[[nodiscard]] inline std::vector<layout> design_gates(const layout&                                  skeleton,
                                                      const std::vector<kitty::dynamic_truth_table>& spec,
                                                      const design_gates_params&                     params = {},
                                                      design_gates_stats*                            stats  = nullptr)
{
    auto timed_params                        = params;
    timed_params.operational_params.deadline = utils::make_deadline(params.timeout, params.operational_params.deadline);
    utils::check_deadline(timed_params.operational_params.deadline);
    if (spec.empty())
    {
        throw std::invalid_argument{"spec must not be empty"};
    }

    assert(skeleton.num_pis() > 0 && "skeleton needs input dots");
    assert(skeleton.num_pos() > 0 && "skeleton needs output dots");
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.end());

    design_gates_stats        st{};
    detail::design_gates_impl p{skeleton, spec, timed_params, st};

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

    utils::check_deadline(timed_params.operational_params.deadline);
    if (stats != nullptr)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction::sidb::generators
