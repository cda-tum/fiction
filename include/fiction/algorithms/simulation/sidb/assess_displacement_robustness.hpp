//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
#define FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/io/print_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "fiction/utils/phmap_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct displacement_robustness_domain
{
    std::vector<std::pair<Lyt, operational_status>> operational_values{};
};

template <typename TT>
struct displacement_robustness_params
{
    std::pair<uint64_t, uint64_t>              displacement_variations = {1, 1};
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

    std::size_t num_non_operational_sidb_displacements{0};
};

template <typename Lyt>
std::size_t num_operational_layouts(const displacement_robustness_domain<Lyt>& domain)
{
    std::size_t operational_counter = 0;
    for (const auto& robust : domain.operational_values)
    {
        if (robust.second == operational_status::OPERATIONAL)
        {
            operational_counter++;
        }
    }
    return operational_counter;
}

template <typename Lyt>
std::size_t num_non_operational_layouts(const displacement_robustness_domain<Lyt>& domain)
{
    std::size_t non_operational_counter = 0;
    for (const auto& robust : domain.operational_values)
    {
        if (robust.second == operational_status::NON_OPERATIONAL)
        {
            non_operational_counter++;
        }
    }
    return non_operational_counter;
}

namespace detail
{

template <typename Lyt, typename TT>
class displacement_robustness_domain_impl
{
  public:
    displacement_robustness_domain_impl(const Lyt& lyt, const displacement_robustness_params<TT>& ps,
                                        displacement_robustness_stats& st) noexcept :
            layout{lyt},
            params{ps},
            stats{st}
    {
        all_displacements_for_all_coordinates.reserve(layout.num_cells());
        layout.foreach_cell(
            [&](const auto& c)
            {
                auto new_pos_se = c;
                auto new_pos_nw = c;
                new_pos_se.x -= params.displacement_variations.first;
                new_pos_se.y -= params.displacement_variations.second;
                new_pos_nw.x += params.displacement_variations.first;
                new_pos_nw.y += params.displacement_variations.second;

                const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(new_pos_se, new_pos_nw);
                all_displacements_for_all_coordinates.push_back(all_coord);
                cells.push_back(c);
            });
    };

    void generate_combinations(std::vector<std::vector<cell<Lyt>>>& result, std::vector<cell<Lyt>>& current_combination,
                               size_t index)
    {
        if (index == all_displacements_for_all_coordinates.size())
        {
            // Base case: Add the current combination to the result
            result.push_back(current_combination);
            return;
        }

        // Iterate over all cells in the current vector
        for (const auto& cell : all_displacements_for_all_coordinates[index])
        {
            // Add the current cell to the current combination
            current_combination.push_back(cell);
            // Recursive call for the next vector
            generate_combinations(result, current_combination, index + 1);
            // Remove the last cell to generate a new combination
            current_combination.pop_back();
        }
    }

    [[nodiscard]] std::vector<std::vector<cell<Lyt>>> generate_all_combinations()
    {
        std::vector<std::vector<cell<Lyt>>> result;
        std::vector<cell<Lyt>>              current_combination;
        generate_combinations(result, current_combination, 0);
        return result;
    }

    [[nodiscard]] displacement_robustness_domain<Lyt> run()
    {
        const auto                          all_combinations = generate_all_combinations();
        displacement_robustness_domain<Lyt> domain{};
        for (const auto& combination : all_combinations)
        {
            if (combination.size() != layout.num_cells())
            {
                // Skip combinations that don't have the correct number of cells
                continue;
            }

            Lyt    lyt{};
            size_t i = 0;
            // Iterate over each cell in the combination
            for (const auto& cell : combination)
            {
                // Assign cell type from the layout to the new layout
                lyt.assign_cell_type(cell, layout.get_cell_type(cells[i]));
                i++;
            }

            // Assess operational status of the new layout
            const auto op_status = is_operational(lyt, params.tt, params.operational_params);

            // Store the operational status in the domain
            domain.operational_values.emplace_back(lyt, op_status.first);
            if (op_status.first == operational_status::OPERATIONAL)
            {
                stats.num_operational_sidb_displacements++;
                // print_layout(lyt, std::cout);
            }
            else
            {
                stats.num_non_operational_sidb_displacements++;
            }
        }

        return domain;
    }

  private:
    const Lyt&                                     layout;
    const displacement_robustness_params<TT>& params;
    displacement_robustness_stats&                 stats;
    // displacement_robustness_stats&                            stats;
    std::vector<std::vector<cell<Lyt>>> all_displacements_for_all_coordinates{};
    std::vector<cell<Lyt>>              cells{};
};

}  // namespace detail

template <typename Lyt, typename TT>
[[nodiscard]] displacement_robustness_domain<Lyt>
assess_displacement_robustness(const Lyt& layout, const displacement_robustness_params<TT>& params,
                               displacement_robustness_stats* stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_stats                        st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, params, st};

    const auto result = p.run();

    if (stats)
    {
        *stats = st;
    }
    return result;
}

}  // namespace fiction

#endif  // FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
