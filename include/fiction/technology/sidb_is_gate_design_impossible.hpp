//
// Created by Jan Drewniok on 25.10.23.
//

#ifndef FICTION_SIDB_IS_GATE_DESIGN_IMPOSSIBLE_HPP
#define FICTION_SIDB_IS_GATE_DESIGN_IMPOSSIBLE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/truth_table_utils.hpp"

namespace fiction
{

/**
 * This function assesses whether it is impossible to design an SiDB gate for a given truth table in the provided layout
 * due to atomic defects.
 *
 * @note If the function returns `false`, it does not imply that it is possible to design an SiDB gate for given
 * parameters.
 *
 * @tparam Lyt The type of the layout.
 * @tparam TT The type of the truth table.
 * @param layout The layout for which gate design feasibility is being checked.
 * @param spec A vector of truth tables representing the gate's functionality.
 * @param params Parameter for the simulation.
 * @return `true` if gate design is impossible, `false` otherwise.
 *
 */
template <typename Lyt, typename TT>
bool is_gate_design_impossible(const Lyt& layout, const std::vector<TT>& spec, const is_operational_params& params = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    assert(layout.num_pis() > 0 && "lyt needs input cells");
    assert(layout.num_pos() > 0 && "lyt needs output cells");

    const auto bdl_pairs = detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params);

    auto bdl_iter = bdl_input_iterator<Lyt>{layout, params.bdl_params};

    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bdl_iter)
    {
        auto charge_lyt = charge_distribution_surface<Lyt>{layout, params.simulation_parameter};
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        layout.foreach_sidb_defect(
            [&charge_lyt](const auto& cd)
            {
                if (is_charged_defect(cd.second))
                {
                    charge_lyt.add_sidb_defect_to_potential_landscape(cd.first, cd.second);
                }
            });

        // this function checks if parts of the bdl pairs are already neutrally charged due to nearby charged atomic
        // defects.
        for (const auto& bdl : bdl_pairs)
        {
            if ((-(*charge_lyt.get_local_potential(bdl.lower)) + params.simulation_parameter.mu_minus) >
                -physical_constants::POP_STABILITY_ERR)
            {
                return true;  // the lower part can never be negatively charged. Thus, BDL property is not fulfilled
                              // anymore
            }
            if ((-(*charge_lyt.get_local_potential(bdl.upper)) + params.simulation_parameter.mu_minus) >
                -physical_constants::POP_STABILITY_ERR)
            {
                return true;  // the upper part can never be negatively charged. Thus, BDL property is not fulfilled
                              // anymore
            }
        }
    }
    return false;
}

}  // namespace fiction

#endif  // FICTION_SIDB_IS_GATE_DESIGN_IMPOSSIBLE_HPP
