//
// Created by Jan Drewniok on 18.12.22.
//

#ifndef FICTION_QUICKEXACT_HPP
#define FICTION_QUICKEXACT_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/enum_class_exhaustive_algorithm.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * An enumeration of modes to use for the QuickExact algorithm.
 */
enum class automatic_base_number_detection
{
    /**
     * It automatically detects if a 2-state simulation is sufficient or if a 3-state simulation is required. It checks
     * if a positive charge could occur due to maximum band bending. If this mode is active, 3-state simulation could be
     * run even if base_num = 2 was set in the physical parameters.
     */
    ON,
    /**
     * The base number from the physical parameter is used for the simulation.
     */
    OFF
};

/**
 * This struct stores the parameters for the `quickexact` algorithm.
 */
template <typename Lyt>
struct quickexact_params
{
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters physical_parameters{};
    /**
     * If ON, quickexact checks before which base number is required for the simulation (i.e. 3-state or 2-state
     * simulation).
     */
    automatic_base_number_detection base_number_detection = automatic_base_number_detection::ON;
    /**
     * Local external electrostatic potentials (e.g locally applied electrodes).
     */
    std::unordered_map<typename Lyt::cell, double> local_external_potential = {};
    /**
     * Global external electrostatic potential. Value is applied on each cell in the layout.
     */
    double global_potential = 0;
};

namespace detail
{

template <typename Lyt>
class quickexact_impl
{
  public:
    quickexact_impl(Lyt& lyt, const quickexact_params<Lyt>& parameter) :
            layout{lyt},
            charge_lyt{lyt, parameter.physical_parameters, sidb_charge_state::NEGATIVE},
            params{parameter}
    {}

    sidb_simulation_result<Lyt> run()
    {
        result.algorithm_name      = "quickexact";
        result.physical_parameters = params.physical_parameters;
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};
            initialize_charge_layout();

            //  Determine all SiDBs that have to be negatively charged to fulfill the population stability. This is an
            //  efficient way to prune the search space by 2^k with k being the number of detected negatively charged
            //  SiDBs.
            //  Determine if three state simulation (i.e. positively charged SiDBs can occur) is required.
            const bool three_state_simulation_required =
                params.base_number_detection == automatic_base_number_detection::ON &&
                charge_lyt.three_state_simulation_required();

