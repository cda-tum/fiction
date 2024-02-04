//
// Created by Willem Lambooy on 21/12/2023.
//

#ifndef FICTION_EXACT_CLUSTER_ITERATION_HPP
#define FICTION_EXACT_CLUSTER_ITERATION_HPP

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
// #include <fiction/technology/potential_hierarchy.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_cluster_hierarchy.hpp>
#include <fiction/utils/hash.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction
{

struct cluster
{
    std::set<uint64_t>                            c;
    std::vector<std::optional<sidb_charge_state>> preassigned;
    uint64_t                                      preassigned_count = 0;

    explicit cluster() = default;

    explicit cluster(const std::set<uint64_t>& c_) : c{c_}
    {
        preassigned.reserve(c.size());
        for (uint64_t i = 0; i < c_.size(); ++i)
        {
            preassigned.emplace_back(std::nullopt);
        }
    }

    [[nodiscard]] constexpr inline uint64_t size() const noexcept
    {
        return c.size();
    }

    constexpr inline bool operator==(const cluster& other) const noexcept
    {
        return c == other.c;
    }

    constexpr inline bool operator!=(const cluster& other) const noexcept
    {
        return c != other.c;
    }

    constexpr inline bool operator<(const cluster& other) const noexcept
    {
        return c < other.c;
    }
};

using clustering = std::set<cluster>;

// Define the type for a multiset of charge states
using charge_state_multiset = std::multiset<sidb_charge_state>;

// Define the types for charge state assignments
using sidb_charge_conf    = std::vector<sidb_charge_state>;
using cluster_charge_conf = std::map<std::set<uint64_t>, charge_state_multiset>;

struct charge_multiset_conf
{
    using charge_count = std::map<sidb_charge_state, uint64_t>;

    const std::pair<std::vector<uint64_t>, std::set<uint64_t>> cluster_sizes;

    const charge_count match_charge_counts;

    const std::size_t end_conf_hash;  // too probabilistic?

    uint64_t row_nr;

    charge_count locked_charge_counts{{sidb_charge_state::NEGATIVE, 0},
                                      {sidb_charge_state::NEUTRAL, 0},
                                      {sidb_charge_state::POSITIVE, 0}};
    charge_count non_locked_charge_counts{locked_charge_counts};

    struct configuration
    {
        std::vector<sidb_charge_state> vec{};
        charge_count                   counts{};
        uint64_t                       lock_ix{};
    };

    std::vector<configuration> confs{};

    explicit charge_multiset_conf(const clustering& xv, const charge_state_multiset& m) :
            cluster_sizes{get_cluster_sizes(xv)},
            match_charge_counts{{sidb_charge_state::NEGATIVE, m.count(sidb_charge_state::NEGATIVE)},
                                {sidb_charge_state::NEUTRAL, m.count(sidb_charge_state::NEUTRAL)},
                                {sidb_charge_state::POSITIVE, m.count(sidb_charge_state::POSITIVE)}},
            end_conf_hash{compute_end_conf_hash(cluster_sizes.first, m)},
            row_nr{cluster_sizes.first.size() - 1}
    {
        // initialize with left diagonal configuration
        confs.reserve(cluster_sizes.first.size());
        for (uint64_t i = 0; i < cluster_sizes.first.size(); ++i)
        {
            configuration conf{};
            conf.lock_ix = cluster_sizes.first[i] - 1;

            uint64_t row_total_assigned = 0;
            for (const sidb_charge_state cs : sidb_charge_state_iterator{})
            {
                const uint64_t num_assignable = std::min(match_charge_counts.at(cs) - locked_charge_counts.at(cs),
                                                         cluster_sizes.first[i] - row_total_assigned);

                conf.counts[cs] = num_assignable;

                for (uint64_t k = 0; k < num_assignable; ++k)
                {
                    conf.vec.push_back(cs);
                }

                locked_charge_counts[cs] += num_assignable;
                row_total_assigned += num_assignable;
            }

            confs.push_back(conf);
        }
    }

    void fill_column_from(const uint64_t i, const sidb_charge_state cs, uint64_t& row_total_assigned, const bool rev_ix)
    {
        if (cs == sidb_charge_state::NONE)
        {
            return;
        }

        while (locked_charge_counts.at(cs) + non_locked_charge_counts.at(cs) < match_charge_counts.at(cs) &&
               row_total_assigned < confs[i].vec.size())
        {
            confs[i].vec[rev_ix ? confs[i].vec.size() - 1 - row_total_assigned++ : row_total_assigned++] = cs;
            confs[i].counts[cs]++;
            non_locked_charge_counts[cs]++;
        }

        fill_column_from(i, *(++sidb_charge_state_iterator{cs}), row_total_assigned, rev_ix);
    }

    void fill_rows_from(const uint64_t i)
    {
        if (i == confs.size())
        {
            return;
        }

        confs[i].counts = charge_count{{sidb_charge_state::NEGATIVE, 0},
                                       {sidb_charge_state::NEUTRAL, 0},
                                       {sidb_charge_state::POSITIVE, 0}};

        uint64_t row_total_assigned = 0;
        fill_column_from(i, *sidb_charge_state_iterator{}, row_total_assigned, false);

        fill_rows_from(i + 1);
    }

    bool make_step()
    {
        const sidb_charge_state move_column = confs[row_nr].vec[confs[row_nr].lock_ix];

        const auto column_free = [&](const sidb_charge_state cs)
        { return locked_charge_counts.at(cs) != match_charge_counts.at(cs); };
        const sidb_charge_state next_free_column =
            *std::find_if(++sidb_charge_state_iterator{move_column}, sidb_charge_state_iterator{}.end(), column_free);

        if (next_free_column == sidb_charge_state::NONE)
        {
            return false;
        }

        confs[row_nr].vec[confs[row_nr].lock_ix] = next_free_column;

        confs[row_nr].counts[next_free_column]++;
        confs[row_nr].counts[move_column]--;

        locked_charge_counts[next_free_column]++;
        locked_charge_counts[move_column]--;

        if (next_free_column != sidb_charge_state::POSITIVE)
        {
            //            uint64_t row_total_assigned = 0;
            //            for (const sidb_charge_state cs : sidb_charge_state_iterator{})
            //            {
            //                if (charge_state_to_sign(cs) > charge_state_to_sign(next_free_column))
            //                {
            //                    locked_charge_counts[cs] -= confs[row_nr].counts[cs];
            //                    confs[row_nr].counts[cs] = 0;
            //                }
            //                else
            //                {
            //                    row_total_assigned += confs[row_nr].counts[cs];
            //                }
            //            }

            // branchless
            uint64_t row_total_assigned =
                confs[row_nr].counts[sidb_charge_state::NEGATIVE] + confs[row_nr].counts[sidb_charge_state::NEUTRAL];
            locked_charge_counts[sidb_charge_state::POSITIVE] -= confs[row_nr].counts[sidb_charge_state::POSITIVE];
            confs[row_nr].counts[sidb_charge_state::POSITIVE] = 0;

            fill_column_from(row_nr, sidb_charge_state::NEUTRAL, row_total_assigned, false);
        }

        fill_rows_from(row_nr + 1);

        for (const sidb_charge_state cs : sidb_charge_state_iterator{})
        {
            locked_charge_counts[cs] += non_locked_charge_counts[cs];
        }

        non_locked_charge_counts = {{sidb_charge_state::NEGATIVE, 0},
                                    {sidb_charge_state::NEUTRAL, 0},
                                    {sidb_charge_state::POSITIVE, 0}};

        for (; row_nr != confs.size(); ++row_nr)
        {
            confs[row_nr].lock_ix = confs[row_nr].vec.size() - 1;
        }
        row_nr--;

        return true;
    }

    bool next_conf() noexcept
    {
        if (is_end_conf())
        {
            return false;
        }

        while (!make_step())
        {
            if (confs[row_nr].lock_ix-- != 0)
            {
                continue;
            }

            for (const sidb_charge_state cs : sidb_charge_state_iterator{})
            {
                locked_charge_counts[cs] -= confs[row_nr].counts.at(cs);
            }

            confs[row_nr].lock_ix = confs[row_nr].vec.size() - 1;

            row_nr--;
        }

        return true;
    }

    [[nodiscard]] bool is_end_conf() const noexcept
    {
        size_t h = 0;
        for (const configuration& conf : confs)
        {
            for (const sidb_charge_state cs : sidb_charge_state_iterator{})
            {
                hash_combine(h, conf.counts.at(cs));
            }
        }
        return h == end_conf_hash;
    }

    [[nodiscard]] cluster_charge_conf
    make_charge_multiset_conf(const clustering& xv, cluster_charge_conf sigma = cluster_charge_conf{}) const noexcept
    {
        for (uint64_t i = 0, conf_ix = 0; i < xv.size(); ++i)
        {
            const std::set<uint64_t>& c = std::next(xv.cbegin(), static_cast<int64_t>(i))->c;
            if (cluster_sizes.second.count(i))
            {
                sigma.insert({c, charge_state_multiset{}});
                continue;
            }
            const std::vector<sidb_charge_state> conf = confs[conf_ix++].vec;
            sigma.insert({c, charge_state_multiset{conf.cbegin(), conf.cend()}});
        }

        return sigma;
    }

    static std::pair<std::vector<uint64_t>, std::set<uint64_t>> get_cluster_sizes(const clustering& xv)
    {
        std::pair<std::vector<uint64_t>, std::set<uint64_t>> cluster_sizes{};
        cluster_sizes.first.reserve(xv.size());
        for (uint64_t i = 0; i < xv.size(); ++i)
        {
            const cluster& c    = *std::next(xv.cbegin(), static_cast<int64_t>(i));
            uint64_t       size = c.size() - c.preassigned_count;
            if (size == 0)
            {
                cluster_sizes.second.emplace(i);
            }
            else
            {
                cluster_sizes.first.emplace_back(c.size() - c.preassigned_count);
            }
        }
        return cluster_sizes;
    }

    static size_t compute_end_conf_hash(const std::vector<uint64_t>& bin_sizes, const charge_state_multiset& m) noexcept
    {
        charge_count charge_counts{{sidb_charge_state::NEGATIVE, m.count(sidb_charge_state::NEGATIVE)},
                                   {sidb_charge_state::NEUTRAL, m.count(sidb_charge_state::NEUTRAL)},
                                   {sidb_charge_state::POSITIVE, m.count(sidb_charge_state::POSITIVE)}};

        // compute right diagonal configuration
        std::vector<charge_count> row_charge_counts{};
        row_charge_counts.reserve(bin_sizes.size());
        for (uint64_t i = 0; i < bin_sizes.size(); ++i)
        {
            row_charge_counts.emplace_back();

            uint64_t row_total_assigned = 0;

            for (const sidb_charge_state cs : sidb_charge_state_iterator{sidb_charge_state::POSITIVE, true})
            {
                const uint64_t num_assignable = std::min(charge_counts.at(cs), bin_sizes[i] - row_total_assigned);

                charge_counts[cs] -= num_assignable;
                row_charge_counts[i][cs] = num_assignable;
                row_total_assigned += num_assignable;
            }
        }

        // compute hash
        size_t h = 0;
        for (const charge_count& row_charge_count : row_charge_counts)
        {
            for (const sidb_charge_state cs : sidb_charge_state_iterator{})
            {
                hash_combine(h, row_charge_count.at(cs));
            }
        }

        return h;
    }
};

template <typename Lyt>
class cluster_exact
{
  public:
    explicit cluster_exact(const Lyt& lyt, const std::optional<uint64_t> min_key_size = std::nullopt,
                           const uint64_t num_threads = std::thread::hardware_concurrency()) :
            cl{get_charge_layout_and_check_three_state_simulation_required(lyt)},
            negative_sidbs{get_negative_sidbs(cl)},
            mu_bounds_with_error{physical_constants::POP_STABILITY_ERR - cl.get_phys_params().mu_minus,
                                 -physical_constants::POP_STABILITY_ERR - cl.get_phys_params().mu_minus,
                                 physical_constants::POP_STABILITY_ERR - cl.get_phys_params().mu_plus(),
                                 -physical_constants::POP_STABILITY_ERR - cl.get_phys_params().mu_plus()},
            minimum_key_size{min_key_size.has_value() ?
                                 min_key_size.value() :
                                 std::min(12ul, static_cast<uint64_t>(static_cast<double>(cl.num_cells()) / 2.0))},
            threads{num_threads}
    {}

    static charge_distribution_surface<Lyt> get_charge_layout_and_check_three_state_simulation_required(const Lyt& lyt)
    {
        charge_distribution_surface<Lyt> cl{lyt};
        cl.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        cl.assign_base_number(2);
        cl.is_three_state_simulation_required();
        return cl;
    }

    static std::set<uint64_t> get_negative_sidbs(const charge_distribution_surface<Lyt>& cl)
    {

        const std::vector<int64_t>& negative_sidbs = cl.negative_sidb_detection();
        std::set<uint64_t>          negative_sidbs_converted{};
        for (const int64_t i : negative_sidbs)
        {
            negative_sidbs_converted.emplace(static_cast<uint64_t>(i));
        }
        return negative_sidbs_converted;
    }

    const charge_distribution_surface<Lyt> cl;
    const std::vector<sidb_charge_state>   possible_charge_states;
    const std::set<uint64_t>               negative_sidbs;
    std::set<uint64_t>                     positive_sidbs{};

    const std::array<double, 4> mu_bounds_with_error;

    const uint64_t minimum_key_size;
    const uint64_t threads;

    // Recursive function to generate charge assignments
    void generate_assignments_recursive(std::vector<sidb_charge_conf>&  charge_confs,
                                        std::vector<sidb_charge_state>& current_assignment, uint64_t current_index,
                                        uint64_t remaining_count, const charge_state_multiset& target_multiset)
    {
        // Base case: If all elements are assigned
        if (remaining_count == 0)
        {
            // Check if the assignment matches the target multiset
            if (charge_state_multiset{current_assignment.cbegin(), current_assignment.cend()} == target_multiset)
            {
                charge_confs.push_back(current_assignment);
            }
            return;
        }

        // Recursive case: Try assigning each charge state to the current element
        for (const sidb_charge_state charge_state : {sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL})
        {
            current_assignment[current_index] = charge_state;
            generate_assignments_recursive(charge_confs, current_assignment, current_index + 1, remaining_count - 1,
                                           target_multiset);
        }
    }

    // Function to generate charge state assignments within a cluster
    std::vector<sidb_charge_conf> generate_charge_confs(const uint64_t&              size,
                                                        const charge_state_multiset& target_multiset)
    {
        // Vector to store charge state assignments
        std::vector<sidb_charge_conf> charge_confs;

        // Initialize the current assignment vector
        std::vector<sidb_charge_state> current_assignment(size, sidb_charge_state::NEGATIVE);

        // Start the recursive generation
        generate_assignments_recursive(charge_confs, current_assignment, 0, size, target_multiset);

        return charge_confs;
    }

    struct sidb_charge_conf_store_key
    {
        uint64_t              size{};
        charge_state_multiset multiset{};

      public:
        sidb_charge_conf_store_key() : multiset{} {}
        sidb_charge_conf_store_key(const uint64_t& s, charge_state_multiset m) : size{s}, multiset{std::move(m)} {}

        bool operator==(const sidb_charge_conf_store_key& other) const noexcept
        {
            return size == other.size && multiset == other.multiset;
        }

        std::size_t operator()(const sidb_charge_conf_store_key& key) const noexcept
        {
            std::size_t h = key.size;
            hash_combine(h, key.multiset);
            return h;
        }
    };

    using sidb_charge_conf_store =
        std::unordered_map<sidb_charge_conf_store_key, const std::vector<sidb_charge_conf>, sidb_charge_conf_store_key>;

    sidb_charge_conf_store charge_conf_store{};

    enum potential_bound
    {
        UPPER,
        LOWER
    };

    struct potential_bounds_store_key
    {
        cluster                    c1{};
        cluster                    c2{};
        sidb_charge_conf_store_key charge_confs{};
        potential_bound            bound{};

        potential_bounds_store_key() : charge_confs{} {}
        potential_bounds_store_key(cluster c1_, cluster c2_, const sidb_charge_conf_store_key& charge_confs_,
                                   const potential_bound bound_) :
                c1{std::move(c1_)},
                c2{std::move(c2_)},
                charge_confs{charge_confs_},
                bound{bound_}
        {}

        bool operator==(const potential_bounds_store_key& other) const noexcept
        {
            return c1 == other.c1 && c2 == other.c2 && charge_confs == other.charge_confs;
        }

        std::size_t operator()(const potential_bounds_store_key& key) const noexcept
        {
            std::size_t h = 0;
            hash_combine(h, key.c1.c);
            hash_combine(h, key.c2.c);
            hash_combine(h, sidb_charge_conf_store_key{}(key.charge_confs));
            hash_combine(h, key.bound);
            return h;
        }
    };

    using potential_bounds_store =
        std::unordered_map<potential_bounds_store_key, const double, potential_bounds_store_key>;

    potential_bounds_store pot_bounds_store{};

    // Calculate the minimum or maximum potential energy of c2 projected onto c1
    double calculate_cluster_potential(const cluster& c1, const cluster& c2,
                                       const std::vector<sidb_charge_conf>& charge_confs,
                                       const potential_bound                bound) const noexcept
    {
        double pot_bound = bound == potential_bound::LOWER ? std::numeric_limits<double>::infinity() :
                                                             -std::numeric_limits<double>::infinity();

        for (const uint64_t i : c1.c)
        {
            for (const sidb_charge_conf& charge_conf : charge_confs)
            {
                double energy = 0;

                for (uint64_t cc_ix = 0, ix = 0; ix < c2.size(); ++ix)
                {
                    const uint64_t j = *std::next(c2.c.cbegin(), static_cast<int64_t>(ix));

                    if (j == i)
                    {
                        continue;
                    }

                    energy += cl.get_chargless_potential_by_indices(i, j) *
                              (c2.preassigned[ix].has_value() ? charge_state_to_sign(c2.preassigned[ix].value()) :
                                                                charge_state_to_sign(charge_conf[cc_ix++]));
                }

                pot_bound = bound == potential_bound::LOWER ? std::min(pot_bound, energy) : std::max(pot_bound, energy);
            }
        }

        return pot_bound;
    }

    std::mutex cc_store_mutex{};

    std::vector<sidb_charge_conf> get_charge_confs(const sidb_charge_conf_store_key& key) noexcept
    {
        {
            const std::lock_guard lock{cc_store_mutex};

            if (charge_conf_store.count(key))
            {
                return charge_conf_store.at(key);
            }
        }

        const std::vector<sidb_charge_conf>& charge_confs = generate_charge_confs(key.size, key.multiset);

        {
            const std::lock_guard lock{cc_store_mutex};

            if (!charge_conf_store.count(key))
            {
                charge_conf_store.emplace(std::make_pair(key, charge_confs));
            }
        }

        return charge_confs;
    }

    std::mutex pb_store_mutex{};

    // Get the minimum or maximum potential energy of c2 projected onto c1
    double get_cluster_potential(const cluster& c1, const cluster& c2, const charge_state_multiset& multiset,
                                 const potential_bound bound) noexcept
    {
        const sidb_charge_conf_store_key charge_conf_key{c2.c.size() - c2.preassigned_count, multiset};

        if (c1.size() + c2.size() < minimum_key_size)
        {
            return calculate_cluster_potential(c1, c2, get_charge_confs(charge_conf_key), bound);
        }

        const potential_bounds_store_key pot_bounds_key{c1, c2, charge_conf_key, bound};

        {
            const std::lock_guard lock{pb_store_mutex};

            if (pot_bounds_store.count(pot_bounds_key))
            {
                return pot_bounds_store.at(pot_bounds_key);
            }
        }

        const double pot_bound = calculate_cluster_potential(c1, c2, get_charge_confs(charge_conf_key), bound);

        {
            const std::lock_guard lock{pb_store_mutex};

            if (!pot_bounds_store.count(pot_bounds_key))
            {
                pot_bounds_store.emplace(std::make_pair(pot_bounds_key, pot_bound));
            }
        }

        return pot_bound;
    }

    // Function to calculate the internal lower bound of local potential energy for a cluster C
    double V_int_lower(const cluster& c, const charge_state_multiset& m) noexcept
    {
        if (c.size() == 1)
        {
            return 0;
        }

        return get_cluster_potential(c, c, m, potential_bound::LOWER);
    }

    // Function to calculate the internal upper bound of local potential energy for a cluster C
    double V_int_upper(const cluster& c, const charge_state_multiset& m) noexcept
    {
        if (c.size() == 1)
        {
            return 0;
        }

        return get_cluster_potential(c, c, m, potential_bound::UPPER);
    }

    // Function to calculate the external lower bound of local potential energy for a cluster C in a clustering X
    double V_ext_lower(const cluster& c, const cluster_charge_conf& sigma, const clustering& cs) noexcept
    {
        double energy = 0.0;

        for (const auto& other_c : cs)
        {
            if (other_c != c)
            {
                energy += get_cluster_potential(c, other_c, sigma.at(other_c.c), potential_bound::LOWER);
            }
        }

        return energy;
    }

    // Function to calculate the external upper bound of local potential energy for a cluster C in a clustering X
    double V_ext_upper(const cluster& c, const cluster_charge_conf& sigma, const clustering& cs) noexcept
    {
        double energy = 0.0;

        for (const auto& other_c : cs)
        {
            if (other_c != c)
            {
                energy += get_cluster_potential(c, other_c, sigma.at(other_c.c), potential_bound::UPPER);
            }
        }

        return energy;
    }

    // Function to check population stability
    bool IsPopulationStable(const clustering& cs, const cluster_charge_conf& sigma) noexcept
    {
        for (const cluster& c : cs)
        {
            const charge_state_multiset& sigma_c =
                sigma.at(c.c);  // Assume the charge assignment is the same for all clusters in the given clustering

            if (sigma_c.count(sidb_charge_state::NEGATIVE) != 0)
            {  // NEGATIVE is in sigma_c
                charge_state_multiset m_with_one_neg_less = sigma_c;
                m_with_one_neg_less.erase(m_with_one_neg_less.lower_bound(sidb_charge_state::NEGATIVE));
                m_with_one_neg_less.insert(sidb_charge_state::NEUTRAL);

                if (-V_int_upper(c, m_with_one_neg_less) - V_ext_upper(c, sigma, cs) >= mu_bounds_with_error[0])
                {
                    return false;
                }
            }

            if (possible_charge_states.size() == 3 && sigma_c.count(sidb_charge_state::POSITIVE) != 0)
            {  // POSITIVE is in sigma_c
                charge_state_multiset m_with_one_pos_less = sigma_c;
                m_with_one_pos_less.erase(m_with_one_pos_less.lower_bound(sidb_charge_state::POSITIVE));
                m_with_one_pos_less.insert(sidb_charge_state::NEUTRAL);

                if (-V_int_lower(c, m_with_one_pos_less) - V_ext_lower(c, sigma, cs) <= mu_bounds_with_error[3])
                {
                    return false;
                }
            }

            if (sigma_c.count(sidb_charge_state::NEUTRAL) != 0)
            {  // NEUTRAL is in sigma_c
                if (-V_int_lower(c, sigma_c) - V_ext_lower(c, sigma, cs) <= mu_bounds_with_error[1] ||
                    -V_int_upper(c, sigma_c) - V_ext_upper(c, sigma, cs) >= mu_bounds_with_error[2])
                {
                    return false;
                }
            }
        }

        return true;
    }

    [[nodiscard]] cluster obtain_cluster_information(const std::set<uint64_t>& c) const noexcept
    {
        cluster clus{c};
        for (uint64_t i = 0; i < c.size(); ++i)
        {
            if (negative_sidbs.count(*std::next(c.cbegin(), static_cast<int64_t>(i))) != 0)
            {
                clus.preassigned[i] = sidb_charge_state::NEGATIVE;
                clus.preassigned_count++;
            }
            else if (positive_sidbs.count(*std::next(c.cbegin(), static_cast<int64_t>(i))) != 0)
            {
                clus.preassigned[i] = sidb_charge_state::POSITIVE;
                clus.preassigned_count++;
            }
        }
        return clus;
    }

    [[nodiscard]] clustering get_clustering(const std::set<ch_node_ptr>& v) const noexcept
    {
        clustering xv{};
        for (const ch_node_ptr& h : v)
        {
            xv.emplace(obtain_cluster_information(h->c));
        }
        return xv;
    }

    // Function to find population-stable charge configurations
    std::set<sidb_charge_conf> PopulationStableChargeConfigurations(const std::set<ch_node_ptr>& v,
                                                                    const cluster_charge_conf&   sigma) noexcept
    {
        const clustering& xv = get_clustering(v);

        // Check if the set of clusters is not population-stable
        if (!IsPopulationStable(xv, sigma))
        {
            return {};
        }

        // Check if all nodes in v have empty children
        if (std::all_of(v.cbegin(), v.cend(), [](const ch_node_ptr& node) { return node->v.empty(); }))
        {
            // Return the set of charge configurations where each SiDB has the assigned charge state
            sidb_charge_conf charge_conf{};
            charge_conf.reserve(xv.size());

            for (const auto& [c, m] : sigma)  // automatically sorted
            {
                if (negative_sidbs.count(*c.cbegin()) != 0)
                {
                    charge_conf.emplace_back(sidb_charge_state::NEGATIVE);
                }
                else if (positive_sidbs.count(*c.cbegin()) != 0)
                {
                    charge_conf.emplace_back(sidb_charge_state::POSITIVE);
                }
                else
                {
                    charge_conf.emplace_back(*m.cbegin());
                }
            }
            return {charge_conf};
        }

        // Find the node with the maximum cluster size
        const ch_node_ptr& max_cluster_node =
            *std::max_element(v.cbegin(), v.cend(), [](const ch_node_ptr& node1, const ch_node_ptr& node2)
                              { return node1->c.size() < node2->c.size(); });

        // Create a new set of clusters with the node replaced by its children
        std::set<ch_node_ptr> v_prime = v;
        v_prime.erase(max_cluster_node);

        std::set<ch_node_ptr> v_diff{v_prime};

        v_prime.insert(max_cluster_node->v.cbegin(), max_cluster_node->v.cend());

        const clustering& xv_bar = get_clustering(max_cluster_node->v);

        // Recursively call the function with the new set of clusters and charge configurations
        std::set<sidb_charge_conf> result;

        cluster_charge_conf sigma_prime{};

        for (const cluster& c : get_clustering(v_diff))
        {
            sigma_prime.insert({c.c, sigma.at(c.c)});
        }

        charge_multiset_conf charge_multiset_conf_iterator{xv_bar, sigma.at(max_cluster_node->c)};

        do {
            const auto& sub_results = PopulationStableChargeConfigurations(
                v_prime, charge_multiset_conf_iterator.make_charge_multiset_conf(xv_bar, sigma_prime));
            result.insert(sub_results.cbegin(), sub_results.cend());

        } while (charge_multiset_conf_iterator.next_conf());

        return result;
    }

    std::vector<std::vector<sidb_charge_state>> get_k_element_multisets(const ch_node_ptr& top_node) const noexcept
    {
        charge_state_multiset m{};
        for (const sidb_charge_state cs : {sidb_charge_state::NEGATIVE, sidb_charge_state::NEUTRAL})
        {
            for (uint64_t i = 0; i < cl.num_cells(); ++i)
            {
                m.emplace(cs);
            }
        }

        // compute k_element_multisets
        charge_multiset_conf k_element_multisets{{obtain_cluster_information(top_node->c)}, m};

        std::vector<std::vector<sidb_charge_state>> multiset_vecs{{k_element_multisets.confs[0].vec}};
        while (k_element_multisets.next_conf())
        {
            multiset_vecs.emplace_back(k_element_multisets.confs[0].vec);
        }

        return multiset_vecs;
    }

    bool check_if_all_are_preassigned(sidb_simulation_result<Lyt>& res) const noexcept
    {
        if (negative_sidbs.size() + positive_sidbs.size() < cl.num_cells())
        {
            return false;
        }

        for (const auto& [cs, preassigned_indices] : {std::make_pair(sidb_charge_state::NEGATIVE, negative_sidbs),
                                                      std::make_pair(sidb_charge_state::POSITIVE, positive_sidbs)})
        {
            charge_distribution_surface cl_copy{cl};

            for (const uint64_t i : preassigned_indices)
            {
                cl_copy.assign_charge_state_by_cell_index(i, cs, false);
            }

            cl_copy.update_local_potential();
            cl_copy.recompute_system_energy();
            cl_copy.declare_physically_valid();

            res.charge_distributions.push_back(cl_copy);
        }

        return true;
    }

    void iterate_over_all_multisets(sidb_simulation_result<Lyt>& res, const ch_node_ptr& top_node) noexcept
    {
        if (check_if_all_are_preassigned(res))
        {
            return;
        }

        const clustering& xv = get_clustering(top_node->v);  // put this in parent scope

        const std::vector<std::vector<sidb_charge_state>>& multiset_vecs = get_k_element_multisets(top_node);

        const uint64_t num_threads = std::min(std::max(threads, 1ul), multiset_vecs.size());

        // define the bit string ranges per thread
        std::vector<std::pair<uint64_t, uint64_t>> ranges;
        const uint64_t                             chunk_size = std::max(multiset_vecs.size() / num_threads, 1ul);
        uint64_t                                   start      = 0;
        uint64_t                                   end        = chunk_size - 1;

        for (uint64_t i = 0; i < num_threads; ++i)
        {
            ranges.emplace_back(start, end);
            start = end + 1;
            end   = i == num_threads - 2 ? multiset_vecs.size() - 1 : start + chunk_size - 1;
        }

        std::vector<std::thread> threads_vec{};
        threads_vec.reserve(num_threads);
        std::mutex res_mutex{};

        for (const std::pair<uint64_t, uint64_t>& range : ranges)
        {
            threads_vec.emplace_back(
                [&]
                {
                    // Iterate over all charge multiset configurations in the assigned range
                    for (uint64_t ix = range.first; ix <= range.second; ++ix)
                    {
                        charge_multiset_conf charge_multiset_conf_iterator{
                            xv, charge_state_multiset{multiset_vecs[ix].cbegin(), multiset_vecs[ix].cend()}};

                        do {
                            // Call PopulationStableChargeConfigurations for each charge configuration
                            const std::set<sidb_charge_conf>& pop_stable_confs = PopulationStableChargeConfigurations(
                                top_node->v, charge_multiset_conf_iterator.make_charge_multiset_conf(xv));

                            // Check stability and update ground state if needed
                            for (const sidb_charge_conf& rho : pop_stable_confs)
                            {
                                charge_distribution_surface cl_copy{cl};

                                for (uint64_t i = 0; i < cl.num_cells(); ++i)
                                {
                                    cl_copy.assign_charge_state_by_cell_index(i, rho[i], false);
                                }

                                cl_copy.update_local_potential();

                                if (cl_copy.is_configuration_stable())
                                {
                                    cl_copy.recompute_system_energy();
                                    cl_copy.declare_physically_valid();

                                    {
                                        const std::lock_guard lock{res_mutex};
                                        res.charge_distributions.push_back(cl_copy);
                                    }
                                }
                            }
                        } while (charge_multiset_conf_iterator.next_conf());
                    }
                });
        }

        for (std::thread& t : threads_vec)
        {
            t.join();
        }
    }

    // Function to find the ground state charge state assignment
    sidb_simulation_result<Lyt> FindGroundState(const double theta = 45) noexcept
    {
        sidb_simulation_result<Lyt> res{};

        res.algorithm_name      = "ClusterExact";
        res.physical_parameters = cl.get_phys_params();

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            const ch_node_ptr& top_node = to_ch_node(sidb_cluster_hierarchy(cl));
            //                potential_hierarchy<Lyt>{cl.get_sidb_order(), cl.get_phys_params(),
            //                theta}.make_cluster_hierarchy();

            // first simulate base 2
            iterate_over_all_multisets(res, top_node);

            if (cl.get_phys_params().base == 3)
            {
                for (const cell<Lyt>& c : cl.get_positive_candidates())
                {
                    positive_sidbs.emplace(static_cast<uint64_t>(cl.cell_to_index(c)));
                }
                // then simulate base 2 again, but now with the positive candidates preassigned a POSITIVE charge state
                iterate_over_all_multisets(res, top_node);
            }
        }

        res.simulation_runtime = time_counter;

        return res;
    }
};

}  // namespace fiction

#endif  // FICTION_EXACT_CLUSTER_ITERATION_HPP
