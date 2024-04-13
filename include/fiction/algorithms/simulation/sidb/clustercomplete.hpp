//
// Created by Willem Lambooy on 29.02.2024.
//

#ifndef FICTION_CLUSTERCOMPLETE_HPP
#define FICTION_CLUSTERCOMPLETE_HPP

#if (FICTION_ALGLIB_ENABLED)

#include "fiction/algorithms/simulation/sidb/ground_state_space.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_cluster_hierarchy.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * The struct containing the parameters both passed on to pre-simulator Ground State Space, and used during simulation.
 */
template <typename Lyt>
struct clustercomplete_params
{
    /**
     * Physical simulation parameters.
     */
    sidb_simulation_parameters physical_parameters{};
    /**
     * Local external electrostatic potentials (e.g locally applied electrodes).
     */
    std::unordered_map<cell<Lyt>, double> local_external_potential = {};
    /**
     * Global external electrostatic potential. Value is applied on each cell in the layout.
     */
    double global_potential = 0;
    /**
     * This specifies the maximum cluster size for which Ground State Space will solve an NP-complete sub-problem
     * exhaustively. The sets of SiDBs that witness local population stability for each respective charge state may be
     * partitioned into disjoint sets such that the number of required witnesses for each respective charge state is
     * satisfied. If no such partition exists, the multiset charge configuration associated with the requirements may be
     * rejected.
     */
    uint64_t validity_witness_partitioning_max_cluster_size_gss = 6;
    /**
     * Number of threads to make available to ClusterComplete for the simulation stage.
     */
    uint64_t available_threads = std::thread::hardware_concurrency();
    /**
     * Report the *Ground State Space* statistics to standard output.
     */
    bool report_gss_stats = false;
};

namespace detail
{

template <typename Lyt>
class clustercomplete_impl
{
  public:
    explicit clustercomplete_impl(const Lyt& lyt, const clustercomplete_params<Lyt>& params) noexcept :
            charge_layout{initialize_charge_layout(lyt, params)},
            real_placed_defects{charge_layout.get_defects()},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - params.physical_parameters.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - params.physical_parameters.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - params.physical_parameters.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - params.physical_parameters.mu_plus()},
            num_threads{params.available_threads}
    {}

    sidb_simulation_result<Lyt> run(const clustercomplete_params<Lyt>& params) noexcept
    {
        res.physical_parameters = charge_layout.get_phys_params();
        res.algorithm_name      = "ClusterComplete";
        res.additional_simulation_parameters.emplace("global_potential", params.global_potential);
        res.additional_simulation_parameters.emplace("validity_witness_partitioning_limit",
                                                     params.validity_witness_partitioning_max_cluster_size_gss);

        const ground_state_space_stats& gss_stats = fiction::ground_state_space(
            charge_layout, params.validity_witness_partitioning_max_cluster_size_gss, charge_layout.get_phys_params());

        if (!gss_stats.top_cluster)
        {
            return res;
        }

        if (params.report_gss_stats)
        {
            gss_stats.report();
        }

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            if (!gss_stats.top_cluster->charge_space.empty())
            {
                collect_physically_valid_charge_distributions(gss_stats.top_cluster);
            }
        }

        res.simulation_runtime = time_counter + gss_stats.runtime;

