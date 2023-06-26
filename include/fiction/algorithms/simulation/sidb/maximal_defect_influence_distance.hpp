//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_MAXIMAL_DEFECT_INFLUENCE_DISTANCE_HPP
#define FICTION_MAXIMAL_DEFECT_INFLUENCE_DISTANCE_HPP

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
 * This struct stores the parameters for the `maximal_defect_influence_distance` algorithm.
 */
template <typename Lyt>
struct maximal_defect_influence_distance_params
{
    /**
     * The defect used to calculate the maximal defect influence distance.
     */
    sidb_defect defect{};
    /**
     * Physical simulation parameters.
     */
    sidb_simulation_parameters physical_params{};
    /**
     * The coordinate describes the width and height of the area around the gate, which is
     * also used to place defects (given in siqad coordinates).
     * */
    coordinate<Lyt> additional_scanning_area{50, 6};
};

/**
 * This function determines the maximum distance at which a placed defect can still affect the layout (i.e. different
 * ground state). This means that a defect must be placed further away from the distance from the layout in order not to
 * change the layout's ground state.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which the influence distance is simulated.
 * @param sim_params Parameters which are used to determine the defect influence distance.
 * @return Pair of the maximum influence distance (i.e. the defect should be placed further away than this value to
 * ensure correct behavior of e.g. a gate) with the corresponding position of the defect (helps to identify the
 * sensitive part of the layout).
 */
template <typename Lyt>
std::pair<double, typename Lyt::cell>
maximal_defect_influence_distance(Lyt& lyt, const maximal_defect_influence_distance_params<Lyt>& sim_params)
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

    const quickexact_params<Lyt> params{sim_params.physical_params, automatic_base_number_detection::OFF};
    auto                         simulation_results = quickexact(lyt, params);

    const auto min_energy          = minimum_energy(simulation_results.charge_distributions);
    uint64_t   charge_index_layout = 0;

    for (auto& lyt_result : simulation_results.charge_distributions)
    {
        if (round_to_n_decimal_places(lyt_result.get_system_energy(), 6) == round_to_n_decimal_places(min_energy, 6))
        {
            lyt_result.charge_distribution_to_index_general();
            charge_index_layout = lyt_result.get_charge_index().first;
        }
    }

    const auto [nw, se] = bounding_box_siqad(layout);

    auto north_west = coordinate<Lyt>{};
    auto south_east = coordinate<Lyt>{};

    north_west.x = nw.x - sim_params.additional_scanning_area.x;
    north_west.y = nw.y - sim_params.additional_scanning_area.y;

    std::cout << "nw x: " << std::to_string(north_west.x);
    std::cout << " | nw y: " << std::to_string(north_west.y);
    std::cout << " | nw z: " << std::to_string(north_west.z) << std::endl;

    south_east.x = se.x + sim_params.additional_scanning_area.x;
    south_east.y = se.y + sim_params.additional_scanning_area.y;

    std::cout << "se x: " << std::to_string(south_east.x);
    std::cout << " | se y: " << std::to_string(south_east.y);
    std::cout << " | se z: " << std::to_string(south_east.z) << std::endl;

    auto defect_cell = north_west;
    while (defect_cell <= south_east)
    {
        if (lyt.get_cell_type(defect_cell) == sidb_technology::cell_type::EMPTY)
        {
            layout.assign_sidb_defect(defect_cell, sim_params.defect);
        }
        else
        {
            if (defect_cell.x < south_east.x)
            {
                defect_cell.x += 1;
            }
            else if ((defect_cell.x == south_east.x) && defect_cell.z == 0)
            {
                defect_cell.z += 1;
                defect_cell.x = north_west.x;
            }

            else if ((defect_cell.x == south_east.x) && defect_cell.z == 1)
            {
                defect_cell.x = north_west.x;
                defect_cell.y += 1;
                defect_cell.z = 0;
            }
            continue;
        }

        const quickexact_params<sidb_defect_layout> params_defect{sim_params.physical_params,
                                                                  automatic_base_number_detection::OFF};
        auto                                        simulation_result_defect = quickexact(layout, params_defect);

        const auto min_energy_defect          = minimum_energy(simulation_result_defect.charge_distributions);
        uint64_t   charge_index_defect_layout = 0;

        for (auto& lyt_defect : simulation_result_defect.charge_distributions)
        {
            if (round_to_n_decimal_places(lyt_defect.get_system_energy(), 6) ==
                round_to_n_decimal_places(min_energy_defect, 6))
            {
                lyt_defect.charge_distribution_to_index_general();
                charge_index_defect_layout = lyt_defect.get_charge_index().first;
            }
        }

        if (charge_index_defect_layout != charge_index_layout)
        {
            double distance = std::numeric_limits<double>::max();
            lyt.foreach_cell(
                [&lyt, &defect_cell, &distance](const auto& cell)
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

        if (defect_cell.x < south_east.x)
        {
            defect_cell.x += 1;
        }
        else if ((defect_cell.x == south_east.x) && defect_cell.z == 0)
        {
            defect_cell.z += 1;
            defect_cell.x = north_west.x;
        }

        else if ((defect_cell.x == south_east.x) && defect_cell.z == 1)
        {
            defect_cell.x = north_west.x;
            defect_cell.y += 1;
            defect_cell.z = 0;
        }
    }

    return {avoidance_distance, min_defect_position};
}

}  // namespace fiction

#endif  // FICTION_MAXIMAL_DEFECT_INFLUENCE_DISTANCE_HPP
