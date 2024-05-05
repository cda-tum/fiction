//
// Created by Jan Drewniok on 03.05.24.
//

#ifndef FICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP
#define FICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/types.hpp>

namespace fiction
{

/**
 * This function computes the parameters necessary for ensuring the physical validity of a given charge distribution and
 * determines the corresponding excited state number. The ground state is denoted by zero, with each subsequent excited
 * state incrementally numbered.
 *
 * @tparam Lyt The charge distribution surface type.
 * @param cds The charge distribution surface for which physical parameters are to be determined.
 * @param params Operational domain parameters.
 * @return Physically valid parameters with the corresponding excited state number of the given cds for each parameter
 * point.
 */
template <typename Lyt>
[[nodiscard]] auto determine_physically_valid_parameters(Lyt&                             cds,
                                                         const operational_domain_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_charge_distribution_surface_v<Lyt>, "Lyt is not a charge distribution surface");

    operational_domain_stats                                                                          st{};
    detail::operational_domain_impl<Lyt, tt, operational_domain<parameter_point, operational_status>> p{cds, params,
                                                                                                        st};

    const auto result = p.grid_search_for_physically_valid_parameters(cds);

    return result;
}

}  // namespace fiction

#endif  // FICTION_DETERMINE_PHYSICALLY_VALID_PARAMETERS_HPP