        return res;
    }

  private:
    static charge_distribution_surface<Lyt> initialize_charge_layout(const Lyt&                         lyt,
                                                                     const clustercomplete_params<Lyt>& params) noexcept
    {
        charge_distribution_surface<Lyt> cds{lyt};
        cds.assign_physical_parameters(params.physical_parameters);

        // assign defects if applicable
        if constexpr (has_foreach_sidb_defect_v<Lyt>)
        {
            lyt.foreach_sidb_defect(
                [&](const auto& cd)
                {
                    const auto& [cell, defect] = cd;

                    if (defect.type != sidb_defect_type::NONE)
                    {
                        cds.add_sidb_defect_to_potential_landscape(cell, lyt.get_sidb_defect(cell));
                    }
                });
        }

        cds.assign_local_external_potential(params.local_external_potential);
        cds.assign_global_external_potential(params.global_potential);

        return cds;
    }

    [[nodiscard]] constexpr inline bool fail_onto_negative_charge(const double pot_bound) const noexcept
    {
        // V > e - mu-
        return pot_bound > mu_bounds_with_error.at(0);
    }

    [[nodiscard]] constexpr inline bool fail_onto_positive_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu+
        return pot_bound < mu_bounds_with_error.at(3);
    }

    [[nodiscard]] constexpr inline bool ub_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu-
        return pot_bound < mu_bounds_with_error.at(1);
    }

    [[nodiscard]] constexpr inline bool lb_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V > e - mu+
        return pot_bound > mu_bounds_with_error.at(2);
    }

    [[nodiscard]] bool perform_potential_bound_analysis(const sidb_cluster_state& cst) const noexcept
    {
        uint64_t required_neg_count  = cst.proj_st.get_count<sidb_charge_state::NEGATIVE>(),
                 required_pos_count  = cst.proj_st.get_count<sidb_charge_state::POSITIVE>(),
                 required_neut_count = cst.proj_st.get_count<sidb_charge_state::NEUTRAL>();

        for (const uint64_t sidb_ix : cst.proj_st.cluster->sidbs)
        {
            const double recv_pot_lb = cst.get_pot_bound<bound_direction::LOWER>(sidb_ix);
            const double recv_pot_ub = cst.get_pot_bound<bound_direction::UPPER>(sidb_ix);

            if (required_neg_count != 0 && !fail_onto_negative_charge(recv_pot_lb))
            {
                --required_neg_count;
            }

            if (required_pos_count != 0 && !fail_onto_positive_charge(recv_pot_ub))
            {
                --required_pos_count;
            }

            if (required_neut_count != 0 && !ub_fail_onto_neutral_charge(recv_pot_ub) &&
                !lb_fail_onto_neutral_charge(recv_pot_lb))
            {
                --required_neut_count;
            }
        }

        return required_neg_count == 0 && required_pos_count == 0 && required_neut_count == 0;
    }

    [[nodiscard]] bool
    meets_population_stability_criterion(const sidb_clustering_state& clustering_state) const noexcept
    {
        for (const sidb_cluster_state_ptr& cst : clustering_state)
        {
            if (!perform_potential_bound_analysis(*cst))
            {
                return false;
            }
        }

        return true;
    }

    void add_if_configuration_stability_is_met(const sidb_clustering_state& clustering_state) noexcept
    {
        charge_distribution_surface charge_layout_copy{charge_layout};

        for (const sidb_cluster_state_ptr& cst : clustering_state)
        {
            const uint64_t sidb_ix = get_singleton_sidb_ix(cst->proj_st.cluster);
            charge_layout_copy.assign_charge_state_by_cell_index(
                sidb_ix, singleton_multiset_conf_to_charge_state(cst->proj_st.multiset_conf), false);

            charge_layout_copy.assign_local_potential_by_index(sidb_ix,
                                                               -cst->get_pot_bound<bound_direction::LOWER>(sidb_ix));
        }

        charge_layout_copy.recompute_system_energy();

        if (!charge_layout_copy.is_configuration_stable())
        {
            return;
        }

        charge_layout_copy.declare_physically_valid();

        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            for (const auto& [cell, defect] : real_placed_defects)
            {
                charge_layout_copy.assign_sidb_defect(cell, defect);
            }
        }

        {
            const std::lock_guard lock{res_mutex};
            res.charge_distributions.emplace_back(charge_layout_copy);
        }
    }

    template <bound_direction bound>
    static constexpr inline double get_projector_state_bound_pot(const sidb_cluster_projector_state& pst,
                                                                 const uint64_t                      sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf).v;
    }

    enum class potential_bound_update_operation
    {
        ADD,
        SUBTRACT
    };

    template <potential_bound_update_operation op>
    static constexpr inline void update_cluster_state(sidb_cluster_state& recv_cst, const uint64_t sidb_ix,
                                                      const sidb_cluster_projector_state& pst) noexcept
    {
        if constexpr (op == potential_bound_update_operation::ADD)
        {
            recv_cst.update_pot_bounds(sidb_ix, get_projector_state_bound_pot<bound_direction::LOWER>(pst, sidb_ix),
                                       get_projector_state_bound_pot<bound_direction::UPPER>(pst, sidb_ix));
        }
        else if constexpr (op == potential_bound_update_operation::SUBTRACT)
        {
            recv_cst.update_pot_bounds(sidb_ix, -get_projector_state_bound_pot<bound_direction::LOWER>(pst, sidb_ix),
                                       -get_projector_state_bound_pot<bound_direction::UPPER>(pst, sidb_ix));
        }
    }

    static void apply_inter_cluster_potential(const sidb_cluster_state&       parent_cst,
                                              sidb_cluster_state_composition& parent_composition,
                                              sidb_clustering_state&          clustering_state) noexcept
    {
        for (sidb_cluster_state& child_cst : parent_composition)
        {
            for (const uint64_t sidb_ix : child_cst.proj_st.cluster->sidbs)
            {
                for (const sidb_cluster_state_ptr& cst : clustering_state)
                {
                    update_cluster_state<potential_bound_update_operation::ADD>(child_cst, sidb_ix, cst->proj_st);
                }
            }
        }

        for (sidb_cluster_state_ptr& cst : clustering_state)
        {
            for (const uint64_t sidb_ix : cst->proj_st.cluster->sidbs)
            {
                update_cluster_state<potential_bound_update_operation::SUBTRACT>(*cst, sidb_ix, parent_cst.proj_st);

                for (sidb_cluster_state& child_cst : parent_composition)
                {
                    update_cluster_state<potential_bound_update_operation::ADD>(*cst, sidb_ix, child_cst.proj_st);
                }
            }
        }
    }

    static void undo_apply_inter_cluster_potential(const sidb_cluster_state&       parent_cst,
                                                   sidb_cluster_state_composition& parent_composition,
                                                   sidb_clustering_state&          clustering_state) noexcept
    {
        for (sidb_cluster_state_ptr& cst : clustering_state)
        {
            for (const uint64_t sidb_ix : cst->proj_st.cluster->sidbs)
            {
                for (sidb_cluster_state& child_cst : parent_composition)
                {
                    update_cluster_state<potential_bound_update_operation::SUBTRACT>(*cst, sidb_ix, child_cst.proj_st);
                }

                update_cluster_state<potential_bound_update_operation::ADD>(*cst, sidb_ix, parent_cst.proj_st);
            }
        }
    }

    void add_physically_valid_charge_configurations(sidb_clustering_state& clustering_state) noexcept
    {
        if (!meets_population_stability_criterion(clustering_state))
        {
            return;
        }

        // check if all clusters are singletons
        if (std::all_of(clustering_state.cbegin(), clustering_state.cend(),
                        [](const sidb_cluster_state_ptr& cst) { return cst->proj_st.cluster->children.empty(); }))
        {
            add_if_configuration_stability_is_met(clustering_state);
            return;
        }

        // max_cst <- find the cluster of maximum size
        uint64_t max_cluster_size = clustering_state.front()->proj_st.cluster->size();
        uint64_t max_cst_ix       = 0;
        for (uint64_t ix = 1; ix < clustering_state.size(); ++ix)
        {
            if (const uint64_t cluster_size = clustering_state.at(ix)->proj_st.cluster->size();
                cluster_size > max_cluster_size)
            {
                max_cluster_size = cluster_size;
                max_cst_ix       = ix;
            }
        }

        // swap with last
        std::swap(clustering_state[max_cst_ix], clustering_state.back());

        // move out
        sidb_cluster_state_ptr max_cst = std::move(clustering_state.back());

        // pop
        clustering_state.pop_back();

        // for all compositions of max_cst
        for (sidb_cluster_state_composition& max_cst_composition : get_projector_state_compositions(max_cst->proj_st))
        {
            apply_inter_cluster_potential(*max_cst, max_cst_composition, clustering_state);

            for (sidb_cluster_state& sub_cst : max_cst_composition)
            {
                // move in
                clustering_state.emplace_back(std::make_unique<sidb_cluster_state>(std::move(sub_cst)));
            }

            // recurse
            add_physically_valid_charge_configurations(clustering_state);

            for (uint64_t i = 0; i < max_cst_composition.size(); ++i)
            {
                // handled
                clustering_state.pop_back();
            }

            undo_apply_inter_cluster_potential(*max_cst, max_cst_composition, clustering_state);
        }

        // move back
        clustering_state.emplace_back(std::move(max_cst));

        // swap back
        std::swap(clustering_state.back(), clustering_state[max_cst_ix]);
    }

    void collect_physically_valid_charge_distributions(const sidb_cluster_ptr& top_cluster) noexcept
    {
        const uint64_t top_level_multisets = top_cluster->charge_space.size();

        const uint64_t num_threads_to_use = std::min(std::max(num_threads, uint64_t{1}), top_level_multisets);

        // define the top cluster charge space ranges per thread
        std::vector<std::pair<uint64_t, uint64_t>> ranges;
        ranges.reserve(num_threads_to_use);

        const uint64_t chunk_size = std::max(top_level_multisets / num_threads_to_use, uint64_t{1});
        uint64_t       start      = 0;
        uint64_t       end        = chunk_size - 1;

        for (uint64_t i = 0; i < num_threads_to_use; ++i)
        {
            ranges.emplace_back(start, end);
            start = end + 1;
            end   = i == num_threads_to_use - 2 ? top_level_multisets - 1 : start + chunk_size - 1;
        }

        std::vector<std::thread> threads_vec{};
        threads_vec.reserve(num_threads_to_use);

        for (const std::pair<uint64_t, uint64_t>& range : ranges)
        {
            threads_vec.emplace_back(
                [&]
                {
                    // iterate over all multiset charge configurations in the assigned range
                    for (uint64_t ix = range.first; ix <= range.second; ++ix)
                    {
                        // iterate over all cluster charge assignments in the multiset charge configuration
                        for (sidb_cluster_state_composition& composition :
                             std::next(top_cluster->charge_space.cbegin(), static_cast<int64_t>(ix))->compositions)
                        {
                            sidb_clustering_state clustering_state{};
                            clustering_state.reserve(charge_layout.num_cells());

                            for (sidb_cluster_state& cst : composition)
                            {
                                clustering_state.emplace_back(std::make_unique<sidb_cluster_state>(std::move(cst)));
                            }

                            add_physically_valid_charge_configurations(clustering_state);
                        }
                    }
                });
        }

        for (std::thread& t : threads_vec)
        {
            t.join();
        }
    }

    sidb_simulation_result<Lyt> res{};
    std::mutex                  res_mutex{};

    const charge_distribution_surface<Lyt>                          charge_layout;
    const std::unordered_map<typename Lyt::cell, const sidb_defect> real_placed_defects;

    const std::array<double, 4> mu_bounds_with_error;
    const uint64_t              num_threads;
};

}  // namespace detail

