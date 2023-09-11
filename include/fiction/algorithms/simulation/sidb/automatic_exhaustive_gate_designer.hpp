//
// Created by Jan Drewniok on 11.09.23.
//

#ifndef FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
#define FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/io/read_sqd_layout.hpp"
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

namespace fiction
{

struct automatic_exhaustive_gate_designer_params
{
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
        Lyt skeleton_copy{skeleton_layout};
        for (const auto& combinations : all_combinations_of_n_sidbs_in_cavas)
        {
            add_cells_to_layout_based_on_indices(skeleton_copy, all_sidbs_in_cavas, combinations);
        }
    }

  private:
    Lyt skeleton_layout;

    const automatic_exhaustive_gate_designer_params& parameter;

    std::vector<std::vector<typename Lyt::cell>> all_cells_combinations_added{};

    std::vector<siqad::coord_t> all_sidbs_in_cavas{};

    std::vector<std::vector<uint64_t>> all_combinations_of_n_sidbs_in_cavas{};

    void initialize() noexcept
    {
        all_sidbs_in_cavas = all_sidbs_in_spanned_area(parameter.canvas.first, parameter.canvas.second);
        all_combinations_of_n_sidbs_in_cavas =
            generate_unsigned_integer_combinations(parameter.number_of_sidbs, all_sidbs_in_cavas.size());
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

    detail::automatic_exhaustive_gate_designer_impl<Lyt> p{lyt, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER_HPP
