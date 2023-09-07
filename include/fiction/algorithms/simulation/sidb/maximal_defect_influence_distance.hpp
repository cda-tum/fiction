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

#include <algorithm>
#include <cmath>
#include <limits>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

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
     * The pair describes the width and height of the area around the gate, which is
     * also used to place defects (given in SiQAD coordinates).
     */
    std::pair<int32_t, int32_t> additional_scanning_area{50, 6};
    /**
     * Number of threads to spawn. By default the number of threads is set to the number of available hardware threads.
     */
    uint64_t number_threads{std::thread::hardware_concurrency()};
};

namespace detail
{
/**
 * A class for simulating the maximal influence distance of defects within an SiDB layout.
 *
 * This class is responsible for simulating the distance at which defects placed within an SiDB
 * layout still influence the ground state of the layout. It conducts simulations at various defect positions,
 * identifying the position that maximally impacts the layout and calculating the associated influence distance.
 *
 * The class provides a `run` method to initiate the simulation and compute the maximal influence
 * distance and corresponding defect position. It utilizes multithreading for efficient defect
 * position simulations.
 */
template <typename Lyt>
class maximal_defect_influence_distance_impl
{
  public:
    maximal_defect_influence_distance_impl(const Lyt&                                           lyt,
                                           const maximal_defect_influence_distance_params<Lyt>& sim_params) :
            layout{lyt},
            params{sim_params}
    {
        collect_all_defect_cells();
    }

    std::pair<double, typename Lyt::cell> run()
    {
        const quickexact_params<sidb_defect_cell_clk_lyt_siqad> params_defect{params.physical_params,
                                                                              automatic_base_number_detection::OFF};

        double          avoidance_distance{0};
        coordinate<Lyt> min_defect_position{};

        const auto simulation_results =
            quickexact(layout, quickexact_params<Lyt>{params.physical_params, automatic_base_number_detection::OFF});

        const auto min_energy          = minimum_energy(simulation_results.charge_distributions);
        uint64_t   charge_index_layout = 0;

        for (auto& lyt_result : simulation_results.charge_distributions)
        {
            if (std::fabs(round_to_n_decimal_places(lyt_result.get_system_energy(), 6) -
                          round_to_n_decimal_places(min_energy, 6)) < std::numeric_limits<double>::epsilon())
            {
                lyt_result.charge_distribution_to_index_general();
                charge_index_layout = lyt_result.get_charge_index_and_base().first;
            }
        }

        std::thread::hardware_concurrency();

        // Determine the number of threads to use. Use hardware concurrency by default.
        const uint64_t num_threads =
            (params.number_threads == 0) ? std::thread::hardware_concurrency() : params.number_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);
        std::mutex lock_shared_resources{};  // used to control access to shared resources

        const auto number_per_thread = (defect_cells.size() - (defect_cells.size() % num_threads)) / num_threads;
        const auto number_last       = defect_cells.size() % num_threads;

        // all possible defect positions are simulated by using several threads.
        for (uint64_t z = 0u; z < num_threads; ++z)
        {
            threads.emplace_back(
                [&, z]
                {
                    for (auto i = z * number_per_thread; i < (z + 1) * number_per_thread; i++)
                    {
                        const auto defect = defect_cells[i];

                        sidb_defect_cell_clk_lyt_siqad lyt_defect{};

                        layout.foreach_cell([this, &lyt_defect](const auto& cell)
                                            { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });

                        // assign defect to layout
                        lyt_defect.assign_sidb_defect(defect, params.defect);
                        // conduct simulation with defect.
                        auto simulation_result_defect = quickexact(lyt_defect, params_defect);

                        const auto min_energy_defect = minimum_energy(simulation_result_defect.charge_distributions);
                        uint64_t   charge_index_defect_layout = 0;

                        // get the charge index of the ground state
                        for (auto& lyt_simulation_with_defect : simulation_result_defect.charge_distributions)
                        {
                            if (std::fabs(round_to_n_decimal_places(lyt_simulation_with_defect.get_system_energy(), 6) -
                                          round_to_n_decimal_places(min_energy_defect, 6)) <
                                std::numeric_limits<double>::epsilon())
                            {
                                lyt_simulation_with_defect.charge_distribution_to_index_general();
                                charge_index_defect_layout =
                                    lyt_simulation_with_defect.get_charge_index_and_base().first;
                            }
                        }

                        // defect changes the ground state, i.e., the charge index is changed compared to the charge
                        // distribution without placed defect.
                        if (charge_index_defect_layout != charge_index_layout)
                        {
                            // determine minimal distance of the defect to the layout
                            auto distance = std::numeric_limits<double>::infinity();
                            ;
                            layout.foreach_cell(
                                [this, &defect, &distance](const auto& cell)
                                {
                                    if (sidb_nanometer_distance<Lyt>(layout, cell, defect) < distance)
                                    {
                                        distance = sidb_nanometer_distance<Lyt>(layout, cell, defect);
                                    }
                                });

                            {
                                const std::lock_guard lock{lock_shared_resources};
                                // the distance is larger than the current maximal one.
                                if (distance > avoidance_distance)
                                {
                                    min_defect_position =
                                        defect;    // current placed defect that leads to a change of the ground state
                                    avoidance_distance =
                                        distance;  // new avoidance distance given by the current distance
                                }
                            }
                        }
                    }
                });
        }

        // threads are joined
        for (auto& thread : threads)
        {
            thread.join();
        }

