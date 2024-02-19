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
                                const sidb_simulation_parameters& phys_params = sidb_simulation_parameters{}) :
            clustering{
                get_initial_clustering(to_sidb_cluster(sidb_cluster_hierarchy(lyt)), get_local_potential_bounds(lyt))},
            bot{clustering},
            cds{charge_distribution_surface<Lyt>{lyt}},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - phys_params.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_plus()}
    {
        for (const sidb_cluster_ptr& c : clustering)
        {
            for (const sidb_cluster_ptr& other_c : clustering)
            {
                if (c >= other_c)
                {
                    continue;
                }

                std::shared_ptr<const std::vector<std::vector<double>>> pot_mat =
                    std::make_shared<const std::vector<std::vector<double>>>(
                        3, std::vector<double>(3, cds.get_chargeless_potential_by_indices(*c->sidbs.cbegin(),
                                                                                          *other_c->sidbs.cbegin())));
            }
        }
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

            c->initialize_singleton_cluster_charge_space();

            //            c->local_pot_bounds =
            //            {local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
            //                                   local_potential_bound_containers.second.get_local_potential_by_index(i).value()};

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                if (i == j)
                {
                    c->proj_pot_bounds[i] = projected_potential_bounds{true};
                    continue;
                }

                c->proj_pot_bounds[j] = projected_potential_bounds{
                    projector_bound<bound_calculation_mode::LOWER>{
                        local_potential_bound_containers.first.get_potential_by_indices(i, j),
                        sidb_charge_state::POSITIVE},
                    projector_bound<bound_calculation_mode::UPPER>{
                        local_potential_bound_containers.second.get_potential_by_indices(i, j),
                        sidb_charge_state::NEGATIVE}};
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

    // obtain the bottom element of the charge space of a singleton cluster c respective to the bound.
    template <bound_calculation_mode bound>
    static std::iterator<std::input_iterator_tag, sidb_charge_state>
    charge_state_iterator_respective_to_bound() noexcept
    {
        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            // + -> 0 -> -
            return sidb_charge_state_reversed_iterator{};
        }
        else
        {
            // - -> 0 -> +
            return sidb_charge_state_iterator{};
        }
    }

    // take the join of two potential bounds, which is semantically the bound with most information (the strictest one)
    template <bound_calculation_mode bound>
    static inline void join_bounds(double& a, const double b) noexcept
    {
        if constexpr (bound == bound_calculation_mode::LOWER)
        {
            a = std::max(a, b);
        }
        else
        {
            a = std::min(a, b);
        }
    }

    // take the join of two projector bounds
    template <bound_calculation_mode bound>
    static inline void join_projector_bounds(projector_bound<bound>& a, const projector_bound<bound>& b) noexcept
    {
        if (a < b)
        {
            a = b;
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    inline constexpr projector_bound<bound> get_proj_bound(const sidb_cluster_ptr& c1,
                                                           const sidb_cluster_ptr& c2) const noexcept
    {
        return c1->proj_pot_bounds.at(c2->uid).get<bound, onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    inline void set_proj_bound(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                               const projector_bound<bound>& new_bound) const noexcept
    {
        c1->proj_pot_bounds[c2->uid].add<bound, onto_cs>(new_bound);
    }

    //    // develop the bound of the projected potential of c1 onto an SiDB j recursively through summation.
    //    template <bound_calculation_mode bound>
    //    void develop_proj_pot_bound_onto_sidb(double& dev_proj_pot_bound_sum, const sidb_cluster_ptr& c1,
    //                                          const sidb_cluster_ptr& j) const noexcept
    //    {
    //        if (c1->sidbs.size() == 1)
    //        {
    //            // find the bottom element in the charge space of c1, i.e. the charge state imposing the weakest bound
    //            for (const sidb_charge_state cs : charge_state_iterator_respective_to_bound<bound>())
    //            {
    //                if (c1->charge_space.find(sidb_cluster_charge_state{cs}) != c1->charge_space.cend())
    //                {
    //                    dev_proj_pot_bound_sum +=
    //                        charge_state_to_sign(cs) * cds.get_chargeless_potential_by_indices(*c1->sidbs.cbegin(),
    //                        j);
    //                    return;
    //                }
    //            }
    //
    //            assert(false);  // charge spaces may not be empty
    //        }
    //
    //        // develop the sum of potential of c1's children onto j
    //        for (const sidb_cluster_ptr& c1_child : c1->children)
    //        {
    //            develop_proj_pot_bound_onto_sidb<bound>(dev_proj_pot_bound_sum, c1_child, j);
    //        }
    //    }
    //
    //    // develop the bound of c1's projected potential onto a charge state onto_cs~c2 recursively by joining c2's
    //    children template <bound_calculation_mode bound, sidb_charge_state onto_cs> void
    //    develop_proj_pot_bound(double& dev_proj_pot_bound_join, const sidb_cluster_ptr& c1,
    //                                const sidb_cluster_ptr& c2) const noexcept
    //    {
    //        if (c2->sidbs.size() == 1)
    //        {
    //            // check that onto_cs is contained in the charge space of singleton cluster c2
    //            if (c2->charge_space.find(sidb_cluster_charge_state{onto_cs}) == c2->charge_space.cend())
    //            {
    //                return;
    //            }
    //
    //            double proj_pot_bound_sum = 0;
    //            develop_proj_pot_bound_onto_sidb<bound>(proj_pot_bound_sum, c1, *c2->sidbs.cbegin());
    //
    //            join_bounds<bound>(dev_proj_pot_bound_join, proj_pot_bound_sum);
    //            return;
    //        }
    //
    //        // develop the least upper bound of the potential of c1 onto a charge state onto_cs in c2's children
    //        for (const sidb_cluster_ptr& c2_child : c2->children)
    //        {
    //            develop_proj_pot_bound<bound, onto_cs>(dev_proj_pot_bound_join, c1, c2_child);
    //        }
    //    }

    // develop the bound on the value of the potential projected from c1 with cluster charge state m onto SiDB j
    template <bound_calculation_mode bound>
    void develop_projected_pot_bound_value_onto_sidb(double& dev_proj_pot_bound_join, double& dev_proj_pot_bound_sum,
                                                     const sidb_cluster_projector_state& pst,
                                                     const uint64_t                      j) const noexcept
    {
        // check if pst is ({i}, {{z}})
        if (is_trinary_multiset_conf(pst.multiset_conf))
        {
            // return z * Vij
            dev_proj_pot_bound_sum += trinary_multiset_conf_to_sign(pst.multiset_conf) *
                                      cds.get_chargeless_potential_by_indices(*pst.projector->sidbs.cbegin(), j);
            return;
        }

        // take the strongest bound on the sum of potential of c1's children onto j, for all decompositions
        for (const sidb_cluster_charge_state_decomposition& decomposition : get_projector_state_decompositions(pst))
        {
            // compute the sum of potential of c1's children onto j for this decomposition
            double child_proj_pot_sum_bound = 0;

            for (const sidb_cluster_projector_state& pst_child : decomposition)
            {
                develop_projected_pot_bound_value_onto_sidb<bound>(dev_proj_pot_bound_sum, child_proj_pot_sum_bound,
                                                                   pst_child, j);
            }

            join_bounds<bound>(dev_proj_pot_bound_join, dev_proj_pot_bound_sum);
        }
    }

    // compute the bound on the value of the potential projected from c1 with cluster charge state m onto onto_cs in c2
    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void develop_projected_pot_bound_value(double& dev_proj_pot_bound_join, const sidb_cluster_projector_state& pst,
                                           const sidb_cluster_ptr& c2) const noexcept
    {
        if (c2->sidbs.size() == 1)
        {
            // check that onto_cs is contained in the charge space of singleton cluster c2
            if (c2->charge_space.find(sidb_cluster_charge_state{onto_cs}) == c2->charge_space.cend())
            {
                return;
            }

            double proj_pot_bound_sum = 0;
            develop_projected_pot_bound_value_onto_sidb<bound>(dev_proj_pot_bound_join, proj_pot_bound_sum, pst,
                                                               *c2->sidbs.cbegin());

            join_bounds<bound>(dev_proj_pot_bound_join, proj_pot_bound_sum);
        }

        // develop the least upper bound of the potential of c1 with ccs m onto a charge state onto_cs in c2's children
        for (const sidb_cluster_ptr& c2_child : c2->children)
        {
            develop_projected_pot_bound_value<bound, onto_cs>(dev_proj_pot_bound_join, pst, c2_child);
        }
    }

    // compute the bound on the value of the potential projected from c1 with cluster charge state m onto onto_cs in c2
    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    projector_bound<bound> compute_projector_bound(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                                   const uint64_t m) const
    {
        projector_bound<bound> projected_bound{m};
        develop_projected_pot_bound_value<bound, onto_cs>(projected_bound.V, sidb_cluster_projector_state{c1, m}, c2);
        return projected_bound;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    double compute_received_potential_bound(const sidb_cluster_ptr& c) const noexcept
    {
        double sum = 0;

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            sum += get_proj_bound<bound, onto_cs>(other_c, c).V;
        }

        return sum;
    }

    sidb_charge_state check_for_failing_charge_state(const sidb_cluster_ptr&          c,
                                                     const sidb_cluster_charge_state& m) const noexcept
    {
        // NEG in m
        if (m.neg_count != 0)
        {
            // -V >= e - mu-
            if (-compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c) >=
                mu_bounds_with_error[0])
            {
                return sidb_charge_state::NEGATIVE;
            }
        }

        // POS in m
        if (m.pos_count != 0)
        {
            // -V <= -e - mu+
            if (-compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c) <=
                mu_bounds_with_error[3])
            {
                return sidb_charge_state::POSITIVE;
            }
        }

        // NEUT in m
        if (m.contains_neutral_charge(c->sidbs.size()))
        {
            // -V <= -e - mu-  or  -V >= e - mu+
            if (-compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c) <=
                    mu_bounds_with_error[1] ||
                -compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c) >=
                    mu_bounds_with_error[2])
            {
                return sidb_charge_state::NEUTRAL;
            }
        }

        return sidb_charge_state::NONE;
    }

    using charge_spaces_update_result = std::vector<std::pair<sidb_cluster_ptr, std::vector<uint64_t>>>;

    void check_charge_space(const sidb_cluster_ptr& c, charge_spaces_update_result& res) const noexcept
    {
        std::vector<uint64_t> removed_multisets{};
        removed_multisets.reserve(c->charge_space.size());

        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            const sidb_charge_state cs = check_for_failing_charge_state(c, m);

            if (cs != sidb_charge_state::NONE)
            {
                removed_multisets.push_back(static_cast<uint64_t>(m));
            }
        }

        if (!removed_multisets.empty())
        {
            res.push_back({c, std::move(removed_multisets)});
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    projector_bound<bound> find_new_projector_bound(const sidb_cluster_ptr& c1,
                                                    const sidb_cluster_ptr& c2) const noexcept
    {
        projector_bound<bound> new_proj_bound = projector_bound<bound>();

        for (const sidb_cluster_charge_state& maybe_bound_m : c1->charge_space)
        {
            join_projector_bounds<bound>(
                new_proj_bound, compute_projector_bound<bound, onto_cs>(c1, c2, static_cast<uint64_t>(maybe_bound_m)));
        }

        return new_proj_bound;
    }

    //    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    //    void update_projected_bound(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
    //                                const std::vector<uint64_t>& removed_ms) const noexcept
    //    {
    //        const auto& [new_pot_bound, new_pot_bound_m] = find_new_interaction_bound<bound, onto_cs>(c1, c2,
    //        removed_ms);
    //
    //        //        c2->local_pot_bounds[static_cast<uint64_t>(bound)] +=
    //        //            new_pot_bound -
    //        //            c1->projected_pot_bounds.at(c2->uid).onto_cs[onto_cs].V[static_cast<uint64_t>(bound)];
    //
    //        c1->proj_pot_bounds.at(c2->uid).onto_cs[onto_cs].V[static_cast<uint64_t>(bound)] = new_pot_bound;
    //        c1->proj_pot_bounds.at(c2->uid).onto_cs[onto_cs].M[static_cast<uint64_t>(bound)] = new_pot_bound_m;
    //    }

    template <sidb_charge_state without_cs>
    void propagate_change_onto_singleton(const sidb_cluster_ptr& singleton) const noexcept
    {
        //            if (singleton->charge_space.find(sidb_cluster_charge_state{without_cs}) !=
        //            singleton->charge_space.cend())
        //            {
        //                assert(false && "how did we get here");
        //            }

        //            if (c->charge_space.empty())
        //            {
        //                return;
        //            }

        //            if (c->proj_pot_bounds.at(singleton->uid).onto_cs.at(without_cs).V[static_cast<uint64_t>(bound)] <
        //                c->proj_pot_bounds.at(singleton->parent->uid).onto_cs.at(without_cs).V[static_cast<uint64_t>(bounds)])
        for (const sidb_cluster_ptr& c : clustering)
        {
            set_proj_bound<bound_calculation_mode::LOWER, without_cs>(c, singleton,
                                                                      projector_bound<bound_calculation_mode::LOWER>());
            set_proj_bound<bound_calculation_mode::UPPER, without_cs>(c, singleton,
                                                                      projector_bound<bound_calculation_mode::UPPER>());
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void update_if_bound_is_removed(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c, const uint64_t check_m,
                                    std::vector<bool>& updated) const noexcept
    {
        if (!updated[static_cast<uint64_t>(bound)] && check_m == get_proj_bound<bound, onto_cs>(c, other_c).M)
        {
            set_proj_bound<bound, onto_cs>(c, other_c, find_new_projector_bound<bound, onto_cs>(c, other_c));
            //            propagate_change_to_through_all_projectors<bound, onto_cs>
            updated[static_cast<uint64_t>(bound)] = true;
        }
    }

    template <sidb_charge_state onto_cs>
    void apply_update_inter_cluster_onto_cs(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c,
                                            const std::vector<uint64_t>& removed_ms) const noexcept
    {
        std::vector<bool> updated(2, false);
        for (const uint64_t m : removed_ms)
        {
            if (updated[0] && updated[1])
            {
                break;
            }

            update_if_bound_is_removed<bound_calculation_mode::LOWER, onto_cs>(c, other_c, m, updated);
            update_if_bound_is_removed<bound_calculation_mode::UPPER, onto_cs>(c, other_c, m, updated);

            if (is_trinary_multiset_conf(m) && trinary_multiset_conf_to_sign(m) == charge_state_to_sign(onto_cs))
            {
                propagate_change_onto_singleton<onto_cs>(c);
            }
        }
    }

    void apply_update(const charge_spaces_update_result& update_res) const noexcept
    {
        for (const auto& [c, removed_ms] : update_res)
        {
            for (const uint64_t& m : removed_ms)
            {
                c->charge_space.erase(static_cast<sidb_cluster_charge_state>(m));
            }

            for (const sidb_cluster_ptr& other_c : clustering)
            {
                apply_update_inter_cluster_onto_cs<sidb_charge_state::NEGATIVE>(c, other_c, removed_ms);
                apply_update_inter_cluster_onto_cs<sidb_charge_state::POSITIVE>(c, other_c, removed_ms);
                apply_update_inter_cluster_onto_cs<sidb_charge_state::NEUTRAL>(c, other_c, removed_ms);
            }
        }
    }

    bool update_charge_spaces() const noexcept
    {
        charge_spaces_update_result update_res{};
        update_res.reserve(clustering.size());

        // make a pass over the clustering and see if the charge spaces contain invalid cluster charge states
        for (const sidb_cluster_ptr& c : clustering)
        {
            // skip if |charge space| = 1?
            if (c->charge_space.size() > 1)
            {
                check_charge_space(c, update_res);
            }
        }

        if (update_res.empty())
        {
            // fixpoint
            return true;
        }

        // update bounds and charge spaces ; assumes charge spaces can never become empty
        apply_update(update_res);

        return false;
    }

    void fill_merged_charge_state_space(const sidb_cluster_ptr& c, const uint64_t current_child_ix,
                                        sidb_cluster_charge_state& m) const noexcept
    {
        if (current_child_ix >= c->children.size())
        {
            sidb_cluster_charge_state_space::iterator it = c->charge_space.find(m);

            if (it != c->charge_space.end())
            {
                it->decompositions.emplace_back(m.decompositions[0]);
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
            m.decompositions[0].emplace_back(sidb_cluster_projector_state{cur_child, static_cast<uint64_t>(m_part)});
            fill_merged_charge_state_space(c, current_child_ix + 1, m);
            m -= m_part;
            m.decompositions[0].pop_back();
        }
    }

    void compute_merged_charge_state_space(const sidb_cluster_ptr& c) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.decompositions.emplace_back(sidb_cluster_charge_state_decomposition{});
        fill_merged_charge_state_space(c, 0, m);
    }

    //    void compute_merged_local_potential_bounds(const sidb_cluster_ptr& c) const noexcept
    //    {
    //        c->local_pot_bounds = {potential_bound_bottom<bound_calculation_mode::LOWER>(),
    //                               potential_bound_bottom<bound_calculation_mode::UPPER>()};
    //
    //        for (const sidb_cluster_ptr& child : c->children)
    //        {
    //            for (const sidb_charge_state cs :
    //                 {sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL, sidb_charge_state::POSITIVE})
    //            {
    //                for (const bound_calculation_mode bound :
    //                     {bound_calculation_mode::LOWER, bound_calculation_mode::UPPER})
    //                {
    //                    double pot_bound_without_siblings = child->local_pot_bounds[static_cast<uint64_t>(bound)];
    //
    //                    // should make use of the symmetric property of potentials between children
    //                    for (const sidb_cluster_ptr& other_child : c->children)
    //                    {
    //                        // is this correct?
    //                        if (child == other_child)
    //                        {
    //                            continue;
    //                        }
    //
    //                        pot_bound_without_siblings -= other_child->projected_pot_bounds.at(child->uid)
    //                                                          .onto_cs[cs]
    //                                                          .V[static_cast<uint64_t>(bound)];
    //                    }
    //
    //                    join_bounds<bound>(c->local_pot_bounds[static_cast<uint64_t>(bound)],
    //                    pot_bound_without_siblings);
    //                }
    //            }
    //        }
    //    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void construct_internal_pot_bound(const sidb_cluster_ptr& c) const noexcept
    {
        projector_bound<bound> internal_pot_bound{};

        for (const sidb_cluster_ptr& child : c->children)
        {
            projector_bound<bound> proj_bound_onto_child{0.0};

            for (const sidb_cluster_ptr& other_child : c->children)
            {
                proj_bound_onto_child += get_proj_bound<bound, onto_cs>(other_child, child);
            }

            join_projector_bounds<bound>(internal_pot_bound, proj_bound_onto_child);
        }

        set_proj_bound<bound, onto_cs>(c, c, internal_pot_bound);
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    void compute_merged_bound_onto_cs(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c) const noexcept
    {
        if (c == other_c)
        {
            construct_internal_pot_bound<bound, cs>(c);
            return;
        }

        projector_bound<bound> new_proj_bound{0.0};

        std::set<uint64_t> recv_m{};

        for (const sidb_cluster_ptr& child : c->children)
        {
            new_proj_bound += get_proj_bound<bound, cs>(child, other_c);

            recv_m.emplace(get_proj_bound<bound, cs>(other_c, child).M);
        }

        set_proj_bound<bound, cs>(c, other_c, new_proj_bound);

        // check for bound conflict
        if (recv_m.size() == 1)
        {
            // no conflict  .. construct instead?
            set_proj_bound<bound, cs>(other_c, c, compute_projector_bound<bound, cs>(other_c, c, *recv_m.cbegin()));
            return;
        }

        // conflict found, solve exhaustively (?)
        set_proj_bound<bound, cs>(other_c, c, find_new_projector_bound<bound, cs>(other_c, c));
    }

    void compute_merged_interaction_bounds(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            compute_merged_bound_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, other_c);
            compute_merged_bound_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::POSITIVE>(c, other_c);
            compute_merged_bound_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c, other_c);
            compute_merged_bound_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEGATIVE>(c, other_c);
            compute_merged_bound_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, other_c);
            compute_merged_bound_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c, other_c);
        }
    }

    void move_up_hierarchy() noexcept
    {
        // Find the parent with the minimum cluster size
        const sidb_cluster_ptr& min_parent =
            (*std::min_element(clustering.cbegin(), clustering.cend(),
                               [](const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2)
                               { return c1->parent->sidbs.size() < c2->parent->sidbs.size(); }))
                ->parent;

        clustering.emplace(min_parent);
        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        compute_merged_charge_state_space(min_parent);
        compute_merged_interaction_bounds(min_parent);
        //        compute_merged_local_potential_bounds(min_parent);

        // update the merged cluster charge space
        charge_spaces_update_result res{};
        check_charge_space(min_parent, res);
        apply_update(res);
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
                move_up_hierarchy();
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
