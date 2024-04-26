//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

#include "fiction/algorithms/optimization/simulated_annealing.hpp"
#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

struct design_sidb_gates_sa_params
{
    /**
     * Temperature schedule type.
     */
    temperature_schedule schedule{temperature_schedule::GEOMETRIC};
    /**
     * Initial temperature.
     */
    double initial_temperature{10000};
    /**
     * Final temperature.
     */
    double final_temperature{0.001};
    /**
     * Number of iterations per temperature.
     */
    std::size_t number_of_cycles{10};
    /**
     * Number of parallel runs to pick the overall best result from.
     */
    std::size_t number_of_instances{100};

    critical_temperature_params ct_params{};

    operational_domain_params op_params{};
};
/**
 * This struct contains parameters and settings to design SiDB gates.
 *
 * @tparam Cell-level layout type.
 *
 */
template <typename Lyt>
struct design_sidb_gates_params
{
    /**
     * Selector for the available design approaches.
     */
    enum class design_sidb_gates_mode
    {
        /**
         * All gate layouts are designed exhaustively.
         */
        EXHAUSTIVE,
        /**
         * Gate layouts are designed randomly.
         */
        RANDOM,
        /**
         * Gate layouts are designed with Simulated Annealing.
         */
        SIMANNEAL
    };
    /**
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Gate design mode.
     */
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::SIMANNEAL;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<typename Lyt::cell, typename Lyt::cell> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 1;
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * The design parameters used when designing with Simulated Annealing.
     */
    design_sidb_gates_sa_params simanneal_params{};
    /**
     * The number of gate implementations found before the random design process is stopped.
     */
    std::size_t maximal_random_solutions = 1;
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

