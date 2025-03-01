//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/physical_design/compare_designed_sidb_gates.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

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
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct contains parameters and settings to design SiDB gates.
 *
 * @tparam Lyt SiDB cell-level layout type.
 *
 */
template <typename Lyt>
struct design_sidb_gates_params
{
    /**
     * Selector for the available design approaches.
     */
    enum class design_sidb_gates_mode : uint8_t
    {
        /**
         * Gates are designed by using *QuickCell*.
         */
        QUICKCELL,
        /**
         * Gates are designed by using the *Automatic Exhaustive Gate Designer*.
         */
        EXHAUSTIVE_GATE_DESIGNER,
        /**
         * Gate layouts are designed randomly.
         */
        RANDOM
    };
    /**
     * Selector for the different termination conditions for the SiDB gate design process.
     */
    enum class termination_condition : uint8_t
    {
        /**
         * The design process is terminated as soon as the first valid SiDB gate design is found.
         */
        AFTER_FIRST_SOLUTION,
        /**
         * The design process ends after all possible combinations of SiDBs within the canvas are enumerated.
         */
        ALL_COMBINATIONS_ENUMERATED
    };
    /**
     * Parameters for the `is_operational` function.
     */
    is_operational_params operational_params{};
    /**
     * Gate design mode.
     */
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::EXHAUSTIVE_GATE_DESIGNER;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<cell<Lyt>, cell<Lyt>> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 1;
    /**
     * The design process is terminated after a valid SiDB gate design is found.
     *
     * @note This parameter has no effect unless the gate design is exhaustive.
     */
    termination_condition termination_cond = termination_condition::ALL_COMBINATIONS_ENUMERATED;
    /**
     * After the design process, the returned gates are not sorted.
     *
     * @note This parameter has no effect unless the gate design is exhaustive and all combinations are enumerated.
     */
    designed_sidb_gates_ordering_recipe<Lyt> post_design_process{};
};
/**
 * Statistics for the design of SiDB gates.
 */
