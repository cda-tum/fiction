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
 * @param lyt The layout to simulate.
 * @param params Simulation parameters.
 * @param ps Simulation statistics.
 */
template <typename Lyt>
void exhaustive_ground_state_simulation(Lyt&                              lyt,
                                        const sidb_simulation_parameters& params = sidb_simulation_parameters{},
                                        exgs_stats<Lyt>*                  ps     = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    exgs_stats<Lyt> st{};

    {
        mockturtle::stopwatch stop{st.time_total};

        charge_distribution_surface charge_lyt{lyt};

        charge_lyt.set_physical_parameters(params);
        charge_lyt.set_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.update_after_charge_change();
        const auto negative_sidb_indices = charge_lyt.negative_sidb_detection();
        std::cout << negative_sidb_indices.size() << std::endl;
        std::vector<typename Lyt::cell> negative_sidbs{};
        negative_sidbs.reserve(negative_sidb_indices.size());
        for (const auto& index : negative_sidb_indices)
        {
            const auto cell = charge_lyt.index_to_cell(index);
            negative_sidbs.push_back(cell);
            lyt.assign_cell_type(cell, Lyt::cell_type::EMPTY);
        }

        charge_distribution_surface charge_lyt_new{lyt, params};

        for (const auto& cell : negative_sidbs)
        {
            charge_lyt_new.assign_defect(cell, sidb_defect{sidb_defect_type::UNKNOWN, -1});
        }

        charge_lyt_new.update_after_charge_change();

        while (charge_lyt_new.get_charge_index().first < charge_lyt_new.get_max_charge_index())
        {

            if (charge_lyt_new.is_physically_valid())
            {
                //                for (const auto & cell : negative_sidbs)
                //                {
                //                    charge_lyt_new.assign_cell_type(cell, Lyt::cell_type::NORMAL);
                //                    charge_lyt_new.assign_charge_state(cell, sidb_charge_state::NEGATIVE);
                //                }
                st.valid_lyts.push_back(charge_distribution_surface<Lyt>{charge_lyt_new});
            }

            charge_lyt_new.increase_charge_index_by_one();
        }

        for (const auto& cell : negative_sidbs)
        {
            lyt.assign_cell_type(cell, Lyt::cell_type::NORMAL);
        }
    }

    if (ps)
    {
        *ps = st;
    }
}

}  // namespace fiction

#endif  // FICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
