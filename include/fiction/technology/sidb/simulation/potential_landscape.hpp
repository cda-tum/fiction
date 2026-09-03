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
 * @brief The static electrostatics of an SiDB layout under one set of physical parameters.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/model/physical_constants.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation
{

/**
 * The bounds a local potential is tested against to decide whether a charge state is population stable.
 */
enum class charge_transition_threshold_bounds : uint8_t
{
    /**
     * The upper bound check against `mu_minus` to validate DB-.
     */
    NEGATIVE_UPPER_BOUND = 0,
    /**
     * The lower bound check against `mu_plus` to validate DB+.
     */
    POSITIVE_LOWER_BOUND = 1,
    /**
     * The lower bound check against `mu_minus` to validate DB0.
     */
    NEUTRAL_LOWER_BOUND = 2,
    /**
     * The upper bound check against `mu_plus` to validate DB0.
     */
    NEUTRAL_UPPER_BOUND = 3
};

/**
 * The static electrostatics of an SiDB layout under one set of physical parameters: the screened Coulomb potential
 * every SiDB exerts on every other, the potential the charged surface defects and the external sources exert on every
 * SiDB, and the resulting charge transition thresholds. Nothing in it depends on the charge states, so one landscape
 * serves every charge distribution of the layout and can be shared read-only by worker threads.
 *
 * Distances are stored in nm, potentials in V, energies in eV.
 */
class potential_landscape
{
  public:
    /**
     * Builds the landscape of a layout.
     *
     * @param lyt Layout; its charged surface defects enter the landscape.
     * @param params Physical parameters.
     * @param local_external_potential External potential per site (unit: V). A site holding a defect shifts the
     * potential at that defect; a site holding an SiDB shifts the potential at that SiDB.
     * @param global_external_potential External potential applied to every SiDB (unit: V).
     */
    explicit potential_landscape(const layout&                       lyt,
                                 const model::simulation_parameters& params = model::simulation_parameters{},
                                 const std::unordered_map<lattice_site, double>& local_external_potential  = {},
                                 const double                                    global_external_potential = 0.0) :
            lyt_{lyt},
            params_{params},
            sites_{std::make_shared<const std::vector<lattice_site>>(lyt.sidbs())},
            n_{sites_->size()},
            distances_(n_ * n_, 0.0),
            potentials_(n_ * n_, 0.0),
            local_ext_pot_(n_, 0.0),
            local_pot_caused_by_defects_(n_, 0.0),
            thresholds_(n_)
    {
        assert(params_.lambda_tf > 0.0 && "lambda_tf has to be > 0.0");

        for (std::size_t i = 0; i < n_; ++i)
        {
            for (std::size_t j = 0; j < n_; ++j)
            {
                const auto d = lyt_.get_lattice().nm_distance((*sites_)[i], (*sites_)[j]);

                distances_[(i * n_) + j]  = d;
                potentials_[(i * n_) + j] = chargeless_potential_at_distance(d);
            }
        }

        for (const auto& [s, d] : lyt_.defects())
        {
            if (model::is_charged_defect_type(d) && !lyt_.index_of(s).has_value())
            {
                defects_.emplace_back(s, d);
            }
        }

        defect_sidb_potentials_.resize(defects_.size() * n_, 0.0);
        defect_defect_potentials_.resize(defects_.size() * defects_.size(), 0.0);
        local_ext_pot_at_defect_.resize(defects_.size(), 0.0);

        for (std::size_t d = 0; d < defects_.size(); ++d)
        {
            const auto& [ds, defect] = defects_[d];

            for (std::size_t i = 0; i < n_; ++i)
            {
                const auto dist = lyt_.get_lattice().nm_distance((*sites_)[i], ds);

                local_pot_caused_by_defects_[i] +=
                    chargeless_potential_of_defect_at_distance(dist, defect) * static_cast<double>(defect.charge);

                // an SiDB acts on a defect like a DB-type defect with the surface's own parameters
                defect_sidb_potentials_[(d * n_) + i] = chargeless_potential_of_defect_at_distance(
                    dist, model::defect{model::defect_type::DB, 0, params_.epsilon_r, params_.lambda_tf});
            }

            for (std::size_t e = 0; e < defects_.size(); ++e)
            {
                defect_defect_potentials_[(d * defects_.size()) + e] = chargeless_potential_of_defect_at_distance(
                    lyt_.get_lattice().nm_distance(ds, defects_[e].first), defects_[e].second);
            }
        }

        for (const auto& [s, pot] : local_external_potential)
        {
            if (const auto i = lyt_.index_of(s); i.has_value())
            {
                local_ext_pot_[*i] += pot;
            }
            else
            {
                for (std::size_t d = 0; d < defects_.size(); ++d)
                {
                    if (defects_[d].first == s)
                    {
                        local_ext_pot_at_defect_[d] += pot;
                    }
                }
            }
        }

        for (auto& pot : local_ext_pot_)
        {
            pot += global_external_potential;
        }

        for (std::size_t i = 0; i < n_; ++i)
        {
            thresholds_[i] = {
                local_ext_pot_[i] - params_.mu_minus + utils::math::ERROR_MARGIN,   // DB- (UB)
                local_ext_pot_[i] - params_.mu_plus() - utils::math::ERROR_MARGIN,  // DB+ (LB)
                local_ext_pot_[i] - params_.mu_minus - utils::math::ERROR_MARGIN,   // DB0 (LB)
                local_ext_pot_[i] - params_.mu_plus() + utils::math::ERROR_MARGIN,  // DB0 (UB)
            };
        }
    }
    /**
     * The layout the landscape describes.
     *
     * @return The layout.
     */
    [[nodiscard]] const layout& get_layout() const noexcept
    {
        return lyt_;
    }
    /**
     * The physical parameters.
     *
     * @return The parameters.
     */
    [[nodiscard]] const model::simulation_parameters& params() const noexcept
    {
        return params_;
    }
    /**
     * Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] std::size_t num_sidbs() const noexcept
    {
        return n_;
    }
    /**
     * The SiDB sites in raster order, shared with the charge distributions built over this landscape.
     *
     * @return The shared site list.
     */
    [[nodiscard]] const charge_distribution::site_list& sites() const noexcept
    {
        return sites_;
    }
    /**
     * The charged surface defects that enter the landscape.
     *
     * @return Site-defect pairs.
     */
    [[nodiscard]] const std::vector<std::pair<lattice_site, model::defect>>& defects() const noexcept
    {
        return defects_;
    }
    /**
     * Distance between two SiDBs.
     *
     * @param i Index of the first SiDB.
     * @param j Index of the second SiDB.
     * @return Distance (unit: nm).
     */
    [[nodiscard]] double nm_distance(const std::size_t i, const std::size_t j) const noexcept
    {
        return distances_[(i * n_) + j];
    }
    /**
     * The chargeless potential one SiDB exerts on another, i.e., the potential of a unit charge at the distance of
     * the two.
     *
     * @param i Index of the SiDB the potential acts on.
     * @param j Index of the SiDB that exerts it.
     * @return Chargeless potential (unit: V).
     */
    [[nodiscard]] double chargeless_potential(const std::size_t i, const std::size_t j) const noexcept
    {
        return potentials_[(i * n_) + j];
    }
    /**
     * The external potential at an SiDB: its local external potential plus the global one.
     *
     * @param i Index of the SiDB.
     * @return External potential (unit: V).
     */
    [[nodiscard]] double local_external_potential(const std::size_t i) const noexcept
    {
        return local_ext_pot_[i];
    }
    /**
     * The potential the charged surface defects exert on an SiDB.
     *
     * @param i Index of the SiDB.
     * @return Defect potential (unit: V).
     */
    [[nodiscard]] double local_potential_caused_by_defects(const std::size_t i) const noexcept
    {
        return local_pot_caused_by_defects_[i];
    }
    /**
     * The charge transition thresholds of an SiDB, indexed by `charge_transition_threshold_bounds`.
     *
     * @param i Index of the SiDB.
     * @return The four thresholds (unit: V).
     */
    [[nodiscard]] const std::array<double, 4>&
    effective_charge_transition_thresholds(const std::size_t i) const noexcept
    {
        return thresholds_[i];
    }
    /**
     * The chargeless potential of a unit charge at a distance under the landscape's parameters.
     *
     * @param distance Distance (unit: nm).
     * @return Chargeless potential (unit: V); 0 at distance 0.
     */
    [[nodiscard]] double chargeless_potential_at_distance(const double distance) const noexcept
    {
        if (distance == 0.0)
        {
            return 0.0;
        }

        return (params_.k() / (distance * 1E-9) * std::exp(-distance / params_.lambda_tf) * model::ELEMENTARY_CHARGE);
    }
    /**
     * The chargeless potential a defect exerts at a distance, screened by the defect's own permittivity and screening
     * length.
     *
     * @param distance Distance (unit: nm).
     * @param defect Defect.
     * @return Chargeless potential (unit: V); 0 at distance 0.
     */
    [[nodiscard]] double chargeless_potential_of_defect_at_distance(const double         distance,
                                                                    const model::defect& defect) const noexcept
    {
        if (distance == 0.0)
        {
            return 0.0;
        }

        return params_.k() * params_.epsilon_r / defect.epsilon_r / (distance * 1e-9) *
               std::exp(-distance / defect.lambda_tf) * model::ELEMENTARY_CHARGE;
    }
    /**
     * The local internal potentials of a charge distribution: at every SiDB, the potential of the charged SiDBs plus
     * the potential of the charged defects. O(N²).
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @return Local internal potential per SiDB (unit: V).
     */
    [[nodiscard]] std::vector<double> local_internal_potentials(const charge_distribution& cd) const
    {
        std::vector<double> pot{local_pot_caused_by_defects_};

        for (std::size_t i = 0; i < n_; ++i)
        {
            double collect = 0.0;

            for (std::size_t j = 0; j < n_; ++j)
            {
                collect += potentials_[(i * n_) + j] *
                           static_cast<double>(model::charge_state_to_sign(cd.get_charge_state_by_index(j)));
            }

            pot[i] += collect;
        }

        return pot;
    }
    /**
     * The local potentials of a charge distribution: internal plus external potential per SiDB. O(N²).
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @return Local potential per SiDB (unit: V).
     */
    [[nodiscard]] std::vector<double> local_potentials(const charge_distribution& cd) const
    {
        auto pot = local_internal_potentials(cd);

        for (std::size_t i = 0; i < n_; ++i)
        {
            pot[i] += local_ext_pot_[i];
        }

        return pot;
    }
    /**
     * The electrostatic potential energy of a charge distribution given its local internal potentials, i.e., the
     * energy of the SiDB charges in the potential of the other SiDBs, the defects, and the external sources, plus the
     * energy of the defect charges. O(N + D·N) for D charged defects.
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @param local_internal_potential Local internal potential per SiDB, as `local_internal_potentials` computes it.
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy(const charge_distribution& cd,
                                const std::vector<double>& local_internal_potential) const noexcept
    {
        double collect     = 0.0;
        double collect_ext = 0.0;

        for (std::size_t i = 0; i < n_; ++i)
        {
            const auto q = static_cast<double>(model::charge_state_to_sign(cd.get_charge_state_by_index(i)));

            collect_ext += local_ext_pot_[i] * q;
            collect += local_internal_potential[i] * q;
        }

        for (std::size_t d = 0; d < defects_.size(); ++d)
        {
            const auto q = static_cast<double>(defects_[d].second.charge);

            double pot_at_defect = 0.0;

            for (std::size_t e = 0; e < defects_.size(); ++e)
            {
                pot_at_defect += defect_defect_potentials_[(d * defects_.size()) + e] *
                                 static_cast<double>(defects_[e].second.charge);
            }
            for (std::size_t i = 0; i < n_; ++i)
            {
                pot_at_defect += defect_sidb_potentials_[(d * n_) + i] *
                                 static_cast<double>(model::charge_state_to_sign(cd.get_charge_state_by_index(i)));
            }

            collect_ext += local_ext_pot_at_defect_[d] * q;
            collect += pot_at_defect * q;
        }

        return collect_ext + (0.5 * collect);
    }
    /**
     * The electrostatic potential energy of a charge distribution. O(N²).
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy(const charge_distribution& cd) const
    {
        return energy(cd, local_internal_potentials(cd));
    }
    /**
     * Whether every SiDB's charge state is population stable under its local potential: a negative SiDB below the
     * (0/-) transition, a positive one above the (+/0) transition, a neutral one in between.
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @param local_internal_potential Local internal potential per SiDB.
     * @return `true` iff the population stability holds for every SiDB.
     */
    [[nodiscard]] bool is_population_stable(const charge_distribution& cd,
                                            const std::vector<double>& local_internal_potential) const noexcept
    {
        for (std::size_t i = 0; i < n_; ++i)
        {
            const auto  v = -local_internal_potential[i];
            const auto& t = thresholds_[i];

            const bool valid =
                (cd.get_charge_state_by_index(i) == model::charge_state::NEGATIVE &&
                 v < t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND)]) ||
                (cd.get_charge_state_by_index(i) == model::charge_state::POSITIVE &&
                 v > t[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)]) ||
                (cd.get_charge_state_by_index(i) == model::charge_state::NEUTRAL &&
                 v > t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEUTRAL_LOWER_BOUND)] &&
                 v < t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEUTRAL_UPPER_BOUND)]);

            if (!valid)
            {
                return false;
            }
        }

        return true;
    }
    /**
     * Whether no charge hop between two SiDBs lowers the energy, i.e., the distribution is configuration stable.
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @param local_internal_potential Local internal potential per SiDB.
     * @return `true` iff no energetically favored hop exists.
     */
    [[nodiscard]] bool is_configuration_stable(const charge_distribution& cd,
                                               const std::vector<double>& local_internal_potential) const noexcept
    {
        for (std::size_t i = 0; i < n_; ++i)
        {
            if (cd.get_charge_state_by_index(i) == model::charge_state::POSITIVE)  // we do nothing with SiDB+
            {
                continue;
            }

            for (std::size_t j = 0; j < n_; ++j)
            {
                if (model::charge_state_to_sign(cd.get_charge_state_by_index(j)) <=
                    model::charge_state_to_sign(cd.get_charge_state_by_index(i)))
                {
                    continue;
                }

                // energy change when a charge hops from i to j
                const auto hop_del =
                    local_ext_pot_[i] - local_ext_pot_[j] +
                    (0.5 * (local_internal_potential[i] - local_internal_potential[j] - potentials_[(i * n_) + j]));

                if (hop_del < -utils::math::ERROR_MARGIN)
                {
                    return false;
                }
            }
        }

        return true;
    }
    /**
     * Whether a charge distribution is physically valid: population stable and configuration stable.
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @param local_internal_potential Local internal potential per SiDB.
     * @return `true` iff both stability criteria hold.
     */
    [[nodiscard]] bool is_physically_valid(const charge_distribution& cd,
                                           const std::vector<double>& local_internal_potential) const noexcept
    {
        return is_population_stable(cd, local_internal_potential) &&
               is_configuration_stable(cd, local_internal_potential);
    }
    /**
     * Whether a charge distribution is physically valid. O(N²).
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @return `true` iff both stability criteria hold.
     */
    [[nodiscard]] bool is_physically_valid(const charge_distribution& cd) const
    {
        return is_physically_valid(cd, local_internal_potentials(cd));
    }
    /**
     * Evaluates a charge distribution: returns a copy with its energy set. O(N²).
     *
     * @param cd Charge distribution over this landscape's SiDBs.
     * @return `cd` with its energy assigned.
     */
    [[nodiscard]] charge_distribution evaluate(charge_distribution cd) const
    {
        cd.assign_energy(energy(cd));

        return cd;
    }

  private:
    /**
     * The layout.
     */
    layout lyt_;
    /**
     * Physical parameters.
     */
    model::simulation_parameters params_;
    /**
     * SiDB sites in raster order.
     */
    charge_distribution::site_list sites_;
    /**
     * Number of SiDBs.
     */
    std::size_t n_;
    /**
     * Distances between SiDBs, row-major N×N (unit: nm).
     */
    std::vector<double> distances_;
    /**
     * Chargeless potentials between SiDBs, row-major N×N (unit: V).
     */
    std::vector<double> potentials_;
    /**
     * External potential per SiDB (unit: V).
     */
    std::vector<double> local_ext_pot_;
    /**
     * Potential of the charged defects per SiDB (unit: V).
     */
    std::vector<double> local_pot_caused_by_defects_;
    /**
     * Charge transition thresholds per SiDB (unit: V).
     */
    std::vector<std::array<double, 4>> thresholds_;
    /**
     * The charged defects with their sites.
     */
    std::vector<std::pair<lattice_site, model::defect>> defects_{};
    /**
     * Chargeless potential every SiDB exerts on every defect, row-major D×N (unit: V).
     */
    std::vector<double> defect_sidb_potentials_{};
    /**
     * Chargeless potential every defect exerts on every defect, row-major D×D (unit: V).
     */
    std::vector<double> defect_defect_potentials_{};
    /**
     * External potential per defect (unit: V).
     */
    std::vector<double> local_ext_pot_at_defect_{};
};

}  // namespace fiction::sidb::simulation
