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
                {-local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
                 -local_potential_bound_containers.second.get_local_potential_by_index(i).value()});

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

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_bound(const sidb_cluster_ptr& c1,
                                                                const sidb_cluster_ptr& c2) noexcept
    {
        return c1->pot_projs.at(c2->uid).get<bound, onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_next_proj_pot_bound(const sidb_cluster_ptr& c1,
                                                           const sidb_cluster_ptr& c2) noexcept
    {
        return c1->pot_projs.at(c2->uid).get_next<bound, onto_cs>().V;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline potential_projection get_proj_state_bound(const sidb_cluster_projector_state& pst,
                                                                      const sidb_cluster_ptr&             c2) noexcept
    {
        return *pst.projector->pot_projs.at(c2->uid).get_pot_proj_for_m_conf<bound, onto_cs>(pst.multiset_conf);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void remove_all_cluster_charge_state_occurrences(const sidb_cluster_projector_state& pst,
                                                                             const sidb_cluster_ptr& c2) noexcept
    {
        pst.projector->pot_projs[c2->uid].remove_m_conf<onto_cs>(pst.multiset_conf);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline void add_pot_proj(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                              const potential_projection& pp) noexcept
    {
        c1->pot_projs[c2->uid].add<onto_cs>(pp);
    }

    using pot_proj_ord = potential_projection_orders::pot_proj_order;

    template <sidb_charge_state onto_cs>
    static constexpr inline void add_internal_pot_projs(const sidb_cluster_ptr& c, const pot_proj_ord& pps) noexcept
    {
        c->pot_projs[c->uid].add_multiple<onto_cs>(pps);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline bool onto_cs_pruned(const sidb_cluster_ptr& c) noexcept
    {
        return c->pot_projs.at(c->uid).onto_cs_pruned<onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static void update_external_pot_proj_if_bound_removed(const sidb_cluster_projector_state& pst,
                                                          const sidb_cluster_ptr&             other_c) noexcept
    {
        const potential_projection& cur_bound = get_proj_bound<bound, onto_cs>(pst.projector, other_c);

        if (cur_bound.M == pst.multiset_conf)
        {
            //            std::cout << "removing bound " << pst.projector->uid << " to " << other_c->uid << ": " <<
            //            cur_bound.V
            //                      << " (" << cur_bound.M << ")";
            other_c->update_recv_ext_pot_bound<bound, onto_cs>(
                get_next_proj_pot_bound<bound, onto_cs>(pst.projector, other_c) - cur_bound.V);
            //            std::cout << "\tnew bound: " << get_next_proj_pot_bound<bound, onto_cs>(pst.projector,
            //            other_c)
            //                      << " to give recv pot: " << other_c->get_recv_ext_pot_bound<bound, onto_cs>() <<
            //                      std::endl;
        }
    }

    template <sidb_charge_state onto_cs>
    static void update_external_potential_projection(const sidb_cluster_projector_state& pst,
                                                     const sidb_cluster_ptr&             other_c) noexcept
    {
        if (onto_cs_pruned<onto_cs>(other_c))
        {
            return;
        }

        if constexpr (onto_cs != sidb_charge_state::POSITIVE)
        {
            update_external_pot_proj_if_bound_removed<bound_calculation_mode::LOWER, onto_cs>(pst, other_c);
        }
        if constexpr (onto_cs != sidb_charge_state::NEGATIVE)
        {
            update_external_pot_proj_if_bound_removed<bound_calculation_mode::UPPER, onto_cs>(pst, other_c);
        }

        remove_all_cluster_charge_state_occurrences<onto_cs>(pst, other_c);
    }

    template <sidb_charge_state witness_cs>
    void handle_invalid_state(const sidb_cluster_projector_state& pst) const noexcept
    {
        remove_all_cluster_charge_state_occurrences<witness_cs>(pst, pst.projector);

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == pst.projector)
            {
                continue;
            }

            update_external_potential_projection<sidb_charge_state::NEGATIVE>(pst, other_c);
            update_external_potential_projection<sidb_charge_state::POSITIVE>(pst, other_c);
            update_external_potential_projection<sidb_charge_state::NEUTRAL>(pst, other_c);

            // to be extended
            if (pst.projector->sidbs.size() == 1)
            {
                other_c->pot_projs[pst.projector->uid].prune_cs<witness_cs>();
            }
        }

        //        if (pst.projector->sidbs.size() == 1)  // should be able to omit this
        //        {
        //            pst.projector->recv_ext_pot_bounds[potential_projection_orders::cs_ix<witness_cs>()] = {
        //                potential_projection::top<bound_calculation_mode::LOWER>().V,
        //                potential_projection::top<bound_calculation_mode::UPPER>().V};
        //        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr inline double get_received_potential_bound(const sidb_cluster_projector_state& pst) noexcept
    {
        // sum the internal potential bound with the external potential bound
        return get_proj_state_bound<bound, onto_cs>(pst, pst.projector).V +
               pst.projector->get_recv_ext_pot_bound<bound, onto_cs>();
    }

    constexpr inline bool fail_onto_negative_charge(const sidb_cluster_projector_state& pst,
                                                    const double                        pot_bound) const noexcept
    {
        // V > e - mu-
        return pst.contains_cs<sidb_charge_state::NEGATIVE>() && pot_bound > mu_bounds_with_error[0];
    }

    constexpr inline bool fail_onto_positive_charge(const sidb_cluster_projector_state& pst,
                                                    const double                        pot_bound) const noexcept
    {
        // V < -e - mu+
        return pst.contains_cs<sidb_charge_state::POSITIVE>() && pot_bound < mu_bounds_with_error[3];
    }

    constexpr inline bool ub_fail_onto_neutral_charge(const sidb_cluster_projector_state& pst,
                                                      const double                        pot_bound) const noexcept
    {
        // V < -e - mu-
        return pst.contains_cs<sidb_charge_state::NEUTRAL>() && pot_bound < mu_bounds_with_error[1];
    }

    constexpr inline bool lb_fail_onto_neutral_charge(const sidb_cluster_projector_state& pst,
                                                      const double                        pot_bound) const noexcept
    {
        // V > e - mu+
        return pst.contains_cs<sidb_charge_state::NEUTRAL>() && pot_bound > mu_bounds_with_error[2];
    }

    bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst) const noexcept
    {
        if (fail_onto_negative_charge(
                pst, get_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(pst)))
        {
            handle_invalid_state<sidb_charge_state::NEGATIVE>(pst);
            return false;
        }

        if (fail_onto_positive_charge(
                pst, get_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(pst)))
        {
            handle_invalid_state<sidb_charge_state::POSITIVE>(pst);
            return false;
        }

        if (ub_fail_onto_neutral_charge(
                pst, get_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(pst)) ||
            lb_fail_onto_neutral_charge(
                pst, get_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(pst)))
        {
            handle_invalid_state<sidb_charge_state::NEUTRAL>(pst);
            return false;
        }

        return true;
    }

    bool check_charge_space(const sidb_cluster_ptr& c) const noexcept
    {
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
            // skip if |charge space| = 1?
            if (c->charge_space.size() > 1 || (skip_cluster.has_value() && skip_cluster.value() == c->uid))
            {
                fixpoint &= check_charge_space(c);
            }
        }

        return fixpoint;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    constexpr inline bool perform_potential_bound_analysis_for_composition(const sidb_cluster_projector_state& pst,
                                                                           const double pot_bound) const noexcept
    {
        if constexpr (onto_cs == sidb_charge_state::NEGATIVE)
        {
            return fail_onto_negative_charge(pst, pot_bound);
        }
        else if constexpr (onto_cs == sidb_charge_state::POSITIVE)
        {
            return fail_onto_positive_charge(pst, pot_bound);
        }
        else if constexpr (bound == bound_calculation_mode::LOWER)
        {
            return lb_fail_onto_neutral_charge(pst, pot_bound);
        }
        else if constexpr (bound == bound_calculation_mode::UPPER)
        {
            return ub_fail_onto_neutral_charge(pst, pot_bound);
        }

        return false;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    std::optional<pot_proj_ord>
    validate_composition(const sidb_cluster_projector_state&          composition_owner,
                         const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        if (!composition_owner.contains_cs<onto_cs>())
        {
            return pot_proj_ord{};
        }

        // construct internal potential projection bounds for this composition
        pot_proj_ord internal_pot_proj_bounds{};

        double total_proj_pot_bound = potential_projection::bottom<bound>().V;

        for (const sidb_cluster_projector_state& pst : composition)
        {
            if (onto_cs_pruned<onto_cs>(pst.projector) || !pst.contains_cs<onto_cs>())
            {
                continue;
            }

            potential_projection int_pot_proj_onto_child{};

            double ext_recv_pot_bound_without_siblings = pst.projector->get_recv_ext_pot_bound<bound, onto_cs>();

            for (const sidb_cluster_projector_state& other_pst : composition)
            {
                const potential_projection& pst_bound_onto_sibling =
                    get_proj_state_bound<bound, onto_cs>(other_pst, pst.projector);

                int_pot_proj_onto_child += pst_bound_onto_sibling;

                if (other_pst.projector != pst.projector)
                {
                    ext_recv_pot_bound_without_siblings -= pst_bound_onto_sibling.V;
                }
            }

            internal_pot_proj_bounds.emplace(int_pot_proj_onto_child);

            join_potential_bounds<bound>(total_proj_pot_bound,
                                         ext_recv_pot_bound_without_siblings + int_pot_proj_onto_child.V);
        }

        // validate the current charge space candidate
        if (perform_potential_bound_analysis_for_composition<bound, onto_cs>(composition_owner, total_proj_pot_bound))
        {
            // invalid
            return std::nullopt;
        }

        // valid
        return internal_pot_proj_bounds;
    }

    std::optional<pot_proj_ord>
    validate_composition_onto_neutral(const sidb_cluster_projector_state&          composition_owner,
                                      const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        if (!composition_owner.contains_cs<sidb_charge_state::NEUTRAL>())
        {
            return pot_proj_ord{};
        }

        // construct internal potential projection bounds for this composition
        pot_proj_ord internal_pot_proj_bounds{};

        double total_proj_pot_lb = potential_projection::bottom<bound_calculation_mode::LOWER>().V;
        double total_proj_pot_ub = potential_projection::bottom<bound_calculation_mode::UPPER>().V;

        for (const sidb_cluster_projector_state& pst : composition)
        {
            if (onto_cs_pruned<sidb_charge_state::NEUTRAL>(pst.projector) ||
                !pst.contains_cs<sidb_charge_state::NEUTRAL>())
            {
                continue;
            }

            potential_projection int_pot_proj_onto_child_lb{};
            potential_projection int_pot_proj_onto_child_ub{};

            double ext_recv_pot_lb_without_siblings =
                pst.projector->get_recv_ext_pot_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>();
            double ext_recv_pot_ub_without_siblings =
                pst.projector->get_recv_ext_pot_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>();

            for (const sidb_cluster_projector_state& other_pst : composition)
            {
                const potential_projection& pst_lb_onto_sibling =
                    get_proj_state_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(other_pst,
                                                                                                    pst.projector);

                const potential_projection& pst_ub_onto_sibling =
                    get_proj_state_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(other_pst,
                                                                                                    pst.projector);

                int_pot_proj_onto_child_lb += pst_lb_onto_sibling;
                int_pot_proj_onto_child_ub += pst_ub_onto_sibling;

                if (other_pst.projector != pst.projector)
                {
                    ext_recv_pot_lb_without_siblings -= pst_lb_onto_sibling.V;
                    ext_recv_pot_ub_without_siblings -= pst_ub_onto_sibling.V;
                }
            }

            internal_pot_proj_bounds.emplace(int_pot_proj_onto_child_lb);
            internal_pot_proj_bounds.emplace(int_pot_proj_onto_child_ub);

            join_potential_bounds<bound_calculation_mode::LOWER>(total_proj_pot_lb, ext_recv_pot_lb_without_siblings +
                                                                                        int_pot_proj_onto_child_lb.V);
            join_potential_bounds<bound_calculation_mode::UPPER>(total_proj_pot_ub, ext_recv_pot_ub_without_siblings +
                                                                                        int_pot_proj_onto_child_ub.V);
        }

        // validate the current charge space candidate
        if (perform_potential_bound_analysis_for_composition<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(
                composition_owner, total_proj_pot_ub) ||
            perform_potential_bound_analysis_for_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(
                composition_owner, total_proj_pot_lb))
        {
            // invalid
            return std::nullopt;
        }

        // valid
        return internal_pot_proj_bounds;
    }

    bool
    add_internal_pot_projs_if_composition_valid(const sidb_cluster_projector_state&          pst,
                                                const sidb_cluster_charge_state_composition& composition) const noexcept
    {
        // returned order empty means invalid multiset combination -- do not add to the merged charge state space
        const std::optional<pot_proj_ord>& neg_ord =
            validate_composition<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(pst, composition);
        if (!neg_ord.has_value())
        {
            return false;
        }
        const std::optional<pot_proj_ord>& pos_ord =
            validate_composition<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(pst, composition);
        if (!pos_ord.has_value())
        {
            return false;
        }
        const std::optional<pot_proj_ord>& neut_ord = validate_composition_onto_neutral(pst, composition);
        if (!neut_ord.has_value())
        {
            return false;
        }

        // charge space candidate validated, add the internal potential projection bounds
        add_internal_pot_projs<sidb_charge_state::NEGATIVE>(pst.projector, neg_ord.value());
        add_internal_pot_projs<sidb_charge_state::POSITIVE>(pst.projector, pos_ord.value());
        add_internal_pot_projs<sidb_charge_state::NEUTRAL>(pst.projector, neut_ord.value());

        return true;
    }

    void add_valid_combinations_to_merged_charge_space(const sidb_cluster_ptr& c, const uint64_t current_child_ix,
                                                       sidb_cluster_charge_state& m) const noexcept
    {
        if (current_child_ix >= c->children.size())
        {
            // verify composition validity
            if (!add_internal_pot_projs_if_composition_valid(sidb_cluster_projector_state{c, static_cast<uint64_t>(m)},
                                                             m.compositions[0]))
            {
                return;
            }

            // check if composition exists
            sidb_cluster_charge_state_space::iterator it = c->charge_space.find(m);
            if (it != c->charge_space.cend())
            {
                it->compositions.emplace_back(m.compositions[0]);
            }
            else
            {
                c->charge_space.emplace(m);
            }
            return;
        }

        const sidb_cluster_ptr& cur_child = *std::next(c->children.cbegin(), static_cast<int64_t>(current_child_ix));

        for (const sidb_cluster_charge_state& m_part : cur_child->charge_space)
        {
            m += m_part;
            m.compositions[0].emplace_back(sidb_cluster_projector_state{cur_child, static_cast<uint64_t>(m_part)});
            add_valid_combinations_to_merged_charge_space(c, current_child_ix + 1, m);
            m -= m_part;
            m.compositions[0].pop_back();
        }
    }

    void construct_merged_charge_state_space(const sidb_cluster_ptr& c) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.compositions.emplace_back(sidb_cluster_charge_state_composition{});
        add_valid_combinations_to_merged_charge_space(c, 0, m);
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    static void construct_merged_pot_projs_onto_cs(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c) noexcept
    {
        if (onto_cs_pruned<cs>(other_c))
        {
            return;
        }

        // construct external projected potential bounds for every composition of every element in the charge space
        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            for (const sidb_cluster_charge_state_composition& composition : m.compositions)
            {
                potential_projection pot_proj_onto_other_c{};

                for (const sidb_cluster_projector_state& pst : composition)
                {
                    pot_proj_onto_other_c += get_proj_state_bound<bound, cs>(pst, other_c);
                }

                add_pot_proj<cs>(c, other_c, pot_proj_onto_other_c);
            }
        }

        // update the received external potential bound
        double diff = get_proj_bound<bound, cs>(c, other_c).V;

        for (const sidb_cluster_ptr& child : c->children)
        {
            diff -= get_proj_bound<bound, cs>(child, other_c).V;
        }

        other_c->update_recv_ext_pot_bound<bound, cs>(diff);
    }

    void construct_merged_potential_projections(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            construct_merged_pot_projs_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, other_c);
            construct_merged_pot_projs_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, other_c);
            construct_merged_pot_projs_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c, other_c);
            construct_merged_pot_projs_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c, other_c);

            for (const sidb_cluster_ptr& child : c->children)
            {
                other_c->pot_projs.at(child->uid)
                    .copy_to_parent<sidb_charge_state::NEGATIVE>(other_c->pot_projs[c->uid]);
                other_c->pot_projs.at(child->uid)
                    .copy_to_parent<sidb_charge_state::POSITIVE>(other_c->pot_projs[c->uid]);
                other_c->pot_projs.at(child->uid)
                    .copy_to_parent<sidb_charge_state::NEUTRAL>(other_c->pot_projs[c->uid]);
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void compute_relaxed_recv_external_pot_bound(const sidb_cluster_ptr& c) const noexcept
    {
        if (onto_cs_pruned<onto_cs>(c))
        {
            c->set_recv_ext_pot_bound<bound, onto_cs>(potential_projection::bottom<bound>().V);
            //            std::cout << "bot" << std::endl;
            return;
        }

        double recv_pot_sum = 0;

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            recv_pot_sum += get_proj_bound<bound, onto_cs>(other_c, c).V;
            //            std::cout << other_c->uid << " gives " << get_proj_bound<bound, onto_cs>(other_c, c).V << " to
            //            make "
            //                      << recv_pot_sum << std::endl;
        }

        c->set_recv_ext_pot_bound<bound, onto_cs>(recv_pot_sum);
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

        //        std::cout << "\n\nmerged " << (*min_parent->children.cbegin())->uid << " and "
        //                  << (*std::next(min_parent->children.cbegin(), 1))->uid << " to " << min_parent->uid <<
        //                  std::endl;

        construct_merged_charge_state_space(min_parent);

        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        construct_merged_potential_projections(min_parent);

        // recompute the received potential bounds by summing the received bound from all other clusters; these bounds
        // are consistent with bound updates from other clusters later when we prune an element from a charge space
        //        std::cout << "RELAXED NEG LB" << std::endl;
        compute_relaxed_recv_external_pot_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(min_parent);
        //        std::cout << "RELAXED POS UB" << std::endl;
        compute_relaxed_recv_external_pot_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(min_parent);
        //        std::cout << "RELAXED NEUT LB" << std::endl;
        compute_relaxed_recv_external_pot_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(min_parent);
        //        std::cout << "RELAXED NEUT UB" << std::endl;
        compute_relaxed_recv_external_pot_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(min_parent);

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
