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
        res.algorithm_name      = "clustercomplete";

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            const ground_state_space_result& gss_res = fiction::ground_state_space(
                charge_layout, validity_witness_partitioning_limit, charge_layout.get_phys_params());

            report_ground_state_space_statistics(gss_res);

            collect_physically_valid_charge_distributions(gss_res.top_cluster);
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

        if (const double gss_runtime = gss_res.runtime.count(); gss_runtime > 1.0)
        {
            std::cout << gss_runtime << " seconds" << std::endl;
        }
        else
        {
            std::cout << (gss_runtime * 1000) << " milliseconds" << std::endl;
        }
    }

    void
    add_physically_valid_charge_confs(const sidb_cluster_charge_state_composition& cluster_charge_assignment) noexcept
    {
        // check if all clusters are singletons
        if (std::all_of(cluster_charge_assignment.cbegin(), cluster_charge_assignment.cend(),
                        [](const std::pair<sidb_cluster_projector_state, intra_cluster_pot_bounds>& pst_and_sibling_pot)
                        { return pst_and_sibling_pot.first.cluster->children.empty(); }))
        {
            charge_distribution_surface charge_layout_copy{charge_layout};

            for (const auto& [pst, _] : cluster_charge_assignment)
            {
                charge_layout_copy.assign_charge_state_by_cell_index(
                    get_singleton_sidb_ix(pst.cluster), singleton_multiset_conf_to_charge_state(pst.multiset_conf),
                    false);
            }

            charge_layout_copy.update_after_charge_change();

            if (charge_layout_copy.is_physically_valid())
            {
                const std::lock_guard lock{res_mutex};
                res.charge_distributions.emplace_back(charge_layout_copy);
            }

            return;
        }

        // find the cluster of maximum size
        const sidb_cluster_projector_state max_pst =
            std::max_element(
                cluster_charge_assignment.cbegin(), cluster_charge_assignment.cend(),
                [](const auto& pst_and_sibling_pot1, const auto& pst_and_sibling_pot2)
                { return pst_and_sibling_pot1.first.cluster->size() < pst_and_sibling_pot2.first.cluster->size(); })
                ->first;

        sidb_cluster_charge_state_composition cluster_charge_assignment_without_max_cluster{};
        for (const auto& [pst, _] : cluster_charge_assignment)
        {
            if (pst.cluster != max_pst.cluster)
            {
                cluster_charge_assignment_without_max_cluster.emplace_back(std::make_pair(
                    sidb_cluster_projector_state{pst.cluster, pst.multiset_conf}, intra_cluster_pot_bounds{}));
            }
        }

        for (const sidb_cluster_charge_state_composition& cluster_charge_sub_assignment :
             get_projector_state_compositions(max_pst))
        {
            // do this more efficiently
            sidb_cluster_charge_state_composition cluster_charge_assignment_unfolded{
                cluster_charge_assignment_without_max_cluster};

            for (const auto& sub_pst_and_sibling_pot : cluster_charge_sub_assignment)
            {
                cluster_charge_assignment_unfolded.emplace_back(std::move(sub_pst_and_sibling_pot));
            }

            add_physically_valid_charge_confs(cluster_charge_assignment_unfolded);
        }
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
                        for (const sidb_cluster_charge_state_composition& cluster_charge_assignment :
                             std::next(top_cluster->charge_space.cbegin(), static_cast<int64_t>(ix))->compositions)
                        {
                            add_physically_valid_charge_confs(cluster_charge_assignment);
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
                                    const uint64_t available_threads = std::thread::hardware_concurrency()) noexcept
{
    return detail::clustercomplete(lyt, phys_params, available_threads)
        .run(gss_witness_partitioning_maximum_cluster_size);
}

}  // namespace fiction

#endif  // FICTION_clustercomplete_HPP
