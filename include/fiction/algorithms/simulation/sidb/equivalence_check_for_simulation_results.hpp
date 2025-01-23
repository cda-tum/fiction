//
// Created by Jan Drewniok on 03.03.24.
//

#ifndef FICTION_EQUIVALENCE_CHECK_FOR_SIMULATION_RESULTS_HPP
#define FICTION_EQUIVALENCE_CHECK_FOR_SIMULATION_RESULTS_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>

namespace fiction
{

/**
 * This function compares two SiDB simulation results for equivalence. Two results are considered equivalent if they
 * have the same number of charge distributions and if each corresponding charge distribution has the same electrostatic
 * potential energy and charge states for all cells.
 *
 * @tparam Lyt The SiDB cell-level layout type used in the simulation results.
 * @param result1 The first SiDB simulation result to compare.
 * @param result2 The second SiDB simulation result to compare.
 * @return `true` if the two simulation results are equivalent, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool check_simulation_results_for_equivalence(sidb_simulation_result<Lyt> result1,
                                                            sidb_simulation_result<Lyt> result2)
{
    if (result1.charge_distributions.size() != result2.charge_distributions.size())
    {
        return false;
    }

    if (!result1.charge_distributions.empty() && !result2.charge_distributions.empty())
    {
        if (result1.charge_distributions.front().get_sidb_order().size() !=
            result2.charge_distributions.front().get_sidb_order().size())
        {
            return false;
        }
    }

    std::set<uint64_t> unique_charge_indices1;
    for (const auto& cds1 : result1.charge_distributions)
    {
        unique_charge_indices1.insert(cds1.get_charge_index_and_base().first);
    }

    // check if all charge indices are unique
    if (unique_charge_indices1.size() != result1.charge_distributions.size())
    {
        return false;
    }

    std::set<uint64_t> unique_charge_indices2;
    for (const auto& cds2 : result2.charge_distributions)
    {
        unique_charge_indices2.insert(cds2.get_charge_index_and_base().first);
    }

    // check if all charge indices are unique
    if (unique_charge_indices2.size() != result2.charge_distributions.size())
    {
        return false;
    }

    std::sort(result1.charge_distributions.begin(), result1.charge_distributions.end(),
              [](const auto& lhs, const auto& rhs)
              { return lhs.get_charge_index_and_base().first < rhs.get_charge_index_and_base().first; });

    std::sort(result2.charge_distributions.begin(), result2.charge_distributions.end(),
              [](const auto& lhs, const auto& rhs)
              { return lhs.get_charge_index_and_base().first < rhs.get_charge_index_and_base().first; });

    for (auto i = 0u; i < result1.charge_distributions.size(); i++)
    {
        const auto& cds1 = result1.charge_distributions.at(i);
        const auto& cds2 = result2.charge_distributions.at(i);
        if (std::abs(cds1.get_system_energy() - cds2.get_system_energy()) > std::numeric_limits<double>::epsilon())
        {
            return false;
        }

        bool charge_states_equal = true;
        cds1.foreach_cell(
            [&cds1, &cds2, &charge_states_equal](const auto& c)
            {
                if (cds1.get_charge_state(c) != cds2.get_charge_state(c))
                {
                    charge_states_equal = false;
                }
            });

        if (!charge_states_equal)
        {
            return false;
        }
    }

    return true;
}

}  // namespace fiction

#endif  // FICTION_EQUIVALENCE_CHECK_FOR_SIMULATION_RESULTS_HPP
