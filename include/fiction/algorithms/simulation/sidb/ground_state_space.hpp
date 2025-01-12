//
// Created by Willem Lambooy on 06.02.2024.
//

#ifndef FICTION_GROUND_STATE_SPACE_HPP
#define FICTION_GROUND_STATE_SPACE_HPP

#if (FICTION_ALGLIB_ENABLED)

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_cluster_hierarchy.hpp"

#include <btree.h>
#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * The set of parameters used in the *Ground State Space* construction.
 */
struct ground_state_space_params
{
    /**
     * The physical parameters that *Ground State Space* will use to prune simulation search space.
     */
    const sidb_simulation_parameters simulation_parameters{};
    /**
     * This specifies the maximum cluster size for which *Ground State Space* will solve an NP-complete sub-problem
     * exhaustively. The sets of SiDBs that witness local population stability for each respective charge state may be
     * partitioned into disjoint sets such that the number of required witnesses for each respective charge state is
     * satisfied. If no such partition exists, the multiset charge configuration associated with the requirements may be
     * rejected. The defaulted value is chosen such that some extra pruning may be performed, while the impact on the
     * runtime remains negligible. Validity witness partitioning parameters are relevant for large simulation problems.
     */
    uint64_t witness_partitioning_cluster_size_limit = 12;
    /**
     * The complexity is of validity witness partitioning bounded by a factorial in the number of overlapping witnesses.
     * This parameter thus allows the validity witness partitioning procedure to perform the reduction to overlapping
     * witnesses for larger cluster sizes that could be runtime-impairing, then limiting specifically the length of the
     * input to the factorial call. As above, the defaulted value ensures no hindrance in runtimes.
     */
    uint64_t num_overlapping_witnesses_limit_gss = 6;
};
/**
 * This struct is used to store the results of the *Ground State Space* construction.
 */
struct ground_state_space_results
{
    /**
     * The top cluster is the root of the cluster hierarchy. It therefore allows access to the entire cluster hierarchy,
     * including the charge spaces of each cluster.
     */
    const sidb_cluster_ptr top_cluster{nullptr};
    /**
     * The runtime of the construction is stored.
     */
    const std::chrono::duration<double> runtime{};
    /**
     * The maximum size of the charge space of the top cluster, given the simulation base, can be inferred by the
     * \"stars and bars\" combinatorial idea: the solution to this analogous problem determines the maximum amount of
     * multisets of size \f$N\f$ (where \f$N\f$ is the number of SiDBs in the layout, and therefore in the top cluster)
     * for the given base \f$b\f$. In particular, the analogy is as follows: any such multiset can be seen as \f$N\f$
     * stars and \f$b - 1\f$ bars separating those stars. Then, the \f$b\f$ partitions forming from these \f$b - 1\f$
     * separators each have a respective size, adding up to \f$N\f$. Therefore each partition is associated with an
     * amount of one of the charge states of the multiset. Now we may compute total number of possible multisets for the
     * top cluster as the number of combinations of \f$N\f$ stars and \f$b - 1\f$ bars. Hence this is computed with the
     * following combinatorial formula: \f$\binom{N + b - 1}{b - 1}\f$.
     */
    const uint64_t maximum_top_level_multisets{};
    /**
     * The total number of distinct projector states is counted. At each merge, the projector states in charge space
     * compositions in the charge spaces of the clusters to merge are locked in the final construct, and can therefore
     * be counted. This may be used to estimate the time it would take *ClusterComplete* to unfold the hierarchy.
     */
    const uint64_t projector_state_count{};
    /**
     * Report *Ground State Space* statistics. A quick heuristic to assess the quality of the pruning is captured by the
     * size of the charge space of the top cluster, which depends on the charge spaces of all clusters below it.
     *
     * @param os The output stream to write to (default: standard output).
     * @return Prints the runtime and the number of pruned top level multisets versus the total amount possible.
     */
    void report(std::ostream& os = std::cout) const noexcept
    {
        os << fmt::format("[i] Leaving {} out of {} top level multiset charge configurations\n",
                          top_cluster ? top_cluster->charge_space.size() : 0, maximum_top_level_multisets);

        os << fmt::format("[i] There are {} projector states in the constructed hierarchy\n", projector_state_count);

        const double gss_runtime = mockturtle::to_seconds(runtime);
        os << fmt::format("[i] Ground State Space took {:.4f} {}seconds",
                          gss_runtime > 1.0 ? gss_runtime : gss_runtime * 1000, gss_runtime > 1.0 ? "" : "milli")
           << std::endl;
    }
};

namespace detail
{

template <typename Lyt>
class ground_state_space_impl
{
  public:
    /**
     * Constructor. Invokes the algorithm with the given parameters on the given layout.
     *
     * @param lyt Layout to construct the *Ground State Space* of.
     * @param parameters The parameters that *Ground State Space* will use throughout the construction.
     */
    explicit ground_state_space_impl(const Lyt& lyt, const ground_state_space_params parameters) noexcept :
            params{parameters},
            top_cluster{to_sidb_cluster(sidb_cluster_hierarchy(lyt))},
            clustering{
                get_initial_clustering(top_cluster, get_local_potential_bounds(lyt, params.simulation_parameters))},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - params.simulation_parameters.mu_plus()}
    {}
    /**
     * The main loop in the *Ground State Space* construction. Charge spaces are updated until a fixed point is reached,
     * after which a merging of children to their direct parent is performed. This process repeats until the parent to
     * which the children are merged is the top cluster, the cluster containing all SiDBs in the layout. The Ground
     * State Space is the resulting hierarchical charge space structure.
     *
     * @return The results of the construction, which include the top cluster which parents all other clusters, and
     * thereby contains the charge spaces of each cluster.
     */
    ground_state_space_results run() noexcept
    {
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            while (!terminate)
            {
                while (!update_charge_spaces())
                {}

                move_up_hierarchy();
            }
        }

        const uint64_t max_multisets = maximum_top_level_multisets(top_cluster->num_sidbs());