/**
 * *ClusterComplete* is an instantiation of a general solution to exhaustive state assignment searching for which all
 * local predicates hold, given respective local evaluations that may be aggregated from individual inter-variable
 * interactions. Applied to the problem of exact physical simulation of SiDBs, it is able to efficiently consider
 * positive charges that are rare to occur, but drastically blow up exact simulation runtimes when hierarchical pruning
 * methods are not applied. In fact, the exponential growth in problem complexity for added SiDBs is tamed by
 * *ClusterComplete*, as SiDB layouts to simulate in practise amount to a high pruning efficacy, resulting in a
 * layout-dependent reduction of the simulation base. This amounts to an effective simulation base in the real number
 * range \f$[1,b]\f$, where \f$b\in\{2,3\}\f$ is the given simulation base.
 *
 * The part of the *ClusterComplete* algorithm that is implemented in this file is the destructive phase of the
 * procedure that employs the duality of construction and destruction, folding and unfolding. The phase preceding it is
 * the key ingredient to the achieved efficiency: the *Ground State Space* algorithm, which constructs a minimised
 * hierarchical search space of charge configurations that adhere to the critical population stability criterion. In
 * particular, it generalizes physically informed space pruning that contributes to the capabilities of the *QuickExact*
 * simulator, now applying to all charge states equally, and, most importantly, it lifts the associated potential
 * equations to higher order, allowing us to reason over potential bounds in a cluster hierarchy.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to simulate.
 * @param params Parameter required for both the invocation of *Ground State Space*, and the simulation following.
 * @return Simulation results.
 */
template <typename Lyt>
[[nodiscard]] sidb_simulation_result<Lyt> clustercomplete(const Lyt&                         lyt,
                                                          const clustercomplete_params<Lyt>& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::clustercomplete_impl<Lyt> p{lyt, params};

    return p.run(params);
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CLUSTERCOMPLETE_HPP
