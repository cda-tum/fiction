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

                c->potential_equivalence_classes.emplace(std::make_pair(
                    other_c->uid, ordered_potential_information{true, pot_mat, std::vector{std::set{0ul}}}));

                other_c->potential_equivalence_classes.emplace(
                    std::make_pair(c->uid, ordered_potential_information{false, pot_mat, std::vector{std::set{0ul}}}));
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

            c->local_pot_bounds = {local_potential_bound_containers.first.get_local_potential_by_index(i).value(),
                                   local_potential_bound_containers.second.get_local_potential_by_index(i).value()};

            for (uint64_t j = 0; j < local_potential_bound_containers.first.num_cells(); ++j)
            {
                if (i == j)
                {
                    continue;
                }

                const std::pair<double, uint64_t> lb = {
                    local_potential_bound_containers.first.get_potential_by_indices(i, j),
                    static_cast<uint64_t>(sidb_cluster_charge_state{sidb_charge_state::POSITIVE})};
                const std::pair<double, uint64_t> ub = {
                    local_potential_bound_containers.second.get_potential_by_indices(i, j),
                    static_cast<uint64_t>(sidb_cluster_charge_state{sidb_charge_state::NEGATIVE})};

                c->projected_pot_bounds.emplace(std::make_pair(
                    j, std::make_pair(potential_bounds{lb.first, ub.first}, std::make_pair(lb.second, ub.second))));
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

    std::vector<std::vector<double>> compute_cluster_potential_matrix(const sidb_cluster_ptr& c1,
                                                                      const sidb_cluster_ptr& c2) const noexcept
    {
        std::vector<std::vector<double>> all_pots{};
        all_pots.reserve(c1->sidbs.size() + 2);

        std::vector<double> min_c2_pot(c2->sidbs.size(), std::numeric_limits<double>::infinity()),
            max_c2_pot(c2->sidbs.size(), 0);

        for (const uint64_t i : c1->sidbs)
        {
            std::vector<double> pots{};
            pots.reserve(c2->sidbs.size() + 2);

            double min_c1_pot = std::numeric_limits<double>::infinity(), max_c1_pot = 0;

            for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
            {
                const double pot =
                    cds.get_chargeless_potential_by_indices(i, *std::next(c2->sidbs.cbegin(), static_cast<int64_t>(j)));

                min_c1_pot = std::min(min_c1_pot, pot);
                max_c1_pot = std::max(max_c1_pot, pot);

                min_c2_pot[j] = std::min(min_c2_pot[j], pot);
                max_c2_pot[j] = std::max(max_c2_pot[j], pot);

                pots.push_back(pot);
            }

            pots.push_back(min_c1_pot);
            pots.push_back(max_c1_pot);

            all_pots.push_back(pots);
        }

        all_pots.push_back(min_c2_pot);
        all_pots.push_back(max_c2_pot);

        return all_pots;
    }

    enum class cluster_potential_projection_direction
    {
        C2_ONTO_C1,
        C1_ONTO_C2
    };

    // compute the ordered equivalence classes of the potentials from c2 projected onto c1
    template <cluster_potential_projection_direction proj_dir>
    static ordered_eqv_classes
    compute_projected_potential_eqv_classes(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                            const std::vector<std::vector<double>>& all_pots) noexcept
    {
        std::vector<std::vector<uint64_t>> perceived_order_c2_to_c1{};

        perceived_order_c2_to_c1.reserve(c2->sidbs.size());

        for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
        {
            std::vector<uint64_t> perceived_order{};
            perceived_order.reserve(c1->sidbs.size());

            for (uint64_t i = 0; i < c1->sidbs.size(); ++i)
            {
                perceived_order.push_back(i);
            }

            std::sort(perceived_order.begin(), perceived_order.end(),
                      [&](const uint64_t i1, const uint64_t i2)
                      {
                          if constexpr (proj_dir == cluster_potential_projection_direction::C2_ONTO_C1)
                          {
                              return all_pots[i1][j] < all_pots[i2][j];
                          }
                          else
                          {
                              return all_pots[j][i1] < all_pots[j][i2];
                          }
                      });

            perceived_order_c2_to_c1.emplace_back(perceived_order);
        }

        ordered_eqv_classes perceived_ordered_eq_classes_c2_to_c1{};
        perceived_ordered_eq_classes_c2_to_c1.reserve(c1->sidbs.size());

        std::vector<uint64_t> perceived_order_ixs{};
        perceived_order_ixs.reserve(c2->sidbs.size());
        for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
        {
            perceived_order_ixs.emplace_back(0);
        }

        // key: sidb_ix, val: row ixs to skip, since they have seen sidb_ix already
        std::map<uint64_t, std::set<uint64_t>> not_handled_sidb_ixs_with_skip_rows{};

        if (c1->uid == 24 && c2->uid == 37)
        {
            std::cout << "heree" << std::endl;
        }

        bool done = false;
        while (!done)
        {
            // start a new equivalence class with the first sidb ix not yet in an equivalence class
            const uint64_t sidb_ix_eqv_class_seed = perceived_order_c2_to_c1[0][perceived_order_ixs[0]];
            perceived_ordered_eq_classes_c2_to_c1.emplace_back(std::set{sidb_ix_eqv_class_seed});
            not_handled_sidb_ixs_with_skip_rows.emplace(std::make_pair(sidb_ix_eqv_class_seed, std::set<uint64_t>{}));

            while (!not_handled_sidb_ixs_with_skip_rows.empty())
            {
                // take first element of unhandled equivalence class members (out)
                const auto [sidb_ix, skip_rows] = ([](const std::map<uint64_t, std::set<uint64_t>>::node_type nh)
                                                   { return std::make_pair(nh.key(), nh.mapped()); })(
                    not_handled_sidb_ixs_with_skip_rows.extract(not_handled_sidb_ixs_with_skip_rows.cbegin()));
                //                not_handled_sidb_ixs.erase(not_handled_sidb_ixs.begin());

                for (uint64_t row_ix = 0; row_ix < c2->sidbs.size(); ++row_ix)
                {
                    // skip row if this row already saw the current sidb_ix
                    if (skip_rows.count(row_ix) != 0)
                    {
                        continue;
                    }

                    // find equivalent sidb ixs by incrementing the pointer at the current perceived order
                    while (perceived_order_c2_to_c1[row_ix][perceived_order_ixs[row_ix]++] != sidb_ix)
                    {
                        const uint64_t eqv_sidb_ix = perceived_order_c2_to_c1[row_ix][perceived_order_ixs[row_ix] - 1];
                        perceived_ordered_eq_classes_c2_to_c1.back().emplace(eqv_sidb_ix);
                        //                        not_handled_sidb_ixs.emplace(eqv_sidb_ix);
                        not_handled_sidb_ixs_with_skip_rows[eqv_sidb_ix].emplace(row_ix);
                    }

                    // if one perceived order reaches the end, we have put all sidb ixs in equivalence classes
                    if (perceived_order_ixs[row_ix] == c1->sidbs.size())
                    {
                        not_handled_sidb_ixs_with_skip_rows.clear();
                        done = true;
                        break;
                    }
                }
            }
        }

        return perceived_ordered_eq_classes_c2_to_c1;
    }

    void compute_potential_equivalence_classes(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (c == other_c)
            {
                continue;
            }

            const std::vector<std::vector<double>>& all_pots = compute_cluster_potential_matrix(c, other_c);

            const ordered_eqv_classes& c2_to_c1 =
                compute_projected_potential_eqv_classes<cluster_potential_projection_direction::C2_ONTO_C1>(c, other_c,
                                                                                                            all_pots);
            const ordered_eqv_classes& c1_to_c2 =
                compute_projected_potential_eqv_classes<cluster_potential_projection_direction::C1_ONTO_C2>(other_c, c,
                                                                                                            all_pots);

            std::shared_ptr<const std::vector<std::vector<double>>> sh_all_pots =
                std::make_shared<const std::vector<std::vector<double>>>(std::move(all_pots));

            c->potential_equivalence_classes.emplace(
                std::make_pair(other_c->uid, ordered_potential_information{true, sh_all_pots, c2_to_c1}));
            other_c->potential_equivalence_classes.emplace(
                std::make_pair(c->uid, ordered_potential_information{false, sh_all_pots, c1_to_c2}));
        }
    }

    enum class bound_calculation_mode
    {
        UPPER,
        LOWER
    };

    enum class bound_operation_mode
    {
        C2_BOUND,
        C2_ACCUMULATE
    };

    static bool may_take_cs(const uint64_t i, const sidb_clustering& bot, const sidb_charge_state cs)
    {
        for (const sidb_cluster_ptr& c : bot)
        {
            if (c->sidbs.count(i))
            {
                return c->charge_space.find(sidb_cluster_charge_state{cs}) != c->charge_space.cend();
            }
        }
        return false;
        //        const sidb_cluster_ptr& c_i = *std::find(bot.cbegin(), bot.cend(), [&](const sidb_cluster_ptr& c) {
        //        return c->sidbs.count(i);});
    }

    // compute the bound on the absolute potential projected from c1 onto c2 for n charges using the potential
    // equivalence classes information. for each element of c2, sum the potential received by n highest/lowest ordered
    // elements of c1.
    template <bound_calculation_mode bound, bound_operation_mode mode>
    static double compute_potential_bound_for_num_charges(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                                          uint64_t num_charges, const sidb_clustering& bot,
                                                          const sidb_charge_state cs) noexcept
    {
        const ordered_potential_information& pot_info = c1->potential_equivalence_classes.at(c2->uid);

        std::function<double(uint64_t, uint64_t)> get_c2_onto_c1_pot;

        if (pot_info.b)
        {
            get_c2_onto_c1_pot = [&](const uint64_t c1_ix, const uint64_t c2_ix)
            { return (*pot_info.pot_mat)[c1_ix][c2_ix]; };
        }
        else
        {
            get_c2_onto_c1_pot = [&](const uint64_t c1_ix, const uint64_t c2_ix)
            { return (*pot_info.pot_mat)[c2_ix][c1_ix]; };
        }

        uint64_t t = 0;
        uint64_t last_t;

        double sum = 0;

        if constexpr (bound == bound_calculation_mode::UPPER)
        {
            while (num_charges != 0 &&
                   num_charges >= pot_info.eqv_classes[pot_info.eqv_classes.size() - 1 - t++].size())
            {
                const std::set<uint64_t>& eqv_class = pot_info.eqv_classes[pot_info.eqv_classes.size() - t];

                uint64_t skipped = 0;

                for (const uint64_t eqv_sidb_ix : eqv_class)
                {
                    if (!may_take_cs(*std::next(c1->sidbs.cbegin(), static_cast<int64_t>(eqv_sidb_ix)), bot, cs))
                    {
                        skipped++;
                        continue;
                    }

                    if constexpr (mode == bound_operation_mode::C2_BOUND)
                    {
                        sum += get_c2_onto_c1_pot(eqv_sidb_ix, c2->sidbs.size() + 1);  // ix of stored maximum
                    }
                    else
                    {
                        for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
                        {
                            sum += get_c2_onto_c1_pot(eqv_sidb_ix, j);
                        }
                    }
                }

                num_charges -= eqv_class.size() - skipped;
            }

            last_t = pot_info.eqv_classes.size() - t;
        }
        else
        {
            while (num_charges != 0 && num_charges >= pot_info.eqv_classes[t++].size())
            {
                const std::set<uint64_t>& eqv_class = pot_info.eqv_classes[t - 1];

                uint64_t skipped = 0;

                for (const uint64_t eqv_sidb_ix : eqv_class)
                {
                    if (!may_take_cs(*std::next(c1->sidbs.cbegin(), static_cast<int64_t>(eqv_sidb_ix)), bot, cs))
                    {
                        skipped++;
                        continue;
                    }

                    if constexpr (mode == bound_operation_mode::C2_BOUND)
                    {
                        sum += get_c2_onto_c1_pot(eqv_sidb_ix, c2->sidbs.size());  // ix of stored minimum
                    }
                    else
                    {
                        for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
                        {
                            sum += get_c2_onto_c1_pot(eqv_sidb_ix, j);
                        }
                    }
                }

                num_charges -= eqv_class.size() - skipped;
            }

            last_t = t - 1;
        }

        if (num_charges == 0)
        {
            return sum;
        }

        // get bound of n choose k combinations for the remainder
        double remaining_bound = 0;

        std::vector<bool> bitmask(pot_info.eqv_classes[last_t].size(), false);
        std::fill(bitmask.begin(), std::next(bitmask.begin(), static_cast<int64_t>(num_charges)), true);

        do {
            double combination_sum = 0;

            for (uint64_t k = 0; k < pot_info.eqv_classes[last_t].size(); ++k)
            {
                if (bitmask[k])
                {
                    const uint64_t eqv_sidb_ix =
                        *std::next(pot_info.eqv_classes[last_t].cbegin(), static_cast<int64_t>(k));

                    if (!may_take_cs(*std::next(c1->sidbs.cbegin(), static_cast<int64_t>(eqv_sidb_ix)), bot, cs))
                    {
                        break;
                    }

                    if constexpr (mode == bound_operation_mode::C2_BOUND)
                    {
                        if constexpr (bound == bound_calculation_mode::UPPER)
                        {
                            combination_sum += get_c2_onto_c1_pot(eqv_sidb_ix, c2->sidbs.size() + 1);  // stored max
                        }
                        else
                        {
                            combination_sum += get_c2_onto_c1_pot(eqv_sidb_ix, c2->sidbs.size());  // stored min
                        }
                    }
                    else
                    {
                        for (uint64_t j = 0; j < c2->sidbs.size(); ++j)
                        {
                            sum += get_c2_onto_c1_pot(eqv_sidb_ix, j);
                        }
                    }
                }
            }

            if constexpr (bound == bound_calculation_mode::UPPER)
            {
                remaining_bound = std::min(remaining_bound, combination_sum);
            }
            else
            {
                remaining_bound = std::max(remaining_bound, combination_sum);
            }

        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

        return sum + remaining_bound;
    }

    // compute the bound value of the potential energy of c1 projected onto c2 for a cluster charge state multiset
    template <bound_calculation_mode bound, bound_operation_mode mode = bound_operation_mode::C2_BOUND>
    inline double cluster_charge_state_bound_value(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                                   const sidb_cluster_charge_state& m) const noexcept
    {
        // implement solution caching?
        if constexpr (bound == bound_calculation_mode::UPPER)
        {
            return -compute_potential_bound_for_num_charges<bound_calculation_mode::UPPER, mode>(
                       c1, c2, m.neg_count, bot, sidb_charge_state::NEGATIVE) +
                   compute_potential_bound_for_num_charges<bound_calculation_mode::LOWER, mode>(
                       c1, c2, m.pos_count, bot, sidb_charge_state::POSITIVE);
        }
        else
        {
            return -compute_potential_bound_for_num_charges<bound_calculation_mode::LOWER, mode>(
                       c1, c2, m.neg_count, bot, sidb_charge_state::NEGATIVE) +
                   compute_potential_bound_for_num_charges<bound_calculation_mode::UPPER, mode>(
                       c1, c2, m.pos_count, bot, sidb_charge_state::POSITIVE);
        }
    }

    template <bound_calculation_mode bound, const sidb_charge_state without_one_cs = sidb_charge_state::NONE>
    double internal_pot_bound_value(const sidb_cluster_ptr& c, const sidb_cluster_charge_state& m) const noexcept
    {
        if (c->sidbs.size() == 1)
        {
            return 0;
        }

        double bound_value;
        if constexpr (bound == bound_calculation_mode::UPPER)
        {
            bound_value = std::numeric_limits<double>::infinity();
        }
        else
        {
            bound_value = -std::numeric_limits<double>::infinity();
        }

        // find the bound for all decompositions of m
        for (const std::vector<std::pair<sidb_cluster_ptr, uint64_t>>& decomposition : m.decompositions)
        {
            for (const auto& [child, m_part] : decomposition)
            {
                if constexpr (without_one_cs == sidb_charge_state::NEGATIVE)
                {
                    if (sidb_cluster_charge_state{m_part}.neg_count == 0)
                    {
                        continue;
                    }
                }
                else if constexpr (without_one_cs == sidb_charge_state::POSITIVE)
                {
                    if (sidb_cluster_charge_state{m_part}.pos_count == 0)
                    {
                        continue;
                    }
                }
                else if constexpr (without_one_cs == sidb_charge_state::NEUTRAL)
                {
                    if (!sidb_cluster_charge_state{m_part}.contains_neutral_charge(child->sidbs.size()))
                    {
                        continue;
                    }
                }
                const sidb_cluster_charge_state& located_m_part =
                    *child->charge_space.find(sidb_cluster_charge_state{m_part});

                double maybe_bound_value = internal_pot_bound_value<bound, without_one_cs>(child, located_m_part);

                // sum the bound values of m_part onto each sibling
                for (const auto& [other_child, other_m_part] : decomposition)
                {
                    if (child == other_child)
                    {
                        continue;
                    }

                    const sidb_cluster_charge_state& located_other_m_part =
                        *other_child->charge_space.find(sidb_cluster_charge_state{other_m_part});

                    maybe_bound_value += cluster_charge_state_bound_value<bound, bound_operation_mode::C2_BOUND>(
                        other_child, child, located_other_m_part);
                }

                if constexpr (bound == bound_calculation_mode::UPPER)
                {
                    bound_value = std::min(bound_value, maybe_bound_value);
                }
                else
                {
                    bound_value = std::max(bound_value, maybe_bound_value);
                }
            }
        }

        return bound_value;
    }

    bool check_cluster_charge_state(const sidb_cluster_ptr& c, const sidb_cluster_charge_state& m) const noexcept
    {
        if (m.neg_count != 0)
        {
            if (-c->local_pot_bounds.first -
                    internal_pot_bound_value<bound_calculation_mode::LOWER, sidb_charge_state::NEGATIVE>(c, m) >=
                mu_bounds_with_error[0])
            {
                return false;
            }
        }

        if (m.pos_count != 0)
        {
            if (-c->local_pot_bounds.second -
                    internal_pot_bound_value<bound_calculation_mode::UPPER, sidb_charge_state::POSITIVE>(c, m) <=
                mu_bounds_with_error[3])
            {
                return false;
            }
        }

        if (m.contains_neutral_charge(c->sidbs.size()))
        {
            if (-c->local_pot_bounds.second - internal_pot_bound_value<bound_calculation_mode::UPPER>(c, m) <=
                    mu_bounds_with_error[1] ||
                -c->local_pot_bounds.first - internal_pot_bound_value<bound_calculation_mode::LOWER>(c, m) >=
                    mu_bounds_with_error[2])
            {
                return false;
            }
        }

        return true;
    }

    using charge_spaces_update_result = std::vector<std::pair<sidb_cluster_ptr, std::vector<uint64_t>>>;

    void check_charge_space(const sidb_cluster_ptr& c, charge_spaces_update_result& res) const noexcept
    {
        std::vector<uint64_t> removed_multisets{};
        removed_multisets.reserve(c->charge_space.size());

        for (const sidb_cluster_charge_state& m : c->charge_space)
        {
            if (!check_cluster_charge_state(c, m))
            {
                removed_multisets.push_back(static_cast<uint64_t>(m));
            }
        }

        if (!removed_multisets.empty())
        {
            res.push_back({c, std::move(removed_multisets)});
        }
    }

    template <bound_calculation_mode bound>
    std::pair<double, uint64_t> find_new_interaction_bound(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                                                           const std::vector<uint64_t>& removed_ms = {}) const noexcept
    {
        uint64_t new_bound_m;

        double new_bound;
        if constexpr (bound == bound_calculation_mode::UPPER)
        {
            new_bound = std::numeric_limits<double>::infinity();
        }
        else
        {
            new_bound = -std::numeric_limits<double>::infinity();
        }

        // iterate over the entire charge space (?)
        for (const sidb_cluster_charge_state& maybe_bound_m : c1->charge_space)
        {
            if (std::find(removed_ms.cbegin(), removed_ms.cend(), static_cast<uint64_t>(maybe_bound_m)) !=
                removed_ms.cend())
            {
                continue;
            }

            // double comparison without error?
            const double maybe_bound = cluster_charge_state_bound_value<bound>(c1, c2, maybe_bound_m);

            if constexpr (bound == bound_calculation_mode::UPPER)
            {
                if (maybe_bound < new_bound)
                {
                    new_bound_m = static_cast<uint64_t>(maybe_bound_m);
                    new_bound   = maybe_bound;
                }
            }
            else
            {
                if (maybe_bound > new_bound)
                {
                    new_bound_m = static_cast<uint64_t>(maybe_bound_m);
                    new_bound   = maybe_bound;
                }
            }
        }

        return {new_bound, new_bound_m};
    }

    template <bound_calculation_mode bound>
    void update_bound(const sidb_cluster_ptr& c1, const sidb_cluster_ptr& c2,
                      const std::vector<uint64_t>& removed_ms) const noexcept
    {

        // iterate over the entire charge space?
        const auto& [new_bound, new_bound_m] = find_new_interaction_bound<bound>(c1, c2, removed_ms);

        // make updates
        if constexpr (bound == bound_calculation_mode::UPPER)
        {
            const double current_bound                      = c1->projected_pot_bounds.at(c2->uid).first.second;
            c1->projected_pot_bounds[c2->uid].first.second  = new_bound;
            c1->projected_pot_bounds[c2->uid].second.second = new_bound_m;

            c2->local_pot_bounds.second += new_bound - current_bound;
        }
        else
        {
            const double current_bound = c1->projected_pot_bounds.at(c2->uid).first.first;

            c1->projected_pot_bounds[c2->uid].first.first  = new_bound;
            c1->projected_pot_bounds[c2->uid].second.first = new_bound_m;

            c2->local_pot_bounds.first += new_bound - current_bound;
        }
    }

    void apply_update(const charge_spaces_update_result& update_res) const noexcept
    {
        for (const auto& [c, removed_ms] : update_res)
        {
            for (const sidb_cluster_ptr& other_c : clustering)
            {
                if (c == other_c)  // hmmm
                {
                    continue;
                }

                // perhaps reason over epsilon accurate equivalence sets?
                const cluster_charge_state_bounds& m_bounds = c->projected_pot_bounds.at(other_c->uid).second;

                bool min_updated = false, max_updated = false;
                for (const uint64_t m : removed_ms)
                {
                    if (min_updated && max_updated)
                    {
                        break;
                    }

                    if (!min_updated && m == m_bounds.first)
                    {
                        // update minimum potential
                        update_bound<bound_calculation_mode::LOWER>(c, other_c, removed_ms);
                        min_updated = true;
                    }
                    else if (!max_updated && m == m_bounds.second)
                    {
                        // update maximum potential
                        update_bound<bound_calculation_mode::UPPER>(c, other_c, removed_ms);
                        max_updated = true;
                    }
                }
            }

            for (const uint64_t& m : removed_ms)
            {
                c->charge_space.erase(static_cast<sidb_cluster_charge_state>(m));
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
            m.decompositions[0].emplace_back(std::make_pair(cur_child, static_cast<uint64_t>(m_part)));
            fill_merged_charge_state_space(c, current_child_ix + 1, m);
            m -= m_part;
            m.decompositions[0].pop_back();
        }
    }

    void compute_merged_charge_state_space(const sidb_cluster_ptr& c) const noexcept
    {
        sidb_cluster_charge_state m{};
        m.decompositions.emplace_back(std::vector<std::pair<sidb_cluster_ptr, uint64_t>>{});
        fill_merged_charge_state_space(c, 0, m);
    }

    void compute_merged_local_potential_bounds(const sidb_cluster_ptr& c) const noexcept
    {
        c->local_pot_bounds = {-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};

        for (const sidb_cluster_ptr& child : c->children)
        {
            potential_bounds pot_bounds_without_siblings = child->local_pot_bounds;

            // should make use of the symmetric property of potentials between children
            for (const sidb_cluster_ptr& other_child : c->children)
            {
                // is this correct?
                if (child == other_child)
                {
                    continue;
                }

                const potential_bounds& projected_pot_bounds = other_child->projected_pot_bounds.at(child->uid).first;
                pot_bounds_without_siblings.first -= projected_pot_bounds.first;
                pot_bounds_without_siblings.second -= projected_pot_bounds.second;
            }

            c->local_pot_bounds.first  = std::max(c->local_pot_bounds.first, pot_bounds_without_siblings.first);
            c->local_pot_bounds.second = std::min(c->local_pot_bounds.second, pot_bounds_without_siblings.second);
        }
    }

    void compute_merged_interaction_bounds(const sidb_cluster_ptr& c) const noexcept
    {
        for (const sidb_cluster_ptr& other_c : clustering)
        {
            if (c == other_c)
            {
                continue;
            }

            sidb_cluster_charge_state min_m{}, max_m{};
            potential_bounds          new_proj_pot_bounds{};

            std::set<uint64_t> min_recv_m{}, max_recv_m{};

            for (const sidb_cluster_ptr& child : c->children)
            {
                // projected potential
                const interaction_bounds& proj_bounds = child->projected_pot_bounds.at(other_c->uid);
                new_proj_pot_bounds.first += proj_bounds.first.first;
                new_proj_pot_bounds.second += proj_bounds.first.second;

                min_m += sidb_cluster_charge_state{proj_bounds.second.first};
                max_m += sidb_cluster_charge_state{proj_bounds.second.second};

                // received potential
                const interaction_bounds& recv_bounds = other_c->projected_pot_bounds.at(child->uid);

                min_recv_m.emplace(recv_bounds.second.first);
                max_recv_m.emplace(recv_bounds.second.second);
            }

            c->projected_pot_bounds.emplace(std::make_pair(
                other_c->uid,
                interaction_bounds{new_proj_pot_bounds, {static_cast<uint64_t>(min_m), static_cast<uint64_t>(max_m)}}));

            // construct received potential bounds (other_c onto c)
            potential_bounds            new_recv_pot_bounds{};
            cluster_charge_state_bounds new_recv_pot_bounds_m{};

            // check lower bound conflict
            if (min_recv_m.size() == 1)
            {
                // no conflict
                new_recv_pot_bounds.first = cluster_charge_state_bound_value<bound_calculation_mode::LOWER>(
                    other_c, c, sidb_cluster_charge_state{*min_recv_m.cbegin()});
                new_recv_pot_bounds_m.first = *min_recv_m.cbegin();
            }
            else
            {
                // conflict found, solve exhaustively (?)
                const auto& [new_lb, new_lb_m] = find_new_interaction_bound<bound_calculation_mode::LOWER>(other_c, c);
                new_recv_pot_bounds.first      = new_lb;
                new_recv_pot_bounds_m.first    = new_lb_m;
            }

            // check upper bound conflict
            if (max_recv_m.size() == 1)
            {
                // no conflict
                new_recv_pot_bounds.second = cluster_charge_state_bound_value<bound_calculation_mode::UPPER>(
                    other_c, c, sidb_cluster_charge_state{*max_recv_m.cbegin()});
                new_recv_pot_bounds_m.second = *max_recv_m.cbegin();
            }
            else
            {
                // conflict found, solve exhaustively (?) (also, could it be done for LB and UB at the same time?)
                const auto& [new_ub, new_ub_m] = find_new_interaction_bound<bound_calculation_mode::UPPER>(other_c, c);
                new_recv_pot_bounds.second     = new_ub;
                new_recv_pot_bounds_m.second   = new_ub_m;
            }

            other_c->projected_pot_bounds.emplace(
                std::make_pair(c->uid, interaction_bounds{new_recv_pot_bounds, new_recv_pot_bounds_m}));
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
        compute_potential_equivalence_classes(min_parent);  // dynamically construct?
        compute_merged_interaction_bounds(min_parent);
        compute_merged_local_potential_bounds(min_parent);

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
