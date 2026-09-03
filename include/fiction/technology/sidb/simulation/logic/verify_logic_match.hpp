/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Checks a charge distribution against the logic expected for an input pattern.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"

#include <kitty/traits.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace fiction::sidb::simulation::logic
{

/**
 * Checks whether a given charge distribution of a layout implements the expected output for an input pattern: the
 * output BDL pairs have to encode the truth-table entries and, if the parameters reject kinks, the wires have to be
 * free of kinks. If positively charged SiDBs can occur in the layout under the given parameters (base 2 only), the
 * layout is non-operational.
 *
 * @tparam TT Truth table type.
 * @param lyt The layout the charge distribution belongs to.
 * @param cd The charge distribution to check.
 * @param params Parameters.
 * @param spec The Boolean function(s) to implement.
 * @param input_pattern The input pattern `cd` was simulated for.
 * @param input_wires The input BDL wires of `lyt`.
 * @param output_wires The output BDL wires of `lyt`.
 * @return The operational status.
 */
template <typename TT>
[[nodiscard]] operational_status
verify_logic_match(const layout& lyt, const charge_distribution& cd, const is_operational_params& params,
                   const std::vector<TT>& spec, const uint64_t input_pattern, const std::vector<bdl_wire>& input_wires,
                   const std::vector<bdl_wire>& output_wires) noexcept
{
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.cend());

    if (params.sim_params.base == 2 && analysis::can_positive_charges_occur(lyt, params.sim_params))
    {
        return operational_status::NON_OPERATIONAL;
    }

    const detail::is_operational_impl<TT> p{lyt, spec, params, input_wires, output_wires, false};

    const auto [op_status, _] = p.verify_logic_match_of_cd(cd, input_pattern);

    return op_status;
}

}  // namespace fiction::sidb::simulation::logic
