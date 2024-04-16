//
// Created by Jan Drewniok on 21.03.24.
//

#ifndef FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
#define FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct sidb_gate_displacement_robustness_domain
{
    /**
     * Represents a domain of displacement robustness for layouts resulting from applying a displacement to a given gate
     * layout.
     */
    std::vector<std::pair<Lyt, operational_status>> operational_values{};
};

template <typename Lyt>
struct displacement_robustness_params
{
    /**
     * The atomic displacement in x- and y-direction, respectively. The default value is (1, 1).
     */
    std::pair<uint64_t, uint64_t> displacement_variations = {1, 1};
    /**
     * The simulation engine used.
     */
    sidb_simulation_engine sim_engine{sidb_simulation_engine::QUICKEXACT};
    /**
     * Parameters to check the operation status.
     */
    is_operational_params operational_params{};
    /**
     * Cells/SiDBs of the layout which are not affected by variations.
     */
    std::set<cell<Lyt>> fixed_cells{};
};

struct displacement_robustness_stats
{
    /**
     * Total runtime in sec. to determine the robustness of the SiDB gate.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * The number of operational SiDB gates resulting from the given layout by displacements.
     */
    std::size_t num_operational_sidb_displacements{0};
    /**
     * The number of non-operational SiDB gates resulting from the given layout by displacements.
     */
    std::size_t num_non_operational_sidb_displacements{0};
};

/**
 * Counts the number of operational SiDB gates in the given displacement robustness domain.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @param domain The displacement robustness domain to analyze.
 * @return The number of operational layouts within the domain.
 */
template <typename Lyt>
std::size_t num_operational_layouts(const sidb_gate_displacement_robustness_domain<Lyt>& domain)
{
    return static_cast<std::size_t>(std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                                                  [](const auto& robust)
                                                  { return robust.second == operational_status::OPERATIONAL; }));
}

/**
 * Counts the number of non-operational SiDB gates in the given displacement robustness domain.
 *
 * @tparam Lyt The SiDB cell-level layout type.
 * @param domain The displacement robustness domain to analyze.
 * @return The number of non-operational layouts within the domain.
 */
template <typename Lyt>
std::size_t num_non_operational_layouts(const sidb_gate_displacement_robustness_domain<Lyt>& domain)
{
    return static_cast<std::size_t>(std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                                                  [](const auto& robust)
                                                  { return robust.second == operational_status::NON_OPERATIONAL; }));
}

