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
#include <mockturtle/utils/progress_bar.hpp>
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
         * Gates are designed by using *QuickCell*. This is type of gate design is exhaustive but involves pruning prior
         * to exhaustive enumeration.
         */
        QUICKCELL,
        /**
         * Gates are designed by using the *Automatic Exhaustive Gate Designer*.
         */
        EXHAUSTIVE,
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
         * The design process is terminated as soon as a given amount of valid SiDB gate designs is found
         * (`maximum_number_of_solutions`).
         */
        OBTAINED_N_SOLUTIONS,
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
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::EXHAUSTIVE;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<cell<Lyt>, cell<Lyt>> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_canvas_sidbs = 1;
    /**
     * The design process is either terminated after all canvas layouts have been considered, or the given amount of
     * valid SiDB gate designs is found (`maximum_number_of_solutions`).
     *
     * @note This parameter has no effect unless the gate design is exhaustive. For random gate design, termination
     * always occurs after finding the given amount of solutions.
     */
    termination_condition termination_cond = termination_condition::ALL_COMBINATIONS_ENUMERATED;
    /**
     * Number of solutions that needs to be obtained before termination.
     *
     * @note This parameter has no effect when the gate design is exhaustive and all combinations are enumerated.
     */
    uint64_t maximum_number_of_solutions = 1;
    /**
     * The number of threads available for the SiDB gate design process
     */
    uint64_t available_threads = std::thread::hardware_concurrency();
    /**
     * After the design process, the returned gates can be sorted by the given ordering recipe.
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
        stats.number_of_layouts = binomial_coefficient(all_sidbs_in_canvas.size(), params.number_of_canvas_sidbs);
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
        mockturtle::stopwatch stop{stats.time_total};

        if (stats.number_of_layouts == 0)
        {
            return std::vector<Lyt>{};
        }

        return extract_gate_designs(determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_canvas_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size())));
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

        std::vector<canvas_combination> candidate_combinations{};
        candidate_combinations.reserve(stats.number_of_layouts);

        {
            mockturtle::stopwatch stop_pruning{stats.pruning_total};
            candidate_combinations = run_pruning();
        }

        stats.number_of_layouts_after_first_pruning =
            stats.number_of_layouts - number_of_discarded_layouts_at_first_pruning.load();
        stats.number_of_layouts_after_second_pruning =
            stats.number_of_layouts_after_first_pruning - number_of_discarded_layouts_at_second_pruning.load();
        stats.number_of_layouts_after_third_pruning =
            stats.number_of_layouts_after_second_pruning - number_of_discarded_layouts_at_third_pruning.load();

        return extract_gate_designs(candidate_combinations);
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
            params.canvas, params.number_of_canvas_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::ALLOWED};

        std::vector<std::thread> threads{};
        threads.reserve(params.available_threads);

        std::mutex mutex_to_protect_designed_gate_layouts{};  // used to control access to shared resources

        const auto check_if_gate_design_is_already_present = [&](const Lyt& gate_design)
        {
            for (const Lyt& stored_gate_design : randomly_designed_gate_layouts)
            {
                if (std::all_of(all_sidbs_in_canvas.cbegin(), all_sidbs_in_canvas.cend(), [&](const cell<Lyt>& sidb)
                                { return gate_design.get_cell_type(sidb) == stored_gate_design.get_cell_type(sidb); }))
                {
                    return true;
                }
            }

            return false;
        };

        uint64_t num_solutions_found = 0;

        const uint64_t max_number_of_solutions = std::min(params.maximum_number_of_solutions, stats.number_of_layouts);

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{
            static_cast<uint32_t>(max_number_of_solutions),
            fmt::format("[i] looking for {} random operational gate designs: ", max_number_of_solutions) +
                "|{0}|                            "};
#endif

        for (uint64_t z = 0u; z < params.available_threads; z++)
        {
            threads.emplace_back(
                [this, &num_solutions_found, &max_number_of_solutions, &check_if_gate_design_is_already_present,
                 &mutex_to_protect_designed_gate_layouts, &parameter, &randomly_designed_gate_layouts
#if (PROGRESS_BARS)
                 ,
                 &bar
#endif
            ]
                {
                    while (num_solutions_found < max_number_of_solutions)
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

                        if (const operational_assessment<Lyt>& assessment_results =
                                is_operational(result_lyt.value(), truth_table, params.operational_params,
                                               input_bdl_wires, output_bdl_wires);
                            assessment_results.status == operational_status::OPERATIONAL)
                        {
                            const std::lock_guard lock{mutex_to_protect_designed_gate_layouts};

                            if (check_if_gate_design_is_already_present(result_lyt.value()))
                            {
                                continue;
                            }

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

                            randomly_designed_gate_layouts.push_back(std::move(result_lyt.value()));

                            ++num_solutions_found;

#if (PROGRESS_BARS)
                            if (num_solutions_found < params.maximum_number_of_solutions)
                            {
                                // update the progress bar
                                bar(num_solutions_found);
                            }
#endif
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
     * A canvas combination is a combination of canvas positions as a vector of canvas position indices.
     */
    using canvas_combination = std::vector<uint64_t>;
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
     * This process filters the given candidates for an SiDB gate design for the ones that are operational under the
     * given truth table specification and operational conditions.
     *
     * @param candidate_combinations A vector of canvas combination candidates to obtain the operational gate designs from.
     * @return A vector of operational gate designs that were extracted from the given vector of candidate canvas combinations.
     */
    [[nodiscard]] std::vector<Lyt>
    extract_gate_designs(const std::vector<canvas_combination>& candidate_combinations) const noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        if (candidate_combinations.empty())
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

        const uint64_t max_number_of_solutions = std::min(params.maximum_number_of_solutions, stats.number_of_layouts);

        uint64_t num_solutions_found = 0;

        const std::size_t num_threads = std::min(params.available_threads, candidate_combinations.size());
        const std::size_t chunk_size =
            (candidate_combinations.size() + num_threads - 1) / num_threads;  // Ceiling division

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{
            static_cast<uint32_t>(params.termination_cond ==
                                          design_sidb_gates_params<Lyt>::termination_condition::OBTAINED_N_SOLUTIONS ?
                                      max_number_of_solutions :
                                      chunk_size),
            "[i] extracting operational gate designs... |{0}|" +
                fmt::format(" (number of candidates: {})                            ", candidate_combinations.size())};
