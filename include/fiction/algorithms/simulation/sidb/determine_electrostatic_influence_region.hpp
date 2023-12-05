//
// Created by Jan Drewniok on 03.12.23.
//

#ifndef FICTION_DETERMINE_ELECTROSTATIC_INFLUENCE_REGION_HPP
#define FICTION_DETERMINE_ELECTROSTATIC_INFLUENCE_REGION_HPP

#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

namespace detail
{

template <typename Lyt>
class determine_influence_region_impl
{
  public:
    determine_influence_region_impl(const Lyt& lyt, const Lyt& sublyt, const quickexact_params<cell<Lyt>>& params) :
            layout{lyt},
            sublayout{sublyt},
            quickexact_parameter{params}
    {
        assert(is_sublayout_of_layout() && "sublyt is not a sub-layout of lyt");
        collect_cells_of_layout_without_subcells();
        simulate_ground_state_charge_indices_of_sublayout();
    }
    /**
     * This function determines the influence zone, i.e., the region where SiDBs of the layout still affect the ground
     * state of the sublayout.
     *
     * @return A pair of cells representing the northwest and southeast corners of the influence zone. This means that
     * all cells of the layout outside of the influence region do not influence the ground state of the sublayout. Thus,
     * their charge states can be neglected when simulating the ground state of the sublayout.
     */
    std::pair<typename Lyt::cell, typename Lyt::cell> run() noexcept
    {
        const bounding_box_2d<Lyt> bb{layout};
        const auto                 min_cell_layout = bb.get_min();
        const auto                 max_cell_layout = bb.get_max();

        const bounding_box_2d<Lyt> bb_sub{sublayout};
        const auto                 min_cell_sublayout = bb_sub.get_min();
        const auto                 max_cell_sublayout = bb_sub.get_max();

        auto nw_influence_zone = min_cell_sublayout;
        auto se_influence_zone = max_cell_sublayout;

        const auto all_cells_in_sublayout_box = all_coordinates_in_spanned_area(min_cell_sublayout, max_cell_sublayout);

        sidb_surface<Lyt> layout_with_non_influencing_cells{sublayout};

        bool found_non_influencing_region = false;

        // IMPORTANT: We treat SiDBs with fixed charge states as atomic defects.
        for (const auto& cell : cells_without_sublayout_cells)
        {
            layout_with_non_influencing_cells.assign_sidb_defect(
                cell, sidb_defect{sidb_defect_type::UNKNOWN, -1, quickexact_parameter.physical_parameters.epsilon_r,
                                  quickexact_parameter.physical_parameters.lambda_tf});
        }

        while (!found_non_influencing_region)
        {
            const auto charge_indices = simulate_charge_indices_of_the_ground_states(layout_with_non_influencing_cells);

            if (charge_indices == ground_state_charge_indices_of_sublyt)
            {
                found_non_influencing_region = true;
            }
            else
            {
                // the influence region in the northwest direction is extended further towards the northwest
                if (nw_influence_zone > min_cell_layout)
                {
                    if (nw_influence_zone.x == min_cell_layout.x)
                    {
                        nw_influence_zone.y = nw_influence_zone.y - 1;
                    }
                    else if (nw_influence_zone.y == min_cell_layout.y)
                    {
                        nw_influence_zone.x = nw_influence_zone.x - 1;
                    }
                    else
                    {
                        nw_influence_zone.x = nw_influence_zone.x - 1;
                        nw_influence_zone.y = nw_influence_zone.y - 1;
                    }
                }
                // the influence region in the southeast direction is extended further towards the southeast
                if (se_influence_zone < max_cell_layout)
                {
                    if (se_influence_zone.x == max_cell_layout.x)
                    {
                        se_influence_zone.y = se_influence_zone.y + 1;
                    }
                    else if (se_influence_zone.y == max_cell_layout.y)
                    {
                        se_influence_zone.x = se_influence_zone.x + 1;
                    }
                    else
                    {
                        se_influence_zone.x = se_influence_zone.x + 1;
                        se_influence_zone.y = se_influence_zone.y + 1;
                    }
                }

                for (const auto& cell : cells_without_sublayout_cells)
                {
                    if ((cell.x >= nw_influence_zone.x) && (cell.y >= nw_influence_zone.y) &&
                        (cell.x <= se_influence_zone.x) && (cell.y <= se_influence_zone.y) &&
                        std::find(all_cells_in_sublayout_box.cbegin(), all_cells_in_sublayout_box.cend(), cell) ==
                            all_cells_in_sublayout_box.end())
                    {
                        // IMPORTANT: All SiDBs that are within the influence region are deactivated to check if the
                        // remaining SiDBs do not change the ground state of the sublayout.
                        layout_with_non_influencing_cells.assign_sidb_defect(cell, sidb_defect{sidb_defect_type::NONE});
                    }
                }
            }
        }
        return {nw_influence_zone, se_influence_zone};
    }

