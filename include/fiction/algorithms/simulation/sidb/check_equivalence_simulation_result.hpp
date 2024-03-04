//
// Created by Jan Drewniok on 03.03.24.
//

#ifndef FICTION_CHECK_EQUAL_CDS_RESULT_HPP
#define FICTION_CHECK_EQUAL_CDS_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace fiction
{

/**
 * This function compares two SIDB simulation results for equivalence. Two results are considered equivalent if they
 * have the same number of charge distributions and if each corresponding charge distribution has the same electrostatic
 * potential energy and charge states for all cells.
 *
 * @tparam Lyt The layout type used in the simulation results.
 * @param result1 The first SiDB simulation result to compare.
 * @param result2 The second SiDB simulation result to compare.
 * @return `true` if the two simulation results are equivalent, `false otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool check_equivalence_simulation_result(sidb_simulation_result<Lyt>& result1,
                                                       sidb_simulation_result<Lyt>& result2)
{
    if (result1.charge_distributions.size() != result2.charge_distributions.size())
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
        const auto& cds1 = result1.charge_distributions[i];
        const auto& cds2 = result2.charge_distributions[i];
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

        charge_states_equal = true;
        cds2.foreach_cell(
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

#endif  // FICTION_CHECK_EQUAL_CDS_RESULT_HPP