#endif

        const auto add_combination_to_layout_and_check_operation = [this, &mutex_to_protect_designed_gate_layouts,
                                                                    &designed_gate_layouts, &num_solutions_found

#if (PROGRESS_BARS)
                                                                    ,
                                                                    &bar
#endif
        ](const canvas_combination& combination) noexcept
        {
            // canvas SiDBs are added to the skeleton
            const auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);

            if (const operational_assessment<Lyt>& assessment_results = is_operational(
                    layout_with_added_cells, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires);
                assessment_results.status == operational_status::OPERATIONAL)
            {
                const std::lock_guard lock_vector{mutex_to_protect_designed_gate_layouts};

                designed_gate_layouts.gate_layouts.emplace_back(std::move(layout_with_added_cells));

                if (designed_gate_layouts.simulation_results.has_value())
                {
                    designed_gate_layouts.simulation_results.value().push_back(
                        assessment_results.extract_simulation_results_per_input());
                }

                ++num_solutions_found;

#if (PROGRESS_BARS)
                if (params.termination_cond ==
                        design_sidb_gates_params<Lyt>::termination_condition::OBTAINED_N_SOLUTIONS &&
                    num_solutions_found < params.maximum_number_of_solutions)
                {
                    // update the progress bar
                    bar(num_solutions_found);
                }
#endif
            }
        };

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [this, i, chunk_size, &candidate_combinations, &add_combination_to_layout_and_check_operation,
                 &num_solutions_found, &max_number_of_solutions
#if (PROGRESS_BARS)
                 ,
                 &bar
#endif
            ]
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, candidate_combinations.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        if (params.termination_cond ==
                                design_sidb_gates_params<Lyt>::termination_condition::OBTAINED_N_SOLUTIONS &&
                            num_solutions_found >= max_number_of_solutions)
                        {
                            return;
                        }

                        add_combination_to_layout_and_check_operation(candidate_combinations[j]);

#if (PROGRESS_BARS)
                        if (params.termination_cond ==
                                design_sidb_gates_params<Lyt>::termination_condition::ALL_COMBINATIONS_ENUMERATED &&
                            i == 0)
                        {
                            // if all combinations are enumerated, update the progress bar only for the first thread
                            bar(j);
                        }
