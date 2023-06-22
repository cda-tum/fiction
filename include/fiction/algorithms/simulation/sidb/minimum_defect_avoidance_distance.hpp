//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_MINIMUM_DEFECT_AVOIDANCE_DISTANCE_HPP
#define FICTION_MINIMUM_DEFECT_AVOIDANCE_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_surface.hpp"

#include <utility>

namespace fiction
{

using sidb_layout = sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>;

template <typename Lyt>
std::pair<double, typename Lyt::cell> minimum_avoidance_distance(Lyt& lyt, const sidb_defect& defect)
{
    coordinate<Lyt> min_defect_position{};
    double          min_distance = 1000;

    sidb_layout                     layout{lyt};
    std::vector<typename Lyt::cell> cells{};
    cells.reserve(lyt.num_cells());
    lyt.foreach_cell([&](const auto& cell) { cells.push_back(cell); });
    std::sort(cells.begin(), cells.end());

    const quickexact_params<Lyt> params{sidb_simulation_parameters{}, automatic_base_number_detection::OFF};
    auto                         simulation_results = quickexact(layout, params);

    const auto min_energy = minimum_energy(simulation_results.charge_distributions);
    uint64_t   charge_index_layout;

    for (const auto& lyt_result : simulation_results.charge_distributions)
    {
        if (round_to_n_decimal_places(lyt_result.get_system_energy(), 6) == round_to_n_decimal_places(min_energy, 6))
        {
            lyt_result.charge_distribution_to_index_simple();
            charge_index_layout = lyt_result.get_charge_index().first;
        }
    }

    //    bounding_box_2d box{lyt};
    //    const auto      nw_cell = box.get_min() - coordinate<Lyt>{5, 2};
    //    const auto      se_cell = box.get_max() + coordinate<Lyt>{5, 2};

    const auto nw_cell = cells.front() + coordinate<Lyt>{10, 2};
    const auto se_cell = cells.back() - coordinate<Lyt>{10, 2};

    auto defect_cell = nw_cell;
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
                defect_cell.x = nw_cell.x;
            }

            else if ((defect_cell.x == se_cell.x) && defect_cell.z == 1)
            {
                defect_cell.x = nw_cell.x;
                defect_cell.y += 1;
                defect_cell.z = 0;
            }
            continue;
        }

        std::cout << "x: " << std::to_string(defect_cell.x);
        std::cout << " | y: " << std::to_string(defect_cell.y);
        std::cout << " | z: " << std::to_string(defect_cell.z) << std::endl;

        //        const critical_temperature_params params{simulation_engine::EXACT,
        //                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
        //                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}},
        //                                                 0.99,
        //                                                 350,
        //                                                 create_or_tt(),
        //                                                 0};
        //        critical_temperature_stats<Lyt>   criticalstats{};
        //        critical_temperature(lyt, params, &criticalstats);

        auto simulation_result_defect = quickexact(layout, params);

        const auto min_energy_defect = minimum_energy(simulation_result_defect.charge_distributions);
        uint64_t   charge_index_defect_layout;

        for (auto& lyt_defect : simulation_result_defect.charge_distributions)
        {
            if (round_to_n_decimal_places(lyt_defect.get_system_energy(), 6) ==
                round_to_n_decimal_places(min_energy_defect, 6))
            {
                lyt_defect.charge_distribution_to_index_simple();
                charge_index_defect_layout = lyt_defect.get_charge_index().first;
            }
        }

        if (charge_index_defect_layout == charge_index_layout)
        {

            double distance = 1000;
            lyt.foreach_cell(
                [&](const auto& cell)
                {
                    if (sidb_nanometer_distance<Lyt>(lyt, cell, defect_cell) < distance)
                    {
                        distance = sidb_nanometer_distance<Lyt>(lyt, cell, defect_cell);
                    }
                });
            if (distance < min_distance)
            {
                min_defect_position = defect_cell;
                min_distance        = distance;
                std::cout << min_distance << std::endl;
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
            defect_cell.x = nw_cell.x;
        }

        else if ((defect_cell.x == se_cell.x) && defect_cell.z == 1)
        {
            defect_cell.x = nw_cell.x;
            defect_cell.y += 1;
            defect_cell.z = 0;
        }
    }

    return {min_distance, min_defect_position};
}

}  // namespace fiction
#endif  // FICTION_MINIMUM_DEFECT_AVOIDANCE_DISTANCE_HPP
