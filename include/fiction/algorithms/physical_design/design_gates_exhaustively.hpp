//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_DESIGN_GATES_EXHAUSTIVELY_HPP
#define FICTION_DESIGN_GATES_EXHAUSTIVELY_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
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
 * This struct contains parameters and settings used by the *Automatic Exhaustive
 * Gate Designer*. It is used to configure the simulation and design of SiDB gates.
 *
 */
struct design_gates_exhaustively_params
{
    /**
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Canvas spanned by the northwest and southeast cell.
     */
    std::pair<siqad::coord_t, siqad::coord_t> canvas{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 0;
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
};

namespace detail
{
template <typename Lyt, typename TT>
class design_gate_exhaustively_impl
{
  public:
    /**
     * This constructor initializes an instance of the *Automatic Exhaustive Gate Designer*
     * implementation with the provided skeleton layout and configuration parameters.
     *
     * @param skeleton The skeleton layout used as a basis for gate design.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @param params   Parameters and settings for the gate designer.
     */
    design_gate_exhaustively_impl(const Lyt& skeleton, const std::vector<TT>& tt,
                                  design_gates_exhaustively_params params) :
            skeleton_layout{skeleton},
            truth_table{tt},
            parameter{std::move(params)}
    {}
    /**
     * Run the exhaustive gate layout design process in parallel.
     *
     * This function adds each cell combination to the given skeleton, and determines whether the layout is operational
     * based on the specified parameters. The design process is parallelized to improve performance.
     *
     * @return A vector of found SiDB gate layouts that meet the operational criteria.
     */
    std::vector<Lyt> run()
    {
        const is_operational_params params_is_operational{parameter.phys_params, parameter.sim_engine};
        all_sidbs_in_cavas          = all_sidbs_in_spanned_area(parameter.canvas.first, parameter.canvas.second);
        const auto all_combinations = determine_all_combinations_of_given_sidbs_in_canvas();

        std::vector<Lyt> all_found_gate_layouts = {};

        std::mutex mutex_to_protect_found_gate_layouts;  // Mutex for protecting shared resources

        const auto add_combination_to_layout_and_check_operation =
            [this, &mutex_to_protect_found_gate_layouts, &params_is_operational,
             &all_found_gate_layouts](const auto& combination) noexcept
        {
            if (!are_sidbs_too_close(combination))
            {
                auto layout_with_added_cells = add_cells_to_layout_based_on_indices(combination);

                if (is_operational(layout_with_added_cells, truth_table, params_is_operational).first ==
                    operational_status::OPERATIONAL)
                {
                    const std::lock_guard lock_vector(mutex_to_protect_found_gate_layouts);  // Lock the mutex
                    all_found_gate_layouts.push_back(layout_with_added_cells);
                }
            }
        };

        std::vector<std::future<void>> futures;
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

        return all_found_gate_layouts;
    }

  private:
    /**
     * The skeleton layout serves as a starting layout to which SiDBs are added to create unique SiDB layouts and, if
     * possible, working gates. It defines input and output wires.
     */
    Lyt skeleton_layout;
    /**
     * Truth table of the given gate (if layout is simulated in `gate-based` mode).
     */
    std::vector<TT> truth_table{};
    /**
     * Parameters for the *Automatic Exhaustive Gate Designer*.
     */
    design_gates_exhaustively_params parameter;
    /**
     * All cells within the canvas.
     */
    std::vector<siqad::coord_t> all_sidbs_in_cavas{};
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
        all_combinations.reserve(binomial_coefficient(all_sidbs_in_cavas.size(), parameter.number_of_sidbs));
        std::vector<std::size_t> numbers(all_sidbs_in_cavas.size());
        std::iota(numbers.begin(), numbers.end(), 0);
        combinations::for_each_combination(
            numbers.begin(),
            numbers.begin() + static_cast<std::vector<std::size_t>::difference_type>(parameter.number_of_sidbs),
            numbers.end(),
            [this, &all_combinations](const auto begin, const auto end)
            {
                std::vector<std::size_t> combination{};
                combination.reserve(parameter.number_of_sidbs);
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
     * Checks if any SiDBs within the specified cell indices
     * are located too closely together, with a distance of less than 0.5 nanometers.
     *
     * This function iterates through the provided cell indices and compares the distance
     * between SiDBs. If it finds any pair of SiDBs within a distance of 0.5 nanometers,
     * it returns true to indicate that SiDBs are too close; otherwise, it returns false.
     *
     * @param cell_indices A vector of cell indices to check for SiDB proximity.
     * @return `true` if any SiDBs are too close; otherwise, `false`.
     */
    [[maybe_unused]] bool are_sidbs_too_close(const std::vector<std::size_t>& cell_indices) noexcept
    {
        for (std::size_t i = 0; i < cell_indices.size(); i++)
        {
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
     * Add SiDB cells to a SiDB cell-level layout based on a vector of cell indices.
     *
     * This function creates a copy of the provided layout (`skeleton_layout`) and adds SiDB cells to it
     * based on the cell indices provided in the `cell_indices` vector. If a cell at a specified index
     * is of type `sidb_technology::cell_type::EMPTY`, it is replaced with `sidb_technology::cell_type::NORMAL`
     * to represent the addition of SiDB cells.
     *
     * @param cell_indices A vector containing indices of cells in the layout to be added or modified.
     * @return A copy of the original layout (`skeleton_layout`) with SiDB cells added at specified indices.
     */
    [[nodiscard]] Lyt add_cells_to_layout_based_on_indices(const std::vector<std::size_t>& cell_indices)
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
 * The *Automatic Exhaustive Gate Designer* designs SiDB gate implementations based on a specified Boolean
 * function, a skeleton structure, canvas size, and a predetermined number of canvas SiDBs.
 *
 * It is composed of three steps:
 * 1. In the initial step, all possible distributions of `number_of_sidbs` SiDBs within a given canvas are
 * exhaustively determined. This ensures exhaustive coverage of every potential arrangement of ``number_of_sidbs`` SiDBs
 * across the canvas.
 * 2. The calculated SiDB distributions are then incorporated into the skeleton, resulting in the generation of distinct
 * SiDB layouts.
 * 3. The generated SiDB layouts then undergo an extensive simulation process. All input combinations possible for the
 * given Boolean function are used to verify if the logic is fulfilled.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param skeleton The skeleton layout used as a starting point for gate design.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Parameters for the *Automatic Exhaustive Gate Designer*.
 * @return A vector of SiDB layouts that fulfill the given Boolean function (truth table).
 */
template <typename Lyt, typename TT>
[[nodiscard]] std::vector<Lyt> design_gates_exhaustively(const Lyt& skeleton, const std::vector<TT>& spec,
                                                         const design_gates_exhaustively_params& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(skeleton.num_pis() > 0 && "skeleton needs input cells");
    assert(skeleton.num_pos() > 0 && "skeleton needs output cells");

    detail::design_gate_exhaustively_impl<Lyt, TT> p{skeleton, spec, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_DESIGN_GATES_EXHAUSTIVELY_HPP