struct design_sidb_gates_stats
{
    /**
     * The total runtime of the SiDB gate design process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The runtime of the pruning process.
     */
    mockturtle::stopwatch<>::duration pruning_total{0};
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * The number of all possible layouts.
     */
    std::size_t number_of_layouts{0};
    /**
     * The number of layouts that remain after first pruning (discarding layouts with potential positive SiDBs).
     */
    std::size_t number_of_layouts_after_first_pruning{0};
    /**
     * The number of layouts that remain after second pruning (discarding layouts that fail to satisfy the physical
     * model).
     */
    std::size_t number_of_layouts_after_second_pruning{0};
    /**
     * The number of layouts that remain after third pruning (discarding layouts with unstable I/O signals).
     */
    std::size_t number_of_layouts_after_third_pruning{0};
    /**
     * This function outputs the total time taken for the SiDB gate design process to the provided output stream.
     * If no output stream is provided, it defaults to standard output (`std::cout`).
     *
     * @param out The output stream to which the report will be written.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
    }
};

namespace detail
{

template <typename Lyt, typename TT>
class design_sidb_gates_impl
{
  public:
    /**
     * This constructor initializes an instance of the *SiDB Gate Designer*
     * implementation with the provided skeleton layout and configuration parameters.
     *
     * @param skeleton The skeleton layout used as a basis for gate design.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param ps Parameters and settings for the gate designer.
     * @param st Statistics for the gate design process.
     */
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& spec, const design_sidb_gates_params<Lyt>& ps,
                           design_sidb_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{spec},
            params{set_operational_params_accordingly(ps)},
            all_sidbs_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)},
            stats{st},
            input_bdl_wires{detect_bdl_wires(skeleton_layout,
                                             params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                             bdl_wire_selection::INPUT)},
            output_bdl_wires{detect_bdl_wires(skeleton_layout,
                                              params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                              bdl_wire_selection::OUTPUT)},
            number_of_input_wires{input_bdl_wires.size()},
            number_of_output_wires{output_bdl_wires.size()}
    {
        stats.number_of_layouts = binomial_coefficient(all_sidbs_in_canvas.size(), params.number_of_sidbs);
        stats.sim_engine        = params.operational_params.sim_engine;
    }
    /**
     * Design gates by using the *Automatic Exhaustive Gate Designer*. This algorithm was proposed in \"Minimal
     * Design of SiDB Gates: An Optimal Basis for Circuits Based on Silicon Dangling Bonds\" by J. Drewniok, M. Walter,
     * and R. Wille in NANOARCH 2023 (https://dl.acm.org/doi/10.1145/3611315.3633241).
     *
     * This function adds each cell combination to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_exhaustive_gate_designer() const noexcept
    {
        if (stats.number_of_layouts == 0)
        {
            return std::vector<Lyt>{};
        }

        const std::vector<Lyt>& all_canvas_layouts = create_all_possible_canvas_layouts();

        std::vector<Lyt> gate_candidates{};
        gate_candidates.reserve(all_canvas_layouts.size());

        std::mutex mutex_to_protect_gate_candidates{};  // used to control access to shared resources

        // The canvas layouts are to be inserted in the skeleton to create all gate candidates; this is done in parallel
        auto insert_canvas_sidbs = [&](const Lyt& canvas_lyt)
        {
            Lyt gate_candidate = skeleton_layout.clone();

            canvas_lyt.foreach_cell([&gate_candidate](const auto& c)
                                    { gate_candidate.assign_cell_type(c, Lyt::technology::cell_type::LOGIC); });

            const std::lock_guard lock{mutex_to_protect_gate_candidates};
            gate_candidates.emplace_back(gate_candidate);
        };

        const std::size_t num_threads = std::min(number_of_threads, all_canvas_layouts.size());
        const std::size_t chunk_size  = (all_canvas_layouts.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &insert_canvas_sidbs, &all_canvas_layouts]
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, all_canvas_layouts.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        insert_canvas_sidbs(all_canvas_layouts.at(j));
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        return extract_gate_designs(gate_candidates);
    }
    /**
     * Design Standard Cells/gates by using the *QuickCell* algorithm.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_quickcell() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        if (stats.number_of_layouts == 0)
        {
            return std::vector<Lyt>{};
        }

        std::vector<Lyt> gate_candidates{};
        gate_candidates.reserve(stats.number_of_layouts);

        {
            mockturtle::stopwatch stop_pruning{stats.pruning_total};
            gate_candidates = run_pruning();
        }

        stats.number_of_layouts_after_first_pruning =
            stats.number_of_layouts - number_of_discarded_layouts_at_first_pruning.load();
        stats.number_of_layouts_after_second_pruning =
            stats.number_of_layouts_after_first_pruning - number_of_discarded_layouts_at_second_pruning.load();
        stats.number_of_layouts_after_third_pruning =
            stats.number_of_layouts_after_second_pruning - number_of_discarded_layouts_at_third_pruning.load();

        return extract_gate_designs(gate_candidates);
    }
    /**
     * Design gates randomly and in parallel.
     *
     * This function adds cells randomly to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_random_design() const noexcept
    {
        std::vector<Lyt> randomly_designed_gate_layouts = {};

        // Allow positive charges here, as a layout that displays positive charges without inputs may not exhibit them
        // once inputs are applied.
        const generate_random_sidb_layout_params<cell<Lyt>> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::ALLOWED};

        std::vector<std::thread> threads{};
        threads.reserve(number_of_threads);

        std::mutex mutex_to_protect_designed_gate_layouts{};  // used to control access to shared resources

        std::atomic<bool> gate_layout_is_found(false);

        for (uint64_t z = 0u; z < number_of_threads; z++)
        {
            threads.emplace_back(
                [this, &gate_layout_is_found, &mutex_to_protect_designed_gate_layouts, &parameter,
                 &randomly_designed_gate_layouts]
                {
                    while (!gate_layout_is_found)
                    {
                        auto result_lyt = generate_random_sidb_layout<Lyt>(parameter, skeleton_layout);

                        if (!result_lyt.has_value())
                        {
                            continue;
                        }

                        if constexpr (has_get_sidb_defect_v<Lyt>)
                        {
                            result_lyt.value().foreach_sidb_defect(
                                [&result_lyt](const auto& cd)
                                {
                                    if (is_neutrally_charged_defect(cd.second))
                                    {
                                        result_lyt.value().assign_sidb_defect(cd.first,
                                                                              sidb_defect{sidb_defect_type::NONE});
                                    }
                                });
                        }

                        if (const operational_assessment<Lyt>& assessment_results = is_operational(
                                result_lyt.value(), truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                            assessment_results.status == operational_status::OPERATIONAL)
                        {
                            const std::lock_guard lock{mutex_to_protect_designed_gate_layouts};

                            if constexpr (has_get_sidb_defect_v<Lyt>)
                            {
                                skeleton_layout.foreach_sidb_defect(
                                    [&result_lyt](const auto& cd)
                                    {
                                        if (is_neutrally_charged_defect(cd.second))
                                        {
                                            result_lyt.value().assign_sidb_defect(cd.first, cd.second);
                                        }
                                    });
                            }

                            randomly_designed_gate_layouts.push_back(result_lyt.value());
                            gate_layout_is_found = true;
                            break;
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        return randomly_designed_gate_layouts;
    }

  private:
    /**
     * The skeleton layout serves as a starting layout to which SiDBs are added to create unique SiDB layouts and, if
     * possible, working gates. It defines input and output wires.
     */
    const Lyt skeleton_layout;
    /**
     * Truth table of the given gate.
     */
    const std::vector<TT> truth_table{};
    /**
     * Parameters for the *SiDB Gate Designer*.
     */
    const design_sidb_gates_params<Lyt> params;
    /**
     * All cells within the canvas.
     */
    const std::vector<typename Lyt::cell> all_sidbs_in_canvas{};
    /**
     * The statistics of the gate design.
     */
    design_sidb_gates_stats& stats;
    /**
     * Input BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> input_bdl_wires{};
    /**
     * Output BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> output_bdl_wires{};
    /**
     * Number of input BDL wires.
     */
    const std::size_t number_of_input_wires{};
    /**
     * Number of output BDL wires.
     */
    const std::size_t number_of_output_wires{};
    /**
     * Number of discarded layouts at first pruning.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_first_pruning{0};
    /**
     * Number of discarded layouts at second pruning.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_second_pruning{0};
    /**
     * Number of discarded layouts at third pruning.
     */
    std::atomic<std::size_t> number_of_discarded_layouts_at_third_pruning{0};
    /**
     * Number of threads to be used for the design process.
     */
    std::size_t number_of_threads{std::thread::hardware_concurrency()};

    [[nodiscard]] std::vector<Lyt> extract_gate_designs(std::vector<Lyt>& gate_candidates) const noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        if (gate_candidates.empty())
        {
            return std::vector<Lyt>{};
        }

        designed_sidb_gates<Lyt> designed_gate_layouts{};

        if (!params.post_design_process.empty())
        {
            designed_gate_layouts.simulation_results =
                std::make_optional<std::vector<typename designed_sidb_gates<Lyt>::simulation_results_per_input>>();
        }

        std::mutex mutex_to_protect_designed_gate_layouts{};

        std::atomic<bool> solution_found = false;

        const auto add_combination_to_layout_and_check_operation = [this, &mutex_to_protect_designed_gate_layouts,
                                                                    &designed_gate_layouts,
                                                                    &solution_found](Lyt&& candidate) noexcept
        {
            if (const operational_assessment<Lyt>& assessment_results = is_operational(
                    candidate, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                assessment_results.status == operational_status::OPERATIONAL)
            {
                {
                    const std::lock_guard lock_vector{mutex_to_protect_designed_gate_layouts};

                    designed_gate_layouts.gate_layouts.emplace_back(std::move(candidate));

                    if (designed_gate_layouts.simulation_results.has_value())
                    {
                        designed_gate_layouts.simulation_results.value().push_back(
                            assessment_results.extract_simulation_results_per_input());
                    }
                }

                solution_found = true;
            }
        };

        const std::size_t num_threads = std::min(number_of_threads, gate_candidates.size());

        const std::size_t chunk_size = (gate_candidates.size() + num_threads - 1) / num_threads;  // Ceiling division

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [this, i, chunk_size, &gate_candidates, &add_combination_to_layout_and_check_operation,
                 &solution_found]()
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, gate_candidates.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        if (solution_found &&
                            (params.termination_cond ==
                             design_sidb_gates_params<Lyt>::termination_condition::AFTER_FIRST_SOLUTION))
                        {
                            return;
                        }

                        add_combination_to_layout_and_check_operation(std::move(gate_candidates[j]));
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        if (!params.post_design_process.empty() && designed_gate_layouts.gate_layouts.size() > 1)
        {
            order_designed_sidb_gates(params.post_design_process, designed_gate_layouts);
        }

        return designed_gate_layouts.gate_layouts;
    }
    /**
     * This function processes each layout to determine if it represents a valid gate implementation or if it can be
     * pruned by using three distinct physically-informed pruning steps. It leverages multi-threading to accelerate the
     * evaluation and ensures thread-safe access to shared resources.
     *
     * @return A vector containing the valid gate candidates that were not pruned.
     */
    [[nodiscard]] std::vector<Lyt> run_pruning() noexcept
    {
        std::vector<Lyt> gate_candidates = {};

        if (stats.number_of_layouts == 0)
        {
            return gate_candidates;
        }

        gate_candidates.reserve(stats.number_of_layouts);

        std::mutex mutex_to_protect_gate_candidates{};  // used to control access to shared resources

        const std::vector<Lyt>& all_canvas_layouts = create_all_possible_canvas_layouts();

        // Function to check validity and add layout to all_designs
        auto conduct_pruning_steps = [&](const Lyt& canvas_lyt)
        {
            Lyt current_layout = skeleton_layout.clone();

            canvas_lyt.foreach_cell([&current_layout](const auto& c)
                                    { current_layout.assign_cell_type(c, Lyt::technology::cell_type::LOGIC); });

            auto bii = bdl_input_iterator<Lyt>{current_layout, params.operational_params.input_bdl_iterator_params,
                                               input_bdl_wires};

            is_operational_impl<Lyt, TT> is_operational_impl{
                current_layout, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires, canvas_lyt};

            for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
            {
                if (const auto reason_for_filtering =
                        is_operational_impl.is_layout_invalid(bii.get_current_input_index());
                    reason_for_filtering.has_value())
                {
                    switch (reason_for_filtering.value())
                    {
                        case layout_invalidity_reason::POTENTIAL_POSITIVE_CHARGES:
                        {
                            ++number_of_discarded_layouts_at_first_pruning;
                            break;
                        }
                        case layout_invalidity_reason::PHYSICAL_INFEASIBILITY:
                        {
                            ++number_of_discarded_layouts_at_second_pruning;
                            break;
                        }
                        case layout_invalidity_reason::IO_INSTABILITY:
                        {
                            ++number_of_discarded_layouts_at_third_pruning;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    return;
                }
            }

            const std::lock_guard lock{mutex_to_protect_gate_candidates};
            gate_candidates.emplace_back(current_layout);
        };

        const std::size_t num_threads = std::min(number_of_threads, stats.number_of_layouts);
        const std::size_t chunk_size  = (stats.number_of_layouts + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &conduct_pruning_steps, &all_canvas_layouts]
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, all_canvas_layouts.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        conduct_pruning_steps(all_canvas_layouts.at(j));
                    }
                });
        }

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        return gate_candidates;
    }
    /**
     * This function calculates all combinations of distributing a given number of SiDBs across a specified number of
     * positions in the canvas. Each combination is then used to create a gate layout candidate.
     *
     * @return A vector containing all possible gate layouts generated from the combinations.
     */
    [[nodiscard]] std::vector<Lyt> create_all_possible_canvas_layouts() const noexcept
    {
        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> all_canvas_layouts = {};
        all_canvas_layouts.reserve(all_combinations.size());

        const auto add_cell_combination_to_layout = [this, &all_canvas_layouts](const auto& combination) noexcept
        {
            const auto layout_with_added_cells = convert_canvas_cell_indices_to_layout(combination);
            all_canvas_layouts.emplace_back(layout_with_added_cells);
        };

        for (const auto& combination : all_combinations)
        {
            add_cell_combination_to_layout(combination);
        }

        // Shuffle all canvas layouts to distribute thread load when extracting gate layouts later
        std::shuffle(all_canvas_layouts.begin(), all_canvas_layouts.end(), std::mt19937_64{std::random_device{}()});

        return all_canvas_layouts;
    }
    /**
     * This function generates canvas SiDb layouts.
     *
     * @param cell_indices A vector of indices of cells to be added to the skeleton layout.
     * @return An SiDB cell-level layout consisting of canvas SidBs.
     */
    [[nodiscard]] Lyt convert_canvas_cell_indices_to_layout(const std::vector<std::size_t>& cell_indices) const noexcept
    {
        Lyt lyt{};

        for (const auto i : cell_indices)
        {
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            // SiDBs cannot be placed on positions which are already occupied by atomic defects.
            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                if (skeleton_layout.get_sidb_defect(all_sidbs_in_canvas.at(i)).type != sidb_defect_type::NONE)
                {
                    continue;
                }
            }
            lyt.assign_cell_type(all_sidbs_in_canvas.at(i), sidb_technology::cell_type::LOGIC);
        }

        // the skeleton can already exhibit some canvas SiDBs (partially filled canvas)
        skeleton_layout.foreach_cell(
            [this, &lyt](const auto& c)
            {
                if (skeleton_layout.get_cell_type(c) == sidb_technology::cell_type::LOGIC)
                {
                    lyt.assign_cell_type(c, Lyt::technology::cell_type::LOGIC);
                }
            });

        return lyt;
    }
    /**
     * This function makes sure that underlying parameters for `is_operational` are set according to the given
     * parameters for `design_sidb_gates`.
     *
     * @param params The given parameters for `design_sidb_gates`.
     * @return The same parameters, but now the underlying parameters for `is_operational` are adjusted accordingly.
     */
    static design_sidb_gates_params<Lyt>
    set_operational_params_accordingly(const design_sidb_gates_params<Lyt>& params) noexcept
    {
        design_sidb_gates_params<Lyt> new_params{params};

        if (!new_params.post_design_process.empty())
        {
            new_params.operational_params.simulation_results_retention =
                is_operational_params::simulation_results_mode::KEEP_SIMULATION_RESULTS;
        }

        if (new_params.design_mode == design_sidb_gates_params<Lyt>::design_sidb_gates_mode::QUICKCELL)
        {
            new_params.operational_params.strategy_to_analyze_operational_status =
                is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;
        }

        return new_params;
    }
};

}  // namespace detail

