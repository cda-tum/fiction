//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
#define FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP

#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/is_gate_layout_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/hash.hpp"
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
    uint64_t number_of_sidbs = 0;
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
    automatic_exhaustive_gate_designer_impl(const Lyt&                                       skeleton,
                                            const automatic_exhaustive_gate_designer_params& params) :
            skeleton_layout{skeleton},
            parameter{params}
    {
        initialize();
    }

    std::vector<Lyt> run()
    {
        const is_gate_layout_operational_params params_is_operational{parameter.phys_params, parameter.sim_engine};
        determine_all_combinations_of_k_cells();

        const auto add_combination_to_layout_and_check_operation =
            [this, &params_is_operational](const auto& combination) noexcept
        {
            auto layout_with_added_cells = add_cells_to_layout_based_on_indices(combination);
            if (can_positive_charges_occur(layout_with_added_cells, parameter.phys_params))
            {
                return false;
            }
            if (is_gate_layout_operational(layout_with_added_cells, parameter.truth_table, params_is_operational)
                    .first == operational_status::OPERATIONAL)
            {
                all_found_layouts.push_back(layout_with_added_cells);
            }
        };

        // Apply the add_combination_to_layout_and_check_operation function to each combination using std::for_each
        std::for_each(FICTION_EXECUTION_POLICY_PAR_UNSEQ all_combinations.cbegin(), all_combinations.cend(),
                      add_combination_to_layout_and_check_operation);

        return all_found_layouts;
    }

  private:
    const Lyt skeleton_layout;

    const automatic_exhaustive_gate_designer_params parameter;

    std::vector<siqad::coord_t> all_sidbs_in_cavas{};

    uint64_t number_of_canvas_cells{};

    std::vector<Lyt> all_found_layouts{};

    std::vector<std::vector<uint64_t>> all_combinations{};

    void initialize() noexcept
    {
        all_sidbs_in_cavas     = all_sidbs_in_spanned_area(parameter.canvas.first, parameter.canvas.second);
        number_of_canvas_cells = all_sidbs_in_cavas.size();
    }

    void determine_all_combinations_of_k_cells()
    {
        all_combinations = {};
        all_combinations.reserve(binomial_coefficient(number_of_canvas_cells, parameter.number_of_sidbs));
        std::vector<uint64_t> numbers(number_of_canvas_cells);
        std::iota(numbers.begin(), numbers.end(), 0);
        combinations::for_each_combination(numbers.begin(), numbers.begin() + parameter.number_of_sidbs, numbers.end(),
                                           [this](const auto begin, [[maybe_unused]] const auto end)
                                           {
                                               std::vector<uint64_t> combination{};
                                               combination.reserve(parameter.number_of_sidbs);
                                               for (auto i = 0u; i < parameter.number_of_sidbs; ++i)
                                               {
                                                   combination.push_back(*(begin + i));
                                               }
                                               all_combinations.push_back(combination);
                                               return false;  // keep looping
                                           });
    }

    Lyt add_cells_to_layout_based_on_indices(const std::vector<uint64_t>& cell_indices)
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
 * This *Automatic_Exhaustive_Gate_Designer* designs SiDB gate implementations for a given Boolean function, a given
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

    detail::automatic_exhaustive_gate_designer_impl<Lyt> p{skeleton, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
