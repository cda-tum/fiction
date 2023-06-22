//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_DEFECT_INFLUENCE_DISTANCE_HPP
#define FICTION_DEFECT_INFLUENCE_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <utility>

namespace fiction
{
/**
 * This function determines the minimum avoidance distance. This means that a defect has to be placed further away from
 * the layout than the given distance to not change the ground state of the layout.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which the influence distance is simulated.
 * @param defect Defect for which the infleunce distance is simulated.
 * @return Pair of the maximum influence distance (i.e. the defect should be placed further away than this value to
 * ensure correct behavior of e.g. a gate) with the corresponding position of the defect (helps to identify the
 * sensitive part of the layout).
 */
template <typename Lyt>
std::pair<double, typename Lyt::cell> influence_distance(Lyt& lyt, const sidb_defect& defect)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    coordinate<Lyt> min_defect_position{};
    double          avoidance_distance = 0;

    sidb_defect_layout              layout{lyt};
    std::vector<typename Lyt::cell> cells{};
    cells.reserve(lyt.num_cells());
    lyt.foreach_cell([&cells](const auto& cell) { cells.push_back(cell); });
    std::sort(cells.begin(), cells.end());

    const quickexact_params<Lyt> params{sidb_simulation_parameters{}, automatic_base_number_detection::OFF};
    auto                         simulation_results = quickexact(lyt, params);

    const auto min_energy          = minimum_energy(simulation_results.charge_distributions);
    uint64_t   charge_index_layout = 0;

    for (auto& lyt_result : simulation_results.charge_distributions)
    {
        if (round_to_n_decimal_places(lyt_result.get_system_energy(), 6) == round_to_n_decimal_places(min_energy, 6))
        {
            lyt_result.charge_distribution_to_index_simple();
            charge_index_layout = lyt_result.get_charge_index().first;
        }
    }

    const auto [nw, se] = bounding_box_siqad(layout);

    const auto north_west = nw - coordinate<Lyt>{10, -3};
    const auto se_cell    = se + coordinate<Lyt>{10, -3};

    auto defect_cell = north_west;
    while (defect_cell <= se_cell)
    {
        if (lyt.get_cell_type(defect_cell) == sidb_technology::cell_type::EMPTY)
        {
            layout.assign_sidb_defect(defect_cell, defect);
        }
        else
        {
            if (defect_cell.x < se_cell.x)
            {
                defect_cell.x += 1;
            }
            else if ((defect_cell.x == se_cell.x) && defect_cell.z == 0)
            {
                defect_cell.z += 1;
                defect_cell.x = north_west.x;
            }

            else if ((defect_cell.x == se_cell.x) && defect_cell.z == 1)
            {
                defect_cell.x = north_west.x;
                defect_cell.y += 1;
                defect_cell.z = 0;
            }
            continue;
        }

        const quickexact_params<sidb_defect_layout> params_defect{sidb_simulation_parameters{},
                                                                  automatic_base_number_detection::OFF};
        auto                                        simulation_result_defect = quickexact(layout, params_defect);

        const auto min_energy_defect          = minimum_energy(simulation_result_defect.charge_distributions);
        uint64_t   charge_index_defect_layout = 0;

        for (auto& lyt_defect : simulation_result_defect.charge_distributions)
        {
            if (round_to_n_decimal_places(lyt_defect.get_system_energy(), 6) ==
                round_to_n_decimal_places(min_energy_defect, 6))
            {
                lyt_defect.charge_distribution_to_index_simple();
                charge_index_defect_layout = lyt_defect.get_charge_index().first;
            }
        }

        if (charge_index_defect_layout != charge_index_layout)
        {
            double distance = std::numeric_limits<double>::max();
            lyt.foreach_cell(
                [&](const auto& cell)
                {
                    if (sidb_nanometer_distance<Lyt>(lyt, cell, defect_cell) < distance)
                    {
                        distance = sidb_nanometer_distance<Lyt>(lyt, cell, defect_cell);
                    }
                });
            if (distance > avoidance_distance)
            {
                min_defect_position = defect_cell;
                avoidance_distance  = distance;
                std::cout << avoidance_distance << std::endl;
            }
        }

        layout.assign_sidb_defect(defect_cell, sidb_defect{sidb_defect_type::NONE});

        if (defect_cell.x < se_cell.x)
        {
            defect_cell.x += 1;
        }
        else if ((defect_cell.x == se_cell.x) && defect_cell.z == 0)
        {
            defect_cell.z += 1;
            defect_cell.x = north_west.x;
        }

        else if ((defect_cell.x == se_cell.x) && defect_cell.z == 1)
        {
            defect_cell.x = north_west.x;
            defect_cell.y += 1;
            defect_cell.z = 0;
        }
    }

    return {avoidance_distance, min_defect_position};
}

}  // namespace fiction

#endif  // FICTION_DEFECT_INFLUENCE_DISTANCE_HPP
