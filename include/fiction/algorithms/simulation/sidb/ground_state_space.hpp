//
// Created by Willem Lambooy on 06/02/2024.
//

#ifndef FICTION_GROUND_STATE_SPACE_HPP
#define FICTION_GROUND_STATE_SPACE_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_cluster_hierarchy.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

template <typename Lyt>
class ground_state_space
{
  private:
    static std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>
    get_local_potential_bounds(const Lyt& lyt) noexcept
    {
        charge_distribution_surface<Lyt> cl_min{lyt};
        charge_distribution_surface<Lyt> cl_max{lyt};

        cl_min.assign_all_charge_states(sidb_charge_state::POSITIVE);
        cl_max.assign_all_charge_states(sidb_charge_state::NEGATIVE);

        cl_min.update_after_charge_change();
        cl_max.update_after_charge_change();

        return {cl_min, cl_max};
    }

  public:
    explicit ground_state_space(const Lyt&                        lyt,
                                const sidb_simulation_parameters& phys_params            = sidb_simulation_parameters{},
                                const uint64_t max_cluster_size_for_witness_partitioning = 6) noexcept :
            clustering{
                get_initial_clustering(to_sidb_cluster(sidb_cluster_hierarchy(lyt)), get_local_potential_bounds(lyt))},
            bot{clustering},
            cds{charge_distribution_surface<Lyt>{lyt}},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - phys_params.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_plus()},
            witness_partitioning_max_cluster_size{max_cluster_size_for_witness_partitioning}
    {
        for (uint64_t i = 0; i < lyt.num_cells(); ++i)
        {
            global_pruned_store.emplace(std::make_pair(i, std::array{false, false, false}));
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

    static sidb_clustering
    get_initial_clustering(const sidb_cluster_ptr& c,
                           const std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>&
                               local_potential_bound_containers) noexcept
    {
        sidb_clustering clustering{};

        if (c->sidbs.size() == 1)
        {
            const uint64_t i = *c->sidbs.cbegin();

            c->initialize_singleton_cluster_charge_space(
                i, -local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
                -local_potential_bound_containers.second.get_local_potential_by_index(i).value());

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                if (j == i)
                {
                    c->pot_projs[j] = potential_projection_orders{i};
                    continue;
                }

                c->pot_projs[j] = potential_projection_orders{
                    local_potential_bound_containers.first.get_potential_by_indices(i, j), j};
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

    using pruned_store = std::unordered_map<uint64_t, std::array<bool, 3>>;

    template <sidb_charge_state onto_cs>
    static constexpr inline bool onto_cs_pruned(const sidb_cluster_ptr& c) noexcept
    {
        return c->pot_projs.at(c->uid).onto_cs_pruned<onto_cs>();
    }

    template <sidb_charge_state onto_cs>
    constexpr inline bool get_onto_cs_pruned_for_sidb_ix(const uint64_t sidb_ix) const noexcept
    {
        return global_pruned_store.at(sidb_ix)[potential_projection_orders::cs_ix<onto_cs>()];
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline bool get_onto_cs_pruned_for_sidb_ix(const uint64_t      sidb_ix,
                                                                const pruned_store& local_pruned_store) noexcept
    {
        return local_pruned_store.at(sidb_ix)[potential_projection_orders::cs_ix<onto_cs>()];
    }

    template <sidb_charge_state onto_cs>
    constexpr inline void prune_onto_cs_for_sidb_ix(const uint64_t sidb_ix) noexcept
    {
        global_pruned_store[sidb_ix][potential_projection_orders::cs_ix<onto_cs>()] = true;
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void prune_onto_cs_for_sidb_ix(const uint64_t sidb_ix,
                                                           pruned_store&  local_pruned_store) noexcept
    {
        local_pruned_store[sidb_ix][potential_projection_orders::cs_ix<onto_cs>()] = true;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_bound(const sidb_cluster_ptr&            c,
                                                                const sidb_cluster_receptor_state& rst) noexcept
    {
        return c->pot_projs.at(rst.cluster->uid).get<bound, onto_cs>(rst.sidb_ix);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_next_proj_pot_bound(const sidb_cluster_ptr&            c,
                                                           const sidb_cluster_receptor_state& rst) noexcept
    {
        return c->pot_projs.at(rst.cluster->uid).get_next<bound, onto_cs>(rst.sidb_ix).V;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_state_bound(const sidb_cluster_projector_state& pst,
                                                                      const sidb_cluster_receptor_state&  rst) noexcept
    {
        return pst.cluster->pot_projs.at(rst.cluster->uid)
            .get_pot_proj_for_m_conf<bound, onto_cs>(pst.multiset_conf, rst.sidb_ix);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_proj_pot_bound_for_internal_state(const sidb_cluster_projector_state& pst,
                                                                         const uint64_t sidb_ix) noexcept
    {
        return pst.cluster->pot_projs.at(pst.cluster->uid)
            .get_pot_proj_for_m_conf<bound, onto_cs>(pst.multiset_conf, sidb_ix)
            .V;
    }

    template <sidb_charge_state onto_cs>
    constexpr inline void remove_all_internal_state_occurrences(const sidb_cluster_projector_state& rm_pst,
                                                                const uint64_t                      sidb_ix) noexcept
    {
        if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(sidb_ix) || !rm_pst.contains_cs<onto_cs>())
        {
            return;
        }

        rm_pst.cluster->pot_projs[rm_pst.cluster->uid].remove_m_conf<onto_cs>(rm_pst.multiset_conf, sidb_ix);

        if (rm_pst.cluster->pot_projs[rm_pst.cluster->uid].erase_onto_sidb_ix_if_empty<onto_cs>(sidb_ix))
        {
            prune_onto_cs_for_sidb_ix<onto_cs>(sidb_ix);
        }
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void
    remove_all_cluster_charge_state_occurrences(const sidb_cluster_projector_state& rm_pst,
                                                const sidb_cluster_receptor_state&  rst) noexcept
    {
        rm_pst.cluster->pot_projs[rst.cluster->uid].remove_m_conf<onto_cs>(rm_pst.multiset_conf, rst.sidb_ix);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void add_pot_proj(const sidb_cluster_ptr& c, const sidb_cluster_receptor_state& rst,
                                              const potential_projection& pp) noexcept
    {
        c->pot_projs[rst.cluster->uid].add<onto_cs>(pp, rst.sidb_ix);
    }

    using pot_proj_order_onto_sidb_ix = potential_projection_orders::pot_proj_order_onto_sidb_ix;

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline void add_internal_pot_projs(const sidb_cluster_projector_state& pst,
                                                        const pot_proj_order_onto_sidb_ix&  pps_onto_sidb_ix) noexcept
    {
        pst.cluster->pot_projs[pst.cluster->uid].add_multiple<onto_cs>(pps_onto_sidb_ix);
    }

    template <sidb_charge_state onto_cs>
    constexpr inline bool charge_space_prunes_onto_cs(const sidb_cluster_receptor_state& rst) const noexcept
    {
        if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(rst.sidb_ix))
        {
            return true;
        }

        if constexpr (onto_cs == sidb_charge_state::NEGATIVE)
        {
            return fail_onto_negative_charge(
                get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(rst.cluster, rst).V +
                rst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::LOWER, onto_cs>(rst.sidb_ix));
        }
        else if constexpr (onto_cs == sidb_charge_state::POSITIVE)
        {
            return fail_onto_positive_charge(
                get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(rst.cluster, rst).V +
                rst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::UPPER, onto_cs>(rst.sidb_ix));
        }
        else
        {
            return ub_fail_onto_neutral_charge(
                       get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(rst.cluster, rst).V +
                       rst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::UPPER, onto_cs>(rst.sidb_ix)) ||
                   lb_fail_onto_neutral_charge(
                       get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(rst.cluster, rst).V +
                       rst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::LOWER, onto_cs>(rst.sidb_ix));
        }
    }

    pruned_store derive_local_pruned_store(const sidb_cluster_ptr& c) const noexcept
    {
        pruned_store cluster_pruned_store{};

        for (const uint64_t sidb_ix : c->sidbs)
        {
            sidb_cluster_receptor_state rst{c, sidb_ix};
            cluster_pruned_store.emplace(std::make_pair(
                sidb_ix, std::array<bool, 3>{charge_space_prunes_onto_cs<sidb_charge_state::NEGATIVE>(rst),
                                             charge_space_prunes_onto_cs<sidb_charge_state::NEUTRAL>(rst),
                                             charge_space_prunes_onto_cs<sidb_charge_state::POSITIVE>(rst)}));
        }

        return cluster_pruned_store;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr void update_external_pot_proj_if_bound_removed(const sidb_cluster_projector_state& pst,
                                                                    const sidb_cluster_receptor_state&  rst) noexcept
    {
        const potential_projection& cur_bound = get_proj_bound<bound, onto_cs>(pst.cluster, rst);

        if (cur_bound.M == pst.multiset_conf)
        {
            rst.cluster->update_recv_ext_pot_bound<bound, onto_cs>(
                rst.sidb_ix, get_next_proj_pot_bound<bound, onto_cs>(pst.cluster, rst) - cur_bound.V);
        }
    }

    template <sidb_charge_state onto_cs>
    constexpr void update_external_potential_projection(const sidb_cluster_projector_state& pst,
                                                        const sidb_cluster_receptor_state&  rst) const noexcept
    {
        if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(rst.sidb_ix))
        {
            return;
        }

        if constexpr (onto_cs != sidb_charge_state::POSITIVE)
        {
            update_external_pot_proj_if_bound_removed<bound_calculation_mode::LOWER, onto_cs>(pst, rst);
        }
        if constexpr (onto_cs != sidb_charge_state::NEGATIVE)
        {
            update_external_pot_proj_if_bound_removed<bound_calculation_mode::UPPER, onto_cs>(pst, rst);
        }

        remove_all_cluster_charge_state_occurrences<onto_cs>(pst, rst);
    }

    template <sidb_charge_state witness_cs>
    void handle_invalid_state(const sidb_cluster_projector_state& pst) noexcept
    {
        if constexpr (witness_cs != sidb_charge_state::NONE)
        {
            // to be extended
            if (pst.cluster->sidbs.size() == 1)
            {
                prune_onto_cs_for_sidb_ix<witness_cs>(*pst.cluster->sidbs.cbegin());

                for (const sidb_cluster_ptr& other_c : clustering)
                {
                    if (other_c != pst.cluster)
                    {
                        other_c->pot_projs[pst.cluster->uid].prune_cs<witness_cs>();
                    }
                }
            }
        }

        for (const uint64_t sidb_ix : pst.cluster->sidbs)
        {
            remove_all_internal_state_occurrences<sidb_charge_state::NEGATIVE>(pst, sidb_ix);
            remove_all_internal_state_occurrences<sidb_charge_state::POSITIVE>(pst, sidb_ix);
            remove_all_internal_state_occurrences<sidb_charge_state::NEUTRAL>(pst, sidb_ix);
        }

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == pst.cluster)
            {
                continue;
            }

            for (const uint64_t& sidb_ix : other_c->sidbs)
            {
                const sidb_cluster_receptor_state rst{other_c, sidb_ix};
                update_external_potential_projection<sidb_charge_state::NEGATIVE>(pst, rst);
                update_external_potential_projection<sidb_charge_state::POSITIVE>(pst, rst);
                update_external_potential_projection<sidb_charge_state::NEUTRAL>(pst, rst);
            }
        }
    }

    using witness_set = std::set<uint64_t>;

    template <sidb_charge_state onto_cs>
    witness_set collect_validity_witnesses(const sidb_cluster_projector_state& pst,
                                           const pruned_store&                 local_pruned_store) const noexcept
    {
        witness_set validity_witnesses{};

        if (!pst.contains_cs<onto_cs>())
        {
            return validity_witnesses;
        }

        for (const uint64_t sidb_ix : pst.cluster->sidbs)
        {
            if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(sidb_ix, local_pruned_store))
            {
                continue;
            }

            if constexpr (onto_cs == sidb_charge_state::NEGATIVE)
            {
                if (fail_onto_negative_charge(
                        get_proj_pot_bound_for_internal_state<bound_calculation_mode::LOWER, onto_cs>(pst, sidb_ix) +
                        pst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::LOWER, onto_cs>(sidb_ix)))
                {
                    continue;
                }
            }
            else if constexpr (onto_cs == sidb_charge_state::POSITIVE)
            {
                if (fail_onto_positive_charge(
                        get_proj_pot_bound_for_internal_state<bound_calculation_mode::UPPER, onto_cs>(pst, sidb_ix) +
                        pst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::UPPER, onto_cs>(sidb_ix)))
                {
                    continue;
                }
            }
            else
            {
                if (ub_fail_onto_neutral_charge(
                        get_proj_pot_bound_for_internal_state<bound_calculation_mode::UPPER, onto_cs>(pst, sidb_ix) +
                        pst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::UPPER, onto_cs>(sidb_ix)) ||
                    lb_fail_onto_neutral_charge(
                        get_proj_pot_bound_for_internal_state<bound_calculation_mode::LOWER, onto_cs>(pst, sidb_ix) +
                        pst.cluster->get_recv_ext_pot_bound<bound_calculation_mode::LOWER, onto_cs>(sidb_ix)))
                {
                    continue;
                }
            }

            validity_witnesses.emplace(sidb_ix);
        }

        return validity_witnesses;
    }

    struct witness_partitioning_state
    {
        witness_set neg_w, pos_w, neut_w;
        uint64_t    num_neg, num_pos, num_neut;

        explicit witness_partitioning_state(witness_set&& neg_w_, witness_set&& pos_w_, witness_set&& neut_w_,
                                            uint64_t num_neg_, uint64_t num_pos_, uint64_t num_neut_) noexcept :
                neg_w{std::move(neg_w_)},
                pos_w{std::move(pos_w_)},
                neut_w{std::move(neut_w_)},
                num_neg{num_neg_},
                num_pos{num_pos_},
                num_neut{num_neut_}
        {}

        void omit_free_witnesses() noexcept
        {
            // find free witnesses to reduce the problem, leaving only witnesses to partition that overlap
            for (auto& [this_set, competing_sets] :
                 std::array{std::make_pair(std::make_pair(neg_w, num_neg), std::make_pair(pos_w, neut_w)),
                            std::make_pair(std::make_pair(pos_w, num_pos), std::make_pair(neg_w, neut_w)),
                            std::make_pair(std::make_pair(neut_w, num_neut), std::make_pair(neg_w, pos_w))})
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
            return st.neut_w.size() >= num_witnesses_for_current_cs;
        }
        else if constexpr (current_fill_cs == sidb_charge_state::POSITIVE)
        {
            if (num_witnesses_for_current_cs == 0)
            {
                return find_valid_witness_partitioning<sidb_charge_state::NEUTRAL>(st, st.num_neut);
            }

            for (witness_set::const_iterator it = st.pos_w.cbegin(); it != st.pos_w.cend();)
            {
                const uint64_t take_witness = *st.pos_w.cbegin();

                st.pos_w.erase(it);

                st.neut_w.erase(take_witness);

                if (find_valid_witness_partitioning<sidb_charge_state::POSITIVE>(st, num_witnesses_for_current_cs - 1))
                {
                    return true;
                }

                it = ++st.pos_w.emplace(take_witness).first;

                st.neut_w.emplace(take_witness);
            }

            return false;
        }

        if (num_witnesses_for_current_cs == 0)
        {
            return find_valid_witness_partitioning<sidb_charge_state::POSITIVE>(st, st.num_pos);
        }

        for (witness_set::const_iterator it = st.neg_w.cbegin(); it != st.neg_w.cend();)
        {
            const uint64_t take_witness = *it;

            st.neg_w.erase(it);

            st.pos_w.erase(take_witness);
            st.neut_w.erase(take_witness);

            if (find_valid_witness_partitioning<sidb_charge_state::NEGATIVE>(st, num_witnesses_for_current_cs - 1))
            {
                return true;
            }

            it = ++st.neg_w.emplace(take_witness).first;

            st.pos_w.emplace(take_witness);
            st.neut_w.emplace(take_witness);
        }

        return false;
    }

    bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst,
                                          const pruned_store&                 local_pruned_store) noexcept
    {
        witness_partitioning_state st{collect_validity_witnesses<sidb_charge_state::NEGATIVE>(pst, local_pruned_store),
                                      collect_validity_witnesses<sidb_charge_state::POSITIVE>(pst, local_pruned_store),
                                      collect_validity_witnesses<sidb_charge_state::NEUTRAL>(pst, local_pruned_store),
                                      pst.get_count<sidb_charge_state::NEGATIVE>(),
                                      pst.get_count<sidb_charge_state::POSITIVE>(),
                                      pst.get_count<sidb_charge_state::NEUTRAL>()};

        if (st.neg_w.size() < st.num_neg)
        {
            handle_invalid_state<sidb_charge_state::NEGATIVE>(pst);
            return false;
        }

        if (st.pos_w.size() < st.num_pos)
        {
            handle_invalid_state<sidb_charge_state::POSITIVE>(pst);
            return false;
        }

        if (st.neut_w.size() < st.num_neut)
        {
            handle_invalid_state<sidb_charge_state::NEUTRAL>(pst);
            return false;
        }

        if (pst.cluster->sidbs.size() > witness_partitioning_max_cluster_size)
        {
            return true;
        }

        st.omit_free_witnesses();

        if (!find_valid_witness_partitioning<sidb_charge_state::NEGATIVE>(st, st.num_neg))
        {
            handle_invalid_state<sidb_charge_state::NONE>(pst);
            return false;
        }

        return true;
    }

    bool check_charge_space(const sidb_cluster_ptr& c) noexcept
    {
        // skip if |charge space| = 1?
        if (c->charge_space.size() == 1)
        {
            return true;
        }

        pruned_store local_pruned_store{derive_local_pruned_store(c)};

        bool fixpoint = true;

        std::vector<uint64_t> removed_ms{};
        removed_ms.reserve(c->charge_space.size());

        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            if (!perform_potential_bound_analysis(sidb_cluster_projector_state{c, static_cast<uint64_t>(m)},
                                                  local_pruned_store))
            {
                removed_ms.emplace_back(static_cast<uint64_t>(m));
                fixpoint = false;
            }
        }

        for (const uint64_t m : removed_ms)
        {
            c->charge_space.erase(sidb_cluster_charge_state{m});
        }

        return fixpoint;
    }

    bool update_charge_spaces() noexcept
    {
        bool fixpoint = true;

        // make a pass over the clustering and see if the charge spaces contain invalid cluster charge states
        for (const sidb_cluster_ptr& c : clustering)
        {
            fixpoint &= check_charge_space(c);
        }

        return fixpoint;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void derive_recv_ext_pot_bound_without_siblings(const sidb_cluster_ptr&            parent,
                                                    const sidb_cluster_receptor_state& child_rst) const noexcept
    {
        if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(child_rst.sidb_ix))
        {
            return;
        }

        double recv_pot_without_siblings = child_rst.cluster->get_recv_ext_pot_bound<bound, onto_cs>(child_rst.sidb_ix);

        for (const sidb_cluster_ptr& sibling : parent->children)
        {
            if (sibling != child_rst.cluster)
            {
                recv_pot_without_siblings -= get_proj_bound<bound, onto_cs>(sibling, child_rst).V;
            }
        }

        parent->set_recv_ext_pot_bound<bound, onto_cs>(child_rst.sidb_ix, recv_pot_without_siblings);
    }

    void compute_children_recv_bounds_without_siblings(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const uint64_t& sidb_ix : child->sidbs)
            {
                const sidb_cluster_receptor_state child_rst{child, sidb_ix};
                derive_recv_ext_pot_bound_without_siblings<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(
                    parent, child_rst);
                derive_recv_ext_pot_bound_without_siblings<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(
                    parent, child_rst);
                derive_recv_ext_pot_bound_without_siblings<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(
                    parent, child_rst);
                derive_recv_ext_pot_bound_without_siblings<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(
                    parent, child_rst);
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    constexpr inline bool perform_potential_bound_analysis_for_composition(const sidb_cluster_projector_state& pst,
                                                                           const double pot_bound) const noexcept
    {
        if constexpr (onto_cs == sidb_charge_state::NEGATIVE)
        {
            return pst.contains_cs<sidb_charge_state::NEGATIVE>() && fail_onto_negative_charge(pot_bound);
        }
        else if constexpr (onto_cs == sidb_charge_state::POSITIVE)
        {
            return pst.contains_cs<sidb_charge_state::POSITIVE>() && fail_onto_positive_charge(pot_bound);
        }
        else if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return pst.contains_cs<sidb_charge_state::NEUTRAL>() && lb_fail_onto_neutral_charge(pot_bound);
        }
        else if constexpr (bound == bound_calculation_mode::UPPER)
        {
            return pst.contains_cs<sidb_charge_state::NEUTRAL>() && ub_fail_onto_neutral_charge(pot_bound);
        }

        return false;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    std::optional<pot_proj_order_onto_sidb_ix>
    validate_composition(const sidb_cluster_projector_state&          parent_pst,
                         const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        if (!parent_pst.contains_cs<onto_cs>())
        {
            return pot_proj_order_onto_sidb_ix{};
        }

        // construct internal potential projection bounds for this composition
        pot_proj_order_onto_sidb_ix internal_pot_proj_bounds{};

        double total_proj_pot_bound = potential_projection::top<bound>().V;

        for (const sidb_cluster_projector_state& receiver_pst : composition)
        {
            if (onto_cs_pruned<onto_cs>(receiver_pst.cluster) || !receiver_pst.contains_cs<onto_cs>())
            {
                continue;
            }

            for (const uint64_t sidb_ix : receiver_pst.cluster->sidbs)
            {
                const sidb_cluster_receptor_state rst{receiver_pst.cluster, sidb_ix};

                if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(rst.sidb_ix))
                {
                    continue;
                }

                potential_projection int_pot_proj_onto_receiver{};

                for (const sidb_cluster_projector_state& pst : composition)
                {
                    int_pot_proj_onto_receiver += get_proj_state_bound<bound, onto_cs>(pst, rst);  // add more
                }

                internal_pot_proj_bounds.emplace(std::pair(sidb_ix, std::set{int_pot_proj_onto_receiver}));

                const double total_proj_pot_bound_candidate =
                    parent_pst.cluster->get_recv_ext_pot_bound<bound, onto_cs>(rst.sidb_ix) +
                    int_pot_proj_onto_receiver.V;

                if (potential_bound_meet_is_weaker<bound>(total_proj_pot_bound,
                                                          total_proj_pot_bound_candidate))  // todo
                {
                    total_proj_pot_bound = total_proj_pot_bound_candidate;
                }
            }
        }

        // validate the current charge space candidate
        if (perform_potential_bound_analysis_for_composition<bound, onto_cs>(parent_pst, total_proj_pot_bound))
        {
            // invalid
            return std::nullopt;
        }

        // valid: return the collected potential projection bounds and the
        return internal_pot_proj_bounds;
    }

    bool
    add_internal_pot_projs_if_composition_valid(const sidb_cluster_projector_state&          parent_pst,
                                                const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        // returned order empty means invalid multiset combination -- do not add to the merged charge state space
        const std::optional<pot_proj_order_onto_sidb_ix>& neg_ord =
            validate_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(parent_pst, composition);
        if (!neg_ord.has_value())
        {
            return false;
        }
        const std::optional<pot_proj_order_onto_sidb_ix>& pos_ord =
            validate_composition<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(parent_pst, composition);
        if (!pos_ord.has_value())
        {
            return false;
        }
        const std::optional<pot_proj_order_onto_sidb_ix>& neut_ub_ord =
            validate_composition<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(parent_pst, composition);
        if (!neut_ub_ord.has_value())
        {
            return false;
        }
        const std::optional<pot_proj_order_onto_sidb_ix>& neut_lb_ord =
            validate_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(parent_pst, composition);
        if (!neut_lb_ord.has_value())
        {
            return false;
        }

        // charge space candidate validated, add the internal potential projection bounds
        add_internal_pot_projs<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(parent_pst, neg_ord.value());
        add_internal_pot_projs<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(parent_pst, pos_ord.value());
        add_internal_pot_projs<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(parent_pst,
                                                                                          neut_lb_ord.value());
        add_internal_pot_projs<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(parent_pst,
                                                                                          neut_ub_ord.value());

        return true;
    }

    void add_valid_combinations_to_merged_charge_space(const sidb_cluster_ptr& parent, const uint64_t cur_child_ix,
                                                       sidb_cluster_charge_state& m) const noexcept
    {
        if (cur_child_ix >= parent->children.size())
        {
            // verify composition validity
            if (!add_internal_pot_projs_if_composition_valid(
                    sidb_cluster_projector_state{parent, static_cast<uint64_t>(m)}, m.compositions[0]))
            {
                return;
            }

            // check if composition exists
            sidb_cluster_charge_state_space::iterator it = parent->charge_space.find(m);
            if (it != parent->charge_space.cend())
            {
                it->compositions.emplace_back(m.compositions[0]);
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
            m += m_part;
            m.compositions[0].emplace_back(sidb_cluster_projector_state{cur_child, static_cast<uint64_t>(m_part)});
            add_valid_combinations_to_merged_charge_space(parent, cur_child_ix + 1, m);
            m -= m_part;
            m.compositions[0].pop_back();
        }
    }

    void construct_merged_charge_state_space(const sidb_cluster_ptr& parent) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.compositions.emplace_back(sidb_cluster_charge_state_composition{});
        add_valid_combinations_to_merged_charge_space(parent, 0, m);
    }

    template <sidb_charge_state onto_cs>
    void check_if_onto_cs_may_be_pruned(const sidb_cluster_ptr& parent) noexcept
    {
        for (const uint64_t sidb_ix : parent->sidbs)
        {
            if (!get_onto_cs_pruned_for_sidb_ix<onto_cs>(sidb_ix) &&
                !parent->pot_projs.at(parent->uid).orders[potential_projection_orders::cs_ix<onto_cs>()].count(sidb_ix))
            {
                prune_onto_cs_for_sidb_ix<onto_cs>(sidb_ix);
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void merge_pot_proj_bounds(const sidb_cluster_ptr& parent, const sidb_cluster_receptor_state& rst) const noexcept
    {
        if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(rst.sidb_ix))
        {
            return;
        }

        // construct external projected potential bounds for every composition of every element in the charge space
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const sidb_cluster_charge_state_composition& composition : m.compositions)
            {
                potential_projection pot_proj_onto_other_c{};

                for (const sidb_cluster_projector_state& child_pst : composition)
                {
                    pot_proj_onto_other_c += get_proj_state_bound<bound, onto_cs>(child_pst, rst);
                }

                add_pot_proj<onto_cs>(parent, rst, pot_proj_onto_other_c);
            }
        }

        // update the received external potential bound for the subject
        double diff = get_proj_bound<bound, onto_cs>(parent, rst).V;

        for (const sidb_cluster_ptr& child : parent->children)
        {
            diff -= get_proj_bound<bound, onto_cs>(child, rst).V;
        }

        rst.cluster->update_recv_ext_pot_bound<bound, onto_cs>(rst.sidb_ix, diff);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void merge_recv_pot_proj_bounds(const sidb_cluster_projector_state& pst,
                                    const sidb_cluster_ptr&             parent) const noexcept
    {
        if (onto_cs_pruned<onto_cs>(parent))
        {
            return;
        }

        for (const sidb_cluster_ptr& child : parent->children)
        {
            if (onto_cs_pruned<onto_cs>(child))
            {
                continue;
            }

            for (const uint64_t child_sidb_ix : child->sidbs)
            {
                const sidb_cluster_receptor_state& child_rst{child, child_sidb_ix};

                if (get_onto_cs_pruned_for_sidb_ix<onto_cs>(child_rst.sidb_ix))
                {
                    continue;
                }

                add_pot_proj<onto_cs>(pst.cluster, sidb_cluster_receptor_state{parent, child_sidb_ix},
                                      get_proj_state_bound<bound, onto_cs>(pst, child_rst));
            }
        }
    }

    void construct_merged_potential_projections(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& non_child : clustering)
        {
            for (const uint64_t& sidb_ix : non_child->sidbs)
            {
                const sidb_cluster_receptor_state& rst{non_child, sidb_ix};

                merge_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(parent, rst);
            }

            for (const sidb_cluster_charge_state& m : non_child->charge_space)
            {
                const sidb_cluster_projector_state& pst{non_child, static_cast<uint64_t>(m)};

                merge_recv_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(pst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(pst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(pst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(pst, parent);
            }

            for (const sidb_cluster_ptr& child : parent->children)
            {
                non_child->pot_projs.erase(child->uid);
            }
        }
    }

    void move_up_hierarchy() noexcept
    {
        if (clustering.size() == 1)
        {
            terminate = true;
            return;
            //            return false;
        }

        // Find the parent with the minimum cluster size
        const sidb_cluster_ptr& min_parent =
            (*std::min_element(clustering.cbegin(), clustering.cend(),
                               [](const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2)
                               { return c1->parent->sidbs.size() < c2->parent->sidbs.size(); }))
                ->parent;

        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        compute_children_recv_bounds_without_siblings(min_parent);

        construct_merged_charge_state_space(min_parent);

        check_if_onto_cs_may_be_pruned<sidb_charge_state::NEGATIVE>(min_parent);
        check_if_onto_cs_may_be_pruned<sidb_charge_state::POSITIVE>(min_parent);
        check_if_onto_cs_may_be_pruned<sidb_charge_state::NEUTRAL>(min_parent);

        construct_merged_potential_projections(min_parent);

        // update charge spaces of other clusters
        //        const bool fixpoint = update_charge_spaces();

        clustering.emplace(min_parent);

        //        if (!fixpoint)
        //        {
        //            check_charge_space(min_parent);
        //            return false;
        //        }
        //
        //        return check_charge_space(min_parent);
    }

    std::pair<sidb_cluster_ptr, std::chrono::duration<double>> compute_ground_state_space() noexcept
    {
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            while (!terminate)
            {
                while (!update_charge_spaces())
                    ;
                move_up_hierarchy();
            }
        }

        return {*clustering.cbegin(), time_counter};
    }

    pruned_store    global_pruned_store{};
    sidb_clustering clustering{};
    sidb_clustering bot{};

  private:
    const charge_distribution_surface<Lyt> cds;

    const std::array<double, 4> mu_bounds_with_error;

    const uint64_t witness_partitioning_max_cluster_size;

    bool terminate = false;
};

}  // namespace fiction

#endif  // FICTION_GROUND_STATE_SPACE_HPP