        return ground_state_space_results{top_cluster, time_counter, max_multisets, projector_state_count};
    }

  private:
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
     * Function used to initialize two charge distribution surfaces, each corresponding to the initial lower/upper bound
     * on the electrostatic potential in the layout. The minimum electrostatic potential depends on the given simulation
     * base: when positive charges are considered, there may be negative electrostatic potential.
     *
     * @param lyt Layout to construct the *Ground State Space* of.
     * @param simulation_parameters Parameters used to calculate the electrostatic potential in the layout.
     */
    static std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>
    get_local_potential_bounds(const Lyt& lyt, const sidb_simulation_parameters& simulation_parameters) noexcept
    {
        charge_distribution_surface<Lyt> cds_min{lyt};
        charge_distribution_surface<Lyt> cds_max{lyt};

        cds_min.assign_physical_parameters(simulation_parameters);
        cds_max.assign_physical_parameters(simulation_parameters);

        cds_min.assign_all_charge_states(simulation_parameters.base == 3 ? sidb_charge_state::POSITIVE :
                                                                           sidb_charge_state::NEUTRAL);
        cds_max.assign_all_charge_states(sidb_charge_state::NEGATIVE);

        cds_min.update_after_charge_change();
        cds_max.update_after_charge_change();

        return {cds_min, cds_max};
    }
    /**
     * Recursive function used to get the initial clustering; the data object that collects all SiDBs in the layout that
     * are each individually lifted to (singleton) SiDB cluster objects. Each SiDB cluster object has information of its
     * electrostatic potential effect onto each SiDB in the layout, as well as information on the accumulated
     * electrostatic potential effect that each SiDB has on it.
     *
     * This function is initially called with the top cluster, i.e., the cluster containing all SiDBs in the layout. The
     * function is called recursively on each of the children of this cluster until the function is called on a cluster
     * without children. The aforementioned electrostatic potential information with respect to this singleton cluster
     * is then computed, and it is added to the clustering that eventually contains all singleton clusters, which is
     * then returned.
     *
     * @param c Cluster to add to the clustering if it is a singleton, otherwise this function is called recursively on
     * each of its children.
     * @param local_potential_bound_containers Charge distribution surface objects containing information on the initial
     * minimum and maximum electrostatic potential.
     * @return The clustering that contains only singleton clusters, one for each SiDB in the layout.
     */
    static sidb_clustering
    get_initial_clustering(const sidb_cluster_ptr& c,
                           const std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>&
                               local_potential_bound_containers) noexcept
    {
        const auto& [min_loc_pot_cds, max_loc_pot_cds] = local_potential_bound_containers;

        sidb_clustering clustering{};

        if (c->children.empty())
        {
            const uint64_t i = get_singleton_sidb_ix(c);

            const cell<Lyt>& sidb = min_loc_pot_cds.index_to_cell(i);

            // separate the local potential into potential from SiDBs and external sources
            const double loc_ext_pot = min_loc_pot_cds.get_local_defect_potentials()[sidb] +
                                       min_loc_pot_cds.get_local_external_potentials()[sidb];

            const double min_loc_pot = min_loc_pot_cds.get_local_potential_by_index(i).value() - loc_ext_pot;
            const double max_loc_pot = max_loc_pot_cds.get_local_potential_by_index(i).value() - loc_ext_pot;

            c->initialize_singleton_cluster_charge_space(-min_loc_pot, -max_loc_pot, -loc_ext_pot,
                                                         min_loc_pot_cds.get_simulation_params().base, c);

            c->pot_projs[i] =
                potential_projection_order{-loc_ext_pot, min_loc_pot_cds.get_simulation_params().base, true};

            for (uint64_t j = 0; j < min_loc_pot_cds.num_cells(); ++j)
            {
                if (j != i)
                {
                    c->pot_projs[j] =
                        potential_projection_order{min_loc_pot_cds.get_chargeless_potential_by_indices(i, j),
                                                   min_loc_pot_cds.get_simulation_params().base};
                }
            }

            clustering.emplace(c);
        }

        for (const sidb_cluster_ptr& child : c->children)
        {
            const sidb_clustering& child_clustering = get_initial_clustering(child, local_potential_bound_containers);
            clustering.insert(child_clustering.cbegin(), child_clustering.cend());
        }

        return clustering;
    }
    /**
     * Helper function for obtaining the stored lower or upper bound on the electrostatic potential that SiDBs in the
     * given cluster collectively project onto the given SiDB.
     *
     * @tparam bound Bound to obtain (lower/upper).
     * @param c Projecting cluster.
     * @param sidb_ix Receiving SiDB.
     * @return The potential projection associated with this bound; i.e., an electrostatic potential (in V) associated
     * with a multiset charge configuration of the given cluster.
     */
    template <bound_direction bound>
    static constexpr inline potential_projection get_projection_bound(const sidb_cluster_ptr& c,
                                                                      const uint64_t          sidb_ix) noexcept
    {
        return c->pot_projs.at(sidb_ix).get_bound<bound>();
    }
    /**
     * Helper function for obtaining the stored one-above lower or one-below upper bound on the electrostatic potential
     * that SiDBs in the given cluster collectively project onto the given SiDB.
     *
     * @tparam bound Bound to obtain (one-above lower / one-below upper).
     * @param c Projecting cluster.
     * @param sidb_ix Receiving SiDB.
     * @return The potential projection associated with this bound; i.e., an electrostatic potential (in V) associated
     * with a multiset charge configuration of the given cluster.
     */
    template <bound_direction bound>
    static constexpr inline double get_next_projected_pot_bound(const sidb_cluster_ptr& c,
                                                                const uint64_t          sidb_ix) noexcept
    {
        return c->pot_projs.at(sidb_ix).get_next_bound<bound>().pot_val;
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
    static constexpr inline potential_projection get_projector_state_bound(const sidb_cluster_projector_state& pst,
                                                                           const uint64_t sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf);
    }
    /**
     * Helper function for adding the given potential projection to the store of all potential projections onto the
     * given receiving SiDB. This store only contains multiset charge configurations (that constitute the respective
     * potential projections in this store) that have not yet been judged as physically invalid.
     *
     * @param c Cluster of which the associated given potential projection `pp` onto `sidb_ix` is to be added to its
     * store of possible potential projections onto this SiDB.
     * @param sidb_ix SiDB that receives the given potential projection from `c`.
     * @param pp Potential projection from `c` to `sidb_ix` to add.
     */
    static constexpr inline void add_pot_projection(const sidb_cluster_ptr& c, const uint64_t sidb_ix,
                                                    const potential_projection& pp) noexcept
    {
        c->pot_projs[sidb_ix].add(pp);
    }
    /**
     * Helper function to remove all occurrences of a cluster charge state--i.e., a multiset charge configuration--that
     * is part of the given projector state from the potential projections from the associated cluster onto the given
     * SiDB.
     *
     * @param rm_pst Projector state to move all occurrences of in the projection onto `sidb_ix`.
     * @param sidb_ix SiDB that receives the potential projections to be removed.
     */
    static constexpr inline void remove_all_cluster_charge_state_occurrences(const sidb_cluster_projector_state& rm_pst,
                                                                             const uint64_t sidb_ix) noexcept
    {
        rm_pst.cluster->pot_projs[sidb_ix].remove_m_conf(rm_pst.multiset_conf);
    }
    /**
     * When the multiset charge configuration associated with the given projector state represents a bound, then the
     * one-above lower or one-below upper bound should be used instead in the accumulation of potential projections
     * received by the given receptor state.
     *
     * @tparam bound Bound to check (lower/upper).
     * @param pst Projector state that could be associated with a bound on the potential projected from the associated
     * cluster onto the SiDB in the given receptor state.
     * @param rst Receptor state that contains the cluster object at which the accumulation of externally received
     * potentials may need to be updated, and the relevant SiDB that receives the potential projection.
     */
    template <bound_direction bound>
    static constexpr void
    update_external_pot_projection_if_bound_removed(const sidb_cluster_projector_state& pst,
                                                    const sidb_cluster_receptor_state&  rst) noexcept
    {
        const potential_projection& cur_bound = get_projection_bound<bound>(pst.cluster, rst.sidb_ix);

        if (cur_bound.multiset == pst.multiset_conf)
        {
            // update the received potential with the difference between the removed bound and the next bound
            rst.cluster->received_ext_pot_bounds.update<bound>(
                rst.sidb_ix, get_next_projected_pot_bound<bound>(pst.cluster, rst.sidb_ix) - cur_bound.pot_val);
        }
    }
    /**
     * For a given projector state and receptor state, i.e., an interaction between two clusters with an associated
     * multiset charge configuration of the projecting cluster and an SiDB in the receiving cluster, this multiset
     * charge configuration is to be removed. Corresponding updates are made when it represented a bound, such that the
     * potential projection range shrinks, perhaps leading to more sub-configurations detected to be physically invalid.
     *
     * @param pst Projector state of which the associated multiset charge configuration is to be removed.
     * @param rst Receptor state at which the updates to the accumulation of externally received potential should be
     * made when necessary.
     */
    constexpr void update_external_potential_projection(const sidb_cluster_projector_state& pst,
                                                        const sidb_cluster_receptor_state&  rst) const noexcept
    {
        update_external_pot_projection_if_bound_removed<bound_direction::LOWER>(pst, rst);
        update_external_pot_projection_if_bound_removed<bound_direction::UPPER>(pst, rst);

        // remove projection
        remove_all_cluster_charge_state_occurrences(pst, rst.sidb_ix);
    }
    /**
     * When a multiset charge configuration is found to be invalid for an associated cluster, this has an effect on each
     * SiDB in each other cluster. This function invokes the procedures that perform updates where necessary.
     *
     * @param pst Projector state of which the multiset charge configuration is to be purged from the cluster hierarchy
     * data structure.
     */
    void handle_invalid_state(const sidb_cluster_projector_state& pst) noexcept
    {
        // pruned multiset---update projections onto each other cluster (which respectively update their received store)
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == pst.cluster)
            {
                continue;
            }

            for (const uint64_t sidb_ix : other_c->sidbs)
            {
                update_external_potential_projection(pst, sidb_cluster_receptor_state{other_c, sidb_ix});
            }
        }
    }
    /**
     * The witness partitioning state is used to collect which SiDBs are witness of (i.e., "accept") which charge state.
     * After free witnesses are accounted for, a permutation problem is left: can the witnesses be partitioned in
     * such a way that there are enough witnesses for each charge state? When no such witness partitioning exists, the
     * given multiset charge configuration is invalid. This problem can quickly become very difficult for larger
     * instances; therefore the problem instance size is limited by corresponding parameters.
     */
    struct witness_partitioning_state
    {
        /**
         * A witness set is a set of unsigned integers representing SiDBs.
         */
        using witness_set = phmap::btree_set<uint64_t>;
        /**
         * For each charge state there is a set of witnesses for this charge state.
         */
        witness_set negative_witnesses{}, positive_witnesses{}, neutral_witnesses{};
        /**
         * The given multiset charge configuration determines how many witness are required for each charge state.
         */
        uint64_t required_neg_count, required_pos_count, required_neut_count;
        /**
         * Constructor. Converts the multiset charge configuration into requirements for each charge state.
         */
        explicit witness_partitioning_state(const sidb_cluster_projector_state& pst) noexcept :
                required_neg_count{pst.get_count<sidb_charge_state::NEGATIVE>()},
                required_pos_count{pst.get_count<sidb_charge_state::POSITIVE>()},
                required_neut_count{pst.get_count<sidb_charge_state::NEUTRAL>()}
        {}
        /**
         * This function finds the free witnesses to reduce the problem, leaving only witnesses to partition that
         * overlap.
         *
         * @return The number of overlapping witnesses that determines the resulting problem's complexity.
         */
        uint64_t omit_free_witnesses_and_count_overlap() noexcept
        {
            witness_set overlap{};

            for (const auto& [this_set, competing_sets] :
                 std::array{std::make_pair(std::make_pair(std::ref(negative_witnesses), std::ref(required_neg_count)),
                                           std::make_pair(std::ref(positive_witnesses), std::ref(neutral_witnesses))),
                            std::make_pair(std::make_pair(std::ref(positive_witnesses), std::ref(required_pos_count)),
                                           std::make_pair(std::ref(negative_witnesses), std::ref(neutral_witnesses))),
                            std::make_pair(std::make_pair(std::ref(neutral_witnesses), std::ref(required_neut_count)),
                                           std::make_pair(std::ref(negative_witnesses), std::ref(positive_witnesses)))})
            {
                for (witness_set::const_iterator it = this_set.first.cbegin(); it != this_set.first.cend();)
                {
                    if (competing_sets.first.find(*it) == competing_sets.first.cend() &&
                        competing_sets.second.find(*it) == competing_sets.second.cend())
                    {
                        it = this_set.first.erase(it);
                        this_set.second--;
                        continue;
                    }

                    overlap.emplace(*it);

                    ++it;
                }
            }

            return overlap.size();
        }
    };
    /**
     * A simple brute-force algorithm that solves the validity witness partitioning problem by looking for a
     * partitioning, i.e., an assignment that maps each SiDB to a charge state that they are a witness of in such a way
     * that required witness counts are met for each SiDB. Starting with the negative charge state, the requirement is
     * fulfilled in a greedy manner by taking SiDBs witnessing this charge state. This process continues for the other
     * charge states in the same way, though when an SiDB has already been assigned, it cannot be used to reduce the
     * required witness count again. When all SiDBs have been assigned successfully, a partitioning is found and `true`
     * is returned. Otherwise, when an in-progress assignment cannot be completed, the next assignment is tried through
     * back-tracking. When no assignment can be completed, `false` is returned, which then proves that the multiset
     * charge configuration that was given to create the witness partitioning state is invalid.
     *
     * @tparam current_fill_cs The charge state for which the function currently looks for available witnesses until no
     * more witnesses are required.
     * @param st The witness partitioning state, which holds the required witnesses counters for each charge state.
     * @param num_witnesses_for_current_cs The number of available witnesses that are left for `current_fill_cs`.
     * @return `true` if a validity witness partitioning was found, and `false` if no consistent partitioning was found.
     */
    template <sidb_charge_state current_fill_cs>
    static bool find_valid_witness_partitioning(witness_partitioning_state& st,
                                                const uint64_t              num_witnesses_for_current_cs) noexcept
    {
        if constexpr (current_fill_cs == sidb_charge_state::NEUTRAL)
        {
            return st.neutral_witnesses.size() >= num_witnesses_for_current_cs;
        }
        else if constexpr (current_fill_cs == sidb_charge_state::POSITIVE)
        {
            if (num_witnesses_for_current_cs == 0)
            {
                return find_valid_witness_partitioning<sidb_charge_state::NEUTRAL>(st, st.required_neut_count);
            }

            for (typename witness_partitioning_state::witness_set::const_iterator it = st.positive_witnesses.cbegin();
                 it != st.positive_witnesses.cend();)
            {
                const uint64_t take_witness = *st.positive_witnesses.cbegin();

                st.positive_witnesses.erase(it);

                st.neutral_witnesses.erase(take_witness);

                if (find_valid_witness_partitioning<sidb_charge_state::POSITIVE>(st, num_witnesses_for_current_cs - 1))
                {
                    return true;
                }

                it = ++st.positive_witnesses.emplace(take_witness).first;

                st.neutral_witnesses.emplace(take_witness);
            }

            return false;
        }
        else if constexpr (current_fill_cs == sidb_charge_state::NEGATIVE)
        {
            if (num_witnesses_for_current_cs == 0)
            {
                return find_valid_witness_partitioning<sidb_charge_state::POSITIVE>(st, st.required_pos_count);
            }

            for (typename witness_partitioning_state::witness_set::const_iterator it = st.negative_witnesses.cbegin();
                 it != st.negative_witnesses.cend();)
            {
                const uint64_t take_witness = *it;

                st.negative_witnesses.erase(it);

                st.positive_witnesses.erase(take_witness);
                st.neutral_witnesses.erase(take_witness);

                if (find_valid_witness_partitioning<sidb_charge_state::NEGATIVE>(st, num_witnesses_for_current_cs - 1))
                {
                    return true;
                }

                it = ++st.negative_witnesses.emplace(take_witness).first;

                st.positive_witnesses.emplace(take_witness);
                st.neutral_witnesses.emplace(take_witness);
            }

            return false;
        }
    }
    /**
     * A switch to enable the potential bound analysis function to analyse both multiset charge configurations without
     * composition information, and those with.
     */
    enum class potential_bound_analysis_mode
    {
        /**
         * Switch used to analyse multiset charge configurations without composition information.
         */
        ANALYZE_MULTISET,
        /**
         * Switch used to analyse multiset charge configurations with information of specific composition thereof.
         */
        ANALYZE_COMPOSITION
    };
    /**
     * This function obtains the pair of potential bounds relevant to a potential projection associated with a multiset
     * charge configuration as received by the given SiDB. The entire layout of SiDBs is taken into account in the
     * returned bounds, as this function accumulates the received potential that is external to the projecting cluster
     * with that which is internal to it. Depending on the given potential bound analysis mode, the latter considers
     * different levels of detail.
     *
     * @tparam mode This selects the potential bound analysis mode. Intuitively, when analysing a composition, the
     * information that constitutes bounds has one level more detail; this information from the deeper level in the
     * cluster hierarchy is later flattened to enable analysis of the composed multiset charge configuration without the
     * additional information.
     * @param pst The projector state that, together with a receiving SiDB, yields a pair of bounds on the potential
     * projection from the cluster associated with the projector state onto the receiving SiDB.
     * @param sidb_ix The receiving SiDB.
     * @param composition_pot_bounds This optional parameter supplies the additional composition information when
     * available.
     * @return A pair of doubles that represent the lower and upper bound of the potential projection onto the given
     * SiDB.
     */
    template <potential_bound_analysis_mode mode>
    static inline std::pair<double, double>
    get_received_potential_bounds(const sidb_cluster_projector_state& pst, const uint64_t sidb_ix,
                                  const std::optional<complete_potential_bounds_store>& composition_pot_bounds) noexcept
    {
        if constexpr (mode == potential_bound_analysis_mode::ANALYZE_MULTISET)
        {
            // this considers the flattened self-projection (defined below---CTRL+F)
            return {get_projector_state_bound<bound_direction::LOWER>(pst, sidb_ix).pot_val +
                        pst.cluster->received_ext_pot_bounds.get<bound_direction::LOWER>(sidb_ix),
                    get_projector_state_bound<bound_direction::UPPER>(pst, sidb_ix).pot_val +
                        pst.cluster->received_ext_pot_bounds.get<bound_direction::UPPER>(sidb_ix)};
        }
        else if constexpr (mode == potential_bound_analysis_mode::ANALYZE_COMPOSITION)
        {
            // this considers the flattened self-projection of the previous level
            return {composition_pot_bounds.value().get<bound_direction::LOWER>(sidb_ix) +
                        pst.cluster->parent.lock()->received_ext_pot_bounds.get<bound_direction::LOWER>(sidb_ix),
                    composition_pot_bounds.value().get<bound_direction::UPPER>(sidb_ix) +
                        pst.cluster->parent.lock()->received_ext_pot_bounds.get<bound_direction::UPPER>(sidb_ix)};
        }
    }
    /**
     * This function receives a multiset charge configuration with an associated cluster and finds whether this multiset
     * charge configuration can be declared invalid for this cluster.
     *
     * @tparam mode The potential bound analysis mode that switches the function between analysing a multiset charge
     * configuration either with or without composition information.
     * @param pst The projector state for which the potential bound analysis is to be performed.
     * @param composition_potential_bounds This optional parameter supplies the additional composition information when
     * available.
     * @return `false` if and only if `pst` can be excluded from the *Ground State Space*.
     */
    template <potential_bound_analysis_mode mode>
    [[nodiscard]] bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst,
                                                        const std::optional<complete_potential_bounds_store>&
                                                            composition_potential_bounds = std::nullopt) const noexcept
    {
        witness_partitioning_state st{pst};

        // count witnesses for each charge state
        for (const uint64_t sidb_ix : pst.cluster->sidbs)
        {
            const auto& [recv_pot_lb, recv_pot_ub] =
                get_received_potential_bounds<mode>(pst, sidb_ix, composition_potential_bounds);

            if (st.required_neg_count != 0 && !fail_onto_negative_charge(recv_pot_lb))
            {
                st.negative_witnesses.emplace(sidb_ix);
            }

            if (st.required_pos_count != 0 && !fail_onto_positive_charge(recv_pot_ub))
            {
                st.positive_witnesses.emplace(sidb_ix);
            }

            if (st.required_neut_count != 0 && !ub_fail_onto_neutral_charge(recv_pot_ub) &&
                !lb_fail_onto_neutral_charge(recv_pot_lb))
            {
                st.neutral_witnesses.emplace(sidb_ix);
            }
        }

        if (st.negative_witnesses.size() < st.required_neg_count ||
            st.positive_witnesses.size() < st.required_pos_count ||
            st.neutral_witnesses.size() < st.required_neut_count)
        {
            return false;
        }

        if (pst.cluster->num_sidbs() > params.witness_partitioning_cluster_size_limit)
        {
            return true;
        }

        // reduce problem to overlapping witnesses only and see if it exceeds the limit
        if (st.omit_free_witnesses_and_count_overlap() > params.num_overlapping_witnesses_limit_gss)
        {
            return true;
        }

        // look for UNSAT (factorial)
        return find_valid_witness_partitioning<sidb_charge_state::NEGATIVE>(st, st.required_neg_count);
    }
    /**
     * The charge space of the given cluster is checked by performing the potential bound analysis on each multiset
     * charge configuration in it (without composition information). For each invalid one found, corresponding updates
     * are made to maintain consistency in the cluster hierarchy data structure. Updates make the stored bound
     * information more strict, and as a result, more invalid states may be found.
     *
     * @param c The cluster to check the charge space of.
     * @return `true` if and only if charge space of `c` has no invalid elements.
     */
    bool check_charge_space(const sidb_cluster_ptr& c) noexcept
    {
        // skip if |charge space| = 1
        if (c->charge_space.size() == 1)
        {
            return true;
        }

        bool fixpoint = true;

        std::vector<uint64_t> removed_ms{};
        removed_ms.reserve(c->charge_space.size());

        // perform potential bound analysis on every multiset in the charge space
        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            const sidb_cluster_projector_state pst{c, static_cast<uint64_t>(m)};

            if (!perform_potential_bound_analysis<potential_bound_analysis_mode::ANALYZE_MULTISET>(pst))
            {
                handle_invalid_state(pst);
                removed_ms.emplace_back(pst.multiset_conf);
                fixpoint = false;
            }
        }

        for (const uint64_t m : removed_ms)
        {
            c->charge_space.erase(sidb_cluster_charge_state{m});
        }

        return fixpoint;
    }
    /**
     * The charge spaces of each cluster in the current clustering are checked and updated accordingly when needed.
     *
     * @param skip_cluster This optional parameter specifies a cluster to skip in the pass over all clusters in the
     * current clustering.
     * @return `true` if and only if a fixed point has been reached; i.e., none of the charge space contain an element
     * that may be removed.
     */
    bool update_charge_spaces(const std::optional<uint64_t>& skip_cluster = std::nullopt) noexcept
    {
        bool fixpoint = true;

        // make a pass over the clustering and see if the charge spaces contain invalid cluster charge states
        for (const sidb_cluster_ptr& c : clustering)
        {
            if (!skip_cluster.has_value() || c->uid != skip_cluster.value())
            {
                fixpoint &= check_charge_space(c);
            }
        }

        return fixpoint;
    }
    /**
     * To facilitate efficient unfolding for the second stage of the simulation by *ClusterComplete*, potential bound
     * data that is stored in the hierarchy which will not be subject to change anymore is converted to a handy format:
     * a complete potential bounds store. It holds potential bounds for all SiDBs in the considered layout and thus
     * addition and subtraction of other complete potential bound stores are easy to perform.
     *
     * @param parent The newly-forming parent cluster whose children's charge spaces become fixed upon their merging.
     */
    void write_children_pot_bounds_to_complete_store(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const sidb_cluster_charge_state& m : child->charge_space)
            {
                const auto ccs = static_cast<uint64_t>(m);

                const sidb_cluster_projector_state pst{child, ccs};

                complete_potential_bounds_store complete_pot_store{};
                complete_pot_store.initialize_complete_potential_bounds(top_cluster->num_sidbs());

                for (const uint64_t sidb_ix : top_cluster->sidbs)
                {
                    complete_pot_store.set(sidb_ix,
                                           get_projector_state_bound<bound_direction::LOWER>(pst, sidb_ix).pot_val,
                                           get_projector_state_bound<bound_direction::UPPER>(pst, sidb_ix).pot_val);
                }

                child->pot_projs_complete_store[ccs] = std::move(complete_pot_store);
            }
        }
    }
    /**
     * This function performs the first step to merging a set of clusters to their direct parent. When clusters are
     * merged, their respective charge spaces have reached a fixed point in the construction; thereby, the projections
     * specific to each stored composition in the respective charge spaces, for which previously only the receiving
     * SiDBs in the respective child cluster were considered, are now composed to potential bounds onto each SiDB
     * outside the respective cluster, thus making a complete potential bounds store.
     *
     * @param parent The cluster that parents the clusters to merge.
     * @return The number of projector states that is the accumulation of the number of projector states in the
     * composition of each charge space element of each child.
     */
    static uint64_t compute_external_pot_bounds_for_saved_compositions(const sidb_cluster_ptr& parent) noexcept
    {
        uint64_t saved_projector_states = 0;

        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const sidb_cluster_charge_state& m : child->charge_space)
            {
                for (sidb_charge_space_composition& composition : m.compositions)
                {
                    for (const uint64_t sidb_ix : child->external_sidbs)
                    {
                        for (const sidb_cluster_projector_state& child_pst_of_child : composition.proj_states)
                        {
                            composition.pot_bounds.update(
                                sidb_ix,
                                get_projector_state_bound<bound_direction::LOWER>(child_pst_of_child, sidb_ix).pot_val,
                                get_projector_state_bound<bound_direction::UPPER>(child_pst_of_child, sidb_ix).pot_val);
                        }
                    }

                    saved_projector_states += composition.proj_states.size();
                }
            }
        }

        return saved_projector_states;
    }
    /**
     * This function derives the new externally received partial sums of electrostatic potential local to an SiDB
     * contained by the child (together forming `child_rst`) through subtracting the projections of its siblings.
     *
     * @tparam bound The Bound to handle (lower/upper)
     * @param parent The newly forming parent cluster.
     * @param child_rst Receptor state from one of the children of the parent.
     */
    template <bound_direction bound>
    void subtract_sibling_pot_from_received_ext_pot_bound(const sidb_cluster_ptr&            parent,
                                                          const sidb_cluster_receptor_state& child_rst) const noexcept
    {
        double received_pot_without_siblings = child_rst.cluster->received_ext_pot_bounds.get<bound>(child_rst.sidb_ix);

        for (const sidb_cluster_ptr& sibling : parent->children)
        {
            if (sibling != child_rst.cluster)
            {
                received_pot_without_siblings -= get_projection_bound<bound>(sibling, child_rst.sidb_ix).pot_val;
            }
        }

        parent->received_ext_pot_bounds.set<bound>(child_rst.sidb_ix, received_pot_without_siblings);
    }
    /**
     * This function dynamically updates the externally received partial sums of local electrostatic potential of the
     * children, and sets that of the parent
     *
     * @param parent The newly forming parent cluster.
     */
    void derive_children_received_bounds_without_siblings(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const uint64_t sidb_ix : child->sidbs)
            {
                const sidb_cluster_receptor_state child_rst{child, sidb_ix};
                subtract_sibling_pot_from_received_ext_pot_bound<bound_direction::LOWER>(parent, child_rst);
                subtract_sibling_pot_from_received_ext_pot_bound<bound_direction::UPPER>(parent, child_rst);
            }
        }
    }
    /**
     * This function determines whether a newly composed candidate for the charge space of the newly forming parent
     * cluster can be rejected in the creation of the combined charge space. Here, multiset charge configurations for
     * the parents are checked with composition information: a combination of respective charge space elements of the
     * children is considered, thus, intuitively, the information considered in the analysis goes one level deeper than
     * when later considering the composed multiset with flattened information with respect to its compositions.
     *
     * @param composition A composition of charge space elements of the children, making a candidate for a charge space
     * element of their direct parent.
     * @return `false` if and only if the given composition can be rejected.
     */
    bool verify_composition(sidb_charge_space_composition& composition) const noexcept
    {
        // initialize the composition potential bounds to (0.0, 0.0) for all SiDBs
        composition.pot_bounds.initialize_complete_potential_bounds(top_cluster->num_sidbs());

        // perform physically informed space pruning for a multiset composition
        for (sidb_cluster_projector_state& receiving_pst : composition.proj_states)
        {
            for (const uint64_t sidb_ix : receiving_pst.cluster->sidbs)
            {
                double internal_pot_lb{};
                double internal_pot_ub{};

                for (const sidb_cluster_projector_state& pst : composition.proj_states)
                {
                    internal_pot_lb += get_projector_state_bound<bound_direction::LOWER>(pst, sidb_ix).pot_val;
                    internal_pot_ub += get_projector_state_bound<bound_direction::UPPER>(pst, sidb_ix).pot_val;
                }

                composition.pot_bounds.set(sidb_ix, internal_pot_lb, internal_pot_ub);
            }

            if (!perform_potential_bound_analysis<potential_bound_analysis_mode::ANALYZE_COMPOSITION>(
                    receiving_pst, composition.pot_bounds))
            {
                return false;
            }
        }

        return true;
    }
    /**
     * This recursive function goes through all combinations of charge space elements of the children (exactly one
     * element per child for each combination), and verifies each combination before adding it to the merged charge
     * space of the parent. Combinations with equivalent multiset signatures are preserved under different compositions
     * of the multiset charge configuration in the charge space of the parent.
     *
     * @param parent The newly-forming parent cluster.
     * @param cur_child_ix The index of the child of which precisely one element of its charge space should be added to
     * the combination of charge space element. When it reaches out of bounds, a combination is complete and may be
     * verified before adding it to the parent charge space.
     * @param m The dynamically combined cluster charge state (i.e., multiset charge configuration). A copy of `m` when
     * it is a complete composition of charge space elements of the children is added to the parent charge space if
     * verification passes. After, `m` is dynamically transformed to the next combination until no new combination
     * exists.
     */
    void fill_merged_charge_state_space(const sidb_cluster_ptr& parent, const uint64_t cur_child_ix,
                                        sidb_cluster_charge_state& m) const noexcept
    {
        if (cur_child_ix >= parent->children.size())
        {
            if (!verify_composition(m.compositions.front()))
            {
                return;
            }

            // check if cluster charge state exists
            const auto it = parent->charge_space.find(m);
            if (it != parent->charge_space.cend())
            {
                it->compositions.emplace_back(m.compositions.front());
            }
            else
            {
                parent->charge_space.emplace(m);
            }

            return;
        }

        const sidb_cluster_ptr& cur_child = *std::next(parent->children.cbegin(), static_cast<int64_t>(cur_child_ix));

        for (const sidb_cluster_charge_state& m_part : cur_child->charge_space)
        {
            m.compositions.front().proj_states.emplace_back(
                sidb_cluster_projector_state{cur_child, static_cast<uint64_t>(m_part)});
            m += m_part;

            fill_merged_charge_state_space(parent, cur_child_ix + 1, m);

            m.compositions.front().proj_states.pop_back();
            m -= m_part;
        }
    }
    /**
     * This function calls the recursive function above that goes through all combination of charge space elements of
     * children to form the charge space of their direct parent.
     *
     * @param parent The parent cluster to which charge spaces are combined.
     */
    void construct_merged_charge_state_space(const sidb_cluster_ptr& parent) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.compositions.emplace_back();

        fill_merged_charge_state_space(parent, 0, m);
    }
    /**
     * This function aggregates the bounds on the electrostatic potential from the respective children onto the given
     * receiving SiDB, giving rise to the bound from their direct parent onto this SiDB.
     *
     * @tparam bound Bound to handle (lower/upper)
     * @param parent The newly-forming parent cluster.
     * @param rst The receptor state with the receiving SiDB that is currently handled.
     */
    template <bound_direction bound>
    void merge_pot_projection_bounds(const sidb_cluster_ptr&            parent,
                                     const sidb_cluster_receptor_state& rst) const noexcept
    {
        // construct external projected potential bounds for every composition of every element in the charge space
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const sidb_charge_space_composition& composition : m.compositions)
            {
                potential_projection pot_proj_onto_other_c{};

                for (const sidb_cluster_projector_state& pst : composition.proj_states)
                {
                    pot_proj_onto_other_c += get_projector_state_bound<bound>(pst, rst.sidb_ix);
                }

                add_pot_projection(parent, rst.sidb_ix, pot_proj_onto_other_c);
            }
        }

        // update the received external potential bound for the subject
        double diff = get_projection_bound<bound>(parent, rst.sidb_ix).pot_val;

        for (const sidb_cluster_ptr& child : parent->children)
        {
            diff -= get_projection_bound<bound>(child, rst.sidb_ix).pot_val;
        }

        rst.cluster->received_ext_pot_bounds.update<bound>(rst.sidb_ix, diff);
    }
    /**
     * After the charge space of the parent has been created, this function combines associated potential projections by
     * going through the charge space elements of the children, and, for each recipient SiDB, aggregates the potential
     * projections from each child onto that SiDB.
     *
     * @param parent The newly-forming parent cluster.
     */
    void construct_merged_potential_projections(const sidb_cluster_ptr& parent) const noexcept
    {
        // merge the projections of the children to projections of the parent
        for (const sidb_cluster_ptr& non_child : clustering)
        {
            for (const uint64_t sidb_ix : non_child->sidbs)
            {
                const sidb_cluster_receptor_state rst{non_child, sidb_ix};

                merge_pot_projection_bounds<bound_direction::LOWER>(parent, rst);
                merge_pot_projection_bounds<bound_direction::UPPER>(parent, rst);
            }
        }
    }
    /**
     * This function performs the flatten operation; the partial sum of the electrostatic potential local to all
     * contained SiDBs as received from within the cluster is, for each multiset charge configuration in the new charge
     * space, flattened, with respect to all compositions of the multiset, to the self-projection of the parent cluster.
     *
     * The above, and the fact that the self-projection for singletons is their respective local external
     * electrostatic potential, thus defines "self-projections" inductively.
     *
     * @param parent The newly-forming parent cluster.
     */
    static void compute_meets_for_internal_pot_bounds(const sidb_cluster_ptr& parent) noexcept
    {
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const uint64_t sidb_ix : parent->sidbs)
            {
                double lb_meet = potential_bound_top<bound_direction::LOWER>();
                double ub_meet = potential_bound_top<bound_direction::UPPER>();

                for (const sidb_charge_space_composition& composition : m.compositions)
                {
                    take_meet_of_potential_bounds<bound_direction::LOWER>(
                        lb_meet, composition.pot_bounds.get<bound_direction::LOWER>(sidb_ix));
                    take_meet_of_potential_bounds<bound_direction::UPPER>(
                        ub_meet, composition.pot_bounds.get<bound_direction::UPPER>(sidb_ix));
                }

                add_pot_projection(parent, sidb_ix, potential_projection{lb_meet, static_cast<uint64_t>(m)});
                add_pot_projection(parent, sidb_ix, potential_projection{ub_meet, static_cast<uint64_t>(m)});
            }
        }
    }
    /**
     * This function performs the complete merging operation from a set of sibling clusters to their direct parent. In
     * this process, combinations of charge space elements of the children are considered in special detail, before
     * passing them to the charge space of the parent and flattening these details such that the process of checking
     * multiset charge configurations in respective charge spaces of the clusters in the clustering may continue as
     * before. The termination of the *Ground State Space* algorithm is signalled when the clustering only contains
     * the top cluster.
     */
    void move_up_hierarchy() noexcept
    {
        if (clustering.size() == 1 && *clustering.cbegin() == top_cluster)
        {
            terminate = true;
            return;
        }

        // find the parent with the minimum cluster size
        const sidb_cluster_ptr& min_parent =
            (*std::min_element(clustering.cbegin(), clustering.cend(),
                               [](const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2)
                               { return c1->get_parent()->num_sidbs() < c2->get_parent()->num_sidbs(); }))
                ->get_parent();

        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        write_children_pot_bounds_to_complete_store(min_parent);

        projector_state_count += compute_external_pot_bounds_for_saved_compositions(min_parent);

        derive_children_received_bounds_without_siblings(min_parent);

        construct_merged_charge_state_space(min_parent);

        if (min_parent->charge_space.empty())
        {
            terminate = true;
            return;
        }

        construct_merged_potential_projections(min_parent);

        compute_meets_for_internal_pot_bounds(min_parent);

        clustering.emplace(min_parent);

        if (update_charge_spaces(min_parent->uid))
        {
            move_up_hierarchy();
        }
    }
    /**
     * This function computes the maximum size of the charge space of the top cluster. Comparing this number with the
     * size of the charge space of the top cluster that is returned after running *Ground State Space* gives an
     * indication of how the search space was reduced.
     *
     * @param number_of_sidbs The number of SiDBs (\f$N\f$).
     * @return \f$\binom{N + 2}{2}\f$ if base = 3, or otherwise \f$\binom{N + 1}{1}\f$ when the base is 2.
     */
    [[nodiscard]] constexpr inline uint64_t maximum_top_level_multisets(const uint64_t number_of_sidbs) const noexcept
    {
        //
        return params.simulation_parameters.base == 3 ? ((number_of_sidbs + 1) * (number_of_sidbs + 2)) / 2 :
                                                        number_of_sidbs + 1;
    }
    /**
     * Parameters used during the construction.
     */
    const ground_state_space_params params;
    /**
     * The top cluster, the cluster that contains all SiDBs, is returned as the result of the construction.
     */
    const sidb_cluster_ptr top_cluster;
    /**
     * The clustering starts at all singletons, then moves up through merges until only the top cluster remains
     */
    sidb_clustering clustering{};
    /**
     * Count the total number of projector states that are stored in the constructed hierarchy.
     */
    uint64_t projector_state_count{};
    /**
     * `true` if and only if the construction is to be terminated.
     */
    bool terminate = false;
    /**
     * Globally available array of bounds that section the band gap, used for pruning.
     */
    const std::array<double, 4> mu_bounds_with_error;
};

}  // namespace detail

