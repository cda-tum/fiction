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
                                const sidb_simulation_parameters& phys_params = sidb_simulation_parameters{}) noexcept :
            clustering{
                get_initial_clustering(to_sidb_cluster(sidb_cluster_hierarchy(lyt)), get_local_potential_bounds(lyt))},
            bot{clustering},
            cds{charge_distribution_surface<Lyt>{lyt}},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - phys_params.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_plus()}
    {}

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
                -local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
                -local_potential_bound_containers.second.get_local_potential_by_index(i).value());

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                if (j == i)
                {
                    c->pot_projs[j] = potential_projection_orders{true};
                    continue;
                }

                c->pot_projs[j] =
                    potential_projection_orders{local_potential_bound_containers.first.get_potential_by_indices(i, j)};
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

    template <sidb_charge_state onto_cs>
    static constexpr inline bool onto_cs_pruned(const sidb_cluster_ptr& c) noexcept
    {
        return c->pot_projs.at(c->uid).onto_cs_pruned<onto_cs>();
    }

    template <sidb_charge_state onto_cs>
    static inline void initialize_pot_proj_ord_onto_cs(const sidb_cluster_ptr& proj,
                                                       const sidb_cluster_ptr& recv) noexcept
    {
        if (onto_cs_pruned<onto_cs>(recv))
        {
            return;
        }

        for (const sidb_cluster_charge_state& m : recv->charge_space)
        {
            const sidb_cluster_receptor_state rst{recv, static_cast<uint64_t>(m)};

            if (rst.contains_cs<onto_cs>())
            {
                proj->pot_projs[recv->uid].initialize_pot_proj_ord<onto_cs>(rst.multiset_conf);
            }
        }
    }

    static inline void initialize_inter_cluster_pot_proj_ords(const sidb_cluster_ptr& c1,
                                                              const sidb_cluster_ptr& c2) noexcept
    {
        for (const auto& [proj, recv] : std::array<std::pair<const sidb_cluster_ptr&, const sidb_cluster_ptr&>, 2>{
                 std::make_pair(c1, c2), std::make_pair(c2, c1)})
        {
            initialize_pot_proj_ord_onto_cs<sidb_charge_state::NEGATIVE>(proj, recv);
            initialize_pot_proj_ord_onto_cs<sidb_charge_state::NEUTRAL>(proj, recv);
            initialize_pot_proj_ord_onto_cs<sidb_charge_state::POSITIVE>(proj, recv);
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_bound(const sidb_cluster_ptr&            c,
                                                                const sidb_cluster_receptor_state& rst) noexcept
    {
        return c->pot_projs.at(rst.cluster->uid).get<bound, onto_cs>(rst.multiset_conf);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_next_proj_pot_bound(const sidb_cluster_ptr&            c,
                                                           const sidb_cluster_receptor_state& rst) noexcept
    {
        return c->pot_projs.at(rst.cluster->uid).get_next<bound, onto_cs>(rst.multiset_conf).V;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_state_bound(const sidb_cluster_projector_state& pst,
                                                                      const sidb_cluster_receptor_state&  rst) noexcept
    {
        return pst.cluster->pot_projs.at(rst.cluster->uid)
            .get_pot_proj_for_m_conf<bound, onto_cs>(pst.multiset_conf, rst.multiset_conf);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void
    remove_all_cluster_charge_state_occurrences(const sidb_cluster_projector_state& rm_pst,
                                                const sidb_cluster_receptor_state&  rst) noexcept
    {
        rm_pst.cluster->pot_projs[rst.cluster->uid].remove_m_conf<onto_cs>(rm_pst.multiset_conf, rst.multiset_conf);
        rst.cluster->pot_projs[rm_pst.cluster->uid].remove_onto_m_conf_if_present<sidb_charge_state::NEGATIVE>(
            rm_pst.multiset_conf);
        rst.cluster->pot_projs[rm_pst.cluster->uid].remove_onto_m_conf_if_present<sidb_charge_state::POSITIVE>(
            rm_pst.multiset_conf);
        rst.cluster->pot_projs[rm_pst.cluster->uid].remove_onto_m_conf_if_present<sidb_charge_state::NEUTRAL>(
            rm_pst.multiset_conf);

        rm_pst.cluster->recv_ext_pot_bounds.erase(rm_pst.multiset_conf);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void add_pot_proj(const sidb_cluster_ptr& c, const sidb_cluster_receptor_state& rst,
                                              const potential_projection& pp) noexcept
    {
        c->pot_projs[rst.cluster->uid].add<onto_cs>(pp, rst.multiset_conf);
    }

    using pot_proj_order = potential_projection_orders::pot_proj_order;

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline void
    add_internal_pot_projs_and_set_recv_bound(const sidb_cluster_projector_state&      pst,
                                              const std::pair<pot_proj_order, double>& pps_and_recv_bound) noexcept
    {
        if (!is_potential_bound_bottom(pps_and_recv_bound.second))
        {
            pst.cluster->pot_projs[pst.cluster->uid].add_multiple<onto_cs>(pps_and_recv_bound.first, pst.multiset_conf);
            pst.cluster->meet_recv_ext_pot_bound<bound, onto_cs>(pst.multiset_conf, pps_and_recv_bound.second);
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static void update_external_pot_proj_if_bound_removed(const sidb_cluster_projector_state& pst,
                                                          const sidb_cluster_receptor_state&  rst) noexcept
    {
        const potential_projection& cur_bound = get_proj_bound<bound, onto_cs>(pst.cluster, rst);

        if (cur_bound.M == pst.multiset_conf)
        {
            rst.cluster->update_recv_ext_pot_bound<bound, onto_cs>(
                rst.multiset_conf, get_next_proj_pot_bound<bound, onto_cs>(pst.cluster, rst) - cur_bound.V);
        }
    }

    template <sidb_charge_state onto_cs>
    static void update_external_potential_projection(const sidb_cluster_projector_state& pst,
                                                     const sidb_cluster_receptor_state&  rst) noexcept
    {
        if (onto_cs_pruned<onto_cs>(rst.cluster) || !rst.contains_cs<onto_cs>())
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
    void handle_invalid_state(const sidb_cluster_projector_state& pst) const noexcept
    {
        remove_all_cluster_charge_state_occurrences<witness_cs>(pst, pst);  // verify

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == pst.cluster)
            {
                continue;
            }

            for (const sidb_cluster_charge_state& m : other_c->charge_space)
            {
                const sidb_cluster_receptor_state rst{other_c, static_cast<uint64_t>(m)};
                update_external_potential_projection<sidb_charge_state::NEGATIVE>(pst, rst);
                update_external_potential_projection<sidb_charge_state::POSITIVE>(pst, rst);
                update_external_potential_projection<sidb_charge_state::NEUTRAL>(pst, rst);
            }

            // to be extended
            if (pst.cluster->sidbs.size() == 1)
            {
                other_c->pot_projs[pst.cluster->uid].prune_cs<witness_cs>();
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_received_potential_bound(const sidb_cluster_projector_state& pst) noexcept
    {
        // sum the internal potential bound with the external potential bound
        return get_proj_state_bound<bound, onto_cs>(pst, pst).V +
               pst.cluster->get_recv_ext_pot_bound<bound, onto_cs>(pst.multiset_conf);
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

    bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst) const noexcept
    {
        if (pst.contains_cs<sidb_charge_state::NEGATIVE>() &&
            fail_onto_negative_charge(
                get_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(pst)))
        {
            handle_invalid_state<sidb_charge_state::NEGATIVE>(pst);
            return false;
        }

        if (pst.contains_cs<sidb_charge_state::POSITIVE>() &&
            fail_onto_positive_charge(
                get_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(pst)))
        {
            handle_invalid_state<sidb_charge_state::POSITIVE>(pst);
            return false;
        }

        if (pst.contains_cs<sidb_charge_state::NEUTRAL>() &&
            (ub_fail_onto_neutral_charge(
                 get_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(pst)) ||
             lb_fail_onto_neutral_charge(
                 get_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(pst))))
        {
            handle_invalid_state<sidb_charge_state::NEUTRAL>(pst);
            return false;
        }

        return true;
    }

    bool check_charge_space(const sidb_cluster_ptr& c) const noexcept
    {
        // skip if |charge space| = 1?
        if (c->charge_space.size() == 1)
        {
            return true;
        }

        bool fixpoint = true;

        std::vector<uint64_t> removed_ms{};
        removed_ms.reserve(c->charge_space.size());

        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            if (!perform_potential_bound_analysis(sidb_cluster_projector_state{c, static_cast<uint64_t>(m)}))
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

    bool update_charge_spaces(std::optional<sidb_cluster::uid_t> skip_cluster = std::nullopt) const noexcept
    {
        bool fixpoint = true;

        // make a pass over the clustering and see if the charge spaces contain invalid cluster charge states
        for (const sidb_cluster_ptr& c : clustering)
        {
            if (!skip_cluster.has_value() || skip_cluster.value() != c->uid)
            {
                fixpoint &= check_charge_space(c);
            }
        }

        return fixpoint;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void compute_recv_ext_pot_bound_without_siblings(const sidb_cluster_receptor_state& child_rst) const noexcept
    {
        if (onto_cs_pruned<onto_cs>(child_rst.cluster) || !child_rst.contains_cs<onto_cs>())
        {
            return;
        }

        double pot_without_siblings = 0;

        for (const sidb_cluster_ptr& non_sibling : clustering)
        {
            pot_without_siblings += get_proj_bound<bound, onto_cs>(non_sibling, child_rst).V;  // ?
        }

        child_rst.cluster->set_recv_ext_pot_bound<bound, onto_cs>(child_rst.multiset_conf, pot_without_siblings);
    }

    void compute_children_recv_bounds_without_siblings(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const sidb_cluster_charge_state& m : child->charge_space)
            {
                const sidb_cluster_receptor_state child_rst{child, static_cast<uint64_t>(m)};
                compute_recv_ext_pot_bound_without_siblings<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(
                    child_rst);
                compute_recv_ext_pot_bound_without_siblings<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(
                    child_rst);
                compute_recv_ext_pot_bound_without_siblings<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(
                    child_rst);
                compute_recv_ext_pot_bound_without_siblings<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(
                    child_rst);
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
    std::optional<std::pair<pot_proj_order, double>>
    validate_composition(const sidb_cluster_projector_state&          parent_pst,
                         const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        if (!parent_pst.contains_cs<onto_cs>())
        {
            return std::make_pair(pot_proj_order{}, potential_projection::top<bound>().V);
        }

        // construct internal potential projection bounds for this composition
        pot_proj_order internal_pot_proj_bounds{};

        // construct the joined bound on the external and internal projected potential onto onto_cs for this
        // composition, and remember which cluster came with this bound
        double                             total_proj_pot_bound = potential_projection::top<bound>().V;
        const sidb_cluster_receptor_state* bound_rst            = nullptr;

        for (const sidb_cluster_receptor_state& rst : composition)
        {
            if (onto_cs_pruned<onto_cs>(rst.cluster) || !rst.contains_cs<onto_cs>())
            {
                continue;
            }

            potential_projection int_pot_proj_onto_child{};  // verify 37->pp37 at 4294967296; contains 8589934592

            for (const sidb_cluster_projector_state& pst : composition)
            {
                int_pot_proj_onto_child += get_proj_state_bound<bound, onto_cs>(pst, rst);
            }

            internal_pot_proj_bounds.emplace(int_pot_proj_onto_child);

            const double total_proj_pot_bound_candidate =
                rst.cluster->get_recv_ext_pot_bound<bound, onto_cs>(rst.multiset_conf) + int_pot_proj_onto_child.V;

            if (potential_bound_meet_is_weaker<bound>(total_proj_pot_bound, total_proj_pot_bound_candidate))
            {
                total_proj_pot_bound = total_proj_pot_bound_candidate;
                bound_rst            = &rst;
            }
        }

        // validate the current charge space candidate
        if (perform_potential_bound_analysis_for_composition<bound, onto_cs>(parent_pst, total_proj_pot_bound))
        {
            // invalid
            return std::nullopt;
        }

        // valid: return the collected potential projection bounds and the
        return std::make_pair(internal_pot_proj_bounds,
                              bound_rst->cluster->get_recv_ext_pot_bound<bound, onto_cs>(bound_rst->multiset_conf));
    }

    bool
    add_internal_pot_projs_if_composition_valid(const sidb_cluster_projector_state&          parent_pst,
                                                const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        // returned order empty means invalid multiset combination -- do not add to the merged charge state space
        const std::optional<std::pair<pot_proj_order, double>>& neg_ord_and_recv_ext_pot_bound =
            validate_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(parent_pst, composition);
        if (!neg_ord_and_recv_ext_pot_bound.has_value())
        {
            return false;
        }
        const std::optional<std::pair<pot_proj_order, double>>& pos_ord_and_recv_ext_pot_bound =
            validate_composition<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(parent_pst, composition);
        if (!pos_ord_and_recv_ext_pot_bound.has_value())
        {
            return false;
        }
        const std::optional<std::pair<pot_proj_order, double>>& neut_ub_ord_and_recv_ext_pot_bound =
            validate_composition<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(parent_pst, composition);
        if (!neut_ub_ord_and_recv_ext_pot_bound.has_value())
        {
            return false;
        }
        const std::optional<std::pair<pot_proj_order, double>>& neut_lb_ord_and_recv_ext_pot_bound =
            validate_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(parent_pst, composition);
        if (!neut_lb_ord_and_recv_ext_pot_bound.has_value())
        {
            return false;
        }

        // charge space candidate validated, add the internal potential projection bounds
        add_internal_pot_projs_and_set_recv_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(
            parent_pst, neg_ord_and_recv_ext_pot_bound.value());
        add_internal_pot_projs_and_set_recv_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(
            parent_pst, pos_ord_and_recv_ext_pot_bound.value());
        add_internal_pot_projs_and_set_recv_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(
            parent_pst, neut_lb_ord_and_recv_ext_pot_bound.value());
        add_internal_pot_projs_and_set_recv_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(
            parent_pst, neut_ub_ord_and_recv_ext_pot_bound.value());

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

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static void merge_pot_proj_bounds(const sidb_cluster_ptr& parent, const sidb_cluster_receptor_state& rst) noexcept
    {
        if (onto_cs_pruned<onto_cs>(rst.cluster) || !rst.contains_cs<onto_cs>())
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
            diff -= get_proj_bound<bound, onto_cs>(child, rst).V;  // wrong?
        }

        rst.cluster->update_recv_ext_pot_bound<bound, onto_cs>(rst.multiset_conf, diff);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static void merge_recv_pot_proj_bounds(const sidb_cluster_projector_state& pst,
                                           const sidb_cluster_ptr&             parent) noexcept
    {
        if (onto_cs_pruned<onto_cs>(parent))
        {
            return;
        }

        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            const sidb_cluster_receptor_state& rst{parent, static_cast<uint64_t>(m)};

            if (!rst.contains_cs<onto_cs>())
            {
                continue;
            }

            for (const sidb_cluster_charge_state_composition& composition : m.compositions)
            {
                for (const sidb_cluster_receptor_state& child_rst : composition)
                {
                    if (onto_cs_pruned<onto_cs>(child_rst.cluster) || !child_rst.contains_cs<onto_cs>())
                    {
                        continue;
                    }

                    add_pot_proj<onto_cs>(pst.cluster, rst, get_proj_state_bound<bound, onto_cs>(pst, child_rst));
                }
            }
        }
    }

    void construct_merged_potential_projections(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& non_child : clustering)
        {
            initialize_inter_cluster_pot_proj_ords(parent, non_child);

            for (const sidb_cluster_charge_state& m : non_child->charge_space)
            {
                const sidb_cluster_receptor_state& rst{non_child, static_cast<uint64_t>(m)};

                merge_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(parent, rst);
                merge_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(parent, rst);

                merge_recv_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(rst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(rst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(rst, parent);
                merge_recv_pot_proj_bounds<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(rst, parent);
            }

            for (const sidb_cluster_ptr& child : parent->children)
            {
                non_child->pot_projs.erase(child->uid);
            }
        }
    }

    bool move_up_hierarchy() noexcept
    {
        if (clustering.size() == 1)
        {
            return false;
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

        construct_merged_potential_projections(min_parent);

        clustering.emplace(min_parent);

        // update the charge spaces of the other clusters
        return update_charge_spaces(min_parent->uid);
    }

    std::pair<sidb_cluster_ptr, std::chrono::duration<double>> compute_ground_state_space() noexcept
    {
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            while (clustering.size() > 1)
            {
                while (!update_charge_spaces())
                    ;
                while (move_up_hierarchy())
                    ;
            }
        }

        return {*clustering.cbegin(), time_counter};
    }

    sidb_clustering clustering{};
    sidb_clustering bot{};

  private:
    const charge_distribution_surface<Lyt> cds;

    const std::array<double, 4> mu_bounds_with_error;
};

}  // namespace fiction

#endif  // FICTION_GROUND_STATE_SPACE_HPP
