//
// Created by Jan Drewniok on 12.11.24.
//

#ifndef FICTION_DOES_CHARGE_DISTRIBUTION_MATCH_LOGIC_FOR_INPUT_PATTERN_HPP
#define FICTION_DOES_CHARGE_DISTRIBUTION_MATCH_LOGIC_FOR_INPUT_PATTERN_HPP

#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/traits.hpp>

#include <kitty/traits.hpp>

#include <cassert>
#include <cstdint>
#include <vector>

namespace fiction
{

/**
 * This function checks if a charge distribution encodes the correct logic for a specified input pattern
 * based on a provided truth table.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param cds Charge distribution surface, containing charge state information for each SiDB.
 * @param params The parameters used to determine if a layout is `operational` or `non-operational`.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_pattern The specific input pattern.
 * @param input_bdl_wire Vector of input BDL wires.
 * @param output_bdl_wire Vector of output BDL wires.
 *
 * @return The operational status indicating if the charge distribution matches the logic for the given input pattern.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_status does_charge_distribution_match_logic_for_input_pattern(
    const charge_distribution_surface<Lyt>& cds, const is_operational_params& params, const std::vector<TT>& spec,
    const std::uint64_t input_pattern, const std::vector<bdl_wire<Lyt>>& input_bdl_wire,
    const std::vector<bdl_wire<Lyt>>& output_bdl_wire) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    detail::is_operational_impl<Lyt, TT> p{cds, spec, params, input_bdl_wire, output_bdl_wire};

    return p.is_given_cds_operational_for_pattern(cds, input_pattern);
}

}  // namespace fiction

#endif  // FICTION_DOES_CHARGE_DISTRIBUTION_MATCH_LOGIC_FOR_INPUT_PATTERN_HPP
