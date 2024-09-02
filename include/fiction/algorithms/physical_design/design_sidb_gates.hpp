//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_SIDB_GATES_HPP
#define FICTION_DESIGN_SIDB_GATES_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_nm_distance.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/traits.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <mutex>
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
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Gate design mode.
     */
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::EXHAUSTIVE;
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
     * The design process is terminated after a valid SiDB gate design is found.
     *
     * @note This parameter has no effect unless the gate design is exhaustive.
     */
    termination_condition termination_cond = termination_condition::ALL_COMBINATIONS_ENUMERATED;
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
     */
    design_sidb_gates_impl(const Lyt& skeleton, const std::vector<TT>& spec,
                           const design_sidb_gates_params<cell<Lyt>>& ps) :
            skeleton_layout{skeleton},
            truth_table{spec},
            params{ps},
            all_sidbs_in_canvas{all_coordinates_in_spanned_area(params.canvas.first, params.canvas.second)}
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
        const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};

        auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));
        std::unordered_set<coordinate<Lyt>> sidbs_affected_by_defects = {};

        // used to collect all SiDBs that are affected due to neutrally charged defects.
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            sidbs_affected_by_defects = skeleton_layout.all_affected_sidbs(std::make_pair(0, 0));
        }

        std::vector<Lyt>  designed_gate_layouts = {};
        std::mutex        mutex_to_protect_designed_gate_layouts;
        std::atomic<bool> solution_found = false;

        // Shuffle the combinations before dividing them among threads
        std::shuffle(all_combinations.begin(), all_combinations.end(),
                     std::default_random_engine(std::random_device{}()));

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_designed_gate_layouts, &params_is_operational, &designed_gate_layouts,
             &sidbs_affected_by_defects, &solution_found](const auto& combination) noexcept
        {
            for (const auto& comb : combination)
            {
                // if SiDBs are too close of the position are impossible due to closely placed neutrally charged defects
                if (!are_sidbs_too_close(cell_indices_to_cell_vector(comb), sidbs_affected_by_defects))
                {
                    // canvas SiDBs are added to the skeleton
                    auto layout_with_added_cells = add_canvas_sidbs_to_skeleton_layout(comb);

                    if (const auto [status, sim_calls] =
                            is_operational(layout_with_added_cells, truth_table, params_is_operational);
                        status == operational_status::OPERATIONAL)
                    {
                        {
                            const std::lock_guard lock_vector{mutex_to_protect_designed_gate_layouts};
                            designed_gate_layouts.push_back(layout_with_added_cells);
                        }

                        solution_found = true;
                    }

                    if (solution_found &&
                        (params.termination_cond ==
                         design_sidb_gates_params<cell<Lyt>>::termination_condition::AFTER_FIRST_SOLUTION))
                    {
                        return;
                    }

                    continue;
                }
            }
        };

        const auto chunk_size = all_combinations.size() / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        for (auto i = 0u; i < num_threads; ++i)
        {
            const std::size_t start = i * chunk_size;
            const std::size_t end   = (i == num_threads - 1) ? all_combinations.size() : (i + 1) * chunk_size;

            std::vector<std::vector<std::size_t>> chunk_combinations(
                all_combinations.cbegin() + static_cast<int64_t>(start),
                all_combinations.cbegin() + static_cast<int64_t>(end));

            threads.emplace_back(add_combination_to_layout_and_check_operation, chunk_combinations);
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
    [[nodiscard]] std::vector<Lyt> run_random_design() noexcept
    {
        std::vector<Lyt> randomly_designed_gate_layouts = {};

        const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};

        const generate_random_sidb_layout_params<cell<Lyt>> parameter_random_layout{
            params.canvas, params.number_of_sidbs,
            generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::FORBIDDEN};

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        std::mutex mutex_to_protect_designed_gate_layouts{};  // used to control access to shared resources

        std::atomic<bool> gate_layout_is_found(false);

        for (uint64_t z = 0u; z < num_threads; z++)
        {
            threads.emplace_back(
                [this, &gate_layout_is_found, &mutex_to_protect_designed_gate_layouts, &parameter_random_layout,
                 &params_is_operational, &randomly_designed_gate_layouts]
                {
                    while (!gate_layout_is_found)
                    {
                        auto result_lyt = generate_random_sidb_layout<Lyt>(skeleton_layout, parameter_random_layout);
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
    std::vector<typename Lyt::cell> all_sidbs_in_canvas;
    /**
     * Number of threads to be used for parallel execution.
     */
    const std::size_t num_threads{std::thread::hardware_concurrency()};
    /**
     * Checks if any SiDBs within the specified cell indices are located too closely together, with a distance of less
     * than 0.5 nanometers.
     *
     * This function iterates over the provided cell indices and compares the distance between SiDBs. If it finds any
     * pair of SiDBs within a distance of less than 0.5 nanometers, it returns `true` indicating that SiDBs are too
     * close; otherwise, it returns `false`.
     *
     * @param cells A vector of cells to check for proximity.
     * @tparam affected_cells All SiDBs that are affected by atomic defects.
     * @return `true` if any SiDBs are too close; otherwise, `false`.
     */
    [[nodiscard]] bool
    are_sidbs_too_close(const std::vector<typename Lyt::cell>&        cells,
                        const std::unordered_set<typename Lyt::cell>& affected_cells = {}) const noexcept
    {
        for (std::size_t i = 0; i < cells.size(); i++)
        {
            if constexpr (has_get_sidb_defect_v<Lyt>)
            {
                if (skeleton_layout.get_sidb_defect(cells[i]).type != sidb_defect_type::NONE)
                {
                    return true;
                }
            }
            if (affected_cells.count(cells[i]) > 0)
            {
                return true;
            }
            for (std::size_t j = i + 1; j < cells.size(); j++)
            {
                if (sidb_nm_distance<Lyt>(Lyt{}, cells[i], cells[j]) < 0.5)
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
    [[nodiscard]] Lyt add_canvas_sidbs_to_skeleton_layout(const std::vector<std::size_t>& cell_indices) const noexcept
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
    /**
     * Converts a vector of cell indices to a vector of corresponding cells in the layout.
     *
     * @param cell_indices Vector of cell indices to convert.
     * @return A vector of cells corresponding to the given indices.
     */
    [[nodiscard]] std::vector<typename Lyt::cell>
    cell_indices_to_cell_vector(const std::vector<std::size_t>& cell_indices) const noexcept
    {
        std::vector<typename Lyt::cell> cells{};
        cells.reserve(cell_indices.size());
        for (const auto index : cell_indices)
        {
            assert(index < all_sidbs_in_canvas.size() && "Cell index is out of range.");
            cells.push_back(all_sidbs_in_canvas[index]);
        }
        return cells;
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
                                                 const design_sidb_gates_params<cell<Lyt>>& params = {}) noexcept
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

    detail::design_sidb_gates_impl<Lyt, TT> p{skeleton, spec, params};

    if (params.design_mode == design_sidb_gates_params<cell<Lyt>>::design_sidb_gates_mode::EXHAUSTIVE)
    {
        return p.run_exhaustive_design();
    }

    detail::design_sidb_gates_impl<Lyt, TT> p_random{skeleton, spec, params};
    return p.run_random_design();
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
