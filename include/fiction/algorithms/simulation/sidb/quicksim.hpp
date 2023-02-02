//
// Created by Jan Drewniok on 11.01.23.
//

#ifndef FICTION_QUICKSIM_HPP
#define FICTION_QUICKSIM_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the quicksim algorithm.
 */
struct quicksim_params
{
    /**
     * General parameters for the simulation of the physical SiDB system.
     */
    sidb_simulation_parameters phys_params{};
    /**
     * Number of iterations to run the simulation for.
     */
    uint64_t interation_steps{80};
    /**
     * Alpha parameter for the quicksim algorithm.
     */
    double alpha{0.7};
};

/**
 * This struct stores the simulation runtime and all physically valid charge layouts gained by the quicksim algorithm.
 *
 * @paramt Lyt Cell-level layout type.
 */
template <typename Lyt>
struct quicksim_stats
{
    mockturtle::stopwatch<>::duration             time_total{0};
    std::vector<charge_distribution_surface<Lyt>> valid_lyts{};

    void report(std::ostream& out = std::cout)
    {
        out << fmt::format("total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        if (!energy_distribution<Lyt>(valid_lyts).empty())
        {
            for (auto [energy, count] : energy_distribution<Lyt>(valid_lyts))
            {
                out << fmt::format("[i] the lowest state energy is  = {:.4f} \n", minimum_energy(valid_lyts));
                out << fmt::format("energy: {} | occurance: {} \n", energy, count);
            }
        }
        else
        {
            std::cout << "no state found" << std::endl;
        }
        std::cout << "_____________________________________________________ \n";
    }
};

/**
 * The *quicksim* algorithm is an electrostatic ground state simulation algorithm for SiDB layouts. It determines
 * physically valid charge configurations (with minimal energy) of a given (already initialized) charge distribution
 * layout. Depending on the simulation parameters, the ground state is found with a certain probability after one run.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The layout to simulate.
 * @param ps Physical parameters. They are material-specific and may vary from experiment to experiment.
 * @param pst Statistics. They store the simulation results (simulation runtime as well as all physically valid charge
 * distribution layouts).
 */
template <typename Lyt>
void quicksim(const Lyt& lyt, const quicksim_params& ps = quicksim_params{}, quicksim_stats<Lyt>* pst = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    quicksim_stats<Lyt> st{};

    // measure run time (artificial scope)
    {
        mockturtle::stopwatch stop{st.time_total};

        charge_distribution_surface charge_lyt{lyt};

        // set the given physical parameters
        charge_lyt.set_physical_parameters(ps.phys_params);

        std::vector<charge_distribution_surface<Lyt>> result{};

        charge_lyt.set_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        if (charge_lyt.is_physically_valid())
        {
            st.valid_lyts.push_back(charge_distribution_surface<Lyt>{charge_lyt});
        }

        charge_lyt.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.update_after_charge_change();

        if (charge_lyt.is_physically_valid())
        {
            st.valid_lyts.push_back(charge_distribution_surface<Lyt>{charge_lyt});
        }

        auto       best_energy = std::numeric_limits<double>::max();
        const auto bound       = static_cast<uint64_t>(std::round(0.6 * static_cast<double>(charge_lyt.num_cells())));
        for (uint64_t z = 0u; z < ps.interation_steps; z++)
        {
            for (uint64_t i = 0u; i < bound; i++)
            {
                std::vector<uint64_t> index_start{i};
                charge_lyt.set_all_charge_states(sidb_charge_state::NEUTRAL);
                charge_lyt.assign_charge_state_by_cell_index(i, sidb_charge_state::NEGATIVE);
                charge_lyt.update_local_potential();
                charge_lyt.recompute_system_energy();

                const auto upper_limit = static_cast<uint64_t>(static_cast<double>(charge_lyt.num_cells()) / 1.5);
                for (uint64_t num = 0; num < upper_limit; num++)
                {
                    charge_lyt.adjacent_search(ps.alpha, index_start);
                    charge_lyt.validity_check();

                    if (charge_lyt.is_physically_valid() && (charge_lyt.get_system_energy() <= best_energy))
                    {
                        st.valid_lyts.push_back(charge_distribution_surface<Lyt>{charge_lyt});
                    }
                }
            }
        }
    }

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_QUICKSIM_HPP
