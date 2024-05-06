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
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <mutex>
#include <random>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * Parameters for Simulated Annealing-based gate design.
 */
struct design_sidb_gates_metric_driven_simulated_annealing_params
{
    /**
     * Type of temperature schedule used in Simulated Annealing.
     */
    temperature_schedule schedule{temperature_schedule::GEOMETRIC};
    /**
     * Initial temperature for Simulated Annealing.
     */
    double initial_temperature{1000};
    /**
     * Final temperature for Simulated Annealing.
     */
    double final_temperature{0.1};
    /**
     * Number of iterations per temperature in Simulated Annealing.
     */
    std::size_t number_of_cycles{10};
    /**
     * Parameters for simulating the Critical Temperature.
     */
    critical_temperature_params ct_params{};
    /**
     * Parameters for simulating the Operational Domain.
     */
    operational_domain_params op_params{};
    /**
     * Weight assigned to the critical temperature in the overall cost function.
     * A negative value indicates that a high critical temperature is preferred.
     */
    double weight_temperature = -1.0;
    /**
     * Weight assigned to the operational domain in the overall cost function.
     * A negative value indicates that this weight is not used.
     */
    double weight_operational_domain = -1.0;
    /**
     * Weight assigned to the non-operational behavior in the overall cost function.
     * This weight affects the cost function when the design is non-operational.
     */
    double weight_non_operational = 10;
};
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
    RANDOM
};
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
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<CellType, CellType> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 1;
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
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
    /**
     * The cost value of the final gate designed with Simulated Annealing.
     */
    double gate_cost{};
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
     * @param st Statistics for the gate designer.
     */
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& tt,
                           const design_sidb_gates_params<cell<Lyt>>& ps, design_sidb_gates_stats& st) :
            skeleton_layout{skeleton},
            truth_table{tt},
            params{ps},
            stats{st},
            all_sidbs_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)}
    {
        // Use std::uniform_int_distribution to generate a random index
        random_canvas_cell_functor = std::uniform_int_distribution<uint64_t>(0, all_sidbs_in_canvas.size() - 1);
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
        const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};

        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

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

        const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};

        const generate_random_sidb_layout_params<cell<Lyt>> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::FORBIDDEN};

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
     * This function designs gates with Simulated Annealing. The cost function involves the critical temperature and the
     * operational domain. The importance of the individual figures of merit can be adjusted by the weights.
     *
     * @param sa_params Simulated Annealing parameters.
     * @return Designed SiDB gate with minimal cost.
     */
    [[nodiscard]] Lyt run_metric_driven_design_process(
        const design_sidb_gates_metric_driven_simulated_annealing_params& sa_params) noexcept
    {
        mockturtle::stopwatch                         stop{stats.time_total};
        generate_random_sidb_layout_params<cell<Lyt>> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::FORBIDDEN};
        const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};
        auto                        initial_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter);

        const auto temperature_schedule = sa_params.schedule == temperature_schedule::LINEAR ?
                                              linear_temperature_schedule :
                                              geometric_temperature_schedule;

        canvas_sidbs = determine_canvas_sidbs(initial_lyt);

        auto [optimized_gate_design, optimized_net_cost] = simulated_annealing<decltype(initial_lyt)>(
            initial_lyt, sa_params.initial_temperature, sa_params.final_temperature, sa_params.number_of_cycles,
            [&, this](const auto& lyt)
            {
                const auto logic_cost =
                    (is_operational(lyt, truth_table, params_is_operational).first == operational_status::OPERATIONAL) ?
                        0 :
                        sa_params.weight_non_operational;

                double temp_cost = 0;
                double opo_cost  = 0;
                if (logic_cost == 0)
                {
                    if (sa_params.weight_temperature != 0.0)
                    {
                        temp_cost = ((critical_temperature_gate_based(lyt, truth_table, sa_params.ct_params)) /
                                     sa_params.ct_params.max_temperature);
                    }
                    if (sa_params.weight_operational_domain != 0.0)
                    {
                        operational_domain_stats stats_op{};
                        const auto               op_domain = operational_domain_flood_fill(
                            lyt, truth_table, 0, sa_params.op_params,
                            operational_domain::parameter_point{params.simulation_parameters.epsilon_r,
                                                                params.simulation_parameters.lambda_tf},
                            &stats_op);
                        opo_cost = stats_op.percentual_operational_area;
                    }
                }
                return cost_function_chi({logic_cost, temp_cost, opo_cost},
                                         {sa_params.weight_non_operational, sa_params.weight_temperature,
                                          sa_params.weight_operational_domain});
            },
            temperature_schedule,
            [&](auto lyt)
            {
                const auto lyt_swap = move_sidb(lyt);
                return lyt_swap;
            });
        stats.gate_cost = optimized_net_cost;
        return optimized_gate_design;
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
     * The statistics of the gate design.
     */
    design_sidb_gates_stats& stats;
    /**
     * All cells within the canvas.
     */
    const std::vector<typename Lyt::cell> all_sidbs_in_canvas;
    /**
     * All SiDBs within the canvas.
     */
    std::vector<typename Lyt::cell> canvas_sidbs;
    /**
     * Canvas SiDBs before move.
     */
    std::vector<typename Lyt::cell> canvas_sidbs_before_move;
    /**
     * Canvas SiDB was moved from one cell (first cell) to another cell (second cell).
     */
    std::pair<typename Lyt::cell, typename Lyt::cell> sidb_moved_from_to{};
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
    /**
     * This function iterates over each cell in the provided layout `lyt` and checks if the cell
     * corresponds to a canvas SiDB. Canvas SiDBs are defined as SiDBs that are part of the canvas
     * region. It populates a vector with the canvas SiDBs found in the layout and returns it.
     *
     * @param lyt The layout from which canvas SiDBs are to be determined.
     * @return A vector containing the canvas SiDBs found in the layout.
     */
    [[nodiscard]] std::vector<typename Lyt::cell> determine_canvas_sidbs(const Lyt& lyt) const noexcept
    {
        std::vector<typename Lyt::cell> placed_canvas_sidbs = {};
        placed_canvas_sidbs.reserve(params.number_of_sidbs);
        lyt.foreach_cell(
            [&](const auto& cell)
            {
                if (std::find(all_sidbs_in_canvas.begin(), all_sidbs_in_canvas.end(), cell) !=
                    all_sidbs_in_canvas.end())
                {
                    placed_canvas_sidbs.emplace_back(cell);
                }
            });
        return placed_canvas_sidbs;
    }
    /**
     * This function randomly selects a canvas cell from the layout `lyt` and a canvas SiDB
     * to replace it with. It then moves the selected canvas SiDB to the randomly chosen
     * canvas cell, updating the layout accordingly. If the randomly chosen canvas cell is not
     * empty, the layout remains unchanged.
     *
     * @param lyt The layout from which a canvas SiDB is to be moved.
     * @return The layout after the canvas SiDB has been moved, or the original layout if the
     *         randomly chosen canvas cell was not empty.
     */
    [[nodiscard]] Lyt move_sidb(Lyt& lyt) noexcept
    {
        const auto random_index         = random_canvas_cell_functor(generator);
        const auto random_index_replace = random_canvas_sidb_functor(generator);
        const auto random_cell          = all_sidbs_in_canvas[random_index];
        const auto replace_sidb         = canvas_sidbs[random_index_replace];

        // cell has to be empty
        if (!lyt.is_empty_cell(random_cell))
        {
            return lyt;
        }
        lyt.assign_cell_type(replace_sidb, Lyt::technology::cell_type::EMPTY);
        lyt.assign_cell_type(random_cell, Lyt::technology::cell_type::NORMAL);
        sidb_moved_from_to                 = {replace_sidb, random_cell};
        canvas_sidbs_before_move           = canvas_sidbs;
        canvas_sidbs[random_index_replace] = random_cell;
        return lyt;
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
                if (sidb_nm_distance<Lyt>(Lyt{}, all_sidbs_in_canvas[cell_indices[i]],
                                          all_sidbs_in_canvas[cell_indices[j]]) < 0.5)
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
            assert(i < all_sidbs_in_canvas.size() && "cell indices are out-of-range");

            if (lyt_copy.get_cell_type(all_sidbs_in_canvas[i]) == sidb_technology::cell_type::EMPTY)
            {
                lyt_copy.assign_cell_type(all_sidbs_in_canvas[i], sidb_technology::cell_type::NORMAL);
            }
        }

        return lyt_copy;
    }
};
}  // namespace detail