    double cost_of_gate{};
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
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& tt, const design_sidb_gates_params<Lyt>& ps,
                           design_sidb_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{tt},
            params{ps},
            stats{st},
            all_cells_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)}
    {
        // Use std::uniform_int_distribution to generate a random index
        random_canvas_cell_functor = std::uniform_int_distribution<uint64_t>(0, all_cells_in_canvas.size() - 1);
        random_canvas_sidb_functor = std::uniform_int_distribution<uint64_t>(0, params.number_of_sidbs - 1);
    }

    /**
     * Design gates exhaustively and in parallel.
     *
     * This function adds each cell combination to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_exhaustive_design() noexcept
    {
        mockturtle::stopwatch       stop{stats.time_total};
        const is_operational_params params_is_operational{params.phys_params, params.sim_engine};

        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_cells_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};

        std::mutex mutex_to_protect_designer_gate_layouts;  // Mutex for protecting shared resources

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_designer_gate_layouts, &params_is_operational,
             &designed_gate_layouts](const auto& combination) noexcept
        {
            if (!are_sidbs_too_close(combination))
            {
                auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);
                if (const auto [status, sim_calls] =
                        is_operational(layout_with_added_cells, truth_table, params_is_operational);
                    status == operational_status::OPERATIONAL)
                {
                    const std::lock_guard lock_vector{mutex_to_protect_designer_gate_layouts};  // Lock the mutex
                    designed_gate_layouts.push_back(layout_with_added_cells);
                }
            }
        };

        std::vector<std::future<void>> futures{};
        futures.reserve(all_combinations.size());

        // Start asynchronous tasks to process combinations in parallel
        for (const auto& combination : all_combinations)
        {
            futures.emplace_back(
                std::async(std::launch::async, add_combination_to_layout_and_check_operation, combination));
        }

        // Wait for all tasks to finish
        for (auto& future : futures)
        {
            future.wait();
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
    [[nodiscard]] std::vector<Lyt> run_random_design(uint64_t target_num_solutions = 1u) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        std::vector<Lyt>      randomly_designed_gate_layouts = {};

        const is_operational_params params_is_operational{params.phys_params, params.sim_engine};

        const generate_random_sidb_layout_params<Lyt> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<Lyt>::positive_charges::FORBIDDEN};

        const std::size_t        num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads{};
        threads.reserve(num_threads);
        std::mutex               mutex_to_protect_designed_gate_layouts{};
        std::atomic<std::size_t> num_solutions_found(0);

        for (uint64_t z = 0u; z < num_threads; z++)
        {
            threads.emplace_back(
                [this, &num_solutions_found, &mutex_to_protect_designed_gate_layouts, &parameter,
                 &params_is_operational, &randomly_designed_gate_layouts, target_num_solutions]
                {
                    while (num_solutions_found < target_num_solutions)
                    {
                        const auto result_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter);
                        if (const auto [status, sim_calls] =
                                is_operational(result_lyt, truth_table, params_is_operational);
                            status == operational_status::OPERATIONAL)
                        {
                            const std::lock_guard lock{mutex_to_protect_designed_gate_layouts};
                            randomly_designed_gate_layouts.push_back(result_lyt);
                            num_solutions_found++;
                            // break;  // Break out of the loop after finding a solution
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
     * Design gates with Simulated Annealing.
     *
     * This function adds cells randomly to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of designed SiDB gate layouts.
     */
    [[nodiscard]] std::vector<Lyt> run_sa_design() noexcept
    {
        mockturtle::stopwatch                   stop{stats.time_total};
        generate_random_sidb_layout_params<Lyt> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<Lyt>::positive_charges::FORBIDDEN};
        const is_operational_params params_is_operational{params.phys_params, params.sim_engine};
        auto                        initial_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter);

        const auto temperature_schedule = params.simanneal_params.schedule == temperature_schedule::LINEAR ?
                                              linear_temperature_schedule :
                                              geometric_temperature_schedule;

        canvas_sidbs = determine_canvas_sidbs(initial_lyt);

        auto [optimized_placement, optimized_net_cost] = simulated_annealing<decltype(initial_lyt)>(
            initial_lyt, params.simanneal_params.initial_temperature, params.simanneal_params.final_temperature,
            params.simanneal_params.number_of_cycles,
            [&, this](const auto& lyt)
            {
                const auto logic_cost =
                    (is_operational(lyt, truth_table, params_is_operational).first == operational_status::OPERATIONAL) ?
                        0 :
                        10;

                double temp_cost = 0;
                double opo_cost  = 0;
                if (logic_cost == 0)
                {
                    temp_cost =
                        -((critical_temperature_gate_based(lyt, truth_table, params.simanneal_params.ct_params)) /
                          params.simanneal_params.ct_params.max_temperature);
                    operational_domain_stats stats_op{};
                    const auto               op_domain = operational_domain_flood_fill(
                        lyt, truth_table, 0, params.simanneal_params.op_params,
                        operational_domain::parameter_point{params.phys_params.epsilon_r, params.phys_params.lambda_tf},
                        &stats_op);
                    opo_cost = -stats_op.percentual_operational_area;
                }
                //                            if (logic_cost + temp_cost + opo_cost < -0.5) {
                //                                std::cout << fmt::format("cost: {}",
                //                                                         static_cast<double>(logic_cost + temp_cost +
                //                                                         opo_cost))
                //                                          << std::endl;
                //                            }
                return static_cast<double>(logic_cost + temp_cost + opo_cost);
            },
            temperature_schedule,
            [&](auto lyt)
            {
                // print_layout(lyt);
                const auto lyt_swap = move_sidb(lyt);
                //                                const auto cost_logic =
                //                                static_cast<double>(number_of_operational_input_combinations(
                //                                    lyt_swap, truth_table, skeleton_layout, params_is_operational,
                //                                    params.canvas));
                //                                std::cout << fmt::format("cost logic: {}",
                //                                                         cost_logic)
                //                                          << std::endl;
                //                std::cout << fmt::format(
                //                                 "cost temp: {}",
                //                                 static_cast<double>(
                //                                     (400 - critical_temperature_gate_based(lyt_swap, truth_table,
                //                                     ct_params)) / 400))
                //                          << std::endl;
                return lyt_swap;
            });

        std::cout << fmt::format("final cost: {}", optimized_net_cost) << std::endl;
        stats.cost_of_gate = optimized_net_cost;
        return {optimized_placement};
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
    const design_sidb_gates_params<Lyt>& params;
    /**
     * The statistics of the gate design.
     */
    design_sidb_gates_stats& stats;
    /**
     * All cells within the canvas.
     */
    const std::vector<typename Lyt::cell> all_cells_in_canvas;
    /**
     * All SiDBs within the canvas.
     */
    std::vector<typename Lyt::cell> canvas_sidbs;

    std::vector<typename Lyt::cell>                   old_canvas_sidbs;
    std::pair<typename Lyt::cell, typename Lyt::cell> swaped_pair{};

    double logical_cost{};

    /**
     * A random-number generator.
     */
    std::mt19937_64 generator{std::random_device{}()};
    /**
     *
     */
    std::uniform_int_distribution<uint64_t> random_canvas_sidb_functor;
    /**
     *
     */
    std::uniform_int_distribution<uint64_t> random_canvas_cell_functor;

    [[nodiscard]] std::vector<typename Lyt::cell> determine_canvas_sidbs(const Lyt& lyt) const noexcept
    {
        std::vector<typename Lyt::cell> placed_canvas_sidbs = {};
        placed_canvas_sidbs.reserve(params.number_of_sidbs);
        lyt.foreach_cell(
            [&](const auto& cell)
            {
                if (std::find(all_cells_in_canvas.begin(), all_cells_in_canvas.end(), cell) !=
                    all_cells_in_canvas.end())
                {
                    placed_canvas_sidbs.emplace_back(cell);
                }
            });
        return placed_canvas_sidbs;
    }

    [[nodiscard]] Lyt move_sidb(Lyt& lyt) noexcept
    {
        const auto random_index         = random_canvas_cell_functor(generator);
        const auto random_index_replace = random_canvas_sidb_functor(generator);
        const auto random_cell          = all_cells_in_canvas[random_index];
        const auto replace_sidb         = canvas_sidbs[random_index_replace];

        // cell has to be empty
        if (!lyt.is_empty_cell(random_cell))
        {
            return lyt;
        }
        lyt.assign_cell_type(replace_sidb, Lyt::technology::cell_type::EMPTY);
        lyt.assign_cell_type(random_cell, Lyt::technology::cell_type::NORMAL);
        swaped_pair                        = {replace_sidb, random_cell};
        old_canvas_sidbs                   = canvas_sidbs;
        canvas_sidbs[random_index_replace] = random_cell;
        return lyt;
    }

    void undo_swap(const Lyt& lyt)
    {
        lyt.assign_cell_type(swaped_pair.firsr, Lyt::technology::cell_type::NORMAL);
        lyt.assign_cell_type(swaped_pair.second, Lyt::technology::cell_type::NORMAL);
    }

    /**
     * Checks if any SiDBs within the specified cell indices are located too closely together, with a distance of less
     * than 0.5 nanometers.
     *
     * This function iterates through the provided cell indices and compares the distance between SiDBs. If it finds any
     * pair of SiDBs within a distance of 0.5 nanometers, it returns `true` to indicate that SiDBs are too close;
     * otherwise, it returns `false`.
     *
     * @param cell_indices A vector of cell indices to check for SiDB proximity.
     * @return `true` if any SiDBs are too close; otherwise, `false`.
     */
    [[nodiscard]] bool are_sidbs_too_close(const std::vector<std::size_t>& cell_indices) noexcept
    {
        for (std::size_t i = 0; i < cell_indices.size(); i++)
        {
            for (std::size_t j = i + 1; j < cell_indices.size(); j++)
            {
                if (sidb_nanometer_distance<Lyt>(skeleton_layout, all_cells_in_canvas[cell_indices[i]],
                                                 all_cells_in_canvas[cell_indices[j]]) < 0.5)
                {
                    return true;
                }
            }
        }
        return false;
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
            assert(i < all_cells_in_canvas.size() && "cell indices are out-of-range");

            if (lyt_copy.get_cell_type(all_cells_in_canvas[i]) == sidb_technology::cell_type::EMPTY)
            {
                lyt_copy.assign_cell_type(all_cells_in_canvas[i], sidb_technology::cell_type::NORMAL);
            }
        }

        return lyt_copy;
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
                                                 const design_sidb_gates_params<Lyt>& params = {},
                                                 design_sidb_gates_stats*             pst    = nullptr) noexcept
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
        result = p.run_exhaustive_design();
    }
    else if (params.design_mode == design_sidb_gates_params<Lyt>::design_sidb_gates_mode::RANDOM)
    {
        result = p.run_random_design(params.maximal_random_solutions);
    }
    else
    {
        result = p.run_sa_design();
    }
    if (pst)
    {
        *pst = st;
    }
    return result;
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
