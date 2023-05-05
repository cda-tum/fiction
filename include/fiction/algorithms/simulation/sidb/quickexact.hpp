//
// Created by Jan Drewniok on 18.12.22.
//

#ifndef FICTION_QUICKEXACT_HPP
#define FICTION_QUICKEXACT_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

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
     * It automatically detects if 3-state simulation is required. It checks if a positive charge could occur due to
     * maximum band bending. If this mode is active, 3-state simulation could be run even if base_num = 2 was set in the
     * physical parameters.
     */
    ON,
    /**
     * The base number from the physical parameter are used for the simulation.
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
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters physical_parameters{};
    /**
     * If ON, quickexact checks before the simulation starts if 3-state simulation is required or not.
     */
    automatic_base_number_detection base_number_detection = automatic_base_number_detection::ON;
    /**
     * All placed defects (cell + defect).
     */
    std::unordered_map<typename Lyt::cell, const sidb_defect> defects{};
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
    quickexact_impl(Lyt& lyt, const quickexact_params<Lyt>& params) :
            layout{lyt},
            parameter{params},
            charge_lyt{lyt, params.physical_parameters, sidb_charge_state::NEGATIVE}
    {}

    void run()
    {
        sidb_simulation_result<Lyt> simulation_result{};
        simulation_result.algorithm_name      = "quickexact";
        simulation_result.physical_parameters = parameter.physical_parameters;
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};
            this->initialize_charge_layout();

            //  Determine all SiDBs that have to be negatively charged to fulfill the population stability. This is an
            //  efficient way to prune the search space by 2^k with k being the number of detected negatively charged
            //  SiDBs.
            //  Determine if three state simulation (i.e. positively charged SiDBs can occur) is required.
            bool three_state_simulation_required = false;
            if (parameter.base_number_detection == automatic_base_number_detection::ON)
            {
                three_state_simulation_required = charge_lyt.three_state_sim_required();
            }

            // If layout has at least two SiDBs, the code inside this if-statement is executed.
            if (number_of_SiDBs > 1)
            {
                this->generate_layout_without_negative_sidbs();
            }
            if (!all_sidbs_in_lyt_without_negative_detected_ones.empty() && number_of_SiDBs > 1)
            {
                // The first cell from all_sidbs_in_lyt_without_negative_detected_ones is chosen as the dependent cell
                // to initialize the layout (detected negatively charged SiDBs were erased in the step before).
                charge_distribution_surface charge_lyt_new{layout, parameter.physical_parameters,
                                                           sidb_charge_state::NEUTRAL,
                                                           all_sidbs_in_lyt_without_negative_detected_ones[0]};
                charge_lyt_new.set_local_external_potential(parameter.local_external_potential);
                charge_lyt_new.set_global_external_potential(parameter.global_potential);
                // IMPORTANT: The detected negatively charged SiDBs (they have to be negatively charged to fulfill the
                // population stability) are considered as negatively charged defects in the layout. Hence, there are no
                // "real" defects assigned but in order to set some SiDBs with a fixed negative charge, this way of
                // implementation is chosen.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_new.assign_defect(cell, sidb_defect{sidb_defect_type::UNKNOWN, -1,
                                                                   charge_lyt_new.get_phys_params().epsilon_r,
                                                                   charge_lyt_new.get_phys_params().lambda_tf});
                }
                for (const auto& [cell, defect] : real_placed_defects)
                {
                    charge_lyt_new.assign_defect(cell, defect);
                }
                // Update all local potentials, system energy and physically validity. Flag is set to "false" to allow
                // dependent cell to change its charge state based on the N-1 SiDBs to fulfill the local population
                // stability at its position.

                // False declares that the dependent cell is updated based on the local potential at the position.
                charge_lyt_new.update_after_charge_change(false);

                // If no positively charged DB can occur in the layout, this scope is executed.
                if (!three_state_simulation_required)
                {
                    simulation_result.additional_simulation_parameters.emplace_back("base_number",
                                                                                    static_cast<uint64_t>(2));
                    this->two_state_simulation(charge_lyt_new, simulation_result);
                }

                // If positively charged DBs can occur in the layout, 3-state simulation is conducted.
                else
                {
                    simulation_result.additional_simulation_parameters.emplace_back("base_number",
                                                                                    static_cast<uint64_t>(3));
                    this->three_state_simulation(charge_lyt_new, simulation_result);
                }
            }

            // In the case with only one SiDB in the layout (due to external potentials or defects, this single SiDB can
            // be neutrally or even positively charged.)
            else if (number_of_SiDBs == 1)
            {
                if (three_state_simulation_required)
                {
                    charge_lyt.set_base_number(3);
                }
                else
                {
                    charge_lyt.set_base_number(2);
                }

                while (charge_lyt.get_charge_index().first < charge_lyt.get_max_charge_index())
                {

                    if (charge_lyt.is_physically_valid())
                    {
                        charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                        simulation_result.charge_distributions.push_back(charge_lyt_copy);
                    }
                    charge_lyt.increase_charge_index_by_one(
                        false);  // "false" allows that the charge state of the dependent cell is automatically changed
                                 // based on the new charge distribution.
                }
                if (charge_lyt.is_physically_valid())
                {
                    charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                    simulation_result.charge_distributions.push_back(charge_lyt_copy);
                }
            }
            // If the layout consists of only detected negatively charged SiDBs, this scope is
            // executed.
            else if (all_sidbs_in_lyt_without_negative_detected_ones.empty() && number_of_SiDBs > 1)
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt.adding_sidb_to_layout(cell, -1);
                }
                simulation_result.charge_distributions.push_back(charge_lyt_copy);
            }
        }
        simulation_result.simulation_runtime = time_counter;
        result                               = simulation_result;
    }

    sidb_simulation_result<Lyt> get_simulation_results() const
    {
        return result;
    }

  private:
    /**
     * This function conducts 2-state physical simulation (negative, neutral).
     *
     * @param charge_lyt_new Initialized charge layout.
     * @param sim_result sidb_simulation_result to collect gained results.
     */
    void two_state_simulation(charge_distribution_surface<Lyt>& charge_lyt_new, sidb_simulation_result<Lyt>& sim_result)
    {
        charge_lyt_new.set_base_number(2);
        uint64_t current_charge_index  = 0;
        uint64_t previous_charge_index = 0;
        for (uint64_t i = 0; i <= charge_lyt_new.get_max_charge_index(); i++)
        {
            current_charge_index = (i ^ (i >> 1));  // gray code is used for the charge index.
            charge_lyt_new.set_charge_index_by_gray_code(
                current_charge_index, previous_charge_index, false, false,
                true);  // "false" allows that the charge state of the dependent cell is
                        // automatically changed based on the new charge distribution.
            previous_charge_index = current_charge_index;

            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
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
     * @param charge_lyt_new Initialized charge layout.
     * @param sim_result sidb_simulation_result to collect gained results.
     */
    void three_state_simulation(charge_distribution_surface<Lyt>& charge_lyt_new,
                                sidb_simulation_result<Lyt>&      sim_result)
    {
        charge_lyt_new.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt_new.update_after_charge_change();
        // Not executed to detect if 3-state simulation is required, but to detect the SiDBs that could be positively
        // charged (important to speed up the simulation).
        const auto three_state_simulation_required = charge_lyt_new.three_state_sim_required();
        charge_lyt_new.update_after_charge_change(false);
        while (charge_lyt_new.get_charge_index().first < charge_lyt_new.get_max_charge_index())
        {
            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
            }

            while (charge_lyt_new.get_charge_index_sub_layout().first <
                   charge_lyt_new.get_max_charge_index_sub_layout())
            {
                if (charge_lyt_new.is_physically_valid())
                {
                    charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                    charge_lyt_copy.recompute_system_energy();
                    // The previously detected negatively charged SiDBs are added to the final layout.
                    for (const auto& cell : detected_negative_sidbs)
                    {
                        charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                    }
                    sim_result.charge_distributions.push_back(charge_lyt_copy);
                }
                charge_lyt_new.increase_charge_index_of_sub_layout_by_one(
                    false, false, true,
                    true);  // "false" allows that the charge state of the dependent cell is
                            // automatically changed based on the new charge distribution.
            }

            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
            }

            charge_lyt_new.reset_charge_index_sub_layout();

            charge_lyt_new.increase_charge_index_by_one(
                false, false, true, true);  // "false" allows that the charge state of the dependent cell is
                                            // automatically changed based on the new charge distribution.
        }
        // Charge configurations of the sub layout are looped.
        while (charge_lyt_new.get_charge_index_sub_layout().first < charge_lyt_new.get_max_charge_index_sub_layout())
        {
            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // The previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
            }
            charge_lyt_new.increase_charge_index_of_sub_layout_by_one(
                false, false, true,
                true);  // "false" allows that the charge state of the dependent cell is
                        // automatically changed based on the new charge distribution.
        }

        if (charge_lyt_new.is_physically_valid())
        {
            charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
            for (const auto& cell : detected_negative_sidbs)
            {
                charge_lyt_copy.adding_sidb_to_layout(cell, -1);
            }
            sim_result.charge_distributions.push_back(charge_lyt_copy);
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
        // defects are initialized.
        for (const auto& [cell, defect] : parameter.defects)
        {
            if (defect.epsilon_r == 0 && defect.lambda_tf == 0)
            {
                charge_lyt.assign_defect(cell,
                                         sidb_defect{defect.type, defect.charge, charge_lyt.get_phys_params().epsilon_r,
                                                     charge_lyt.get_phys_params().lambda_tf});
            }
            else if (defect.epsilon_r == 0 && defect.lambda_tf != 0)
            {
                charge_lyt.assign_defect(
                    cell, sidb_defect{defect.type, defect.charge, charge_lyt.get_phys_params().epsilon_r});
            }
            else if (defect.epsilon_r != 0 && defect.lambda_tf == 0)
            {
                charge_lyt.assign_defect(cell, sidb_defect{defect.type, defect.charge, defect.epsilon_r,
                                                           charge_lyt.get_phys_params().lambda_tf});
            }
            else
            {
                charge_lyt.assign_defect(cell, defect);
            }
        }

        charge_lyt.set_local_external_potential(parameter.local_external_potential);
        charge_lyt.set_global_external_potential(parameter.global_potential, false);
        detected_negative_sidb_indices = charge_lyt.negative_sidb_detection();
        detected_negative_sidbs.reserve(detected_negative_sidb_indices.size());
        all_sidbs_in_lyt_without_negative_detected_ones = charge_lyt.get_sidb_order();
        real_placed_defects                             = charge_lyt.get_defects();
        number_of_SiDBs                                 = charge_lyt.get_sidb_order().size();
    }
    /**
     * This function is used to generate a layout without the SiDBs that are detected to be negatively charged in a
     * physically-valid layout.
     */
    void generate_layout_without_negative_sidbs()
    {
        for (const auto& index : detected_negative_sidb_indices)
        {
            const auto cell = charge_lyt.index_to_cell(index);
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
                               return std::find(detected_negative_sidbs.begin(), detected_negative_sidbs.end(), n) !=
                                      detected_negative_sidbs.end();
                           }),
            all_sidbs_in_lyt_without_negative_detected_ones.end());
    };
    /**
     * Cell-level layout
     */
    Lyt layout;
    /**
     * Charge distribution surface
     */
    charge_distribution_surface<Lyt> charge_lyt{};
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
    uint64_t number_of_SiDBs{};
    /**
     * Parameter used for the simulation.
     */
    quickexact_params<Lyt> parameter{};
    /**
     * Simulation results.
     */
    sidb_simulation_result<Lyt> result{};
};

}  // namespace detail

/**
 * *Quickexact* is a *quick* and *exact* physical simulation algorithm for SiDB layouts. It determines all physically
 * valid charge configurations of a given layout. It shows a performance advantage of more than three orders of
 * magnitude over the state of the art. It also computes efficiently when positively charged SiDB can occur
 * in the layout due to small spacing. Finally, it also allows the simulation of an SiDB layout while taking global and
 * local electrostatic potentials and existing defects into account.
 *
 * @tparam Lyt Cell-level layout type.
 * @param params Parameters used for the simulation. This includes physical parameters, external potentials, and
 * defects.
 * @return sidb_simulation_result is returned with all results.
 */
template <typename Lyt>
sidb_simulation_result<Lyt> quickexact(Lyt& lyt, const quickexact_params<Lyt>& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    detail::quickexact_impl<Lyt> p{lyt, params};

    p.run();

    return p.get_simulation_results();
}

}  // namespace fiction

#endif  // FICTION_QUICKEXACT_HPP