        // the remaining defect positions are analyzed. As an example: Suppose we have 33 defect locations
        // and three threads. Each thread considers ten defects. The following code then analyzes the last three
        // defects.
        for (auto f = num_threads * number_per_thread; f < num_threads * number_per_thread + number_last; f++)
        {
            const auto defect = defect_cells[f];

            sidb_defect_cell_clk_lyt_siqad lyt_defect{};

            layout.foreach_cell([this, &lyt_defect](const auto& cell)
                                { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });

            lyt_defect.assign_sidb_defect(defect, params.defect);

            auto simulation_result_defect = quickexact(lyt_defect, params_defect);

            const auto min_energy_defect          = minimum_energy(simulation_result_defect.charge_distributions);
            uint64_t   charge_index_defect_layout = 0;

            // get the charge index of the ground state
            for (const auto& lyt_simulation_with_defect : simulation_result_defect.charge_distributions)
            {
                if (std::fabs(round_to_n_decimal_places(lyt_simulation_with_defect.get_system_energy(), 6) -
                              round_to_n_decimal_places(min_energy_defect, 6)) < std::numeric_limits<double>::epsilon())
                {
                    lyt_simulation_with_defect.charge_distribution_to_index_general();
                    charge_index_defect_layout = lyt_simulation_with_defect.get_charge_index_and_base().first;
                }
            }
            // defect changes the ground state, i.e., the charge index is changed compared to the charge
            // distribution without placed defect.
            if (charge_index_defect_layout != charge_index_layout)
            {
                auto distance = std::numeric_limits<double>::max();
                layout.foreach_cell(
                    [this, &defect, &distance](const auto& cell)
                    {
                        if (sidb_nanometer_distance<Lyt>(layout, cell, defect) < distance)
                        {
                            distance = sidb_nanometer_distance<Lyt>(layout, cell, defect);
                        }
                    });

                // the distance is larger than the current maximal one.
                if (distance > avoidance_distance)
                {
                    min_defect_position = defect;
                    avoidance_distance  = distance;
                }
            }
        }

        return {avoidance_distance, min_defect_position};
    }

  private:
    /**
     * SiDB cell-level layout to simulate.
     */
    Lyt layout;
    /**
     * Parameters used for the simulation.
     */
    maximal_defect_influence_distance_params<Lyt> params{};
    /**
     * All allowed defect positions.
     */
    std::vector<typename Lyt::cell> defect_cells{};
    /**
     * Collects all possible defect cell positions within a given layout while avoiding SiDB cells.
     *
     * This function calculates a bounding box around the provided layout, encompassing the area
     * where defect cells can be placed. It then iterates through this bounding box, scanning from
     * top to bottom and left to right, and identifies all valid positions for defect cells. A defect
     * cell can only be placed in locations where there are no SiDB cells.
     */
    void collect_all_defect_cells()
    {
        // bounding box around the given layout to have north-west and south-east cells.
        bounding_box_2d<Lyt> bb{layout};

        auto nw = bb.get_min();  // north-west cell
        auto se = bb.get_max();  // south-east cell

        // shift nw and se cell by the additional scanning area to cover an area that is larger than the gate area.
        nw.x = nw.x - params.additional_scanning_area.first;
        nw.y = nw.y - params.additional_scanning_area.second;

        se.x = se.x + params.additional_scanning_area.first;
        se.y = se.y + params.additional_scanning_area.second;

        // start to place the defect at the north-west cell
        auto defect_cell = nw;

        // maximal number of placable defects in the given bounding box
        const uint64_t max_defect_positions =
            static_cast<uint64_t>(std::abs(se.x - nw.x) + 1) * static_cast<uint64_t>(std::abs(se.y - nw.y) + 1) * 2;
        defect_cells.reserve(max_defect_positions);

        // collect all cells in the bounding box area (spanned by the nw and se) going from top to down from left to
        // right.
        while (se >= defect_cell)
        {
            // Defect cannot be where a SiDB is placed.
            if (layout.get_cell_type(defect_cell) == sidb_technology::cell_type::EMPTY)
            {
                defect_cells.push_back(defect_cell);
            }
            if (defect_cell.x < se.x)
            {
                defect_cell.x += 1;
            }
            else if ((defect_cell.x == se.x) && defect_cell.z == 0)
            {
                defect_cell.z += 1;
                defect_cell.x = nw.x;
            }
            else if ((defect_cell.x == se.x) && defect_cell.z == 1)
            {
                defect_cell.x = nw.x;
                defect_cell.y += 1;
                defect_cell.z = 0;
            }
        }
    }
};

}  // namespace detail

/**
 * Calculates the maximum distance at which a placed defect can influence the layout's ground state.
 *
 * This function simulates the influence of defects on a SiDB cell-level layout. It computes the
 * maximum influence distance, defined as the minimum distance between any SiDB cell and the defect, at which the
 * defect can still affect the layout's ground state, potentially altering its behavior, such as gate functionality.
 *
 * @tparam Lyt The type representing the SiDB cell-level layout.
 * @param lyt The layout for which the influence distance is being determined.
 * @param sim_params Parameters used to calculate the defect's influence distance.
 * @return A pair consisting of the maximum influence distance (indicating that the defect should be placed farther
 * away to avoid impact) and the position of the defect within the layout, aiding in identifying sensitive regions
 * of the layout.
 */
template <typename Lyt>
std::pair<double, typename Lyt::cell>
maximal_defect_influence_distance(const Lyt& lyt, const maximal_defect_influence_distance_params<Lyt>& sim_params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    detail::maximal_defect_influence_distance_impl<Lyt> p{lyt, sim_params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_MAXIMAL_DEFECT_INFLUENCE_DISTANCE_HPP
