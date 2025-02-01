//
// Created by Jan Drewniok on 25.10.23.
//

#ifndef FICTION_IS_SIDB_GATE_DESIGN_IMPOSSIBLE_HPP
#define FICTION_IS_SIDB_GATE_DESIGN_IMPOSSIBLE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <cassert>
#include <vector>

namespace fiction
{
/**
 * This struct contains parameters to determine if SiDB gate design is impossible.
 */
struct is_sidb_gate_design_impossible_params
{
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb_simulation_parameters simulation_params{};
    /**
     * Parameters used for the BDL input iterator.
     */
    bdl_input_iterator_params bdl_iterator_params{};
};
/**
 * This function evaluates whether it is impossible to design an SiDB gate for a given truth table and a given skeleton
 * with atomic defects. It determines the possible charge states at the output BDL pairs. Atomic defects can cause a BDL
 * pair to be neutrally charged only. Thus, the BDL pair would not work as intended.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The truth table type.
 * @param skeleton_with_defects An SiDB skeleton layout with atomic defects.
 * @param spec A vector of truth tables (each truth table is representing one output) representing the gate's intended
 * functionality.
 * @param params Parameters to determine if the gate design is impossible.
 * @return `true` if gate design is impossible, `false` otherwise.
 */
template <typename Lyt, typename TT>
[[nodiscard]] bool is_sidb_gate_design_impossible(const Lyt& skeleton_with_defects, const std::vector<TT>& spec,
                                                  const is_sidb_gate_design_impossible_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_sidb_defect_surface_v<Lyt>, "Lyt is not an SiDB defect surface");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not an SiDB cell-level layout");

    assert(skeleton_with_defects.num_pis() > 0 && "lyt needs input cells");
    assert(skeleton_with_defects.num_pos() > 0 && "lyt needs output cells");

    const auto output_pairs = detect_bdl_pairs(skeleton_with_defects, sidb_technology::cell_type::OUTPUT,
                                               params.bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

    assert(output_pairs.empty() == false && "lyt needs output BDL pairs");

    auto bdl_iter = bdl_input_iterator<Lyt>{skeleton_with_defects, params.bdl_iterator_params};

    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bdl_iter)
    {
        auto charge_lyt = charge_distribution_surface<Lyt>{skeleton_with_defects, params.simulation_params};
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        skeleton_with_defects.foreach_sidb_defect(
            [&charge_lyt](const auto& cd)
            {
                const auto& [defect_pos, defect] = cd;
                if (is_charged_defect_type(defect))
                {
                    charge_lyt.add_sidb_defect_to_potential_landscape(defect_pos, defect);
                }
            });

        // checks if parts of the bdl pairs are already neutrally charged due to nearby charged atomic defects.
        for (const auto& bdl : output_pairs)
        {
            if ((-(*charge_lyt.get_local_potential(bdl.lower)) + params.simulation_params.mu_minus) >
                -constants::ERROR_MARGIN)
            {
                return true;  // the lower part can never be negatively charged. Thus, BDL property is not fulfilled
                              // anymore
            }
            if ((-(*charge_lyt.get_local_potential(bdl.upper)) + params.simulation_params.mu_minus) >
                -constants::ERROR_MARGIN)
            {
                return true;  // the upper part can never be negatively charged. Thus, BDL property is not fulfilled
                              // anymore
            }
        }
    }

    return false;
}

}  // namespace fiction

#endif  // FICTION_IS_SIDB_GATE_DESIGN_IMPOSSIBLE_HPP
