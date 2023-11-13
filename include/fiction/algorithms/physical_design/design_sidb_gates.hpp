//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

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

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <numeric>
#include <thread>
#include <utility>
#include <vector>

#include <combinations.h>

namespace fiction
{

/**
 * This struct contains parameters and settings to design SiDB gates.
 *
 */
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
        RANDOM
    };
    /**
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Gate design mode.
     */
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::EXHAUSTIVE;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<siqad::coord_t, siqad::coord_t> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 1;
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * The percentage of all combinations that are tested before the design process is canceled.
     */
    double procentual_maximum_attemps = 1.0;
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
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& tt, const design_sidb_gates_params& ps) :
            skeleton_layout{skeleton},
            truth_table{tt},
            params{ps},
            all_sidbs_in_cavas{all_sidbs_in_spanned_area(params.canvas.first, params.canvas.second)}
    {}
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
        const is_operational_params params_is_operational{params.phys_params, params.sim_engine};
        auto                        all_combinations = determine_all_combinations_of_given_sidbs_in_canvas();

        std::unordered_set<typename Lyt::coordinate> sidbs_affected_by_defects = {};

        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            sidbs_affected_by_defects = skeleton_layout.all_affected_sidbs(std::make_pair(0, 0));
        }

        std::vector<Lyt>      designed_gate_layouts = {};
        std::mutex            mutex_to_protect_designer_gate_layouts;
        std::atomic<bool>     solution_found = false;
        std::atomic<uint64_t> global_iteration_counter(0);

        const auto total_comb = binomial_coefficient(all_sidbs_in_cavas.size(), params.number_of_sidbs);

        // Shuffle the combinations before dividing them among threads
        std::shuffle(all_combinations.begin(), all_combinations.end(),
                     std::default_random_engine(std::random_device()()));

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_designer_gate_layouts, &params_is_operational, &designed_gate_layouts,
             &sidbs_affected_by_defects, &solution_found, &global_iteration_counter,
             &total_comb](const auto& combination) noexcept
        {
            for (const auto& comb : combination)
            {
                if (!solution_found && !are_sidbs_too_close(comb, sidbs_affected_by_defects) &&
                    global_iteration_counter <
                        static_cast<uint64_t>(params.procentual_maximum_attemps * static_cast<double>(total_comb)))
                {
                    // canvas SiDBs are added to the skeleton
                    auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(comb);

                    if (!solution_found)
                    {
                        if (const auto [status, sim_calls] =
                                is_operational(layout_with_added_cells, truth_table, params_is_operational);
                            status == operational_status::OPERATIONAL)
                        {
                            {
                                const std::lock_guard lock_vector{mutex_to_protect_designer_gate_layouts};
                                designed_gate_layouts.push_back(layout_with_added_cells);
                            }
                            solution_found = true;
                        }
                    }
                }
                global_iteration_counter++;
            }
        };

        const uint64_t    num_threads = std::thread::hardware_concurrency();
        const std::size_t chunk_size  = all_combinations.size() / num_threads;

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (auto i = 0u; i < num_threads; ++i)
        {
            const std::size_t start = i * chunk_size;
            const std::size_t end   = (i == num_threads - 1) ? all_combinations.size() : (i + 1) * chunk_size;
            std::vector<std::vector<std::size_t>> chunk_combinations(all_combinations.begin() + start,
                                                                     all_combinations.begin() + end);
            threads.emplace_back(add_combination_to_layout_and_check_operation, chunk_combinations);
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

        const is_operational_params params_is_operational{params.phys_params, params.sim_engine};

        const generate_random_sidb_layout_params<Lyt> parameter{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<Lyt>::positive_charges::FORBIDDEN};

        const std::size_t        num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads{};
        threads.reserve(num_threads);
        std::mutex mutex_to_protect_designed_gate_layouts{};  // used to control access to shared resources

        std::atomic<bool> gate_layout_is_found(false);

        for (uint64_t z = 0u; z < num_threads; z++)
        {
            threads.emplace_back(
                [this, &gate_layout_is_found, &mutex_to_protect_designed_gate_layouts, &parameter,
                 &params_is_operational, &randomly_designed_gate_layouts]
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
                                is_operational(result_lyt, truth_table, params_is_operational);
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
            thread.join();
        }

        return randomly_designed_gate_layouts;
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
    const design_sidb_gates_params& params;
    /**
     * All cells within the canvas.
     */
    const std::vector<fiction::siqad::coord_t> all_sidbs_in_cavas;
    /**
     * Calculates all possible combinations of distributing the given number of SiDBs within a canvas
     * based on the provided parameters. It generates combinations of SiDB indices (representing the cell position in
     * the canvas from top to bottom and left to right) and stores them in the `all_combinations` vector. The number of
     * SiDBs in each combination is determined by `parameter.number_of_sidbs`.
     *
     * @return All possible combinations as a vector of vectors of indices.
     */
    [[nodiscard]] std::vector<std::vector<std::size_t>> determine_all_combinations_of_given_sidbs_in_canvas() noexcept
    {
        std::vector<std::vector<std::size_t>> all_combinations{};
        all_combinations.reserve(binomial_coefficient(all_sidbs_in_cavas.size(), params.number_of_sidbs));

        std::vector<std::size_t> numbers(all_sidbs_in_cavas.size());
        std::iota(numbers.begin(), numbers.end(), 0);

        combinations::for_each_combination(
            numbers.begin(),
            numbers.begin() + static_cast<std::vector<std::size_t>::difference_type>(params.number_of_sidbs),
            numbers.end(),
            [this, &all_combinations](const auto begin, const auto end)
            {
                std::vector<std::size_t> combination{};
                combination.reserve(params.number_of_sidbs);

                for (auto it = begin; it != end; ++it)
                {
                    combination.push_back(static_cast<std::size_t>(*it));
                }

                all_combinations.push_back(combination);

                return false;  // keep looping
            });

        return all_combinations;
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
    [[nodiscard]] bool are_sidbs_too_close(const std::vector<std::size_t>&               cell_indices,
                                           const std::unordered_set<typename Lyt::cell>& affected_cells = {}) noexcept
    {
        for (std::size_t i = 0; i < cell_indices.size(); i++)
        {
            if constexpr (has_get_sidb_defect_v<Lyt>)
            {
                if (skeleton_layout.get_sidb_defect(all_sidbs_in_cavas[cell_indices[i]]).type != sidb_defect_type::NONE)
                {
                    return true;
                }
            }
            if (affected_cells.count(all_sidbs_in_cavas[cell_indices[i]]) > 0)
            {
                return true;
            }
            for (std::size_t j = i + 1; j < cell_indices.size(); j++)
            {
                if (sidb_nanometer_distance<sidb_cell_clk_lyt_siqad>(skeleton_layout,
                                                                     all_sidbs_in_cavas[cell_indices[i]],
                                                                     all_sidbs_in_cavas[cell_indices[j]]) < 0.5)
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
            assert(i < all_sidbs_in_cavas.size() && "cell indices are out-of-range");

            if (lyt_copy.get_cell_type(all_sidbs_in_cavas[i]) == sidb_technology::cell_type::EMPTY)
            {
                lyt_copy.assign_cell_type(all_sidbs_in_cavas[i], sidb_technology::cell_type::NORMAL);
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
                                                 const design_sidb_gates_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(skeleton.num_pis() > 0 && "skeleton needs input cells");
    assert(skeleton.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    detail::design_sidb_gates_impl<Lyt, TT> p{skeleton, spec, params};

    if (params.design_mode == design_sidb_gates_params::design_sidb_gates_mode::EXHAUSTIVE)
    {
        return p.run_exhaustive_design();
    }

    detail::design_sidb_gates_impl<Lyt, TT> p_random{skeleton, spec, params};
    return p.run_random_design();
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