/**
 * The *SiDB Gate Designer* designs SiDB gate implementations based on a specified Boolean function, a
 * skeleton layout (can hold defects), canvas size, and a predetermined number of canvas SiDBs. Two different design
 * modes are implemented: `exhaustive` and `random design`.
 *
 * The `exhaustive design` is composed of three steps:
 * 1. In the initial step, all possible distributions of `number_of_sidbs` SiDBs within a given canvas are
 * exhaustively determined. This ensures exhaustive coverage of every potential arrangement of ``number_of_sidbs`` SiDBs
 * across the canvas.
 * 2. The calculated SiDB distributions are then incorporated into the skeleton, resulting in the generation of distinct
 * SiDB layouts.
 * 3. The generated SiDB layouts then undergo an extensive simulation process. All input combinations possible for the
 * given Boolean function are used to verify if the logic is fulfilled.
 *
 * The `random design` is composed of four steps:
 * 1. A specified number of canvas SiDBs (`number_of_sidbs`) are randomly added to the skeleton layout.
 * 2. The operation status of the layout is simulated based on a given Boolean function.
 * 3. If the layout is `operational`, it is returned as the result, and the process terminates successfully.
 * 4. If the layout is `non-operational`, the process is repeated from the first step until an operational layout is
 * found.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param skeleton The skeleton layout used for gate design.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the *SiDB Gate Designer*.
 * @param stats Statistics.
 * @return A vector of designed SiDB gate layouts.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::vector<Lyt> design_sidb_gates(const Lyt& skeleton, const std::vector<TT>& spec,
                                                 const design_sidb_gates_params<Lyt>& params = {},
                                                 design_sidb_gates_stats*             stats  = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    assert(skeleton.num_pis() > 0 && "skeleton needs input cells");
    assert(skeleton.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    design_sidb_gates_stats                 st{};
    detail::design_sidb_gates_impl<Lyt, TT> p{skeleton, spec, params, st};

    std::vector<Lyt> result{};

    if (params.design_mode == design_sidb_gates_params<Lyt>::design_sidb_gates_mode::EXHAUSTIVE_GATE_DESIGNER)
    {
        result = p.run_exhaustive_gate_designer();
    }

    else if (params.design_mode == design_sidb_gates_params<Lyt>::design_sidb_gates_mode::QUICKCELL)
    {
        result = p.run_quickcell();
    }

    else
    {
        result = p.run_random_design();
    }

    if (stats)
    {
        *stats = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