            // If layout has at least two SiDBs, the code inside this if-statement is executed.
            if (number_of_sidbs > 1)
            {
                generate_layout_without_negative_sidbs();
            }
            if (!all_sidbs_in_lyt_without_negative_detected_ones.empty() && number_of_sidbs > 1)
            {
                // The first cell from all_sidbs_in_lyt_without_negative_detected_ones is chosen as the dependent cell
                // to initialize the layout (detected negatively charged SiDBs were erased in the step before).
                charge_distribution_surface charge_lyt_with_assigned_dependent_cell{
                    layout, params.physical_parameters, sidb_charge_state::NEUTRAL,
                    all_sidbs_in_lyt_without_negative_detected_ones[0]};
                charge_lyt_with_assigned_dependent_cell.assign_local_external_potential(
                    params.local_external_potential);
                charge_lyt_with_assigned_dependent_cell.assign_global_external_potential(params.global_potential);

                if constexpr (has_get_sidb_defect_v<Lyt>)
                {
                    for (const auto& [cell, defect] : real_placed_defects)
                    {
                        charge_lyt_with_assigned_dependent_cell.assign_defect_to_charge_distribution_surface(cell,
                                                                                                             defect);
                    }
                }
                // IMPORTANT: The detected negatively charged SiDBs (they have to be negatively charged to fulfill the
                // population stability) are considered as negatively charged defects in the layout. Hence, there are no
                // "real" defects assigned but in order to set some SiDBs with a fixed negative charge, this way of
                // implementation is chosen.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_with_assigned_dependent_cell.assign_defect_to_charge_distribution_surface(
                        cell, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                          charge_lyt_with_assigned_dependent_cell.get_phys_params().epsilon_r,
                                          charge_lyt_with_assigned_dependent_cell.get_phys_params().lambda_tf});
                }
                // Update all local potentials, system energy and physically validity. Flag is set to "false" to allow
                // dependent cell to change its charge state based on the N-1 SiDBs to fulfill the local population
                // stability at its position.
                charge_lyt_with_assigned_dependent_cell.update_after_charge_change(dependent_cell_mode::VARIABLE);

                // If no positively charged SiDB can occur in the layout.
                if (!three_state_simulation_required)
                {
                    result.additional_simulation_parameters.emplace_back("base_number", uint64_t{2});
                    two_state_simulation(charge_lyt_with_assigned_dependent_cell);
                }

                // If positively charged SiDBs can occur in the layout, 3-state simulation is conducted.
                else
                {
                    result.additional_simulation_parameters.emplace_back("base_number", uint64_t{3});
                    three_state_simulation(charge_lyt_with_assigned_dependent_cell);
                }
            }

            // In the case with only one SiDB in the layout (due to external potentials or defects, this single SiDB can
            // be neutrally or even positively charged.)
            else if (number_of_sidbs == 1)
            {
                if (three_state_simulation_required)
                {
                    charge_lyt.assign_base_number(3);
                }
                else
                {
                    charge_lyt.assign_base_number(2);
                }

                // A check is made to see if the charge index is still below the maximum charge index. If not, the
                // charge index is increased and the corresponding charge distribution is checked for physical validity.
                while (charge_lyt.get_charge_index().first < charge_lyt.get_max_charge_index())
                {

                    if (charge_lyt.is_physically_valid())
                    {
                        charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                        result.charge_distributions.push_back(charge_lyt_copy);
                    }
                    charge_lyt.increase_charge_index_by_one(
                        dependent_cell_mode::VARIABLE);  // "false" allows that the charge state of the dependent cell
                                                         // is automatically changed based on the new charge
                                                         // distribution.
                }
                if (charge_lyt.is_physically_valid())
                {
                    charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                    result.charge_distributions.push_back(charge_lyt_copy);
                }
            }
            // If the layout consists of only detected negatively charged SiDBs.
            else if (all_sidbs_in_lyt_without_negative_detected_ones.empty() && number_of_sidbs > 1)
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt.adding_sidb_to_layout(cell, -1);
                }
                result.charge_distributions.push_back(charge_lyt_copy);
            }
        }
        result.simulation_runtime = time_counter;
        return result;
    }

  private:
    /**
     * This function conducts 2-state physical simulation (negative, neutral).
     *
     * @param charge_layout Initialized charge layout.
     * @param sim_result sidb_simulation_result to collect gained results.
     */
    void two_state_simulation(charge_distribution_surface<Lyt>& charge_layout)
    {
        charge_layout.assign_base_number(2);
        uint64_t current_charge_index  = 0;
        uint64_t previous_charge_index = 0;
        for (uint64_t i = 0; i <= charge_layout.get_max_charge_index(); i++)
        {
            current_charge_index = (i ^ (i >> 1));  // gray code is used for the charge index.
            charge_layout.assign_charge_index_by_gray_code(
                current_charge_index, previous_charge_index, dependent_cell_mode::VARIABLE,
                energy_calculation::KEEP_OLD_ENERGY_VALUE, charge_distribution_history::CONSIDER);
            previous_charge_index = current_charge_index;

            if (charge_layout.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                result.charge_distributions.push_back(charge_lyt_copy);
            }
        }

        // The cells of the previously detected negatively charged SiDBs are added to the cell level layout.
        for (const auto& cell : detected_negative_sidbs)
        {
            layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }
    /**
     * This function conducts 3-state physical simulation (negative, neutral, positive).
     *
     * @param charge_layout Initialized charge layout.
     * @param sim_result sidb_simulation_result to collect gained results.
     */
    void three_state_simulation(charge_distribution_surface<Lyt>& charge_layout)
    {
        charge_layout.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_layout.update_after_charge_change();
        // Not executed to detect if 3-state simulation is required, but to detect the SiDBs that could be positively
        // charged (important to speed up the simulation).
        charge_layout.three_state_simulation_required();
        charge_layout.update_after_charge_change(dependent_cell_mode::VARIABLE);
        while (charge_layout.get_charge_index().first < charge_layout.get_max_charge_index())
        {
            if (charge_layout.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                result.charge_distributions.push_back(charge_lyt_copy);
            }

            while (charge_layout.get_charge_index_sub_layout().first < charge_layout.get_max_charge_index_sub_layout())
            {
                if (charge_layout.is_physically_valid())
                {
                    charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
                    charge_lyt_copy.recompute_system_energy();
                    // The previously detected negatively charged SiDBs are added to the final layout.
                    for (const auto& cell : detected_negative_sidbs)
                    {
                        charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                    }
                    result.charge_distributions.push_back(charge_lyt_copy);
                }
                charge_layout.increase_charge_index_of_sub_layout_by_one(
                    dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                    charge_distribution_history::CONSIDER,
                    exhaustive_algorithm::QUICKEXACT);  // "false" allows that the charge state of the dependent cell is
                                                        // automatically changed based on the new charge distribution.
            }

            if (charge_layout.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
                charge_lyt_copy.recompute_system_energy();
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                result.charge_distributions.push_back(charge_lyt_copy);
            }

            charge_layout.reset_charge_index_sub_layout();

            charge_layout.increase_charge_index_by_one(
                dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                charge_distribution_history::CONSIDER,
                exhaustive_algorithm::QUICKEXACT);  // "false" allows that the charge state of the dependent cell is
                                                    // automatically changed based on the new charge distribution.
        }
        // Charge configurations of the sub layout are looped.
        while (charge_layout.get_charge_index_sub_layout().first < charge_layout.get_max_charge_index_sub_layout())
        {
            if (charge_layout.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                result.charge_distributions.push_back(charge_lyt_copy);
            }
            charge_layout.increase_charge_index_of_sub_layout_by_one(
                dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                charge_distribution_history::CONSIDER, exhaustive_algorithm::QUICKEXACT);
        }

        if (charge_layout.is_physically_valid())
        {
            charge_distribution_surface<Lyt> charge_lyt_copy{charge_layout};
            for (const auto& cell : detected_negative_sidbs)
            {
                charge_lyt_copy.adding_sidb_to_layout(cell, -1);
            }
            result.charge_distributions.push_back(charge_lyt_copy);
        }

        for (const auto& cell : detected_negative_sidbs)
        {
            layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }
    /**
     * This function is required for the initialization.
     */
    void initialize_charge_layout()
    {
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            layout.foreach_sidb_defect(
                [this](const auto& defect)
                {
                    if (layout.get_sidb_defect(defect.first) != sidb_defect{sidb_defect_type::NONE})
                    {
                        charge_lyt.assign_defect_to_charge_distribution_surface(defect.first,
                                                                                layout.get_sidb_defect(defect.first));
                    }
                });
        }

        charge_lyt.assign_local_external_potential(params.local_external_potential);
        charge_lyt.assign_global_external_potential(params.global_potential, dependent_cell_mode::VARIABLE);
        detected_negative_sidb_indices = charge_lyt.negative_sidb_detection();
        detected_negative_sidbs.reserve(detected_negative_sidb_indices.size());
        all_sidbs_in_lyt_without_negative_detected_ones = charge_lyt.get_sidb_order();
        real_placed_defects                             = charge_lyt.get_defects();
        number_of_sidbs                                 = charge_lyt.get_sidb_order().size();
    }
    /**
     * This function is used to generate a layout without the SiDBs that are detected to be negatively charged in a
     * physically-valid layout.
     */
    void generate_layout_without_negative_sidbs()
    {
        for (const auto& index : detected_negative_sidb_indices)
        {
            const auto cell = charge_lyt.index_to_cell(static_cast<uint64_t>(index));
            detected_negative_sidbs.push_back(cell);
            layout.assign_cell_type(cell, Lyt::cell_type::EMPTY);
        }

        // All detected negatively charged SiDBs are erased from the all_sidbs_in_lyt_without_negative_detected_ones
        // vector.
        all_sidbs_in_lyt_without_negative_detected_ones.erase(
            std::remove_if(all_sidbs_in_lyt_without_negative_detected_ones.begin(),
                           all_sidbs_in_lyt_without_negative_detected_ones.end(),
                           [this](const typename Lyt::cell& n)
                           {
                               return std::find(detected_negative_sidbs.cbegin(), detected_negative_sidbs.cend(), n) !=
                                      detected_negative_sidbs.cend();
                           }),
            all_sidbs_in_lyt_without_negative_detected_ones.cend());
    }
    /**
     * Cell-level layout
     */
    Lyt layout;
    /**
     * Charge distribution surface
     */
    charge_distribution_surface<Lyt> charge_lyt{};
    /**
     * Parameters used for the simulation.
     */
    quickexact_params<Lyt> params{};
    /**
     * Indices of all SiDBs that are detected to be negatively charged in a physically valid layout.
     */
    std::vector<int64_t> detected_negative_sidb_indices{};
    /**
     * All SiDBs that are detected to be negatively charged in a physically valid layout.
     */
    std::vector<typename Lyt::cell> detected_negative_sidbs{};
    /**
     * All SiDBs of the layout but without the negatively charged SiDBs.
     */
    std::vector<typename Lyt::cell> all_sidbs_in_lyt_without_negative_detected_ones{};
    /**
     * Collection of defects that are placed in addition to the SiDBs.
     */
    std::unordered_map<typename Lyt::cell, const sidb_defect> real_placed_defects{};
    /**
     * Number of SiDBs of the input layout.
     */
    uint64_t number_of_sidbs{};
    /**
     * Simulation results.
     */
    sidb_simulation_result<Lyt> result{};
};

}  // namespace detail

