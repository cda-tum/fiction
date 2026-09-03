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
 * @brief The unified result type every SiDB simulation algorithm returns.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/minimum_energy.hpp"
#include "fiction/technology/sidb/surfaces/charge_distribution_surface.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <algorithm>
#include <any>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace fiction::sidb::simulation
{

/**
 * The unified return type of every SiDB simulation algorithm: the name of the algorithm, its runtime, the layout it
 * simulated, the physically valid charge distributions it found over that layout, the physical parameters, and
 * optional algorithm-specific named parameters.
 *
 * The layout is stored once; each charge distribution holds one charge state per SiDB in the layout's raster order
 * plus its energy.
 */
struct result
{
    /**
     * Name of the algorithm used to determine the charge distributions.
     */
    std::string algorithm_name{};
    /**
     * Total simulation runtime in seconds.
     */
    std::chrono::duration<double> simulation_runtime{};
    /**
     * The simulated layout.
     */
    layout lyt{};
    /**
     * Charge distributions determined by the algorithm.
     */
    std::vector<charge_distribution> charge_distributions{};
    /**
     * Physical parameters used in the simulation.
     */
    model::simulation_parameters sim_params{};
    /**
     * Additional named simulation parameters. This is used to store algorithm-dependent parameters that are not part of
     * the `sidb::model::simulation_parameters` struct.
     *
     * The key of the map is the name of the parameter, the element is the value of the parameter.
     */
    std::unordered_map<std::string, std::any> additional_simulation_parameters{};
    /**
     * The charge state of an SiDB in one of the charge distributions.
     *
     * @param distribution Index into `charge_distributions`.
     * @param s Site of the SiDB.
     * @return The charge state, `charge_state::NONE` if `s` holds no SiDB.
     */
    [[nodiscard]] model::charge_state charge_state(const std::size_t distribution, const lattice_site& s) const noexcept
    {
        return charge_distributions[distribution].get_charge_state(s);
    }
    /**
     * The charge distributions of minimal energy. Distributions with identical charge states count once.
     *
     * @note If degenerate states exist in the simulation result, this function returns multiple ground states that all
     * possess the same energy.
     *
     * @return The charge distributions with the minimal energy.
     */
    [[nodiscard]] std::vector<charge_distribution> groundstates() const
    {
        std::vector<charge_distribution> ground{};

        if (charge_distributions.empty())
        {
            return ground;
        }

        const auto min_energy = analysis::minimum_energy(charge_distributions.cbegin(), charge_distributions.cend());

        for (const auto& cd : charge_distributions)
        {
            if (std::abs(cd.energy() - min_energy) < utils::math::ERROR_MARGIN &&
                std::ranges::none_of(ground, [&cd](const auto& g) { return g.same_charge_states(cd); }))
            {
                ground.push_back(cd);
            }
        }

        return ground;
    }
};

/**
 * The former result type over `charge_distribution_surface` copies, kept for the algorithms that still consume it.
 * New code uses `result`; `to_legacy_result` converts between the two.
 *
 * @tparam Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct legacy_result
{
    /**
     * Default constructor. It only exists to allow for the use of `static_assert` statements that restrict the type of
     * `Lyt`.
     */
    legacy_result() noexcept
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Name of the algorithm used to determine the charge distributions.
     */
    std::string algorithm_name{};
    /**
     * Total simulation runtime in seconds.
     */
    std::chrono::duration<double> simulation_runtime{};
    /**
     * Charge distributions determined by the algorithm.
     */
    std::vector<sidb::surfaces::charge_distribution_surface<Lyt>> charge_distributions{};
    /**
     * Physical parameters used in the simulation.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * Additional named simulation parameters. This is used to store algorithm-dependent parameters that are not part of
     * the `sidb::model::simulation_parameters` struct.
     *
     * The key of the map is the name of the parameter, the element is the value of the parameter.
     */
    std::unordered_map<std::string, std::any> additional_simulation_parameters{};
    /**
     * This function computes the ground state of the charge distributions.
     *
     * @note If degenerate states exist in the simulation result, this function will return multiple ground states that
     * all possess the same system energy.
     *
     * @return A vector of charge distributions with the minimal energy.
     */
    [[nodiscard]] std::vector<sidb::surfaces::charge_distribution_surface<Lyt>> groundstates() const noexcept
    {
        std::vector<sidb::surfaces::charge_distribution_surface<Lyt>> groundstate_charge_distributions{};
        std::set<uint64_t>                                            charge_indices{};

        // Find all unique charge indices. This is done because simulation results can have multiple identical charge
        // distributions.
        for (auto& cds : charge_distributions)
        {
            cds.charge_distribution_to_index_general();
            charge_indices.insert(cds.get_charge_index_and_base().first);
        }

        // Find the minimum energy
        double min_energy = std::numeric_limits<double>::infinity();
        if (!charge_distributions.empty())
        {
            min_energy = analysis::minimum_energy(charge_distributions.cbegin(), charge_distributions.cend());
        }

        for (const auto charge_index : charge_indices)
        {
            const auto cds_it =
                std::ranges::find_if(charge_distributions,
                                     [&](const auto& cds)
                                     {
                                         return cds.get_charge_index_and_base().first == charge_index &&
                                                std::abs(cds.get_electrostatic_potential_energy() - min_energy) <
                                                    fiction::utils::math::ERROR_MARGIN;
                                     });

            if (cds_it != charge_distributions.cend())
            {
                groundstate_charge_distributions.push_back(*cds_it);
            }
        }

        return groundstate_charge_distributions;
    }
};

