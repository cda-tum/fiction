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

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                c->proj_pots[j] =
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
    inline constexpr potential_projection get_proj_bound(const sidb_cluster_ptr& c1,
                                                         const sidb_cluster_ptr& c2) const noexcept
    {
        return c1->proj_pots.at(c2->uid).get<bound, onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    inline void add_pot_proj(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                             const potential_projection& pp) const noexcept
    {
        c1->proj_pots[c2->uid].add<bound, onto_cs>(pp);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void handle_invalid_state(const sidb_cluster_ptr& c, const sidb_cluster_charge_state& m) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            c->proj_pots[other_c->uid].remove_m_conf<onto_cs>(static_cast<uint64_t>(m));
        }
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

    bool detect_state_validity_and_handle_invalid_states(const sidb_cluster_ptr&          c,
                                                         const sidb_cluster_charge_state& m) const noexcept
    {
        // NEG in m
        if (m.neg_count != 0)
        {
            // V >= e - mu-
            if (compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c) >=
                mu_bounds_with_error[0])
            {
                handle_invalid_state<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, m);
                return false;
            }
        }

        // POS in m
        if (m.pos_count != 0)
        {
            // V <= -e - mu+
            if (compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c) <=
                mu_bounds_with_error[3])
            {
                handle_invalid_state<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, m);
                return false;
            }
        }

        // NEUT in m
        if (m.contains_neutral_charge(c->sidbs.size()))
        {
            // V <= -e - mu-  or  V >= e - mu+
            if (compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c) <=
                mu_bounds_with_error[1])
            {
                handle_invalid_state<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c, m);
                return false;
            }
            else if (compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c) >=
                     mu_bounds_with_error[2])
            {
                handle_invalid_state<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c, m);
                return false;
            }
        }

        return true;
    }

    bool update_charge_spaces() const noexcept
    {
        bool fixpoint = true;

        // make a pass over the clustering and see if the charge spaces contain invalid cluster charge states
        for (const sidb_cluster_ptr& c : clustering)
        {
            // skip if |charge space| = 1?
            if (c->charge_space.size() > 1)
            {
                for (const sidb_cluster_charge_state& m : c->charge_space)
                {
                    fixpoint &= detect_state_validity_and_handle_invalid_states(c, m);
                }
            }
        }

        return fixpoint;
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

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void construct_internal_proj_pots(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& child : c->children)
        {
            potential_projection pot_proj_onto_child{};

            for (const sidb_cluster_ptr& other_child : c->children)
            {
                pot_proj_onto_child += get_proj_bound<bound, onto_cs>(other_child, child);
            }

            add_pot_proj<bound, onto_cs>(c, c, pot_proj_onto_child);
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    void compute_merged_proj_pots_onto_cs(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c) const noexcept
    {
        if (c == other_c)  // omitable?
        {
            construct_internal_proj_pots<bound, cs>(c);
            return;
        }

        potential_projection new_pot_proj{};

        std::set<uint64_t> recv_m_confs{};

        for (const sidb_cluster_ptr& child : c->children)
        {
            new_pot_proj += get_proj_bound<bound, cs>(child, other_c);

            recv_m_confs.emplace(get_proj_bound<bound, cs>(other_c, child).M);
        }

        add_pot_proj<>() _proj_bound<bound, cs>(c, other_c, new_proj_bound);

        // check for bound conflict
        if (recv_m_confs.size() == 1)
        {
            // no conflict  .. construct instead?
            set_proj_bound<bound, cs>(other_c, c, compute_projector_bound<bound, cs>(other_c, c, *recv_m.cbegin()));
            return;
        }

        // conflict found, solve exhaustively (?)
        set_proj_bound<bound, cs>(other_c, c, find_new_projector_bound<bound, cs>(other_c, c));
    }

    void compute_merged_potential_projections(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, other_c);
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::POSITIVE>(c, other_c);
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c, other_c);
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEGATIVE>(c, other_c);
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, other_c);
            compute_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c, other_c);
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
        compute_merged_potential_projections(min_parent);
        //        compute_merged_local_potential_bounds(min_parent);

        // update the merged cluster charge space
        check_charge_space(min_parent);
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
