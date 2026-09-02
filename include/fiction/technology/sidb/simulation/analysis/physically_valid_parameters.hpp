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
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/technology/sidb/simulation/domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/types.hpp>

#include <cstdint>

namespace fiction::sidb::simulation::analysis
{

/**
 * This function computes the physical parameters necessary for ensuring the physical validity of a given charge
 * distribution and determines the corresponding excited state number. The ground state is denoted by zero, with each
 * subsequent excited state incrementally numbered.
 *
 * This function is designed to derive the physical parameters from charge distribution measurements of SiDB layouts,
 * often acquired through Atomic Force Microscopy (AFM). Given a specific charge distribution, the function typically
 * yields several physically valid parameters.
 *
 * As more SiDB layouts with corresponding charge distributions are recorded, the number of physically valid
 * parameters for all layouts decreases. Consequently, this enables a more precise determination of the physical
 * parameters present on the surface.
 *
 * @tparam Lyt The charge distribution surface type.
 * @param cds The charge distribution surface for which physical parameters are to be determined.
 * @param params Operational domain parameters.
 * @return Physically valid parameters with the corresponding excited state number of the given charge distribution
 * surface for each parameter point.
 */
template <typename Lyt>
[[nodiscard]] sidb::simulation::domain<sidb::simulation::logic::parameter_point, uint64_t>
physically_valid_parameters(Lyt& cds, const sidb::simulation::logic::operational_domain_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(is_charge_distribution_surface_v<Lyt>, "Lyt is not a charge distribution surface");

    sidb::simulation::logic::operational_domain_stats st{};

    fiction::sidb::simulation::logic::detail::operational_domain_impl<Lyt, tt,
                                                                      sidb::simulation::logic::operational_domain>
        p{cds, params, st};

    const auto result = p.grid_search_for_physically_valid_parameters(cds);

    return result;
}

}  // namespace fiction::sidb::simulation::analysis
