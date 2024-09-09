//
// Created by Jan Drewniok on 09.09.24.
//

#ifndef FICTION_COMPUTE_OPERATIONAL_TO_WHOLE_RATIO_AROUND_GIVEN_POINT_HPP
#define FICTION_COMPUTE_OPERATIONAL_TO_WHOLE_RATIO_AROUND_GIVEN_POINT_HPP

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>

namespace fiction
{

struct compute_operational_to_whole_ratio_around_given_point_params
{
    operational_domain_params operational_domain_params;
};

template <typename Lyt>
[[nodiscard]] double
compute_operational_to_whole_ratio_around_given_point(const Lyt& lyt, const compute_operational_to_whole_ratio_around_given_point_params& params, const parameter_point& pp) noexcept
{

    // calculate the total number of parameter points
    std::size_t total_number_of_parameter_points = 1;
    for (const auto& val : values)
    {
        total_number_of_parameter_points *= val.size();
    }

    // calculate the ratio of operational parameter pairs to the total number of parameter pairs
    stats.operational_to_total_ratio = static_cast<double>(stats.num_operational_parameter_combinations) /
                                       static_cast<double>(total_number_of_parameter_points);

    const auto op_domain = operational_domain_flood_fill(lyt, params.operational_domain_params, pp);
}

}  // namespace fiction

#endif  // FICTION_COMPUTE_OPERATIONAL_TO_WHOLE_RATIO_AROUND_GIVEN_POINT_HPP
