//
// Created by Jan Drewniok on 28.12.23.
//

#ifndef FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP
#define FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/utils/layout_utils.hpp"

namespace fiction
{

struct maximum_defect_influence_position_and_distance_of_sidb_gate_params
{
    maximum_defect_influence_distance_params defect_influence_params{};
    detect_bdl_pairs_params                  bdl_pairs_params{};
};

template <typename Lyt, typename TT>
[[nodiscard]] std::pair<typename Lyt::cell, double> maximum_defect_influence_position_and_distance_of_sidb_gate(
    const Lyt& lyt, const std::vector<TT>& spec,
    const maximum_defect_influence_position_and_distance_of_sidb_gate_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(!has_offset_ucoord_v<Lyt>, "Lyt should not be based on offset coordinates");
    static_assert(!is_charge_distribution_surface_v<Lyt>, "Lyt cannot be a charge distribution surface");

    assert(lyt.num_pis() > 0 && "skeleton needs input cells");
    assert(lyt.num_pos() > 0 && "skeleton needs output cells");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    bdl_input_iterator<Lyt> bii{lyt, params.bdl_pairs_params};
    double                  maximum_defect_influence_distance = 0.0;
    cell<Lyt>               defect_cell{};
    // number of different input combinations
    for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
    {
        maximum_defect_influence_distance_stats stats_defect{};
        const auto                              influence_cell_distance =
            maximum_defect_influence_position_and_distance(lyt, params.defect_influence_params, &stats_defect);
        std::cout << mockturtle::to_seconds(stats_defect.time_total) << std::endl;
        if (influence_cell_distance.second > maximum_defect_influence_distance)
        {
            maximum_defect_influence_distance = influence_cell_distance.second;
            defect_cell                       = influence_cell_distance.first;
        }
    }
    return {defect_cell, maximum_defect_influence_distance};
}

}  // namespace fiction

#endif  // FICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_OF_SIDB_GATE_HPP