/**
 * The *SiDB Gate Designer* designs SiDB gate implementations based on a specified Boolean function, a
 * skeleton layout (can hold defects), canvas size, and a predetermined number of canvas SiDBs. Two different design
 * modes are implemented: `exhaustive` and `random`.
 *
 * The `exhaustive design` is composed of three steps:
 * 1. In the initial step, all possible distributions of `number_of_sidbs` SiDBs within a given canvas are
 * exhaustively determined. This ensures exhaustive coverage of every potential arrangement of `number_of_sidbs` SiDBs
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
 * @param design_mode The design mode to use.
 * @param stats The design statistics.
 * @return A vector of designed SiDB gate layouts.
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::vector<Lyt>
design_sidb_gates(const Lyt& skeleton, const std::vector<TT>& spec,
                  const design_sidb_gates_params<cell<Lyt>>& params      = {},
                  const design_sidb_gates_mode               design_mode = design_sidb_gates_mode::EXHAUSTIVE,
                  design_sidb_gates_stats*                   stats       = nullptr) noexcept
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

    if (design_mode == design_sidb_gates_mode::EXHAUSTIVE)
    {
        result = p.run_exhaustive_design();
    }
    else
    {
        result = p.run_random_design(params.maximal_random_solutions);
    }
    if (stats)
    {
        *stats = st;
    }
    return result;
}

/**
 * This function designs SiDB gates to minimize the cost function \f$\chi\f$, considering a layout skeleton, a set of
 * truth tables, and specified parameters for gate design and simulated annealing. Currently, only the critical
 * temperature and the operational domain are incorporated into the cost function.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param skeleton The layout skeleton used as the basis for gate design.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params The parameters for gate design.
 * @param sa_params Parameters for simulated annealing.
 * @param stats Statistics for gate design.
 * @return A layout with SiDB gates designed to minimize the cost function.
 */
template <typename Lyt, typename TT>
[[nodiscard]] Lyt design_sidb_gates_metric_driven_simulated_annealing(
    const Lyt& skeleton, const std::vector<TT>& spec, const design_sidb_gates_params<cell<Lyt>>& params,
    const design_sidb_gates_metric_driven_simulated_annealing_params& sa_params,
    design_sidb_gates_stats*                                          stats = nullptr) noexcept
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

    const auto result = p.run_metric_driven_design_process(sa_params);

    if (stats)
    {
        *stats = st;
    }
    return result;
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
