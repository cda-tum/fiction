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
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace fiction
{

/*
 * This struct is used to store the results of the *Ground State Space* construction.
 */
struct ground_state_space_stats
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
     * A quick heuristic to assess the quality of the pruning is captured by the size of the charge space of the top
     * cluster, which depends on the charge spaces of all clusters below it. The number of pruned elements is stored
     * here, which is computed by subtracting the size of the charge space of the top cluster from the maximum size it
     * can have for the given simulation base.
     */
    const uint64_t pruned_top_level_multisets{};
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
     * Report *Ground State Space* statistics.
     *
     * @return Prints the runtime and the number of pruned top level multisets versus the total amount possible.
     */
    void report() const noexcept
    {
        std::cout << "Pruned " << pruned_top_level_multisets << " out of " << maximum_top_level_multisets
                  << " top level multiset charge configurations\n";

        std::cout << "Ground State Space took ";

        if (const double gss_runtime = mockturtle::to_seconds(runtime); gss_runtime > 1.0)
        {
            std::cout << gss_runtime << " seconds" << std::endl;
        }
        else
        {
            std::cout << (gss_runtime * 1000) << " milliseconds" << std::endl;
        }
    }
};

namespace detail
{

template <typename Lyt>
class ground_state_space_impl
{
  public:
    // The documentation for the constructor parameters is found at the bottom of the file.
    explicit ground_state_space_impl(const Lyt& lyt, const uint64_t max_cluster_size_for_witness_partitioning,
                                     const sidb_simulation_parameters& phys_params) noexcept :
            base{phys_params.base},
            top_cluster{to_sidb_cluster(sidb_cluster_hierarchy(lyt))},
            clustering{get_initial_clustering(top_cluster, get_local_potential_bounds(lyt, phys_params))},
            witness_partitioning_max_cluster_size{max_cluster_size_for_witness_partitioning},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - phys_params.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_plus()}
    {}

    ground_state_space_stats run() noexcept
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