#endif
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
     * @return A vector containing the canvas combinations associated with valid gate candidates that were not pruned.
     */
    [[nodiscard]] std::vector<canvas_combination> run_pruning() noexcept
    {
        std::vector<canvas_combination> candidate_combinations = {};

        if (stats.number_of_layouts == 0)
        {
            return candidate_combinations;
        }

        candidate_combinations.reserve(stats.number_of_layouts);

        std::mutex mutex_to_protect_candidate_combinations{};  // used to control access to shared resources

        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_canvas_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        // Function to check validity and add layout to all_designs
        auto conduct_pruning_steps = [&](const canvas_combination& combination)
        {
            auto&& canvas_lyt = convert_canvas_cell_indices_to_layout(combination);

            if (!canvas_lyt.has_value())
            {
                return;
            }

            auto current_layout = skeleton_layout.clone();

            canvas_lyt.value().foreach_cell([&current_layout](const auto& c)
                                            { current_layout.assign_cell_type(c, Lyt::technology::cell_type::LOGIC); });

            charge_distribution_surface<Lyt> cds_canvas{
                std::move(canvas_lyt.value()), params.operational_params.simulation_parameters,
                sidb_charge_state::NEGATIVE, cds_configuration::CHARGE_LOCATION_ONLY};

            auto bii = bdl_input_iterator<Lyt>{current_layout, params.operational_params.input_bdl_iterator_params,
                                               input_bdl_wires};

            is_operational_impl<Lyt, TT> is_operational_impl{
                current_layout, truth_table, params.operational_params, input_bdl_wires, output_bdl_wires, cds_canvas};

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

            const std::lock_guard lock{mutex_to_protect_candidate_combinations};
            candidate_combinations.emplace_back(combination);
        };

        const std::size_t num_threads = std::min(params.available_threads, stats.number_of_layouts);
        const std::size_t chunk_size  = (stats.number_of_layouts + num_threads - 1) / num_threads;

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{
            static_cast<uint32_t>(chunk_size),
            "[i] performing QuickCell pruning... |{0}|" +
                fmt::format(" (number of candidates: {})                            ", candidate_combinations.size())};
#endif

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &conduct_pruning_steps, &all_combinations
#if (PROGRESS_BARS)
                 ,
                 &bar
#endif
            ]
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, all_combinations.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        conduct_pruning_steps(all_combinations.at(j));

#if (PROGRESS_BARS)
                        // update the progress bar only for the first thread
                        if (i == 0)
                        {
                            bar(j);
                        }
#endif
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

        return candidate_combinations;
    }
    /**
     * This function adds SiDBs (given by indices) to the skeleton layout that is returned afterward.
     *
     * @param cell_indices A vector of indices of cells to be added to the skeleton layout.
     * @return A copy of the original layout (`skeleton_layout`) with SiDB cells added at specified indices.
     */
    [[nodiscard]] Lyt skeleton_layout_with_canvas_sidbs(const std::vector<std::size_t>& cell_indices) const noexcept
    {
        Lyt lyt_copy{skeleton_layout.clone()};

        for (const auto i : cell_indices)
        {
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            if (lyt_copy.get_cell_type(all_sidbs_in_canvas[i]) == sidb_technology::cell_type::EMPTY)
            {
                lyt_copy.assign_cell_type(all_sidbs_in_canvas[i], sidb_technology::cell_type::LOGIC);
            }
        }

        return lyt_copy;
    }
    /**
     * This function generates canvas SiDB layouts.
     *
     * @param cell_indices A vector of indices of cells to be added to the skeleton layout.
     * @return An SiDB cell-level layout consisting of canvas SiDBs.
     */
    [[nodiscard]] std::optional<Lyt>
    convert_canvas_cell_indices_to_layout(const std::vector<std::size_t>& cell_indices) const noexcept
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
                    return std::nullopt;
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
 * 1. In the initial step, all possible distributions of `number_of_canvas_sidbs` SiDBs within a given canvas are
 * exhaustively determined. This ensures exhaustive coverage of every potential arrangement of `number_of_canvas_sidbs`
 * SiDBs across the canvas.
 * 2. The calculated SiDB distributions are then incorporated into the skeleton, resulting in the generation of distinct
 * SiDB layouts.
 * 3. The generated SiDB layouts then undergo an extensive simulation process. All input combinations possible for the
 * given Boolean function are used to verify if the logic is fulfilled.
 *
 * The `random design` is composed of four steps:
 * 1. A specified number of canvas SiDBs (`number_of_canvas_sidbs`) are randomly added to the skeleton layout.
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

    if (params.design_mode == design_sidb_gates_params<Lyt>::design_sidb_gates_mode::EXHAUSTIVE)
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