/**
 * Converts a result into the former representation over `charge_distribution_surface` copies of a cell-level layout,
 * for the algorithms that still consume it. Each distribution's charge states are assigned to the surface and its
 * potentials and energy recomputed.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param res Result to convert.
 * @param lyt The cell-level layout the result's layout was converted from.
 * @return The result over surfaces of `lyt`.
 */
template <typename Lyt>
[[nodiscard]] legacy_result<Lyt> to_legacy_result(const result& res, const Lyt& lyt)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    legacy_result<Lyt> legacy{};
    legacy.algorithm_name                   = res.algorithm_name;
    legacy.simulation_runtime               = res.simulation_runtime;
    legacy.sim_params                       = res.sim_params;
    legacy.additional_simulation_parameters = res.additional_simulation_parameters;
    legacy.charge_distributions.reserve(res.charge_distributions.size());

    // a layout that already is a charge distribution surface is wrapped as-is and reparameterized
    sidb::surfaces::charge_distribution_surface<Lyt> base = [&]()
    {
        if constexpr (is_charge_distribution_surface_v<Lyt>)
        {
            sidb::surfaces::charge_distribution_surface<Lyt> b{lyt};
            b.assign_physical_parameters(res.sim_params);

            // wrapping skips the surface's initialization, so the defects' electrostatic influence is added here
            if constexpr (is_sidb_defect_surface_v<Lyt>)
            {
                lyt.foreach_sidb_defect(
                    [&b](const auto& cd)
                    {
                        if (const auto& [cell, defect] = cd; defect.type != sidb::model::defect_type::NONE &&
                                                             sidb::model::is_charged_defect_type(defect))
                        {
                            b.add_sidb_defect_to_potential_landscape(cell, defect);
                        }
                    });
            }

            return b;
        }
        else
        {
            return sidb::surfaces::charge_distribution_surface<Lyt>{lyt, res.sim_params};
        }
    }();

    if (const auto it = res.additional_simulation_parameters.find("global_potential");
        it != res.additional_simulation_parameters.cend() && it->second.type() == typeid(double))
    {
        base.assign_global_external_potential(std::any_cast<double>(it->second));
    }

    for (const auto& cd : res.charge_distributions)
    {
        sidb::surfaces::charge_distribution_surface<Lyt> cds{base};

        for (std::size_t i = 0; i < cd.size(); ++i)
        {
            cds.assign_charge_state(to_cell<Lyt>(cd.sites()[i]), cd.get_charge_state_by_index(i),
                                    sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        }

        cds.update_after_charge_change();
        cds.charge_distribution_to_index_general();

        legacy.charge_distributions.push_back(std::move(cds));
    }

    return legacy;
}

}  // namespace fiction::sidb::simulation