/**
 * The purely constructive *Ground State Space* algorithm is the key ingredient of the *ClusterComplete* exact SiDB
 * simulator that lifts exact SiDB simulation to permit multiple gates in connection. It uses iterative "loop until
 * fixpoint" concepts to prune the simulation search space for not only a flat layout of SiDBs, but rather generalizes,
 * and lifts the physically informed space pruning technique introduced with *QuickExact* to higher order, allowing
 * *Ground State Space* to prune multiset charge state configurations at any level in a cluster hierarchy.
 *
 * The role of the cluster hierarchy is to rank interactions between groups, or clusters of SiDBs that together make up
 * the whole layout, such that the variation in electrostatic potential under different charge state assignments is
 * highest between the children clusters of clusters that low in the hierarchy. Thereby, the structure allows us to
 * consider the most charge state assignment-dependent interaction in a more detailed physically informed space pruning
 * analysis, enabling high pruning efficacy for the few pruning tests (with respect to the exponential search space).
 *
 * Starting at a clustering of all singleton clusters, the charge spaces, ie. a set of multiset charge configurations
 * (initially { {{-}}, {{0}}, {{+}} } or omitting the singleton multiset {{+}} in the case of base 2 pre-simulation),
 * are pruned iteratively through potential bound analysis. Through merges, ie., replacing a set of children in the
 * clustering with their parent, we may inspect the most crucially dependant interactions in the layout separately. The
 * procedure finishes when the charge spaces have been folded all the way up to the top cluster, parent of all, which
 * then contains all information resulting from the construction. *ClusterComplete*, without much trickery, now simply
 * unfolds this result, allowing simulation of problems that were previously seen as astronomical, due to the
 * (base 2 or 3) exponential growth in the number of SiDBs.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to construct the *Ground State Space* of.
 * @param params The parameters that *Ground State Space* will use throughout the construction. The physical parameters
 * that *Ground State Space* will use to prune simulation search space are stored in there. In particular, the user may
 * configure parameters that decide limits on the problem sizes of pruning by validity witness partitioning. By default,
 * these are set to avoid runtimes from being affected, as these sub-problems may scale factorially. Thereby, these
 * parameters are especially useful for large simulation problems that could benefit from extra intensive pruning before
 * *ClusterComplete* unfolds the constructed hierarchical charge space.
 * @return The results of the construction, which include the top cluster which parents all other clusters, and thereby
 * contains the charge spaces of each cluster.
 */
template <typename Lyt>
[[nodiscard]] ground_state_space_results ground_state_space(const Lyt&                       lyt,
                                                            const ground_state_space_params& params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (lyt.num_cells() == 0)
    {
        return ground_state_space_results{};
    }

    detail::ground_state_space_impl<Lyt> p{lyt, params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_GROUND_STATE_SPACE_HPP
