//
// Created by Willem Lambooy on 29.02.2024.
//

#ifndef FICTION_CLUSTERCOMPLETE_HPP
#define FICTION_CLUSTERCOMPLETE_HPP

#if (FICTION_ALGLIB_ENABLED)

#include "fiction/algorithms/simulation/sidb/ground_state_space.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
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
#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace fiction
{

/**
 * The struct containing the parameters both passed on to pre-simulator Ground State Space, and used during simulation.
 */
template <typename CellType = siqad::coord_t>
struct clustercomplete_params
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
        ON,
        /**
         * Disabling this option will suppress the output of *Ground State Space* statistics.
         */
        OFF
    };
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
    ground_state_space_reporting report_gss_stats = ground_state_space_reporting::OFF;
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
            available_threads{std::max(uint64_t{1}, params.available_threads)},
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

        if (params.report_gss_stats == clustercomplete_params<cell<Lyt>>::ground_state_space_reporting::ON)
        {
            gss_stats.report();
        }

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            if (!gss_stats.top_cluster->charge_space.empty())
            {
                // initialization
                initialize_worker_queues(extract_work_from_top_cluster(gss_stats.top_cluster));

                // set up threads
                std::vector<std::thread> supporting_threads{};
                supporting_threads.reserve(available_threads);

                for (uint64_t i = 1; i < available_threads; ++i)
                {
                    supporting_threads.emplace_back(
                        [&, ix = i]
                        {
                            worker& w = *workers.at(ix);

                            // keep unfolding on this thread until no more work exists
                            while (const std::optional<work_t>& work = w.obtain_work())
                            {
                                unfold_composition(w, work->get());
                            }
                        });
                }

                // keep unfolding on the main thread until no more work exists
                while (const std::optional<work_t>& work = workers.front()->obtain_work())
                {
                    unfold_composition(*workers.front(), work->get());
                }

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
    const uint64_t available_threads;
    /**
     * Vector containing all workers.
     */
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
    [[nodiscard]] static constexpr double get_projector_state_bound(const sidb_cluster_projector_state& pst,
                                                                    const uint64_t sidb_ix) noexcept
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
            return;
        }

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
    [[nodiscard]] static uint64_t
    find_cluster_of_maximum_size(const std::vector<sidb_cluster_projector_state_ptr>& proj_states) noexcept
    {
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
     * Before the parent projector state may be specialized to a specific composition of its children, first the
     * projections of the parent must be subtracted. The parent projector state is moved out and returned.
     *
     * @param clustering_state The clustering state from which the parent projector state should be taken out.
     * @param parent_pst_ix The index of the parent projector state in the given clustering state that should be taken
     * out.
     * @return The parent projector state that was taken out of the given clustering state.
     */
    [[nodiscard]] static sidb_cluster_projector_state_ptr take_parent_out(sidb_clustering_state& clustering_state,
                                                                          const uint64_t         parent_pst_ix) noexcept
    {
        // swap with last
        std::swap(clustering_state.proj_states.at(parent_pst_ix), clustering_state.proj_states.back());

        // move out
        sidb_cluster_projector_state_ptr parent_pst = std::move(clustering_state.proj_states.back());

        // pop
        clustering_state.proj_states.pop_back();

        // subtract parent potential from potential bounds store
        clustering_state.pot_bounds -= parent_pst->cluster->pot_projs_complete_store.at(parent_pst->multiset_conf);

        return parent_pst;
    }
    /**
     * After all specializations have been tried, the clustering state needs to un-specialize in order for other
     * specializations to take place later. This action undoes the action performed by the function above, adding the
     * given parent projector state and putting it back at the given index.
     *
     * @param clustering_state Clustering state to which the parent projector state should be added.
     * @param parent_pst_ix The index in the vector of projector states in the given clustering state at which the added
     * parent projector state should be placed.
     * @param parent_pst The parent projector state that needs to be added back to the given clustering state at the
     * given index.
     */
    static void add_parent(sidb_clustering_state& clustering_state, const uint64_t parent_pst_ix,
                           sidb_cluster_projector_state_ptr parent_pst) noexcept
    {
        // add parent potential from potential bounds store
        clustering_state.pot_bounds += parent_pst->cluster->pot_projs_complete_store.at(parent_pst->multiset_conf);

        // move back
        clustering_state.proj_states.emplace_back(std::move(parent_pst));

        // swap back
        std::swap(clustering_state.proj_states.back(), clustering_state.proj_states[parent_pst_ix]);
    }
    /**
     * A composition is added to the given clustering state, i.e., the projector states in the composition are added to
     * the clustering state and the potential bounds store is updated accordingly.
     *
     * @param clustering_state Clustering state to which the given composition should be added.
     * @param composition The composition that needs to be added to the given clustering state.
     */
    static void add_composition(sidb_clustering_state&               clustering_state,
                                const sidb_charge_space_composition& composition) noexcept
    {

        clustering_state.pot_bounds += composition.pot_bounds;

        for (const sidb_cluster_projector_state& child_pst : composition.proj_states)
        {
            // move child projector state in clustering state
            clustering_state.proj_states.emplace_back(std::make_unique<sidb_cluster_projector_state>(child_pst));
        }
    }
    /**
     * A composition is removed from the given clustering state, i.e., the projector states in the compositions are
     * removed from the clustering state and the potential bounds store is updated accordingly.
     *
     * @param clustering_state Clustering state from which the given composition should be removed.
     * @param composition The composition that needs to be removed from the given clustering state.
     */
    static void remove_composition(sidb_clustering_state&               clustering_state,
                                   const sidb_charge_space_composition& composition) noexcept
    {
        for (uint64_t i = 0; i < composition.proj_states.size(); ++i)
        {
            // handled child projector state --- remove
            clustering_state.proj_states.pop_back();
        }

        clustering_state.pot_bounds -= composition.pot_bounds;
    }
    /**
     * A work item is a constant reference to SiDB charge space composition.
     */
    using work_t = std::reference_wrapper<const sidb_charge_space_composition>;
    /**
     * A worker queue contains a double-layer queue of work items, a clustering state for thieves that want to steal
     * from the lowest layer of the queue, along with a queue of moles that tell how to transition this clustering state
     * for thieves to facilitate stealing from one layer to the next.
     */
    struct worker_queue
    {
        /**
         * A mole contains information on how to transition from one clustering state to a subsequent one.
         */
        struct mole
        {
            /**
             * The index of the cluster in the clustering state that is the selected parent cluster to unfold next. It
             * needs to be taken out in a clustering state transition.
             */
            uint64_t parent_to_move_out_ix;
            /**
             * The composition of the previously selected parent that fills the gap made by previously taking out this
             * selected parent. In a clustering state transition, first the composition is added (filling the gap made
             * by the previously selected parent), then the currently selected parent is taken out according to the
             * `parent_to_move_out_ix` above. This way, a work item may be unfolded as it fills the gap made by taking
             * out the currently selected parent. Thus, this work item becomes the `composition` value of the next mole
             * in line.
             */
            const sidb_charge_space_composition& composition;
        };
        /**
         * The clustering state for thieves, which enables thieves to join in and steal work from the bottom of the
         * queue, while the owner of this queue will take items from the top of the queue.
         */
        sidb_clustering_state clustering_state_for_thieves;
        /**
         * The queue of moles. For each transition between layers of the double layer work queue below, there is an
         * associated mole which informs how the transition takes place. This way, the clustering state for thieves can
         * be dynamically updated through forward-tracking (opposite of backtracking).
         */
        std::deque<mole> thief_informants{};
        /**
         * Double layer queue of work items. Each layer corresponds with a clustering state that needs to be used to
         * unfold the items in that layer. The clustering states of subsequent layers are each one informant application
         * apart.
         */
        std::deque<std::deque<work_t>> queue;
        /**
         * Counter to keep track of the total amount of work in the double-layer work queue.
         */
        uint64_t work_in_queue_count{0};
        /**
         * Mutex used to protect shared resources in this queue.
         */
        std::mutex mutex_to_protect_this_queue;
        /**
         * Standard constructor.
         *
         * @param num_sidbs_in_layout The number of SiDBs in the layout to simulate. Required for initializing
         * clustering states.
         */
        explicit worker_queue(const uint64_t num_sidbs_in_layout) noexcept :
                clustering_state_for_thieves{num_sidbs_in_layout}
        {}
        /**
         * Initializes this queue with stolen work. The work itself is kept on the stack.
         */
        void initialize_queue_after_stealing(const sidb_clustering_state& clustering_state) noexcept
        {
            const std::lock_guard lock(mutex_to_protect_this_queue);

            clustering_state_for_thieves = sidb_clustering_state{clustering_state};

            thief_informants.clear();

            queue.clear();
            queue.emplace_front();

            work_in_queue_count = 0;
        }
        /**
         * A mole is popped from the queue which says which composition to add to the clustering state for thieves, and
         * which cluster is selected for the subsequent unfolding, which should then be taken out.
         */
        void apply_informant() noexcept
        {
            mole informant = std::move(thief_informants.front());
            thief_informants.pop_front();

            add_composition(clustering_state_for_thieves, informant.composition);

            take_parent_out(clustering_state_for_thieves, informant.parent_to_move_out_ix);
        }
        /**
         * Called during backtracking to descend to the previous layer of the queue, along with popping the unnecessary
         * mole.
         */
        void pop_last_layer() noexcept
        {
            const std::lock_guard lock{mutex_to_protect_this_queue};

            if (thief_informants.empty())
            {
                return;
            }

            queue.pop_front();

            thief_informants.pop_back();
        }
        /**
         * Adds a vector of work items to the queue, along with adding an informant that allows for a dynamic update of
         * the clustering state for thieves to assume one of the work items that are added to the queue.
         *
         * @param compositions Vector of work items.
         * @param informant A mole providing the required information to update the clustering state for thieves to
         * enable forward-tracking. The mole says which composition to add to the clustering state, and which cluster is
         * selected for the subsequent unfolding.
         */
        void add_to_queue(const std::vector<sidb_charge_space_composition>& compositions, mole&& informant) noexcept
        {
            const std::lock_guard lock{mutex_to_protect_this_queue};

            queue.emplace_front();

            // add all composition except first to queue
            for (uint64_t i = 1; i < compositions.size(); ++i)
            {
                queue.front().emplace_front(std::ref(compositions.at(i)));
            }

            work_in_queue_count += compositions.size() - 1;

            // add informant
            thief_informants.emplace_back(std::move(informant));

            assert(queue.empty() || queue.size() == thief_informants.size() + 1);
        }
        /**
         * Own work is obtained in a blocking fashion. If there is no more work in the queue, `false` is returned to
         * indicate no backtracking is necessary, since there is no follow-up work item to backtrack towards.
         *
         * @return Either work if there is work left to do on the current level---i.e., for the current clustering state
         * of the worker that calls this function---or `true` if this not the case and backtracking is required in order
         * to do more work that is in this queue, or `false` when there is no more such work and thus backtracking can
         * be skipped.
         */
        [[nodiscard]] std::variant<work_t, bool> get_from_this_queue() noexcept
        {
            const std::lock_guard lock{mutex_to_protect_this_queue};

            if (work_in_queue_count == 0)
            {
                // no more work, no need to backtrack
                return false;
            }

            if (queue.front().empty())
            {
                // there is work, but not on this level anymore so backtracking is required
                return true;
            }

            // obtaining own work goes from the front
            work_t work = queue.front().front();
            queue.front().pop_front();

            --work_in_queue_count;

            return work;
        }
        /**
         * Attempt to steal work from this queue in a non-blocking fashion. When a lock is acquired, forward-tracking is
         * applied to dynamically update the clustering state for thieves to where it can be copied for a thief that
         * steals the last work item in this queue.
         *
         * @return Either `true` when the queue is locked, `false` when there is no work in this queue, or a pair of a
         * copy of the updated (forward-tracked) clustering state for thieves along with the corresponding work item.
         */
        [[nodiscard]] std::variant<bool, std::pair<sidb_clustering_state, work_t>> try_steal_from_this_queue() noexcept
        {
            const std::unique_lock lock{mutex_to_protect_this_queue, std::try_to_lock};

            if (!lock.owns_lock())
            {
                // non-blocking lock attempt failed
                return true;
            }

            // acquired lock

            if (work_in_queue_count == 0)
            {
                // nothing to steal
                return false;
            }

            // apply forward-tracking (opposite of backtracking) to update the clustering state for thieves such that it
            // enables the next available work to be stolen
            while (!thief_informants.empty() && queue.back().empty())
            {
                queue.pop_back();

                apply_informant();
            }

            // stealing goes from the back
            work_t work = queue.back().back();
            queue.back().pop_back();

            --work_in_queue_count;

            // make copy
            sidb_clustering_state stolen_clustering_state{clustering_state_for_thieves};

            return std::pair{std::move(stolen_clustering_state), work};
        }
    };
    /**
     * Each thread has a unique worker object with its own dynamic state and queue of work that it generated. When it
     * has no work of its own, it will steal work from another worker.
     */
    struct worker
    {
        /**
         * Worker index in the vector of all workers.
         */
        const uint64_t index;
        /**
         * This worker's queue where work can be obtained from either by this worker or by others (work stealing).
         */
        worker_queue work_stealing_queue;
        /**
         * This worker's current state, consisting of a clustering where each cluster has an assigned multiset charge
         * configuration, and a store containing lower and upper bounds on the local potential for each SiDB under this
         * multiset charge configuration assignment.
         */
        sidb_clustering_state clustering_state;
        /**
         * The vector of all workers where this worker is at `ix`.
         */
        const std::vector<std::unique_ptr<worker>>& all_workers;
        /**
         * Standard constructor.
         *
         * @param ix Worker index in the vector of all workers.
         * @param num_sidbs The number of SiDBs in the layout to simulate.
         * @param workers The vector of all workers where this worker is at `ix`.
         */
        explicit worker(const uint64_t ix, const uint64_t num_sidbs,
                        const std::vector<std::unique_ptr<worker>>& workers) noexcept :
                index{ix},
                work_stealing_queue{num_sidbs},
                clustering_state{num_sidbs},
                all_workers{workers}
        {}
        /**
         * Obtains work for this worker, either from their own queue, or else from another worker's queue (work
         * stealing).
         *
         * @return Either nothing, when no work was found and this thread can thus terminate, or that was obtained.
         */
        [[nodiscard]] std::optional<work_t> obtain_work() noexcept
        {
            if (const std::variant<work_t, bool>& work = work_stealing_queue.get_from_this_queue();
                std::holds_alternative<work_t>(work))
            {
                return std::get<work_t>(work);
            }

            // no own work found, look for work until all threads have no work in their queue and are not busy
            bool encountered_locked_queue = true;

            while (encountered_locked_queue)
            {
                encountered_locked_queue = false;

                for (uint64_t i = 0; i < all_workers.size(); ++i)
                {
                    if (i == index)
                    {
                        // skip own queue
                        continue;
                    }

                    std::variant<bool, std::pair<sidb_clustering_state, work_t>> work =
                        all_workers.at(i)->work_stealing_queue.try_steal_from_this_queue();

                    if (!std::holds_alternative<bool>(work))
                    {
                        clustering_state = std::move(std::get<std::pair<sidb_clustering_state, work_t>>(work).first);

                        work_stealing_queue.initialize_queue_after_stealing(clustering_state);

                        return std::get<std::pair<sidb_clustering_state, work_t>>(work).second;
                    }

                    encountered_locked_queue |= std::get<bool>(work);
                }
            }

            // no more work -> terminate thread
            return std::nullopt;
        }
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
                    if (const auto& [cell, defect] = cd; defect.type != sidb_defect_type::NONE)
                    {
                        cds.add_sidb_defect_to_potential_landscape(cell, lyt.get_sidb_defect(cell));
                    }
                });
        }

        cds.assign_local_external_potential(params.local_external_potential);
        cds.assign_global_external_potential(params.global_potential);

        return cds;
    }
    /**
     * Work in the form of compositions of charge space elements of the top cluster are extracted into a vector and
     * shuffled at random before being returned.
     *
     * @param top_cluster The top cluster that is returned by running the *Ground State Space* construction.
     * @return A vector containing all work contained by the top cluster in random order.
     */
    [[nodiscard]] static std::vector<work_t> extract_work_from_top_cluster(const sidb_cluster_ptr& top_cluster) noexcept
    {
        std::vector<work_t> work_from_top_cluster{};

        for (const sidb_cluster_charge_state& ccs : top_cluster->charge_space)
        {
            for (const sidb_charge_space_composition& composition : ccs.compositions)
            {
                work_from_top_cluster.emplace_back(std::ref(composition));
            }
        }

        std::shuffle(work_from_top_cluster.begin(), work_from_top_cluster.end(),
                     std::mt19937_64{std::random_device{}()});

        return work_from_top_cluster;
    }
    /**
     * Initializes the worker queues with work from the top cluster, dividing it evenly over the available threads.
     *
     * @param work_from_top_cluster A vector containing all compositions of all charge space elements of the top
     * cluster.
     */
    void initialize_worker_queues(const std::vector<work_t>& work_from_top_cluster) noexcept
    {
        const uint64_t num_threads_with_initial_work =
            std::min(available_threads, uint64_t{work_from_top_cluster.size()});

        // divide the work into sections for each thread to handle
        std::vector<std::pair<uint64_t, uint64_t>> ranges{};
        ranges.reserve(available_threads);

        const uint64_t chunk_size = work_from_top_cluster.size() / num_threads_with_initial_work;
        uint64_t       start      = 0;
        uint64_t       end        = chunk_size - 1;

        for (uint64_t i = 0; i < num_threads_with_initial_work; ++i)
        {
            ranges.emplace_back(start, end);
            start = end + 1;
            end   = i == num_threads_with_initial_work - 2 ? work_from_top_cluster.size() - 1 : start + chunk_size - 1;
        }

        workers.reserve(available_threads);

        // for each worker, add work to the queue from the respectively assigned section
        for (uint64_t i = 0; i < num_threads_with_initial_work; ++i)
        {
            std::unique_ptr<worker> w = std::make_unique<worker>(i, charge_layout.num_cells(), workers);

            w->work_stealing_queue.queue.emplace_front();

            const auto& [i_start, i_end] = ranges.at(i);

            // fill queue with work
            for (uint64_t work_ix = i_start; work_ix <= i_end; ++work_ix)
            {
                w->work_stealing_queue.queue.front().emplace_front(work_from_top_cluster.at(work_ix));
            }

            w->work_stealing_queue.work_in_queue_count = w->work_stealing_queue.queue.front().size();

            workers.emplace_back(std::move(w));
        }

        // initialize each worker that did not get initial work as thieves
        for (uint64_t thread_ix = 0; thread_ix < available_threads - num_threads_with_initial_work; ++thread_ix)
        {
            workers.emplace_back(std::make_unique<worker>(thread_ix, charge_layout.num_cells(), workers));
        }
    }
    /**
     * This recursive function is the heart of the *ClusterComplete* destruction. The given clustering state is
     * dissected at the largest cluster to each possible specialization of it, which then enters the recursive call
     * with the clustering state modified to have a set of sibling children replacing their direct parent. For each
     * specialization, appropriate updates are made to the potential bounds store that is part of the clustering
     * state. After a specialization has been handled completely, i.e., when the recursive call for this
     * specialization returns, the specialization to the potential bounds store is undone so that a new
     * specialization may be applied.
     *
     * The two base cases to the recursion are as follows: (1) the charge distributions implied by the given
     * clustering state do not meet the population stability, meaning that this branch of the search space may be
     * pruned through terminating the recursion at this level, and, (2) the clustering state hold only singleton
     * clusters and passes the population stability check. In the latter case, the configuration stability check is
     * performed before the associated charge distribution is added to the simulation results.
     *
     * @param w The worker running on the current thread. It has a clustering state that holds a specific combination of
     * multiset charge configurations as projector states of which the respectively associated clusters form a
     * clustering in the cluster hierarchy.
     * @param composition To enable dynamic updates of the clustering states that thieves can assume, the composition
     * that the current worker's clustering state is specialized to is to be stored in the worker's queue if further
     * compositions are to be unfolded.
     * @return `false` if and only if queue of this worker is found to be completely empty and thus backtracking is
     * not required.
     */
    [[nodiscard]] bool
    add_physically_valid_charge_configurations(worker& w, const sidb_charge_space_composition& composition) noexcept
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

        // un-apply max_pst, thereby making space for specialization
        sidb_cluster_projector_state_ptr max_pst = take_parent_out(w.clustering_state, max_pst_ix);

        // unfold all compositions
        if (!unfold_all_compositions(w, get_projector_state_compositions(*max_pst),
                                     typename worker_queue::mole{max_pst_ix, composition}))
        {
            return false;
        }

        // apply max_pst back
        add_parent(w.clustering_state, max_pst_ix, std::move(max_pst));

        w.work_stealing_queue.pop_last_layer();

        return true;
    }
    /**
     * After a cluster in a clustering state was chosen to be unfolded next, the unfolding is performed through ensuring
     * that all compositions of the multiset associated with the cluster in the clustering state are each unfolded. The
     * current worker will always unfold the first composition, while putting the other compositions in its queue such
     * that threads without work may steal those if the current worker is still working on this first composition.
     *
     * @param w The worker running on the current thread.
     * @param compositions A vector containing all compositions to unfold.
     * @param informant For other workers to be able to unfold one of those compositions that are not being unfolded
     * yet, they need to obtain the right clustering state. The informant adds to the required information to
     * dynamically update the clustering state for other workers looking to steal work.
     * @return `false` if and only if the queue of this worker is found to be completely empty and thus backtracking is
     * not required.
     */
    [[nodiscard]] bool unfold_all_compositions(worker&                                           w,
                                               const std::vector<sidb_charge_space_composition>& compositions,
                                               typename worker_queue::mole&&                     informant) noexcept
    {
        w.work_stealing_queue.add_to_queue(compositions, std::move(informant));

        // unfold first composition
        unfold_composition(w, compositions.front());

        // unfold other compositions while there are ones left on this level to unfold
        std::variant<work_t, bool> work = w.work_stealing_queue.get_from_this_queue();

        while (std::holds_alternative<work_t>(work))
        {
            if (!unfold_composition(w, std::get<work_t>(work).get()))
            {
                // continue walking back up the stack without backtracking
                return false;
            }

            work = w.work_stealing_queue.get_from_this_queue();
        }

        return std::get<bool>(work);
    }
    /**
     * The clustering state of the current worker is specialized according to the given composition preceding the
     * recursion. If there is still work left to do by this worker, backtracking is performed, for which also the
     * aforementioned specialization needs to be undone.
     *
     * @param w The worker running on the current thread.
     * @param composition The composition to unfold.
     */
    [[nodiscard]] bool unfold_composition(worker& w, const sidb_charge_space_composition& composition) noexcept
    {
        // specialize parent to a specific composition of its children
        add_composition(w.clustering_state, composition);

        // recurse with specialized composition
        if (add_physically_valid_charge_configurations(w, composition))
        {
            // undo specialization such that the specialization may consider a different children composition
            remove_composition(w.clustering_state, composition);

            return true;
        }

        return false;
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
 * the key ingredient to the achieved efficiency: the *Ground State Space* algorithm, which constructs a minimized
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
