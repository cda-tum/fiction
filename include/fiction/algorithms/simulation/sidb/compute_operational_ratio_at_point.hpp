//
// Created by Jan Drewniok on 09.09.24.
//

#ifndef FICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP
#define FICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"

#include <kitty/traits.hpp>

#include <vector>

namespace fiction
{
/**
 * Parameters for computing the ratio of operational parameter points around a specified parameter point
 * to the total number of parameter points in the given parameter space.
 */
struct compute_operational_ratio_at_point_params
{
    operational_domain_params op_domain_params{};
};

/**
 * Calculates the ratio of operational parameter points surrounding a specified parameter point to the total
 * number of parameter points in the given parameter space. This function is useful for assessing how robust
 * a gate design is to variations in its parameters.
 *
 * A ratio close to 1 indicates that the gate is robust, meaning it functions correctly across a broad range of
 * parameter values. A ratio close to 0 indicates that the gate is highly sensitive to parameter variations and may fail
 * to operate correctly.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Truth table type.
 * @param lyt The SiDB layout for which to compute the ratio of operational parameter points surrounding a specified
 * parameter point to the total number of parameter points.
 * @param spec The expected Boolean function of the layout, provided as a multi-output truth table.
 * @param params Parameters.
 * @param pp The specific parameter point around which the operational ratio is computed.
 * @return The ratio of operational parameter points to the total number of parameter points in the parameter space.
 */
template <typename Lyt, typename TT>
[[nodiscard]] double compute_operational_ratio_at_point(const Lyt& lyt, const std::vector<TT>& spec,
                                                        const compute_operational_ratio_at_point_params& params,
                                                        const parameter_point&                           pp) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(kitty::is_truth_table<TT>::value, "TT is not a truth table");

    operational_domain_stats stats{};

    detail::operational_domain_impl<Lyt, TT, operational_domain<parameter_point, operational_status>> p{
        lyt, spec, params.op_domain_params, stats};

    const auto op_domain = p.flood_fill(0, pp);

    // calculate the ratio of operational parameter pairs to the total number of parameter pairs
    return static_cast<double>(stats.num_operational_parameter_combinations) /
           static_cast<double>(stats.total_number_of_parameter_points);
}

}  // namespace fiction

#endif  // FICTION_COMPUTE_OPERATIONAL_RATIO_AT_POINT_HPP
