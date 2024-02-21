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

            c->initialize_singleton_cluster_charge_space();

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                if (j == i)
                {
                    c->proj_pots[j] = potential_projection_orders{true};
                    continue;
                }

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
    static inline constexpr potential_projection get_proj_bound(const sidb_cluster_ptr& c1,
                                                                const sidb_cluster_ptr& c2) noexcept
    {
        return c1->proj_pots.at(c2->uid).get<bound, onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr potential_projection get_proj_state_bound(const sidb_cluster_projector_state& pst,
                                                               const sidb_cluster_ptr&             c2) noexcept
    {
        return *pst.projector->proj_pots.at(c2->uid).get_pot_proj_for_m_conf<bound, onto_cs>(pst.multiset_conf);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    static constexpr double get_internal_proj_pot_bound_for_m(const sidb_cluster_ptr&          c,
                                                              const sidb_cluster_charge_state& m_conf) noexcept
    {
        return c->proj_pots.at(c->uid).get_pot_proj_for_m_conf<bound, onto_cs>(static_cast<uint64_t>(m_conf))->V;
    }

    template <sidb_charge_state onto_cs>
    static inline void add_pot_proj(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                    const potential_projection& pp) noexcept
    {
        c1->proj_pots[c2->uid].add<onto_cs>(pp);
    }

    template <sidb_charge_state onto_cs>
    static constexpr inline bool onto_cs_pruned(const sidb_cluster_ptr& c) noexcept
    {
        return c->proj_pots.at(c->uid).onto_cs_pruned<onto_cs>();
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs, sidb_charge_state witness_cs>
    static potential_projection get_new_proj_bound_without_witness_cs(const sidb_cluster_ptr& c,
                                                                      const sidb_cluster_ptr& other_c,
                                                                      const uint64_t          m) noexcept
    {
        if (c->sidbs.size() == 1)
        {
            if (trinary_multiset_conf_to_sign(m) == charge_state_to_sign(witness_cs))
            {
                return c->proj_pots.at(other_c->uid).get_sub_bound<bound, onto_cs>();
            }

            return potential_projection::top<bound>();
        }

        potential_projection new_bound{potential_projection::top<bound>()};

        for (const sidb_cluster_ptr& child : c->children)
        {
            const potential_projection& proj_bound_from_child = get_proj_bound<bound, onto_cs>(child, other_c);

            new_bound.meet<bound>((get_proj_bound<bound, onto_cs>(c, other_c) - proj_bound_from_child) +
                                  get_new_proj_bound_without_witness_cs<bound, onto_cs, witness_cs>(
                                      child, other_c, proj_bound_from_child.M));
        }

        return new_bound;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs, sidb_charge_state witness_cs>
    static potential_projection get_new_proj_bound_without_witness_cs_with_join(const sidb_cluster_ptr& c,
                                                                                const sidb_cluster_ptr& other_c,
                                                                                const uint64_t          m) noexcept
    {
        if (c->sidbs.size() == 1)
        {
            if (trinary_multiset_conf_to_sign(m) == charge_state_to_sign(witness_cs))
            {
                return c->proj_pots.at(other_c->uid).get_sub_bound<bound, onto_cs>();
            }

            return potential_projection::bottom<bound>();
        }

        potential_projection new_bound{potential_projection::bottom<bound>()};

        for (const sidb_cluster_ptr& child : c->children)
        {
            const potential_projection& proj_bound_from_child = get_proj_bound<bound, onto_cs>(child, other_c);

            new_bound.join<bound>((get_proj_bound<bound, onto_cs>(c, other_c) - proj_bound_from_child) +
                                  get_new_proj_bound_without_witness_cs<bound, onto_cs, witness_cs>(
                                      child, other_c, proj_bound_from_child.M));
        }

        return new_bound;
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs, sidb_charge_state witness_cs>
    static void replace_projection_bound(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c,
                                         const uint64_t m) noexcept
    {
        const potential_projection& new_proj_bound =
            get_new_proj_bound_without_witness_cs<bound, onto_cs, witness_cs>(c, other_c, m);
        if (!new_proj_bound.is_bottom())
        {
            std::cout << (c->proj_pots.at(other_c->uid)
                                  .get_pot_proj_for_m_conf<bound_calculation_mode::LOWER, onto_cs>(new_proj_bound.M)
                                  ->is_bottom() ?
                              "NOT EXISTS" :
                              "EXISTS")
                      << std::endl;
            add_pot_proj<onto_cs>(c, other_c, new_proj_bound);
        }
    }

    template <sidb_charge_state onto_cs, sidb_charge_state witness_cs>
    static void replace_proj_bound_if_removed(const sidb_cluster_projector_state& pst,
                                              const sidb_cluster_ptr&             other_c) noexcept
    {
        const sidb_cluster_ptr& c = pst.projector;
        const uint64_t          m = pst.multiset_conf;

        if (onto_cs_pruned<onto_cs>(other_c))
        {
            return;
        }

        std::cout << "WITNESS: " << charge_configuration_to_string(std::vector{witness_cs})
                  << "\tONTO:" << charge_configuration_to_string(std::vector{onto_cs}) << std::endl;

        const auto show = [](const uint64_t m, const uint64_t s)
        {
            return std::string{"N"} + std::to_string(m >> 32) + " Z" +
                   std::to_string(s - ((m >> 32) + (m & 0xFFFFFFFF))) + " P" + std::to_string(m & 0xFFFFFFFF);
        };

        std::cout << "OLD BOUNDS:\t"
                  << show(get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).M, c->sidbs.size()) << "\t"
                  << get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).V << "\n\t\t"
                  << show(get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).M, c->sidbs.size()) << "\t"
                  << get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).V << std::endl
                  << std::endl;

        std::cout << "SUB BOUNDS:\t"
                  << show(c->proj_pots.at(other_c->uid).get_sub_bound<bound_calculation_mode::LOWER, onto_cs>().M,
                          c->sidbs.size())
                  << "\t" << c->proj_pots.at(other_c->uid).get_sub_bound<bound_calculation_mode::LOWER, onto_cs>().V
                  << "\n\t\t"
                  << show(c->proj_pots.at(other_c->uid).get_sub_bound<bound_calculation_mode::UPPER, onto_cs>().M,
                          c->sidbs.size())
                  << "\t" << c->proj_pots.at(other_c->uid).get_sub_bound<bound_calculation_mode::UPPER, onto_cs>().V
                  << std::endl
                  << std::endl;

        if (m == get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).M)
        {
            std::cout << "REMOVING BOUND: "
                      << show(get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).M, c->sidbs.size())
                      << "\t" << get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).V << std::endl;
            replace_projection_bound<bound_calculation_mode::LOWER, onto_cs, witness_cs>(c, other_c, m);

            std::cout
                << "MEET BOUND: \t"
                << show(get_new_proj_bound_without_witness_cs<bound_calculation_mode::LOWER, onto_cs, witness_cs>(
                            c, other_c, m)
                            .M,
                        c->sidbs.size())
                << "\t"
                << get_new_proj_bound_without_witness_cs<bound_calculation_mode::LOWER, onto_cs, witness_cs>(c, other_c,
                                                                                                             m)
                       .V
                << "\n"
                << "JOIN BOUND: \t"
                << show(get_new_proj_bound_without_witness_cs_with_join<bound_calculation_mode::LOWER, onto_cs,
                                                                        witness_cs>(c, other_c, m)
                            .M,
                        c->sidbs.size())
                << "\t"
                << get_new_proj_bound_without_witness_cs_with_join<bound_calculation_mode::LOWER, onto_cs, witness_cs>(
                       c, other_c, m)
                       .V
                << std::endl;
        }

        if (m == get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).M)
        {
            std::cout << "REMOVING BOUND: "
                      << show(get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).M, c->sidbs.size())
                      << "\t" << get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).V << std::endl;
            replace_projection_bound<bound_calculation_mode::UPPER, onto_cs, witness_cs>(c, other_c, m);

            std::cout
                << "MEET BOUND: \t"
                << show(get_new_proj_bound_without_witness_cs<bound_calculation_mode::UPPER, onto_cs, witness_cs>(
                            c, other_c, m)
                            .M,
                        c->sidbs.size())
                << "\t"
                << get_new_proj_bound_without_witness_cs<bound_calculation_mode::UPPER, onto_cs, witness_cs>(c, other_c,
                                                                                                             m)
                       .V
                << "\n"
                << "JOIN BOUND: \t"
                << show(get_new_proj_bound_without_witness_cs_with_join<bound_calculation_mode::UPPER, onto_cs,
                                                                        witness_cs>(c, other_c, m)
                            .M,
                        c->sidbs.size())
                << "\t"
                << get_new_proj_bound_without_witness_cs_with_join<bound_calculation_mode::UPPER, onto_cs, witness_cs>(
                       c, other_c, m)
                       .V
                << std::endl;
        }

        std::cout << "removing " << show(m, c->sidbs.size()) << std::endl << std::endl;

        c->proj_pots[other_c->uid].remove_m_conf<onto_cs>(m);

        std::cout << "NEW BOUNDS:\t"
                  << show(get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).M, c->sidbs.size()) << "\t"
                  << get_proj_bound<bound_calculation_mode::LOWER, onto_cs>(c, other_c).V << "\n\t\t"
                  << show(get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).M, c->sidbs.size()) << "\t"
                  << get_proj_bound<bound_calculation_mode::UPPER, onto_cs>(c, other_c).V << std::endl
                  << std::endl
                  << std::endl;
    }

    template <sidb_charge_state witness_cs>
    void handle_invalid_state(const sidb_cluster_projector_state& pst) const noexcept
    {
        pst.projector->proj_pots[pst.projector->uid].remove_m_conf<witness_cs>(pst.multiset_conf);

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == pst.projector)
            {
                continue;
            }

            pst.projector->proj_pots[other_c->uid].remove_m_conf<sidb_charge_state::NEGATIVE>(pst.multiset_conf);
            pst.projector->proj_pots[other_c->uid].remove_m_conf<sidb_charge_state::POSITIVE>(pst.multiset_conf);
            pst.projector->proj_pots[other_c->uid].remove_m_conf<sidb_charge_state::NEUTRAL>(pst.multiset_conf);
            //            replace_proj_bound_if_removed<sidb_charge_state::NEGATIVE, witness_cs>(pst, other_c);
            //            replace_proj_bound_if_removed<sidb_charge_state::POSITIVE, witness_cs>(pst, other_c);
            //            replace_proj_bound_if_removed<sidb_charge_state::NEUTRAL, witness_cs>(pst, other_c);

            // to be extended
            if (pst.projector->sidbs.size() == 1)
            {
                other_c->proj_pots[pst.projector->uid].prune_cs<witness_cs>();
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    double compute_received_potential_bound(const sidb_cluster_projector_state& pst) const noexcept
    {
        const sidb_cluster_ptr&         c = pst.projector;
        const sidb_cluster_charge_state m{pst.multiset_conf};

        std::cout << "JUDGING:\tNEG: " << m.neg_count << "  POS: " << m.pos_count
                  << "  NEUT: " << (c->sidbs.size() - (m.neg_count + m.pos_count)) << std::endl;

        std::cout << "V_{" << c->uid << " to " << c->uid << "} :\t"
                  << [](const double x)
        { return (x == 0.0) ? "0.000000" : std::to_string(x); }(get_internal_proj_pot_bound_for_m<bound, onto_cs>(c, m))
                  << "\t" << (bound == bound_calculation_mode::LOWER ? "LB" : "UB") << " onto "
                  << charge_configuration_to_string(std::vector<sidb_charge_state>{onto_cs}) << std::endl;

        double sum = get_internal_proj_pot_bound_for_m<bound, onto_cs>(c, m);

        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (other_c == c)
            {
                continue;
            }

            sum += get_proj_bound<bound, onto_cs>(other_c, c).V;

            std::cout << "V_{" << other_c->uid << " to " << c->uid << "} :\t"
                      << [](const double x)
            { return (x == 0.0) ? "0.000000" : std::to_string(x); }(get_proj_bound<bound, onto_cs>(other_c, c).V)
                      << "\t" << (bound == bound_calculation_mode::LOWER ? "LB" : "UB") << " onto "
                      << charge_configuration_to_string(std::vector<sidb_charge_state>{onto_cs}) << std::endl;
        }

        std::cout << "V" << c->uid << ":\t" << sum << "\t" << (bound == bound_calculation_mode::LOWER ? "LB" : "UB")
                  << " onto " << charge_configuration_to_string(std::vector<sidb_charge_state>{onto_cs}) << std::endl
                  << std::endl;

        return sum;
    }

    bool perform_potential_bound_analysis(const sidb_cluster_projector_state& pst) const noexcept
    {
        // NEG in m
        if (pst.contains_cs<sidb_charge_state::NEGATIVE>())
        {
            // V > e - mu-
            if (compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(pst) >
                mu_bounds_with_error[0])
            {
                handle_invalid_state<sidb_charge_state::NEGATIVE>(pst);
                return false;
            }
        }

        // POS in m
        if (pst.contains_cs<sidb_charge_state::POSITIVE>())
        {
            // V < -e - mu+
            if (compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(pst) <
                mu_bounds_with_error[3])
            {
                handle_invalid_state<sidb_charge_state::POSITIVE>(pst);
                return false;
            }
        }

        // NEUT in m
        if (pst.contains_cs<sidb_charge_state::NEUTRAL>())
        {
            // V < -e - mu-  or  V > e - mu+
            if (compute_received_potential_bound<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(pst) <
                mu_bounds_with_error[1])
            {
                handle_invalid_state<sidb_charge_state::NEUTRAL>(pst);
                return false;
            }
            else if (compute_received_potential_bound<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(pst) >
                     mu_bounds_with_error[2])
            {
                handle_invalid_state<sidb_charge_state::NEUTRAL>(pst);
                return false;
            }
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
                std::cout << "REMOVE\n" << std::endl;
                std::cout << "REMOVE\n" << std::endl;
                std::cout << "REMOVE\n" << std::endl;
                removed_ms.emplace_back(static_cast<uint64_t>(m));
                fixpoint = false;
            }
        }

        if (fixpoint)
        {
            return true;
        }

        for (const uint64_t m : removed_ms)
        {
            c->charge_space.erase(sidb_cluster_charge_state{m});
        }

        return false;
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
                fixpoint &= check_charge_space(c);
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

    void construct_merged_charge_state_space(const sidb_cluster_ptr& c) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.decompositions.emplace_back(sidb_cluster_charge_state_decomposition{});
        fill_merged_charge_state_space(c, 0, m);
    }

    template <bound_calculation_mode bound, sidb_charge_state onto_cs>
    void construct_internal_proj_pots(const sidb_cluster_ptr& c) const noexcept
    {
        // construct internally projected potential bounds for every decomposition of every element in the charge space
        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            for (const sidb_cluster_charge_state_decomposition& decomposition : m.decompositions)
            {
                for (const sidb_cluster_projector_state& pst : decomposition)
                {
                    if (onto_cs_pruned<onto_cs>(pst.projector) || !pst.contains_cs<onto_cs>())
                    {
                        continue;
                    }

                    potential_projection pot_proj_onto_child{};

                    for (const sidb_cluster_projector_state& other_pst : decomposition)
                    {
                        pot_proj_onto_child += get_proj_state_bound<bound, onto_cs>(other_pst, pst.projector);
                    }

                    add_pot_proj<onto_cs>(c, c, pot_proj_onto_child);
                }
            }
        }
    }

    template <bound_calculation_mode bound, sidb_charge_state cs>
    void construct_merged_proj_pots_onto_cs(const sidb_cluster_ptr& c, const sidb_cluster_ptr& other_c) const noexcept
    {
        if (onto_cs_pruned<cs>(other_c))
        {
            return;
        }

        //        for (const sidb_cluster_ptr& child : c->children)
        //        {
        //            other_c->proj_pots.at(child->uid)
        //                .forward_proj_pot_bound<bound, cs>(other_c->proj_pots[c->uid]);  // ADD MORE?
        //        }

        // construct external projected potential bounds for every decomposition of every element in the charge space
        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            for (const sidb_cluster_charge_state_decomposition& decomposition : m.decompositions)
            {
                potential_projection pot_proj_onto_other_c{};

                for (const sidb_cluster_projector_state& pst : decomposition)
                {
                    pot_proj_onto_other_c += get_proj_state_bound<bound, cs>(pst, other_c);
                }

                add_pot_proj<cs>(c, other_c, pot_proj_onto_other_c);
            }
        }

        //        potential_projection parent_pot_proj_onto_other_c{};

        //        const auto show = [](const uint64_t m, const uint64_t s)
        //        {
        //            return std::string{"N"} + std::to_string(m >> 32) + " Z" +
        //                   std::to_string(s - ((m >> 32) + (m & 0xFFFFFFFF))) + " P" + std::to_string(m & 0xFFFFFFFF);
        //        };
        //        std::cout << "CONSTRUCTING " << (bound == bound_calculation_mode::LOWER ? "LOWER" : "UPPER") << "
        //        BOUND ONTO "
        //                  << charge_configuration_to_string(std::vector<sidb_charge_state>{cs}) << " IN " <<
        //                  other_c->uid
        //                  << std::endl;
        //        for (const sidb_cluster_ptr& child : c->children)
        //        {
        //            std::cout << "ADDING BOUND: " << show(get_proj_bound<bound, cs>(child, other_c).M,
        //            child->sidbs.size())
        //                      << "\t" << get_proj_bound<bound, cs>(child, other_c).V << "\t from: " << child->uid
        //                      << std::endl;
        //            parent_pot_proj_onto_other_c += get_proj_bound<bound, cs>(child, other_c);

        //            other_c->proj_pots.at(child->uid).forward_proj_pot_bound<bound, cs>(other_c->proj_pots[c->uid]);
        //        }

        //        add_pot_proj<cs>(c, other_c, parent_pot_proj_onto_other_c);

        //        std::cout << "PARENT BOUND: " << show(get_proj_bound<bound, cs>(c, other_c).M, c->sidbs.size()) <<
        //        "\t"
        //                  << get_proj_bound<bound, cs>(c, other_c).V << "\t to: " << c->uid << std::endl
        //                  << std::endl;
    }

    void construct_merged_potential_projections(const sidb_cluster_ptr& c) const noexcept
    {
        sidb_clustering unhandled_clusters{clustering};

        while (!unhandled_clusters.empty())
        {
            const sidb_cluster_ptr other_c = *unhandled_clusters.cbegin();
            unhandled_clusters.erase(unhandled_clusters.begin());

            construct_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, other_c);
            construct_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::POSITIVE>(c, other_c);
            construct_merged_proj_pots_onto_cs<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(c, other_c);
            construct_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEGATIVE>(c, other_c);
            construct_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, other_c);
            construct_merged_proj_pots_onto_cs<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(c, other_c);

            for (const sidb_cluster_ptr& child : c->children)
            {
                other_c->proj_pots.at(child->uid)
                    .copy_to_parent<sidb_charge_state::NEGATIVE>(other_c->proj_pots[c->uid]);
                other_c->proj_pots.at(child->uid)
                    .copy_to_parent<sidb_charge_state::POSITIVE>(other_c->proj_pots[c->uid]);
                other_c->proj_pots.at(child->uid)
                    .copy_to_parent<sidb_charge_state::NEUTRAL>(other_c->proj_pots[c->uid]);
            }

            for (const sidb_cluster_ptr& child : other_c->children)
            {
                unhandled_clusters.emplace(child);
            };
        }
    }

    bool move_up_hierarchy() noexcept
    {
        // Find the parent with the minimum cluster size
        const sidb_cluster_ptr& min_parent =
            (*std::min_element(clustering.cbegin(), clustering.cend(),
                               [](const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2)
                               { return c1->parent->sidbs.size() < c2->parent->sidbs.size(); }))
                ->parent;

        construct_merged_charge_state_space(min_parent);

        construct_internal_proj_pots<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(min_parent);
        construct_internal_proj_pots<bound_calculation_mode::LOWER, sidb_charge_state::POSITIVE>(min_parent);
        construct_internal_proj_pots<bound_calculation_mode::LOWER, sidb_charge_state::NEUTRAL>(min_parent);
        construct_internal_proj_pots<bound_calculation_mode::UPPER, sidb_charge_state::NEGATIVE>(min_parent);
        construct_internal_proj_pots<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(min_parent);
        construct_internal_proj_pots<bound_calculation_mode::UPPER, sidb_charge_state::NEUTRAL>(min_parent);

        for (const sidb_cluster_ptr& c : min_parent->children)
        {
            clustering.erase(c);
        }

        construct_merged_potential_projections(min_parent);

        // update the merged cluster charge space
        check_charge_space(min_parent);

        // and after that the charge spaces of the other clusters
        const bool fixpoint = update_charge_spaces();

        clustering.emplace(min_parent);

        return fixpoint;
    }

    std::pair<sidb_cluster_ptr, std::chrono::duration<double>>
    compute_ground_state_space(const uint64_t max_merges = 100) noexcept
    {
        uint64_t merges = 0;

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            while (clustering.size() > 1 && merges++ < max_merges)
            {
                while (!update_charge_spaces())
                    ;
                while (move_up_hierarchy() && clustering.size() > 1 && merges++ < max_merges)
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
