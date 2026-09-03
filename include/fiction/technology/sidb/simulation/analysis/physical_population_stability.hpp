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
 * @brief Potential margin each SiDB has before its charge state flips.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/potential_to_distance_conversion.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::analysis
{

/**
 * Possible types of charge transitions that can occur in an SiDB layout. These transitions represent
 * changes in the charge state of SiDBs, including transitions from neutral to negative, negative to
 * neutral, neutral to positive, and positive to neutral.
 */
enum class transition_type : uint8_t
{
    /**
     * SiDB is neutrally charged, but is closest to being negatively charged.
     */
    NEUTRAL_TO_NEGATIVE,
    /**
     * SiDB is negatively charged, but is closest to being neutrally charged.
     */
    NEGATIVE_TO_NEUTRAL,
    /**
     * SiDB is neutrally charged, but is closest to being positively charged.
     */
    NEUTRAL_TO_POSITIVE,
    /**
     * SiDB is positively charged, but is closest to being neutrally charged.
     */
    POSITIVE_TO_NEUTRAL,
};

/**
 * Population stability of one physically valid charge distribution: for every transition type, the SiDB that is
 * closest to that transition and the potential change it would take, plus the distance an SiDB would have to be
 * placed at to induce that potential change.
 */
struct population_stability_information
{
    /**
     * SiDB with the minimum potential difference to any charge transition threshold.
     */
    lattice_site critical_cell{};
    /**
     * For each transition type, the SiDB closest to it and the required potential difference (unit: V).
     */
    std::unordered_map<transition_type, std::pair<lattice_site, double>> transition_potentials{};
    /**
     * For each transition type, the distance (unit: nm) an additional SiDB would have to be placed at to induce
     * the required potential difference.
     */
    std::unordered_map<transition_type, double> distance_corresponding_to_potential{};
    /**
     * Electrostatic potential energy of the charge distribution (unit: eV).
     */
    double system_energy{};
};

/**
 * Parameters of the population stability analysis.
 */
struct physical_population_stability_params
{
    /**
     * Physical parameters of the simulation.
     */
    model::simulation_parameters sim_params{};
    /**
     * Number of decimal places of the distance corresponding to a potential difference.
     */
    uint64_t precision_for_distance_corresponding_to_potential = 2;
};

namespace detail
{

/**
 * Implementation of the population stability analysis.
 */
class physical_population_stability_impl
{
  public:
    /**
     * Constructor.
     *
     * @param lyt The layout to analyze.
     * @param parameters Parameters.
     */
    physical_population_stability_impl(const layout& lyt, const physical_population_stability_params& parameters) :
            layout_{lyt},
            params_{parameters}
    {}
    /**
     * Simulates the layout with *QuickExact* and analyzes every physically valid charge distribution, ordered by
     * energy.
     *
     * @return The population stability information, one entry per distinct charge distribution.
     */
    [[nodiscard]] std::vector<population_stability_information> run() noexcept
    {
        const engines::quickexact_params quickexact_parameters{.sim_params = params_.sim_params};

        auto simulation_results = engines::quickexact(layout_, quickexact_parameters);

        const potential_landscape land{layout_, params_.sim_params};

        // ascending energy, distinct charge distributions only
        std::ranges::stable_sort(simulation_results.charge_distributions,
                                 [](const auto& lhs, const auto& rhs) { return lhs.energy() < rhs.energy(); });

        std::unordered_set<uint64_t> seen{};

        std::vector<population_stability_information> popstability_information{};
        popstability_information.reserve(simulation_results.charge_distributions.size());

        for (const auto& cd : simulation_results.charge_distributions)
        {
            if (!seen.insert(cd.charge_index(params_.sim_params.base)).second)
            {
                continue;
            }

            population_stability_information info{};

            for (const auto t : {transition_type::NEUTRAL_TO_NEGATIVE, transition_type::NEGATIVE_TO_NEUTRAL,
                                 transition_type::NEUTRAL_TO_POSITIVE, transition_type::POSITIVE_TO_NEUTRAL})
            {
                info.transition_potentials.insert({t, {lattice_site{}, std::numeric_limits<double>::infinity()}});
            }

            const auto local_potentials = land.local_potentials(cd);

            for (std::size_t i = 0; i < cd.size(); ++i)
            {
                const auto& c = cd.sites()[i];

                switch (cd.get_charge_state_by_index(i))
                {
                    case model::charge_state::NEGATIVE:
                    {
                        handle_negative_charges(local_potentials[i], c, info);
                        break;
                    }
                    case model::charge_state::NEUTRAL:
                    {
                        handle_neutral_charges(local_potentials[i], c, info);
                        break;
                    }
                    case model::charge_state::POSITIVE:
                    {
                        handle_positive_charges(local_potentials[i], c, info);
                        break;
                    }
                    case model::charge_state::NONE:
                    {
                        break;
                    }
                }
            }

            info.system_energy = cd.energy();

            auto minimum_potential_difference = std::numeric_limits<double>::infinity();

            for (const auto& [transition, cell_and_potential] : info.transition_potentials)
            {
                info.distance_corresponding_to_potential[transition] =
                    model::potential_to_distance_conversion(cell_and_potential.second, params_.sim_params,
                                                            params_.precision_for_distance_corresponding_to_potential);

                if (cell_and_potential.second < minimum_potential_difference)
                {
                    info.critical_cell           = cell_and_potential.first;
                    minimum_potential_difference = cell_and_potential.second;
                }
            }

            popstability_information.push_back(std::move(info));
        }

        return popstability_information;
    }

  private:
    /**
     * The layout to analyze.
     */
    const layout& layout_;
    /**
     * Parameters.
     */
    const physical_population_stability_params& params_;
    /**
     * Records the negative-to-neutral transition of a negatively charged SiDB if it is the closest so far.
     *
     * @param local_potential Local potential at the SiDB.
     * @param c The SiDB.
     * @param info The information to update.
     */
    void handle_negative_charges(const double local_potential, const lattice_site& c,
                                 population_stability_information& info) const noexcept
    {
        const auto required = std::abs(-local_potential + params_.sim_params.mu_minus);

        if (required < info.transition_potentials.at(transition_type::NEGATIVE_TO_NEUTRAL).second)
        {
            info.transition_potentials[transition_type::NEGATIVE_TO_NEUTRAL] = {c, required};
        }
    }
    /**
     * Records the neutral-to-negative and neutral-to-positive transitions of a neutral SiDB if they are the
     * closest so far.
     *
     * @param local_potential Local potential at the SiDB.
     * @param c The SiDB.
     * @param info The information to update.
     */
    void handle_neutral_charges(const double local_potential, const lattice_site& c,
                                population_stability_information& info) const noexcept
    {
        const auto to_negative = std::abs(-local_potential + params_.sim_params.mu_minus);
        const auto to_positive = std::abs(-local_potential + params_.sim_params.mu_plus());

        if (to_negative < to_positive &&
            to_negative < info.transition_potentials.at(transition_type::NEUTRAL_TO_NEGATIVE).second)
        {
            info.transition_potentials.at(transition_type::NEUTRAL_TO_NEGATIVE) = {c, to_negative};
        }

        if (to_positive < info.transition_potentials.at(transition_type::NEUTRAL_TO_POSITIVE).second)
        {
            info.transition_potentials[transition_type::NEUTRAL_TO_POSITIVE] = {c, to_positive};
        }
    }
    /**
     * Records the positive-to-neutral transition of a positively charged SiDB if it is the closest so far.
     *
     * @param local_potential Local potential at the SiDB.
     * @param c The SiDB.
     * @param info The information to update.
     */
    void handle_positive_charges(const double local_potential, const lattice_site& c,
                                 population_stability_information& info) const noexcept
    {
        const auto required = std::abs(-local_potential + params_.sim_params.mu_plus());

        if (required < info.transition_potentials.at(transition_type::POSITIVE_TO_NEUTRAL).second)
        {
            info.transition_potentials[transition_type::POSITIVE_TO_NEUTRAL] = {c, required};
        }
    }
};

}  // namespace detail

/**
 * Assesses the population stability of an SiDB layout: for every physically valid charge distribution (found with
 * *QuickExact*), it determines the SiDB closest to each charge transition and the potential change required for
 * it, in ascending order of the distributions' energies. The distance an SiDB would have to be placed at to cause
 * that potential change is reported as well.
 *
 * @param lyt The layout to analyze.
 * @param params Parameters.
 * @return The population stability information, one entry per distinct charge distribution.
 */
[[nodiscard]] inline std::vector<population_stability_information>
physical_population_stability(const layout& lyt, const physical_population_stability_params& params) noexcept
{
    detail::physical_population_stability_impl p{lyt, params};

    return p.run();
}

}  // namespace fiction::sidb::simulation::analysis