        return ground_state_space_stats{top_cluster, time_counter, max_multisets - top_cluster->charge_space.size(),
                                        max_multisets};
    }

  private:
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

    static std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>
    get_local_potential_bounds(const Lyt& lyt, const sidb_simulation_parameters& phys_params) noexcept
    {
        charge_distribution_surface<Lyt> cds_min{lyt};
        charge_distribution_surface<Lyt> cds_max{lyt};

        cds_min.assign_physical_parameters(phys_params);
        cds_max.assign_physical_parameters(phys_params);

        cds_min.assign_all_charge_states(phys_params.base == 3 ? sidb_charge_state::POSITIVE :
                                                                 sidb_charge_state::NEUTRAL);
        cds_max.assign_all_charge_states(sidb_charge_state::NEGATIVE);

        cds_min.update_after_charge_change();
        cds_max.update_after_charge_change();

        return {cds_min, cds_max};
    }

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
                                                         min_loc_pot_cds.get_phys_params().base, c);

            c->pot_projs[i] = potential_projection_order{-loc_ext_pot, min_loc_pot_cds.get_phys_params().base, true};

            for (uint64_t j = 0; j < min_loc_pot_cds.num_cells(); ++j)
            {
                if (j != i)
                {
                    c->pot_projs[j] =
                        potential_projection_order{min_loc_pot_cds.get_chargeless_potential_by_indices(i, j),
                                                   min_loc_pot_cds.get_phys_params().base};
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

    template <bound_direction bound>
    static constexpr inline potential_projection get_projection_bound(const sidb_cluster_ptr& c,
                                                                      const uint64_t          sidb_ix) noexcept
    {
        return c->pot_projs.at(sidb_ix).get_bound<bound>();
    }

    template <bound_direction bound>
    static constexpr inline double get_next_projected_pot_bound(const sidb_cluster_ptr& c,
                                                                const uint64_t          sidb_ix) noexcept
    {
        return c->pot_projs.at(sidb_ix).get_next_bound<bound>().pot_val;
    }

    template <bound_direction bound>
    static constexpr inline potential_projection get_projector_state_bound(const sidb_cluster_projector_state& pst,
                                                                           const uint64_t sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(sidb_ix).get_pot_proj_for_m_conf<bound>(pst.multiset_conf);
    }

    static constexpr inline void add_pot_projection(const sidb_cluster_ptr& c, const uint64_t sidb_ix,
                                                    const potential_projection& pp) noexcept
    {
        c->pot_projs[sidb_ix].add(pp);
    }

    static constexpr inline void remove_all_cluster_charge_state_occurrences(const sidb_cluster_projector_state& rm_pst,
                                                                             const uint64_t sidb_ix) noexcept
    {
        rm_pst.cluster->pot_projs[sidb_ix].remove_m_conf(rm_pst.multiset_conf);
    }

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

    constexpr void update_external_potential_projection(const sidb_cluster_projector_state& pst,
                                                        const sidb_cluster_receptor_state&  rst) const noexcept
    {
        update_external_pot_projection_if_bound_removed<bound_direction::LOWER>(pst, rst);
        update_external_pot_projection_if_bound_removed<bound_direction::UPPER>(pst, rst);

        // remove projection
        remove_all_cluster_charge_state_occurrences(pst, rst.sidb_ix);
    }

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

    struct witness_partitioning_state
    {
        using witness_set = phmap::btree_set<uint64_t>;

        witness_set negative_witnesses{}, positive_witnesses{}, neutral_witnesses{};
        uint64_t    required_neg_count, required_pos_count, required_neut_count;

        explicit witness_partitioning_state(const sidb_cluster_projector_state& pst) noexcept :
                required_neg_count{pst.get_count<sidb_charge_state::NEGATIVE>()},
                required_pos_count{pst.get_count<sidb_charge_state::POSITIVE>()},
                required_neut_count{pst.get_count<sidb_charge_state::NEUTRAL>()}
        {}

        void omit_free_witnesses() noexcept
        {
            // find free witnesses to reduce the problem, leaving only witnesses to partition that overlap
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

                    ++it;
                }
            }
        }
    };

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

    enum class potential_bound_analysis_mode
    {
        ANALYZE_MULTISET,
        ANALYZE_COMPOSITION
    };

    template <potential_bound_analysis_mode mode>
    static inline std::pair<double, double>
    get_received_potential_bounds(const sidb_cluster_projector_state& pst, const uint64_t sidb_ix,
                                  const std::optional<partial_potential_bounds_store>& composition_pot_bounds) noexcept
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

    template <potential_bound_analysis_mode mode>
    [[nodiscard]] bool perform_potential_bound_analysis(
        const sidb_cluster_projector_state&          pst,
        const std::optional<partial_potential_bounds_store>& composition_potential_bounds = std::nullopt) const noexcept
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

        if (pst.cluster->num_sidbs() > witness_partitioning_max_cluster_size)
        {
            return true;
        }

        // reduce problem to overlapping witnesses only
        st.omit_free_witnesses();

        // look for UNSAT (factorial)
        return find_valid_witness_partitioning<sidb_charge_state::NEGATIVE>(st, st.required_neg_count);
    }

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

    template <bound_direction bound>
    void subtract_sibling_pot_from_received_ext_pot_bound(const sidb_cluster_ptr&            parent,
                                                          const sidb_cluster_receptor_state& child_rst) const noexcept
    {
        // derive the new externally received partial sums of electrostatic potential local to SiDB contained by the
        // child through subtracting the projections of its sibling
        double recv_pot_without_siblings = child_rst.cluster->received_ext_pot_bounds.get<bound>(child_rst.sidb_ix);

        for (const sidb_cluster_ptr& sibling : parent->children)
        {
            if (sibling != child_rst.cluster)
            {
                recv_pot_without_siblings -= get_projection_bound<bound>(sibling, child_rst.sidb_ix).pot_val;
            }
        }

        parent->received_ext_pot_bounds.set<bound>(child_rst.sidb_ix, recv_pot_without_siblings);
    }

    void derive_children_received_bounds_without_siblings(const sidb_cluster_ptr& parent) const noexcept
    {
        // dynamically update the externally received partial sums (of local electrostatic potential) of the children,
        // and set that of the parent; what the new parent determines what interactions are external
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

    bool verify_composition(sidb_charge_space_composition& composition) const noexcept
    {
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
            const sidb_cluster_charge_state_space::iterator it = parent->charge_space.find(m);
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

            m.compositions.front().proj_states.pop_back();  // ????????
            m -= m_part;
        }
    }

    void construct_merged_charge_state_space(const sidb_cluster_ptr& parent) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.compositions.emplace_back();

        fill_merged_charge_state_space(parent, 0, m);
    }

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

    static void compute_meets_for_internal_pot_bounds(const sidb_cluster_ptr& parent) noexcept
    {
        // this is the flatten operation; the partial sum of the electrostatic potential local to all contained SiDBs as
        // received from within the cluster is, for each multiset in the new charge space, flattened, with respect to
        // all compositions of the multiset, to the self-projection of the parent cluster

        // the above, and the fact that the self-projection for singletons is their respective local external
        // electrostatic potential,  thus defines "self-projections" inductively

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

        update_charge_spaces(min_parent->uid);
    }

    [[nodiscard]] constexpr inline uint64_t maximum_top_level_multisets(const uint64_t number_of_sidbs) const noexcept
    {
        // computes nCr(N + 2, 2)                             // computes nCr(N + 1, 1)
        return base == 3 ? ((number_of_sidbs + 1) * (number_of_sidbs + 2)) / 2 : number_of_sidbs + 1;
    }

    const uint8_t base;

    const sidb_cluster_ptr top_cluster;

    sidb_clustering clustering{};

    bool terminate = false;

    const uint64_t witness_partitioning_max_cluster_size;

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
 * @param lyt Layout to construct the ground state space of.
 * @param max_cluster_size_for_witness_partitioning This specifies the maximum cluster size for which *Ground State
 * Space* will solve an NP-complete sub-problem exhaustively. The sets of SiDBs that witness local population stability
 * for each respective charge state may be partitioned into disjoint sets such that the number of required witnesses for
 * each respective charge state is satisfied. If no such partition exists, the multiset charge configuration associated
 * with the requirements may be rejected. Defaulted to `6` to avoid impairing runtimes, while allowing some pruning.
 * @param phys_params The physical parameters that *Ground State Space* will use to prune simulation search space.
 * @return The results of the construction, which include the top cluster which parents all other clusters, and thereby
 * contains the charge spaces of each cluster.
 */
template <typename Lyt>
ground_state_space_stats
ground_state_space(const Lyt& lyt, const uint64_t max_cluster_size_for_witness_partitioning = 6,
                   const sidb_simulation_parameters& phys_params = sidb_simulation_parameters{}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (lyt.num_cells() == 0)
    {
        return ground_state_space_stats{};
    }

    detail::ground_state_space_impl<Lyt> p{lyt, max_cluster_size_for_witness_partitioning, phys_params};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_ALGLIB_ENABLED

#endif  // FICTION_GROUND_STATE_SPACE_HPP
