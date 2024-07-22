//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <limits>
#include <mutex>
#include <optional>
#include <thread>
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
     * Selector for the available design approaches.
     */
    enum class design_sidb_gates_mode
    {
        /**
         * Gates are designed by using *QuickCell*.
         */
        QUICKCELL,
        /**
         * Gates are designed by using the *AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER*.
         */
        AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        /**
         * Gate layouts are designed randomly.
         */
        RANDOM
    };
    /**
     * Parameters to check the operation of the designed gate.
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
     * @param tt Expected Boolean function of the layout given as a multi-output truth table.
     * @param ps Parameters and settings for the gate designer.
     */
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& tt,
                           const design_sidb_gates_params<cell<Lyt>>& ps, design_sidb_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{tt},
            params{ps},
            all_sidbs_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)},
            stats{st}
    {
        initialize();
    }

    /**
     * Design gates by using the *Automatic Exhaustive Gate Desginer*. This algorithm which was proposed in \"Minimal
     * Design of SiDB Gates: An Optimal Basis for Circuits Based on Silicon Dangling Bonds\" by J. Drewniok, M. Walter,
     * and R. Wille in NANOARCH 2023 (https://dl.acm.org/doi/10.1145/3611315.3633241).
     *
     * This function adds each cell combination to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_automatic_exhaustive_gate_designer() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};

        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};

        std::mutex mutex_to_protect_designer_gate_layouts;  // Mutex for protecting shared resources

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_designer_gate_layouts, &designed_gate_layouts](const auto& combination) noexcept
        {
            auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);
            if (const auto [status, sim_calls] =
                    is_operational(layout_with_added_cells, truth_table, params.operational_params);
                status == operational_status::OPERATIONAL)
            {
                const std::lock_guard lock_vector{mutex_to_protect_designer_gate_layouts};  // Lock the mutex
                designed_gate_layouts.push_back(layout_with_added_cells);
            }
        };

        const std::size_t num_threads =
            std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), all_combinations.size());
        const std::size_t chunk_size = (all_combinations.size() + num_threads - 1) / num_threads;  // Ceiling division

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &all_combinations, &add_combination_to_layout_and_check_operation]()
                {
                    std::size_t start_index = i * chunk_size;
                    std::size_t end_index   = std::min(start_index + chunk_size, all_combinations.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        add_combination_to_layout_and_check_operation(all_combinations[j]);
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
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
    [[nodiscard]] std::vector<Lyt> run_random_design() noexcept
    {
        std::vector<Lyt> randomly_designed_gate_layouts = {};

        const generate_random_sidb_layout_params<cell<Lyt>> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::FORBIDDEN};

        const std::size_t        num_threads = std::thread::hardware_concurrency();
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
                        const auto result_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter);
                        if (const auto [status, sim_calls] =
                                is_operational(result_lyt, truth_table, params.operational_params);
                            status == operational_status::OPERATIONAL)
                        {
                            const std::lock_guard lock{mutex_to_protect_designed_gate_layouts};
                            randomly_designed_gate_layouts.push_back(result_lyt);
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
     * Design Standard Cells/gates by using the *QuickCell* algorithm
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_quickcell() noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        const auto            gate_candidates = run_pruning();

        std::vector<Lyt> gate_layouts{};
        gate_layouts.reserve(gate_candidates.size());

        const std::size_t num_threads =
            std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), gate_candidates.size());
        const std::size_t chunk_size = (gate_candidates.size() + num_threads - 1) / num_threads;  // Ceiling division

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        const auto check_operational_status = [this, &gate_layouts](const auto& candidate) noexcept
        {
            if (const auto [status, sim_calls] = is_operational(candidate, truth_table, params.operational_params);
                status == operational_status::OPERATIONAL)
            {
                gate_layouts.push_back(candidate);
            }
        };

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, &gate_candidates, &check_operational_status]()
                {
                    std::size_t start_index = i * chunk_size;
                    std::size_t end_index   = std::min(start_index + chunk_size, gate_candidates.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        check_operational_status(gate_candidates[j]);
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        return gate_layouts;
    }

  private:
    /**
     * The skeleton layout serves as a starting layout to which SiDBs are added to create unique SiDB layouts and, if
     * possible, working gates. It defines input and output wires.
     */
    Lyt skeleton_layout;
    /**
     * Truth table of the given gate.
     */
    std::vector<TT> truth_table;
    /**
     * Parameters for the *SiDB Gate Designer*.
     */
    const design_sidb_gates_params<cell<Lyt>>& params;
    /**
     * All cells within the canvas.
     */
    const std::vector<typename Lyt::cell> all_sidbs_in_canvas;
    /**
     * Input BDL wires.
     */
    std::vector<bdl_wire<Lyt>> input_bdl_wires;
    /**
     * Output BDL wires.
     */
    std::vector<bdl_wire<Lyt>> output_bdl_wires;
    /**
     * Directions of the input wires.
     */
    std::vector<bdl_wire_direction> input_bdl_wire_directions{};
    /**
     * Directions of the output wires.
     */
    std::vector<bdl_wire_direction> output_bdl_wire_directions{};

    std::vector<Lyt> all_canvas_layouts{};
    /**
     * The statistics of the gate design.
     */
    design_sidb_gates_stats& stats;

    /**
     * This function performs the following steps to initialize the necessary components for the layout evaluation:
     * 1. Detects the input and output BDL wires in the skeleton layout using the specified BDL wire parameters.
     * 2. Determines the directions of the input BDL wires and stores them in the input BDL wire directions container.
     * 3. Determines the directions of the output BDL wires and stores them in the output BDL wire directions container.
     * 4. Determines and stores all possible canvas layouts.
     *
     * @tparam Lyt The type representing the layout.
     */
    void initialize() noexcept
    {
        input_bdl_wires =
            detect_bdl_wires(skeleton_layout, params.operational_params.bdl_wire_params, bdl_wire_selection::INPUT);
        output_bdl_wires =
            detect_bdl_wires(skeleton_layout, params.operational_params.bdl_wire_params, bdl_wire_selection::OUTPUT);

        input_bdl_wire_directions.reserve(input_bdl_wires.size());

        std::transform(input_bdl_wires.cbegin(), input_bdl_wires.cend(), std::back_inserter(input_bdl_wire_directions),
                       [](const auto& wire) { return determine_wire_direction<Lyt>(wire); });

        output_bdl_wire_directions.reserve(output_bdl_wires.size());

        std::transform(output_bdl_wires.cbegin(), output_bdl_wires.cend(),
                       std::back_inserter(output_bdl_wire_directions),
                       [](const auto& wire) { return determine_wire_direction<Lyt>(wire); });

        all_canvas_layouts = determine_all_possible_canvas_layouts();
    }

    void
    set_charge_distribution_of_input_wires_based_on_input_pattern(charge_distribution_surface<Lyt>& layout,
                                                                  const uint64_t current_input_index) const noexcept
    {
        layout.assign_all_charge_states(sidb_charge_state::NEGATIVE, false);

        for (auto i = 0u; i < input_bdl_wires.size(); i++)
        {
            if (input_bdl_wire_directions[input_bdl_wires.size() - 1 - i] == bdl_wire_direction::NORTH_SOUTH)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i])
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i])
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
            }
            else if (input_bdl_wire_directions[input_bdl_wires.size() - 1 - i] == bdl_wire_direction::SOUTH_NORTH)
            {
                if ((current_input_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i])
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
                else
                {
                    for (const auto& bdl : input_bdl_wires[input_bdl_wires.size() - 1 - i])
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
            }
        }
    }
    /**
     * This function assigns the charge states of the input wires in the layout according to the provided input pattern
     * index. It performs the following steps:
     *    - For `NORTH-SOUTH` direction wires, if the corresponding bit in the input pattern is set, assigns `NEUTRAL`
     * charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     *    - For `NORTH-SOUTH` direction wires, if the corresponding bit in the input pattern is not set, assigns
     * `NEGATIVE` charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     *    - For `SOUTH-NORTH` direction wires, if the corresponding bit in the input pattern is set, assigns `NEGATIVE`
     * charge to the upper part and `NEUTRAL` charge to the lower part of the BDLs of the wire.
     *    - For `SOUTH-NORTH` direction wires, if the corresponding bit in the input pattern is not set, assigns
     * `NEUTRAL` charge to the upper part and `NEGATIVE` charge to the lower part of the BDLs of the wire.
     *
     * @tparam Lyt The type representing the layout.
     * @param layout The charge distribution surface layout to be modified.
     * @param current_input_index The index representing the current input pattern.
     * @return void
     */
    void set_charge_distribution_of_output_wires_based_on_output_index(charge_distribution_surface<Lyt>& layout,
                                                                       const uint64_t output_wire_index) const noexcept
    {
        for (auto i = 0u; i < output_bdl_wires.size(); i++)
        {
            if (output_bdl_wire_directions[i] == bdl_wire_direction::NORTH_SOUTH)
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
            }
            else if (output_bdl_wire_directions[i] == bdl_wire_direction::SOUTH_NORTH)
            {
                if ((output_wire_index & (uint64_t{1ull} << i)) != 0ull)
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        if (bdl.type == sidb_technology::INPUT)
                        {
                            continue;
                        }
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
            }
        }
    }

    /**
     * This function calculates the output index for a given input index by evaluating the truth table.
     * The result is a bitset where each bit represents the output state for a corresponding BDL wire.
     *
     * @param current_input_index The index representing the current input pattern.
     * @return uint64_t The output index derived from the truth table for the given input index.
     */
    [[nodiscard]] uint64_t determine_output_index_of_output(const uint64_t current_input_index) const noexcept
    {
        std::bitset<64> bits{};
        for (auto i = 0u; i < output_bdl_wires.size(); i++)
        {
            bits[i] = kitty::get_bit(truth_table[i], current_input_index);
        }
        return bits.to_ulong();
    }
    /**
     * This function assigns the charge states of the output wires in the layout according to the values in the truth
     * table for the provided input pattern index.
     *
     * @tparam Lyt The type representing the layout.
     * @param layout The charge distribution surface layout to be modified.
     * @param input_index The index representing the current input pattern.
     * @return void
     */
    void set_charge_distribution_of_output_wires_based_on_truth_table(charge_distribution_surface<Lyt>& layout,
                                                                      const uint64_t input_index) const noexcept
    {
        for (auto i = 0u; i < output_bdl_wires.size(); i++)
        {
            if (output_bdl_wire_directions[i] == bdl_wire_direction::NORTH_SOUTH)
            {
                if (kitty::get_bit(truth_table[i], input_index))
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
            }
            else if (output_bdl_wire_directions[i] == bdl_wire_direction::SOUTH_NORTH)
            {
                if (kitty::get_bit(truth_table[i], input_index))
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEGATIVE, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEUTRAL, false);
                    }
                }
                else
                {
                    for (const auto& bdl : output_bdl_wires[i])
                    {
                        layout.assign_charge_state(bdl.upper, sidb_charge_state::NEUTRAL, false);
                        layout.assign_charge_state(bdl.lower, sidb_charge_state::NEGATIVE, false);
                    }
                }
            }
        }
    }

    /**
     * This function determines if there is a charge distribution of the canvas SiDBs for which the charge distribution
     * of the whole layout is physically valid
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @param cds_layout The charge distribution surface layout to be evaluated.
     * @param cds_canvas The charge distribution surface of the canvas SiDBs. All possible configurations are enumerated
     * @return The minimum energy value if a physically valid configuration is found, `std::nullopt`
     * otherwise.
     *
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
            cds_canvas.foreach_cell([&](const auto& c)
                                    { cds_layout.assign_charge_state(c, cds_canvas.get_charge_state(c), false); });
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
     * @tparam Lyt SiDB-cell level layout type.
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
            for (auto output_wire_index = 0u; output_wire_index < std::pow(2, output_bdl_wires.size());
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
                                            const cell<Lyt>& dependent_cell) const noexcept
    {
        charge_distribution_surface<Lyt> cds_canvas{canvas_lyt, params.operational_params.simulation_parameters,
                                                    sidb_charge_state::NEGATIVE,
                                                    cds_configuration::ONLY_CHARGE_LOCATION};

        cds_canvas.assign_dependent_cell(dependent_cell);

        auto bii = bdl_input_iterator<Lyt>{current_layout, params.operational_params.bdl_wire_params, input_bdl_wires,
                                           input_bdl_wire_directions};

        for (auto i = 0u; i < truth_table.front().num_bits(); ++i, ++bii)
        {
            charge_distribution_surface cds_layout{*bii, params.operational_params.simulation_parameters};

            if (can_positive_charges_occur(cds_layout, params.operational_params.simulation_parameters))
            {
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
                    return true;
                };
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    /**
     * This function processes each layout to determine if it represents a valid gate implementation or if it can be
     * pruned. It leverages multi-threading to accelerate the evaluation and ensures thread-safe access to shared
     * resources.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @return A vector containing the valid gate candidates that were not pruned.
     */
    [[nodiscard]] std::vector<Lyt> run_pruning() noexcept
    {
        std::vector<Lyt> gate_candidate = {};
        gate_candidate.reserve(all_canvas_layouts.size());

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

        const std::size_t num_threads =
            std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), all_canvas_layouts.size());
        const std::size_t chunk_size = (all_canvas_layouts.size() + num_threads - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (std::size_t i = 0; i < num_threads; ++i)
        {
            threads.emplace_back(
                [i, chunk_size, this, &conduct_pruning_steps]()
                {
                    std::size_t start_index = i * chunk_size;
                    std::size_t end_index   = std::min(start_index + chunk_size, all_canvas_layouts.size());

                    for (std::size_t j = start_index; j < end_index; ++j)
                    {
                        conduct_pruning_steps(all_canvas_layouts[j]);
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        return gate_candidate;
    }

    /**
     * This function calculates all combinations of distributing a given number of SiDBs across a specified number of
     * positions in the canvas. Each combination is then used to create a gate layout candidate.
     *
     * @tparam Lyt SiDB cell-level layout type.
     * @return A vector containing all possible gate layouts generated from the combinations.
     */
    [[nodiscard]] std::vector<Lyt> determine_all_possible_canvas_layouts() noexcept
    {
        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};
        designed_gate_layouts.reserve(all_combinations.size());

        const auto add_combination_to_layout_and_check_operation =
            [this, &designed_gate_layouts](const auto& combination) noexcept
        {
            auto layout_with_added_cells = canvas_sidb_layout(combination);
            designed_gate_layouts.push_back(layout_with_added_cells);
        };

        for (const auto& combination : all_combinations)
        {
            add_combination_to_layout_and_check_operation(combination);
        }

        return designed_gate_layouts;
    }
    /**
     * This function adds SiDBs (given by indices) to the skeleton layout that is returned afterwards.
     *
     * @param cell_indices A vector of indices of cells to be added to the skeleton layout.
     * @return A copy of the original layout (`skeleton_layout`) with SiDB cells added at specified indices.
     */
    [[nodiscard]] Lyt skeleton_layout_with_canvas_sidbs(const std::vector<std::size_t>& cell_indices) noexcept
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
    [[nodiscard]] Lyt canvas_sidb_layout(const std::vector<std::size_t>& cell_indices) noexcept
    {
        Lyt lyt{};

        for (const auto i : cell_indices)
        {
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            lyt.assign_cell_type(all_sidbs_in_canvas[i], sidb_technology::cell_type::LOGIC);
        }

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
 * @param skeleton The skeleton layout used as a starting point for gate design.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the *SiDB Gate Designer*.
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
