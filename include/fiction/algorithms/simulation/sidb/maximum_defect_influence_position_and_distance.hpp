//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP
#define FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{
/**
 * This struct stores the parameters for the maximum_defect_influence_position_and_distance algorithm.
 */
struct maximum_defect_influence_position_and_distance_params
{
    /**
     * The defect to calculate the maximum defect influence distance for.
     */
    sidb_defect defect{};
    /**
     * Physical simulation parameters.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * The pair describes the width and height of the area around the gate, which is
     * also used to place defects.
     *
     * @note If SiQAD coordinates are used, the second entry describes the number of dimer rows.
     */
    std::pair<int32_t, int32_t> additional_scanning_area{50, 6};
};

/**
 * Statistics for the maximum defect influence simulation.
 */
struct maximum_defect_influence_position_and_distance_stats
{
    /**
     * The total runtime of the maximum defect influence simulation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
};

namespace detail
{
/**
 * A class for simulating the maximum influence distance of defects within an SiDB layout.
 *
 * This class is responsible for simulating the distance at which defects placed within an SiDB
 * layout still influence the ground state of the layout. It conducts simulations at various defect positions,
 * identifying the position that maximally impacts the layout and calculating the associated influence distance.
 *
 * The class provides a `run` method to initiate the simulation and compute the maximum influence
 * distance and corresponding defect position. It utilizes multithreading for efficient defect
 * position simulations.
 */
template <typename Lyt>
class maximum_defect_influence_position_and_distance_impl
{
  public:
    maximum_defect_influence_position_and_distance_impl(
        const Lyt& lyt, maximum_defect_influence_position_and_distance_params sim_params,
        maximum_defect_influence_position_and_distance_stats& st) :
            layout{lyt},
            params{sim_params},
            stats{st}
    {
        collect_all_defect_cells();
    }
    /**
     * Determines the maximum defect influence position and distance for the given layout.
     *
     * @return A pair containing the maximum defect influence position and the corresponding distance.
     */
    std::pair<typename Lyt::cell, double> run() noexcept
    {
        if (layout.is_empty())
        {
            return {{}, 0.0};
        }

        const quickexact_params<cell<Lyt>> params_defect{
            params.simulation_parameters, quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};
        mockturtle::stopwatch stop{stats.time_total};

        std::mutex mutex;

        double          avoidance_distance{0};
        coordinate<Lyt> max_defect_position{};

        const auto simulation_results = quickexact(layout, params_defect);

        const auto gs = determine_groundstate_from_simulation_results(simulation_results).front();

        const auto charge_index_of_ground_state = gs.get_charge_index_and_base().first;

        // simulate the impact of the defect at a given position on the ground state of the SiDB layout
        const auto process_defect = [&](const cell<Lyt>& defect_pos) noexcept
        {
            if (layout.get_cell_type(defect_pos) == Lyt::technology::cell_type::EMPTY)
            {
                sidb_defect_surface<Lyt> lyt_defect{};

                layout.foreach_cell([this, &lyt_defect](const auto& cell)
                                    { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });

                // assign defect to layout
                lyt_defect.assign_sidb_defect(defect_pos, params.defect);
                // conduct simulation with defect
                auto simulation_result_defect = quickexact(lyt_defect, params_defect);

                const auto min_energy_defect = minimum_energy(simulation_result_defect.charge_distributions.cbegin(),
                                                              simulation_result_defect.charge_distributions.cend());

                uint64_t charge_index_defect_layout = 0;

                // get the charge index of the ground state
                for (const auto& lyt_simulation_with_defect : simulation_result_defect.charge_distributions)
                {
                    if (std::fabs(round_to_n_decimal_places(lyt_simulation_with_defect.get_system_energy(), 6) -
                                  round_to_n_decimal_places(min_energy_defect, 6)) <
                        std::numeric_limits<double>::epsilon())
                    {
                        lyt_simulation_with_defect.charge_distribution_to_index_general();
                        charge_index_defect_layout = lyt_simulation_with_defect.get_charge_index_and_base().first;
                    }
                }

                // defect changes the ground state, i.e., the charge index is changed compared to the charge
                // distribution without a placed defect.
                if (charge_index_defect_layout != charge_index_of_ground_state)
                {
                    auto distance = std::numeric_limits<double>::infinity();
                    layout.foreach_cell(
                        [this, &defect_pos, &distance](const auto& cell)
                        {
                            if (sidb_nm_distance<Lyt>(layout, cell, defect_pos) < distance)
                            {
                                distance = sidb_nm_distance<Lyt>(layout, cell, defect_pos);
                            }
                        });

                    {
                        const std::lock_guard<std::mutex> lock(mutex);
                        // the distance is larger than the current maximum one.
                        if (distance > avoidance_distance)
                        {
                            max_defect_position = defect_pos;
                            avoidance_distance  = distance;
                        }
                    }
                }
            }
        };

        static const std::size_t num_threads = std::thread::hardware_concurrency();

        // calculate the size of each slice
        const auto slice_size = (defect_cells.size() - 1) / num_threads;

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);

        // launch threads, each with its own slice of random step points
        for (auto i = 0ul; i < num_threads; ++i)
        {
            const auto start = i * slice_size;
            const auto end   = std::min(start + slice_size, defect_cells.size());

            if (start >= end)
            {
                break;  // no more work to distribute
            }

            threads.emplace_back(
                [this, start, end, &process_defect]
                {
                    for (auto it = defect_cells.cbegin() + static_cast<int64_t>(start);
                         it != defect_cells.cbegin() + static_cast<int64_t>(end); ++it)
                    {
                        process_defect(*it);
                    }
                });
        }

        // wait for all threads to complete
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        return {max_defect_position, avoidance_distance};
    }

  private:
    /**
     * SiDB cell-level layout to simulate.
     */
    Lyt layout;
    /**
     * Parameters used for the simulation.
     */
    const maximum_defect_influence_position_and_distance_params params;
    /**
     * The statistics of the maximum defect influence position.
     */
    maximum_defect_influence_position_and_distance_stats& stats;
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
    void collect_all_defect_cells() noexcept
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

        defect_cells = all_coordinates_in_spanned_area(nw, se);
    }
};

}  // namespace detail

/**
 * This function simulates the influence of defects on a SiDB cell-level layout. It computes the
 * maximum influence distance, defined as the minimum distance between any SiDB cell and the given defect, at which the
 * defect can still affect the layout's ground state, potentially altering its behavior, such as gate functionality.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The SiDB cell-level layout for which the influence position and distance is being determined.
 * @param params Parameters used to calculate the defect's maximum influence distance.
 * @param pst Statistics of the maximum defect influence distance.
 * @return Pair with the first element describing the position with maximum distance to the layout where a placed defect
 * can still affect the ground state of the layout. The second entry describes the distance of the defect from the
 * layout.
 */
template <typename Lyt>
std::pair<typename Lyt::cell, double>
maximum_defect_influence_position_and_distance(const Lyt&                                                   lyt,
                                               const maximum_defect_influence_position_and_distance_params& params = {},
                                               maximum_defect_influence_position_and_distance_stats* pst = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!has_offset_ucoord_v<Lyt>, "Lyt cannot be based on offset coordinates");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    maximum_defect_influence_position_and_distance_stats st{};

    detail::maximum_defect_influence_position_and_distance_impl<Lyt> p{lyt, params, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP
