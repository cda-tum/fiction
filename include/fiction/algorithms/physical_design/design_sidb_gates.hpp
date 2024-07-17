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
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"

#include <kitty/traits.hpp>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <mutex>
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
         * All gate layouts are designed exhaustively.
         */
        EXHAUSTIVE,
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
    design_sidb_gates_mode design_mode = design_sidb_gates_mode::EXHAUSTIVE;
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<CellType, CellType> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 1;
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
                           const design_sidb_gates_params<cell<Lyt>>& ps) :
            skeleton_layout{skeleton},
            truth_table{tt},
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
        const auto all_combinations = determine_all_combinations_of_distributing_k_entities_on_n_positions(
            params.number_of_sidbs, static_cast<std::size_t>(all_sidbs_in_canvas.size()));

        std::vector<Lyt> designed_gate_layouts = {};

        std::mutex mutex_to_protect_designer_gate_layouts;  // Mutex for protecting shared resources

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_designer_gate_layouts, &designed_gate_layouts](const auto& combination) noexcept
        {
            if (!are_sidbs_too_close(combination))
            {
                auto layout_with_added_cells = skeleton_layout_with_canvas_sidbs(combination);
                if (const auto [status, sim_calls] =
                        is_operational(layout_with_added_cells, truth_table, params.operational_params);
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
                lyt_copy.assign_cell_type(all_sidbs_in_canvas[i], sidb_technology::cell_type::LOGIC);
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

    return p.run_random_design();
}

}  // namespace fiction

#endif  // FICTION_DESIGN_SIDB_GATES_HPP
