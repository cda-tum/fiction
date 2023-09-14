//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
#define FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP

#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/is_cell_level_layout_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include <combinations.h>

namespace fiction
{
/**
 * This struct contains parameters and settings used by the *Automatic Exhaustive
 * Gate Designer*. It is used to configure the simulation and design of SiDB gates.
 */
struct automatic_exhaustive_gate_designer_params
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
     * Truth table of the given gate (if layout is simulated in `gate-based` mode).
     */
    tt truth_table{};
    /**
     * Number of SiDBs placed in the canvas to create a working gate.
     */
    std::size_t number_of_sidbs = 0;
    /**
     * The simulation engine to be used for the operational domain computation.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::EXGS};
};

namespace detail
{
template <typename Lyt>
class automatic_exhaustive_gate_designer_impl
{
  public:
    /**
     * This constructor initializes an instance of the *Automatic Exhaustive Gate Designer*
     * implementation with the provided skeleton layout and configuration parameters.
     *
     * @param skeleton The skeleton layout used as a basis for gate design.
     * @param params   Parameters and settings for the gate designer.
     */
    automatic_exhaustive_gate_designer_impl(const Lyt& skeleton, automatic_exhaustive_gate_designer_params params) :
            skeleton_layout{skeleton},
            parameter{std::move(params)}
    {
        initialize();
    }

    /**
     * Run the exhaustive designing process of gate layouts.
     *
     * It adds each cell combination to the given skeleton, checks if positive charges can occur
     * in the layout, and determines if the layout is operationally valid based on specified parameters.
     */
    std::vector<Lyt> run()
    {
        const is_gate_layout_operational_params params_is_operational{parameter.phys_params, parameter.sim_engine};
        determine_all_combinations_of_given_sidbs_in_canvas();

        const auto add_combination_to_layout_and_check_operation =
            [this, &params_is_operational](const auto& combination) noexcept
        {
            auto layout_with_added_cells = add_cells_to_layout_based_on_indices(combination);
            if (!can_positive_charges_occur(layout_with_added_cells, parameter.phys_params))
            {
                if (is_gate_layout_operational(layout_with_added_cells, parameter.truth_table, params_is_operational)
                        .first == operational_status::OPERATIONAL)
                {
                    all_found_gate_layouts.push_back(layout_with_added_cells);
                }
            }
        };

        // Apply the add_combination_to_layout_and_check_operation function to each combination using std::for_each
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ all_combinations.cbegin(), all_combinations.cend(),
                      add_combination_to_layout_and_check_operation);

        return all_found_gate_layouts;
    }

  private:
    /**
     * The skeleton layout serves as a starting layout to which SiDBs are added to create unique SiDB layouts and, if
     * possible, working gates. It defines input and output wires.
     */
    Lyt skeleton_layout;
    /**
     * Parameters for the *Automatic Exhaustive Gate Designer*.
     */
    automatic_exhaustive_gate_designer_params parameter;
    /**
     * All cells within the canvas.
     */
    std::vector<siqad::coord_t> all_sidbs_in_cavas{};
    /**
     * Stores various combinations of SiDB indices, representing the cell positions
     * within the canvas. These combinations are used to distribute a specified number of SiDBs
     * across the canvas (all possible distributions are covered), typically from top to bottom and left to right.
     */
    std::vector<std::vector<std::size_t>> all_combinations{};
    /**
     * Number of canvas cells.
     */
    std::size_t number_of_canvas_cells{};
    /**
     * All found SiDB cell-level layouts that fulfill the given truth table.
     */
    std::vector<Lyt> all_found_gate_layouts{};

    /**
     * Initialize the *Automatic Exhaustive Gate Designer*.
     *
     * This function initializes various internal data structures and parameters required for
     * the automatic exhaustive gate designer. It calculates the set of SiDBs within the specified
     * canvas area, determines the total number of canvas cells, and prepares containers for
     * storing combinations and found layouts.
     */
    void initialize() noexcept
    {
        all_sidbs_in_cavas     = all_sidbs_in_spanned_area(parameter.canvas.first, parameter.canvas.second);
        number_of_canvas_cells = all_sidbs_in_cavas.size();
    }

    /**
     * Calculates all possible combinations of distributing the given number of SiDBs within a canvas
     * based on the provided parameters. It generates combinations of SiDB indices (representing the cell position in
     * the canvas from top to bottom and left to right) and stores them in the `all_combinations` vector. The number of
     * SiDBs in each combination is determined by `parameter.number_of_sidbs`.
     *
     * @note The function uses a combinatorial algorithm to generate the combinations.
     *
     */
    void determine_all_combinations_of_given_sidbs_in_canvas()
    {
        all_combinations = {};
        all_combinations.reserve(binomial_coefficient(number_of_canvas_cells, parameter.number_of_sidbs));
        std::vector<std::size_t> numbers(number_of_canvas_cells);
        std::iota(numbers.begin(), numbers.end(), 0);
        combinations::for_each_combination(
            numbers.begin(),
            numbers.begin() + static_cast<std::vector<std::size_t>::difference_type>(parameter.number_of_sidbs),
            numbers.end(),
            [this](const auto begin, const auto end)
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
    Lyt add_cells_to_layout_based_on_indices(const std::vector<std::size_t>& cell_indices)
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
 * The *Automatic Exhaustive Gate Designer* designs SiDB gate implementations for a given Boolean function, a given
 * canvas size, a given number of canvas SiDBs, and a given skeleton.
 *
 * It is composed of three steps:
 * 1. In the initial step, all possible distributions of ``number_of_sidbs`` SiDBs within a given canvas are
 * exhaustively determined. This ensures exhaustive coverage of every potential arrangement of ``number_of_sidbs`` SiDBs
 * across the canvas.
 * 2. The calculated SiDB distributions are then incorporated into the skeleton, resulting in the generation of distinct
 * SiDB layouts.
 * 3. The generated SiDB layouts then undergo an extensive simulation process. All input combinations possible for the
 * given Boolean function are used to verify if the logic is fulfilled.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param skeleton The skeleton layout used as a starting point for gate design.
 * @param params Parameters for the *Automatic Exhaustive Gate Designer*.
 * @return A vector of SiDB layouts that fulfill the given Boolean function (truth table).
 */
template <typename Lyt>
std::vector<Lyt> automatic_exhaustive_gate_designer(const Lyt&                                       skeleton,
                                                    const automatic_exhaustive_gate_designer_params& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    assert(skeleton.num_pis() > 0 && "skeleton needs input cells");
    assert(skeleton.num_pos() > 0 && "skeleton needs output cells");

    detail::automatic_exhaustive_gate_designer_impl<Lyt> p{skeleton, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
