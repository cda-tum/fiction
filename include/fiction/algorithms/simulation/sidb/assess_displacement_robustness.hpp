//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
#define FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"
#include "fiction/io/print_layout.hpp"

#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <cstdint>
#include <cstdlib>
#include <set>

namespace fiction
{

template <typename Lyt>
struct displacement_robustness_domain
{
    locked_parallel_flat_hash_map<Lyt, operational_status> operational_values{};
};

template <typename TT>
struct displacement_robustness_params
{
    std::size_t            displacement_variations = 1;
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    is_operational_params  operational_params{};
    std::vector<TT>        tt{};
};

struct displacement_robustness_stats
{
    /**
     *
     */
    mockturtle::stopwatch<>::duration time_total{0};

    std::size_t num_operational_sidb_displacements{0};
};

namespace detail
{

template <typename Lyt, typename TT>
class displacement_robustness_domain_impl
{
  public:
    displacement_robustness_domain_impl(const Lyt& lyt, const displacement_robustness_params<TT>& ps, displacement_robustness_stats& st) noexcept :
            layout{lyt},
            params{ps},
            stats{st}
    {
        all_displacements_for_all_coordinates.reserve(layout.num_cells());
        layout.foreach_cell(
            [&](const auto& c)
            {
                const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(
                    {c.x - params.displacement_variations, c.y}, {c.x + params.displacement_variations, c.y});
                all_displacements_for_all_coordinates.push_back(all_coord);
                cells.push_back(c);
            });
    };

    void generate_combinations(const std::vector<std::vector<cell<Lyt>>>& all_displacements_for_all_coordinates,
                               std::vector<std::vector<cell<Lyt>>>& result,
                               std::vector<cell<Lyt>>& current_combination,
                               size_t index) {
        if (index == all_displacements_for_all_coordinates.size()) {
            // Base case: Add the current combination to the result
            result.push_back(current_combination);
            return;
        }

        // Iterate over all cells in the current vector
        for (const auto& cell : all_displacements_for_all_coordinates[index]) {
            // Add the current cell to the current combination
            current_combination.push_back(cell);
            // Recursive call for the next vector
            generate_combinations(all_displacements_for_all_coordinates, result, current_combination, index + 1);
            // Remove the last cell to generate a new combination
            current_combination.pop_back();
        }
    }

    std::vector<std::vector<cell<Lyt>>>
    generate_all_combinations(const std::vector<std::vector<cell<Lyt>>>& all_displacements_for_all_coordinates)
    {
        std::vector<std::vector<cell<Lyt>>> result;
        std::vector<cell<Lyt>> current_combination;
        generate_combinations(all_displacements_for_all_coordinates, result, current_combination, 0);
        return result;
    }


    [[nodiscard]] displacement_robustness_domain<Lyt> run() {
        const auto all_combinations = generate_all_combinations(all_displacements_for_all_coordinates);
        displacement_robustness_domain<Lyt> domain{};
        for (const auto& combination : all_combinations) {
            if (combination.size() != layout.num_cells()) {
                // Skip combinations that don't have the correct number of cells
                continue;
            }

            Lyt lyt{};
            size_t i = 0;
            // Iterate over each cell in the combination
            for (const auto& cell : combination) {
                // Assign cell type from the layout to the new layout
                lyt.assign_cell_type(cell, layout.get_cell_type(cells[i]));
                i++;
            }


            // Assess operational status of the new layout
            const auto op_status = is_operational(lyt, params.tt, params.operational_params);

            // Store the operational status in the domain
            domain.operational_values[lyt] = op_status.first;
            if (op_status.first == operational_status::OPERATIONAL) {
                stats.num_operational_sidb_displacements++;
                // print_layout(lyt, std::cout);
            }
        }

        return domain;
    }


  private:
    const Lyt&                                layout;
    const displacement_robustness_params<TT>& params;
    displacement_robustness_stats&            stats;
    // displacement_robustness_stats&                            stats;
    std::vector<std::vector<cell<Lyt>>> all_displacements_for_all_coordinates{};
    std::vector<cell<Lyt>>              cells{};
};

}  // namespace detail

template <typename Lyt, typename TT>
[[nodiscard]] displacement_robustness_domain<Lyt>
assess_displacement_robustness(const Lyt& layout, const displacement_robustness_params<TT>& params, displacement_robustness_stats* stats = nullptr)
{
    displacement_robustness_stats st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, params, st};
    // return p.run(p.generate_combinations(0));
    const auto result =  p.run();

    if (stats)
    {
        *stats = st;
    }
    return result;
}

}  // namespace fiction

#endif  // FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
