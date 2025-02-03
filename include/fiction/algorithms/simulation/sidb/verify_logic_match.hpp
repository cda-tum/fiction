//
// Created by Jan Drewniok on 12.11.24.
//

#ifndef FICTION_VERIFY_LOGIC_MATCH_HPP
#define FICTION_VERIFY_LOGIC_MATCH_HPP

#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <kitty/traits.hpp>

#include <cassert>
#include <cstdint>
#include <vector>

namespace fiction
{

/**
 * Checks if a given charge distribution correctly encodes the expected logic for a specified input pattern,
 * based on a provided truth table.
 *
 * @note Kinks are rejected.
 *
 * Example:
 * In the ground state charge distribution of an AND gate, kinks are rejected for the gate to be considered operational.
 * Given an input pattern of `01`, this function will:
 * - Verify that the left input wire encodes `0`.
 * - Verify that the right input wire encodes `1`.
 * - Verify that the output wire encodes `0`.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param cds Charge distribution surface, containing charge state information for each SiDB.
 * @param params The parameters used to determine if a layout is `operational` or `non-operational`.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param input_pattern The specific input pattern of the given charge distribution surface.
 * @param input_wires Input BDL wires.
 * @param output_wires Output BDL wires.
 * @return The operational status indicating if the charge distribution matches the logic for the given input pattern.
 */
template <typename Lyt, typename TT>
[[nodiscard]] operational_status verify_logic_match(const charge_distribution_surface<Lyt>& cds,
                                                    const is_operational_params& params, const std::vector<TT>& spec,
                                                    const uint64_t                    input_pattern,
                                                    const std::vector<bdl_wire<Lyt>>& input_wires,
                                                    const std::vector<bdl_wire<Lyt>>& output_wires) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.cbegin(), spec.cend(), [](const auto& a, const auto& b)
                              { return a.num_vars() != b.num_vars(); }) == spec.cend());

    detail::is_operational_impl<Lyt, TT> p{cds, spec, params, input_wires, output_wires, false};

    const auto [op_status, _] = p.verify_logic_match_of_cds(cds, input_pattern);

    return op_status;
}

}  // namespace fiction

#endif  // FICTION_VERIFY_LOGIC_MATCH_HPP
