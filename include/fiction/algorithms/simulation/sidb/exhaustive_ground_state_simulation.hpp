//
// Created by Jan Drewniok on 18.12.22.
//

#ifndef FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
#define FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <iostream>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct exgs_stats
{
    mockturtle::stopwatch<>::duration             time_total{0};
    std::vector<charge_distribution_surface<Lyt>> valid_lyts{};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("total time  = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        if (!valid_lyts.empty())
        {
            for (const auto& [energy, count] : energy_distribution<Lyt>(valid_lyts))
            {
                out << fmt::format("energy: {} | occurance: {} \n", energy, count);
            }
            out << fmt::format("the ground state energy is  = {:.4f} \n", minimum_energy(valid_lyts));
        }
        else
        {
            std::cout << "no state found | if two state simulation is used, continue with three state" << std::endl;
        }

        out << fmt::format("{} phyiscally valid charge states were found \n", valid_lyts.size());
        std::cout << "_____________________________________________________ \n";
    }
};

/**
 *  All metastable and physically valid charge distribution layouts are computed, stored in a vector and returned.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt Charge distribution layout.
 * @param params Simulation parameters.
 * @param ps Simulation statistics.
 */
template <typename Lyt>
void exhaustive_ground_state_simulation(charge_distribution_surface<Lyt>& lyt,
                                        const sidb_simulation_parameters& params = sidb_simulation_parameters{},
                                        exgs_stats<Lyt>*                  ps     = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    exgs_stats<Lyt> st{};

    {
        mockturtle::stopwatch stop{st.time_total};
        lyt.set_physical_parameters(params);
        lyt.set_all_charge_states(sidb_charge_state::NEGATIVE);
        lyt.update_after_charge_change();

        while (lyt.get_charge_index().first < lyt.get_max_charge_index())
        {

            if (lyt.is_physically_valid())
            {
                charge_distribution_surface<Lyt> lyt_new{lyt};
                st.valid_lyts.push_back(lyt_new);
            }

            lyt.increase_charge_index_by_one();
        }

        if (lyt.is_physically_valid())
        {
            charge_distribution_surface<Lyt> lyt_new{lyt};
            st.valid_lyts.push_back(lyt_new);
        }
    }

    if (ps)
    {
        *ps = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
