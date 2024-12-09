//
// Created by Willem Lambooy on 29.02.2024.
//

#ifndef FICTION_CLUSTERCOMPLETE_HPP
#define FICTION_CLUSTERCOMPLETE_HPP

#if (FICTION_ALGLIB_ENABLED)

#include "fiction/algorithms/simulation/sidb/ground_state_space.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_cluster_hierarchy.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace fiction
{

/**
 * This enum class provides meaningful options for configuring the reporting of the *Ground State Space* statistics.
 * These statistic may be used especially to configure the validity witness partitioning options for *Ground State
 * Space*, that may impair runtimes when set too high, but could provide a large benefit to the complexity of the
 * unfolding process of large simulation problems by performing more involved pruning procedures in the construction
 * stage.
 */
enum class ground_state_space_reporting : uint8_t
{
    /**
     * Enabling this option will output *Ground State Space* statistics to the standard output.
     */
    ENABLED,
    /**
     * Disabling this option will suppress the output of *Ground State Space* statistics.
     */
    DISABLED
};

/**
 * The struct containing the parameters both passed on to pre-simulator Ground State Space, and used during simulation.
 */
template <typename CellType = siqad::coord_t>
struct clustercomplete_params
{
    /**
     * Physical simulation parameters.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Local external electrostatic potentials (e.g., locally applied electrodes).
     */
    std::unordered_map<CellType, double> local_external_potential = {};
    /**
     * Global external electrostatic potential. Value is applied on each cell in the layout.
     */
    double global_potential = 0;
    /**
     * This specifies the maximum cluster size for which *Ground State Space* will solve an NP-complete sub-problem
     * exhaustively. The sets of SiDBs that witness local population stability for each respective charge state may be
     * partitioned into disjoint sets such that the number of required witnesses for each respective charge state is
     * satisfied. If no such partition exists, the multiset charge configuration associated with the requirements may be
     * rejected.
     */
    uint64_t validity_witness_partitioning_max_cluster_size_gss = 6;
    /**
     * The complexity is of validity witness partitioning bounded by a factorial in the number of overlapping witnesses.
     * This parameter thus allows the validity witness partitioning procedure to perform the reduction to overlapping
     * witnesses for larger cluster sizes that could be runtime-impairing, then limiting specifically the length of the
     * input to the factorial call.
     */
    uint64_t num_overlapping_witnesses_limit_gss = 6;
    /**
     * Number of threads to make available to *ClusterComplete* for the unfolding stage.
     */
    uint64_t available_threads = std::thread::hardware_concurrency();
    /**
     * Option to decide if the *Ground State Space* statistics are reported to the standard output. By default, this
     * option is disabled.
     */
    ground_state_space_reporting report_gss_stats = ground_state_space_reporting::DISABLED;
};

namespace detail
{

template <typename Lyt>
class clustercomplete_impl
{
  public:
    /**
     * Constructor.
     *
     * @param lyt Layout to simulate.
     * @param params Parameter required for both the invocation of *Ground State Space*, and the simulation following.
     */
    explicit clustercomplete_impl(const Lyt& lyt, const clustercomplete_params<cell<Lyt>>& params) noexcept :
            available_threads{1},  // std::max(uint64_t{1}, params.available_threads)},
            charge_layout{initialize_charge_layout(lyt, params)},
            real_placed_defects{charge_layout.get_defects()},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_plus()}
    {}
    /**
     * This function performs the *ClusterComplete* simulation; first executing the *Ground State Space* construction,
     * then destructing the result into the set of all physically valid charge distributions that are returned.
     *
     * @param params Parameter required for both the invocation of *Ground State Space*, and the simulation following.
     * @return Results of the exact simulation.
     */
    [[nodiscard]] sidb_simulation_result<Lyt> run(const clustercomplete_params<cell<Lyt>>& params) noexcept
    {
        result.simulation_parameters = params.simulation_parameters;
        result.algorithm_name        = "ClusterComplete";
        result.additional_simulation_parameters.emplace("global_potential", params.global_potential);
        result.additional_simulation_parameters.emplace("validity_witness_partitioning_limit",
                                                        params.validity_witness_partitioning_max_cluster_size_gss);
        result.additional_simulation_parameters.emplace("num_overlapping_witnesses_limit",
                                                        params.num_overlapping_witnesses_limit_gss);

        // run Ground State Space to obtain the complete hierarchical charge space
        const ground_state_space_results& gss_stats = ground_state_space(
            charge_layout, ground_state_space_params{params.simulation_parameters,
                                                     params.validity_witness_partitioning_max_cluster_size_gss,
                                                     params.num_overlapping_witnesses_limit_gss});

        if (!gss_stats.top_cluster)
        {
            return result;
        }

        if (params.report_gss_stats == ground_state_space_reporting::ENABLED)
        {
            gss_stats.report();
        }

        std::cout << "running with " << available_threads << " threads" << std::endl;

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            if (!gss_stats.top_cluster->charge_space.empty())
            {
                initialize_worker_queues(gss_stats.top_cluster);

                std::vector<std::thread> supporting_threads{};
                supporting_threads.reserve(available_threads);

                for (uint64_t i = 1; i < available_threads; ++i)
                {
                    supporting_threads.emplace_back(
                        [&, ix = i]
                        {
                            // std::cout << "starting thread " << ix << "..." << std::endl;

                            worker& w = *workers.at(ix);

                            while (const std::optional<sidb_charge_space_composition>& work = w.obtain_work())
                            {
                                // std::cout << "thread " << ix << " doing work..." << std::endl;
                                unfold_composition(w, *work);
                                // std::cout << "thread " << ix << " finished work" << std::endl;
                            }

                            // std::cout << "thread " << ix << " stopped working" << std::endl;
                        });
                }

                // std::cout << "starting thread 0..." << std::endl;

                while (const std::optional<sidb_charge_space_composition>& work = workers.front()->obtain_work())
                {
                    std::cout << "thread 0 doing work..." << std::endl;
                    unfold_composition(*workers.front(), *work);
                    std::cout << "thread 0 finished work" << std::endl;
                }

                // std::cout << "thread 0 stopped working" << std::endl;

                // wait for all threads to complete
                for (auto& thread : supporting_threads)
                {
                    if (thread.joinable())
                    {
                        thread.join();
                    }
                }
            }
        }

        // The ClusterComplete runtime includes the runtime for the Ground State Space procedure
        result.simulation_runtime = time_counter + gss_stats.runtime;

        return result;
    }

  private:
    /**
     * Forward declaration of the worker struct.
     */
    struct worker;
    /**
     * Simulation results.
     */
    sidb_simulation_result<Lyt> result{};
    /**
     * Number of available threads.
     */
    const uint64_t                       available_threads{};
    std::vector<std::unique_ptr<worker>> workers{};
    /**
     * Mutex to protect the simulation results.
     */
    std::mutex mutex_to_protect_the_simulation_results;
    /**
     * The base layout, along with the map of placed defects, that are used to create charge distribution surface
     * copies.
     */
    const charge_distribution_surface<Lyt> charge_layout;
    /**
     * Atomic defects that are placed in the layout.
     */
    const std::unordered_map<typename Lyt::cell, const sidb_defect> real_placed_defects;
    /**
     * Globally available array of bounds that section the band gap, used for pruning.
     */
    const std::array<double, 4> mu_bounds_with_error;
    /**
     * Helper function for obtaining the stored lower or upper bound on the electrostatic potential that SiDBs in the
     * given projector state--i.e., a cluster together with an associated multiset charge configuration--collectively
     * project onto the given SiDB.
     *
     * @tparam bound Bound to obtain (lower/upper).
     * @param pst Projector state.
     * @param sidb_ix Receiving SiDB.
     * @return The potential projection value associated with this bound; i.e., an electrostatic potential (in V),
     */
    template <bound_direction bound>
    static constexpr double get_projector_state_bound(const sidb_cluster_projector_state& pst,
                                                      const uint64_t                      sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf).pot_val;
    }
    /**
     * Returns `true` if and only if the given potential bound closes out SiDB-.
     *
     * @param pot_bound Potential lower bound.
     */
    [[nodiscard]] constexpr bool fail_onto_negative_charge(const double pot_bound) const noexcept
    {
        // V > e - mu-
        return pot_bound > mu_bounds_with_error.at(0);
    }
    /**
     * Returns `true` if and only if the given potential bound closes out SiDB+.
     *
     * @param pot_bound Potential upper bound.
     */
    [[nodiscard]] constexpr bool fail_onto_positive_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu+
        return pot_bound < mu_bounds_with_error.at(3);
    }
    /**
     * Returns `true` if and only if the given potential bound closes out SiDB0.
     *
     * @param pot_bound Potential upper bound.
     */
    [[nodiscard]] constexpr bool ub_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V < -e - mu-
        return pot_bound < mu_bounds_with_error.at(1);
    }
    /**
     * Returns `true` if and only if the given potential bound closes out SiDB0.
     *
     * @param pot_bound Potential lower bound.
     */
    [[nodiscard]] constexpr bool lb_fail_onto_neutral_charge(const double pot_bound) const noexcept
    {
        // V > e - mu+
        return pot_bound > mu_bounds_with_error.at(2);
    }
    /**
     * This function performs an analysis that is crucial to the *ClusterComplete*'s efficiency: as the *Ground State
     * Space* construct is broken down, combinations of multiset charge configurations are tried together in more detail
     * than in the construction preceding this second phase of the simulation.
     *
     * @param clustering_state The clustering state that bundles a slice (i.e., a clustering) in the cluster hierarchy
     * along with respective projection states (i.e., multiset charge configurations) together with a store of
     * accumulated potential projection bounds onto all SiDBs in the layout, where this information is specific to the
     * projection states in the clustering state.
     * @return `false` if and only if a physically valid charge distribution cannot be extracted from the clustering
     * state.
     */
    [[nodiscard]] bool
    meets_population_stability_criterion(const sidb_clustering_state& clustering_state) const noexcept
    {
        for (const sidb_cluster_projector_state_ptr& pst : clustering_state.proj_states)
        {
            // number of respective witnesses to count
            uint64_t required_neg_count  = pst->get_count<sidb_charge_state::NEGATIVE>();
            uint64_t required_pos_count  = pst->get_count<sidb_charge_state::POSITIVE>();
            uint64_t required_neut_count = pst->get_count<sidb_charge_state::NEUTRAL>();

            for (const auto sidb_ix : pst->cluster->sidbs)
            {
                const double recv_pot_lb = clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix);
                const double recv_pot_ub = clustering_state.pot_bounds.get<bound_direction::UPPER>(sidb_ix);

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

            // SAT iff all witness counts are satisfied
            if (required_neg_count != 0 || required_pos_count != 0 || required_neut_count != 0)
            {
                return false;
            }
        }

        return true;
    }
    /**
     * This function handles performs the last analysis step before collecting a simulation result. In order to judge
     * whether a population stable charge distribution is physically valid, the *configuration stability* needs to be
     * tested. If this criterion passes, the charge distribution is added to the simulation results.
     *
     * @param clustering_state A clustering state consisting of only singleton clusters along with associated charge
     * states that make up a charge distribution that conforms to the *population stability* criterion.
     */
    void add_if_configuration_stability_is_met(const sidb_clustering_state& clustering_state) noexcept
    {
        std::cout << "FOUND POP STABLE CLUSTERING STATE" << std::endl;
        // std::cout << '\t' << clustering_state.proj_states.at(0).get()->cluster->uid << ' '
        //           << clustering_state.proj_states.at(1).get()->cluster->uid << ' '
        //           << clustering_state.proj_states.at(2).get()->cluster->uid << ' '
        //           << clustering_state.proj_states.at(3).get()->cluster->uid << std::endl;

        // std::cout
        //     << '\t'
        //     << charge_configuration_to_string(
        //            {singleton_multiset_conf_to_charge_state(clustering_state.proj_states.at(0).get()->multiset_conf)})
        //     << ' '
        //     << charge_configuration_to_string(
        //            {singleton_multiset_conf_to_charge_state(clustering_state.proj_states.at(1).get()->multiset_conf)})
        //     << ' '
        //     << charge_configuration_to_string(
        //            {singleton_multiset_conf_to_charge_state(clustering_state.proj_states.at(2).get()->multiset_conf)})
        //     << ' '
        //     << charge_configuration_to_string(
        //            {singleton_multiset_conf_to_charge_state(clustering_state.proj_states.at(3).get()->multiset_conf)})
        //     << std::endl;

        charge_distribution_surface charge_layout_copy{charge_layout};

        // convert bottom clustering state to charge distribution
        for (const auto& pst : clustering_state.proj_states)
        {
            const uint64_t sidb_ix = get_singleton_sidb_ix(pst->cluster);
            charge_layout_copy.assign_charge_state_by_cell_index(
                sidb_ix, singleton_multiset_conf_to_charge_state(pst->multiset_conf),
                charge_index_mode::KEEP_CHARGE_INDEX);

            charge_layout_copy.assign_local_potential_by_index(
                sidb_ix, -clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix));
        }

        charge_layout_copy.recompute_system_energy();

        if (!charge_layout_copy.is_configuration_stable())
        {
            std::cout << "NOT CONF STABLE" << std::endl;
            return;
        }

        std::cout << "CONF STABLE, ADDING TO RESULTS..." << std::endl;

        charge_layout_copy.charge_distribution_to_index();

        // population stability is a given when this function is called; hence the charge distribution is physically
        // valid when configuration stability is met
        charge_layout_copy.declare_physically_valid();

        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            for (const auto& [cell, defect] : real_placed_defects)
            {
                charge_layout_copy.assign_sidb_defect(cell, defect);
            }
        }

        {
            const std::lock_guard lock{mutex_to_protect_the_simulation_results};
            result.charge_distributions.emplace_back(charge_layout_copy);
        }
    }
    /**
     * Finds the cluster of the maximum size in the clustering associated with the input.
     *
     * @param proj_states A vector of projector states that forms a clustering when only the respectively contained
     * clusters are considered.
     * @return The index in this vector of the projector state that contains the cluster of maximum size.
     */
    static uint64_t
    find_cluster_of_maximum_size(const std::vector<sidb_cluster_projector_state_ptr>& proj_states) noexcept
    {
        std::cout << "choosing cluster to unfold\n\t";
        for (const auto& pst : proj_states)
        {
            std::cout << pst->cluster->uid << ' ';
        }
        std::cout << std::endl;

        uint64_t max_cluster_size = proj_states.front()->cluster->num_sidbs();
        uint64_t max_pst_ix       = 0;

        for (uint64_t ix = 1; ix < proj_states.size(); ++ix)
        {
            if (const uint64_t cluster_size = proj_states.at(ix)->cluster->num_sidbs(); cluster_size > max_cluster_size)
            {
                max_cluster_size = cluster_size;
                max_pst_ix       = ix;
            }
        }

        return max_pst_ix;
    }
    /**
     * Helper function for obtaining the stored lower or upper bound on the electrostatic potential that SiDBs in the
     * given projector state--i.e., a cluster together with an associated multiset charge configuration--collectively
     * project onto the given SiDB.
     *
     * @tparam bound Bound to obtain (lower/upper).
     * @param pst Projector state.
     * @param sidb_ix Receiving SiDB.
     * @return The potential projection associated with this bound; i.e., an electrostatic potential (in V) associated
     * with the given projector state.
     */
    template <bound_direction bound>
    [[nodiscard]] static constexpr double get_projector_state_bound_pot(const sidb_cluster_projector_state& pst,
                                                                        const uint64_t sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf).pot_val;
    }
    /**
     * Before the parent projector state may be specialized to a specific composition of its children, first the
     * projections of the parent must be subtracted. They are later added back after all compositions were handled.
     * Depending on the potential bound update operation, either the subtraction of addition step is performed.
     *
     * @param clustering_state todo
     * @param parent_pst todo
     */
    static sidb_cluster_projector_state_ptr move_parent_out(sidb_clustering_state& clustering_state,
                                                            const uint64_t         parent_pst_ix) noexcept
    {

        // swap with last
        std::swap(clustering_state.proj_states.at(parent_pst_ix), clustering_state.proj_states.back());

        // move out
        sidb_cluster_projector_state_ptr max_pst = std::move(clustering_state.proj_states.back());

        // pop
        clustering_state.proj_states.pop_back();

        // subtract parent potential from potential bounds store
        for (uint64_t sidb_ix = 0; sidb_ix < clustering_state.pot_bounds.num_sidbs(); ++sidb_ix)
        {
            clustering_state.pot_bounds.update(
                sidb_ix, -get_projector_state_bound_pot<bound_direction::LOWER>(*max_pst, sidb_ix),
                -get_projector_state_bound_pot<bound_direction::UPPER>(*max_pst, sidb_ix));
            assert(clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix) -
                       clustering_state.pot_bounds.get<bound_direction::UPPER>(sidb_ix) <=
                   100 * std::numeric_limits<double>::epsilon());
        }

        return max_pst;
    }
    /**
     * Before the parent projector state may be specialized to a specific composition of its children, first the
     * projections of the parent must be subtracted. They are later added back after all compositions were handled.
     * Depending on the potential bound update operation, either the subtraction of addition step is performed.
     *
     * @param clustering_state todo
     * @param parent_pst todo
     */
    static void add_parent(sidb_clustering_state& clustering_state, const uint64_t parent_pst_ix,
                           sidb_cluster_projector_state_ptr parent_pst) noexcept
    {
        // add parent potential from potential bounds store
        for (uint64_t sidb_ix = 0; sidb_ix < clustering_state.pot_bounds.num_sidbs(); ++sidb_ix)
        {
            clustering_state.pot_bounds.update(
                sidb_ix, get_projector_state_bound_pot<bound_direction::LOWER>(*parent_pst, sidb_ix),
                get_projector_state_bound_pot<bound_direction::UPPER>(*parent_pst, sidb_ix));
            assert(clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix) -
                       clustering_state.pot_bounds.get<bound_direction::UPPER>(sidb_ix) <=
                   100 * std::numeric_limits<double>::epsilon());
        }

        // move back
        clustering_state.proj_states.emplace_back(std::move(parent_pst));

        // swap back
        std::swap(clustering_state.proj_states.back(), clustering_state.proj_states[parent_pst_ix]);
    }

    static void add_composition(sidb_clustering_state&               clustering_state,
                                const sidb_charge_space_composition& composition) noexcept
    {

        clustering_state.pot_bounds += composition.pot_bounds;

        for (uint64_t sidb_ix = 0; sidb_ix < clustering_state.pot_bounds.num_sidbs(); ++sidb_ix)
        {
            assert(clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix) -
                       clustering_state.pot_bounds.get<bound_direction::UPPER>(sidb_ix) <=
                   100 * std::numeric_limits<double>::epsilon());
        }

        for (const sidb_cluster_projector_state& child_pst : composition.proj_states)
        {
            // move child projector state in clustering state
            clustering_state.proj_states.emplace_back(std::make_unique<sidb_cluster_projector_state>(child_pst));
        }
    }
    static void remove_composition(sidb_clustering_state&               clustering_state,
                                   const sidb_charge_space_composition& composition) noexcept
    {

        for (uint64_t i = 0; i < composition.proj_states.size(); ++i)
        {
            // handled child projector state --- remove
            clustering_state.proj_states.pop_back();
        }

        clustering_state.pot_bounds -= composition.pot_bounds;

        for (uint64_t sidb_ix = 0; sidb_ix < clustering_state.pot_bounds.num_sidbs(); ++sidb_ix)
        {
            assert(clustering_state.pot_bounds.get<bound_direction::LOWER>(sidb_ix) - clustering_state.pot_bounds.get<bound_direction::UPPER>(sidb_ix) <=
                   100 * std::numeric_limits<double>::epsilon());
        }
    }

    struct worker_queue
    {
        std::deque<std::deque<sidb_charge_space_composition>> queue{};

        sidb_clustering_state clustering_state_for_thieves;

        // uint64_t work_left_at_current_level{0};

        std::mutex mutex_to_protect_this_queue;

        explicit worker_queue(const uint64_t num_sidbs_in_layout) noexcept :
                clustering_state_for_thieves{num_sidbs_in_layout}
        {}

        // work, need to backtrack
        std::pair<std::optional<sidb_charge_space_composition>, bool> get_from_this_queue() noexcept
        {
            const std::lock_guard lock{mutex_to_protect_this_queue};

            if (queue.empty())
            {
                return {std::nullopt, false};
            }

            if (queue.front().empty())
            {
                queue.pop_front();

                return {std::nullopt, true};
            }

            // obtaining own work goes from the front
            sidb_charge_space_composition work = std::move(queue.front().front());
            queue.front().pop_front();

            if (queue.front().empty())
            {
                queue.pop_front();

                return {std::optional{std::move(work)}, true};
            }

            return {std::optional{std::move(work)}, false};
        }

        // Attempt to steal work in non-blocking fashion
        std::optional<std::pair<sidb_clustering_state, sidb_charge_space_composition>>
        try_steal_from_this_queue() noexcept
        {
            if (!mutex_to_protect_this_queue.try_lock())  // Non-blocking lock attempt
            {
                return std::nullopt;
            }

            if (queue.empty())
            {
                mutex_to_protect_this_queue.unlock();

                return std::nullopt;
            }

            // stealing goes from the back
            sidb_charge_space_composition work = std::move(queue.back().back());
            queue.back().pop_back();
            if (queue.back().empty())
            {
                queue.pop_back();  // todo
            }

            sidb_clustering_state stolen_clustering_state{clustering_state_for_thieves};  // make copy

            mutex_to_protect_this_queue.unlock();

            return std::optional{std::make_pair(std::move(stolen_clustering_state), std::move(work))};
        }
    };

    struct worker
    {
        const uint64_t                              index;
        worker_queue                                work_stealing_queue;
        sidb_clustering_state                       clustering_state;
        const std::vector<std::unique_ptr<worker>>& all_workers;

        explicit worker(const uint64_t ix, const uint64_t num_sidbs,
                        const std::vector<std::unique_ptr<worker>>& workers) noexcept :
                index{ix},
                work_stealing_queue{num_sidbs},
                clustering_state{num_sidbs},
                all_workers{workers}
        {}

        // nullopt -> terminate thread (todo: use global termination synchronization)
        std::optional<sidb_charge_space_composition> obtain_work() noexcept
        {
            // std::cout << "thread " << index << " trying to look for own work" << std::endl;
            if (std::optional<sidb_charge_space_composition> work = work_stealing_queue.get_from_this_queue().first)
            {
                // clustering_state = std::move(work->first);
                // std::cout << "thread " << index << " took own work" << std::endl;
                return work;
            }

            // std::cout << "thread " << index << " found no own work" << std::endl;

            for (uint64_t i = 0; i < all_workers.size(); ++i)
            {
                if (i == index)
                {
                    // skip own queue
                    continue;
                }

                // std::cout << "thread " << index << " trying to steal work from thread " << i << std::endl;

                if (std::optional<std::pair<sidb_clustering_state, sidb_charge_space_composition>> work =
                        all_workers.at(i)->work_stealing_queue.try_steal_from_this_queue())
                {
                    clustering_state = std::move(work->first);
                    // std::cout << "thread " << index << " stole work from thread " << i << std::endl;
                    return work->second;
                }
            }

            // std::cout << "thread " << index << " found no work" << std::endl;
            return std::nullopt;
        }

        // // nullopt -> backtrack
        // std::optional<sidb_charge_space_composition> obtain_work_from_current_level() noexcept
        // {
        //     const std::lock_guard lock{work_stealing_queue.mutex_to_protect_this_queue};
        //
        //     if (work_stealing_queue.thief_informants.back().available_work_to_steal == 0)
        //     {
        //         work_stealing_queue.thief_informants.c.pop_back();  // todo
        //
        //         return std::nullopt;
        //     }
        //
        //     --work_stealing_queue.thief_informants.back().available_work_to_steal;
        //
        //     sidb_charge_space_composition work = std::move(work_stealing_queue.queue.front());
        //     work_stealing_queue.queue.pop_front();
        //
        //     return std::optional{std::move(work)};
        // }
    };
    /**
     * Function to initialize the charge layout.
     *
     * @param lyt Layout to simulate.
     * @param params Parameters for ClusterComplete.
     */
    [[nodiscard]] static charge_distribution_surface<Lyt>
    initialize_charge_layout(const Lyt& lyt, const clustercomplete_params<cell<Lyt>>& params) noexcept
    {
        charge_distribution_surface<Lyt> cds{lyt};
        cds.assign_physical_parameters(params.simulation_parameters);

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

    void initialize_worker_queues(const sidb_cluster_ptr& top_cluster) noexcept
    {
        workers.reserve(available_threads);

        // the first worker gets the entire top cluster charge space in its work queue
        workers.emplace_back(std::make_unique<worker>(0, charge_layout.num_cells(), workers));

        workers.front()->work_stealing_queue.queue.emplace_front();

        for (const sidb_cluster_charge_state& ccs : top_cluster->charge_space)
        {
            const sidb_cluster_projector_state pst{top_cluster, static_cast<uint64_t>(ccs)};

            complete_potential_bounds_store store{};
            store.initialise_complete_potential_bounds(charge_layout.num_cells());

            for (uint64_t sidb_ix = 0; sidb_ix < charge_layout.num_cells(); ++sidb_ix)
            {
                store.set(sidb_ix, get_projector_state_bound<bound_direction::LOWER>(pst, sidb_ix),
                          get_projector_state_bound<bound_direction::UPPER>(pst, sidb_ix));
            }

            workers.front()->work_stealing_queue.queue.front().emplace_front(
                sidb_charge_space_composition{{pst}, store});
        }

        // initialize other workers as thieves
        for (uint64_t i = 1; i < available_threads; ++i)
        {
            workers.emplace_back(std::make_unique<worker>(i, charge_layout.num_cells(), workers));
        }
    }
    /**
     * This recursive function is the heart of the *ClusterComplete* destruction. The given clustering state is
     * dissected at the largest cluster to each possible specialization of it, which then enters the recursive call with
     * the clustering state modified to have a set of sibling children replacing their direct parent. For each
     * specialization, appropriate updates are made to the potential bounds store that is part of the clustering state.
     * After a specialization has been handled completely, i.e., when the recursive call for this specialization
     * returns, the specialization to the potential bounds store is undone so that a new specialization may be applied.
     *
     * The two base cases to the recursion are as follows: (1) the charge distributions implied by the given clustering
     * state do not meet the population stability, meaning that this branch of the search space may be pruned through
     * terminating the recursion at this level, and, (2) the clustering state hold only singleton clusters and passes
     * the population stability check. In the latter case, the configuration stability check is performed before the
     * associated charge distribution is added to the simulation results.
     *
     * @param clustering_state A clustering state that holds a specific combination of multiset charge configurations as
     * projector states of which the respectively associated clusters form a clustering in the cluster hierarchy. todo
     */
    bool add_physically_valid_charge_configurations(worker& w) noexcept
    {
        // check for pruning
        if (!meets_population_stability_criterion(w.clustering_state))
        {
            return true;
        }

        // check if all clusters are singletons
        if (w.clustering_state.proj_states.size() == charge_layout.num_cells())
        {
            add_if_configuration_stability_is_met(w.clustering_state);
            return true;
        }

        // choose the biggest cluster to unfold
        const uint64_t max_pst_ix = find_cluster_of_maximum_size(w.clustering_state.proj_states);

        std::cout << "removing parent, clustering size = " << w.clustering_state.proj_states.size() << std::endl;
        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }
        // un-apply max_pst, thereby making space for specialization
        sidb_cluster_projector_state_ptr max_pst = move_parent_out(w.clustering_state, max_pst_ix);

        std::cout << "done removing parent, clustering size = " << w.clustering_state.proj_states.size() << std::endl;
        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }

        // unfold all compositions
        const std::vector<sidb_charge_space_composition>& compositions = get_projector_state_compositions(*max_pst);

        {
            const std::lock_guard lock{w.work_stealing_queue.mutex_to_protect_this_queue};

            w.work_stealing_queue.queue.emplace_front();

            // add all composition except first to queue
            for (uint64_t i = 1; i < compositions.size(); ++i)
            {
                w.work_stealing_queue.queue.front().emplace_front(std::move(compositions.at(i)));
            }

            std::cout << "added " << compositions.size() - 1 << " items to the queue" << std::endl;
        }

        std::cout << "unfolding first, clustering size = " << w.clustering_state.proj_states.size() << std::endl;
        // unfold first composition
        unfold_composition(w, compositions.front());
        std::cout << "done unfolding first, clustering size = " << w.clustering_state.proj_states.size() << std::endl;

        // unfold other compositions while there are ones left on this level to unfold
        std::pair<std::optional<sidb_charge_space_composition>, bool> work;

        do {
            work = w.work_stealing_queue.get_from_this_queue();

            if (!work.first.has_value())
            {
                std::cout << "quit since work has no value (backtrack = " << work.second << ")" << std::endl;
                if (work.second)  // false iff queue is completely empty; no need to backtrack
                {
                    add_parent(w.clustering_state, max_pst_ix, std::move(max_pst));
                    return true;
                }

                return false;
            }

            std::cout << "unfolding other" << std::endl;
            unfold_composition(w, *work.first);
        } while (!work.second);

        std::cout << "adding back parent, clustering size = " << w.clustering_state.proj_states.size() << std::endl;
        // apply max_pst back
        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }
        add_parent(w.clustering_state, max_pst_ix, std::move(max_pst));
        std::cout << "done adding back parent, clustering size = " << w.clustering_state.proj_states.size()
                  << std::endl;
        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }
        return true;
    }

    void unfold_composition(worker& w, const sidb_charge_space_composition& composition) noexcept
    {
        // specialise parent to a specific composition of its children
        std::cout << "adding composition, clustering size = " << w.clustering_state.proj_states.size() << std::endl;
        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }
        add_composition(w.clustering_state, composition);
        std::cout << "done adding composition, clustering size = " << w.clustering_state.proj_states.size()
                  << std::endl;

        // for (uint64_t i = 0; i < 4; ++i)
        // {
        //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
        //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
        //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
        // }

        // recurse with specialised composition
        if (add_physically_valid_charge_configurations(w))
        {
            std::cout << "removing composition, clustering size = " << w.clustering_state.proj_states.size()
                      << std::endl;
            for (const auto& pst : composition.proj_states)
            {
                std::cout << pst.cluster->uid << ' ';
            }
            std::cout << std::endl;
            // for (uint64_t i = 0; i < 4; ++i)
            // {
            //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
            //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
            //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
            // }
            // undo specialization such that the specialization may consider a different children composition
            remove_composition(w.clustering_state, composition);
            std::cout << "done removing composition, clustering size = " << w.clustering_state.proj_states.size()
                      << std::endl;
            for (const auto& pst : composition.proj_states)
            {
                std::cout << pst.cluster->uid << ' ';
            }
            std::cout << std::endl;
            // for (uint64_t i = 0; i < 4; ++i)
            // {
            //     std::cout << fmt::format("\t{}: {:.2f} {:.2f}\n", i,
            //                              w.clustering_state.pot_bounds.template get<bound_direction::LOWER>(i),
            //                              w.clustering_state.pot_bounds.template get<bound_direction::UPPER>(i));
            // }
        }
    }
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
[[nodiscard]] sidb_simulation_result<Lyt> clustercomplete(const Lyt&                               lyt,
                                                          const clustercomplete_params<cell<Lyt>>& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    detail::clustercomplete_impl<Lyt> p{lyt, params};

    return p.run(params);
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_CLUSTERCOMPLETE_HPP
