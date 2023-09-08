//
// Created by Jan Drewniok on 21.06.23.
//

#ifndef FICTION_MAXIMUM_DEFECT_INFLUENCE_DISTANCE_HPP
#define FICTION_MAXIMUM_DEFECT_INFLUENCE_DISTANCE_HPP

#include "fiction/algorithms/simulation/sidb/critical_temperature.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/layouts/bounding_box.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_surface.hpp"
#include "fiction/utils/execution_utils.hpp"
#include "fiction/utils/layout_utils.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace fiction
{
/**
 * This struct stores the parameters for the `maximum_defect_influence_distance` algorithm.
 */
template <typename Lyt>
struct maximum_defect_influence_distance_params
{
    /**
     * The defect used to calculate the maximum defect influence distance.
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
};

/**
 * This struct stores the `Maximum defect influence distance` and the corresponding `Maximum defect position` found by
 * the algorithm.
 */
template <typename Lyt>
struct maximum_defect_influence_distance_stats
{
    /**
     * Maximum defect position at which the
     * defect can still affect the layout's ground state.
     */
    typename Lyt::cell maximum_influence_defect_position{};
    /**
     * Maximum defect influence distance, defined as the minimum distance between any SiDB cell and the defect, at which
     * the defect can still affect the layout's ground state.
     */
    double maximum_defect_influence_distance{};
    /**
     * Print the results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout)
    {
        out << fmt::format("Maximum influence distance: {} \n Defect position: {}, {}, {} \n",
                           maximum_defect_influence_distance, maximum_influence_defect_position.x,
                           maximum_influence_defect_position.y, std::to_string(maximum_influence_defect_position.z));
    }
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
class maximum_defect_influence_distance_impl
{
  public:
    maximum_defect_influence_distance_impl(const Lyt&                                           lyt,
                                           const maximum_defect_influence_distance_params<Lyt>& sim_params,
                                           maximum_defect_influence_distance_stats<Lyt>&        st) :
            layout{lyt},
            params{sim_params},
            defect_distance_stats{st}
    {
        collect_all_defect_cells();
    }

    bool run()
    {
        const quickexact_params<sidb_defect_cell_clk_lyt_siqad> params_defect{params.physical_params,
                                                                              automatic_base_number_detection::OFF};

        double          avoidance_distance{0};
        coordinate<Lyt> max_defect_position{};

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

        // simulate the impact of the defect at a given position on the ground state of the SiDB layout
        auto process_defect = [&](const auto& defect)
        {
            sidb_defect_cell_clk_lyt_siqad lyt_defect{};

            layout.foreach_cell([this, &lyt_defect](const auto& cell)
                                { lyt_defect.assign_cell_type(cell, layout.get_cell_type(cell)); });

            // assign defect to layout
            lyt_defect.assign_sidb_defect(defect, params.defect);
            // conduct simulation with defect
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

                // the distance is larger than the current maximum one.
                if (distance > avoidance_distance)
                {
                    max_defect_position = defect;
                    avoidance_distance  = distance;
                }
            }
        };

        // Apply the process_defect function to each defect using std::for_each
        std::for_each(FICTION_EXECUTION_POLICY_PAR defect_cells.begin(), defect_cells.end(), process_defect);

        defect_distance_stats.maximum_influence_defect_position = max_defect_position;
        defect_distance_stats.maximum_defect_influence_distance = avoidance_distance;

        return EXIT_SUCCESS;
    }

  private:
    /**
     * SiDB cell-level layout to simulate.
     */
    Lyt layout;
    /**
     * Parameters used for the simulation.
     */
    maximum_defect_influence_distance_params<Lyt> params{};
    /**
     * All allowed defect positions.
     */
    std::vector<typename Lyt::cell> defect_cells{};
    /**
     * Statistics.
     */
    maximum_defect_influence_distance_stats<Lyt>& defect_distance_stats;
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

        // maximum number of placable defects in the given bounding box
        const uint64_t max_defect_positions =
            static_cast<uint64_t>(std::abs(se.x - nw.x) + 1) * static_cast<uint64_t>(std::abs(se.y - nw.y) + 1) * 2;
        defect_cells.reserve(max_defect_positions);

        // collect all cells in the bounding box area (spanned by the nw and se) going from top to down from left to
        // right.
        while (defect_cell <= se)
        {
            // Defect can only be placed at free locations.
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
            else
            {
                break;
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
 * @param sim_params Parameters used to calculate the defect's maximum influence distance.
 * @return pst Statistics.
 */
template <typename Lyt>
bool maximum_defect_influence_distance(const Lyt&                                           lyt,
                                       const maximum_defect_influence_distance_params<Lyt>& sim_params = {},
                                       maximum_defect_influence_distance_stats<Lyt>*        pst        = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    maximum_defect_influence_distance_stats<Lyt> st{};

    detail::maximum_defect_influence_distance_impl<Lyt> p{lyt, sim_params, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_MAXIMUM_DEFECT_INFLUENCE_DISTANCE_HPP
