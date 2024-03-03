//
// Created by Willem Lambooy on 29/02/2024.
//

#ifndef FICTION_clustercomplete_HPP
#define FICTION_clustercomplete_HPP

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

namespace detail
{

template <typename Lyt>
class clustercomplete
{
  public:
    explicit clustercomplete(const Lyt& lyt, const sidb_simulation_parameters& physical_parameters,
                             const uint64_t available_threads) noexcept :
            charge_layout{initialize_charge_layout(lyt, physical_parameters)},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - physical_parameters.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - physical_parameters.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - physical_parameters.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - physical_parameters.mu_plus()},
            num_threads{available_threads}
    {}

    sidb_simulation_result<Lyt> run(const uint64_t validity_witness_partitioning_limit) noexcept
    {
        res.physical_parameters = charge_layout.get_phys_params();
        res.algorithm_name      = "ClusterComplete";

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            const ground_state_space_result& gss_res = fiction::ground_state_space(
                charge_layout, validity_witness_partitioning_limit, charge_layout.get_phys_params());

            report_ground_state_space_statistics(gss_res);

            if (!gss_res.top_cluster->charge_space.empty())
            {
                collect_physically_valid_charge_distributions(gss_res.top_cluster);
            }
        }

        res.simulation_runtime = time_counter;

        return res;
    }

  private:
    static charge_distribution_surface<Lyt>
    initialize_charge_layout(const Lyt& lyt, const sidb_simulation_parameters& phys_params) noexcept
    {
        charge_distribution_surface cl{lyt};
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

    //    template <potential_bound_analysis_mode mode>
    static inline std::pair<double, double>
    get_received_potential_bounds(const sidb_cluster_projector_state& pst, const uint64_t sidb_ix,
                                  const std::optional<intra_cluster_potential_bounds>& internal_pot_bounds) noexcept
    {
        //        if constexpr (mode == potential_bound_analysis_mode::ANALYZE_MULTISET)
        //        {
        //            return {get_proj_state_bound<bound_direction::LOWER>(pst, sidb_ix).V +
        //                        pst.cluster->get_recv_ext_pot_bound<bound_direction::LOWER>(sidb_ix),
        //                    get_proj_state_bound<bound_direction::UPPER>(pst, sidb_ix).V +
        //                        pst.cluster->get_recv_ext_pot_bound<bound_direction::UPPER>(sidb_ix)};
        //        }
        //        else if constexpr (mode == potential_bound_analysis_mode::ANALYZE_COMPOSITION)
        //        {
        //            return {internal_pot_bounds.value().at(sidb_ix)[static_cast<uint8_t>(bound_direction::LOWER)] +
        //                        pst.cluster->get_recv_ext_pot_bound<bound_direction::LOWER>(sidb_ix),
        //                    internal_pot_bounds.value().at(sidb_ix)[static_cast<uint8_t>(bound_direction::UPPER)] +
        //                        pst.cluster->get_recv_ext_pot_bound<bound_direction::UPPER>(sidb_ix)};
        //        }
    }

    bool perform_potential_bound_analysis(const sidb_cluster_state& cst) const noexcept
    {
        uint64_t required_neg_count  = cst.proj_st.get_count<sidb_charge_state::NEGATIVE>(),
                 required_pos_count  = cst.proj_st.get_count<sidb_charge_state::POSITIVE>(),
                 required_neut_count = cst.proj_st.get_count<sidb_charge_state::NEUTRAL>();

        for (const uint64_t sidb_ix : cst.proj_st.cluster->sidbs)
        {
            const auto& [recv_pot_lb, recv_pot_ub] =
                get_received_potential_bounds(cst.proj_st, sidb_ix, cst.internal_pot_bounds);

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

        return required_neg_count == 0 && required_pos_count != 0 && required_neut_count != 0;
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

    void add_physically_valid_charge_confs(sidb_clustering_state& clustering_state) noexcept
    {
        //        if (!meets_population_stability_criterion(clustering_state))
        //        {
        //            return;
        //        }

        // check if all clusters are singletons
        if (std::all_of(clustering_state.cbegin(), clustering_state.cend(),
                        [](const sidb_cluster_state_ptr& cst) { return cst->proj_st.cluster->children.empty(); }))
        {
            charge_distribution_surface charge_layout_copy{charge_layout};

            for (const sidb_cluster_state_ptr& cst : clustering_state)
            {
                charge_layout_copy.assign_charge_state_by_cell_index(
                    get_singleton_sidb_ix(cst->proj_st.cluster),
                    singleton_multiset_conf_to_charge_state(cst->proj_st.multiset_conf), false);
            }

            charge_layout_copy.update_after_charge_change();

            if (charge_layout_copy.is_physically_valid())
            {
                const std::lock_guard lock{res_mutex};
                res.charge_distributions.emplace_back(charge_layout_copy);
            }

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
        for (const sidb_cluster_state_composition& max_cst_composition :
             get_projector_state_compositions(max_cst->proj_st))
        {
            for (const sidb_cluster_state& sub_cst : max_cst_composition)
            {
                // move in
                clustering_state.emplace_back(std::make_unique<const sidb_cluster_state>(std::move(sub_cst)));
            }

            // recurse
            add_physically_valid_charge_confs(clustering_state);

            for (uint64_t i = 0; i < max_cst_composition.size(); ++i)
            {
                // handled
                clustering_state.pop_back();
            }
        }

        // move back
        clustering_state.emplace_back(std::move(max_cst));

        // swap back
        std::swap(clustering_state.back(), clustering_state[max_cst_ix]);
    }

    void collect_physically_valid_charge_distributions(const sidb_cluster_ptr& top_cluster) noexcept
    {
        const uint64_t top_level_multisets = top_cluster->charge_space.size();

        const uint64_t num_threads_to_use = std::min(std::max(num_threads, 1ul), top_level_multisets);

        // define the bit string ranges per thread
        std::vector<std::pair<uint64_t, uint64_t>> ranges;
        const uint64_t                             chunk_size = std::max(top_level_multisets / num_threads_to_use, 1ul);
        uint64_t                                   start      = 0;
        uint64_t                                   end        = chunk_size - 1;

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

                            for (const sidb_cluster_state& cst : composition)
                            {
                                clustering_state.emplace_back(
                                    std::make_unique<const sidb_cluster_state>(std::move(cst)));
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

template <typename Lyt>
sidb_simulation_result<Lyt>
clustercomplete(const Lyt& lyt, const uint64_t gss_witness_partitioning_maximum_cluster_size = 6,
                const sidb_simulation_parameters& phys_params = sidb_simulation_parameters{},
                const uint64_t                    available_threads = std::thread::hardware_concurrency()) noexcept
{
    return detail::clustercomplete(lyt, phys_params, available_threads)
        .run(gss_witness_partitioning_maximum_cluster_size);
}

}  // namespace fiction

#endif  // FICTION_clustercomplete_HPP
