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

#include <iostream>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the `critical_temperature` algorithm.
 */
template <typename Lyt>
struct quickexact_params
{
    /**
     * All Parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters physical_parameters{};

    std::unordered_map<typename Lyt::cell, const sidb_defect> defects{};

    std::unordered_map<typename Lyt::cell, double> local_external_potential = {};
    double                                         global_potential         = 0;
};

namespace detail
{
/**
 *  All metastable and physically valid charge distribution layouts are computed, stored in a vector and returned.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The layout to simulate.
 * @param params Simulation parameters.
 * @param ps Simulation statistics.
 * @return sidb_simulation_result is returned with all results.
 */
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

            //  determine all SiDBs that have to be negatively charged to fulfill the population stability. This is an
            //  efficient way to prune the search space by 2^k with k being the number of detected negatively charged
            //  SiDBs.
            //  determine if three state simulation (i.e. positively charged SiDBs can occur) is required.
            const auto three_state_simulation_required = charge_lyt.three_state_sim_required();
            // std::vector<typename Lyt::cell> detected_negative_sidbs{};
            // detected_negative_sidbs.reserve(detected_negative_sidb_indices.size());

            // if layout has at least two SiDBs, the code inside this if-statement is executed.
            if (num_cells > 1)
            {
                this->generate_layout_without_negative_sidbs();
            }
            if (!all_sidbs_in_lyt_without_detected_ones.empty() && num_cells > 1)
            {
                // the first cell from all_sidbs_in_lyt_without_detected_ones is chosen as the dependent cell to
                // initialize the layout (detected negatively charged SiDBs were erased in the step before).
                charge_distribution_surface charge_lyt_new{layout, parameter.physical_parameters,
                                                           sidb_charge_state::NEUTRAL,
                                                           all_sidbs_in_lyt_without_detected_ones[0]};
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
                // update all local potentials, system energy and physically validity. Flag is set to "false" to allow
                // dependent cell to change its charge state based on the N-1 SiDBs to fulfill the local population
                // stability at its position.

                // false declares that the dependent cell is updated based on the local potential at the position.
                charge_lyt_new.update_after_charge_change(false);

                // if no positively charged DB can occur in the layout, this scope is executed.
                if (!three_state_simulation_required)
                {
                    this->two_state_sim(charge_lyt_new, simulation_result);
                }

                // if positively charged DBs can occur in the layout, 3-state simulation is conducted.
                else
                {
                    this->three_state_sim(charge_lyt_new, simulation_result);
                }
            }

            // in the case with only one SiDB in the layout (due to external potentials or defects, this single SiDB can
            // be neutrally or even positively charged.)
            else if (num_cells == 1)
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
            // if the layout consists of only detected negatively charged SiDBs, this scope is
            // executed.
            else if (all_sidbs_in_lyt_without_detected_ones.empty() && num_cells > 1)
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
    void two_state_sim(charge_distribution_surface<Lyt>& charge_lyt_new, sidb_simulation_result<Lyt>& sim_result)
    {
        charge_lyt_new.set_base_number(2);
        uint64_t current_charge_index  = 0;
        uint64_t previous_charge_index = 0;
        for (uint64_t i = 0; i <= charge_lyt_new.get_max_charge_index(); i++)
        {
            current_charge_index = (i ^ (i >> 1));
            charge_lyt_new.set_charge_index_by_gray_code(
                current_charge_index, previous_charge_index, false, false,
                true);  // "false" allows that the charge state of the dependent cell is
                        // automatically changed based on the new charge distribution.
            previous_charge_index = current_charge_index;

            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // the previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
            }
        }

        // the cells of the previously detected negatively charged SiDBs are added to the cell level layout.
        for (const auto& cell : detected_negative_sidbs)
        {
            layout.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }

    void three_state_sim(charge_distribution_surface<Lyt>& charge_lyt_new, sidb_simulation_result<Lyt>& sim_result)
    {
        charge_lyt_new.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt_new.update_after_charge_change();
        const auto three_state_simulation_required = charge_lyt_new.three_state_sim_required();
        charge_lyt_new.update_after_charge_change(false);
        while (charge_lyt_new.get_charge_index().first < charge_lyt_new.get_max_charge_index())
        {
            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // the previously detected negatively charged SiDBs are added to the final layout.
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
                    // the previously detected negatively charged SiDBs are added to the final layout.
                    for (const auto& cell : detected_negative_sidbs)
                    {
                        charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                    }
                    sim_result.charge_distributions.push_back(charge_lyt_copy);
                }
                charge_lyt_new.increase_charge_index_by_sub_layout(
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

            charge_lyt_new.reset_charge_index_sub_layout(false, false, true, true);

            charge_lyt_new.increase_charge_index_by_one(
                false, false, true, true);  // "false" allows that the charge state of the dependent cell is
                                            // automatically changed based on the new charge distribution.
        }

        while (charge_lyt_new.get_charge_index_sub_layout().first < charge_lyt_new.get_max_charge_index_sub_layout())
        {
            if (charge_lyt_new.is_physically_valid())
            {
                charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt_new};
                charge_lyt_copy.recompute_system_energy();
                // the previously detected negatively charged SiDBs are added to the final layout.
                for (const auto& cell : detected_negative_sidbs)
                {
                    charge_lyt_copy.adding_sidb_to_layout(cell, -1);
                }
                sim_result.charge_distributions.push_back(charge_lyt_copy);
            }
            charge_lyt_new.increase_charge_index_by_sub_layout(
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

    void initialize_charge_layout()
    {
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
        all_sidbs_in_lyt_without_detected_ones = charge_lyt.get_sidb_order();
        real_placed_defects                    = charge_lyt.get_defects();
        num_cells                              = charge_lyt.get_sidb_order().size();
    }

    void generate_layout_without_negative_sidbs()
    {
        for (const auto& index : detected_negative_sidb_indices)
        {
            const auto cell = charge_lyt.index_to_cell(index);
            detected_negative_sidbs.push_back(cell);
            layout.assign_cell_type(cell, Lyt::cell_type::EMPTY);
        }

        // all detected negatively charged SiDBs are erased from the all_sidbs_in_lyt_without_detected_ones
        // vector.
        all_sidbs_in_lyt_without_detected_ones.erase(
            std::remove_if(all_sidbs_in_lyt_without_detected_ones.begin(), all_sidbs_in_lyt_without_detected_ones.end(),
                           [this](const typename Lyt::cell& n)
                           {
                               return std::find(detected_negative_sidbs.begin(), detected_negative_sidbs.end(), n) !=
                                      detected_negative_sidbs.end();
                           }),
            all_sidbs_in_lyt_without_detected_ones.end());
    };

    Lyt layout{};
    /**
     * Parameters for the `critical_temperature` algorithm.
     */
    charge_distribution_surface<Lyt> charge_lyt{};
    std::vector<int64_t>             detected_negative_sidb_indices;
    std::vector<typename Lyt::cell>  detected_negative_sidbs{};
    std::vector<typename Lyt::cell>  all_sidbs_in_lyt_without_detected_ones;

    std::unordered_map<typename Lyt::cell, const sidb_defect> real_placed_defects;
    uint64_t                                                  num_cells{};

    quickexact_params<Lyt> parameter;

    sidb_simulation_result<Lyt> result{};
};
}  // namespace detail

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
