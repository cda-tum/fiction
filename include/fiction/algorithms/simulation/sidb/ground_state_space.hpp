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
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace fiction
{

template <typename Lyt>
class ground_state_space
{
  public:
    explicit ground_state_space(const Lyt&                        lyt,
                                const sidb_simulation_parameters& phys_params = sidb_simulation_parameters{}) noexcept :
            clustering{
                get_initial_clustering(to_sidb_cluster(sidb_cluster_hierarchy(lyt)), get_local_potential_bounds(lyt))},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_minus,
                                 physical_constants::POP_STABILITY_ERR - phys_params.mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - phys_params.mu_plus()}
    {}

    std::pair<sidb_cluster_ptr, std::chrono::duration<double>> compute_ground_state_space() noexcept
    {
        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            move_up_hierarchy();
        }

        return {*clustering.cbegin(), time_counter};
    }

  private:
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

    static sidb_clustering
    get_initial_clustering(const sidb_cluster_ptr& c,
                           const std::pair<charge_distribution_surface<Lyt>, charge_distribution_surface<Lyt>>&
                               local_potential_bound_containers) noexcept
    {
        sidb_clustering clustering{};

        if (c->size() == 1)
        {
            const uint64_t i = *c->sidbs.cbegin();

            c->initialize_singleton_cluster_charge_space(
                i, -local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
                -local_potential_bound_containers.second.get_local_potential_by_index(i).value());

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
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

    template <bound_direction bound>
    static constexpr inline potential_projection get_proj_bound(const sidb_cluster_ptr&            c,
                                                                const sidb_cluster_receptor_state& rst) noexcept
    {
        return c->pot_projs.at(rst.cluster->uid).get<bound>(rst.sidb_ix);
    }

    template <bound_direction bound>
    static constexpr inline potential_projection get_proj_state_bound(const sidb_cluster_projector_state& pst,
                                                                      const sidb_cluster_receptor_state&  rst) noexcept
    {
        return pst.cluster->pot_projs.at(rst.cluster->uid)
            .get_pot_proj_for_m_conf<bound>(pst.multiset_conf, rst.sidb_ix);
    }

    static constexpr inline void add_pot_proj(const sidb_cluster_ptr& c, const sidb_cluster_receptor_state& rst,
                                              const potential_projection& pp) noexcept
    {
        c->pot_projs[rst.cluster->uid].add(pp, rst.sidb_ix);
    }

    bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst,
                                          const intra_cluster_pot_bounds&     sibling_pot_bounds) const noexcept
    {
        uint64_t required_negative_witnesses = pst.get_count<sidb_charge_state::NEGATIVE>();
        uint64_t required_positive_witnesses = pst.get_count<sidb_charge_state::POSITIVE>();
        uint64_t required_neutral_witnesses  = pst.get_count<sidb_charge_state::NEUTRAL>();

        const auto& test_if_sidb_is_negative_witness = [&](const double recv_pot_lb, const uint64_t sidbs_to_check)
        {
            if (required_negative_witnesses != 0)
            {
                if (required_negative_witnesses > sidbs_to_check)
                {
                    return false;
                }
                else if (!fail_onto_negative_charge(recv_pot_lb))
                {
                    required_negative_witnesses--;
                }
            }
            return true;
        };

        const auto& test_if_sidb_is_positive_witness = [&](const double recv_pot_ub, const uint64_t sidbs_to_check)
        {
            if (required_positive_witnesses != 0)
            {
                if (required_positive_witnesses > sidbs_to_check)
                {
                    return false;
                }
                else if (!fail_onto_positive_charge(recv_pot_ub))
                {
                    required_positive_witnesses--;
                }
            }
            return true;
        };

        for (uint64_t sidbs_to_check = pst.cluster->size(); sidbs_to_check != 0; --sidbs_to_check)
        {
            const uint64_t sidb_ix = *std::next(pst.cluster->sidbs.cbegin(), static_cast<int64_t>(sidbs_to_check - 1));

            if (required_neutral_witnesses != 0)
            {
                if (required_neutral_witnesses > sidbs_to_check)
                {
                    return false;
                }

                const double recv_pot_lb =
                    sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::LOWER)] +
                    pst.cluster->get_recv_ext_pot_bound<bound_direction::LOWER>(sidb_ix);

                const double recv_pot_ub =
                    sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::UPPER)] +
                    pst.cluster->get_recv_ext_pot_bound<bound_direction::UPPER>(sidb_ix);

                if (!ub_fail_onto_neutral_charge(recv_pot_ub) && !lb_fail_onto_neutral_charge(recv_pot_lb))
                {
                    required_neutral_witnesses--;
                }

                if (!test_if_sidb_is_negative_witness(recv_pot_lb, sidbs_to_check) ||
                    !test_if_sidb_is_positive_witness(recv_pot_ub, sidbs_to_check))
                {
                    return false;
                }
                continue;
            }

            if (!test_if_sidb_is_negative_witness(
                    sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::LOWER)] +
                        pst.cluster->get_recv_ext_pot_bound<bound_direction::LOWER>(sidb_ix),
                    sidbs_to_check) ||
                !test_if_sidb_is_positive_witness(
                    sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::UPPER)] +
                        pst.cluster->get_recv_ext_pot_bound<bound_direction::UPPER>(sidb_ix),
                    sidbs_to_check))
            {
                return false;
            }
        }

        if (required_negative_witnesses != 0 || required_positive_witnesses != 0 || required_neutral_witnesses != 0)
        {
            return false;
        }

        return true;
    }

    template <bound_direction bound>
    void subtract_sibling_pot_from_recv_ext_pot_bound(const sidb_cluster_ptr&            parent,
                                                      const sidb_cluster_receptor_state& child_rst) const noexcept
    {
        double recv_pot_without_siblings = child_rst.cluster->get_recv_ext_pot_bound<bound>(child_rst.sidb_ix);

        for (const sidb_cluster_ptr& sibling : parent->children)
        {
            if (sibling != child_rst.cluster)
            {
                recv_pot_without_siblings -= get_proj_bound<bound>(sibling, child_rst).V;
            }
        }

        child_rst.cluster->set_recv_ext_pot_bound<bound>(child_rst.sidb_ix, recv_pot_without_siblings);
        parent->set_recv_ext_pot_bound<bound>(child_rst.sidb_ix, recv_pot_without_siblings);
    }

    void derive_children_recv_bounds_without_siblings(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& child : parent->children)
        {
            for (const uint64_t sidb_ix : child->sidbs)
            {
                const sidb_cluster_receptor_state child_rst{child, sidb_ix};
                subtract_sibling_pot_from_recv_ext_pot_bound<bound_direction::LOWER>(parent, child_rst);
                subtract_sibling_pot_from_recv_ext_pot_bound<bound_direction::UPPER>(parent, child_rst);
            }
        }
    }

    bool verify_composition(sidb_cluster_charge_state_composition& composition) const noexcept
    {
        for (auto& [receiver_pst, sibling_pot_bounds] : composition)
        {
            for (const uint64_t sidb_ix : receiver_pst.cluster->sidbs)
            {
                const sidb_cluster_receptor_state& rst = receiver_pst.to_receptor_state(sidb_ix);

                double sibling_pot_lb{};
                double sibling_pot_ub{};

                for (const auto& [pst, _] : composition)
                {
                    sibling_pot_lb += get_proj_state_bound<bound_direction::LOWER>(pst, rst).V;
                    sibling_pot_ub += get_proj_state_bound<bound_direction::UPPER>(pst, rst).V;
                }

                sibling_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::LOWER)] = sibling_pot_lb;
                sibling_pot_bounds[sidb_ix][static_cast<uint8_t>(bound_direction::UPPER)] = sibling_pot_ub;
            }

            if (!perform_potential_bound_analysis(receiver_pst, sibling_pot_bounds))
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
            if (!verify_composition(m.compositions[0]))
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
            m.compositions[0].emplace_back(std::make_pair(
                sidb_cluster_projector_state{cur_child, static_cast<uint64_t>(m_part)}, intra_cluster_pot_bounds{}));
            m += m_part;

            fill_merged_charge_state_space(parent, cur_child_ix + 1, m);

            m -= m_part;
            m.compositions[0].pop_back();
        }
    }

    void construct_merged_charge_state_space(const sidb_cluster_ptr& parent) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.compositions.emplace_back(sidb_cluster_charge_state_composition{});
        fill_merged_charge_state_space(parent, 0, m);
    }

    template <bound_direction bound>
    void merge_pot_proj_bounds(const sidb_cluster_ptr& parent, const sidb_cluster_receptor_state& rst) const noexcept
    {
        // construct external projected potential bounds for every composition of every element in the charge space
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const sidb_cluster_charge_state_composition& composition : m.compositions)
            {
                potential_projection pot_proj_onto_other_c{};

                for (const auto& [child_pst, _] : composition)
                {
                    pot_proj_onto_other_c += get_proj_state_bound<bound>(child_pst, rst);
                }

                add_pot_proj(parent, rst, pot_proj_onto_other_c);
            }
        }

        // update the received external potential bound for the subject
        double diff = get_proj_bound<bound>(parent, rst).V;

        for (const sidb_cluster_ptr& child : parent->children)
        {
            diff -= get_proj_bound<bound>(child, rst).V;
        }

        rst.cluster->update_recv_ext_pot_bound<bound>(rst.sidb_ix, diff);
    }

    template <bound_direction bound>
    void merge_recv_pot_proj_bounds(const sidb_cluster_projector_state& pst,
                                    const sidb_cluster_ptr&             parent) const noexcept
    {
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const sidb_cluster_charge_state_composition& composition : m.compositions)
            {
                for (const auto& [child_pst, _] : composition)
                {
                    for (const uint64_t child_sidb_ix : child_pst.cluster->sidbs)
                    {
                        add_pot_proj(pst.cluster, sidb_cluster_receptor_state{parent, child_sidb_ix},
                                     get_proj_state_bound<bound>(pst, child_pst.to_receptor_state(child_sidb_ix)));
                    }
                }
            }
        }
    }

    void construct_merged_potential_projections(const sidb_cluster_ptr& parent) const noexcept
    {
        for (const sidb_cluster_ptr& non_child : clustering)
        {
            for (const uint64_t sidb_ix : non_child->sidbs)
            {
                const sidb_cluster_receptor_state rst{non_child, sidb_ix};

                merge_pot_proj_bounds<bound_direction::LOWER>(parent, rst);
                merge_pot_proj_bounds<bound_direction::UPPER>(parent, rst);
            }

            for (const sidb_cluster_charge_state& m : non_child->charge_space)
            {
                const sidb_cluster_projector_state pst{non_child, static_cast<uint64_t>(m)};

                merge_recv_pot_proj_bounds<bound_direction::LOWER>(pst, parent);
                merge_recv_pot_proj_bounds<bound_direction::UPPER>(pst, parent);
            }

            for (const sidb_cluster_ptr& child : parent->children)
            {
                non_child->pot_projs.erase(child->uid);
            }
        }
    }

    static void compute_meets_for_internal_pot_bounds(const sidb_cluster_ptr& parent) noexcept
    {
        for (const sidb_cluster_charge_state& m : parent->charge_space)
        {
            for (const uint64_t sidb_ix : parent->sidbs)
            {
                double lb_meet = potential_bound_top<bound_direction::LOWER>();
                double ub_meet = potential_bound_top<bound_direction::UPPER>();

                for (const sidb_cluster_charge_state_composition& composition : m.compositions)
                {
                    for (const auto& [pst, sibling_pot_bounds] : composition)
                    {
                        if (sibling_pot_bounds.count(sidb_ix) != 0)
                        {
                            take_meet_of_potential_bounds<bound_direction::LOWER>(
                                lb_meet, sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::LOWER)]);
                            take_meet_of_potential_bounds<bound_direction::UPPER>(
                                ub_meet, sibling_pot_bounds.at(sidb_ix)[static_cast<uint8_t>(bound_direction::UPPER)]);
                        }
                    }
                }

                add_pot_proj(parent, sidb_cluster_receptor_state{parent, sidb_ix},
                             potential_projection{lb_meet, static_cast<uint64_t>(m)});
                add_pot_proj(parent, sidb_cluster_receptor_state{parent, sidb_ix},
                             potential_projection{ub_meet, static_cast<uint64_t>(m)});
            }
        }
    }

    void move_up_hierarchy() noexcept
    {
        if (clustering.size() == 1)
        {
            return;
        }

        // find the parent with the minimum cluster size
        const sidb_cluster_ptr& min_parent =
            (*std::min_element(clustering.cbegin(), clustering.cend(),
                               [](const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2)
                               { return c1->parent->size() < c2->parent->size(); }))
                ->parent;

        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        derive_children_recv_bounds_without_siblings(min_parent);

        construct_merged_charge_state_space(min_parent);

        construct_merged_potential_projections(min_parent);

        compute_meets_for_internal_pot_bounds(min_parent);

        clustering.emplace(min_parent);

        move_up_hierarchy();
    }

    const std::array<double, 4> mu_bounds_with_error;

    sidb_clustering clustering{};
};

}  // namespace fiction

#endif  // FICTION_GROUND_STATE_SPACE_HPP