namespace detail
{

template <typename Lyt, typename TT>
class displacement_robustness_domain_impl
{
  public:
    displacement_robustness_domain_impl(const Lyt& lyt, const std::vector<TT>& spec,
                                        const displacement_robustness_params<Lyt>& ps,
                                        displacement_robustness_stats&             st) noexcept :
            layout{lyt},
            params{ps},
            stats{st},
            truth_table{spec}
    {
        for (const auto& c : params.fixed_cells)
        {
            assert(!layout.is_empty_cell(c) && "Not all fixed cells are part of the layout");
        }
        all_displacements_for_all_coordinates.reserve(layout.num_cells());
        all_displacements_layouts.reserve((layout.num_cells() - params.fixed_cells.size() + 1) *
                                          (params.displacement_variations.first + 1) *
                                          (params.displacement_variations.second + 1));

        layout.foreach_cell(
            [&](const auto& c)
            {
                if constexpr (has_siqad_coord_v<Lyt>)
                {
                    auto new_pos_se = siqad::to_fiction_coord<cube::coord_t>(c);
                    auto new_pos_nw = siqad::to_fiction_coord<cube::coord_t>(c);

                    // the cell c is a fixed cell, i.e., no displacement is considered.
                    if (params.fixed_cells.find(c) == params.fixed_cells.end())
                    {
                        new_pos_se.x -= static_cast<decltype(new_pos_se.x)>(params.displacement_variations.first);
                        new_pos_se.y -= static_cast<decltype(new_pos_se.y)>(params.displacement_variations.second);
                        new_pos_nw.x += static_cast<decltype(new_pos_nw.x)>(params.displacement_variations.first);
                        new_pos_nw.y += static_cast<decltype(new_pos_nw.y)>(params.displacement_variations.second);
                    }
                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(
                        siqad::to_siqad_coord(new_pos_se), siqad::to_siqad_coord(new_pos_nw));
                    all_displacements_for_all_coordinates.push_back(all_coord);
                    cells.push_back(c);
                }
                else
                {
                    auto new_pos_se = c;
                    auto new_pos_nw = c;
                    if (params.fixed_cells.find(c) == params.fixed_cells.end())
                    {
                        new_pos_se.x -= params.displacement_variations.first;
                        new_pos_se.y -= params.displacement_variations.second;
                        new_pos_nw.x += params.displacement_variations.first;
                        new_pos_nw.y += params.displacement_variations.second;
                    }

                    const auto all_coord = all_coordinates_in_spanned_area<cell<Lyt>>(new_pos_se, new_pos_nw);
                    all_displacements_for_all_coordinates.push_back(all_coord);
                    cells.push_back(c);
                }
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

    [[nodiscard]] sidb_gate_displacement_robustness_domain<Lyt> run()
    {
        const auto                                    all_combinations = generate_all_combinations();
        sidb_gate_displacement_robustness_domain<Lyt> domain{};
        for (const auto& combination : all_combinations)
        {
            if (combination.size() != layout.num_cells())
            {
                // Skip combinations that don't have the correct number of cells
                continue;
            }

            Lyt         lyt{};
            std::size_t i = 0;

            // Iterate over each cell in the combination
            for (const auto& cell : combination)
            {
                // Assign a cell type from the layout to the new layout
                lyt.assign_cell_type(cell, layout.get_cell_type(cells[i]));
                i++;
            }

            if (lyt.num_cells() != layout.num_cells())
            {
                continue;
            }

            // Assess the operational status of the new layout
            const auto op_status = is_operational(lyt, truth_table, params.operational_params);

            // Store the operational status in the domain
            domain.operational_values.emplace_back(lyt, op_status.first);
            if (op_status.first == operational_status::OPERATIONAL)
            {
                stats.num_operational_sidb_displacements++;
            }
            else
            {
                stats.num_non_operational_sidb_displacements++;
            }
        }

        return domain;
    }

  private:
    /**
     * The SiDB cell-level layout/SiDB gate for which the displacement robustness calculation is performed.
     */
    const Lyt& layout;
    /**
     * The parameters for the displacement robustness computation.
     */
    const displacement_robustness_params<Lyt>& params;
    /**
     * The statistics of the displacement robustness computation.
     */
    displacement_robustness_stats&      stats;
    std::vector<std::vector<cell<Lyt>>> all_displacements_for_all_coordinates{};
    std::vector<Lyt>                    all_displacements_layouts{};
    std::vector<cell<Lyt>>              cells{};
    /**
     * The specification of the layout.
     */
    const std::vector<TT>& truth_table;
};

}  // namespace detail

/**
 * This function analyzes the operational status of all possible displacements of the SiDBs in the SiDB gate,
 * based on the provided truth table specification and displacement robustness computation parameters.
 *
 * @tparam Lyt The type of SiDB cell-level layout.
 * @param spec The truth table specification of the gate.
 * @param params The parameters for the displacement robustness computation.
 * @param stats Statistics related to the displacement robustness computation.
 * @return The displacement robustness of the SiDB gate.
 */
template <typename Lyt, typename TT>
[[nodiscard]] sidb_gate_displacement_robustness_domain<Lyt>
assess_sidb_gate_displacement_robustness(const Lyt& layout, const std::vector<TT>& spec,
                                         const displacement_robustness_params<Lyt>& params,
                                         displacement_robustness_stats*             stats = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    displacement_robustness_stats                        st{};
    detail::displacement_robustness_domain_impl<Lyt, TT> p{layout, spec, params, st};

    const auto result = p.run();

    if (stats)
    {
        *stats = st;
    }
    return result;
}

}  // namespace fiction

#endif  // FICTION_ASSESS_DISPLACEMENT_ROBUSTNESS_HPP
