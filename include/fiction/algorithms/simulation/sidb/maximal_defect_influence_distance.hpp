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

#include <mutex>
#include <thread>
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
    /**
     * Number of threads to spawn. By default the number of threads is set to the number of available hardware threads.
     */
    uint64_t number_threads{std::thread::hardware_concurrency()};
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

    const quickexact_params<sidb_defect_layout> params_defect{sim_params.physical_params,
                                                              automatic_base_number_detection::OFF};

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

    //    std::cout << "nw x: " << std::to_string(north_west.x);
    //    std::cout << " | nw y: " << std::to_string(north_west.y);
    //    std::cout << " | nw z: " << std::to_string(north_west.z) << std::endl;

    south_east.x = se.x + sim_params.additional_scanning_area.x;
    south_east.y = se.y + sim_params.additional_scanning_area.y;

    //    std::cout << "se x: " << std::to_string(south_east.x);
    //    std::cout << " | se y: " << std::to_string(south_east.y);
    //    std::cout << " | se z: " << std::to_string(south_east.z) << std::endl;

    auto                            defect_cell = north_west;
    std::vector<typename Lyt::cell> defect_cells{};

    while (defect_cell <= south_east)
    {
        if (lyt.get_cell_type(defect_cell) == sidb_technology::cell_type::EMPTY)
        {
            defect_cells.push_back(defect_cell);
        }

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

    // If the number of threads is initially set to zero, the simulation is run with one thread.
    const uint64_t num_threads = std::max(sim_params.number_threads, uint64_t{1});

    std::vector<std::thread> threads{};
    threads.reserve(num_threads);
    std::mutex mutex{};  // used to control access to shared resources

    const auto number_per_thread = (defect_cells.size() - (defect_cells.size() % num_threads)) / num_threads;
    const auto number_last       = defect_cells.size() % num_threads;

    for (uint64_t z = 0u; z < num_threads; ++z)
    {
        threads.emplace_back(
            [&, z, defect_cells]
            {
                for (auto i = z * number_per_thread; i < (z + 1) * number_per_thread; i++)
                {
                    const auto defect = defect_cells[i];

                    sidb_defect_layout lyt_defect{};

                    if (layout.num_cells() != 0)
                    {
                        layout.foreach_cell([&layout, &lyt_defect](const auto& cell)
                                            { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });
                    }

                    lyt_defect.assign_sidb_defect(defect, sim_params.defect);

                    auto simulation_result_defect = quickexact(lyt_defect, params_defect);

                    const auto min_energy_defect = minimum_energy(simulation_result_defect.charge_distributions);
                    uint64_t   charge_index_defect_layout = 0;

                    for (auto& lyt_simulation_with_defect : simulation_result_defect.charge_distributions)
                    {
                        if (round_to_n_decimal_places(lyt_simulation_with_defect.get_system_energy(), 6) ==
                            round_to_n_decimal_places(min_energy_defect, 6))
                        {
                            lyt_simulation_with_defect.charge_distribution_to_index_general();
                            charge_index_defect_layout = lyt_simulation_with_defect.get_charge_index().first;
                        }
                    }

                    if (charge_index_defect_layout != charge_index_layout)
                    {
                        auto distance = std::numeric_limits<double>::max();
                        layout.foreach_cell(
                            [&layout, &defect, &distance](const auto& cell)
                            {
                                if (sidb_nanometer_distance<Lyt>(layout, cell, defect) < distance)
                                {
                                    distance = sidb_nanometer_distance<Lyt>(layout, cell, defect);
                                }
                            });

                        const std::lock_guard lock{mutex};
                        {
                            if (distance > avoidance_distance)
                            {
                                min_defect_position = defect;
                                avoidance_distance  = distance;
                            }
                        }
                    }
                }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    for (auto f = num_threads * number_per_thread; f < num_threads * number_per_thread + number_last; f++)
    {
        const auto defect = defect_cells[f];

        sidb_defect_layout lyt_defect{};

        if (layout.num_cells() != 0)
        {
            layout.foreach_cell([&layout, &lyt_defect](const auto& cell)
                                { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });
        }

        lyt_defect.assign_sidb_defect(defect, sim_params.defect);

        auto simulation_result_defect = quickexact(lyt_defect, params_defect);

        const auto min_energy_defect          = minimum_energy(simulation_result_defect.charge_distributions);
        uint64_t   charge_index_defect_layout = 0;

        for (const auto& lyt_simulation_with_defect : simulation_result_defect.charge_distributions)
        {
            if (round_to_n_decimal_places(lyt_simulation_with_defect.get_system_energy(), 6) ==
                round_to_n_decimal_places(min_energy_defect, 6))
            {
                lyt_simulation_with_defect.charge_distribution_to_index_general();
                charge_index_defect_layout = lyt_simulation_with_defect.get_charge_index().first;
            }
        }

        if (charge_index_defect_layout != charge_index_layout)
        {
            auto distance = std::numeric_limits<double>::max();
            layout.foreach_cell(
                [&layout, &defect, &distance](const auto& cell)
                {
                    if (sidb_nanometer_distance<Lyt>(layout, cell, defect) < distance)
                    {
                        distance = sidb_nanometer_distance<Lyt>(layout, cell, defect);
                    }
                });

            if (distance > avoidance_distance)
            {
                min_defect_position = defect;
                avoidance_distance  = distance;
            }
        }
    }

    return {avoidance_distance, min_defect_position};
}

}  // namespace fiction

#endif  // FICTION_MAXIMAL_DEFECT_INFLUENCE_DISTANCE_HPP