/**
 * *QuickExact* is a quick and exact physical simulation algorithm designed specifically for SiDB layouts.
 * It determines all physically valid charge configurations of a given layout, providing a significant
 * performance advantage of more than three orders of magnitude over the state of the art algorithm *ExGs*
 * (exhaustive_ground_state_simulation.hpp).
 *
 * The performance improvement in QuickExact can be attributed to the incorporation of three key ideas:
 *
 * 1. Advanced Negative SiDB Detection: QuickExact efficiently identifies SiDBs that require negative charges
 *    in a physically valid charge distribution. By detecting them in advance, the search space is pruned
 *    by a factor of 2^k, where k is the number of found SiDBs.
 *
 * 2. Dependent SiDB Selection: The algorithm selects a dependent SiDB, whose charge state is always derived
 *    from its n-1 neighbors. This dependency simplifies the computation process and contributes to the overall
 *    efficiency of QuickExact.
 *
 * 3. Gray Code Representation: QuickExact employs gray code to represent and traverse through all charge
 *    configurations. By using gray code, only one charge state changes at a time, making the computation
 *    of the local electrostatic potential easier.
 *
 * Additionally, Quickexact allows for the simulation of SiDB layouts while considering global and local
 * electrostatic potentials, as well as existing defects. This comprehensive approach ensures an accurate
 * representation of the physical behavior of the SiDB layout.
 *
 * In summary, Quickexact combines advanced SiDB detection, dependent SiDB selection, and the use of gray code
 * to achieve outstanding performance and enable efficient simulations of SiDB layouts, even in scenarios where
 * positively charged SiDBs occur due to small spacing.
 */
template <typename Lyt>
sidb_simulation_result<Lyt> quickexact(Lyt& lyt, const quickexact_params<Lyt>& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    detail::quickexact_impl<Lyt> p{lyt, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_QUICKEXACT_HPP