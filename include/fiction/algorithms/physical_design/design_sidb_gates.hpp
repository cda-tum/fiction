//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <fmt/format.h>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct contains parameters and settings to design SiDB gates.
 *
 * @tparam CellType Cell type.
 *
 */
template <typename CellType>
struct design_sidb_gates_params
{
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
        AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        /**
         * Gate layouts are designed randomly.
         */
        RANDOM
    };
    /**
     * Parameters for the `is_operational` function.
     */
    is_operational_params operational_params{};
    /**
     * Gate design mode.
     */
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<CellType, CellType> canvas{};
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
};

/**
 * Statistics for the design of SiDB gates.
 */
struct design_sidb_gates_stats
{
    /**
     * The total runtime of SiDB gate design process.
     */
    mockturtle::stopwatch<>::duration time_total{0};
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
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& spec,
                           const design_sidb_gates_params<cell<Lyt>>& ps, design_sidb_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{spec},
            params{ps},
            all_sidbs_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)},
            stats{st},
            input_bdl_wires{detect_bdl_wires(skeleton_layout,
                                             params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                             bdl_wire_selection::INPUT)},
            output_bdl_wires{detect_bdl_wires(skeleton_layout,
                                              params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                              bdl_wire_selection::OUTPUT)},
            number_of_input_wires{input_bdl_wires.size()},
            number_of_output_wires{output_bdl_wires.size()},
            all_canvas_layouts{determine_all_possible_canvas_layouts()}
    {
        stats.number_of_layouts = all_canvas_layouts.size();
        stats.sim_engine        = params.operational_params.sim_engine;
    }

    /**
     * Design gates by using the *Automatic Exhaustive Gate Desginer*. This algorithm was proposed in \"Minimal
     * Design of SiDB Gates: An Optimal Basis for Circuits Based on Silicon Dangling Bonds\" by J. Drewniok, M. Walter,
     * and R. Wille in NANOARCH 2023 (https://dl.acm.org/doi/10.1145/3611315.3633241).
     *
     * This function adds each cell combination to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_automatic_exhaustive_gate_designer() const noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};

        if (all_combinations.empty())
        {
            return designed_gate_layouts;
        }

        std::unordered_set<coordinate<Lyt>> sidbs_affected_by_defects = {};

        // used to collect all SiDBs that are affected due to neutrally charged defects.
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            sidbs_affected_by_defects = skeleton_layout.all_affected_sidbs(std::make_pair(0, 0));
        }

        std::mutex mutex_to_protect_designed_gate_layouts{};

        std::atomic<bool> solution_found = false;

        // Shuffle the combinations before dividing them among threads
        std::shuffle(all_combinations.begin(), all_combinations.end(),
                     std::default_random_engine(std::random_device{}()));

        const auto add_combination_to_layout_and_check_operation = [this, &mutex_to_protect_designed_gate_layouts,
                                                                    &designed_gate_layouts,
                                                                    &solution_found](const auto& combination) noexcept
        {
            // canvas SiDBs are added to the skeleton
            const auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);

            if (const auto [status, sim_calls] =
                    is_operational(layout_with_added_cells, truth_table, params.operational_params,
                                   std::optional{input_bdl_wires}, std::optional{output_bdl_wires});
                status == operational_status::OPERATIONAL)
            {
                {
                    const std::lock_guard lock_vector{mutex_to_protect_designed_gate_layouts};
                    designed_gate_layouts.push_back(layout_with_added_cells);
                }

                solution_found = true;
            }

            if (solution_found && (params.termination_cond ==
                                   design_sidb_gates_params<cell<Lyt>>::termination_condition::AFTER_FIRST_SOLUTION))
            {
                return;
            }
        };

        const std::size_t number_of_used_threads =
            std::min(static_cast<std::size_t>(num_threads), all_combinations.size());

        const std::size_t chunk_size =
            (all_combinations.size() + number_of_used_threads - 1) / number_of_used_threads;  // Ceiling division

        std::vector<std::thread> threads{};
        threads.reserve(number_of_used_threads);

        for (std::size_t i = 0; i < number_of_used_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &all_combinations, &add_combination_to_layout_and_check_operation, &solution_found,
                 this]()
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, all_combinations.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        if (solution_found &&
                            (params.termination_cond ==
                             design_sidb_gates_params<cell<Lyt>>::termination_condition::AFTER_FIRST_SOLUTION))
                        {
                            return;
                        }
                        add_combination_to_layout_and_check_operation(all_combinations[j]);
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

        return designed_gate_layouts;
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
        threads.reserve(num_threads);

        std::mutex mutex_to_protect_designed_gate_layouts{};  // used to control access to shared resources

        std::atomic<bool> gate_layout_is_found(false);

        for (uint64_t z = 0u; z < num_threads; z++)
        {
            threads.emplace_back(
                [this, &gate_layout_is_found, &mutex_to_protect_designed_gate_layouts, &parameter,
                 &randomly_designed_gate_layouts]
                {
                    while (!gate_layout_is_found)
                    {
                        auto result_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter);
                        if constexpr (has_get_sidb_defect_v<Lyt>)
                        {
                            result_lyt.foreach_sidb_defect(
                                [&result_lyt](const auto& cd)
                                {
                                    if (is_neutrally_charged_defect(cd.second))
                                    {
                                        result_lyt.assign_sidb_defect(cd.first, sidb_defect{sidb_defect_type::NONE});
                                    }
                                });
                        }
                        if (const auto [status, sim_calls] =
                                is_operational(result_lyt, truth_table, params.operational_params,
                                               std::optional{input_bdl_wires}, std::optional{output_bdl_wires});
                            status == operational_status::OPERATIONAL)
                        {
                            const std::lock_guard lock{mutex_to_protect_designed_gate_layouts};
                            if constexpr (has_get_sidb_defect_v<Lyt>)
                            {
                                skeleton_layout.foreach_sidb_defect(
                                    [&result_lyt](const auto& cd)
                                    {
                                        if (is_neutrally_charged_defect(cd.second))
                                        {
                                            result_lyt.assign_sidb_defect(cd.first, cd.second);
                                        }
                                    });
                            }

                            randomly_designed_gate_layouts.push_back(result_lyt);
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

    /**
     * Design Standard Cells/gates by using the *QuickCell* algorithm.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_quickcell() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        const auto            gate_candidates = run_pruning();

        stats.number_of_layouts_after_first_pruning =
            all_canvas_layouts.size() - number_of_discarded_layouts_at_first_pruning;
        stats.number_of_layouts_after_second_pruning =
            stats.number_of_layouts_after_first_pruning - number_of_discarded_layouts_at_second_pruning;
        stats.number_of_layouts_after_third_pruning =
            stats.number_of_layouts_after_second_pruning - number_of_discarded_layouts_at_third_pruning;

        std::vector<Lyt> gate_layouts{};
        gate_layouts.reserve(gate_candidates.size());

        if (gate_candidates.empty())
        {
            return gate_layouts;
        }

        std::mutex mutex_to_protect_gate_designs{};

        gate_layouts.reserve(gate_candidates.size());

        const std::size_t number_of_threads =
            std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), gate_candidates.size());
        const std::size_t chunk_size = (gate_candidates.size() + num_threads - 1) / num_threads;  // Ceiling division

        std::vector<std::thread> threads;
        threads.reserve(number_of_threads);

        std::atomic<bool> gate_design_found = false;

        const auto check_operational_status =
            [this, &gate_layouts, &mutex_to_protect_gate_designs, &gate_design_found](const auto& candidate) noexcept
        {
            // Early exit if a solution is found and only the first solution is required
            if (gate_design_found && (params.termination_cond ==
                                      design_sidb_gates_params<cell<Lyt>>::termination_condition::AFTER_FIRST_SOLUTION))
            {
                return;
            }
            if (const auto [status, sim_calls] =
                    is_operational(candidate, truth_table, params.operational_params, std::optional{input_bdl_wires},
                                   std::optional{output_bdl_wires});
                status == operational_status::OPERATIONAL)
            {
                // Lock and update shared resources
                {
                    const std::lock_guard lock{mutex_to_protect_gate_designs};
                    gate_layouts.push_back(candidate);
                }
                gate_design_found = true;  // Notify all threads that a solution has been found
            }
        };

        for (std::size_t i = 0; i < number_of_threads; ++i)
        {
            threads.emplace_back(
                [this, i, chunk_size, &gate_candidates, &check_operational_status, &gate_design_found]()
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, gate_candidates.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        if (gate_design_found &&
                            (params.termination_cond ==
                             design_sidb_gates_params<cell<Lyt>>::termination_condition::AFTER_FIRST_SOLUTION))
                        {
                            return;
                        }

                        check_operational_status(gate_candidates[j]);
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

        return gate_layouts;
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
    const std::vector<TT> truth_table;
    /**
     * Parameters for the *SiDB Gate Designer*.
     */
    const design_sidb_gates_params<cell<Lyt>>& params;
    /**
     * All cells within the canvas.
     */
    std::vector<typename Lyt::cell> all_sidbs_in_canvas;
    /**
     * Number of threads to be used for parallel execution.
     */
    const std::size_t num_threads{std::thread::hardware_concurrency()};
    /**
     * The statistics of the gate design.
     */
    design_sidb_gates_stats& stats;
    /**
     * Input BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * Output BDL wires.
     */
    const std::vector<bdl_wire<Lyt>> output_bdl_wires;
    /**
     * Number of input BDL wires.
     */
    const std::size_t number_of_input_wires;
    /**
     * Number of output BDL wires.
     */
    const std::size_t number_of_output_wires;
    /**
     * All Canvas SiDB layout (without I/O pins).
     */
    const std::vector<Lyt> all_canvas_layouts{};
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
     * This function assigns the charge states of the input wires in the layout according to the provided input pattern
     * index. It performs the following steps:
     * - For `NORTH-SOUTH` port wires, if the corresponding bit in the input pattern is set, assigns `NEUTRAL`
     * charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     * - For `NORTH-SOUTH` port wires, if the corresponding bit in the input pattern is not set, assigns `NEGATIVE`
     *   charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     * - For `SOUTH-NORTH` port wires, if the corresponding bit in the input pattern is set, assigns `NEGATIVE`
     * charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     * - For `SOUTH-NORTH` port wires, if the corresponding bit in the input pattern is not set, assigns `NEUTRAL`
     *   charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     *
     * @param layout The charge distribution surface layout to be modified.
     * @param current_input_index The index representing the current input pattern.
     */
    void
    set_charge_distribution_of_input_wires_based_on_input_pattern(charge_distribution_surface<Lyt>& layout,
                                                                  const uint64_t current_input_index) const noexcept
    {
        layout.assign_all_charge_states(sidb_charge_state::NEGATIVE, charge_index_mode::KEEP_CHARGE_INDEX);

        for (auto i = 0u; i < number_of_input_wires; i++)
        {
            if (input_bdl_wires[number_of_input_wires - 1 - i].port.dir == port_direction::SOUTH ||
                input_bdl_wires[number_of_input_wires - 1 - i].port.dir == port_direction::EAST)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[number_of_input_wires - 1 - i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
        }
    }

    /**
     * This function assigns the charge states of the input wires in the layout according to the provided input pattern
     * index. It performs the following steps:
     *    - For `NORTH-SOUTH` port wires, if the corresponding bit in the input pattern is set, assigns `NEUTRAL`
     * charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     *    - For `NORTH-SOUTH` port wires, if the corresponding bit in the input pattern is not set, assigns
     * `NEGATIVE` charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     *    - For `SOUTH-NORTH` port wires, if the corresponding bit in the input pattern is set, assigns `NEGATIVE`
     * charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     *    - For `SOUTH-NORTH` port wires, if the corresponding bit in the input pattern is not set, assigns
     * `NEUTRAL` charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     *
     * @param layout The charge distribution surface layout to be modified.
     * @param current_input_index The index representing the current input pattern.
     * @return void
     */
    void set_charge_distribution_of_output_wires_based_on_output_index(charge_distribution_surface<Lyt>& layout,
                                                                       const uint64_t output_wire_index) const noexcept
    {
        for (auto i = 0u; i < number_of_output_wires; i++)
        {
            if (output_bdl_wires[i].port.dir == port_direction::SOUTH ||
                output_bdl_wires[i].port.dir == port_direction::EAST)
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
        }
    }

    /**
     * This function calculates the output index for a given input index by evaluating the truth table.
     *
     * @param current_input_index The index representing the current input pattern.
     * @return The output index derived from the truth table for the given input index.
     */
    [[nodiscard]] uint64_t determine_output_index_of_output(const uint64_t current_input_index) const noexcept
    {
        std::bitset<64> bits{};
        for (auto i = 0u; i < number_of_output_wires; i++)
        {
            bits[i] = kitty::get_bit(truth_table[i], current_input_index);
        }
        return bits.to_ulong();
    }

    /**
     * This function assigns the charge states of the output wires in the layout according to the values in the truth
     * table for the provided input pattern index.
     *
     * @param layout The charge distribution surface layout to be modified.
     * @param input_index The index representing the current input pattern.
     */
    void set_charge_distribution_of_output_wires_based_on_truth_table(charge_distribution_surface<Lyt>& layout,
                                                                      const uint64_t input_index) const noexcept
    {
        for (auto i = 0u; i < number_of_output_wires; i++)
        {
            if (output_bdl_wires[i].port.dir == port_direction::SOUTH ||
                output_bdl_wires[i].port.dir == port_direction::EAST)
            {
                if (kitty::get_bit(truth_table[i], input_index))
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
            else
            {
                if (kitty::get_bit(truth_table[i], input_index))
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i].pairs)
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE,
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                    }
                }
            }
        }
    }

    /**
     * This function determines if there is a charge distribution of the canvas SiDBs for which the charge distribution
     * of the whole layout is physically valid.
     *
     * @param cds_layout The charge distribution surface layout to be evaluated.
     * @param cds_canvas The charge distribution surface of the canvas SiDBs. All possible configurations are enumerated
     * @return The minimum energy value if a physically valid configuration is found, `std::nullopt`
     * otherwise.
     */
    [[nodiscard]] std::optional<double>
    is_physical_validity_feasible(charge_distribution_surface<Lyt>& cds_layout,
                                  charge_distribution_surface<Lyt>& cds_canvas) const noexcept
    {
        auto min_energy = std::numeric_limits<double>::infinity();

        uint64_t canvas_charge_index = 0;
        cds_canvas.assign_charge_index(canvas_charge_index);

        while (cds_canvas.get_charge_index_and_base().first <= cds_canvas.get_max_charge_index())
        {
            cds_canvas.foreach_cell(
                [&](const auto& c)
                {
                    cds_layout.assign_charge_state(c, cds_canvas.get_charge_state(c),
                                                   charge_index_mode::KEEP_CHARGE_INDEX);
                });
            cds_layout.update_after_charge_change(dependent_cell_mode::VARIABLE,
                                                  energy_calculation::KEEP_OLD_ENERGY_VALUE);

            if (cds_layout.is_physically_valid())
            {
                cds_layout.recompute_system_energy();
                if (cds_layout.get_system_energy() + physical_constants::POP_STABILITY_ERR < min_energy)
                {
                    min_energy = cds_layout.get_system_energy();
                }
            }

            if (cds_canvas.get_charge_index_and_base().first == cds_canvas.get_max_charge_index())
            {
                break;
            }

            canvas_charge_index++;
            cds_canvas.assign_charge_index(canvas_charge_index);
        }

        if (min_energy < std::numeric_limits<double>::infinity())
        {
            return min_energy;
        }

        return std::nullopt;
    }

    /**
     * This function iterates through various input patterns and output wire indices to determine if any configuration
     * results in a physically valid layout with energy below the given energy value, indicating I/O signal instability.
     *
     * @param cds_layout The charge distribution surface layout to be modified and checked.
     * @param cds_canvas The charge distribution of the canvas SiDBs.
     * @param max_input_pattern_index The maximum index for input pattern
     * @param input_pattern The specific input pattern for which the stability check is conducted.
     * @param logical_correct_output_pattern The expected correct output pattern for the given input.
     * @param minimal_energy_of_physically_valid_layout The minimum energy threshold below which the layout is
     * considered unstable.
     * @return `true` if the I/O signal is unstable, `false` otherwise.
     */
    [[nodiscard]] bool is_io_signal_unstable(charge_distribution_surface<Lyt>& cds_layout,
                                             charge_distribution_surface<Lyt>& cds_canvas,
                                             const uint64_t max_input_pattern_index, const uint64_t input_pattern,
                                             const uint64_t logical_correct_output_pattern,
                                             const double   minimal_energy_of_physically_valid_layout) const noexcept
    {
        for (auto kink_states_input = 0u; kink_states_input < max_input_pattern_index; ++kink_states_input)
        {
            for (auto output_wire_index = 0u; output_wire_index < std::pow(2, number_of_output_wires);
                 output_wire_index++)
            {
                if (output_wire_index == logical_correct_output_pattern && kink_states_input == input_pattern)
                {
                    continue;
                }

                set_charge_distribution_of_input_wires_based_on_input_pattern(cds_layout, kink_states_input);
                set_charge_distribution_of_output_wires_based_on_output_index(cds_layout, output_wire_index);

                const auto physical_validity = is_physical_validity_feasible(cds_layout, cds_canvas);

                if (physical_validity.has_value())
                {
                    if (physical_validity.value() + physical_constants::POP_STABILITY_ERR <
                        minimal_energy_of_physically_valid_layout)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    /**
     * This function evaluates whether the given layout can be discarded since it cannot implement the given Boolean
     * function. The pruning is subdivided into three single pruning steps: (1) discarding SiDB layouts with potentially
     * positively charged SiDBs, (2) utilizing an efficient method to identify and discard SiDB layouts that do not
     * satisfy physical model constraints under the I/O pin conditions required for the desired Boolean function, and
     * (3) detecting I/O signal instability.
     *
     * @param current_layout The layout being evaluated for pruning.
     * @param canvas_lyt The canvas layout comprising of the canvas SiDBs.
     * @param dependent_cell A dependent-cell of the canvas SiDBs.
     * @return `true` if the current layout can be pruned. `false` otherwise, which means that the layout is a candidate
     * to be a valid gate implementation. Physical simulation is required as a second step to conduct the final
     * validation.
     */
    [[nodiscard]] bool layout_can_be_pruned(const Lyt& current_layout, const Lyt& canvas_lyt,
                                            const cell<Lyt>& dependent_cell) noexcept
    {
        charge_distribution_surface<Lyt> cds_canvas{canvas_lyt, params.operational_params.simulation_parameters,
                                                    sidb_charge_state::NEGATIVE,
                                                    cds_configuration::CHARGE_LOCATION_ONLY};

        cds_canvas.assign_dependent_cell(dependent_cell);

        auto bii = bdl_input_iterator<Lyt>{current_layout, params.operational_params.input_bdl_iterator_params,
                                           input_bdl_wires};

        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            charge_distribution_surface cds_layout{*bii, params.operational_params.simulation_parameters};

            if (can_positive_charges_occur(cds_layout, params.operational_params.simulation_parameters))
            {
                number_of_discarded_layouts_at_first_pruning++;
                return true;
            }

            cds_layout.assign_dependent_cell(dependent_cell);
            set_charge_distribution_of_input_wires_based_on_input_pattern(cds_layout, i);
            set_charge_distribution_of_output_wires_based_on_truth_table(cds_layout, i);

            const auto physical_validity = is_physical_validity_feasible(cds_layout, cds_canvas);

            if (physical_validity.has_value())
            {
                const auto output_index = determine_output_index_of_output(i);
                if (is_io_signal_unstable(cds_layout, cds_canvas, truth_table.front().num_bits(), i, output_index,
                                          physical_validity.value()))
                {
                    number_of_discarded_layouts_at_third_pruning++;
                    return true;
                };
            }
            else
            {
                number_of_discarded_layouts_at_second_pruning++;
                return true;
            }
        }

        return false;
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
        std::vector<Lyt> gate_candidate = {};

        if (all_canvas_layouts.empty())
        {
            return gate_candidate;
        }

        std::mutex mutex_to_protect_gate_candidates{};  // used to control access to shared resources

        // Function to check validity and add layout to all_designs
        auto conduct_pruning_steps = [&](const Lyt& canvas_lyt)
        {
            auto current_layout = skeleton_layout.clone();

            cell<Lyt> dependent_cell{};
            canvas_lyt.foreach_cell(
                [&](const auto& c)
                {
                    current_layout.assign_cell_type(c, Lyt::technology::cell_type::LOGIC);
                    dependent_cell = c;
                });

            if (!layout_can_be_pruned(current_layout, canvas_lyt, dependent_cell))
            {
                const std::lock_guard lock{mutex_to_protect_gate_candidates};
                gate_candidate.push_back(current_layout);
            }
        };

        gate_candidate.reserve(all_canvas_layouts.size());

        const std::size_t number_of_threads =
            std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), all_canvas_layouts.size());
        const std::size_t chunk_size = (all_canvas_layouts.size() + number_of_threads - 1) / number_of_threads;

        std::vector<std::thread> threads{};
        threads.reserve(number_of_threads);

        for (std::size_t i = 0; i < number_of_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, this, &conduct_pruning_steps]()
                {
                    const std::size_t start_index = i * chunk_size;
                    const std::size_t end_index   = std::min(start_index + chunk_size, all_canvas_layouts.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        conduct_pruning_steps(all_canvas_layouts[j]);
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

        return gate_candidate;
    }

    /**
     * This function calculates all combinations of distributing a given number of SiDBs across a specified number of
     * positions in the canvas. Each combination is then used to create a gate layout candidate.
     *
     * @return A vector containing all possible gate layouts generated from the combinations.
     */
    [[nodiscard]] std::vector<Lyt> determine_all_possible_canvas_layouts() const noexcept
    {
        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};
        designed_gate_layouts.reserve(all_combinations.size());

        const auto add_cell_combination_to_layout = [this, &designed_gate_layouts](const auto& combination) noexcept
        {
            const auto layout_with_added_cells = convert_canvas_cell_indices_to_layout(combination);
            designed_gate_layouts.push_back(layout_with_added_cells);
        };

        for (const auto& combination : all_combinations)
        {
            add_cell_combination_to_layout(combination);
        }

        return designed_gate_layouts;
    }

    /**
     * This function adds SiDBs (given by indices) to the skeleton layout that is returned afterwards.
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
                if (skeleton_layout.get_sidb_defect(all_sidbs_in_canvas[i]).type != sidb_defect_type::NONE)
                {
                    continue;
                }
            }
            lyt.assign_cell_type(all_sidbs_in_canvas[i], sidb_technology::cell_type::LOGIC);
        }

        // the skeleton can already exhibit some canvas SiDBs (partially filled canvas)
        skeleton_layout.foreach_cell(
            [&](const auto& c)
            {
                if (skeleton_layout.get_cell_type(c) == sidb_technology::cell_type::LOGIC)
                {
                    lyt.assign_cell_type(c, Lyt::technology::cell_type::LOGIC);
                }
            });

        return lyt;
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
                                                 const design_sidb_gates_params<cell<Lyt>>& params = {},
                                                 design_sidb_gates_stats*                   stats  = nullptr) noexcept
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

    if (params.design_mode ==
        design_sidb_gates_params<cell<Lyt>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER)
    {
        result = p.run_automatic_exhaustive_gate_designer();
    }

    else if (params.design_mode == design_sidb_gates_params<cell<Lyt>>::design_sidb_gates_mode::QUICKCELL)
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
