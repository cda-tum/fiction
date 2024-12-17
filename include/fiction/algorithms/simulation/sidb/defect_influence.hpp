//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_DEFECT_INFLUENCE_HPP
#define FICTION_DEFECT_INFLUENCE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
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
 * This struct stores the parameters for the `defect_influence` algorithm.
 */
struct defect_influence_params
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
 * Parameters for the `defect_influence_of_sidb_gate` algorithm.
 */
struct defect_influence_of_sidb_gate_params
{
    /**
     * Parameters for the defect influence simulation.
     */
    defect_influence_params defect_influence_params{};
    /**
     * Parameters for the input BDL iterator.
     */
    bdl_input_iterator_params bdl_iterator_params{};
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
    maximum_defect_influence_position_and_distance_impl(const Lyt& lyt, defect_influence_params sim_params,
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

        const quickexact_params<cell<Lyt>> qe_params{
            params.simulation_parameters, quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};

        mockturtle::stopwatch stop{stats.time_total};

        std::mutex mutex{};

        double          avoidance_distance{0};
        coordinate<Lyt> max_defect_position{};

        const auto simulation_results = quickexact(layout, qe_params);

        const auto gs = determine_groundstate_from_simulation_results(simulation_results).front();

        const auto charge_index_of_ground_state = gs.get_charge_index_and_base().first;

        // simulate the impact of the defect at a given position on the ground state of the SiDB layout
        const auto process_defect = [this, &mutex, &avoidance_distance, &qe_params, &charge_index_of_ground_state,
                                     &max_defect_position](const auto& defect_pos) noexcept
        {
            if (layout.get_cell_type(defect_pos) == Lyt::technology::cell_type::EMPTY)
            {
                sidb_defect_surface<Lyt> lyt_defect{};

                layout.foreach_cell([this, &lyt_defect](const auto& cell)
                                    { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });

                // assign defect to layout
                lyt_defect.assign_sidb_defect(defect_pos, params.defect);
                // conduct simulation with defect
                auto simulation_result_defect = quickexact(lyt_defect, qe_params);

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
                        const std::lock_guard<std::mutex> lock{mutex};
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
    const defect_influence_params params;
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
 * This function simulates the influence of defects on an SiDB layout. It computes the
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
defect_influence(const Lyt& lyt, const defect_influence_params& params = {},
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

/**
 * This function calculates the maximum influence position and distance of a defect on the ground state
 * of an SiDB layout. It iterates over all input combinations and finds the defect position at maximum position
 * that affects the gate's ground state.
 *
 * @note The "maximum defect influence distance" describes the maximum distance at which a defect influences the ground
 * state. It does not check if the layout gets non-operational, since a change in the ground state can still
 * lead to an operational gate.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to compute the maximum defect influence position and distance for.
 * @param params Parameters for the defect influence simulation and BDL pair detection.
 * @return A pair containing the maximum influence defect position and its distance from the layout/gate.
 */
template <typename Lyt>
[[nodiscard]] std::pair<typename Lyt::cell, double>
defect_influence_of_sidb_gate(const Lyt& lyt, const defect_influence_of_sidb_gate_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!has_offset_ucoord_v<Lyt>, "Lyt should not be based on offset coordinates");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    bdl_input_iterator<Lyt> bii{lyt, params.bdl_iterator_params};
    double                  maximum_defect_influence_distance = 0.0;
    cell<Lyt>               defect_cell{};

    // number of different input combinations
    for (auto i = 0u; i < std::pow(2, lyt.num_pis()); ++i, ++bii)
    {
        maximum_defect_influence_position_and_distance_stats stats_defect{};
        const auto influence_cell_distance = defect_influence(lyt, params.defect_influence_params, &stats_defect);

        if (influence_cell_distance.second > maximum_defect_influence_distance)
        {
            maximum_defect_influence_distance = influence_cell_distance.second;
            defect_cell                       = influence_cell_distance.first;
        }
    }

    return {defect_cell, maximum_defect_influence_distance};
}

}  // namespace fiction

#endif  // FICTION_DEFECT_INFLUENCE_HPP
