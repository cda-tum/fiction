//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
#define FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <string_view>
#include <thread>
#include <unordered_set>
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
    ;
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

    void run() noexcept
    {
        Lyt                   skeleton_copy{skeleton_layout};
        std::vector<uint64_t> numbers(number_of_canvas_cells);
        std::iota(numbers.begin(), numbers.end(), 0);
        combinations::for_each_combination(numbers.begin(), numbers.begin() + parameter.number_of_sidbs, numbers.end(),
                                           [this, skeleton_copy](const auto begin, [[maybe_unused]] const auto end)
                                           {
                                               for (auto i = 0u; i < parameter.number_of_sidbs; ++i)
                                               {
                                                   add_cells_to_layout_based_on_indices(skeleton_copy, *(begin + i));
                                               }
                                               if (skeleton_copy.is_gate_layout_operational(
                                                       skeleton_copy, parameter.phys_params, parameter.truth_table))
                                               {
                                                   all_found_layouts.insert(skeleton_copy);
                                               };
                                               return false;  // keep looping
                                           });
    }

  private:
    Lyt skeleton_layout;

    const automatic_exhaustive_gate_designer_params& parameter;

    std::vector<siqad::coord_t> all_sidbs_in_cavas{};

    uint64_t number_of_canvas_cells{};

    std::set<Lyt> all_found_layouts{};

    void initialize() noexcept
    {
        all_sidbs_in_cavas     = all_sidbs_in_spanned_area(parameter.canvas.first, parameter.canvas.second);
        number_of_canvas_cells = all_sidbs_in_cavas.size();
    }

    /**
     * Modify an SiDB cell-level layout by adding cells based on specified indices.
     *
     * This function takes a cell-level SiDB layout, a list of all available cells, and a list of cell indices
     * to add to the layout. It iterates through the provided indices, assigns NORMAL cell type to non-empty cells,
     * and returns the modified layout.
     *
     * @tparam Lyt The cell-level SiDB layout type.
     * @param lyt The SiDB cell-level layout to be modified.
     * @param all_cells A vector containing all available cells.
     * @param cell_indices A vector of indices indicating which cells to add.
     * @return The modified layout with added cells.
     */
    void add_cells_to_layout_based_on_indices(Lyt& lyt, const uint64_t cell_indices) noexcept
    {
        assert(cell_indices < all_cells.size() && "cell indices are out-of-range");
        if (lyt.get_cell_type(all_sidbs_in_cavas[cell_indices]) == sidb_technology::cell_type::EMPTY)
        {
            lyt.assign_cell_type(all_sidbs_in_cavas[cell_indices], sidb_technology::cell_type::NORMAL);
        }
    }
};
}  // namespace detail

template <typename Lyt>
bool automatic_exhaustive_gate_designer(const Lyt&                                       skeleton,
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