  private:
    /**
     * This function performs a simulation of the specified layout using *QuickExact*.
     * It identifies the ground state charge distribution and their charge indices are returned.
     *
     * @param layout The layout to simulate.
     * @return A set containing the charge indices of ground states.
     */
    [[nodiscard]] std::set<uint64_t> simulate_charge_indices_of_the_ground_states(
        const sidb_surface<Lyt>& layout_with_non_influencing_cells) const noexcept
    {
        const auto         simulation_results  = quickexact(layout_with_non_influencing_cells, quickexact_parameter);
        const auto         ground_state_energy = minimum_energy(simulation_results.charge_distributions);
        std::set<uint64_t> charge_indices      = {};
        for (const auto& lyt : simulation_results.charge_distributions)
        {
            if (std::fabs(lyt.get_system_energy() - ground_state_energy) < std::numeric_limits<double>::epsilon())
            {
                charge_indices.insert(lyt.get_charge_index_and_base().first);
            }
        }
        return charge_indices;
    }
    /**
     * This function iterates through all cells in the specified layout and sublayout.
     * It identifies cells in the layout that are not present in the sublayout and collects
     * them.
     *
     * @tparam Lyt The layout type.
     */
    void collect_cells_of_layout_without_subcells() noexcept
    {
        std::vector<typename Lyt::cell> all_sidbs = {};
        all_sidbs.reserve(layout.num_cells());
        layout.foreach_cell([&all_sidbs](const auto& c1) { all_sidbs.push_back(c1); });

        std::vector<typename Lyt::cell> all_sidbs_sublayout = {};
        all_sidbs_sublayout.reserve(sublayout.num_cells());
        sublayout.foreach_cell([&all_sidbs_sublayout](const auto& c1) { all_sidbs_sublayout.push_back(c1); });

        for (const auto& cell : all_sidbs)
        {
            auto it = std::find(all_sidbs_sublayout.begin(), all_sidbs_sublayout.end(), cell);

            if (it == all_sidbs_sublayout.end())
            {
                cells_without_sublayout_cells.insert(cell);
            }
        }
    }
    /**
     * This function simulates the ground state of the sublayout using *QuickExact*.
     * The function then collects the charge indices of all charge distributions being considered as ground states.
     *
     * @tparam Lyt The layout type.
     */
    void simulate_ground_state_charge_indices_of_sublayout() noexcept
    {
        const auto simulation_results = quickexact(sublayout, quickexact_parameter);
        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state_energy = minimum_energy(simulation_results.charge_distributions);
        for (const auto& lyt : simulation_results.charge_distributions)
        {
            if (std::fabs(lyt.get_system_energy() - ground_state_energy) < std::numeric_limits<double>::epsilon())
            {
                ground_state_charge_indices_of_sublyt.insert(lyt.get_charge_index_and_base().first);
            }
        }
    }
    /**
     * It determines if every cell in the sublayout is part of the layout.
     *
     * @tparam Lyt The type of the layout.
     * @return `true` if the sublayout is a subset of the layout, `false` otherwise.
     */
    [[nodiscard]] bool is_sublayout_of_layout() const noexcept
    {
        std::vector<typename Lyt::cell> all_sidbs = {};
        all_sidbs.reserve(layout.num_cells());
        layout.foreach_cell([&all_sidbs](const auto& c1) { all_sidbs.push_back(c1); });

        std::vector<typename Lyt::cell> all_sidbs_sublayout = {};
        all_sidbs_sublayout.reserve(layout.num_cells());
        sublayout.foreach_cell([&all_sidbs_sublayout](const auto& c1) { all_sidbs_sublayout.push_back(c1); });

        for (const auto& cell : all_sidbs_sublayout)
        {
            auto it = std::find(all_sidbs.begin(), all_sidbs.end(), cell);

            if (it == all_sidbs.end())
            {
                return false;
            }
        }
        return true;
    }
    /**
     * Complete Layout.
     */
    Lyt layout{};
    /**
     * Sublayout of layout.
     */
    Lyt sublayout{};
    /**
     * Simulation parameters.
     */
    quickexact_params<cell<Lyt>> quickexact_parameter{};
    /**
     * Charge indices of the ground state of the sublayout without neighboring SiDBs, i.e., undisturbed.
     */
    std::set<uint64_t> ground_state_charge_indices_of_sublyt{};
    /**
     * All cells of the layout, excluding the cells of the sublayout.
     */
    std::set<typename Lyt::cell> cells_without_sublayout_cells{};
};

}  // namespace detail

/**
 * This function identifies the influence zone for a given sublayout within a larger layout. The influence zone
 * encompasses the neighboring SiDBs whose charge states can potentially affect the ground state of the sublayout.
 * SiDBs outside the influence zone are considered to have no significant impact on the sublayout's ground state,
 * and their charge states do not need to be considered during the simulation of the sublayout.
 *
 * @tparam Lyt The type representing the layout.
 * @param layout Layout which includes the sublayout.
 * @param sublayout Sublayout whose influence region is simulated and which is part of the layout.
 * @param params *QuickExact* parameters for the physical simulation.
 * @return A pair of cells representing the northwest and southeast corners of the determined influence zone.
 */
template <typename Lyt>
std::pair<typename Lyt::cell, typename Lyt::cell>
determine_influence_region(const Lyt& layout, const Lyt& sublayout,
                           const quickexact_params<cell<Lyt>>& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::determine_influence_region_impl<Lyt> p{layout, sublayout, params};
    return p.run();
}

}  // namespace fiction

#endif  // FICTION_DETERMINE_ELECTROSTATIC_INFLUENCE_REGION_HPP
