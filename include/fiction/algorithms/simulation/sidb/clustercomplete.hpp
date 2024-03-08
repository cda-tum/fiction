//
// Created by Willem Lambooy on 29/02/2024.
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
#include "fiction/utils/hash.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * The struct containing the parameters both passed on to pre-simulator Ground State Space, and used during simulation.
 */
struct clustercomplete_params
{
    /**
     * Physical simulation parameters.
     */
    sidb_simulation_parameters physical_parameters{};
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
    explicit clustercomplete_impl(const Lyt& lyt, const sidb_simulation_parameters& physical_parameters,
                                  const uint64_t available_threads) noexcept :
            charge_layout{initialize_charge_layout(lyt, physical_parameters)},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - physical_parameters.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - physical_parameters.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - physical_parameters.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - physical_parameters.mu_plus()},
            num_threads{available_threads}
    {}

    sidb_simulation_result<Lyt> run(const uint64_t validity_witness_partitioning_limit, const bool rep_stats) noexcept
    {
        res.physical_parameters = charge_layout.get_phys_params();
        res.algorithm_name      = "ClusterComplete";
        res.additional_simulation_parameters.emplace("validity_witness_partitioning_limit",
                                                     validity_witness_partitioning_limit);

        const ground_state_space_result& gss_res = fiction::ground_state_space(
            charge_layout, validity_witness_partitioning_limit, charge_layout.get_phys_params());

        if (!gss_res.top_cluster)
        {
            return res;
        }

        if (rep_stats)
        {
            report_ground_state_space_statistics(gss_res);
        }

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            if (!gss_res.top_cluster->charge_space.empty())
            {
                collect_physically_valid_charge_distributions(gss_res.top_cluster);
            }
        }

        res.simulation_runtime = time_counter + gss_res.runtime;

        return res;
    }

  private:
    static charge_distribution_surface<Lyt>
    initialize_charge_layout(const Lyt& lyt, const sidb_simulation_parameters& phys_params) noexcept
    {
        charge_distribution_surface<Lyt> cl{lyt};
        cl.assign_physical_parameters(phys_params);
        return cl;
    }

    static void report_ground_state_space_statistics(const ground_state_space_result& gss_res) noexcept
    {
        std::cout << "Pruned " << gss_res.pruned_top_level_multisets << " out of "
                  << gss_res.maximum_top_level_multisets << " top level multiset charge configurations\n";

        std::cout << "Ground State Space took ";

        if (const double gss_runtime = mockturtle::to_seconds(gss_res.runtime); gss_runtime > 1.0)
        {
            std::cout << gss_runtime << " seconds" << std::endl;
        }
        else
        {
            std::cout << (gss_runtime * 1000) << " milliseconds" << std::endl;
        }
    }

    constexpr inline bool fail_onto_negative_charge(const double pot_bound) const noexcept
    {
        // V > e - mu-
        return pot_bound > mu_bounds_with_error[0];
    }

    constexpr inline bool fail_onto_positive_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu+
        return pot_bound < mu_bounds_with_error[3];
    }

    constexpr inline bool ub_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu-
        return pot_bound < mu_bounds_with_error[1];
    }

    constexpr inline bool lb_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V > e - mu+
        return pot_bound > mu_bounds_with_error[2];
    }

    bool perform_potential_bound_analysis(const sidb_cluster_state& cst) const noexcept
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

    bool meets_population_stability_criterion(const sidb_clustering_state& clustering_state) const noexcept
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

        {
            const std::lock_guard lock{res_mutex};
            res.charge_distributions.emplace_back(charge_layout_copy);
        }
    }

    template <bound_direction bound>
    static constexpr inline double get_proj_state_bound_val(const sidb_cluster_projector_state& pst,
                                                            const uint64_t                      sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf).V;
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
            recv_cst.update_pot_bounds(sidb_ix, get_proj_state_bound_val<bound_direction::LOWER>(pst, sidb_ix),
                                       get_proj_state_bound_val<bound_direction::UPPER>(pst, sidb_ix));
        }
        else if constexpr (op == potential_bound_update_operation::SUBTRACT)
        {
            recv_cst.update_pot_bounds(sidb_ix, -get_proj_state_bound_val<bound_direction::LOWER>(pst, sidb_ix),
                                       -get_proj_state_bound_val<bound_direction::UPPER>(pst, sidb_ix));
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

    void add_physically_valid_charge_confs(sidb_clustering_state& clustering_state) noexcept
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
        uint64_t max_cluster_size = clustering_state[0]->proj_st.cluster->size();
        uint64_t max_cst_ix       = 0;
        for (uint64_t ix = 1; ix < clustering_state.size(); ++ix)
        {
            if (const uint64_t cluster_size = clustering_state[ix]->proj_st.cluster->size();
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
            add_physically_valid_charge_confs(clustering_state);

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

        // define the bit string ranges per thread
        std::vector<std::pair<uint64_t, uint64_t>> ranges;
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

                            add_physically_valid_charge_confs(clustering_state);
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

    const charge_distribution_surface<Lyt> charge_layout;
    const std::array<double, 4>            mu_bounds_with_error;
    const uint64_t                         num_threads;
};

}  // namespace detail

/**
 * *ClusterComplete* is an *exact* simulator that radicalizes the SiDB simulation domain with its potential. SiDB logic
 * simulation in *base 3* with a 100% certainty of accuracy is made available to the domain for well over 50 SiDBs. It
 * was created by Willem Lambooy for his Master's Thesis in Theoretical Computing Science at Radboud University,
 * Nijmegen, and employs the dual concepts of construction and destruction, ie. abstraction and computation, ie.
 * pre-simulation and simulation in practice, breaking the rapid exponential growth in problem complexity for SiDB
 * simulation problems in application.
 *
 * The key ingredient is the *Ground State Space* algorithm, which constructs a minimal search space of charge
 * configurations that adhere to the critical population stability criterion. In particular, it generalizes Jan
 * Drewniok's physically informed space pruning concept employed in his game-changing *QuickExact* simulator to apply to
 * all charge states equally, and, most importantly, it lifts the associated potential equations to higher order,
 * allowing us to reason over potential bounds in a cluster hierarchy.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to simulate.
 * @param params Parameters required for both the invocation of *Ground State Space*, and the simulation following.
 * @return All physically valid layouts for the given physical parameters and base are returned.
 */
template <typename Lyt>
[[nodiscard]] sidb_simulation_result<Lyt> clustercomplete(const Lyt&                    lyt,
                                                          const clustercomplete_params& cc_params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::clustercomplete_impl<Lyt> p{lyt, cc_params.physical_parameters, cc_params.available_threads};

    return p.run(cc_params.validity_witness_partitioning_max_cluster_size_gss, cc_params.report_gss_stats);
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CLUSTERCOMPLETE_HPP
