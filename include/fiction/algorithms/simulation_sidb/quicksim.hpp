//
// Created by Jan Drewniok on 11.01.23.
//

#ifndef FICTION_QUICKSIM_HPP
#define FICTION_QUICKSIM_HPP

#include "fiction/algorithms/simulation_sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation_sidb/minimum_energy.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/algorithms/simulation_sidb/quicksim_params.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

namespace fiction
{
/**
 * This struct stores the simulation runtime and all physically valid charge layouts gained by the quicksim simulation
 * (see quicksim.hpp).
 *
 * @paramt Lyt cell-level layout.
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
 * quicksim determines physically valid charge configurations (with minimal energy) of a given (already initialized)
 * charge distribution layout. Depending on the simulation paramaters, the ground state is found with a certain
 * probability after one run.
 *
 * @tparam Lyt cell-level layout.
 * @param lyt charge distribution layout.
 * @param ps struct that stores the simulation results (simulation runtime, and all physically valid charge distribution
 * layouts).
 * @param physical_params physical parameters, they are material-specific and may vary from experiment to experiment.
 */
template <typename Lyt>
void quicksim(charge_distribution_surface<Lyt>& lyt, const quicksim_params &quick_params = quicksim_params{},
              quicksim_stats<Lyt>* ps = nullptr)
{
    quicksim_stats<Lyt> st{};
    // set the given physical parameters

    lyt.set_physical_parameters(quick_params.phys_params);

    // measure run time (artificial scope)
    {
        mockturtle::stopwatch stop{st.time_total};

        std::vector<charge_distribution_surface<Lyt>> result{};

        lyt.set_all_charge_states(sidb_charge_state::NEUTRAL);
        lyt.update_after_charge_change();

        if (lyt.is_physically_valid())
        {
            charge_distribution_surface<Lyt> lyt_new{lyt};
            st.valid_lyts.push_back(lyt_new);
        }

        lyt.set_all_charge_states(sidb_charge_state::NEGATIVE);
        lyt.update_after_charge_change();

        if (lyt.is_physically_valid())
        {
            charge_distribution_surface<Lyt> lyt_new{lyt};
            st.valid_lyts.push_back(lyt_new);
        }

        auto best_energy = std::numeric_limits<double>::max();
        auto bound       = static_cast<uint64_t>(std::round(0.6 * static_cast<double>(lyt.num_cells())));
        for (uint64_t z = 0u; z < quick_params.interation_steps; z++)
        {
            for (uint64_t i = 0u; i < bound; i++)
            {
                std::vector<uint64_t> index_start = {i};
                lyt.set_all_charge_states(sidb_charge_state::NEUTRAL);
                lyt.assign_charge_state_by_cell_index(i, sidb_charge_state::NEGATIVE);
                lyt.update_local_potential();
                lyt.system_energy();

                auto upperlimit = static_cast<uint64_t>(static_cast<double>(lyt.num_cells()) / 1.5);
                for (uint64_t num = 0; num < upperlimit; num++)
                {
                    lyt.adjacent_search(quick_params.alpha, index_start);
                    lyt.validity_check();

                    if (lyt.is_physically_valid() && (lyt.get_system_energy() <= best_energy))
                    {
                        charge_distribution_surface<Lyt> lyt_new{lyt};
                        st.valid_lyts.push_back(lyt_new);
                    }
                }
            }
        }
    }

    if (ps)
    {
        *ps = st;
    }
}

}  // namespace fiction

#endif  // FICTION_QUICKSIM_HPP
