//
// Created by Willem Lambooy on 12/12/2023.
//

#ifndef FICTION_POTENTIAL_HIERARCHY_HPP
#define FICTION_POTENTIAL_HIERARCHY_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_nm_position.hpp"
#include "fiction/types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace fiction
{

struct ch_node;
using ch_node_ptr = std::shared_ptr<ch_node>;

struct ch_node
{
    std::set<uint64_t>    c;
    std::set<ch_node_ptr> v;

    explicit ch_node(const std::set<uint64_t>& c_, std::set<ch_node_ptr>& v_) : c{c_}, v{v_} {}

    explicit ch_node(const std::set<ch_node_ptr>& v_)
    {
        if (v_.size() == 1)
        {
            c = (*v_.cbegin())->c;
            v = (*v_.cbegin())->v;
            return;
        }

        for (const ch_node_ptr& h : v_)
        {
            c.insert(h->c.cbegin(), h->c.cend());
        }
        v = v_;
    };
};

template <typename Lyt>
class potential_hierarchy
{
  public:
    explicit potential_hierarchy(const std::vector<cell<Lyt>>& sidbs, const sidb_simulation_parameters& ps,
                                 const double cone_angle) :
            sidb_indices{make_sidb_index_set(sidbs.size())},
            theta{cone_angle},
            positions{get_positions(sidbs, ps)},
            params{ps}
    {}

    /* s contains all sidbs in the layout apart from i and j, i.e. the observer and the observed respectively. */
    std::set<uint64_t> make_observation(const std::set<uint64_t>& s, const double d_0, const uint64_t i,
                                        const uint64_t j) const noexcept
    {
        const auto& [x_i, y_i] = positions[i];
        const auto& [x_j, y_j] = positions[j];
        const double dx        = x_j - x_i;
        const double dy        = y_j - y_i;
        const double d         = std::hypot(dx, dy);

        const double gamma = std::atan(dy / dx);
        const double b     = std::tan(physical_constants::PI / 360 * theta) * d;
        const double a1    = std::min(b, b * std::sqrt(d / d_0));
        const double a2    = std::max(1.0, std::sqrt(d / d_0)) * b * std::sqrt(2);

        std::set<uint64_t> observation{j};

        for (const uint64_t sidb : s)
        {
            const auto& [x, y] = positions[sidb];

            if (std::pow((std::min(0.0, x - x_j) * std::cos(gamma) + (y - y_j) * std::sin(gamma)) /
                             (std::max(0, sgn(dx)) * a1 - std::min(0, sgn(dx)) * a2),
                         2) +
                    std::pow((std::max(0.0, x - x_j) * std::cos(gamma) + (y - y_j) * std::sin(gamma)) /
                                 (std::max(0, sgn(dx)) * a2 - std::min(0, sgn(dx)) * a1),
                             2) +
                    std::pow(((x - x_j) * std::sin(gamma) - (y - y_j) * std::cos(gamma)) / b, 2) <=
                1.0)
            {
                observation.emplace(sidb);
            }
        }

        return observation;
    }

    /* s contains all sidbs in the layout without the observer */
    std::set<std::set<uint64_t>> observations_to_clusters(const std::set<uint64_t>&     s,
                                                          std::set<std::set<uint64_t>>& observations) const noexcept
    {
        bool no_more_supersets = false;
        while (!no_more_supersets)
        {
            no_more_supersets   = true;
            bool superset_found = false;

            for (uint64_t i = 0; i < observations.size() && !superset_found; ++i)
            {
                for (uint64_t j = 0; j < observations.size(); ++j)
                {
                    if (i == j)
                    {
                        continue;
                    }

                    const auto& o_i = std::next(observations.cbegin(), static_cast<int64_t>(i));
                    const auto& o_j = std::next(observations.cbegin(), static_cast<int64_t>(j));

                    if (std::includes(o_i->cbegin(), o_i->cend(), o_j->cbegin(), o_j->cend()))
                    {
                        superset_found = true;
                        observations.erase(o_i);
                        break;
                    }
                }
            }

            if (superset_found)
            {
                no_more_supersets = false;
            }
        }

        std::set<std::set<uint64_t>> clusters{};  // X ← ∅
        std::set<uint64_t>           seen{};      // V ← ∅

        for (const uint64_t i : s)
        {
            if (seen.count(i) != 0)
            {
                continue;
            }

            // O′ ← { c ∈ O | i ∈ c }
            std::set<std::set<uint64_t>> observations_with_i{};
            std::set<std::set<uint64_t>> observations_without_i{};
            for (const std::set<uint64_t>& observation : observations)
            {
                if (observation.count(i) != 0)
                {
                    observations_with_i.emplace(observation);
                }
                else
                {
                    observations_without_i.emplace(observation);
                }
            }

            // C ← { j ∈ μ (O′) | ∀c ∈ O′ (j ∈ c) and ∀c ∈ O \ O′ (j /∈ c) }
            std::set<uint64_t> cluster{};
            for (const std::set<uint64_t>& o : observations_with_i)
            {
                for (const uint64_t j : o)
                {
                    if (std::all_of(observations_with_i.cbegin(), observations_with_i.cend(),
                                    [&j](const std::set<uint64_t>& c) { return c.count(j) != 0; }) &&
                        std::all_of(observations_without_i.cbegin(), observations_without_i.cend(),
                                    [&j](const std::set<uint64_t>& c) { return c.count(j) == 0; }))
                    {
                        cluster.emplace(j);
                        seen.emplace(j);  // V ← V ∪ C
                    }
                }
            }

            if (cluster.size() > 1)
            {
                clusters.emplace(cluster);  // X ← X ∪ {C}
            }
        }

        return clusters;
    }

    // Powerset-pair monoid
    class pp_monoid
    {
      public:
        std::pair<std::set<ch_node_ptr>, std::set<ch_node_ptr>> pp;

        explicit pp_monoid() : pp{} {}
        explicit pp_monoid(const ch_node_ptr& n, const bool conflict) :
                pp{conflict ? std::make_pair(std::set<ch_node_ptr>{n}, std::set<ch_node_ptr>{}) :
                              std::make_pair(std::set<ch_node_ptr>{}, std::set<ch_node_ptr>{n})}
        {}

        pp_monoid operator+=(const pp_monoid& m) noexcept
        {
            std::set<ch_node_ptr> u1{}, u2{};
            std::set_union(this->pp.first.cbegin(), this->pp.first.cend(), m.pp.first.cbegin(), m.pp.first.cend(),
                           std::inserter(u1, u1.begin()));
            std::set_union(this->pp.second.cbegin(), this->pp.second.cend(), m.pp.second.cbegin(), m.pp.second.cend(),
                           std::inserter(u2, u2.begin()));
            pp.first  = u1;
            pp.second = u2;
            return *this;
        }
    };

    pp_monoid resolve_conflict(const ch_node_ptr& n, const std::set<uint64_t>& conflict) const noexcept
    {
        if (conflict.empty())
        {
            return pp_monoid(n, false);
        }
        if (conflict == n->c)
        {
            return pp_monoid(n, true);
        }

        pp_monoid pp{};
        for (const ch_node_ptr& h : n->v)
        {
            std::set<uint64_t> conflict_cut{};
            std::set_intersection(h->c.cbegin(), h->c.cend(), conflict.cbegin(), conflict.cend(),
                                  std::inserter(conflict_cut, conflict_cut.begin()));
            pp += resolve_conflict(h, conflict_cut);
        }

        return pp;
    }

    void insert_cluster(ch_node& n, const std::set<uint64_t>& cluster) const noexcept
    {
        if (n.c == cluster || cluster.empty())
        {
            return;
        }

        for (const auto& h : n.v)
        {
            if (std::includes(h->c.cbegin(), h->c.cend(), cluster.cbegin(), cluster.cend()))
            {
                insert_cluster(*h, cluster);
                return;
            }
        }

        std::set<ch_node_ptr>                     children_overlapping_cluster{};
        std::map<ch_node_ptr, std::set<uint64_t>> children_overlapping_cluster_intersections;
        for (const ch_node_ptr& h : n.v)
        {
            std::set<uint64_t> intersection{};
            std::set_intersection(h->c.cbegin(), h->c.cend(), cluster.cbegin(), cluster.cend(),
                                  std::inserter(intersection, intersection.begin()));

            if (!intersection.empty())
            {
                children_overlapping_cluster.emplace(h);
                children_overlapping_cluster_intersections[h] = intersection;
            }
        }

        std::set<ch_node_ptr> conflicting_children{};
        for (const ch_node_ptr& h : children_overlapping_cluster)
        {
            std::set<uint64_t> difference{};
            std::set_difference(h->c.cbegin(), h->c.cend(), cluster.cbegin(), cluster.cend(),
                                std::inserter(difference, difference.begin()));

            if (!difference.empty())
            {
                conflicting_children.emplace(h);
            }
        }

        if (conflicting_children.empty())
        {
            for (const auto& h : children_overlapping_cluster)
            {
                n.v.erase(h);
            }
            n.v.emplace(std::make_shared<ch_node>(cluster, children_overlapping_cluster));
            return;
        }

        for (const ch_node_ptr& h : conflicting_children)
        {
            n.v.erase(h);

            const std::set<uint64_t>& conflict = children_overlapping_cluster_intersections.at(h);

            const auto& [conflicting, non_conflicting] = resolve_conflict(h, conflict).pp;

            ch_node_ptr h_b = std::make_shared<ch_node>(conflicting);
            ch_node_ptr h_t = std::make_shared<ch_node>(non_conflicting);

            std::set<uint64_t> non_conflict{};
            std::set_difference(conflict.cbegin(), conflict.cend(), h_b->c.cbegin(), h_b->c.cend(),
                                std::inserter(non_conflict, non_conflict.begin()));

            insert_cluster(*h_t, non_conflict);

            n.v.emplace(h_t);
            n.v.emplace(h_b);
        }
    }

    double solve_differential_equation(double a = 1e-10, double b = 1e-7, const uint64_t max_iter = 100,
                                       const double tolerance = 1e-12) const noexcept
    {
        const auto func = [&epsilon_r = params.epsilon_r, lambda_tf = params.lambda_tf * 1e-9](double r)
        {
            return 1 - physical_constants::ELEMENTARY_CHARGE * std::exp(-r / lambda_tf) * (lambda_tf + r) /
                           (4 * physical_constants::PI * physical_constants::EPSILON * epsilon_r * r * r * lambda_tf);
        };

        double   root = (a + b) / 2.0;
        uint64_t iter = 0;

        while (iter < max_iter && std::abs(func(root)) > tolerance)
        {
            if (func(a) * func(root) < 0)
            {
                b = root;
            }
            else
            {
                a = root;
            }
            root = (a + b) / 2.0;
            iter++;
        }

        return root;
    }

    ch_node_ptr make_cluster_hierarchy() const noexcept
    {
        const double                 d_0 = solve_differential_equation();
        std::set<std::set<uint64_t>> agreed_clusters{};

        for (const uint64_t i : sidb_indices)
        {
            std::set<std::set<uint64_t>> observations{};
            std::set<uint64_t>           indices_without_i{sidb_indices};
            indices_without_i.erase(i);
            for (const uint64_t j : indices_without_i)
            {
                std::set<uint64_t> indices_without_i_and_j{indices_without_i};
                indices_without_i_and_j.erase(j);
                observations.emplace(make_observation(indices_without_i_and_j, d_0, i, j));
            }

            const std::set<std::set<uint64_t>>& clusters = observations_to_clusters(indices_without_i, observations);
            agreed_clusters.insert(clusters.cbegin(), clusters.cend());
        }

        std::set<ch_node_ptr> leaves{};
        for (const uint64_t i : sidb_indices)
        {
            std::set<ch_node_ptr> leaf{};
            leaves.emplace(std::make_shared<ch_node>(std::set<uint64_t>{i}, leaf));
        }

        ch_node_ptr root = std::make_shared<ch_node>(sidb_indices, leaves);

        for (const std::set<uint64_t>& c : agreed_clusters)
        {
            insert_cluster(*root, c);
        }

        for (int64_t i = static_cast<int64_t>(agreed_clusters.size()) - 1; i >= 0; --i)
        {
            insert_cluster(*root, *std::next(agreed_clusters.cbegin(), i));
        }

        return root;
    }

  private:
    const std::set<uint64_t> sidb_indices;

    const double theta;

    const std::vector<std::pair<double, double>> positions;

    const sidb_simulation_parameters params;

    static std::set<uint64_t> make_sidb_index_set(const uint64_t max)
    {
        std::set<uint64_t> sidb_index_set{};
        for (uint64_t i = 0; i < max; ++i)
        {
            sidb_index_set.emplace(i);
        }
        return sidb_index_set;
    }

    static std::vector<std::pair<double, double>> get_positions(const std::vector<cell<Lyt>>&     sidbs,
                                                                const sidb_simulation_parameters& ps)
    {
        std::vector<std::pair<double, double>> positions{};
        for (const cell<Lyt>& sidb : sidbs)
        {
            const auto [x, y] = sidb_nm_position<Lyt>(ps, sidb);
            positions.push_back({x * 1e-9, y * 1e-9});
        }
        return positions;
    }

    template <typename T>
    static int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }
};

}  // namespace fiction

#endif  // FICTION_POTENTIAL_HIERARCHY_HPP
